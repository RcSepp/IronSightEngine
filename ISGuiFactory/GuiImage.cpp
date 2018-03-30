#include "GuiFactory.h"
#include <ISImage.h>

//-----------------------------------------------------------------------------
// Name: Paint()
// Desc: Recreate composit image if needed and return d2dbmp
//-----------------------------------------------------------------------------
ID2D1Bitmap* GuiImage::Paint(RenderTarget *rendertarget)
{
	rendertarget = rendertarget;

	if(!d2dbmp)
		OnCreateDevice(rendertarget);

	return d2dbmp;
}

//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load image from file
//-----------------------------------------------------------------------------
Result GuiBitmap::Load(const FilePath& filename, RenderTarget *rendertarget)
{
	IWICBitmapDecoder *bmpdecoder = NULL;
	IWICBitmapFrameDecode *bmpframe = NULL;

	HRESULT hr = wicfactory->CreateDecoderFromFilename(filename.ToWCharString(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &bmpdecoder);

	if(SUCCEEDED(hr))
		// Create the initial frame
		hr = bmpdecoder->GetFrame(0, &bmpframe);

	if(SUCCEEDED(hr))
	{
		UINT width, height;
		hr = bmpframe->GetSize(&width, &height);
		size = ::Size<float>((float)width, (float)height);
	}

	if(SUCCEEDED(hr))
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED)
		hr = wicfactory->CreateFormatConverter(&fmtconverter);

	if(SUCCEEDED(hr))
		hr = fmtconverter->Initialize(bmpframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);


	if(SUCCEEDED(hr) && rendertarget)
		OnCreateDevice(rendertarget);

	RELEASE(bmpdecoder);
	RELEASE(bmpframe);

	return Error(hr);
}

//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load image from memory
//-----------------------------------------------------------------------------
Result GuiBitmap::Load(LPVOID data, DWORD datasize, RenderTarget *rendertarget)
{
	IWICStream *stream = NULL;
	IWICBitmapDecoder *bmpdecoder = NULL;
	IWICBitmapFrameDecode *bmpframe = NULL;

	HRESULT hr = wicfactory->CreateStream(&stream);

	if (SUCCEEDED(hr))
		// Initialize the stream with the memory pointer and size.
		hr = stream->InitializeFromMemory(reinterpret_cast<BYTE*>(data), datasize);

	if(SUCCEEDED(hr))
		// Create a decoder for the stream.
		hr = wicfactory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnLoad, &bmpdecoder);

	if(SUCCEEDED(hr))
		// Create the initial frame
		hr = bmpdecoder->GetFrame(0, &bmpframe);

	if(SUCCEEDED(hr))
	{
		UINT width, height;
		hr = bmpframe->GetSize(&width, &height);
		size = ::Size<float>((float)width, (float)height);
	}

	if(SUCCEEDED(hr))
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED)
		hr = wicfactory->CreateFormatConverter(&fmtconverter);

	if(SUCCEEDED(hr))
		hr = fmtconverter->Initialize(bmpframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);

	if(SUCCEEDED(hr) && rendertarget)
		OnCreateDevice(rendertarget);

	RELEASE(bmpdecoder);
	RELEASE(bmpframe);

	return Error(hr);
}

//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load image ISImage Image
//-----------------------------------------------------------------------------
Result GuiBitmap::Load(const Image* image)
{
	if(imagedata)
		delete[] imagedata;

	switch(image->bpp)
	{
	case 1:
		{
			// Convert G to BGRA
			imagedata = new BYTE[image->width * image->height * 4];
			for(int i = 0; i < image->width * image->height; i++)
			{
				imagedata[4 * i + 0] = imagedata[4 * i + 1] = imagedata[4 * i + 2] = image->data[i];
				imagedata[4 * i + 3] = 255;
			}
		}
		break;
	case 3:
		{
			// Convert RGB to BGRA
			imagedata = new BYTE[image->width * image->height * 4];
			for(int i = 0; i < image->width * image->height; i++)
			{
				imagedata[4 * i + 0] = image->data[3 * i + 2];
				imagedata[4 * i + 1] = image->data[3 * i + 1];
				imagedata[4 * i + 2] = image->data[3 * i + 0];
				imagedata[4 * i + 3] = 255;
			}
		}
		break;
	case 4:
		{
			// Convert RGBA to BGRA
			imagedata = new BYTE[image->width * image->height * 4];
			for(int i = 0; i < image->width * image->height; i++)
			{
				imagedata[4 * i + 0] = image->data[4 * i + 2];
				imagedata[4 * i + 1] = image->data[4 * i + 1];
				imagedata[4 * i + 2] = image->data[4 * i + 0];
				imagedata[4 * i + 3] = 255;
			}
		}
		break;
	default:
		return ERR(String("Unsupported bit depth: ") << 8 * image->bpp);
	}

	size = ::Size<float>((float)image->width, (float)image->height);
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: OnCreateDevice()
// Desc: Recreate device dependend ressources
//-----------------------------------------------------------------------------
Result GuiBitmap::OnCreateDevice(RenderTarget *rendertarget)
{
	if(fmtconverter)
		return Error(rendertarget->context->CreateBitmapFromWicBitmap(fmtconverter, &d2dbmp));
	if(imagedata)
	{
		/*D2D1_BITMAP_PROPERTIES1 props;
		props.pixelFormat.alphaMode = image->bpp == 4 ? D2D1_ALPHA_MODE_STRAIGHT : D2D1_ALPHA_MODE_IGNORE;
		switch(image->bpp)
		{
		case 1: props.pixelFormat.format = DXGI_FORMAT_R8_UNORM; break;
		case 2: props.pixelFormat.format = DXGI_FORMAT_R8G8_UNORM; break;
		case 3: props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM; break; //EDIT R8G8B8 not supported
		case 4: props.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
		}
		d2dfactory->GetDesktopDpi(&props.dpiX, &props.dpiY);
		props.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
		props.colorContext = NULL;*/

		// Get screen DPI
		FLOAT dpiX, dpiY;
		d2dfactory->GetDesktopDpi(&dpiX, &dpiY);

		D2D1_BITMAP_PROPERTIES1 bitmapProperties =
			D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);
		return Error(rendertarget->context->CreateBitmap(D2D1::SizeU((UINT32)size.width, (UINT32)size.height), imagedata, (UINT32)size.width * 4, bitmapProperties, &d2dbmp));
	}
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void GuiBitmap::Release()
{
	Unregister();
	parentwnd->destroyableobjects.remove(this);

	OnDestroyDevice();
	RELEASE(fmtconverter);
	if(imagedata)
		delete[] imagedata;

	delete this;
}


//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create image as rendertarget
//-----------------------------------------------------------------------------
Result GuiCompositeImage::Create(RenderTarget *rendertarget)
{
	Result rlt;

	this->rendertarget = rendertarget;

	RELEASE(d2dbmp);

	// The content of composit images is unaffected by external translations
	D2D1::Matrix3x2F matscale = rendertarget->transform;
	matscale._31 = matscale._32 = 0.0f; // Clear translation

	// Transform the image size to match the primary rendertarget's transformation
	D2D1_POINT_2F transformedpoint = matscale.TransformPoint(D2D1::Point2F(ceil(size.width), ceil(size.height)));
	UINT imgwidth = (UINT)ceil(transformedpoint.x);
	UINT imgheight = (UINT)ceil(transformedpoint.y);

	if(!imgwidth || !imgheight)
		return R_OK;

ID2D1Device* device;
rendertarget->context->GetDevice(&device);

RenderTarget targetcontext;
targetcontext.dxgidevice = rendertarget->dxgidevice;
targetcontext.transform = matscale;
CHKRESULT(device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &targetcontext.context));
device->Release();

// Get screen DPI
FLOAT dpiX, dpiY;
d2dfactory->GetDesktopDpi(&dpiX, &dpiY);

// Create a Direct2D surface (bitmap) linked to the Direct3D texture back buffer via the DXGI back buffer
D2D1_BITMAP_PROPERTIES1 bitmapProperties =
	D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
	D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);

DXGI_SURFACE_DESC surfacedesc = {0};
surfacedesc.Width = imgwidth;
surfacedesc.Height = imgheight;
surfacedesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
surfacedesc.SampleDesc.Count = 1;
surfacedesc.SampleDesc.Quality = 0;
IDXGISurface* targetsurface;
CHKRESULT(targetcontext.dxgidevice->CreateSurface(&surfacedesc, 1, DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT, NULL, &targetsurface));
CHKRESULT(targetcontext.context->CreateBitmapFromDxgiSurface(targetsurface, &bitmapProperties, &d2dbmp));
targetcontext.context->SetTarget(d2dbmp);

targetcontext.context->BeginDraw();
Rect<float> bounds(0, 0, size.width, size.height);
targetcontext.context->Clear(D2D1::ColorF(0x00000000, 0.0f));
std::list<Shape*>::const_iterator iter;
LIST_FOREACH(shapes, iter)
	(*iter)->Paint(&targetcontext, bounds, 1.0f);
HRESULT hr = targetcontext.context->EndDraw();
if(hr != D2DERR_RECREATE_TARGET)
	IFFAILED(CheckResult(hr))
		LOG(rlt.GetLastResult());

RELEASE(targetcontext.context);

	/*ID2D1BitmapRenderTarget* bmptarget = NULL;
	CHKRESULT(rendertarget->CreateCompatibleRenderTarget(D2D1::SizeF(size.width, size.height), &bmptarget));

	bmptarget->BeginDraw();
	Rect<float> bounds(0, 0, size.width, size.height);
	bmptarget->Clear(D2D1::ColorF(0x00000000, 0.0f));
	std::list<Shape*>::const_iterator iter;
	LIST_FOREACH(shapes, iter)
		(*iter)->Paint(bmptarget, bounds, 1.0f);
	bmptarget->EndDraw();

	IFFAILED(bmptarget->GetBitmap(&d2dbmp))
	{
		RELEASE(bmptarget);
		return rlt;
	}
	RELEASE(bmptarget);*/

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Recreate()
// Desc: Redraw composite image shapes
//-----------------------------------------------------------------------------
Result GuiCompositeImage::Recreate()
{
	if(!rendertarget)
		return R_OK;
	return Create(rendertarget);
}

//-----------------------------------------------------------------------------
// Name: OnCreateDevice()
// Desc: Called on device recreation. Recreate composit image
//-----------------------------------------------------------------------------
Result GuiCompositeImage::OnCreateDevice(RenderTarget *rendertarget)
{
	return Create(rendertarget);

	//rendertarget->CreateBitmap(D2D1::SizeU(size.x, size.y),

	/*Rect<float> localbounds(this->pos.GetX(), this->pos.GetY(), this->size.GetWidth(), this->size.GetHeight());

	Rect<float> finalbounds;
	GuiFactory::TransformBounds(dock, refdock, areabounds, localbounds, &finalbounds);

	for(std::list<Shape*>::const_iterator iter = shapes.begin(); iter != shapes.end(); iter++)
		(*iter)->Paint(rendertarget, finalbounds, alpha.GetVal());*/
}

//-----------------------------------------------------------------------------
// Name: AddXXX()
// Desc: Add XXX to this control's shapes
//-----------------------------------------------------------------------------
void GuiCompositeImage::AddFilledRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius, DockType dock, DockType refdock, IGuiShape** shape)
{
	FilledRectangle* newshape;
	newshape = new FilledRectangle(this, bounds, radius, brush, dock, refdock);

	shapes.push_back(newshape); //receivers.push_back(newshape);
	if(shape)
		*shape = newshape;

	RELEASE(d2dbmp); // Redraw necessary
}
void GuiCompositeImage::AddDrawnRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius, DockType dock, DockType refdock, IGuiShape** shape)
{
	DrawnRectangle* newshape;
	newshape = new DrawnRectangle(this, bounds, radius, brush, dock, refdock);

	shapes.push_back(newshape); //receivers.push_back(newshape);
	if(shape)
		*shape = newshape;

	RELEASE(d2dbmp); // Redraw necessary
}
void GuiCompositeImage::AddFilledEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock, IGuiShape** shape)
{
	FilledEllipse* newshape;
	newshape = new FilledEllipse(this, center, radius, brush, dock, refdock);

	shapes.push_back(newshape); //receivers.push_back(newshape);
	if(shape)
		*shape = newshape;

	RELEASE(d2dbmp); // Redraw necessary
}
void GuiCompositeImage::AddDrawnEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock, IGuiShape** shape)
{
	DrawnEllipse* newshape;
	newshape = new DrawnEllipse(this, center, radius, brush, dock, refdock);

	shapes.push_back(newshape); //receivers.push_back(newshape);
	if(shape)
		*shape = newshape;

	RELEASE(d2dbmp); // Redraw necessary
}
void GuiCompositeImage::AddDrawnPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed, float strokewidth, DockType dock, DockType refdock, IGuiShape** shape)
{
	DrawnPolygon* newshape;
	newshape = new DrawnPolygon(this, vertices, numvertices, isclosed, strokewidth, brush, dock, refdock);

	shapes.push_back(newshape); //receivers.push_back(newshape);
	if(shape)
		*shape = newshape;

	RELEASE(d2dbmp); // Redraw necessary
}
void GuiCompositeImage::AddFilledPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed, DockType dock, DockType refdock, IGuiShape** shape)
{
	FilledPolygon* newshape;
	newshape = new FilledPolygon(this, vertices, numvertices, isclosed, brush, dock, refdock);

	shapes.push_back(newshape); //receivers.push_back(newshape);
	if(shape)
		*shape = newshape;

	RELEASE(d2dbmp); // Redraw necessary
}
void GuiCompositeImage::AddFilledGeometry(IGuiBrush* brush, const Point<float>* vertices, size_t numtriangles, DockType dock, DockType refdock, IGuiShape** shape)
{
	FilledGeometry* newshape;
	newshape = new FilledGeometry(this, vertices, numtriangles, brush, dock, refdock);

	shapes.push_back(newshape); //receivers.push_back(newshape);
	if(shape)
		*shape = newshape;

	RELEASE(d2dbmp); // Redraw necessary
}
void GuiCompositeImage::AddLine(IGuiBrush* brush, const Point<float>& p0, const Point<float>& p1, float strokewidth, DockType dock, DockType refdock, IGuiShape** shape)
{
	Line* newshape;
	newshape = new Line(this, p0, p1, strokewidth, brush, dock, refdock);

	shapes.push_back(newshape); //receivers.push_back(newshape);
	if(shape)
		*shape = newshape;

	RELEASE(d2dbmp); // Redraw necessary
}
void GuiCompositeImage::AddBitmap(IGuiBitmap* bmp, const Rect<float>& bounds, DockType dock, DockType refdock, IGuiShape** shape)
{
	Bitmap* newshape;
	newshape = new Bitmap(this, bmp, bounds, dock, refdock);

	shapes.push_back(newshape); //receivers.push_back(newshape);
	if(shape)
		*shape = newshape;

	RELEASE(d2dbmp); // Redraw necessary
}
void GuiCompositeImage::AddTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Rect<float>& bounds,
									 GuiTextAllignment allignment, DockType dock, DockType refdock, IGuiShape** shape)
{
	TextField* newshape; Result rlt;
	newshape = new TextField(format, bounds, brush, dock, refdock);
	IFFAILED(newshape->SetText(text))
	{
		LOG(rlt.GetLastResult());
		return;
	}
	newshape->SetTextAllignment(allignment);

	shapes.push_back(newshape); //receivers.push_back(newshape);
	if(shape)
		*shape = newshape;

	RELEASE(d2dbmp); // Redraw necessary
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void GuiCompositeImage::Release()
{
	RELEASE(d2dbmp);
	Unregister();
	parentwnd->destroyableobjects.remove(this);

	std::list<Shape*>::iterator iter;
	LIST_FOREACH(shapes, iter)
		(*iter)->Release();
	shapes.clear();

	delete this;
}