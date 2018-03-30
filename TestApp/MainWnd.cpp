#include "main.h"
#include <ISDirect3D.h>
#include <ISDirectInput.h>


Result __stdcall Init()
{
	Result rlt;

	// >>> Init Forms

	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create WinForm
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(800, 600);

	LPWINFORM form1;
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(100, 100);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form1));

	LPWINFORM form2;
	frmsettings.caption = "Iron Sight Engine - Window 2";
	frmsettings.windowpos = Point<int>(1000, 100);
	frmsettings.clientsize = Size<int>(600, 400);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form2));

	// >>> Init DirectInput

	CHKALLOC(dip = CreateDirectInput()); dip->Sync(GLOBALVAR_LIST);

	CHKRESULT(dip->Init());
	CHKRESULT(dip->EnableMouse(true, false));
	CHKRESULT(dip->EnableKeyboard(true, false));

	dip->DisableKeyboard();
	CHKRESULT(dip->EnableKeyboard(false, false));


	// >>> Init Direct3D

	CHKALLOC(d3d = CreateDirect3D()); d3d->Sync(GLOBALVAR_LIST);
	D3dStartupSettings d3dsettings;
	d3dsettings.isfullscreen = false;
	CHKRESULT(d3d->Init(&d3dsettings));

	// Create Direct3D window
	IOutputWindow::Settings wndsettings;
	wndsettings.backbuffersize = Size<UINT>(1920, 1200);
	wndsettings.enablemultisampling = true;
	wndsettings.screenidx = 0;

	LPOUTPUTWINDOW wnd1;
	wndsettings.wnd = form1->GetHwnd();
	CHKRESULT(d3d->CreateOutputWindow(&wndsettings, &wnd1));

	LPOUTPUTWINDOW wnd2;
	wndsettings.wnd = form2->GetHwnd();
	CHKRESULT(d3d->CreateOutputWindow(&wndsettings, &wnd2));

	wnd1->EnableSprites("K:\\Resources\\Shader\\Common\\Sprite.fx");
	wnd1->MakeConsoleTarget(11, 11);
	wnd1->ShowFps();

	wnd2->EnableSprites("K:\\Resources\\Shader\\Common\\Sprite.fx");
	wnd2->ShowFps();
	wnd2->backcolor.g = 1.0f;

	// >>> Create perspective view

	LPCAMERA cam;
	CHKRESULT(d3d->CreateCamera(90.0f * PI / 180.0f, 0.001f, 200.0f, &cam));
	cam->viewmode = ICamera::VM_FLY;
	wnd1->SetCamera(cam);
	wnd1->RegisterForUpdating(cam);
	wnd2->SetCamera(cam);
	wnd2->RegisterForUpdating(cam);

	// Create shader
	LPRENDERSHADER sdrDefault;
	CHKRESULT(wnd1->CreateRenderShader(FilePath("K:\\Resources\\Shader\\Common\\Default.fx"), &sdrDefault));

	// Create teapot
	LPOBJECT objTeapot;
	CHKRESULT(wnd1->CreateObject(FilePath("Debug_teapot.x"), false, &objTeapot));
	objTeapot->SetShader(sdrDefault);
	(*objTeapot->mats)[0].ambient = Color(0.6f * 0.3f, 0.6f * 0.3f, 0.7f * 0.3f);
	(*objTeapot->mats)[0].diffuse = Color(0.6f, 0.6f, 0.7f);
	(*objTeapot->mats)[0].specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	(*objTeapot->mats)[0].power = 70.0f;
	wnd1->RegisterForRendering(objTeapot, RT_DEFAULT);
	wnd2->RegisterForRendering(objTeapot, RT_DEFAULT);

	// Create sphere
	LPOBJECT objSphere;
	D3dSphereShapeDesc sdesc;
	sdesc.radius = 0.2f;
	sdesc.slices = sdesc.stacks = 100;
	CHKRESULT(wnd1->CreateObject(sdesc, &objSphere));
	wnd1->RegisterForRendering(objSphere, RT_DEFAULT);
	wnd2->RegisterForRendering(objSphere, RT_DEFAULT);
	objSphere->SetShader(sdrDefault);

	return R_OK;
}

Result __stdcall PostInit()
{
	return R_OK;
}

void __stdcall Cyclic()
{
	if(dip)
		dip->Update();

	if(d3d)
		d3d->Render();
}
CYCLICFUNC cyclic = Cyclic;
//CYCLICFUNC cyclic = NULL;

void __stdcall End()
{
	RELEASE(d3d);
	RELEASE(cle);
	RELEASE(fms);
}