#include "Main.h"
#include <ISDirect3D.h>
#include <ISDirectInput.h>
#include <vld.h>



LPWINFORM form1 = NULL;
LPOUTPUTWINDOW d3dwnd = NULL;
LPCAMERA cam;
LPBOXEDLEVEL blevel = NULL;
LPTEXTURE tex, tex2;
LPSPRITECONTAINER sprite;
LPOBJECT objBox;

LPSCENENODE cameranode;

const UINT chunksize[] = { 11, 11, 11 };
BoxedLevelChunk* chunks;
const UINT numchunks = 1;

LPOBJECT objPlane[16];
void __stdcall OnNumber0(int key, LPVOID user);
void __stdcall OnNumber1(int key, LPVOID user);
void __stdcall OnNumber2(int key, LPVOID user);
void __stdcall OnNumber3(int key, LPVOID user);
void __stdcall OnNumber4(int key, LPVOID user);
void __stdcall OnNumber5(int key, LPVOID user);
void __stdcall OnNumber6(int key, LPVOID user);
void __stdcall OnNumber7(int key, LPVOID user);
void __stdcall OnNumber8(int key, LPVOID user);
void __stdcall OnNumber9(int key, LPVOID user);
void __stdcall OnNumber10(int key, LPVOID user);
void __stdcall OnNumber11(int key, LPVOID user);
void __stdcall OnNumber12(int key, LPVOID user);
void __stdcall OnNumber13(int key, LPVOID user);
void __stdcall OnNumber14(int key, LPVOID user);
void __stdcall OnNumber15(int key, LPVOID user);
void __stdcall OnLeft(int key, LPVOID user);
void __stdcall OnRight(int key, LPVOID user);
void UpdatePositions();


Result __stdcall Init()
{
	Result rlt;


	// Init Forms
	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create WinForm
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(1920, 1200);
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(0, 0);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form1));


	// Init DirectInput
	CHKALLOC(dip = CreateDirectInput()); dip->Sync(GLOBALVAR_LIST);

	CHKRESULT(dip->Init());
	CHKRESULT(dip->EnableMouse(true, false));
	CHKRESULT(dip->EnableKeyboard(true, false));

	dip->AddKeyHandler(Key::Numpad0, OnNumber0, NULL);
	dip->AddKeyHandler(Key::Numpad1, OnNumber1, NULL);
	dip->AddKeyHandler(Key::Numpad2, OnNumber2, NULL);
	dip->AddKeyHandler(Key::Numpad3, OnNumber3, NULL);
	dip->AddKeyHandler(Key::Numpad4, OnNumber4, NULL);
	dip->AddKeyHandler(Key::Numpad5, OnNumber5, NULL);
	dip->AddKeyHandler(Key::Numpad6, OnNumber6, NULL);
	dip->AddKeyHandler(Key::Numpad7, OnNumber7, NULL);
	dip->AddKeyHandler(Key::Numpad8, OnNumber8, NULL);
	dip->AddKeyHandler(Key::Numpad9, OnNumber9, NULL);
	dip->AddKeyHandler(Key::X, OnNumber10, NULL);
	dip->AddKeyHandler(Key::C, OnNumber11, NULL);
	dip->AddKeyHandler(Key::V, OnNumber12, NULL);
	dip->AddKeyHandler(Key::B, OnNumber13, NULL);
	dip->AddKeyHandler(Key::N, OnNumber14, NULL);
	dip->AddKeyHandler(Key::M, OnNumber15, NULL);
	dip->AddKeyHandler(Key::Left, OnLeft, NULL);
	dip->AddKeyHandler(Key::Right, OnRight, NULL);


	// Init Direct3D
	CHKALLOC(d3d = CreateDirect3D()); d3d->Sync(GLOBALVAR_LIST);
	D3dStartupSettings d3dsettings;
	d3dsettings.isfullscreen = false;
	CHKRESULT(d3d->Init(&d3dsettings));

	IOutputWindow::Settings wndsettings;
	wndsettings.backbuffersize = Size<UINT>(1920, 1200);
	wndsettings.wnd = form1->GetHwnd();
	wndsettings.enablemultisampling = true;
	wndsettings.screenidx = 0;
	CHKRESULT(d3d->CreateOutputWindow(&wndsettings, &d3dwnd));

	d3dwnd->EnableSprites("K:\\Resources\\Shader\\Common\\Sprite.fx");
	d3dwnd->MakeConsoleTarget(11, 11);
	d3dwnd->ShowFps();

	//cle->PrintD3D(123);

	CHKRESULT(d3d->CreateCamera(90.0f * PI / 180.0f, 0.001f, 200.0f, &cam));
	/*cam->pos = Vector3(3.0f, 2.0f, 3.0f);
	cam->rot.y = 0.75f * PI;
	cam->rot.x = -0.25f * PI;*/
	//cam->pos = Vector3(0.0f, 0.0f, -2.0f);

	//cam->pos = Vector3(5.0f, 10.0f, 5.0f);
	//cam->rot.x = -0.5f * PI;
	//cam->pos = Vector3(0.0f, 0.0f, -2.0f);
	d3dwnd->SetCamera(cam);
	cam->viewmode = ICamera::VM_FLY; d3dwnd->RegisterForUpdating(cam);

	/*CHKRESULT(d3dwnd->CreateTexture(FilePath("Pause.png"), ITexture::TU_SKIN_NOMIPMAP, true, &tex));
	CHKRESULT(d3dwnd->CreateTexture(FilePath("heightmap_64x64.png"), ITexture::TU_SKIN_NOMIPMAP, true, &tex2));
	CHKRESULT(d3dwnd->CreateSpriteContainer(16, &sprite));
	d3dwnd->RegisterForRendering(sprite, RT_FOREGROUND);


	LPSKYBOX skybox;
	CHKRESULT(d3dwnd->CreateSkyBox(&skybox));
	CHKRESULT(skybox->CreateLayer(FilePath("SkyBox\\North_Sky.bmp"), FilePath("SkyBox\\East_Sky.bmp"), FilePath("SkyBox\\South_Sky.bmp"),
	FilePath("SkyBox\\West_Sky.bmp"), FilePath("SkyBox\\Top.bmp"), FilePath("SkyBox\\Bottom.bmp"), 0));
	CHKRESULT(skybox->CreateLayer(FilePath("SkyBox\\North_Scene.dds"), FilePath("SkyBox\\East_Scene.dds"), FilePath("SkyBox\\South_Scene.dds"),
	FilePath("SkyBox\\West_Scene.dds"), FilePath(""), FilePath(""), 8));
	d3dwnd->RegisterForRendering(skybox, RT_BACKGROUND);
	d3dwnd->backcolorenabled = false; // Disable screen clearing*/

	LPD3DFONT fnt;
	CHKRESULT(d3dwnd->CreateD3dFont(FontType("Old English Text MT", 64.0f), &fnt));

	LPHUD hud;
	CHKRESULT(d3dwnd->CreateHUD(&hud));
	//hud->CreateElement(tex2, 100, 0, DT_TOPLEFT, 0xFFFFFFFF, Rect<int>(25, 25, 50, 50), 1);
	//hud->CreateElement(tex2, 120, 0, DT_TOPLEFT, 0xFFFFFFFF, Rect<int>(25, 25, 50, 50), 0);
	CHKRESULT(hud->CreateElement("Teaparty", fnt, 32, 32, DT_TOPLEFT, Color(0xFFC0C0C0), Size<int>(), 0, ID3dFont::DF_NOCLIP));
	d3dwnd->RegisterForRendering(hud, RT_FOREGROUND);

	//LPRENDERSHADER sdrDefault;
	//CHKRESULT(d3dwnd->CreateRenderShader(FilePath("shader.fx"), &sdrDefault));

	LPRENDERSHADER sdrUnlit;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("K:\\Resources\\Shader\\Common\\Unlit.fx"), &sdrUnlit));

	/*// Create table
	LPOBJECT objTable;
	D3dBoxShapeDesc tablelegdesc[4];
	Vec3Set(&tablelegdesc[0].pos, -0.54f, 0.27f, -0.34f);
	Vec3Set(&tablelegdesc[1].pos, 0.54f, 0.27f, -0.34f);
	Vec3Set(&tablelegdesc[2].pos, -0.54f, 0.27f, 0.34f);
	Vec3Set(&tablelegdesc[3].pos, 0.54f, 0.27f, 0.34f);
	Vec3Set(&tablelegdesc[0].size, 0.08f, 0.54f, 0.08f);
	tablelegdesc[3].size = tablelegdesc[2].size = tablelegdesc[1].size = tablelegdesc[0].size;
	Vec2Set(&tablelegdesc[0].texcoordscale, 0.08f, 0.54f);
	tablelegdesc[3].texcoordscale = tablelegdesc[2].texcoordscale = tablelegdesc[1].texcoordscale = tablelegdesc[0].texcoordscale;
	D3dBoxShapeDesc tabletopdesc;
	Vec3Set(&tabletopdesc.pos, 0.0f, 0.57f, 0.0f);
	Vec3Set(&tabletopdesc.size, 1.20f, 0.06f, 0.80f);

	const D3dShapeDesc* tablecompounddesc[5];
	for (int i = 0; i < ARRAYSIZE(tablelegdesc); i++)
		tablecompounddesc[i] = &tablelegdesc[i];
	tablecompounddesc[4] = &tabletopdesc;
	d3dwnd->CreateObject(tablecompounddesc, 5, &objTable);
	objTable->SetShader(sdrDefault);
	(*objTable->mats)[0].ambient = Color(0.55f * 0.3f, 0.46f * 0.3f, 0.34f * 0.3f);
	(*objTable->mats)[0].diffuse = Color(0.55f, 0.46f, 0.34f);
	(*objTable->mats)[0].specular = Color(0.3f, 0.3f, 0.3f);
	(*objTable->mats)[0].power = 10.0f;
	CHKRESULT(d3dwnd->CreateTexture(FilePath("K:\\Resources\\Level\\Wood2.DDS"), ITexture::TU_SKIN, false, &(*objTable->mats)[0].tex));

	// Create teapot
	LPOBJECT objTeapot;
	CHKRESULT(d3dwnd->CreateObject(FilePath("Debug_teapot.x"), false, &objTeapot));
	objTeapot->SetShader(sdrDefault);
	(*objTeapot->mats)[0].ambient = Color(0.6f * 0.3f, 0.6f * 0.3f, 0.7f * 0.3f);
	(*objTeapot->mats)[0].diffuse = Color(0.6f, 0.6f, 0.7f);
	(*objTeapot->mats)[0].specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	(*objTeapot->mats)[0].power = 70.0f;
	//d3dwnd->RegisterForRendering(objTeapot, RT_DEFAULT);

	// Create floor
	LPOBJECT objFloor;
	D3dPlaneShapeDesc floordesc;
	Vec3Set(&floordesc.pos, 0.0f, 0.0f, 0.0f);
	Vec3Set(&floordesc.nml, 0.0f, 1.0f, 0.0f);
	Vec2Set(&floordesc.size, 10.0f, 10.0f);
	Vec2Set(&floordesc.texcoordscale, 20.0f, 20.0f);
	d3dwnd->CreateObject(floordesc, &objFloor);
	objFloor->SetShader(sdrDefault);
	(*objFloor->mats)[0].ambient = Color(0.3f, 0.3f, 0.3f);
	(*objFloor->mats)[0].diffuse = Color(1.0f, 1.0f, 1.0f);
	(*objFloor->mats)[0].specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	(*objFloor->mats)[0].power = 50.0f;
	CHKRESULT(d3dwnd->CreateTexture(FilePath("K:\\Resources\\Level\\Floorwood1.DDS"), ITexture::TU_SKIN, false, &(*objFloor->mats)[0].tex));*/

	for (int i = 0; i < 16; ++i)
	{
		LPOBJECT _objPlane;
		D3dPlaneShapeDesc planedesc;
		//Vec3Set(&planedesc.pos, radius * sin(alpha[i] * PI / 180.0f), 0.0f, radius * cos(alpha[i] * PI / 180.0f));
		//Vec3Inverse(&planedesc.nml, &planedesc.pos);
		//Vec3Normalize(&planedesc.nml, &planedesc.nml);
		Vec3Set(&planedesc.pos, 0.0f, 0.0f, 0.0f);
		Vec3Set(&planedesc.nml, 0.0f, 0.0f, -1.0f);
		Vec2Set(&planedesc.size, 1.5f, 1.0f);
		Vec2Set(&planedesc.texcoordscale, 1.0f, -1.0f);
		d3dwnd->CreateObject(planedesc, &_objPlane);
		objPlane[i] = _objPlane;
		_objPlane->SetShader(sdrUnlit);
		CHKRESULT(d3dwnd->CreateTexture((FilePath)(String("K:\\Android\\PanoApp\\testset2_small\\") << String(i + 1) << String(".jpg")), ITexture::TU_SKIN, false, &(*_objPlane->mats)[0].tex));
		d3dwnd->RegisterForRendering(_objPlane, RT_DEFAULT);
	}
	UpdatePositions();

	return R_OK;
}

Result __stdcall PostInit()
{
	return R_OK;
}


int currentId = 0;
void __stdcall OnNumber0(int key, LPVOID user) { currentId = 0; }
void __stdcall OnNumber1(int key, LPVOID user) { currentId = 1; }
void __stdcall OnNumber2(int key, LPVOID user) { currentId = 2; }
void __stdcall OnNumber3(int key, LPVOID user) { currentId = 3; }
void __stdcall OnNumber4(int key, LPVOID user) { currentId = 4; }
void __stdcall OnNumber5(int key, LPVOID user) { currentId = 5; }
void __stdcall OnNumber6(int key, LPVOID user) { currentId = 6; }
void __stdcall OnNumber7(int key, LPVOID user) { currentId = 7; }
void __stdcall OnNumber8(int key, LPVOID user) { currentId = 8; }
void __stdcall OnNumber9(int key, LPVOID user) { currentId = 9; }
void __stdcall OnNumber10(int key, LPVOID user) { currentId = 10; }
void __stdcall OnNumber11(int key, LPVOID user) { currentId = 11; }
void __stdcall OnNumber12(int key, LPVOID user) { currentId = 12; }
void __stdcall OnNumber13(int key, LPVOID user) { currentId = 13; }
void __stdcall OnNumber14(int key, LPVOID user) { currentId = 14; }
void __stdcall OnNumber15(int key, LPVOID user) { currentId = 15; }

//float alpha[] = { 0.0f, 22.5f, 45.0f, 67.5f, 90.0f, 112.5f, 135.0f, 157.5f, 180.0f, 202.5f, 225.0f, 247.5f, 270.0f, 292.5f, 315.0f, 337.5f };
float alpha[] = { 0.000000, 20.500000, 42.000000, 59.500000, 88.000000, 110.500000, 125.000000, 151.500000, 182.000000, 208.500000, 231.000000, 245.500000, 269.000000, 293.500000, 314.000000, 340.500000 };
float radius = 2.45f;
void UpdatePositions()
{
	for (int i = 0; i < 16; ++i)
	{
		LPOBJECT _objPlane = objPlane[i];
		Vec3Set(&_objPlane->pos, radius * sin(alpha[i] * PI / 180.0f), 0.0f, radius * cos(alpha[i] * PI / 180.0f));
		_objPlane->rot.y = -alpha[i] * PI / 180.0f;
	}
	char buf[1024];
	sprintf(buf, "float alpha[] = { %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f };",
		alpha[0], alpha[1], alpha[2], alpha[3], alpha[4], alpha[5], alpha[6], alpha[7], alpha[8], alpha[9], alpha[10], alpha[11], alpha[12], alpha[13], alpha[14], alpha[15]
	);
	cle->PrintLine(buf);
}

void __stdcall OnLeft(int key, LPVOID user)
{
	alpha[currentId] -= 1.0f;
	UpdatePositions();
}
void __stdcall OnRight(int key, LPVOID user)
{
	alpha[currentId] += 1.0f;
	UpdatePositions();
}

void __stdcall Cyclic()
{
	Result rlt;

	if (dip)
		dip->Update();

	if (d3d)
		d3d->Render();
}
CYCLICFUNC cyclic = Cyclic;

void __stdcall End()
{
	RELEASE(d3d);
	RELEASE(dip);
	RELEASE(cle);
	RELEASE(fms);
}