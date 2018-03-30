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

const UINT chunksize[] = {11, 11, 11};
BoxedLevelChunk* chunks;
const UINT numchunks = 1;

void __stdcall OnSpace(int key, LPVOID user);

Result __stdcall Init()
{
	Result rlt;


	// Init Forms
	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create WinForm
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(1024, 1024);
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(0, 0);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form1));


	// Init DirectInput
	CHKALLOC(dip = CreateDirectInput()); dip->Sync(GLOBALVAR_LIST);

	CHKRESULT(dip->Init());
	CHKRESULT(dip->EnableMouse(true, false));
	CHKRESULT(dip->EnableKeyboard(true, false));

	dip->DisableKeyboard();
	CHKRESULT(dip->EnableKeyboard(false, false));
	dip->AddKeyHandler(Key::Space, OnSpace, NULL);


	// Init Direct3D
	CHKALLOC(d3d = CreateDirect3D()); d3d->Sync(GLOBALVAR_LIST);
	D3dStartupSettings d3dsettings;
	d3dsettings.isfullscreen = false;
	CHKRESULT(d3d->Init(&d3dsettings));

	IOutputWindow::Settings wndsettings;
	wndsettings.backbuffersize = Size<UINT>(1920, 1200);
	wndsettings.wnd = form1->GetHwnd();
	wndsettings.enablemultisampling = false;//true;
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
	d3dwnd->SetCamera(cam);
//cam->viewmode = ICamera::VM_FLY; d3dwnd->RegisterForUpdating(cam);

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

	LPRENDERSHADER sdrDefault;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("shader.fx"), &sdrDefault));

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
	CHKRESULT(d3dwnd->CreateTexture(FilePath("K:\\Resources\\Level\\Floorwood1.DDS"), ITexture::TU_SKIN, false, &(*objFloor->mats)[0].tex));

/*LPOBJECT objSphere;
D3dSphereShapeDesc spheredesc;
Vec3Set(&spheredesc.pos, 0.0f, 0.0f, 0.0f);
spheredesc.radius = 1.0f;
spheredesc.slices = spheredesc.stacks = 32;
d3dwnd->CreateObject(spheredesc, &objSphere);
d3dwnd->RegisterForRendering(objSphere, RT_DEFAULT);
objSphere->SetShader(sdrDefault);
//objSphere->pos.z = 2.0f;*/

	/*// Create player
	LPOBJECT objPlayer;
	LPRENDERSHADER sdrBlend;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("shader.fx"), &sdrBlend)); //shader withblending.fx
	CHKRESULT(d3dwnd->CreateObject(FilePath("human.x"), true, &objPlayer)); //human_withbones.x
	objPlayer->pos = Vector3(0.0f, 0.0f, 0.0f);
	d3dwnd->RegisterForRendering(objPlayer, RT_DEFAULT);
	objPlayer->SetShader(sdrBlend);*/

	LPSCENEMANAGER scenemgr;
	CHKRESULT(d3dwnd->CreateSceneManager(&scenemgr));
	d3dwnd->RegisterForRendering(scenemgr, RT_DEFAULT);
	d3dwnd->RegisterForUpdating(scenemgr);

	/*LPSCENENODE somescenenode = scenemgr->CreateNode();
	scenemgr->GetRoot()->AddChild(somescenenode);
	somescenenode->SetPosition(&Vector3(2.0f, 0.0f, 2.0f));
	Quaternion qrot;
	QuaternionRotationY(&qrot, 0.5f * PI);
	somescenenode->SetOrientation(&qrot, ISceneNode::TS_PARENT);
	somescenenode->AddRenderable(objPlayer);*/

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


//scenemgr->GetRoot()->AddRenderable(objPlayer);

/*LPRENDERSHADER sdrDefault;
D3dBoxShapeDesc bdesc;
bdesc.pos = Vector3(0.0f, 0.0f, 0.0f);
bdesc.size = Vector3(1.0f, 1.0f, 1.0f);
CHKRESULT(d3dwnd->CreateRenderShader(FilePath("shader.fx"), &sdrDefault));
CHKRESULT(d3dwnd->CreateObject(bdesc, &objBox));
d3dwnd->RegisterForRendering(objBox, RT_DEFAULT);
objBox->SetShader(sdrDefault);

objBox->orientmode = OM_QUAT;
objBox->pos = Vector3(0.0f, 0.0f, 0.0f);*/


	/*// Create chunks
	CHKALLOC(chunks = new IBoxedLevel::Chunk[numchunks]);
	for(UINT i = 0; i < numchunks; i++)
	{
		chunks[i].boxmask = new IBoxedLevel::BoxDesc[chunksize[0] * chunksize[1] * chunksize[2]];
		ZeroMemory(chunks[i].boxmask, chunksize[0] * chunksize[1] * chunksize[2] * sizeof(IBoxedLevel::BoxDesc));
		chunks[i].numboxes = 0;
		chunks[i].pos = Vector3((float)i * 11.0f + 5.0f, 0.0f, 0.0f);
	}

	// Create boxed level
	LPTEXTURE texboxes;
	CHKRESULT(d3dwnd->CreateTexture("MincraftDefaultTexture.png", ITexture::TU_SKIN, false, &texboxes));

	IBoxedLevel::BoxType* boxtypes = new IBoxedLevel::BoxType[3];
	boxtypes[0].SetTextureIndices(3);
	boxtypes[0].textureindex[4] = 146;
	boxtypes[0].shape = IBoxedLevel::SHAPE_CUBIC;

	boxtypes[1].SetTextureIndices(3);
	boxtypes[1].textureindex[4] = 146;
	boxtypes[1].shape = IBoxedLevel::SHAPE_STAIRS;

	boxtypes[2].SetTextureIndices(8);
	boxtypes[2].textureindex[4] = 10;
	boxtypes[2].shape = IBoxedLevel::SHAPE_CUBIC;

	for(int z = 0; z <= 10; z++)
		for(int x = 0; x <= 10; x++)
		{
			chunks[0].boxmask[z * (chunksize[0] * chunksize[1]) + 0 * chunksize[0] + x] = IBoxedLevel::BoxDesc(1);
			chunks[0].numboxes++;
		}
	for(int x = 0; x <= 5; x++)
		for(int y = 1; y <= x; y++)
		{
			chunks[0].boxmask[5 * (chunksize[0] * chunksize[1]) + y * chunksize[0] + x] = x == y ? IBoxedLevel::BoxDesc(2, 1) : IBoxedLevel::BoxDesc(1);
			chunks[0].numboxes++;
		}

	LPRENDERSHADER sdrboxes2;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("K:\\Resources\\Shader\\boxed_level.fx"), &sdrboxes2));
	CHKRESULT(d3dwnd->CreateBoxedLevel(chunksize, numchunks, texboxes, sdrboxes2, &blevel));
	blevel->SetBoxTypes(boxtypes, 3);
	d3dwnd->RegisterForRendering(blevel, RT_DEFAULT);

	blevel->AssignChunk(&chunks[0]);*/


	/*// Init GuiFactory
	CHKALLOC(gui = CreateGuiFactory()); gui->Sync(GLOBALVAR_LIST);
	CHKRESULT(gui->Init());

	// Create output window
	LPGUIOUTPUTWINDOW wnd;
	//CHKRESULT(gui->CreateOutputWindow(fms->GetHwnd(0), &wnd));
	CHKRESULT(gui->CreateOutputWindow(fms->GetHwnd(0), d3dwnd->GetDevice(), &wnd));

LPGUIBRUSH s0, s1;
CHKRESULT(wnd->CreateSolidBrush(0.290f, 0.494f, 0.733f, 1.0f, &s0));
CHKRESULT(wnd->CreateSolidBrush(0.733f, 0.290f, 0.290f, 1.0f, &s1));

LPGUITEXTFORMAT txtfmt;
CHKRESULT(gui->CreateTextFormat("Verdana", 15, false, &txtfmt));
wnd->DrawTextField(txtfmt, "this is a test", s0, 100.0f, 100.0f, 200.0f, 100.0f);*/

	return R_OK;
}

Result __stdcall PostInit()
{
	return R_OK;
}

int frame = 0;
Quaternion basecamrot;
void __stdcall OnSpace(int key, LPVOID user)
{
	frame++;
	d3dwnd->TakeScreenShot(FilePath(String("screeny") << String(frame) << String(".png")), D3DX11_IFF_PNG);
}

void __stdcall Cyclic()
{
	Result rlt;

	if(dip)
		dip->Update();

	/*Quaternion camrot, deltacamrot;
	cameranode->GetOrientation(&camrot, ISceneNode::TS_WORLD);
	QuaternionRotationY(&deltacamrot, eng->time.dt / 10.0f);
	camrot = camrot * deltacamrot;
	cameranode->SetOrientation(&camrot, ISceneNode::TS_WORLD);*/

if(frame == 0)
	cameranode->GetOrientation(&basecamrot, ISceneNode::TS_WORLD);

if(frame == 7)
{
	fms->CloseAll();
	return;
}
Quaternion camrot;
QuaternionRotationY(&camrot, (float)frame * 2.0f * PI / 8.0f);
cameranode->SetOrientation(&(basecamrot * camrot), ISceneNode::TS_WORLD);

	/*if(hvk)
		hvk->Update();*/

	/*Sprite sprt;
	sprt.tex = tex;
	for(int x = 0; x < 20; x++)
	{
		MatrixTranslation(&sprt.worldmatrix, &Vector3((float)x / 10.0f - 1.0f, 0.0f, 0.0f));
		d3dwnd->DrawSprite(&sprt);
	}
	sprt.tex = tex2;
	MatrixTranslation(&sprt.worldmatrix, &Vector3(0.5f, 0.5f, 0.0f));
	sprite->DrawSprite(&sprt);*/

	/*Quaternion qrot;
	QuaternionRotationX(&qrot, eng->time.dt);
	QuaternionMultiply(&objBox->qrot, &objBox->qrot, &qrot);*/

	/*LPD3DFONT font;
	//d3dwnd->CreateD3dFont(FontType("Consolas", 15, true), &font);
	d3dwnd->CreateD3dFont(FontType("Calibri", 27, false), &font);
	sprite->SetIdentityViewTransform();
	sprite->SetIdentityProjectionTransform();
	//font->DrawTextDelayed(sprite, "This is a test\ttab\t\tdoubletab\nlinefeed", Rect<float>(20.0f, 10.0f, 100.0f, 200.0f), 0xFF0080FF);
	//font->DrawTextDelayed(sprite, "nfowd8Z)ZH)(&(/H&E$%&/G<\n>UIh87tg(G%6e453%&%%&&ftg<\n>vhKOJJI....,pjopJMjmogz8iiugi8g9", Rect<float>(20.0f, 10.0f, 100.0f, 200.0f), 0xFF0080FF);
	//font->DrawTextDelayed(sprite, "a§b", Rect<float>(20.0f, 10.0f, 100.0f, 200.0f), 0xFF0080FF);
font->DrawTextDelayed(sprite, "\
for()\n\
{\n\
333	foo\n\
	int abc = 3;\n\
		bla?\n\
1	23	bla?\n\
}", Rect<float>(20.0f, 10.0f, 1000.0f, 1000.0f), 0xFFFFFFFF, 0);

font->DrawTextDelayed(sprite, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris viverra hendrerit orci, a viverra velit. Donec et consequat ipsum, sed euismod magna. Phasellus mattis, velit sit amet ultricies vestibulum, nibh arcu porttitor enim, nec scelerisque arcu nisi non mi. Mauris eleifend sem id nisi condimentum, nec ultricies magna porta. Aenean mollis libero quam. Nam nec eleifend tellus. Mauris fringilla enim sed dui ultricies consequat. Quisque molestie orci ac augue egestas lacinia. Nunc eget molestie elit, id lobortis lectus.",
					  Rect<float>(50.0f, 50.0f, 400.0f, 200.0f), 0xFF000000, ID3dFont::DF_SINGLELINE | ID3dFont::DF_BREAKWORDS);*/

	if(d3d)
		d3d->Render();
}
CYCLICFUNC cyclic = Cyclic;

void __stdcall End()
{
	RELEASE(d3d);
	//RELEASE(gui);
	RELEASE(cle);
	RELEASE(fms);
}