#include "main.h"
#define OPEN_GL
#ifdef OPEN_GL
#include <ISOpenGL.h>
#else
#include <ISDirect3D.h>
#endif

LPWINFORM form;

Result __stdcall Init()
{
	Result rlt;

	// >>> Init Forms

	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create forms
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(512, 512);
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(100, 100);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form));

	// >>> Init OpenGL

#ifdef OPEN_GL
	CHKALLOC(ogl = CreateOpenGL()); ogl->Sync(GLOBALVAR_LIST);
	OglStartupSettings oglsettings;
	oglsettings.oglversion_major = 3;
	oglsettings.oglversion_minor = 0;
	oglsettings.isfullscreen = false;
	CHKRESULT(ogl->Init(&oglsettings));
#else
	CHKALLOC(d3d = CreateDirect3D()); d3d->Sync(GLOBALVAR_LIST);
	D3dStartupSettings d3dsettings;
	d3dsettings.isfullscreen = false;
	CHKRESULT(d3d->Init(&d3dsettings));
#endif

	return R_OK;
}

Result __stdcall PostInit()
{
	Result rlt;

	// Create OpenGL window
	LPOUTPUTWINDOW oglwnd;
	IOutputWindow::Settings wndsettings;
	wndsettings.backbuffersize = Size<UINT>(1920, 1200);
	wndsettings.wnd = form->GetHwnd();
	wndsettings.enablemultisampling = true;
	wndsettings.screenidx = 0;
#ifdef OPEN_GL
	CHKRESULT(ogl->CreateOutputWindow(&wndsettings, &oglwnd));
#else
	CHKRESULT(d3d->CreateOutputWindow(&wndsettings, &oglwnd));
#endif

	// Create default shader
	LPRENDERSHADER sdrDefault;
#ifdef OPEN_GL
	CHKRESULT(oglwnd->CreateRenderShader("default.c", &sdrDefault));
#else
	CHKRESULT(oglwnd->CreateRenderShader("K:\\Resources\\Shader\\Common\\Unlit.fx", &sdrDefault));
#endif

	// Create object
	LPOBJECT obj;
	CHKRESULT(oglwnd->CreateObject("K:\\Resources\\Objects\\human.x", true, &obj)); //"K:\\Resources\\Objects\\G36C.x"
/*#ifdef OPEN_GL
	OglBoxShapeDesc bdesc;
#else
	D3dBoxShapeDesc bdesc;
#endif
	bdesc.pos = Vector3(0.0f, 0.0f, 0.0f);
	bdesc.size = Vector3(1.0f, 1.0f, 1.0f);
	CHKRESULT(oglwnd->CreateObject(bdesc, &obj));*/
	obj->SetShader(sdrDefault);
	oglwnd->RegisterForRendering(obj, RT_DEFAULT);
	float offset;
	if(obj->GetBoundingBoxSize().z > obj->GetBoundingBoxSize().x)
	{
		offset = obj->GetBoundingBoxSize().z;
		obj->rot.y = PI / 2.0f;
	}
	else
		offset = obj->GetBoundingBoxSize().x;
	obj->pos = Vector3(0.0f, 0.0f, offset);
	obj->rot.x = 0.5f;

	// Create perspective view
	LPCAMERA oglcam;
#ifdef OPEN_GL
	CHKRESULT(ogl->CreateCamera(90.0f * PI / 180.0f, 0.001f, 4.0f * offset, &oglcam));
#else
	CHKRESULT(d3d->CreateCamera(90.0f * PI / 180.0f, 0.001f, 200.0f, &oglcam));
#endif
	//Vec3Set(&oglcam->pos, 0.0f, 0.0f, 3.0f);
	//oglcam->rot.y = PI;
	oglcam->viewmode = ICamera::VM_DEFAULT;
	oglwnd->SetCamera(oglcam);
	oglwnd->RegisterForUpdating(oglcam);

	return R_OK;
}

void __stdcall Cyclic()
{
#ifdef OPEN_GL
	if(ogl)
		ogl->Render();
#else
	if(d3d)
		d3d->Render();
#endif
}
CYCLICFUNC cyclic = Cyclic;

void __stdcall End()
{
	RELEASE(cle);
	RELEASE(fms);
}