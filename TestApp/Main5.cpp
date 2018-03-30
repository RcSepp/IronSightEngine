#include "main.h"
#include <ISDirect3D.h>
#include <ISDirectInput.h>
#include "RTScreenshotUtility.h"

// Direct 3D variables
LPSCENENODE cameranode;
LPOUTPUTWINDOW d3dwnd;
LPTEXTURE texrt;

RTScreenshotUtility rsu;

void __stdcall OnSpace(int key, LPVOID user);


Result __stdcall Init()
{
	Result rlt;

	// >>> Init Forms

	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create WinForm
	LPWINFORM form1;
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(1024, 1024);
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(0, 0);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form1));

	// >>> Init DirectInput

	CHKALLOC(dip = CreateDirectInput()); dip->Sync(GLOBALVAR_LIST);

	CHKRESULT(dip->Init());
	CHKRESULT(dip->EnableMouse(true, false));
	CHKRESULT(dip->EnableKeyboard(true, false));

	dip->DisableKeyboard();
	CHKRESULT(dip->EnableKeyboard(false, false));
	dip->AddKeyHandler(Key::Space, OnSpace, NULL);


	// >>> Init Direct3D

	CHKALLOC(d3d = CreateDirect3D()); d3d->Sync(GLOBALVAR_LIST);
	D3dStartupSettings d3dsettings;
	d3dsettings.isfullscreen = false;
	CHKRESULT(d3d->Init(&d3dsettings));

	// Create Direct3D window
	IOutputWindow::Settings wndsettings;
	wndsettings.backbuffersize = Size<UINT>(1920, 1200);
	wndsettings.wnd = form1->GetHwnd();
	wndsettings.enablemultisampling = true;
	wndsettings.screenidx = 0;
	CHKRESULT(d3d->CreateOutputWindow(&wndsettings, &d3dwnd));
	dip->AddKeyHandler(Key::F12, RTScreenshotUtility::OnKeyTakeScreenshot, d3dwnd);

	d3dwnd->EnableSprites("K:\\Resources\\Shader\\Common\\Sprite.fx");
	//d3dwnd->EnableLineDrawing("K:\\Resources\\Shader\\Common\\Line.fx");
	d3dwnd->MakeConsoleTarget(11, 11);
	d3dwnd->ShowFps();

	// Create render target texture
	CHKRESULT(d3dwnd->CreateTexture(wndsettings.backbuffersize.width, wndsettings.backbuffersize.height, ITexture::TU_RENDERTARGET, false, DXGI_FORMAT_R8G8B8A8_UNORM, &texrt));

	// >>> Create perspective view

	LPCAMERA cam;
	CHKRESULT(d3d->CreateCamera(90.0f * PI / 180.0f, 0.001f, 200.0f, &cam));
	d3dwnd->SetCamera(cam);

	// >>> Create overlay text ("Teaparty")

	LPD3DFONT fnt;
	CHKRESULT(d3dwnd->CreateD3dFont(FontType("Old English Text MT", 64.0f), &fnt));

	LPHUD hud;
	CHKRESULT(d3dwnd->CreateHUD(&hud));
	CHKRESULT(hud->CreateElement("Teaparty", fnt, 32, 32, DT_TOPLEFT, Color(0xFFC0C0C0), Size<int>(), 0, ID3dFont::DF_NOCLIP));
	d3dwnd->RegisterForRendering(hud, RT_FOREGROUND);

	// >>> Create 3D objects

	LPRENDERSHADER sdrDefault;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("K:\\Resources\\Shader\\Common\\Default.fx"), &sdrDefault));

	// Create table
	LPOBJECT objTable;
	D3dBoxShapeDesc tablelegdesc[4];
	Vec3Set(&tablelegdesc[0].pos, -0.54f, 0.27f, -0.34f);
	Vec3Set(&tablelegdesc[1].pos,  0.54f, 0.27f, -0.34f);
	Vec3Set(&tablelegdesc[2].pos, -0.54f, 0.27f,  0.34f);
	Vec3Set(&tablelegdesc[3].pos,  0.54f, 0.27f,  0.34f);
	Vec3Set(&tablelegdesc[0].size, 0.08f, 0.54f, 0.08f);
	tablelegdesc[3].size = tablelegdesc[2].size = tablelegdesc[1].size = tablelegdesc[0].size;
	Vec2Set(&tablelegdesc[0].texcoordscale, 0.08f, 0.54f);
	tablelegdesc[3].texcoordscale = tablelegdesc[2].texcoordscale = tablelegdesc[1].texcoordscale = tablelegdesc[0].texcoordscale;
	D3dBoxShapeDesc tabletopdesc;
	Vec3Set(&tabletopdesc.pos, 0.0f, 0.57f, 0.0f);
	Vec3Set(&tabletopdesc.size, 1.20f, 0.06f, 0.80f);

	const D3dShapeDesc* tablecompounddesc[5];
	for(int i = 0; i < ARRAYSIZE(tablelegdesc); i++)
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
	CHKRESULT(d3dwnd->CreateTexture(FilePath("K:\\Resources\\Level\\Floorwood1.DDS"), ITexture::TU_SKIN, false, &(*objFloor->mats)[0].tex));

	// >>> Create scene node graph

	LPSCENEMANAGER scenemgr;
	CHKRESULT(d3dwnd->CreateSceneManager(&scenemgr));
	d3dwnd->RegisterForRendering(scenemgr, RT_DEFAULT);
	d3dwnd->RegisterForUpdating(scenemgr);

	// Create camera node
	cameranode = scenemgr->CreateNode();
	cameranode->SetPosition(&Vector3(2.0f, 1.5f, 2.0f));
	cameranode->RotateX(0.25f * PI);
	cameranode->RotateY(-0.75f * PI);
	cameranode->AddMoveable(cam); // Add camera
	scenemgr->GetRoot()->AddChild(cameranode);

	// Add floor to scene root
	scenemgr->GetRoot()->AddRenderable(objFloor);

	// Create teapot node (not attached to scene node graph)
	LPSCENENODE teapotnode = scenemgr->CreateNode();
	teapotnode->SetScale(&Vector3(0.1f, 0.1f, 0.1f)); // Scale teapot
	teapotnode->SetPosition(&Vector3(0.0f, 0.08f, 0.0f)); // Move coordinate system to the bottom of the teapot
	teapotnode->AddRenderable(objTeapot); // Add teapot object

	// Create table node (not attached to scene node graph)
	LPSCENENODE tablenode = scenemgr->CreateNode();
	tablenode->AddRenderable(objTable); // Add table object

	// Put teapots on the table
	LPSCENENODE teapotinstancenodes[3];
	for(int i = 0; i < ARRAYSIZE(teapotinstancenodes); i++)
	{
		teapotinstancenodes[i] = scenemgr->CloneNode(teapotnode);
		teapotinstancenodes[i]->TranslateY(0.60f); // Move coordinate system to table-top
		Vector3 valign(0.20f * cos(2.0f * PI * (float)i / (float)ARRAYSIZE(teapotinstancenodes)), 0.0f, 0.20f * sin(2.0f * PI * (float)i / (float)ARRAYSIZE(teapotinstancenodes)));
		teapotinstancenodes[i]->Translate(&valign); // Allign teapots in a circle with radius = 20cm
		tablenode->AddChild(teapotinstancenodes[i]); // Put teapot on table
	}

	// Put tables in the scene 
	LPSCENENODE tableinstancenodes[4];
	for(int i = 0; i < ARRAYSIZE(tableinstancenodes); i++)
	{
		tableinstancenodes[i] = scenemgr->CloneNode(tablenode);
		scenemgr->GetRoot()->AddChild(tableinstancenodes[i]);
	}
	tableinstancenodes[0]->SetPosition(&Vector3(-1.0f, 0.0f, -1.0f));
	tableinstancenodes[1]->SetPosition(&Vector3(-1.0f, 0.0f,  1.0f));
	tableinstancenodes[2]->SetPosition(&Vector3( 1.0f, 0.0f, -1.0f));
	tableinstancenodes[3]->SetPosition(&Vector3( 1.0f, 0.0f,  1.0f));

	// Put spotlights above the tables
	LPLIGHT spotlights[4];
	for(int i = 0; i < ARRAYSIZE(spotlights); i++)
	{
		CHKRESULT(d3d->CreateLight(ILight::LT_SPOT, &spotlights[i])); // Create light
		spotlights[i]->SpotPower = 10;

		LPSCENENODE lightnode = scenemgr->CreateNode();
		lightnode->AddMoveable(spotlights[i]); // Add light
		lightnode->TranslateY(0.60f + 1.0f); // Move coordinate system to 1m above table-top
		lightnode->RotateX(-PI / 2.0f); // Move spotlight cone direction to face down
		tableinstancenodes[i]->AddChild(lightnode); // Put spotlight above table
	}

	return R_OK;
}

Result __stdcall PostInit()
{
	/*for(int i = 0; i < 25; i++)
	{
		d3d->Render();

		RTScreenshotUtility::OnKeyTakeScreenshot(0, d3dwnd);

		Quaternion camrot, deltacamrot;
		cameranode->GetOrientation(&camrot, ISceneNode::TS_WORLD);
		QuaternionRotationY(&deltacamrot, 2.0f * PI / 25.0f);
		camrot = camrot * deltacamrot;
		cameranode->SetOrientation(&camrot, ISceneNode::TS_WORLD);
	}
return ERR("Done");*/

	return R_OK;
}

int saveidx = 0;
void __stdcall OnSpace(int key, LPVOID user)
{
	LPTEXTURE texbackbuffer = d3dwnd->Backbuffer;
	d3dwnd->GetDeviceContext()->ResolveSubresource(texrt->GetD3DTexture(), 0, texbackbuffer->GetD3DTexture(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
	//d3dwnd->Backbuffer = NULL;
	texrt->Save(FilePath(String("screenshot_") << String(saveidx) << String(".png")), D3DX11_IFF_PNG);
	//d3dwnd->Backbuffer = texbackbuffer;
	saveidx++;
}

void __stdcall Cyclic()
{
	Result rlt;

	if(dip)
		dip->Update();

	Quaternion camrot, deltacamrot;
	cameranode->GetOrientation(&camrot, ISceneNode::TS_WORLD);
	QuaternionRotationY(&deltacamrot, eng->time.dt / 10.0f);
	camrot = camrot * deltacamrot;
	cameranode->SetOrientation(&camrot, ISceneNode::TS_WORLD);

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