#include "OpenGL.h"


#define WGL_DRAW_TO_WINDOW_ARB         0x2001
#define WGL_ACCELERATION_ARB           0x2003
#define WGL_SWAP_METHOD_ARB            0x2007
#define WGL_SUPPORT_OPENGL_ARB         0x2010
#define WGL_DOUBLE_BUFFER_ARB          0x2011
#define WGL_PIXEL_TYPE_ARB             0x2013
#define WGL_COLOR_BITS_ARB             0x2014
#define WGL_DEPTH_BITS_ARB             0x2022
#define WGL_STENCIL_BITS_ARB           0x2023
#define WGL_FULL_ACCELERATION_ARB      0x2027
#define WGL_SWAP_EXCHANGE_ARB          0x2028
#define WGL_TYPE_RGBA_ARB              0x202B
#define GL_ARRAY_BUFFER                0x8892
#define GL_STATIC_DRAW                 0x88E4
#define GL_FRAGMENT_SHADER             0x8B30
#define GL_VERTEX_SHADER               0x8B31
#define GL_COMPILE_STATUS              0x8B81
#define GL_LINK_STATUS                 0x8B82
#define GL_INFO_LOG_LENGTH             0x8B84
#define GL_TEXTURE0                    0x84C0
#define GL_BGRA                        0x80E1
#define GL_ELEMENT_ARRAY_BUFFER        0x8893


//-----------------------------------------------------------------------------
// Name: OutputWindow()
// Desc: Constructor
//-----------------------------------------------------------------------------
OutputWindow::OutputWindow(IOutputWindow::Settings* wndsettings) : IOutputWindow(*wndsettings), devicectx(0), renderctx(NULL), cam(NULL)
{
	backcolorenabled = true;
	ClrSet(&backcolor, 0.2f, 0.2f, 0.4f, 1.0f);
}

Result OutputWindow::CreateDevice(IOutputWindow::Settings* settings)
{
	this->backbuffersize = settings->backbuffersize;
	this->display = settings->display;
	this->wnd = settings->wnd;

	//int attributeListInt[19];
	//int pixelFormat[1];
	//unsigned int formatCount;
	int result;
	//float fieldOfView, screenAspect;
	//char *vendorString, *rendererString;

//GLX.Query(settings->wnd, false);

	// Get the device context for this window.
	devicectx = GetDeviceContext(wnd);
	if(!devicectx)
		return ERR("Error retrieving device context");
	
	/*// Support for OpenGL rendering.
	attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
	attributeListInt[1] = TRUE;

	// Support for rendering to a window.
	attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
	attributeListInt[3] = TRUE;

	// Support for hardware acceleration.
	attributeListInt[4] = WGL_ACCELERATION_ARB;
	attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;

	// Support for 24bit color.
	//attributeListInt[6] = WGL_COLOR_BITS_ARB;
	//attributeListInt[7] = 24;
attributeListInt[6] = WGL_COLOR_BITS_ARB;
attributeListInt[7] = 32;

	// Support for 24 bit depth buffer.
	attributeListInt[8] = WGL_DEPTH_BITS_ARB;
	attributeListInt[9] = 24;

	// Support for double buffer.
	attributeListInt[10] = WGL_DOUBLE_BUFFER_ARB;
	attributeListInt[11] = TRUE;

	// Support for swapping front and back buffer.
	attributeListInt[12] = WGL_SWAP_METHOD_ARB;
	attributeListInt[13] = WGL_SWAP_EXCHANGE_ARB;

	// Support for the RGBA pixel type.
	attributeListInt[14] = WGL_PIXEL_TYPE_ARB;
	attributeListInt[15] = WGL_TYPE_RGBA_ARB;

	// Support for a 8 bit stencil buffer.
	attributeListInt[16] = WGL_STENCIL_BITS_ARB;
	attributeListInt[17] = 8;

	// Null terminate the attribute list.
	attributeListInt[18] = 0;

	// Query for a pixel format that fits the attributes we want.
	result = GLX.wglChoosePixelFormatARB(devicectx, attributeListInt, NULL, 1, pixelFormat, &formatCount);
	if(result != 1)
	{
		return false;
	}*/

/*PIXELFORMATDESCRIPTOR pfd = { 
    sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
    1,                     // version number  
    PFD_DRAW_TO_WINDOW |   // support window  
    PFD_SUPPORT_OPENGL |   // support OpenGL  
    PFD_DOUBLEBUFFER |     // double buffered  
	PFD_SUPPORT_COMPOSITION,
    PFD_TYPE_RGBA,         // RGBA type  
    24,                    // 24-bit color depth  
    0, 0, 0, 0, 0, 0,      // color bits ignored  
    0,                     // no alpha buffer  
    0,                     // shift bit ignored  
    0,                     // no accumulation buffer  
    0, 0, 0, 0,            // accum bits ignored  
    32,                    // 32-bit z-buffer  
    0,                     // no stencil buffer  
    0,                     // no auxiliary buffer  
    PFD_MAIN_PLANE,        // main layer  
    0,                     // reserved  
    0, 0, 0                // layer masks ignored  
};
 
// get the best available match of pixel format for the device context   
pixelFormat[0] = ChoosePixelFormat(devicectx, &pfd); 

	// If the video card/display can handle our desired pixel format then we set it as the current one.
	result = SetPixelFormat(devicectx, pixelFormat[0], &pfd);*/ //pixelFormatDescriptor

#ifdef _WIN32
PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
PIXELFORMATDESCRIPTOR pfd = {0};
pfd.nSize = sizeof(pfd);
pfd.nVersion = 1;
pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION;
pfd.cColorBits = 24;
pfd.cAlphaBits = 8;
pfd.cDepthBits = 0;
pfd.cStencilBits = 0;
int i = ChoosePixelFormat(devicectx, &pfd);
result = SetPixelFormat(devicectx, i, &pixelFormatDescriptor);
//PIXELFORMATDESCRIPTOR pfd;
//result = SetPixelFormat(devicectx, 1, &pfd);

//DWORD err = GetLastError();
	if(result != 1)
	{
		return false;
	}
#endif

if(!GLX.Query(display, devicectx))
	return ERR("Error querying OpenGL extensions");

	// Create a OpenGL X.Y rendering context.
	Result rlt;
	CHKRESULT(CreateRenderContext(display, devicectx, ((OpenGL*)&*ogl)->oglversion_major, ((OpenGL*)&*ogl)->oglversion_minor, &renderctx));

	// Set the rendering context to active.
	if(!SelectRenderingContext(display, devicectx, renderctx))
		return ERR("Error switching to OpenGL context");
	
	// Set the depth buffer to be entirely cleared to 1.0 values.
	glClearDepth(1.0f);

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);
	
	// Set the polygon winding to front facing for the left handed system.
	glFrontFace(GL_CW);

	// Enable back face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/*// Get the name of the video card.
	vendorString = (char*)glGetString(GL_VENDOR);
	rendererString = (char*)glGetString(GL_RENDERER);

	// Store the video card name in a class member variable so it can be retrieved later.
	strcpy_s(m_videoCardDescription, vendorString);
	strcat_s(m_videoCardDescription, " - ");
	strcat_s(m_videoCardDescription, rendererString);

	// Turn on or off the vertical sync depending on the input bool value.
	if(vsync)
	{
		result = wglSwapIntervalEXT(1);
	}
	else
	{
		result = wglSwapIntervalEXT(0);
	}

	// Check if vsync was set correctly.
	if(result != 1)
	{
		return false;
	}*/

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Render all visual content for this output window
//-----------------------------------------------------------------------------
void OutputWindow::Render()
{
	if(!cam)
	{
		Result::PrintLogMessage("No camera has been assigned to the current output window");
		return;
	}

	std::list<RenderDelegate*>::const_iterator iter;

	// Clear the backbuffer
	if(backcolorenabled)
	{
		glClearColor(backcolor.r, backcolor.g, backcolor.b, backcolor.a); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else
		glClear(GL_DEPTH_BUFFER_BIT);

	LIST_FOREACH(renderlist[RT_BACKGROUND], iter)
		(*iter)->operator()(this, RT_BACKGROUND);

	LIST_FOREACH(renderlist[RT_DEFAULT], iter)
		(*iter)->operator()(this, RT_DEFAULT);

	LIST_FOREACH(renderlist[RT_FAST], iter)
		(*iter)->operator()(this, RT_FAST);

	LIST_FOREACH(renderlist[RT_FOREGROUND], iter)
		(*iter)->operator()(this, RT_FOREGROUND);

	SwapBuffers(display, devicectx);
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
	std::list<RenderDelegate*>::iterator iter;
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
	std::list<RenderDelegate*>::iterator iter;
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
	std::list<RenderDelegate*>::iterator iter;
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
// Name: CreateTexture()
// Desc: Create and load a texture
//-----------------------------------------------------------------------------
Result OutputWindow::CreateTexture(const FilePath& filename, ITexture::Usage usage, ITexture** tex)
{
	Result rlt;

	*tex = NULL;

	Texture* newtex;
	CHKALLOC(newtex = new Texture());
	CHKRESULT(newtex->Load(this, filename, usage));

	textures.push_back(newtex);
	*tex = newtex;

	return R_OK;
}
Result OutputWindow::CreateTexture(UINT width, UINT height, ITexture::Usage usage, ITexture** tex)
{
	Result rlt;

	*tex = NULL;

	Texture* newtex;
	CHKALLOC(newtex = new Texture());
	CHKRESULT(newtex->CreateNew(this, width, height, usage));

	textures.push_back(newtex);
	*tex = newtex;

	return R_OK;
}
Result OutputWindow::CreateTexture(UINT width, UINT height, ITexture::Usage usage, LPVOID data, ITexture** tex)
{
	Result rlt;

	*tex = NULL;

	Texture* newtex;
	CHKALLOC(newtex = new Texture());
	CHKRESULT(newtex->CreateNew(this, width, height, usage, 0xFFFFFFFF, data));

	textures.push_back(newtex);
	*tex = newtex;

	return R_OK;
}
Result OutputWindow::CreateTexture(ITexture* source, ITexture::Usage usage, ITexture** tex)
{
	Result rlt;

	*tex = NULL;

	Texture* newtex;
	CHKALLOC(newtex = new Texture());
	CHKRESULT(newtex->CreateCopy((Texture*)source, usage));

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
	CHKRESULT(newshader->LoadFX(filename));

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
Result OutputWindow::CreateObject(const OglShapeDesc& shapedesc, IObject** obj)
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
Result OutputWindow::CreateObject(const OglShapeDesc* shapedescs[], UINT numshapedescs, IObject** obj)
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
	std::list<Object*>::iterator iter;
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
	std::list<Object*>::iterator iter;
	LIST_FOREACH(objects, iter)
	{
		DeregisterFromRendering(*iter);
		(*iter)->Release();
	}
	objects.clear();
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void OutputWindow::Release()
{
	// Release the rendering context.
	ReleaseRenderContext(display, renderctx);

	// Release the device context.
	ReleaseDeviceContext(wnd, devicectx);

	delete this;
}