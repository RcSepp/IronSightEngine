#include "GLX.h"

OpenGLExtensions GLX;

#ifdef _WIN32

#define glGetProcAddress(str) wglGetProcAddress(str)

DeviceContext GetDeviceContext(WindowHandle wnd)
{
	return GetDC(wnd);
}
Result CreateRenderContext(DisplayHandle display, DeviceContext devicectx, int version_major, int version_minor, RenderContext* renderctx)
{
#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092

	// Set OpenGL version in the attribute list
	const int attrlist[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, version_major,
		WGL_CONTEXT_MINOR_VERSION_ARB, version_minor,
		0 // Null terminator
	};

	*renderctx = GLX.wglCreateContextAttribsARB(devicectx, NULL, attrlist);
	if(!*renderctx)
		return ERR("Error creating OpenGL context");

	return R_OK;
}
bool SelectRenderingContext(DisplayHandle display, DeviceContext devicectx, RenderContext renderctx)
{
	return wglMakeCurrent(devicectx, renderctx) != 0;
}
void ReleaseDeviceContext(WindowHandle wnd, DeviceContext devicectx)
{
	if(devicectx)
	{
		ReleaseDC(wnd, devicectx);
		devicectx = NULL;
	}
}
void ReleaseRenderContext(DisplayHandle display, RenderContext renderctx)
{
	if(renderctx)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(renderctx);
		renderctx = NULL;
	}
}
void SwapBuffers(DisplayHandle display, DeviceContext devicectx)
{
	SwapBuffers(devicectx);
}
DisplayHandle CreateDisplay()
{
	return NULL;
}

#else

#define glGetProcAddress(str) glXGetProcAddress((const GLubyte*)str)

DeviceContext GetDeviceContext(WindowHandle wnd)
{
	return wnd;
}
Result CreateRenderContext(DisplayHandle display, DeviceContext devicectx, int version_major, int version_minor, RenderContext* renderctx)
{
	// Check GLX version support
	int major, minor;
	if(!glXQueryVersion( display, &major, &minor))
		return ERR("Error querying maximum supported OpenGL version");
	if((( major == version_major ) && ( minor < 4 ) ) || ( major < 1 ))
		return ERR(String("Unsupported GLX version, maximum supported version = ") << String(major) << String('.') << String(minor));

	// Check frame buffer configuration support
	static int visual_attribs[] =
	{
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		//GLX_SAMPLE_BUFFERS  , 1,
		//GLX_SAMPLES         , 4,
		None
	};
	int fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
	if(!fbc)
		return ERR("Failed to retrieve a framebuffer config");

// Pick frame buffer configuration with the most samples per pixel
//printf( "Getting XVisualInfos\n" );
int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
for(int i = 0; i < fbcount; ++i)
{
	XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
	if(vi)
	{
		int samp_buf, samples;
		glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
		glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );
 
		//printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d, SAMPLES = %d\n", i, vi -> visualid, samp_buf, samples );
 
		if(best_fbc < 0 || samp_buf && samples > best_num_samp)
			best_fbc = i, best_num_samp = samples;
		if(worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
			worst_fbc = i, worst_num_samp = samples;
	}
	XFree(vi);
}
GLXFBConfig bestfbc = fbc[best_fbc];
XFree(fbc);

	int XGL_CONTEXT_MAJOR_VERSION_ARB = 0, XGL_CONTEXT_MINOR_VERSION_ARB = 0;
    glGetIntegerv(GL_MAJOR_VERSION,&XGL_CONTEXT_MAJOR_VERSION_ARB);
    glGetIntegerv(GL_MINOR_VERSION,&XGL_CONTEXT_MINOR_VERSION_ARB);

	// Set OpenGL version in the attribute list
	const int attrlist[] = {
		XGL_CONTEXT_MAJOR_VERSION_ARB, version_major,
		XGL_CONTEXT_MINOR_VERSION_ARB, version_minor,
		0 // Null terminator
	};

	*renderctx = GLX.wglCreateContextAttribsARB(display, bestfbc, NULL, true, attrlist);
	if(!*renderctx)
		return ERR("Error creating OpenGL context");

	return R_OK;
}
bool SelectRenderingContext(DisplayHandle display, DeviceContext devicectx, RenderContext renderctx)
{
	return glXMakeCurrent(display, devicectx, renderctx);
}
void ReleaseDeviceContext(WindowHandle wnd, DeviceContext devicectx)
{
	devicectx = 0;
}
void ReleaseRenderContext(DisplayHandle display, RenderContext renderctx)
{
	if(renderctx)
	{
		glXMakeCurrent(display, None, NULL);
		glXDestroyContext(display, renderctx);
		renderctx = NULL;
	}
}
void SwapBuffers(DisplayHandle display, DeviceContext devicectx)
{
	glXSwapBuffers(display, devicectx);
}
DisplayHandle CreateDisplay()
{
	return XOpenDisplay(NULL);
}

#endif

bool OpenGLExtensions::Query(DisplayHandle display, DeviceContext devicectx)
{
	if(queried)
		return true;

#ifdef _WIN32
	// Create temporary rendering context
	RenderContext renderctx = wglCreateContext(devicectx);

	// Set the temporary rendering context as the current rendering context for this window
	int error = wglMakeCurrent(devicectx, renderctx);
	if(error != 1)
		return false;
#else
	// Create temporary rendering context
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo* vi = glXChooseVisual(display, 0, att);
	RenderContext renderctx = glXCreateContext(display, vi, NULL, GL_TRUE);

	// Set the temporary rendering context as the current rendering context for this window
	glXMakeCurrent(display, devicectx, renderctx);
#endif

	glGenBuffers = (PFNGLGENBUFFERSPROC)glGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)glGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)glGetProcAddress("glBufferData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glGetProcAddress("glDeleteBuffers");

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)glGetProcAddress("glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glGetProcAddress("glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)glGetProcAddress("glUseProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)glGetProcAddress("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)glGetProcAddress("glDetachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)glGetProcAddress("glLinkProgram");
	glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)glGetProcAddress("glProgramParameteri");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glGetProcAddress("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glGetProcAddress("glGetProgramInfoLog");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glGetProcAddress("glGetShaderInfoLog");
	glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)glGetProcAddress("glGetProgramBinary");
	glProgramBinary = (PFNGLPROGRAMBINARYPROC)glGetProcAddress("glProgramBinary");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glGetProcAddress("glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)glGetProcAddress("glUniform1i");
	glUniform2i = (PFNGLUNIFORM2IPROC)glGetProcAddress("glUniform2i");
	glUniform3i = (PFNGLUNIFORM3IPROC)glGetProcAddress("glUniform3i");
	glUniform4i = (PFNGLUNIFORM4IPROC)glGetProcAddress("glUniform4i");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)glGetProcAddress("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)glGetProcAddress("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)glGetProcAddress("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)glGetProcAddress("glUniform4iv");
	glUniform1f = (PFNGLUNIFORM1FPROC)glGetProcAddress("glUniform1f");
	glUniform2f = (PFNGLUNIFORM2FPROC)glGetProcAddress("glUniform2f");
	glUniform3f = (PFNGLUNIFORM3FPROC)glGetProcAddress("glUniform3f");
	glUniform4f = (PFNGLUNIFORM4FPROC)glGetProcAddress("glUniform4f");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)glGetProcAddress("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)glGetProcAddress("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)glGetProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)glGetProcAddress("glUniform4fv");
	glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)glGetProcAddress("glUniformMatrix3fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glGetProcAddress("glUniformMatrix4fv");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)glGetProcAddress("glGetAttribLocation");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)glGetProcAddress("glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)glGetProcAddress("glVertexAttrib1fv");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)glGetProcAddress("glVertexAttrib2fv");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)glGetProcAddress("glVertexAttrib3fv");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)glGetProcAddress("glVertexAttrib4fv");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glGetProcAddress("glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glGetProcAddress("glEnableVertexAttribArray");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)glGetProcAddress("glBindAttribLocation");
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)glGetProcAddress("glActiveTexture");

	glCreateShader = (PFNGLCREATESHADERPROC)glGetProcAddress("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)glGetProcAddress("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)glGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)glGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)glGetProcAddress("glGetShaderiv");

	glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)glGetProcAddress("glBindBufferBase");

	//wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)glGetProcAddress("wglChoosePixelFormatARB");
#ifdef _WIN32
	wglCreateContextAttribsARB = (PFNGLCREATECONTEXTATTRIBSARBPROC)glGetProcAddress("wglCreateContextAttribsARB");
#else
	wglCreateContextAttribsARB = (PFNGLCREATECONTEXTATTRIBSARBPROC)glGetProcAddress("glXCreateContextAttribsARB");
#endif

	// Release temporary rendering context
	ReleaseRenderContext(display, renderctx);

	queried = true;
	return true;
}