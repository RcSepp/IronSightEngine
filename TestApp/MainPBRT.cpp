#include "main.h"
#include <ISPbrt.h>
#include <ISDirect3D.h>
#include <ISDirectInput.h>
#include <ISHavok.h>

#ifdef _DEBUG
	#pragma comment (lib, "ISEngine_d.lib")
	#pragma comment (lib, "ISForms_d.lib")
	#pragma comment (lib, "ISMath_d.lib")
	#pragma comment (lib, "ISPbrt_d.lib")
	#pragma comment (lib, "ISDirect3D_d.lib")
	#pragma comment (lib, "ISDirectInput_d.lib")
	#pragma comment (lib, "ISHavok_d.lib")
#else
	#pragma comment (lib, "ISEngine.lib")
	#pragma comment (lib, "ISForms.lib")
	#pragma comment (lib, "ISMath.lib")
	#pragma comment (lib, "ISPbrt.lib")
	#pragma comment (lib, "ISDirect3D.lib")
	#pragma comment (lib, "ISDirectInput.lib")
	#pragma comment (lib, "ISHavok.lib")
#endif

//#define LEVEL_SCENE

void __stdcall OnR(int key, LPVOID user);
void __stdcall OnX(int key, LPVOID user);

//LPWINFORM pbrtform;

// Direct 3D variables
LPOUTPUTWINDOW d3dwnd;
LPRENDERSHADER sdrDefault;

// Pbrt variables
FilterDesc* pbrtfilter;
RendererDesc* pbrtrenderer;

Result __stdcall Init()
{
	Result rlt;

	// >>> Init Forms

	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create forms
	LPWINFORM d3dform;
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(320, 240);
	//frmsettings.clientsize = Size<int>(1920, 1080);
	frmsettings.caption = "D3D";
	frmsettings.options = FS_CENTERTOSCREEN;
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &d3dform));

	frmsettings.caption = "PBRT";
	frmsettings.windowpos = Point<int>(612, 100);
	//CHKRESULT(fms->CreateForm(&frmsettings, NULL, &pbrtform));

	// >>> Init DirectInput

	CHKALLOC(dip = CreateDirectInput()); dip->Sync(GLOBALVAR_LIST);

	CHKRESULT(dip->Init());
	CHKRESULT(dip->EnableMouse(true, false));
	CHKRESULT(dip->EnableKeyboard(true, false));

	/*dip.DisableKeyboard();
	dip.EnableKeyboard(true, false);
	dip.EnableMouse(true, false);*/
	dip->AddKeyHandler(Key::R, OnR, NULL);
	dip->AddKeyHandler(Key::X, OnX, NULL);

	// >>> Init Direct3D

	CHKALLOC(d3d = CreateDirect3D()); d3d->Sync(GLOBALVAR_LIST);
	D3dStartupSettings d3dsettings;
	d3dsettings.isfullscreen = false;
	CHKRESULT(d3d->Init(&d3dsettings));

	// Create Direct3D window
	IOutputWindow::Settings wndsettings;
	wndsettings.backbuffersize = Size<UINT>(d3dform->ClientWidth, d3dform->ClientHeight);
	wndsettings.wnd = d3dform->GetHwnd();
	wndsettings.enablemultisampling = false;
	wndsettings.screenidx = 0;
	CHKRESULT(d3d->CreateOutputWindow(&wndsettings, &d3dwnd));
	d3dwnd->EnableSprites("K:\\Resources\\Shader\\Common\\Sprite.fx");
	d3dwnd->EnableLineDrawing("K:\\Resources\\Shader\\Common\\Line.fx");
	d3dwnd->MakeConsoleTarget(11, 11);
	d3dwnd->ShowFps();
	//d3dwnd->backcolor = Color(0.0f, 0.0f, 0.0f);

	// Create perspective view
	LPCAMERA d3dcam;
	CHKRESULT(d3d->CreateCamera(90.0f * PI / 180.0f, 0.001f, 200.0f, &d3dcam));
	Vec3Set(&d3dcam->pos, 0.0f, 0.0f, 3.0f);
	d3dcam->rot.y = PI;
	d3dcam->viewmode = ICamera::VM_FLY;
	d3dwnd->SetCamera(d3dcam);
	d3dwnd->RegisterForUpdating(d3dcam);

#ifdef LEVEL_SCENE
	d3dcam->pos = Vector3(0.0f, 4.0f, 2.0f);
	d3dcam->rot.y = 120.0f * PI / 180.0f;
#endif

	// Create default shader
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("K:\\Resources\\Shader\\Common\\Default.fx"), &sdrDefault));

	// Create level
	LPOBJECT objLevel = NULL;
#ifdef LEVEL_SCENE
	d3dwnd->CreateObject("K:\\Resources\\Level\\Level.x", true, &objLevel);
	d3dwnd->RegisterForRendering(objLevel, RT_DEFAULT);
	objLevel->SetShader(sdrDefault);
#endif

#ifndef LEVEL_SCENE
	// Create misc
	LPOBJECT objMisc;
	/*D3dBoxShapeDesc shapedesc;
	Vec3Set(&shapedesc.pos, 0.0f, 0.0f, 0.0f);
	Vec3Set(&shapedesc.size, 1.0f, 1.0f, 1.0f);*/
	D3dSphereShapeDesc shapedesc;
	shapedesc.radius = 1.0f;
	shapedesc.slices = shapedesc.stacks = 16;
	d3dwnd->CreateObject(shapedesc, &objMisc);
	//d3dwnd->CreateObject("K:\\Resources\\Objects\\Desert Eagle.x", true, &objMisc);
	objMisc->ComputeNormals();
	objMisc->CommitChanges();
	d3dwnd->RegisterForRendering(objMisc, RT_DEFAULT);
	objMisc->SetShader(sdrDefault);
	objMisc->pos.z = 1.0f;
	objMisc->rot.x = PI / 8.0f;
	objMisc->rot.z = PI / 4.0f;
	//objMisc->scl = Vector3(5.0f, 5.0f, 5.0f);
	d3dwnd->CreateTexture("K:\\Resources\\Textures\\Wood\\CedarBoards.png", ITexture::TU_SKIN, false, &(*objMisc->mats)[0].tex);

	/*// Create sphere
	LPOBJECT objSphere;
	D3dSphereShapeDesc spheredesc;
	Vec3Set(&spheredesc.pos, 0.0f, 0.0f, 0.0f);
	spheredesc.radius = 1.0f;
	spheredesc.slices = spheredesc.stacks = 32;
	d3dwnd->CreateObject(spheredesc, &objSphere);
	d3dwnd->RegisterForRendering(objSphere, RT_DEFAULT);
	objSphere->SetShader(sdrDefault);
//	Vec3Set(&objSphere->pos, 0.0f, 4.0f, -1.0f);
	(*objSphere->mats)[0].ambient = Color(0.17f * 0.3f, 0.18f * 0.3f, 0.50f * 0.3f);
	(*objSphere->mats)[0].diffuse = Color(0.17f * 0.9f, 0.18f * 0.9f, 0.50f * 0.9f);
	(*objSphere->mats)[0].specular = Color(0.17f * 1.0f, 0.18f * 1.0f, 0.50f * 1.0f);
	(*objSphere->mats)[0].power = 200.0f;*/
#endif

	/*// Create teapot
	LPOBJECT objTeapot;
	CHKRESULT(d3dwnd->CreateObject(FilePath("Debug_teapot.x"), false, &objTeapot));
	d3dwnd->RegisterForRendering(objTeapot, RT_DEFAULT);
	objTeapot->SetShader(sdrDefault);
	//Vec3Set(&objTeapot->pos, 0.0f, 4.0f, -2.0f);
//QuaternionRotationX(&objTeapot->qrot, PI / 4.0f);
QuaternionRotationY(&objTeapot->qrot, PI / 4.0f);
objTeapot->orientmode = OM_QUAT;
	(*objTeapot->mats)[0].ambient = Color(0.17f * 0.3f, 0.18f * 0.3f, 0.50f * 0.3f);
	(*objTeapot->mats)[0].diffuse = Color(0.17f * 0.9f, 0.18f * 0.9f, 0.50f * 0.9f);
	(*objTeapot->mats)[0].specular = Color(0.17f * 1.0f, 0.18f * 1.0f, 0.50f * 1.0f);
	(*objTeapot->mats)[0].power = 200.0f;*/

	// Create lights
	LPLIGHT d3dpointlight;
	CHKRESULT(d3d->CreateLight(ILight::LT_POINT, &d3dpointlight));
#ifdef LEVEL_SCENE
	d3dpointlight->SetPos(Vector3( 2.0f, 6.0f, 1.0f));
	d3dpointlight->SetPos(Vector3(-2.0f, 6.0f, 1.0f));
#else
	d3dpointlight->SetPos(Vector3( 2.0f, 1.0f, 3.0f));
	d3dpointlight->SetPos(Vector3(-2.0f, 1.0f, 3.0f));
#endif
	CHKRESULT(d3d->CreateLight(ILight::LT_POINT, &d3dpointlight));

	// >>> Init Havok

Vector3 worldsize_min = Vector3(-50.0f, -50.0f, -50.0f);
Vector3 worldsize_max = Vector3( 50.0f,  50.0f,  50.0f);
	CHKALLOC(hvk = CreateHavok()); hvk->Sync(GLOBALVAR_LIST);
	CHKRESULT(hvk->Init(worldsize_min, worldsize_max, VT_VISUALDEBUGGER, NULL, NULL));

	// Create level entity
	if(objLevel)
	{
		LPREGULARENTITY levelentity;
		hvk->CreateCollisionMesh(objLevel, &levelentity);
	}

	hvk->InitDone();

	// >>> Init Pbrt

	//VLDMarkAllLeaksAsReported();
	CHKALLOC(pbrt = CreatePbrt()); pbrt->Sync(GLOBALVAR_LIST);
	CHKRESULT(pbrt->CreateDirectRenderView(d3dwnd));

	// Create filter
	BoxFilterDesc* boxfilter = new BoxFilterDesc();
	pbrtfilter = boxfilter;

	// Create renderer
	SamplerRendererDesc* samplerrenderer = new SamplerRendererDesc();
	LowDiscrepancySamplerDesc* samplerdesc = new LowDiscrepancySamplerDesc();
	samplerdesc->pixelsamples = 1;//16;
	samplerrenderer->sampler = samplerdesc;
	//DirectLightingSurfaceIntegratorDesc* surfaceintegratordesc;
	PathSurfaceIntegratorDesc* surfaceintegratordesc = new PathSurfaceIntegratorDesc();
	samplerrenderer->surfaceintegrator = surfaceintegratordesc;
	EmissionVolumeIntegratorDesc* volumeintegratordesc = new EmissionVolumeIntegratorDesc();
	samplerrenderer->volumeintegrator = volumeintegratordesc;
	pbrtrenderer = samplerrenderer;

	return R_OK;
}

Result __stdcall PostInit() {return R_OK;}

bool capture = false;

#define USE_CYCLIC
#ifdef USE_CYCLIC
void __stdcall Cyclic()
{
	dip->Update();
	hvk->Update();
	d3d->Render();

	if(capture)
		pbrt->Render(d3dwnd, pbrtfilter, pbrtrenderer);
}
CYCLICFUNC cyclic = Cyclic;
#else
CYCLICFUNC cyclic = NULL;
#endif

void __stdcall OnR(int key, LPVOID user)
{
	pbrt->Render(d3dwnd, pbrtfilter, pbrtrenderer);
}

LPOBJECT objBox = NULL;
LPREGULARENTITY boxentity;
void __stdcall OnX(int key, LPVOID user)
{
	if(objBox)
	{
		d3dwnd->RemoveObject(objBox);
		boxentity->Release();
	}

	D3dBoxShapeDesc cubedesc;
	Vec3Set(&cubedesc.pos, 0.0f, 0.0f, 0.0f);
	Vec3Set(&cubedesc.size, 1.0f, 1.0f, 1.0f);
	d3dwnd->CreateObject(cubedesc, &objBox);
	objBox->ComputeNormals();
	objBox->CommitChanges();
	d3dwnd->RegisterForRendering(objBox, RT_DEFAULT);
	objBox->SetShader(sdrDefault);
	objBox->pos = Vector3(-2.0f, 4.0f, 0.0f);
	Quaternion qrotz;
	QuaternionRotationX(&objBox->qrot, PI / 8.0f);
	QuaternionRotationZ(&qrotz, PI / 4.0f);
	objBox->qrot *= qrotz;
	objBox->orientmode = OM_QUAT;
	d3dwnd->CreateTexture("K:\\Resources\\Textures\\Wood\\CedarBoards.png", ITexture::TU_SKIN, false, &(*objBox->mats)[0].tex);

	HvkBoxShapeDesc bdesc;
	bdesc.size = Vector3(1.0f, 1.0f, 1.0f);
	bdesc.mass = 10.0f;
	bdesc.mtype = MT_DYNAMIC;
	bdesc.qual = CQ_MOVING;
	hvk->CreateRegularEntity(&bdesc, IHavok::LAYER_DEFAULT, &objBox->pos, &objBox->qrot, &boxentity);
	boxentity->applyLinearImpulse(&Vector3(-100.0f, 70.0f, -100.0f));

	if(dip->keys[Key::LAlt])
	{
		eng->time.Configure(true, 30.0f);
		capture = true;
	}
}

void __stdcall End()
{
	RELEASE(pbrt);
	RELEASE(d3d);
	RELEASE(dip);
	RELEASE(cle);
	RELEASE(fms);
}