#include "Direct3D.h"
#include <ISHavok.h>


//-----------------------------------------------------------------------------
// Name: CreateDirect3D()
// Desc: DLL API for creating an instance of Direct3D
//-----------------------------------------------------------------------------
DIRECT3D_EXTERN_FUNC LPDIRECT3D __cdecl CreateDirect3D()
{
	return new Direct3D();
}

//-----------------------------------------------------------------------------
// Name: CheckDirect3DSupport()
// Desc: DLL API for checking support for Direct3D on the target system
//-----------------------------------------------------------------------------
DIRECT3D_EXTERN_FUNC bool __cdecl CheckDirect3DSupport(LPTSTR* missingdllname)
{
	if(!Engine::FindDLL("d3d10.dll", missingdllname))
		return false;

   return true;
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void Direct3D::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
	if(pse)
		SyncWithPython();
}

//-----------------------------------------------------------------------------
// Name: Direct3D()
// Desc: Constructor
//-----------------------------------------------------------------------------
Direct3D::Direct3D()
{
	dxgifty = NULL;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize Direct3D
//-----------------------------------------------------------------------------
Result Direct3D::Init(const D3dStartupSettings* settings)
{
	Result rlt;

	this->isfullscreen = settings->isfullscreen;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Call Render(RenderType rendertype) for every output window
//-----------------------------------------------------------------------------
void Direct3D::Render()
{
	Result rlt;

	std::list<IOutputWindow*>::iterator iter;
	LIST_FOREACH(wnds, iter)
	{
		OutputWindow* wnd = (OutputWindow*)*iter;
		wnd->Update(); // Update device
		wnd->Render(); // Render device
	}
}

//-----------------------------------------------------------------------------
// Name: CastRay()
// Desc: Cast a ray against every visible object in the scene
//-----------------------------------------------------------------------------
bool Direct3D::CastRayTo(const Vector3& src, const Vector3& dest, float* dist)
{
	if(hvk)
	{
		if(dist)
		{
			*dist = hvk->CastRayTo(src, dest);
			return *dist < 1e20f;
		}
		else
			return hvk->CastRayTo(src, dest) < 1e20f;
	}
else
	return false;

	/*std::list<OutputWindow*>::iterator wnd_iter; //EDIT
	std::list<Object*>::iterator obj_iter;
	LIST_FOREACH(wnds, wnd_iter)
		LIST_FOREACH((*wnd_iter)->objects, obj_iter)
		{
			(*obj_iter)->RayTrace(...)
		}*/
}
bool Direct3D::CastRayDir(const Vector3& src, const Vector3& dir, float* dist)
{
	if(hvk)
	{
		if(dist)
		{
			*dist = hvk->CastRayDir(src, dir);
			return *dist < 1e20f;
		}
		else
			return hvk->CastRayDir(src, dir) < 1e20f;
	}
else
	return false;

	/*std::list<OutputWindow*>::iterator wnd_iter; //EDIT
	std::list<Object*>::iterator obj_iter;
	LIST_FOREACH(wnds, wnd_iter)
		LIST_FOREACH((*wnd_iter)->objects, obj_iter)
		{
			(*obj_iter)->RayTrace(...)
		}*/
}

//-----------------------------------------------------------------------------
// Name: TakeScreenShots()
// Desc: Take a screenshot on every output window
//-----------------------------------------------------------------------------
void Direct3D::TakeScreenShots(const FilePath& filename, D3DX_IMAGE_FILEFORMAT fileformat)
{
	FilePath subfilename;

	std::list<IOutputWindow*>::iterator iter;
	int i;
	LIST_FOREACH_I(wnds, iter, i)
	{
		filename.AddSuffix(String('_') << String(i), &subfilename);
		(*iter)->TakeScreenShot(subfilename, fileformat);
	}
}

//-----------------------------------------------------------------------------
// Name: GetEnabledLights()
// Desc: Get a list of enabled lights
//-----------------------------------------------------------------------------
void Direct3D::GetEnabledLights(std::list<ILight*>* lights)
{
	std::list<Light*>::iterator iter;
	LIST_FOREACH(this->lights, iter)
		if((*iter)->enabled)
			lights->push_back(*iter);
}

//-----------------------------------------------------------------------------
// Name: RemoveAllLights()
//-----------------------------------------------------------------------------
void Direct3D::RemoveAllLights()
{
	while(lights.size())
		lights.front()->Release();
}

//-----------------------------------------------------------------------------
// Name: CreateOutputWindow()
// Desc: Create a new output window by creating a new device (first window or fullscreen mode) or copying the existing one (windowed mode)
//-----------------------------------------------------------------------------
Result Direct3D::CreateOutputWindow(IOutputWindow::Settings* settings, IOutputWindow** wnd)
{
	Result rlt;

	*wnd = NULL;

	if(!dxgifty)
		CHKRESULT(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgifty));

	// Adjust size
	RECT clientrect;
	if(!isfullscreen && GetClientRect(settings->wnd, &clientrect))
	{
		settings->backbuffersize.width = clientrect.right - clientrect.left;
		settings->backbuffersize.height = clientrect.bottom - clientrect.top;
	}

	OutputWindow* newwnd;
	CHKALLOC(newwnd = new OutputWindow(this, settings));
	if(wnds.empty() || isfullscreen)
	{
		// First window or fullscreen mode
		IFFAILED(newwnd->CreateDevice(dxgifty, settings))
		{
			REMOVE(newwnd);
			return rlt;
		}
	}
	else
	{
		// Windowed mode
		IFFAILED(newwnd->AdoptDevice(dxgifty, settings, (OutputWindow*)wnds.front()))
		{
			REMOVE(newwnd);
			return rlt;
		}
	}

	wnds.push_back(newwnd);
	*wnd = newwnd;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateOutputWindow()
// Desc: Create a new device with no associated window (can only render internaly and store results to file)
//-----------------------------------------------------------------------------
Result Direct3D::CreateWindowlessDevice(IWindowlessDevice** wldev)
{
	Result rlt;

	*wldev = NULL;

	WindowlessDevice* newwldev;
	CHKALLOC(newwldev = new WindowlessDevice());
	CHKRESULT(newwldev->Create());

	wldevs.push_back(newwldev);
	*wldev = newwldev;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateCamera()
// Desc: Create a camera object
//-----------------------------------------------------------------------------
Result Direct3D::CreateCamera(float fov, float clipnear, float clipfar, ICamera** cam)
{
	Result rlt;

	*cam = NULL;

	Camera* newcam;
	CHKALLOC(newcam = new Camera(fov, clipnear, clipfar));

	cameras.push_back(newcam);
	*cam = newcam;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateLight()
// Desc: Create a light source
//-----------------------------------------------------------------------------
Result Direct3D::CreateLight(ILight::LightType type, ILight** light)
{
	Result rlt;

	*light = NULL;

	Light* newlight;
	CHKALLOC(newlight = new Light(type));

	lights.push_back(newlight);
	*light = newlight;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Direct3D::Release()
{
	for(std::list<Camera*>::iterator iter = cameras.begin(); iter != cameras.end(); iter++)
		(*iter)->Release();
	cameras.clear();
	while(lights.size())
		lights.front()->Release();
	while(wnds.size())
		wnds.front()->Release();
	for(std::list<WindowlessDevice*>::iterator iter = wldevs.begin(); iter != wldevs.end(); iter++)
		(*iter)->Release();
	wldevs.clear();

	RELEASE(dxgifty);

	delete this;
}