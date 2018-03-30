#include "Direct3D.h"
#include "LineDrawer.h"


//#define BACKBUFFERFORMAT	DXGI_FORMAT_B8G8R8A8_UNORM
#define BACKBUFFERFORMAT	DXGI_FORMAT_R8G8B8A8_UNORM


//-----------------------------------------------------------------------------
// Name: OutputWindow()
// Desc: Constructor
//-----------------------------------------------------------------------------
OutputWindow::OutputWindow(Direct3D* parent, IOutputWindow::Settings* wndsettings) : IOutputWindow(*wndsettings), parent(parent)
{
	devspec = NULL;
	swapchain = NULL;
	texbackbuffer = NULL;
	texdepthstencil = NULL;
	dsview = NULL;
	cam = NULL;

	d3dcle = NULL;
	ZeroMemory(linedrawer, sizeof(linedrawer));
	defaultspritecontainer = NULL;
	backcolorenabled = true;
	takescreenshot = false;
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);

	backcolor[0] = 0.2f;
	backcolor[1] = 0.2f;
	backcolor[2] = 0.4f;
	/*srand(time.LowPart); // Generate random seed
	backcolor[0] = (float)(rand() % 256) / 255.0f; // R
	backcolor[1] = (float)(rand() % 256) / 255.0f; // G
	backcolor[2] = (float)(rand() % 256) / 255.0f; // B*/
	backcolor[3] = 1.0f;						   // A
}

//-----------------------------------------------------------------------------
// Name: CreateDevice()
// Desc: Create new Direct3D 10 device
//-----------------------------------------------------------------------------
Result OutputWindow::CreateDevice(IDXGIFactory* dxgifty, IOutputWindow::Settings* settings)
{
	Result rlt;

	this->backbuffersize = settings->backbuffersize;
	this->wnd = settings->wnd;

	CHKALLOC(devspec = new DeviceSpecific());

	/*// Get harware adapter and check DirectX 10 support
	IDXGIAdapter* dxgiadapter;
	HRESULT hr;
	for(UINT i = 0;; i++)
	{
		if(FAILED(hr = dxgifty->EnumAdapters(i, &dxgiadapter)))
			return ERR("No DirectX 10 supporting graphics card found.");

		if(SUCCEEDED(hr = dxgiadapter->CheckInterfaceSupport(__uuidof(ID3D11Device1), NULL)))
			break;
	}*/

	/*// Create device
	UINT createdeviceflags = NULL;
#ifdef _DEBUG
	createdeviceflags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//CHKRESULT(D3D10CreateDevice(dxgiadapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, createdeviceflags, D3D10_SDK_VERSION, &devspec->device));
CHKRESULT(D3D10CreateDevice1(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createdeviceflags, D3D11_FEATURE_LEVEL_10_0, D3D10_1_SDK_VERSION, &devspec->device));*/

	// Create device
	UINT createdeviceflags = NULL;
//UINT createdeviceflags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Needed for Direct2D interop
#ifdef _DEBUG
	createdeviceflags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL fls[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	D3D_FEATURE_LEVEL fl;
	IFFAILED(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createdeviceflags, fls, ARRAYSIZE(fls), D3D11_SDK_VERSION, &devspec->device, &fl, &devspec->devicecontext))
	{
		// Try creating a device with the debug layer disabled
		createdeviceflags &= ~D3D11_CREATE_DEVICE_DEBUG;
		CHKRESULT(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createdeviceflags, fls, ARRAYSIZE(fls), D3D11_SDK_VERSION, &devspec->device, &fl, &devspec->devicecontext));
	}

IDXGIDevice* dxgidevice;
devspec->device->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgidevice);
IDXGIAdapter* dxgiadapter;
dxgidevice->GetParent(__uuidof(IDXGIAdapter), (void **)& dxgiadapter);

dxgiadapter->GetParent(__uuidof(IDXGIFactory), (void **)& dxgifty);


// Query display adapter info
//IFSUCCEEDED(dxgifty->EnumAdapters(0, &dxgiadapter))
{
	DXGI_ADAPTER_DESC adapterdesc;
	dxgiadapter->GetDesc(&adapterdesc);
	dxgiadapter->Release();
	char* buffer = new char[1024];
	sprintf_s(buffer, 1024, "Display adapter:\n\tName:%s\n\tDedicated system memory: %iMB\n\tDedicated video memory: %iMB\n\tShared system memory: %iMB\n",
			  String(adapterdesc.Description).ToCharString(), adapterdesc.DedicatedSystemMemory / (1024*1024),
			  adapterdesc.DedicatedVideoMemory / (1024*1024), adapterdesc.SharedSystemMemory / (1024*1024));
	cle->Print(buffer);
	delete[] buffer;
}

	// Configure multisampling
	if(settings->enablemultisampling)
	{
		devspec->multisampling.Count = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT + 1;
		do
			CHKRESULT(devspec->device->CheckMultisampleQualityLevels(BACKBUFFERFORMAT, --devspec->multisampling.Count, &devspec->multisampling.Quality));
		while(!devspec->multisampling.Quality);
		devspec->multisampling.Quality--; //EDIT: Why?
	}
	else
	{
		devspec->multisampling.Count = 1;
		devspec->multisampling.Quality = 0;
	}

	// Create swap chain
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = backbuffersize.width;
	desc.BufferDesc.Height = backbuffersize.height;
	desc.BufferDesc.Format = BACKBUFFERFORMAT;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = wnd;
	desc.SampleDesc.Count = devspec->multisampling.Count;
	desc.SampleDesc.Quality = devspec->multisampling.Quality;
	desc.Windowed = !d3d->IsFullscreen();
	CHKRESULT(dxgifty->CreateSwapChain(devspec->device, &desc, &swapchain));

	//// Select display
	//IDXGIOutput* dxgioutput;
	//IFSUCCEEDED(dxgiadapter->EnumOutputs(settings->screenidx, &dxgioutput))
	//	swapchain->SetFullscreenState(TRUE, dxgioutput);

	/*// Create render target view
	ID3D11Texture2D *backbuffer;
	CHKRESULT(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));
	rlt = Error(devspec->device->CreateRenderTargetView(backbuffer, NULL, &devspec->view));
	RELEASE(backbuffer)
	CHKRESULT(rlt);*/

// Create backbuffer texture
texbackbuffer = new Texture();
texbackbuffer->CreateEmpty(this);
CHKRESULT(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&texbackbuffer->tex));
CHKRESULT(devspec->device->CreateRenderTargetView(texbackbuffer->tex, NULL, &texbackbuffer->rtview));
texbackbuffer->width = backbuffersize.width;
texbackbuffer->height = backbuffersize.height;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC depthdesc;
	ZeroMemory(&depthdesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthdesc.Width = backbuffersize.width;
	depthdesc.Height =  backbuffersize.height;
	depthdesc.MipLevels = 1;
	depthdesc.ArraySize = 1;
	depthdesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthdesc.SampleDesc.Count = devspec->multisampling.Count;
	depthdesc.SampleDesc.Quality = devspec->multisampling.Quality;
	depthdesc.Usage = D3D11_USAGE_DEFAULT;
	depthdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthdesc.CPUAccessFlags = NULL;
	depthdesc.MiscFlags = NULL;
	CHKRESULT(devspec->device->CreateTexture2D(&depthdesc, NULL, &texdepthstencil));

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvdesc;
	ZeroMemory(&dsvdesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvdesc.Format = depthdesc.Format;
	dsvdesc.ViewDimension = devspec->multisampling.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvdesc.Texture2D.MipSlice = 0;
	CHKRESULT(devspec->device->CreateDepthStencilView(texdepthstencil, &dsvdesc, &dsview));

	CHKRESULT(OnReset());

	IFFAILED(textrenderer.Init(this))
		Result::PrintLogMessage("Error initializing TextRenderer. Font generation disabled");

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: AdoptDevice()
// Desc: Create new swapchain in an existing Direct3D 10 device
//-----------------------------------------------------------------------------
Result OutputWindow::AdoptDevice(IDXGIFactory* dxgifty, IOutputWindow::Settings* settings, OutputWindow* sourcewnd)
{
	Result rlt;

	this->backbuffersize = settings->backbuffersize;
	this->wnd = settings->wnd;

	this->devspec = sourcewnd->devspec;
	devspec->refcount++;

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = backbuffersize.width;
	desc.BufferDesc.Height = backbuffersize.height;
	desc.BufferDesc.Format = BACKBUFFERFORMAT;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = wnd;
	desc.SampleDesc.Count = devspec->multisampling.Count;
	desc.SampleDesc.Quality = devspec->multisampling.Quality;
	desc.Windowed = !d3d->IsFullscreen();
	CHKRESULT(dxgifty->CreateSwapChain(devspec->device, &desc, &swapchain));

//this->device = sourcewnd->device;
//this->pparams = sourcewnd->pparams;
//this->wnd = wnd;

//CHKRESULT(device->CreateAdditionalSwapChain(&pparams, &swapchain));

// Create backbuffer texture
texbackbuffer = new Texture();
texbackbuffer->CreateEmpty(this);
CHKRESULT(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&texbackbuffer->tex));
CHKRESULT(devspec->device->CreateRenderTargetView(texbackbuffer->tex, NULL, &texbackbuffer->rtview));
texbackbuffer->width = backbuffersize.width;
texbackbuffer->height = backbuffersize.height;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC depthdesc;
	ZeroMemory(&depthdesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthdesc.Width = backbuffersize.width;
	depthdesc.Height =  backbuffersize.height;
	depthdesc.MipLevels = 1;
	depthdesc.ArraySize = 1;
	depthdesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthdesc.SampleDesc.Count = devspec->multisampling.Count;
	depthdesc.SampleDesc.Quality = devspec->multisampling.Quality;
	depthdesc.Usage = D3D11_USAGE_DEFAULT;
	depthdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthdesc.CPUAccessFlags = NULL;
	depthdesc.MiscFlags = NULL;
	CHKRESULT(devspec->device->CreateTexture2D(&depthdesc, NULL, &texdepthstencil));

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvdesc;
	ZeroMemory(&dsvdesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvdesc.Format = depthdesc.Format;
	dsvdesc.ViewDimension = devspec->multisampling.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvdesc.Texture2D.MipSlice = 0;
	CHKRESULT(devspec->device->CreateDepthStencilView(texdepthstencil, &dsvdesc, &dsview));

	CHKRESULT(OnReset());

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetBackbuffer()
// Desc: Bind the given texture as rendertarget or unbind the rendertarget (by passing NULL)
//-----------------------------------------------------------------------------
ITexture* OutputWindow::SetBackbuffer(ITexture* texbackbuffer)
{
	this->texbackbuffer = (Texture*)texbackbuffer;
	if(this->texbackbuffer)
		devspec->devicecontext->OMSetRenderTargets(1, &this->texbackbuffer->rtview, dsview);
	else
		devspec->devicecontext->OMSetRenderTargets(0, NULL, NULL);
	return texbackbuffer;
}

//-----------------------------------------------------------------------------
// Name: Resize()
// Desc: Resize backbuffer for windowed applications
//-----------------------------------------------------------------------------
Result OutputWindow::Resize(UINT width, UINT height)
{
	Result rlt;

	// Store new size
	this->backbuffersize.width = width;
	this->backbuffersize.height = height;

/*if(fullscreen) //EDIT: Handle fullscreen resizes
	return;*/

	// Detach render target
	devspec->devicecontext->OMSetRenderTargets(0, NULL, NULL);

	// Release all outstanding references to the swap chain's buffers
	//devspec->view->Release();
texbackbuffer->Release();
	dsview->Release();
	texdepthstencil->Release();

	// Perform the actual device resize
	CHKRESULT(swapchain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	/*// Create render target view
	ID3D11Texture2D *backbuffer;
	CHKRESULT(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));
	rlt = Error(devspec->device->CreateRenderTargetView(backbuffer, NULL, &devspec->view));
	RELEASE(backbuffer)
	CHKRESULT(rlt);*/

// Create backbuffer texture
texbackbuffer = new Texture();
texbackbuffer->CreateEmpty(this);
CHKRESULT(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&texbackbuffer->tex));
CHKRESULT(devspec->device->CreateRenderTargetView(texbackbuffer->tex, NULL, &texbackbuffer->rtview));
texbackbuffer->width = width;
texbackbuffer->height = height;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC depthdesc;
	depthdesc.Width = backbuffersize.width;
	depthdesc.Height =  backbuffersize.height;
	depthdesc.MipLevels = 1;
	depthdesc.ArraySize = 1;
	depthdesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthdesc.SampleDesc.Count = devspec->multisampling.Count;
	depthdesc.SampleDesc.Quality = devspec->multisampling.Quality;
	depthdesc.Usage = D3D11_USAGE_DEFAULT;
	depthdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthdesc.CPUAccessFlags = NULL;
	depthdesc.MiscFlags = NULL;
	CHKRESULT(devspec->device->CreateTexture2D(&depthdesc, NULL, &texdepthstencil));

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvdesc;
	dsvdesc.Format = depthdesc.Format;
	dsvdesc.ViewDimension = devspec->multisampling.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvdesc.Texture2D.MipSlice = 0;
	// Create the depth stencil view
	CHKRESULT(devspec->device->CreateDepthStencilView(texdepthstencil, &dsvdesc, &dsview));

	CHKRESULT(OnReset());

	// Notify misc. components of resize
	if(cam)
		cam->OnOutputWindowResize(backbuffersize);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: OnReset()
// Desc: On device reset
//-----------------------------------------------------------------------------
Result OutputWindow::OnReset()
{
	Result rlt;

	// Setup viewport
	viewport.Width = (FLOAT)backbuffersize.width;
	viewport.Height = (FLOAT)backbuffersize.height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	// Create rasterizer state
	D3D11_RASTERIZER_DESC rasterdesc;
	rasterdesc.FillMode = D3D11_FILL_SOLID;
	rasterdesc.CullMode = D3D11_CULL_BACK;
	rasterdesc.FrontCounterClockwise = FALSE;
	rasterdesc.DepthBias = 0;
	rasterdesc.SlopeScaledDepthBias = 0.0f;
	rasterdesc.DepthBiasClamp = 0;
	rasterdesc.DepthClipEnable = TRUE;
	rasterdesc.ScissorEnable = FALSE;
	rasterdesc.AntialiasedLineEnable = FALSE;
	rasterdesc.MultisampleEnable = devspec->multisampling.Count > 1;
	CHKRESULT(devspec->device->CreateRasterizerState(&rasterdesc, &devspec->rasterizer));
	devspec->devicecontext->RSSetState(devspec->rasterizer);

	// Create default depth-stencil state
	D3D11_DEPTH_STENCIL_DESC dsdesc = {0};
	// Depth test parameters
	dsdesc.DepthEnable = TRUE; // Depth read enable
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Depth write enable
	dsdesc.DepthFunc = D3D11_COMPARISON_LESS;
	// Stencil test parameters
	dsdesc.StencilEnable = FALSE; // Stencil disable
	dsdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	// Stencil operations for front-facing pixels
	dsdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil operations for back-facing pixels
	dsdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	CHKRESULT(devspec->device->CreateDepthStencilState(&dsdesc, &devspec->dssdefault));

	// Create depth-stencil state for foreground and background rendering
	// Depth test parameters
	dsdesc.DepthEnable = FALSE; // Depth read disable
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Depth write disable
	dsdesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	// Stencil test parameters
	dsdesc.StencilEnable = FALSE; // Stencil disable
	CHKRESULT(devspec->device->CreateDepthStencilState(&dsdesc, &devspec->dssnodepth));

ID3D11BlendState* bstate; //EDIT: Cleanup (enable alpha blending)
D3D11_BLEND_DESC bdesc;
bdesc.AlphaToCoverageEnable = FALSE;
bdesc.IndependentBlendEnable = FALSE;
bdesc.RenderTarget[0].BlendEnable = TRUE;
bdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
bdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
bdesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
bdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
bdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
bdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
bdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
devspec->device->CreateBlendState(&bdesc, &bstate);
devspec->devicecontext->OMSetBlendState(bstate, NULL, 0xFFFFFFFF);

	devspec->devicecontext->OMSetDepthStencilState(devspec->dssnodepth, 0);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Prepare content for rendering of the next frame
//-----------------------------------------------------------------------------
void OutputWindow::Update()
{
	std::list<IUpdateable*>::const_iterator iter;
	LIST_FOREACH(updatelist, iter)
		(*iter)->Update();

	if(cam)
		cam->UpdateMatrices();
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Render all visual content for this output window and fail if the device has been lost
//-----------------------------------------------------------------------------
void OutputWindow::Render()
{
	if(!cam)
	{
		Result::PrintLogMessage("No camera has been assigned to the current output window");
		return;
	}

	std::list<RenderDelegate*>::const_iterator iter;

	// Set render target
	devspec->devicecontext->OMSetRenderTargets(1, &texbackbuffer->rtview, dsview);

	// Set viewport
	devspec->devicecontext->RSSetViewports(1, &viewport);

	// Clear the backbuffer
	if(backcolorenabled)
		//devspec->devicecontext->ClearRenderTargetView(devspec->view, backcolor);
		devspec->devicecontext->ClearRenderTargetView(texbackbuffer->rtview, backcolor);
	devspec->devicecontext->ClearDepthStencilView(dsview, D3D11_CLEAR_DEPTH, 1.0f, 0);

	LIST_FOREACH(renderlist[RT_BACKGROUND], iter)
		(*iter)->operator()(this, RT_BACKGROUND);

	if(linedrawer[RT_BACKGROUND])
		linedrawer[RT_BACKGROUND]->Render(RT_BACKGROUND);

	devspec->devicecontext->OMSetDepthStencilState(devspec->dssdefault, 0);

	LIST_FOREACH(renderlist[RT_DEFAULT], iter)
		(*iter)->operator()(this, RT_DEFAULT);

	if(linedrawer[RT_DEFAULT])
		linedrawer[RT_DEFAULT]->Render(RT_DEFAULT);

	if(defaultspritecontainer)
		defaultspritecontainer->Render(RT_DEFAULT);

	LIST_FOREACH(renderlist[RT_FAST], iter)
		(*iter)->operator()(this, RT_FAST);

	if(linedrawer[RT_FAST])
		linedrawer[RT_FAST]->Render(RT_FAST);

	devspec->devicecontext->OMSetDepthStencilState(devspec->dssnodepth, 0);

	LIST_FOREACH(renderlist[RT_FOREGROUND], iter)
		(*iter)->operator()(this, RT_FOREGROUND);

	if(linedrawer[RT_FOREGROUND])
		linedrawer[RT_FOREGROUND]->Render(RT_FOREGROUND);

/*defaultspritecontainer->SetIdentityViewTransform();
defaultspritecontainer->SetIdentityProjectionTransform();
defaultspritecontainer->Begin();
textrenderer.DrawTextImmediate(defaultspritecontainer, "This is a test", "Verdana", 24.0f);
defaultspritecontainer->End();*/

	swapchain->Present(0, NULL);

	if(takescreenshot) //EDIT: Fails when multisampling is enabled
	{
		takescreenshot = false;
		ID3D11Texture2D *backbuffer = NULL;
		Result rlt;
		IFFAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer))
			LOG(rlt.GetLastResult());
		else
		{
			IFFAILED(D3DX11SaveTextureToFile(devspec->devicecontext, backbuffer, screenshotfileformat, screenshotfilename))
				LOG(rlt.GetLastResult());
			RELEASE(backbuffer);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: RegisterForRendering()
// Desc: Add function/class to the ones beeing called during rendering stage
//-----------------------------------------------------------------------------
void OutputWindow::RegisterForRendering(RENDERFUNC func, RenderType rendertype, LPVOID user)
{
	renderlist[rendertype].push_back(new FuncRenderDelegate(func, user));
}
void OutputWindow::RegisterForRendering(PyObject* pyfunc, RenderType rendertype)
{
	renderlist[rendertype].push_back(new PyFuncRenderDelegate(pyfunc));
}
void OutputWindow::RegisterForRendering(IRenderable* cls, RenderType rendertype)
{
	renderlist[rendertype].push_back(new ClassRenderDelegate(cls));
}

//-----------------------------------------------------------------------------
// Name: DeregisterFromRendering()
// Desc: Remove function/class from the ones beeing called during rendering stage
//-----------------------------------------------------------------------------
void OutputWindow::DeregisterFromRendering(RENDERFUNC func, RenderType rendertype)
{
	std::list<RenderDelegate*>::const_iterator iter;
	LIST_FOREACH(renderlist[rendertype], iter)
		if((*iter)->ptr.func == func)
		{
			delete *iter;
			renderlist[rendertype].erase(iter);
			return;
		}
}
void OutputWindow::DeregisterFromRendering(PyObject* pyfunc, RenderType rendertype)
{
	std::list<RenderDelegate*>::const_iterator iter;
	LIST_FOREACH(renderlist[rendertype], iter)
		if((*iter)->ptr.pyfunc == pyfunc)
		{
			delete *iter;
			renderlist[rendertype].erase(iter);
			return;
		}
}
void OutputWindow::DeregisterFromRendering(IRenderable* cls, RenderType rendertype)
{
	std::list<RenderDelegate*>::const_iterator iter;
	LIST_FOREACH(renderlist[rendertype], iter)
		if((*iter)->ptr.cls == cls)
		{
			delete *iter;
			renderlist[rendertype].erase(iter);
			return;
		}
}
void OutputWindow::DeregisterFromRendering(IRenderable* cls)
{
	for(UINT rendertype = 0; rendertype < 4; rendertype++)
		DeregisterFromRendering(cls, (RenderType)rendertype);
}

//-----------------------------------------------------------------------------
// Name: RegisterForUpdating()
// Desc: Add class to the ones beeing called before rendering stage
//-----------------------------------------------------------------------------
void OutputWindow::RegisterForUpdating(IUpdateable* cls)
{
	updatelist.push_back(cls);
}

//-----------------------------------------------------------------------------
// Name: DeregisterFromUpdating()
// Desc: Remove class from the ones beeing called before rendering stage
//-----------------------------------------------------------------------------
void OutputWindow::DeregisterFromUpdating(IUpdateable* cls)
{
	updatelist.remove(cls);
}

//-----------------------------------------------------------------------------
// Name: SortRegisteredClasses()
// Desc: Sort classes registered for rendering so that nearer objects get rendered earlyer (which may fix alpha blending artifacts)
//-----------------------------------------------------------------------------
/*Vector3 sortcampos;
bool CompareRenderables(const IRenderable* first, const IRenderable* second)
{
	Vector3 p0, p1;
	if(!first->GetRefPos(p0))
		return true;
	if(!second->GetRefPos(p1))
		return false;

	return Vec3LengthSq(&(p0 - sortcampos)) < Vec3LengthSq(&(p1 - sortcampos));
}*/ //EDIT (only do this for RenderDelegate's of type ClassRenderDelegate)
void OutputWindow::SortRegisteredClasses(RenderType rendertype, const Vector3& campos)
{
	/*sortcampos = campos;
	renderlist[rendertype].sort(CompareRenderables);*/ //EDIT
	Result::PrintLogMessage("SortRegisteredClasses() CURRENTLY NOT IMPLEMENTED");
}

//-----------------------------------------------------------------------------
// Name: GetRegisteredClasses()
// Desc: Fire callback for all classes registered for rendering in rendering order
//-----------------------------------------------------------------------------
Result OutputWindow::GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype)
{
	Result rlt;

	std::list<RenderDelegate*>::const_iterator iter;
	LIST_FOREACH(renderlist[rendertype], iter)
		if((*iter)->type == RenderDelegate::DT_CLASS)
			{CHKRESULT((*iter)->ptr.cls->GetRegisteredClasses(cbk, cbkuser, rendertype));}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawLine()
// Desc: Draw a line to screen for a single frame
//-----------------------------------------------------------------------------
void OutputWindow::DrawLine(const Vector3& v0, const Vector3& v1, Color c0, Color c1, RenderType rendertype)
{
	if(linedrawer[rendertype])
		linedrawer[rendertype]->DrawLine(v0, v1, c0, c1);
	else
		LOG("Line drawer not initialized");
}
void OutputWindow::DrawLine(const Vector2& v0, const Vector2& v1, Color c0, Color c1, RenderType rendertype)
{
	if(linedrawer[rendertype])
		linedrawer[rendertype]->DrawLine(v0, v1, c0, c1);
	else
		LOG("Line drawer not initialized");
}

//-----------------------------------------------------------------------------
// Name: EnableSprites()
// Desc: Create default sprite-container. User created sprite container will use its shader
//-----------------------------------------------------------------------------
Result OutputWindow::EnableSprites(const FilePath spriteshaderfilename)
{
	defaultspritecontainer = new SpriteContainer(0);
	Result rlt = defaultspritecontainer->Init(this, spriteshaderfilename);
	IFFAILED(rlt)
	{
		RELEASE(defaultspritecontainer);
		return rlt;
	}
	defaultspritecontainer->SetCameraViewTransform();
	defaultspritecontainer->SetCameraProjectionTransform();
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: EnableLineDrawing()
// Desc: Create LineDrawer utility
//-----------------------------------------------------------------------------
Result OutputWindow::EnableLineDrawing(const FilePath lineshaderfilename)
{
	Result rlt;

	for(int i = 0; i < ARRAYSIZE(linedrawer); i++)
	{
		CHKALLOC(linedrawer[i] = new LineDrawer());
		IFFAILED(linedrawer[i]->Init(this, lineshaderfilename))
		{
			delete linedrawer[i];
			linedrawer[i] = NULL;
			return rlt;
		}
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawSprite()
// Desc: Draw a line to screen for a single frame
//-----------------------------------------------------------------------------
void OutputWindow::DrawSprite(const Sprite* sprite)
{
	if(defaultspritecontainer)
		defaultspritecontainer->DrawSpriteDelayed(sprite);
	else
		LOG("Sprite drawing not enabled");
}

//-----------------------------------------------------------------------------
// Name: Show/HideFps()
// Desc: Enable/Disable frames per second counter rendering in the D3D console
//-----------------------------------------------------------------------------
void OutputWindow::ShowFps()
{
	if(d3dcle)
		d3dcle->ShowFps();
	else
		LOG("D3D Console not initialized; Call OutputWindow::MakeConsoleTarget()");
}
void OutputWindow::HideFps()
{
	if(d3dcle)
		d3dcle->HideFps();
}

//-----------------------------------------------------------------------------
// Name: TakeScreenShot()
// Desc: Issue screenshot saving
//-----------------------------------------------------------------------------
void OutputWindow::TakeScreenShot(const FilePath& filename, D3DX_IMAGE_FILEFORMAT fileformat)
{
	filename.Copy(&screenshotfilename);
	screenshotfileformat = fileformat;
	takescreenshot = true;
}

//-----------------------------------------------------------------------------
// Name: SetCamera()
// Desc: Assign a camera to this output window
//-----------------------------------------------------------------------------
void OutputWindow::SetCamera(ICamera* cam)
{
	this->cam = reinterpret_cast<Camera*>(cam);
	this->cam->OnOutputWindowResize(backbuffersize);
	this->cam->UpdateMatrices();
}

//-----------------------------------------------------------------------------
// Name: MakeConsoleTarget()
// Desc: Let subsequent calls to D3DPrint render onto this window
//-----------------------------------------------------------------------------
Result OutputWindow::MakeConsoleTarget(UINT numlines, UINT historysize)
{
	Result rlt;

	d3dcle = new D3DConsole(numlines, historysize);
	CHKRESULT(d3dcle->Init(this));
	d3dcle->target = cle->target;

	RELEASE(cle);
	this->RegisterForRendering(d3dcle, RT_FOREGROUND);
	cle = d3dcle;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateTexture()
// Desc: Create and load a texture
//-----------------------------------------------------------------------------
Result OutputWindow::CreateTexture(const FilePath& filename, ITexture::Usage usage, bool deviceindependent, ITexture** tex)
{
	Result rlt;

	*tex = NULL;

	Texture* newtex;
	CHKALLOC(newtex = new Texture());
	CHKRESULT(newtex->Load(this, filename, usage, deviceindependent));

	textures.push_back(newtex);
	*tex = newtex;

	return R_OK;
}
Result OutputWindow::CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, ITexture** tex)
{
	Result rlt;

	*tex = NULL;

	Texture* newtex;
	CHKALLOC(newtex = new Texture());
	CHKRESULT(newtex->CreateNew(this, width, height, usage, deviceindependent, format));

	textures.push_back(newtex);
	*tex = newtex;

	return R_OK;
}
Result OutputWindow::CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, LPVOID data, ITexture** tex)
{
	Result rlt;

	*tex = NULL;

	Texture* newtex;
	CHKALLOC(newtex = new Texture());
	CHKRESULT(newtex->CreateNew(this, width, height, usage, deviceindependent, format, 0xFFFFFFFF, data));

	textures.push_back(newtex);
	*tex = newtex;

	return R_OK;
}
Result OutputWindow::CreateTexture(ITexture* source, ITexture::Usage usage, bool deviceindependent, ITexture** tex)
{
	Result rlt;

	*tex = NULL;

	Texture* newtex;
	CHKALLOC(newtex = new Texture());
	CHKRESULT(newtex->CreateCopy((Texture*)source, usage, deviceindependent));

	textures.push_back(newtex);
	*tex = newtex;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateRenderShader()
// Desc: Create and load an SM 5.0 shader for rendering
//-----------------------------------------------------------------------------
Result OutputWindow::CreateRenderShader(const FilePath filename, IRenderShader** shader)
{
	Result rlt;

	*shader = NULL;

	RenderShader* newshader;
	CHKALLOC(newshader = new RenderShader());
	CHKRESULT(newshader->LoadFX(filename, devspec->device, devspec->devicecontext));

	rendershaders.push_back(newshader);
	*shader = newshader;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateObject()
// Desc: Create and load a 3D Object
//-----------------------------------------------------------------------------
Result OutputWindow::CreateObject(const FilePath& filename, bool loadtextures, IObject** obj)
{
	Result rlt;

	*obj = NULL;

	Object* newobj;
	CHKALLOC(newobj = new Object());
	CHKRESULT(newobj->LoadX(filename, loadtextures, this));

	objects.push_back(newobj);
	*obj = newobj;

	return R_OK;
}
Result OutputWindow::CreateObject(IObject* srcobj, bool newmaterials, IObject** obj)
{
	Result rlt;

	*obj = NULL;

	Object* newobj;
	CHKALLOC(newobj = new Object());
	CHKRESULT(newobj->Create(srcobj, newmaterials));

	objects.push_back(newobj);
	*obj = newobj;

	return R_OK;
}
Result OutputWindow::CreateObject(UINT numvertices, UINT numfaces, IObject** obj)
{
	Result rlt;

	*obj = NULL;

	Object* newobj;
	CHKALLOC(newobj = new Object());
	CHKRESULT(newobj->Create(numvertices, numfaces, this));

	objects.push_back(newobj);
	*obj = newobj;

	return R_OK;
}
Result OutputWindow::CreateObject(const D3dShapeDesc& shapedesc, IObject** obj)
{
	Result rlt;

	*obj = NULL;

	Object* newobj;
	CHKALLOC(newobj = new Object());
	CHKRESULT(newobj->Create(shapedesc, this));

	objects.push_back(newobj);
	*obj = newobj;

	return R_OK;
}
Result OutputWindow::CreateObject(const D3dShapeDesc* shapedescs[], UINT numshapedescs, IObject** obj)
{
	Result rlt;

	*obj = NULL;

	Object* newobj;
	CHKALLOC(newobj = new Object());
	CHKRESULT(newobj->Create(shapedescs, numshapedescs, this));

	objects.push_back(newobj);
	*obj = newobj;

	return R_OK;
}
void OutputWindow::RemoveObject(IObject* obj)
{
	bool objectexists = false;
	std::list<Object*>::const_iterator iter;
	LIST_FOREACH(objects, iter)
		if(*iter == obj)
		{
			objects.erase(iter);
			objectexists = true;
			break;
		}
	if(!objectexists)
		return; // Cancel operation if the object is inexistent (has already been removed)

	DeregisterFromRendering(obj);
	((Object*)obj)->Release();
}
void OutputWindow::RemoveAllObjects()
{
	std::list<Object*>::const_iterator iter;
	LIST_FOREACH(objects, iter)
	{
		DeregisterFromRendering(*iter);
		(*iter)->Release();
	}
	objects.clear();
}

//-----------------------------------------------------------------------------
// Name: CreateHUD()
// Desc: Create a container class for textures rendered directly to screen
//-----------------------------------------------------------------------------
Result OutputWindow::CreateHUD(IHUD** hud)
{
	Result rlt;

	*hud = NULL;

	HUD* newhud;
	CHKALLOC(newhud = new HUD());
	CHKRESULT(newhud->Init(this));

	huds.push_back(newhud);
	*hud = newhud;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateCursor()
// Desc: Create a texture moved by mouse move events
//-----------------------------------------------------------------------------
Result OutputWindow::CreateCursor(ID3dCursor** cursor)
{
	Result rlt;

	*cursor = NULL;

	D3dCursor* newcursor;
	CHKALLOC(newcursor = new D3dCursor());
	CHKRESULT(newcursor->Init(this));

	cursors.push_back(newcursor);
	*cursor = newcursor;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateSpriteContainer()
// Desc: Create a container class for sprites
//-----------------------------------------------------------------------------
Result OutputWindow::CreateSpriteContainer(UINT buffersize, ISpriteContainer** spc)
{
	Result rlt;

	*spc = NULL;

	if(!defaultspritecontainer)
		return ERR("Sprite drawing not enabled");

	SpriteContainer* newspc;
	CHKALLOC(newspc = new SpriteContainer(buffersize));
	CHKRESULT(newspc->Init(this, defaultspritecontainer));

	spritecontainers.push_back(newspc);
	*spc = newspc;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateD3dFont()
// Desc: Create a fontmap for a given font type
//-----------------------------------------------------------------------------
Result OutputWindow::CreateD3dFont(const FontType& type, ID3dFont** font)
{
	return textrenderer.CreateD3dFont(type, font);
}

//-----------------------------------------------------------------------------
// Name: CreateSceneManager()
// Desc: Create a scene manager
//-----------------------------------------------------------------------------
Result OutputWindow::CreateSceneManager(ISceneManager** mgr)
{
	Result rlt;

	*mgr = NULL;

	SceneManager* newmgr;
	CHKALLOC(newmgr = new SceneManager());
	CHKRESULT(newmgr->Init(this));

	scenemanagers.push_back(newmgr);
	*mgr = newmgr;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateSkyBox()
// Desc: Create a multi-layer skybox
//-----------------------------------------------------------------------------
Result OutputWindow::CreateSkyBox(ISkyBox** sky)
{
	Result rlt;

	*sky = NULL;

	SkyBox* newsky;
	CHKALLOC(newsky = new SkyBox());
	CHKRESULT(newsky->Init(this));

	skyboxes.push_back(newsky);
	*sky = newsky;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateBoxedLevel()
// Desc: ?
//-----------------------------------------------------------------------------
Result OutputWindow::CreateBoxedLevel(const UINT (&chunksize)[3], UINT numchunkbuffers, LPTEXTURE texboxes, LPRENDERSHADER blevelshader, IBoxedLevel** blevel)
{
	Result rlt;

	*blevel = NULL;

	BoxedLevel* newblevel;
	CHKALLOC(newblevel = new BoxedLevel(chunksize, texboxes, blevelshader));
	CHKRESULT(newblevel->Init(this, numchunkbuffers));

	blevels.push_back(newblevel);
	*blevel = newblevel;

	return R_OK;
}

/*//-----------------------------------------------------------------------------
// Name: CreateLandscape()
// Desc: ?
//-----------------------------------------------------------------------------
Result OutputWindow::CreateLandscape(FilePath& filename, LPRENDERSHADER shader, ILandscape** landscape)
{
	Result rlt;

	*landscape = NULL;

	Landscape* newlandscape;
	CHKALLOC(newlandscape = new Landscape());
	CHKRESULT(newlandscape->Load(filename, shader, this));

	landscapes.push_back(newlandscape);
	*landscape = newlandscape;

	return R_OK;
}*/ //EDIT11

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void OutputWindow::Release()
{
	parent->wnds.remove(this);

	if(d3dcle)
		d3dcle->Release();

	while(textures.size())
		textures.front()->Release();
	for(std::list<RenderShader*>::iterator iter = rendershaders.begin(); iter != rendershaders.end(); iter++)
		(*iter)->Release();
	rendershaders.clear();
/*	for(std::list<Object*>::iterator iter = objects.begin(); iter != objects.end(); iter++)
		(*iter)->Release();
	objects.clear();*/ //EDIT11
	while(huds.size())
		huds.front()->Release();
	while(cursors.size())
		cursors.front()->Release();
	while(scenemanagers.size())
		scenemanagers.front()->Release();
	for(std::list<SkyBox*>::iterator iter = skyboxes.begin(); iter != skyboxes.end(); iter++)
		(*iter)->Release();
	skyboxes.clear();
	while(spritecontainers.size())
		spritecontainers.front()->Release();
	spritecontainers.clear();
	for(std::list<BoxedLevel*>::iterator iter = blevels.begin(); iter != blevels.end(); iter++)
		(*iter)->Release();
	blevels.clear();
/*	for(std::list<Landscape*>::iterator iter = landscapes.begin(); iter != landscapes.end(); iter++)
		(*iter)->Release();
	landscapes.clear();*/ //EDIT11

	for(int i = 0; i < ARRAYSIZE(linedrawer); i++)
		REMOVE(linedrawer[i]);
	RELEASE(defaultspritecontainer);

	if(devspec && --devspec->refcount <= 0)
		REMOVE(devspec);

	RELEASE(swapchain);

	std::list<RenderDelegate*>::const_iterator iter;
	for(UINT rendertype = 0; rendertype < 4; rendertype++)
		LIST_FOREACH(renderlist[rendertype], iter)
			delete *iter;

	delete this;
}