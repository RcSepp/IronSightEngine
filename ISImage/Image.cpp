#include <ISTypes.h>
#include "ISImage.h"
#include <FreeImage.h>

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
	printf("\n*** "); 
	if(fif != FIF_UNKNOWN) {
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	}
	printf(message);
	printf(" ***\n");
}

unsigned DLL_CALLCONV myReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle)
{
	return fread(buffer, size, count, (FILE *)handle);
}
unsigned DLL_CALLCONV myWriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle)
{
	return fwrite(buffer, size, count, (FILE *)handle);
}
int DLL_CALLCONV mySeekProc(fi_handle handle, long offset, int origin)
{
	return fseek((FILE *)handle, offset, origin);
}
long DLL_CALLCONV myTellProc(fi_handle handle)
{
	return ftell((FILE *)handle);
}

Result LoadImage(FilePath filename, Image** _img)
{
	Result rlt;

#ifdef FREEIMAGE_LIB
	FreeImage_Initialise();
#endif

	FreeImage_SetOutputMessage(FreeImageErrorHandler);

	FreeImageIO io;

	io.read_proc  = myReadProc;
	io.write_proc = myWriteProc;
	io.seek_proc  = mySeekProc;
	io.tell_proc  = myTellProc;

	FILE *file = fopen(filename, "rb");
	if(!file)
	{
#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
#endif
		return ERR(String("File not found: ") << filename);
	}

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromHandle(&io, (fi_handle)file, 0);

	if(fif == FIF_UNKNOWN)
	{
		fclose(file);
#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
#endif
		return ERR(String("Unknown image file format: ") << filename);
	}

	FIBITMAP *dib = FreeImage_LoadFromHandle(fif, &io, (fi_handle)file, 0);

	Image* img = *_img = Image::New((int)FreeImage_GetWidth(dib), (int)FreeImage_GetHeight(dib), (int)FreeImage_GetBPP(dib) / 8);

	unsigned char* src = (unsigned char*)FreeImage_GetBits(dib);
	int pitch = img->width * img->bpp;
	int srcpitch = FreeImage_GetPitch(dib);

	switch(img->bpp)
	{
	case 1:
		for(int y = 0; y < img->height; ++y)
			for(int x = 0; x < img->width; ++x)
			{
				int invy = img->height - y - 1;
				img->data[y * img->width + x] = src[invy * srcpitch + x];
			}
		break;

	case 3:
		for(int y = 0; y < img->height; ++y)
			for(int x = 0; x < img->width; ++x)
			{
				int invy = img->height - y - 1;
				img->data[y * pitch + x * 3 + 0] = src[invy * srcpitch + x * 3 + 2];
				img->data[y * pitch + x * 3 + 1] = src[invy * srcpitch + x * 3 + 1];
				img->data[y * pitch + x * 3 + 2] = src[invy * srcpitch + x * 3 + 0];
			}
		break;

	case 4:
		for(int y = 0; y < img->height; ++y)
			for(int x = 0; x < img->width; ++x)
			{
				int invy = img->height - y - 1;
				img->data[y * pitch + x * 4 + 0] = src[invy * srcpitch + x * 4 + 2];
				img->data[y * pitch + x * 4 + 1] = src[invy * srcpitch + x * 4 + 1];
				img->data[y * pitch + x * 4 + 2] = src[invy * srcpitch + x * 4 + 0];
				img->data[y * pitch + x * 4 + 3] = src[invy * srcpitch + x * 4 + 3];
			}
		break;

	default:
		memcpy(img->data, FreeImage_GetBits(dib), img->width * img->height * img->bpp);
	}

	FreeImage_Unload(dib);

#ifdef FREEIMAGE_LIB
	FreeImage_DeInitialise();
#endif

	return R_OK;
}

Result SaveImage(FilePath filename, Image* img)
{
	return ERR("Not implemented");
}

Result SaveImage(LPVOID* data, DWORD* datasize, FilePath ext, Image* img)
{
	return ERR("Not implemented");
}