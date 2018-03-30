#include "GuiFactory.h"
#include <DXGIDebug.h>

//-----------------------------------------------------------------------------
// EXTERN VARS
//-----------------------------------------------------------------------------
ID2D1Factory1 *d2dfactory = NULL;
IDWriteFactory1* dwtfactory = NULL;
IWICImagingFactory2* wicfactory = NULL;
IUIAnimationManager *animanager = NULL;
IUIAnimationTimer *anitimer = NULL;
IUIAnimationTransitionLibrary *anitranslib = NULL;

//-----------------------------------------------------------------------------
// Name: CreateCryptPP()
// Desc: DLL API for creating an instance of GuiFactory
//-----------------------------------------------------------------------------
__declspec(dllexport) LPGUIFACTORY __cdecl CreateGuiFactory()
{
	return CheckGuiFactorySupport(NULL) ? new GuiFactory() : NULL;
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void GuiFactory::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: CheckCryptPPSupport()
// Desc: DLL API for checking support for GuiFactory on the target system
//-----------------------------------------------------------------------------
#ifdef EXPORT_GUIFACTORY
	__declspec(dllexport) bool __cdecl CheckGuiFactorySupport(LPTSTR* missingdllname)
#else
	__declspec(dllimport) bool __cdecl CheckGuiFactorySupport(LPTSTR* missingdllname)
#endif
{
	//EDIT: Check if Direct2D, ... are available on the system

   return true;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize GuiFactory
//-----------------------------------------------------------------------------
Result GuiFactory::Init()
{
	Result rlt;

	// >>> Direct2D (device independent initializations):

	// Create Direct2D and DirectWrite factories
	D2D1_FACTORY_OPTIONS d2doptions;
/*#ifdef _DEBUG
	d2doptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#else*/
	d2doptions.debugLevel = D2D1_DEBUG_LEVEL_NONE;
/*#endif*/
	CHKRESULT(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &d2doptions, (LPVOID*)&d2dfactory));
	//CHKRESULT(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2doptions, &d2dfactory));
	CHKRESULT(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory1), reinterpret_cast<IUnknown**>(&dwtfactory)));

	// >>> UIAnimation:

	// Create Animation Manager
	CHKRESULT(CoCreateInstance(CLSID_UIAnimationManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&animanager)));

	// Create Animation Timer
	CHKRESULT(CoCreateInstance(CLSID_UIAnimationTimer, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&anitimer)));

	// Create Animation Transition Library
	CHKRESULT(CoCreateInstance(CLSID_UIAnimationTransitionLibrary, NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&anitranslib)));

/*// Create and set the ManagerEventHandler to start updating when animations are scheduled
IUIAnimationManagerEventHandler *pManagerEventHandler;
CHKRESULT(CManagerEventHandler::CreateInstance(this,&pManagerEventHandler));
CHKRESULT(m_pAnimationManager->SetManagerEventHandler(pManagerEventHandler));
pManagerEventHandler->Release();*/

	// >>> Windows Imaging Component:

	// Create WIC factory
	//CHKRESULT(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicfactory));
	CHKRESULT(CoCreateInstance(CLSID_WICImagingFactory2, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory2), (LPVOID*)&wicfactory));

	return R_OK;
}

void GuiFactory::TransformBounds(DockType dock, DockType refdock, const Rect<float>& parentbounds, const Rect<float>& localbounds, Rect<float>* finalbounds)
{
	/* No refdock
	switch(dock & 0xF)
	{
	case 0x0:
		finalbounds->x = parentbounds.x + localbounds.x;
		break;
	case 0x1:
		finalbounds->x = parentbounds.x + localbounds.x + (parentbounds.width - localbounds.width) / 2.0f;
		break;
	case 0x2:
		finalbounds->x = parentbounds.right() - localbounds.right();
	}
	switch((dock >> 4) & 0xF)
	{
	case 0x0:
		finalbounds->y = parentbounds.y + localbounds.y;
		break;
	case 0x1:
		finalbounds->y = parentbounds.y + localbounds.y + (parentbounds.height - localbounds.height) / 2.0f;
		break;
	case 0x2:
		finalbounds->y = parentbounds.bottom() - localbounds.bottom();
	}*/

	Point<float> refpos;
	switch(refdock & 0xF)
	{
	case 0x0:
		refpos.x = -0.0f;
		break;
	case 0x1:
		refpos.x = -localbounds.width / 2.0f;
		break;
	case 0x2:
		refpos.x = -localbounds.width;
	}
	switch((refdock >> 4) & 0xF)
	{
	case 0x0:
		refpos.y = -0.0f;
		break;
	case 0x1:
		refpos.y = -localbounds.height / 2.0f;
		break;
	case 0x2:
		refpos.y = -localbounds.height;
	}

	switch(dock & 0xF)
	{
	case 0x0:
		finalbounds->x = parentbounds.x + localbounds.x + refpos.x;
		break;
	case 0x1:
		finalbounds->x = parentbounds.x + localbounds.x + parentbounds.width / 2.0f + refpos.x;
		break;
	case 0x2:
		finalbounds->x = parentbounds.right() - localbounds.x + refpos.x;
	}
	switch((dock >> 4) & 0xF)
	{
	case 0x0:
		finalbounds->y = parentbounds.y + localbounds.y + refpos.y;
		break;
	case 0x1:
		finalbounds->y = parentbounds.y + localbounds.y + parentbounds.height / 2.0f + refpos.y;
		break;
	case 0x2:
		finalbounds->y = parentbounds.bottom() - localbounds.y + refpos.y;
	}

	if(finalbounds != &localbounds)
		finalbounds->size = localbounds.size;
}
void GuiFactory::TransformBoundsBack(DockType dock, DockType refdock, const Rect<float>& parentbounds, const Rect<float>& localbounds, Rect<float>* finalbounds)
{
	Point<float> refpos;
	switch(refdock & 0xF)
	{
	case 0x0:
		refpos.x = 0.0f;
		break;
	case 0x1:
		refpos.x = localbounds.width / 2.0f;
		break;
	case 0x2:
		refpos.x = localbounds.width;
	}
	switch((refdock >> 4) & 0xF)
	{
	case 0x0:
		refpos.y = 0.0f;
		break;
	case 0x1:
		refpos.y = localbounds.height / 2.0f;
		break;
	case 0x2:
		refpos.y = localbounds.height;
	}

	switch(dock & 0xF)
	{
	case 0x0:
		finalbounds->x = parentbounds.x - localbounds.x - refpos.x;
		break;
	case 0x1:
		finalbounds->x = parentbounds.x - localbounds.x - parentbounds.width / 2.0f - refpos.x;
		break;
	case 0x2:
		finalbounds->x = parentbounds.right() + localbounds.right() + refpos.x;
	}
	switch((dock >> 4) & 0xF)
	{
	case 0x0:
		finalbounds->y = parentbounds.y - localbounds.y - refpos.y;
		break;
	case 0x1:
		finalbounds->y = parentbounds.y - localbounds.y - parentbounds.height / 2.0f - refpos.y;
		break;
	case 0x2:
		finalbounds->y = parentbounds.bottom() + localbounds.bottom() + refpos.y;
	}

	if(finalbounds != &parentbounds)
		finalbounds->size = parentbounds.size;
}

//-----------------------------------------------------------------------------
// Name: OnPaint()
// Desc: Render D2D content to rendertarget
//-----------------------------------------------------------------------------
Result GuiFactory::OnPaint(HWND wnd, HDC hdc, const Rect<> &rect, bool& painthandled)
{
	Result rlt;

	std::map<HWND, GuiOutputWindow*>::iterator pair = wnds.find(wnd);
	if(pair == wnds.end())
	{
		painthandled = false;
		return R_OK;
	}
	painthandled = true;

	// Update animation manager with the current time
	UI_ANIMATION_SECONDS seconds;
	CHKRESULT(anitimer->GetTime(&seconds));
	CHKRESULT(animanager->Update(seconds));

	pair->second->OnPaint(hdc, rect);

	// Release finished animations
	for(std::list<GuiAnimation*>::const_iterator iter = animations.begin(); iter != animations.end(); iter++)
		if((*iter)->done)
		{
			(*iter)->Release(); // Removing from animations is done inside Release()
			break;
		}

	// Continue redrawing the client area as long as there are animations scheduled
	UI_ANIMATION_MANAGER_STATUS status;
	CHKRESULT(animanager->GetStatus(&status));
	if(status == UI_ANIMATION_MANAGER_BUSY)
		InvalidateRect(wnd, NULL, FALSE);

	return rlt;
}

void GuiFactory::OnResize(HWND wnd, ::Size<UINT>& newsize)
{
	std::map<HWND, GuiOutputWindow*>::iterator pair = wnds.find(wnd);
	if(pair != wnds.end())
		pair->second->OnResize(newsize);
}
void GuiFactory::OnHScrolling(HWND wnd, short newpos)
{
	std::map<HWND, GuiOutputWindow*>::iterator pair = wnds.find(wnd);
	if(pair != wnds.end())
		pair->second->OnHScrolling(newpos);
}
void GuiFactory::OnVScrolling(HWND wnd, short newpos)
{
	std::map<HWND, GuiOutputWindow*>::iterator pair = wnds.find(wnd);
	if(pair != wnds.end())
		pair->second->OnVScrolling(newpos);
}
void GuiFactory::OnHScrolled(HWND wnd, short newpos)
{
	std::map<HWND, GuiOutputWindow*>::iterator pair = wnds.find(wnd);
	if(pair != wnds.end())
		pair->second->OnHScrolled(newpos);
}
void GuiFactory::OnVScrolled(HWND wnd, short newpos)
{
	std::map<HWND, GuiOutputWindow*>::iterator pair = wnds.find(wnd);
	if(pair != wnds.end())
		pair->second->OnVScrolled(newpos);
}
void GuiFactory::OnMouseDown(HWND wnd, const MouseEventArgs& args)
{
	std::map<HWND, GuiOutputWindow*>::iterator pair = wnds.find(wnd);
	if(pair != wnds.end())
		pair->second->OnMouseDown(args);
}
void GuiFactory::OnMouseUp(HWND wnd, const MouseEventArgs& args)
{
	std::map<HWND, GuiOutputWindow*>::iterator pair = wnds.find(wnd);
	if(pair != wnds.end())
		pair->second->OnMouseUp(args);
}
void GuiFactory::OnMouseMove(HWND wnd, const MouseEventArgs& args)
{
	std::map<HWND, GuiOutputWindow*>::iterator pair = wnds.find(wnd);
	if(pair != wnds.end())
		pair->second->OnMouseMove(args);
}
void GuiFactory::OnMouseDoubleClick(HWND wnd, const MouseEventArgs& args)
{
	std::map<HWND, GuiOutputWindow*>::iterator pair = wnds.find(wnd);
	if(pair != wnds.end())
		pair->second->OnMouseDoubleClick(args);
}

Result GuiFactory::CreateOutputWindow(HWND targetwnd, IGuiOutputWindow** wnd)
{
	Result rlt;

	GuiOutputWindow* newwnd;
	CHKALLOC(newwnd = new GuiOutputWindow());
	CHKRESULT(newwnd->Init(targetwnd));

	wnds[targetwnd] = newwnd;
	*wnd = newwnd;

	return R_OK;
}
Result GuiFactory::CreateOutputWindow(HWND targetwnd, ID3D11Device *d3ddevice, IGuiOutputWindow** wnd)
{
	Result rlt;

	GuiOutputWindow* newwnd;
	CHKALLOC(newwnd = new GuiOutputWindow());
	CHKRESULT(newwnd->Init(targetwnd, d3ddevice));

	wnds[targetwnd] = newwnd;
	*wnd = newwnd;

	return R_OK;
}

Result GuiFactory::CreateTextFormat(const String& family, float size, bool italic, GuiFontWeight weight, GuiFontStretch stretch, IGuiTextFormat** fmt)
{
	Result rlt;

	*fmt = NULL;

	GuiTextFormat* newfmt;
	CHKALLOC(newfmt = new GuiTextFormat());
	CHKRESULT(newfmt->Create(family, size, italic, weight, stretch));

	RegisterObject(newfmt); //textformats.push_back(newfmt);
	*fmt = newfmt;

	return R_OK;
}

Result GuiFactory::CreateSlider(IGuiControl* bgctrl, IGuiControl* sliderctrl, Orientation orientation, float startpos, float endpos, ISliderControl** slider)
{
	Result rlt;

	*slider = NULL;

	SliderControl* newslider;
	CHKALLOC(newslider = new SliderControl(reinterpret_cast<GuiControl*>(bgctrl), reinterpret_cast<GuiControl*>(sliderctrl), orientation, startpos, endpos));
	CHKRESULT(newslider->Init());

	RegisterObject(newslider); //interactivectrls.push_back(newslider);
	*slider = newslider;

	return R_OK;
}

Result GuiFactory::CreateAnimation(IGuiAnimation** ani)
{
	Result rlt;

	*ani = NULL;

	GuiAnimation* newani;
	CHKALLOC(newani = new GuiAnimation());
	CHKRESULT(newani->Create());

	RegisterObject(newani); animations.push_back(newani);
	*ani = newani;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void GuiFactory::Release()
{
	/*for(std::map<HWND, GuiOutputWindow*>::iterator iter = wnds.begin(); iter != wnds.end(); iter++) //EDIT: Cause strange run-time errors
		iter->second->Release();*/
	ReleaseRegisteredObjects();

	// UIAnimation
	RELEASE(animanager);
	RELEASE(anitimer);
	RELEASE(anitranslib);

	// DirectWrite
	RELEASE(dwtfactory);

	// Direct2D
	RELEASE(d2dfactory);

	// WIC
	RELEASE(wicfactory);

#ifdef _DEBUG
	/*HMODULE dxgidebug_dll = GetModuleHandle("dxgidebug.dll"); //EDIT: Cause strange run-time errors
	if(dxgidebug_dll)
	{
		typedef HRESULT(__stdcall *DXGIGetDebugInterfaceProc)(const IID&, void**);
		DXGIGetDebugInterfaceProc DXGIGetDebugInterface = (DXGIGetDebugInterfaceProc)GetProcAddress(dxgidebug_dll, "DXGIGetDebugInterface");

		IDXGIDebug* dxgidebug;
		HRESULT hr = DXGIGetDebugInterface(__uuidof(IDXGIDebug), (LPVOID*)&dxgidebug);
		GUID _DXGI_DEBUG_ALL = {0xe48ae283, 0xda80, 0x490b, 0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8};
		hr = dxgidebug->ReportLiveObjects(_DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		dxgidebug->Release();
	}*/
#endif

	delete this;
}