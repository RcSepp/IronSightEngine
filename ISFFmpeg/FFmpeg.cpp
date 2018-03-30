#include "FFmpeg.h"

//-----------------------------------------------------------------------------
// Name: CreateFFmpeg()
// Desc: DLL API for creating an instance of FFmpeg
//-----------------------------------------------------------------------------
FFMPEG_EXTERN_FUNC LPFFMPEG __cdecl CreateFFmpeg()
{
	return CheckFFmpegSupport(NULL) ? new FFmpeg() : NULL;
}

//-----------------------------------------------------------------------------
// Name: CheckFFmpegSupport()
// Desc: DLL API for checking support for FFmpeg on the target system
//-----------------------------------------------------------------------------
FFMPEG_EXTERN_FUNC bool __cdecl CheckFFmpegSupport(LPTSTR* missingdllname)
{
	if(!Engine::FindDLL("avcodec-52.dll", missingdllname))
		return false;
	if(!Engine::FindDLL("avformat-52.dll", missingdllname))
		return false;
	if(!Engine::FindDLL("avutil-50.dll", missingdllname))
		return false;
	if(!Engine::FindDLL("swscale-0.dll", missingdllname))
		return false;

   return true;
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void FFmpeg::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize FFmpeg
//-----------------------------------------------------------------------------
Result FFmpeg::Init()
{
	Result rlt;

	// Initialize libavformat and libavcodec
	av_register_all();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateNew()
// Desc: Create new file
//-----------------------------------------------------------------------------
Result FFmpeg::CreateNew(FilePath path, IMediaFile** file, String* ext)
{
	MediaFile* newfile;
	Result rlt;

	*file = NULL;

	CHKALLOC(newfile = new MediaFile(this));
	IFFAILED(newfile->New(path, ext))
	{
		RELEASE(newfile);
		return rlt;
	}

	files.push_back(newfile);
	*file = newfile;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: LoadFile()
// Desc: Load existing file
//-----------------------------------------------------------------------------
Result FFmpeg::LoadFile(FilePath path, IMediaFile** file)
{
	MediaFile* newfile;
	Result rlt;

	*file = NULL;

	CHKALLOC(newfile = new MediaFile(this));
	IFFAILED(newfile->Load(path))
	{
		RELEASE(newfile);
		return rlt;
	}

	files.push_back(newfile);
	*file = newfile;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CloseFile()
// Desc: Close file and remove reference
//-----------------------------------------------------------------------------
void FFmpeg::CloseFile(IMediaFile* file)
{
	MediaFile* basefile = reinterpret_cast<MediaFile*>(file);
	if(basefile)
	{
		files.remove(basefile);
		basefile->Release();
	}
}

Result FFmpeg::CreateFrame(int width, int height, PixelFormat pixfmt, uint8_t **buffer, AVFrame** frame) const
{
	int bufferlength;
	uint8_t* buf;

	if(buffer)
		*buffer = NULL;

	CHKALLOC(*frame = avcodec_alloc_frame());

	bufferlength = avpicture_get_size(pixfmt, width, height);
	buf = (uint8_t*)av_malloc(bufferlength);
	if(!buf)
	{
		AVREMOVE(frame)
		return ERR("Out of memory");
	}

	avpicture_fill((AVPicture*)*frame, buf, pixfmt, width, height);

	if(buffer)
		*buffer = buf;

	return R_OK;
}

Result FFmpeg::CreateRescaler(int srcwidth, int srcheight, PixelFormat srcpixfmt, int destwidth, int destheight, PixelFormat destpixfmt, int flags, SwsContext** context) const
{
	*context = sws_getContext(srcwidth, srcheight, srcpixfmt, destwidth, destheight, destpixfmt, flags, NULL, NULL, NULL);
	if(!*context)
		return ERR("Error creating software scale context");

	return R_OK;
}

#pragma region BlitToWnd
HDC hdc = NULL, hmemDC = NULL;
HBITMAP hmemBMP = NULL;
BITMAPINFO bmpinfo;
Rect<int> bltdestrect;

//-----------------------------------------------------------------------------
// Name: PrepareBlittingToWnd()
// Desc: Create a backbuffer for later calls to BlitToWnd
//-----------------------------------------------------------------------------
Result FFmpeg::PrepareBlittingToWnd(HWND wndtarget, const Rect<int>* destrect) //EDIT: Optimize and clean up
{
	bltdestrect = Rect<int>(*destrect);

	RECT rect;
	GetClientRect(wndtarget, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	ZeroMemory(&bmpinfo, sizeof(BITMAPINFO));
	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = destrect->width;
	bmpinfo.bmiHeader.biHeight = destrect->height;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 24;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biSizeImage = destrect->width * destrect->height * 3;

	// Get display DC
	hdc = GetDC(wndtarget);

	// Create backbuffer bitmap
	hmemBMP = CreateCompatibleBitmap(hdc, destrect->width, destrect->height);

	// Create off-screen DC compatible to displays DC
	hmemDC = CreateCompatibleDC(hdc);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: BlitToWnd()
// Desc: Helper function for copying 24 bit RGB frame data into the target window
//-----------------------------------------------------------------------------
void FFmpeg::BlitToWnd(uint8_t* framedata) //EDIT: Optimize and clean up
{
	// Select backbuffer into off-screen DC
	HGDIOBJ hobjOld = SelectObject(hmemDC, hmemBMP);

	// Draw onto backbuffer
	SetDIBits(hmemDC, hmemBMP, 0, bltdestrect.height, framedata, &bmpinfo, DIB_RGB_COLORS);

	// Blit backbuffer to screen
	StretchBlt(hdc, bltdestrect.x, bltdestrect.y + bltdestrect.height - 1, bltdestrect.width, -bltdestrect.height,
			   hmemDC, 0, 0, bltdestrect.width, bltdestrect.height, SRCCOPY);

	// Restore previous GDI object
	SelectObject(hmemDC, hobjOld);
}

//-----------------------------------------------------------------------------
// Name: EndBlittingToWnd()
// Desc: Free backbuffer and DCs
//-----------------------------------------------------------------------------
void FFmpeg::EndBlittingToWnd() //EDIT: Optimize and clean up
{
	// Clean up
	DeleteObject(hmemBMP);
	DeleteDC(hmemDC);
	DeleteDC(hdc);
}
#pragma endregion

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void FFmpeg::Release()
{
	for(std::list<MediaFile*>::iterator iter = files.begin(); iter != files.end(); iter++)
		(*iter)->Release();
	files.clear();

	delete this;
}