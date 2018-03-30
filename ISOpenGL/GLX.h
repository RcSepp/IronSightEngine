#ifndef __GLX_H
#define __GLX_H

#include <ISTypes.h>

#ifdef _WIN32

#include <GL/glut.h>
#include "glext.h"

typedef LPVOID DisplayHandle;
typedef HWND WindowHandle;
typedef HDC DeviceContext;
typedef HGLRC RenderContext;

//typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef RenderContext (WINAPI * PFNGLCREATECONTEXTATTRIBSARBPROC) (DeviceContext devicectx, RenderContext sharectx, const int *attribList);

#else

#include<GL/gl.h>
#include<GL/glx.h>

#ifndef Window
typedef long unsigned int Window;
#endif

typedef Display* DisplayHandle;
typedef Window WindowHandle;
typedef Window DeviceContext;
typedef GLXContext RenderContext;

typedef RenderContext (* PFNGLCREATECONTEXTATTRIBSARBPROC)(DisplayHandle display, GLXFBConfig config, RenderContext sharectx, Bool direct, const int* attrib_list);

#endif

DeviceContext GetDeviceContext(WindowHandle wnd);
Result CreateRenderContext(DisplayHandle display, DeviceContext devicectx, int version_major, int version_minor, RenderContext* renderctx);
bool SelectRenderingContext(DisplayHandle display, DeviceContext devicectx, RenderContext renderctx);
void ReleaseDeviceContext(WindowHandle wnd, DeviceContext devicectx);
void ReleaseRenderContext(DisplayHandle display, RenderContext renderctx);
void SwapBuffers(DisplayHandle display, DeviceContext devicectx);
DisplayHandle CreateDisplay();

//-----------------------------------------------------------------------------
// Name: OpenGLExtensions
// Desc: Contains OpenGL functions above OpenGL 1.1, which have to be querried at runtime
//-----------------------------------------------------------------------------
extern struct OpenGLExtensions
{
	bool queried;

	OpenGLExtensions() : queried(false) {}

	PFNGLGENBUFFERSPROC glGenBuffers;
	PFNGLBINDBUFFERPROC glBindBuffer;
	PFNGLBUFFERDATAPROC glBufferData;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers;

	PFNGLCREATEPROGRAMPROC glCreateProgram;
	PFNGLDELETEPROGRAMPROC glDeleteProgram;
	PFNGLUSEPROGRAMPROC glUseProgram;
	PFNGLATTACHSHADERPROC glAttachShader;
	PFNGLDETACHSHADERPROC glDetachShader;
	PFNGLLINKPROGRAMPROC glLinkProgram;
	PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;
	PFNGLGETPROGRAMIVPROC glGetProgramiv;
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	PFNGLGETPROGRAMBINARYPROC glGetProgramBinary;
	PFNGLPROGRAMBINARYPROC glProgramBinary;
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	PFNGLUNIFORM1IPROC glUniform1i;
	PFNGLUNIFORM2IPROC glUniform2i;
	PFNGLUNIFORM3IPROC glUniform3i;
	PFNGLUNIFORM4IPROC glUniform4i;
	PFNGLUNIFORM1IVPROC glUniform1iv;
	PFNGLUNIFORM2IVPROC glUniform2iv;
	PFNGLUNIFORM3IVPROC glUniform3iv;
	PFNGLUNIFORM4IVPROC glUniform4iv;
	PFNGLUNIFORM1FPROC glUniform1f;
	PFNGLUNIFORM2FPROC glUniform2f;
	PFNGLUNIFORM3FPROC glUniform3f;
	PFNGLUNIFORM4FPROC glUniform4f;
	PFNGLUNIFORM1FVPROC glUniform1fv;
	PFNGLUNIFORM2FVPROC glUniform2fv;
	PFNGLUNIFORM3FVPROC glUniform3fv;
	PFNGLUNIFORM4FVPROC glUniform4fv;
	PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
	PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
	PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
	PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
	PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
	PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
	PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
	PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
	PFNGLACTIVETEXTUREPROC glActiveTexture;

	PFNGLCREATESHADERPROC glCreateShader;
	PFNGLDELETESHADERPROC glDeleteShader;
	PFNGLSHADERSOURCEPROC glShaderSource;
	PFNGLCOMPILESHADERPROC glCompileShader;
	PFNGLGETSHADERIVPROC glGetShaderiv;

	PFNGLBINDBUFFERBASEPROC glBindBufferBase;

	//PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	PFNGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

	bool Query(DisplayHandle display, DeviceContext devicectx);
} GLX;

#endif