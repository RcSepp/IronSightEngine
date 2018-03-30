#include "OpenGL.h"
#include "GLX.h"


//-----------------------------------------------------------------------------
// Name: CreateOpenGL()
// Desc: DLL API for creating an instance of OpenGL
//-----------------------------------------------------------------------------
OPENGL_EXTERN_FUNC LPOPENGL __cdecl CreateOpenGL()
{
	return new OpenGL();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void OpenGL::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: OpenGL()
// Desc: Constructor
//-----------------------------------------------------------------------------
OpenGL::OpenGL() : isfullscreen(false), oglversion_major(0), oglversion_minor(0)
{
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize OpenGL
//-----------------------------------------------------------------------------
Result OpenGL::Init(const OglStartupSettings* settings)
{
	isfullscreen = settings->isfullscreen;
	oglversion_major = settings->oglversion_major;
	oglversion_minor = settings->oglversion_minor;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Call Render() for every output window
//-----------------------------------------------------------------------------
void OpenGL::Render()
{
	Result rlt;

	std::list<IOutputWindow*>::iterator iter;
	LIST_FOREACH(wnds, iter)
	{
		OutputWindow* wnd = (OutputWindow*)*iter;
		wnd->Render(); // Render device
	}
}

//-----------------------------------------------------------------------------
// Name: CreateOutputWindow()
// Desc: Create a new output window by creating a new device (first window or fullscreen mode) or copying the existing one (windowed mode)
//-----------------------------------------------------------------------------
Result OpenGL::CreateOutputWindow(IOutputWindow::Settings* settings, IOutputWindow** wnd)
{
	Result rlt;

	*wnd = NULL;

	// Adjust size
#ifdef _WIN32
	RECT clientrect;
	if(!isfullscreen && GetClientRect(settings->wnd, &clientrect))
	{
		settings->backbuffersize.width = clientrect.right - clientrect.left;
		settings->backbuffersize.height = clientrect.bottom - clientrect.top;
	}
#else
	if(!settings->display)
		settings->display = CreateDisplay();
	if(!isfullscreen)
	{
		XWindowAttributes wndattrs;
		if(XGetWindowAttributes(settings->display, settings->wnd, &wndattrs))
		{
			settings->backbuffersize.width = wndattrs.width;
			settings->backbuffersize.height = wndattrs.height;
		}
	}
#endif

	OutputWindow* newwnd;
	CHKALLOC(newwnd = new OutputWindow(settings));
	IFFAILED(newwnd->CreateDevice(settings))
	{
		REMOVE(newwnd);
		return rlt;
	}

	wnds.push_back(newwnd);
	*wnd = newwnd;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateCamera()
// Desc: Create a camera object
//-----------------------------------------------------------------------------
Result OpenGL::CreateCamera(float fov, float clipnear, float clipfar, ICamera** cam)
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
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void OpenGL::Release()
{
	delete this;
}