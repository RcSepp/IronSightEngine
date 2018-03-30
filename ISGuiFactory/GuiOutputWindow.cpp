#include "GuiFactory.h"

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize GuiFactory
//-----------------------------------------------------------------------------
Result GuiOutputWindow::Init(HWND targetwnd)
{
	Result rlt;

	this->targetwnd = targetwnd;
	this->d3ddevice = NULL;

	// Create main render area
	RECT rc;
	GetClientRect(targetwnd, &rc);
	renderareas.push_front(mainrenderarea = new RenderArea(this)); receivers.push_front(mainrenderarea);
	mainrenderarea->bounds.x = 0.0f;
	mainrenderarea->bounds.y = 0.0f;
	mainrenderarea->bounds.width = (float)(rc.right - rc.left);
	mainrenderarea->bounds.height = (float)(rc.bottom - rc.top);

	UpdateScrollbars();

	return R_OK;
}
Result GuiOutputWindow::Init(HWND targetwnd, ID3D11Device *d3ddevice)
{
	Result rlt;

	this->targetwnd = targetwnd;
	this->d3ddevice = d3ddevice;

	// Create main render area
	RECT rc;
	GetClientRect(targetwnd, &rc);
	renderareas.push_front(mainrenderarea = new RenderArea(this)); receivers.push_front(mainrenderarea);
	mainrenderarea->bounds.x = 0.0f;
	mainrenderarea->bounds.y = 0.0f;
	mainrenderarea->bounds.width = (float)(rc.right - rc.left);
	mainrenderarea->bounds.height = (float)(rc.bottom - rc.top);

	UpdateScrollbars();

	return R_OK;
}

void GuiOutputWindow::SetScale(const Point<float>& val)
{
	scale = val;
	if(rendertarget)
	{
		rendertarget->transform = D2D1::Matrix3x2F::Scale(scale.x, scale.y) *
								  D2D1::Matrix3x2F::Translation(translation.x, translation.y) *
								  D2D1::Matrix3x2F::Translation(scrolltranslation.x, scrolltranslation.y);

		// Recreate composit images
		std::list<RenderArea*>::const_iterator renderarea_iter;
		std::list<GuiControl*>::const_iterator control_iter;
		LIST_FOREACH(renderareas, renderarea_iter)
			LIST_FOREACH((*renderarea_iter)->ctrls, control_iter)
			{
				if((*control_iter)->img)
					(*control_iter)->img->Recreate();
				if((*control_iter)->overimg)
					(*control_iter)->overimg->Recreate();
				if((*control_iter)->downimg)
					(*control_iter)->downimg->Recreate();
			}

		UpdateScrollbars();
	}
}
void GuiOutputWindow::SetTranslation(const Point<float>& val)
{
	translation = val;
	if(rendertarget)
	{
		rendertarget->transform = D2D1::Matrix3x2F::Scale(scale.x, scale.y) *
								  D2D1::Matrix3x2F::Translation(translation.x, translation.y) *
								  D2D1::Matrix3x2F::Translation(scrolltranslation.x, scrolltranslation.y);

		UpdateScrollbars();
	}
}
void GuiOutputWindow::SetContentPadding(const Point<float>& val)
{
	contentpadding = val;
	if(rendertarget)
		UpdateScrollbars();
}

//-----------------------------------------------------------------------------
// Name: CreateDevice()
// Desc: Recreate device dependent resources
//-----------------------------------------------------------------------------
Result GuiOutputWindow::CreateDevice()
{
if(rendertarget)
	return R_OK;

	Result rlt;

	rendertarget = new RenderTarget();

	/*RECT rc;
	GetClientRect(targetwnd, &rc);

	// Create Direct2D render target
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	CHKRESULT(d2dfactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(targetwnd, size), &rendertarget));*/


if(!d3ddevice)
{
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Needed for Direct2D interop
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create Direct3D device and context
	ID3D11DeviceContext *context;
	D3D_FEATURE_LEVEL returnedFeatureLevel;
	CHKRESULT(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, NULL, 0, D3D11_SDK_VERSION, &d3ddevice, &returnedFeatureLevel, &context));
}
ID3D11Device1 *Direct3DDevice;
CHKRESULT(d3ddevice->QueryInterface(__uuidof(ID3D11Device), (void **)&Direct3DDevice));

CHKRESULT(Direct3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&rendertarget->dxgidevice));

CHKRESULT(d2dfactory->CreateDevice(rendertarget->dxgidevice, &device));

CHKRESULT(device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &rendertarget->context));

rendertarget->transform = D2D1::Matrix3x2F::Scale(scale.x, scale.y) *
						  D2D1::Matrix3x2F::Translation(translation.x, translation.y) *
						  D2D1::Matrix3x2F::Translation(scrolltranslation.x, scrolltranslation.y);



// Get the GPU we are using
IDXGIAdapter *dxgiAdapter;
rendertarget->dxgidevice->GetAdapter(&dxgiAdapter);
rendertarget->dxgidevice->Release();

// Get the DXGI factory instance
IDXGIFactory2 *dxgiFactory;
dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
dxgiAdapter->Release();

// Describe Windows 7-compatible Windowed swap chain
DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

swapChainDesc.Width = 0;
swapChainDesc.Height = 0;
swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
swapChainDesc.Stereo = false;
swapChainDesc.SampleDesc.Count = 1;
swapChainDesc.SampleDesc.Quality = 0;
swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
swapChainDesc.BufferCount = 2;
swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
swapChainDesc.Flags = 0;

// Create DXGI swap chain targeting a window handle (the only Windows 7-compatible option)
CHKRESULT(dxgiFactory->CreateSwapChainForHwnd(Direct3DDevice, targetwnd, &swapChainDesc, nullptr, nullptr, &swapchain));
dxgiFactory->Release();
Direct3DDevice->Release();

// Get the back buffer as an IDXGISurface (Direct2D doesn't accept an ID3D11Texture2D directly as a render target)
IDXGISurface *dxgibackbuffer;
CHKRESULT(swapchain->GetBuffer(0, IID_PPV_ARGS(&dxgibackbuffer)));

// Get screen DPI
FLOAT dpiX, dpiY;
d2dfactory->GetDesktopDpi(&dpiX, &dpiY);

// Create a Direct2D surface (bitmap) linked to the Direct3D texture back buffer via the DXGI back buffer
D2D1_BITMAP_PROPERTIES1 bitmapProperties =
	D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
	D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), dpiX, dpiY);
CHKRESULT(rendertarget->context->CreateBitmapFromDxgiSurface(dxgibackbuffer, &bitmapProperties, &backbuffer));
dxgibackbuffer->Release();

// Set surface as render target in Direct2D device context
rendertarget->context->SetTarget(backbuffer);

	// Create a solid color brush for the background
	rendertarget->context->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFFFF), (ID2D1SolidColorBrush**)&defaultbgbrush);

	for(std::list<IDestroyable*>::iterator iter = destroyableobjects.begin(); iter != destroyableobjects.end(); iter++)
		CHKRESULT((*iter)->OnCreateDevice(rendertarget));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: OnPaint()
// Desc: Render D2D content to rendertarget
//-----------------------------------------------------------------------------
Result GuiOutputWindow::OnPaint(HDC hdc, const Rect<> &rect)
{
//EDIT: Support const Rect<> &rect for regional refresh
	Result rlt;

	// Begin drawing
	CHKRESULT(CreateDevice());
	rendertarget->context->BeginDraw();
	rendertarget->context->SetTransform(D2D1::Matrix3x2F::Identity());

	// Retrieve the size of the render target
	D2D1_SIZE_U sizeRenderTarget = rendertarget->context->GetPixelSize();

	// Paint background
/*bool gdcsvisible = false;
for(std::list<GuiDirectContent*>::const_iterator iter = gdcs.begin(); iter != gdcs.end(); iter++)
	if((*iter)->visible)
	{
		gdcsvisible = true;
		break;
	}
if(!gdcsvisible)*/
	rendertarget->context->FillRectangle(D2D1::RectF(0.0f, 0.0f, (float)sizeRenderTarget.width, (float)sizeRenderTarget.height), bgbrush ? ((GuiBrush*)bgbrush)->d2dbrush : defaultbgbrush);

	/*// Render direct content
	for(std::list<GuiDirectContent*>::const_iterator iter = gdcs.begin(); iter != gdcs.end(); iter++)
		(*iter)->Render(rendertarget, hdc, rect);*/

	// Paint foreground
	for(std::list<RenderArea*>::const_iterator iter = renderareas.begin(); iter != renderareas.end(); iter++)
		(*iter)->Render(rendertarget);

	// End drawing
	HRESULT hr = rendertarget->context->EndDraw();
	if(hr == D2DERR_RECREATE_TARGET)
	{
		DestroyDevice();
		return R_OK;
	}
	else
	{
		DXGI_PRESENT_PARAMETERS parameters;
		ZeroMemory(&parameters, sizeof(DXGI_PRESENT_PARAMETERS));
		swapchain->Present1(0, 0, &parameters);
		return CheckResult(hr);
	}
}

Result GuiOutputWindow::OnResize(::Size<UINT>& newsize)
{
	if(!rendertarget)
		return R_OK;

	D2D1_SIZE_U oldsize = rendertarget->context->GetPixelSize();
	Result rlt;

	// Detach render target
	rendertarget->context->SetTarget(NULL);

	// Release all outstanding references to the swap chain's buffers
	backbuffer->Release();

	// Perform the actual device resize
	CHKRESULT(swapchain->ResizeBuffers(1, newsize.width, newsize.height, DXGI_FORMAT_B8G8R8A8_UNORM, 0));

	// Get the back buffer as IDXGISurface (Direct2D doesn't accept an ID3D11Texture2D directly as a render target)
	IDXGISurface *dxgibackbuffer;
	CHKRESULT(swapchain->GetBuffer(0, IID_PPV_ARGS(&dxgibackbuffer)));

	// Get screen DPI
	FLOAT dpiX, dpiY;
	d2dfactory->GetDesktopDpi(&dpiX, &dpiY);

	// Create a Direct2D surface (bitmap) linked to the Direct3D texture back buffer via the DXGI back buffer
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), dpiX, dpiY);
	CHKRESULT(rendertarget->context->CreateBitmapFromDxgiSurface(dxgibackbuffer, &bitmapProperties, &backbuffer));
	dxgibackbuffer->Release();

	// Reattach rendertarget
	rendertarget->context->SetTarget(backbuffer);

	mainrenderarea->bounds.width = (float)newsize.width;
	mainrenderarea->bounds.height = (float)newsize.height;

//if(renderareas.size() >= 2) //DELETE
//{ //DELETE
//	std::list<RenderArea*>::iterator iter = renderareas.begin(); //DELETE
//	iter++; //DELETE
//	(*iter)->SetWidth((float)newsize.width); //DELETE
//	(*iter)->SetHeight(max(0.0f, (float)newsize.height - 57.0f)); //DELETE
//} //DELETE

	if(resizecbk)
		resizecbk(::Size<UINT>(oldsize.width, oldsize.height), newsize, resizecbkuser);

	InvalidateRect(targetwnd, NULL, FALSE); // Invalidate whole device, because docking may have changed each control's individual boundaries

	UpdateScrollbars();

	return R_OK;
}

void GuiOutputWindow::OnHScrolling(short newpos)
{
	scrolltranslation.x = (float)(0x4000 - (int)newpos);
	if(rendertarget)
		rendertarget->transform = D2D1::Matrix3x2F::Scale(scale.x, scale.y) *
								  D2D1::Matrix3x2F::Translation(translation.x, translation.y) *
								  D2D1::Matrix3x2F::Translation(scrolltranslation.x, scrolltranslation.y);

	Invalidate();
}
void GuiOutputWindow::OnVScrolling(short newpos)
{
	scrolltranslation.y = (float)(0x4000 - (int)newpos);
	if(rendertarget)
		rendertarget->transform = D2D1::Matrix3x2F::Scale(scale.x, scale.y) *
								  D2D1::Matrix3x2F::Translation(translation.x, translation.y) *
								  D2D1::Matrix3x2F::Translation(scrolltranslation.x, scrolltranslation.y);

	Invalidate();
}
void GuiOutputWindow::OnHScrolled(short newpos)
{
	UpdateScrollbars();
}
void GuiOutputWindow::OnVScrolled(short newpos)
{
	UpdateScrollbars();
}

Result GuiOutputWindow::TakeScreenshot(const FilePath& filename)
{
	Result rlt;

	// Get backbuffer size
	D2D1_SIZE_U backbuffersize = backbuffer->GetPixelSize();

	// Create a new bitmap to render to
	ID2D1Bitmap1* targetbmp;
	CHKRESULT(rendertarget->context->CreateBitmap(backbuffersize, nullptr, backbuffersize.width * 4,
												  D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
																		  D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &targetbmp));

	// Render to targetbmp
	rendertarget->context->SetTarget(targetbmp);
	CHKRESULT(OnPaint(NULL, Rect<>(0, 0, backbuffersize.width, backbuffersize.height)));
	rendertarget->context->SetTarget(backbuffer);

	// Store targetbmp to file
	HRESULT hr;
	IWICBitmap* bmp;
	ID2D1Factory *pD2DFactory = NULL;
	IWICBitmap *pWICBitmap = NULL;
	ID2D1RenderTarget *pRT = NULL;
	IWICBitmapEncoder *pEncoder = NULL;
	IWICBitmapFrameEncode *pFrameEncode = NULL;
	IWICImageEncoder* imageEncoder = NULL;
	IWICStream *pStream = NULL;

	hr = wicfactory->CreateBitmap(backbuffersize.width, backbuffersize.height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &bmp);

	if(SUCCEEDED(hr))
		hr = wicfactory->CreateStream(&pStream);
	if(SUCCEEDED(hr))
		hr = pStream->InitializeFromFilename(filename.ToWCharString(), GENERIC_WRITE);
	if(SUCCEEDED(hr))
		hr = wicfactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &pEncoder); //EDIT: Support other formats by using an enum as parameter to this method
	if(SUCCEEDED(hr))
		hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
	if(SUCCEEDED(hr))
		hr = pEncoder->CreateNewFrame(&pFrameEncode, NULL);
	if(SUCCEEDED(hr))
		hr = pFrameEncode->Initialize(NULL);
	if(SUCCEEDED(hr))
		hr = wicfactory->CreateImageEncoder(device, &imageEncoder);
	if(SUCCEEDED(hr))
		hr = imageEncoder->WriteFrame(targetbmp, pFrameEncode, NULL);
	if(SUCCEEDED(hr))
		hr = pFrameEncode->Commit();
	if(SUCCEEDED(hr))
		hr = pEncoder->Commit();

	// Free memory
	RELEASE(bmp);
	RELEASE(pRT);
	RELEASE(pEncoder);
	RELEASE(pFrameEncode);
	RELEASE(pStream);
	RELEASE(targetbmp);

	return Error(hr);
}

Result GuiOutputWindow::TakeScreenshot(BYTE** imgbuffer, UINT* imgoffset, UINT* width, UINT* height)
{
	Result rlt;

	*imgbuffer = NULL;

	// Get backbuffer size
	D2D1_SIZE_U backbuffersize = backbuffer->GetPixelSize();

	// Allocate memory
	*imgoffset = 54; // 54... bitmap header size
	DWORD buffersize = backbuffersize.width * backbuffersize.height * 4 + *imgoffset;
	CHKALLOC(*imgbuffer = new BYTE[buffersize]);

	// Create a new bitmap to render to
	ID2D1Bitmap1* targetbmp;
	CHKRESULT(rendertarget->context->CreateBitmap(backbuffersize, nullptr, backbuffersize.width * 4,
												  D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
																		  D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &targetbmp));

	// Render to targetbmp
	rendertarget->context->SetTarget(targetbmp);
	CHKRESULT(OnPaint(NULL, Rect<>(0, 0, backbuffersize.width, backbuffersize.height)));
	rendertarget->context->SetTarget(backbuffer);

	// Store targetbmp to memory
	HRESULT hr;
	IWICBitmap* bmp;
	ID2D1Factory *pD2DFactory = NULL;
	IWICBitmap *pWICBitmap = NULL;
	ID2D1RenderTarget *pRT = NULL;
	IWICBitmapEncoder *pEncoder = NULL;
	IWICBitmapFrameEncode *pFrameEncode = NULL;
	IWICImageEncoder* imageEncoder = NULL;
	IWICStream *pStream = NULL;

	hr = wicfactory->CreateBitmap(backbuffersize.width, backbuffersize.height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &bmp);

	if(SUCCEEDED(hr))
		hr = wicfactory->CreateStream(&pStream);
	if(SUCCEEDED(hr))
		hr = pStream->InitializeFromMemory(*imgbuffer, buffersize);
	if(SUCCEEDED(hr))
		hr = wicfactory->CreateEncoder(GUID_ContainerFormatBmp, NULL, &pEncoder);
	if(SUCCEEDED(hr))
		hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
	if(SUCCEEDED(hr))
		hr = pEncoder->CreateNewFrame(&pFrameEncode, NULL);
	if(SUCCEEDED(hr))
		hr = pFrameEncode->Initialize(NULL);
	if(SUCCEEDED(hr))
		hr = wicfactory->CreateImageEncoder(device, &imageEncoder);
	if(SUCCEEDED(hr))
		hr = imageEncoder->WriteFrame(targetbmp, pFrameEncode, NULL);
	if(SUCCEEDED(hr))
		hr = pFrameEncode->Commit();
	if(SUCCEEDED(hr))
		hr = pEncoder->Commit();

	// Free memory
	RELEASE(bmp);
	RELEASE(pRT);
	RELEASE(pEncoder);
	RELEASE(pFrameEncode);
	RELEASE(pStream);
	RELEASE(targetbmp);

	if(width)
		*width = backbuffersize.width;
	if(height)
		*height = backbuffersize.height;

	return Error(hr);
}

void GuiOutputWindow::OnMouseDown(const MouseEventArgs& args)
{
	MouseEventResult* rlt = NULL;
	D2D1_SIZE_U sizeRenderTarget = D2D1::SizeU(0, 0);//rendertarget->GetPixelSize();
	MouseEventRouter::OnMouseDown(args, Point<float>((float)args.mousepos.x, (float)args.mousepos.y), mainrenderarea->bounds, &rlt);
	if(rlt)
	{
		rlt->cbk(rlt->ctrl, args, rlt->user);
		delete rlt;
	}
}
void GuiOutputWindow::OnMouseUp(const MouseEventArgs& args)
{
	MouseEventResult* rlt = NULL;
	D2D1_SIZE_U sizeRenderTarget = D2D1::SizeU(0, 0);//= rendertarget->GetPixelSize();
	MouseEventRouter::OnMouseUp(args, Point<float>((float)args.mousepos.x, (float)args.mousepos.y), Rect<float>(0.0f, 0.0f, (float)sizeRenderTarget.width, (float)sizeRenderTarget.height), &rlt);
	if(rlt)
	{
		rlt->cbk(rlt->ctrl, args, rlt->user);
		delete rlt;
	}
}
void GuiOutputWindow::OnMouseMove(const MouseEventArgs& args)
{
	MouseEventResult* rlt = NULL;
	D2D1_SIZE_U sizeRenderTarget = D2D1::SizeU(0, 0);//rendertarget->GetPixelSize();
	MouseEventRouter::OnMouseMove(args, Point<float>((float)args.mousepos.x, (float)args.mousepos.y), Rect<float>(0.0f, 0.0f, (float)sizeRenderTarget.width, (float)sizeRenderTarget.height), &rlt);
	if(rlt)
	{
		rlt->cbk(rlt->ctrl, args, rlt->user);
		delete rlt;
	}
}
void GuiOutputWindow::OnMouseDoubleClick(const MouseEventArgs& args)
{
	MouseEventResult* rlt = NULL;
	D2D1_SIZE_U sizeRenderTarget = D2D1::SizeU(0, 0);//rendertarget->GetPixelSize();
	MouseEventRouter::OnMouseDoubleClick(args, Point<float>((float)args.mousepos.x, (float)args.mousepos.y), Rect<float>(0.0f, 0.0f, (float)sizeRenderTarget.width, (float)sizeRenderTarget.height), &rlt);
	if(rlt)
	{
		rlt->cbk(rlt->ctrl, args, rlt->user);
		delete rlt;
	}
}

Result GuiOutputWindow::CreateRenderArea(Rect<float>& bounds, bool antialiased, IGuiRenderArea** area)
{
	ClippedRenderArea* newarea;
	CHKALLOC(newarea = new ClippedRenderArea(this, bounds, antialiased));

	RegisterObject(newarea); renderareas.push_back(newarea); receivers.push_front(newarea);
	*area = newarea;

	return R_OK;
}

Result GuiOutputWindow::CreateSolidBrush(const Color& clr, IGuiBrush** brush)
{
	Result rlt;

	SolidBrush* newbrush;
	CHKALLOC(newbrush = new SolidBrush(D2D1::ColorF(clr.r, clr.g, clr.b, clr.a)));
	CHKRESULT(newbrush->Init(rendertarget));

	RegisterObject(newbrush); destroyableobjects.push_back(newbrush); //brushes.push_back(newbrush);
	*brush = newbrush;

	return R_OK;
}

Result GuiOutputWindow::CreateLinearGradientBrush(GradientStop* stops, UINT numstops, const Point<float> startpos, const Point<float> endpos, IGuiBrush** brush)
{
	Result rlt;

	D2D1_GRADIENT_STOP* d2dstops;
	CHKALLOC(d2dstops = new D2D1_GRADIENT_STOP[numstops]);
	for(UINT i = 0; i < numstops; i++)
		d2dstops[i] = D2D1::GradientStop(stops[i].pos, D2D1::ColorF(stops[i].clr.r, stops[i].clr.g, stops[i].clr.b, stops[i].clr.a));

	LinearGradientBrush* newbrush;
	CHKALLOC(newbrush = new LinearGradientBrush(d2dstops, numstops,
												D2D1::LinearGradientBrushProperties(D2D1::Point2F(startpos.x, startpos.y), D2D1::Point2F(endpos.x, endpos.y))));
	delete[] d2dstops;
	CHKRESULT(newbrush->Init(rendertarget));

	RegisterObject(newbrush); destroyableobjects.push_back(newbrush);  //brushes.push_back(newbrush);
	*brush = newbrush;

	return R_OK;
}

Result GuiOutputWindow::CreateImageFromFile(const FilePath& filename, IGuiBitmap** img)
{
	Result rlt;

	*img = NULL;

	GuiBitmap* newimg;
	CHKALLOC(newimg = new GuiBitmap(this));
	CHKRESULT(newimg->Load(filename, rendertarget));

	RegisterObject(newimg); destroyableobjects.push_back(newimg);  //images.push_back(newimg);
	*img = newimg;

	return R_OK;
}

Result GuiOutputWindow::CreateImageFromMemory(LPVOID data, DWORD datasize, IGuiBitmap** img)
{
	Result rlt;

	*img = NULL;

	GuiBitmap* newimg;
	CHKALLOC(newimg = new GuiBitmap(this));
	CHKRESULT(newimg->Load(data, datasize, rendertarget));

	RegisterObject(newimg); destroyableobjects.push_back(newimg); //images.push_back(newimg);
	*img = newimg;

	return R_OK;
}

Result GuiOutputWindow::CreateImageFromISImage(const Image* image, IGuiBitmap** img)
{
	Result rlt;

	*img = NULL;

	GuiBitmap* newimg;
	CHKALLOC(newimg = new GuiBitmap(this));
	CHKRESULT(newimg->Load(image));

	RegisterObject(newimg); destroyableobjects.push_back(newimg); //images.push_back(newimg);
	*img = newimg;

	return R_OK;
}

Result GuiOutputWindow::CreateCompositeImage(IGuiCompositeImage** img)
{
	Result rlt;

	*img = NULL;

	GuiCompositeImage* newimg;
	CHKALLOC(newimg = new GuiCompositeImage(this));

	RegisterObject(newimg); destroyableobjects.push_back(newimg); //images.push_back(newimg);
	*img = newimg;

	return R_OK;
}

/*void GuiFactory::RemoveImage(IGuiBitmap* img)
{
	GuiBitmap* baseimg = reinterpret_cast<GuiBitmap*>(img);
	if(baseimg)
	{
		images.remove(baseimg);
		baseimg->Release();
	}
}
void GuiFactory::RemoveImage(IGuiCompositeImage* img)
{
	GuiCompositeImage* baseimg = reinterpret_cast<GuiCompositeImage*>(img);
	if(baseimg)
	{
		images.remove(baseimg);
		baseimg->Release();
	}
}*/

/*Result GuiOutputWindow::CreateDirectContent(const Rect<int>& destrect, bool usegdi, IGuiDirectContent** gdc)
{
	Result rlt;

	*gdc = NULL;

	GuiDirectContent* newgdc;
	if(usegdi)
	{
		CHKALLOC(newgdc = new GdiDirectContent(destrect));
	}
	else
	{
		CHKALLOC(newgdc = new D2dDirectContent(destrect));
	}
	RegisterObject(newgdc); destroyableobjects.push_back(newgdc); //gdcs.push_back(newgdc);
	*gdc = newgdc;

	return R_OK;
}*/

//-----------------------------------------------------------------------------
// Name: DestroyDevice()
// Desc: Discard device dependent resources
//-----------------------------------------------------------------------------
void GuiOutputWindow::DestroyDevice()
{
	REMOVE(rendertarget);
	RELEASE(swapchain);
	RELEASE(backbuffer);
	RELEASE(device)

	RELEASE(defaultbgbrush);

	for(std::list<IDestroyable*>::iterator iter = destroyableobjects.begin(); iter != destroyableobjects.end(); iter++)
		(*iter)->OnDestroyDevice();
}

//-----------------------------------------------------------------------------
// Name: OnUpdateContentSpanIncrease()
// Desc: Called by a child render area when its content span has increased
//-----------------------------------------------------------------------------
void GuiOutputWindow::OnContentSpanIncrease(const Size<float>& newspan)
{
	Size<float> oldtotalspan = contentspan;

	contentspan.width = max(contentspan.width, newspan.width);
	contentspan.height = max(contentspan.height, newspan.height);

	if(oldtotalspan.width != contentspan.width || oldtotalspan.height != contentspan.height)
		UpdateScrollbars();
}

//-----------------------------------------------------------------------------
// Name: OnUpdateContentSpanDecrease()
// Desc: Called by a child render area when its content span has decreased
//-----------------------------------------------------------------------------
void GuiOutputWindow::OnContentSpanDecrease(const Size<float>& oldspan)
{
	// If the old contentspan was part of the total contentspan (GuiOutputWindow::contentspan), recompute the total contentspan
	if(oldspan.width >= contentspan.width || oldspan.height >= contentspan.height)
	{
		Size<float> oldtotalspan = contentspan;

		contentspan = ::Size<float>(0.0f, 0.0f);
		std::list<RenderArea*>::const_iterator iter;
		LIST_FOREACH(renderareas, iter)
		{
			contentspan.width = max(contentspan.width, (*iter)->contentspan.width);
			contentspan.height = max(contentspan.height, (*iter)->contentspan.height);
		}

		if(oldtotalspan.width != contentspan.width || oldtotalspan.height != contentspan.height)
			UpdateScrollbars();
	}
}

void GuiOutputWindow::UpdateScrollbars()
{
	DWORD wndstyle = (DWORD)GetWindowLong(targetwnd, GWL_STYLE);
	if(!(wndstyle & WS_HSCROLL) && !(wndstyle & WS_VSCROLL))
		return;

	RECT rect;
	GetClientRect(targetwnd, &rect);

	D2D1_POINT_2F transformedcontentspan = D2D1::Point2F(contentspan.width + contentpadding.x, contentspan.height + contentpadding.y);
	D2D1_POINT_2F transformedorigin = D2D1::Point2F(0.0f, 0.0f);
	if(rendertarget)
	{
		D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Scale(scale.x, scale.y) * D2D1::Matrix3x2F::Translation(translation.x, translation.y);
		transformedcontentspan = transform.TransformPoint(transformedcontentspan);
		transformedorigin = transform.TransformPoint(transformedorigin);
	}

	SCROLLINFO scrifo;
	scrifo.cbSize = sizeof(SCROLLINFO);
	scrifo.fMask = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL | SIF_POS;

	GetScrollInfo(targetwnd, SB_HORZ, &scrifo);
	scrifo.nMin = min(0, (int)transformedorigin.x) + 0x4000; // 0x4000... Bugfix: Avoid negative values
	scrifo.nMax = max(rect.right - rect.left - 1, (int)ceil(transformedcontentspan.x)) + 0x4000; // 0x4000... Bugfix: Avoid negative values
	scrifo.nPage = rect.right - rect.left;
	if(scrifo.nPos >= scrifo.nMin && scrifo.nPos >= scrifo.nMax)
		scrolltranslation.x = (float)(0x4000 - scrifo.nPos);
	SetScrollInfo(targetwnd, SB_HORZ, &scrifo, TRUE);

	GetScrollInfo(targetwnd, SB_VERT, &scrifo);
	scrifo.nMin = min(0, (int)transformedorigin.y) + 0x4000; // 0x4000... Bugfix: Avoid negative values
	scrifo.nMax = max(rect.bottom - rect.top - 1, (int)ceil(transformedcontentspan.y)) + 0x4000; // 0x4000... Bugfix: Avoid negative values
	scrifo.nPage = rect.bottom - rect.top;
	if(scrifo.nPos >= scrifo.nMin && scrifo.nPos >= scrifo.nMax)
		scrolltranslation.y = (float)(0x4000 - scrifo.nPos);
	SetScrollInfo(targetwnd, SB_VERT, &scrifo, TRUE);

	if(rendertarget)
		rendertarget->transform = D2D1::Matrix3x2F::Scale(scale.x, scale.y) *
								  D2D1::Matrix3x2F::Translation(translation.x, translation.y) *
								  D2D1::Matrix3x2F::Translation(scrolltranslation.x, scrolltranslation.y);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void GuiOutputWindow::Release()
{
	while(renderareas.size())
		renderareas.front()->Release();
	renderareas.clear();
	ReleaseRegisteredObjects();

	DestroyDevice();

	delete this;
}