#include "Main.h"
#include <ISDirect3D.h>
#include <ISHavok.h>
#include <ISDirectInput.h>
#include <vld.h>
#include "RTScreenshotUtility.h"

#include <DxErr.h> //DELETE
#pragma comment (lib, "DxErr.lib") //DELETE
#pragma comment (lib, "d3d10.lib") //DELETE
#pragma comment (lib, "dxgi.lib") //DELETE
#ifdef _DEBUG //DELETE
	#pragma comment (lib, "d3dx10d.lib") //DELETE
#else //DELETE
	#pragma comment (lib, "d3dx10.lib") //DELETE
#endif //DELETE



LPWINFORM form1 = NULL;
LPOUTPUTWINDOW d3dwnd = NULL;
LPCAMERA cam;
LPHUD hud = NULL;
LPBOXEDLEVEL blevel = NULL;
LPBOXEDLEVELENTITY pblevel = NULL;

const UINT chunksize[] = {11, 11, 11};
BoxedLevelChunk* chunks = NULL;
const UINT numchunks = 2;

LPVEHICLE vehicle = NULL;

void __stdcall OnSpace(int key, LPVOID user);

/*void __stdcall OnPythonException(const FilePath& scriptname, int linenumber, PythonDictionary& globals, PythonDictionary& locals, const String& extype, const String& exstring, LPVOID user)
{
	MessageBox(NULL, exstring, scriptname << String(" @ line ") << String(linenumber), 0);
}*/

void __stdcall OnHudEvent1(IHUD::LPELEMENT element)
{
	cle->PrintD3D("OnHudEvent1()");
}
void __stdcall OnHudEvent2(IHUD::LPELEMENT element)
{
	cle->PrintD3D("OnHudEvent2()");
}

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
	wndsettings.enablemultisampling = true;
	wndsettings.screenidx = 0;
	CHKRESULT(d3d->CreateOutputWindow(&wndsettings, &d3dwnd));
	dip->AddKeyHandler(Key::F12, RTScreenshotUtility::OnKeyTakeScreenshot, d3dwnd);

	d3dwnd->EnableSprites("K:\\Resources\\Shader\\Common\\Sprite.fx");
	d3dwnd->EnableLineDrawing("K:\\Resources\\Shader\\Common\\Line.fx");
	d3dwnd->MakeConsoleTarget(11, 11);
	d3dwnd->ShowFps();

	cle->PrintD3D(123);

CHKRESULT(d3d->CreateCamera(90.0f * PI / 180.0f, 0.001f, 200.0f, &cam));
cam->pos = Vector3(7.0f, 2.0f, 2.0f);
cam->rot.y = -0.25f * PI;
cam->rot.x = -0.25f * PI;
	cam->viewmode = ICamera::VM_FLY;
	d3dwnd->SetCamera(cam);
	d3dwnd->RegisterForUpdating(cam);

	/*LPTEXTURE tex, tex2;
	CHKRESULT(d3dwnd->CreateTexture(FilePath("Pause.png"), ITexture::TU_SKIN_NOMIPMAP, true, &tex));
	CHKRESULT(d3dwnd->CreateTexture(FilePath("heightmap_64x64.png"), ITexture::TU_SKIN_NOMIPMAP, true, &tex2));
	LPD3DFONT hudfont;
	CHKRESULT(d3dwnd->CreateD3dFont(FontType("Verdana", 18.0f), &hudfont));
	CHKRESULT(d3dwnd->CreateHUD(&hud));
	IHUD::LPELEMENT e1, e2;
	CHKRESULT(hud->CreateElement("abc", hudfont, 100, 0, DT_TOPCENTER, 0xFFFFFFFF, Size<int>(0, 0), 1, 0, (IHUD::LPTEXTELEMENT*)&e1));
	CHKRESULT(hud->CreateElement(tex2, 120, 0, DT_TOPLEFT, 0xFFFFFFFF, Rect<int>(25, 25, 50, 50), 0, (IHUD::LPIMAGEELEMENT*)&e2));
	hud->RegisterMouseUpListener(e1, OnHudEvent1);
	hud->RegisterMouseUpListener(e2, OnHudEvent2);

	d3dwnd->RegisterForRendering(hud, RT_FOREGROUND);*/

	/*// Create cursor
	LPD3DCURSOR cursor;
	CHKRESULT(d3dwnd->CreateCursor(&cursor));
	CHKRESULT(d3dwnd->CreateTexture("K:\\Resources\\Images\\Icons\\cursor_ani.png", ITexture::TU_SKIN_NOMIPMAP, false, &cursor->tex));
	cursor->animated = true;
	cursor->hotspot.x = 5;
	cursor->hotspot.y = 4;
	d3dwnd->RegisterForRendering(cursor, RT_FOREGROUND);*/

	// Create light
/*LPLIGHT light;
CHKRESULT(d3d->CreateLight(ILight::LT_SPOT, &light));
light->SetPos(Vector3(100.0f, 100.0f, 100.0f));*/
	LPLIGHT sunlight;
	CHKRESULT(d3d->CreateLight(ILight::LT_DIRECTIONAL, &sunlight));
	sunlight->Dir = Vector3(0.0f, -0.5f, -1.0f);


	// Create player
	LPOBJECT objPlayer;
	LPRENDERSHADER sdrBlend;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("shader withblending.fx"), &sdrBlend));
	CHKRESULT(d3dwnd->CreateObject(FilePath("human_withbones.x"), true, &objPlayer));
	objPlayer->pos = Vector3(10.0f, 5.0f, 10.0f);
	d3dwnd->RegisterForRendering(objPlayer, RT_DEFAULT);
	objPlayer->SetShader(sdrBlend);


LPOBJECT objBox;
LPRENDERSHADER sdrDefault;
CHKRESULT(d3dwnd->CreateRenderShader(FilePath("shader.fx"), &sdrDefault));
CHKRESULT(d3dwnd->CreateObject(FilePath("K:\\Resources\\Objects\\cube.x"), true, &objBox));
objBox->pos = Vector3(10.0f, 5.0f, 10.0f);
objBox->scl = Vector3(0.01f, 0.01f, 0.01f);
d3dwnd->RegisterForRendering(objBox, RT_DEFAULT);
objBox->SetShader(sdrDefault);

	// Create chunks
	CHKALLOC(chunks = new BoxedLevelChunk[numchunks]);
	for(UINT i = 0; i < numchunks; i++)
	{
		chunks[i].boxmask = new BoxedLevelBoxDesc[chunksize[0] * chunksize[1] * chunksize[2]];
		ZeroMemory(chunks[i].boxmask, chunksize[0] * chunksize[1] * chunksize[2] * sizeof(BoxedLevelBoxDesc));
		chunks[i].numboxes = 0;
		chunks[i].chunkpos[0] = 0;
		chunks[i].chunkpos[1] = 0;
		chunks[i].chunkpos[2] = i;
	}

	// Create boxed level
	LPTEXTURE texboxes;
	CHKRESULT(d3dwnd->CreateTexture("MincraftDefaultTexture.png", ITexture::TU_SKIN, false, &texboxes));

	BoxedLevelBoxType* boxtypes = new BoxedLevelBoxType[3];
	boxtypes[0].SetTextureIndices(3);
	boxtypes[0].textureindex[4] = 146;
	boxtypes[0].shape = SHAPE_CUBIC;

	boxtypes[1].SetTextureIndices(3);
	boxtypes[1].textureindex[4] = 146;
	boxtypes[1].shape = SHAPE_STAIRS;

	boxtypes[2].SetTextureIndices(8);
	boxtypes[2].textureindex[4] = 10;
	boxtypes[2].shape = SHAPE_CUBIC;

	for(int z = 0; z <= 10; z++)
		for(int x = 0; x <= 10; x++)
		{
			chunks[0].boxmask[x * (chunksize[1] * chunksize[2]) + z * chunksize[1] + 0] = BoxedLevelBoxDesc(0);
			chunks[0].numboxes++;
		}
	for(int x = 0; x <= 5; x++)
		for(int y = 1; y <= x; y++)
		{
			chunks[0].boxmask[x * (chunksize[1] * chunksize[2]) + 5 * chunksize[1] + y] = x == y ? BoxedLevelBoxDesc(1, 1) : BoxedLevelBoxDesc(0);
			chunks[0].numboxes++;
		}

	//for(int z = 0; z <= 10; z++)
	//	for(int x = 0; x <= 10; x++)
	//	{
	//		chunks[1].boxmask[x * (chunksize[1] * chunksize[2]) + z * chunksize[1] + y] = BoxedLevelBoxDesc(3);
	//		chunks[1].numboxes++;
	//	}
memcpy(chunks[1].boxmask, chunks[0].boxmask, chunksize[0] * chunksize[1] * chunksize[2] * sizeof(BoxedLevelBoxDesc));
chunks[1].numboxes = chunks[0].numboxes;

	LPRENDERSHADER sdrboxes2;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("K:\\Resources\\Shader\\boxed_level.fx"), &sdrboxes2));
	CHKRESULT(d3dwnd->CreateBoxedLevel(chunksize, numchunks, texboxes, sdrboxes2, &blevel));
	blevel->SetBoxTypes(boxtypes, 3);
	d3dwnd->RegisterForRendering(blevel, RT_DEFAULT);

	blevel->AssignChunk(&chunks[0]);


	// Init Havok
Vector3 worldsize_min = Vector3(-0.5f, -0.5f, -0.5f);
Vector3 worldsize_max = Vector3(100 * chunksize[0] - 0.5f, 10 * chunksize[1] - 0.5f, 100 * chunksize[2] - 0.5f);
Vector3 worldsize_min_withtolerance = worldsize_min - Vector3(100.0f, 100.0f, 100.0f);//Vector3(0.5f, 0.5f, 0.5f);
Vector3 worldsize_max_withtolerance = worldsize_max + Vector3(100.0f, 100.0f, 100.0f);//Vector3(0.5f, 0.5f, 0.5f);
	CHKALLOC(hvk = CreateHavok()); hvk->Sync(GLOBALVAR_LIST);
	CHKRESULT(hvk->Init(worldsize_min_withtolerance, worldsize_max_withtolerance, VT_VISUALDEBUGGER, NULL, NULL));

	// Create floor
	LPREGULARENTITY entityFloor;
	HvkPlaneShapeDesc planedesc;
	planedesc.mtype = MT_FIXED;
	planedesc.vhalfext = (worldsize_max - worldsize_min) / 2.0f;
	planedesc.vcenter = worldsize_min + planedesc.vhalfext;
	planedesc.vcenter.y = -0.5f;
	planedesc.vdir = Vector3(0.0f, 1.0f, 0.0f);
	CHKRESULT(hvk->CreateRegularEntity(&planedesc, IHavok::LAYER_LANDSCAPE, &Vector3(0.0f, 0.0f, 0.0f), &Quaternion(0.0f, 0.0f, 0.0f, 1.0f), &entityFloor));

	// Create player
	LPRAGDOLLENTITY entity1_ragdoll;
	CHKRESULT(objPlayer->CreateCollisionRagdoll(0.3f, true, &entity1_ragdoll));
	cam->SetPlayerModel(objPlayer, entity1_ragdoll);
	cam->viewmode = ICamera::VM_THIRDPERSON;


	// >>> Create physics shapes for each box type shape

	HvkBoxShapeDesc bdesc;
	bdesc.size = Vector3(1.0f, 1.0f, 1.0f);
	bdesc.mtype = MT_FIXED;
	bdesc.qual = CQ_FIXED;

	const Vector3 stairvertices[] = {
		Vector3(-0.5f, -0.5f, -0.5f), Vector3( 0.5f, -0.5f, -0.5f), Vector3(-0.5f, -0.5f,  0.5f), Vector3( 0.5f, -0.5f,  0.5f), // Bottom 4 vertices
		Vector3(-0.5f,  0.0f, -0.5f), Vector3( 0.5f,  0.0f, -0.5f), Vector3(-0.5f,  0.0f,  0.0f), Vector3( 0.5f,  0.0f,  0.0f), // Center 4 vertices
		Vector3(-0.5f,  0.5f,  0.0f), Vector3( 0.5f,  0.5f,  0.0f), Vector3(-0.5f,  0.5f,  0.5f), Vector3( 0.5f,  0.5f,  0.5f), // Top 4 vertices
	};
	const UINT stairfaces[] = {
		4, 5, 0, 0, 5, 1, // Lower front quat
		8, 9, 6, 6, 9, 7, // Upper front quat
		11, 10, 3, 3, 10, 2, // Back quat
		2, 10, 8, 2, 8, 6, 2, 6, 4, 2, 4, 0, // Left fan
		3, 9, 11, 3, 7, 9, 3, 5, 7, 3, 1, 5, // Right fan
		6, 7, 4, 4, 7, 5, // Front top quat
		10, 11, 8, 8, 11, 9, // Back top quat
		0, 1, 2, 2, 1, 3 // Bottom quat
	};
	HvkCompressedMeshShapeDesc cmdesc;
	cmdesc.mtype = MT_FIXED;
	cmdesc.qual = CQ_FIXED;
	cmdesc.vertexdata = (BYTE*)stairvertices;
	cmdesc.numvertices = ARRAYSIZE(stairvertices);
	cmdesc.fvfstride = sizeof(Vector3);
	cmdesc.indices = (UINT*)stairfaces;
	cmdesc.numfaces = ARRAYSIZE(stairfaces) / 3;

	HvkShapeDesc** shapes = (HvkShapeDesc**)new LPVOID[2];
	shapes[0] = &bdesc;
	shapes[1] = &cmdesc;

	// Create boxed level
	CHKRESULT(hvk->CreateBoxedLevelEntity(chunksize, &pblevel));
	pblevel->SetBoxTypes(boxtypes, 3);
	pblevel->CreateBoxShapes(shapes, 2);
	delete[] shapes;

	pblevel->AssignChunk((BoxedLevelChunk*)&chunks[0]);

	// >>> Create vehicle

	LPOBJECT objVehicle;
	CHKRESULT(d3dwnd->CreateObject(FilePath("Lamborghini.x"), false, &objVehicle));
	objVehicle->SetShader(sdrDefault);
	objVehicle->pos = Vector3(4.0f, 5.0f, 8.0f);
	objVehicle->scl = Vector3(0.001f, 0.001f, 0.001f);
	d3dwnd->RegisterForRendering(objVehicle, RT_DEFAULT);

IObject::Vertex* vertices;

CHKRESULT(objVehicle->MapVertexData(&vertices));

HvkConvexVerticesShapeDesc desc;
desc.mtype = MT_DYNAMIC;
desc.mass = 5.0f;
desc.numvertices = objVehicle->GetVertexCount();

objVehicle->orientmode = OM_QUAT;
float* scaledvertices;
CHKALLOC(scaledvertices = new float[desc.numvertices * 3]);
for(int i = 0; i < desc.numvertices; i++)
{
	scaledvertices[3 * i + 0] = vertices[i].pos.x * objVehicle->scl.x;
	scaledvertices[3 * i + 1] = vertices[i].pos.y * objVehicle->scl.y;
	scaledvertices[3 * i + 2] = vertices[i].pos.z * objVehicle->scl.z;
}
desc.vertices = (BYTE*)scaledvertices;
desc.fvfstride = 3 * sizeof(float);
VehicleDesc vdesc;
vdesc.chassis = &desc;
vdesc.wheels.resize(4);
vdesc.wheels[0].SetDefault(0);
vdesc.wheels[1].SetDefault(0);
vdesc.wheels[2].SetDefault(1);
vdesc.wheels[3].SetDefault(1);
vdesc.engine.SetDefault();
CHKRESULT(hvk->CreateVehicle(&vdesc, &objVehicle->pos, &objVehicle->qrot, &vehicle));
delete[] scaledvertices;
CHKRESULT(objVehicle->UnmapData(vertices));



	// Finalize Havok initialization
	hvk->InitDone();
	return R_OK;
}

Result __stdcall PostInit()
{
	return R_OK;
}

INT16 round(float x)
{
	INT16 sign = x < 0 ? -1 : 1;
	x *= sign;
	float remainder = x - (float)(INT16)x;
	return sign * (remainder < 0.5f ? (INT16)x : (INT16)x + 1);
}
void DrawSelection(const Vector3& boxpos, const Color& color)
{
	d3dwnd->DrawLine(Vector3(boxpos.x - 0.5f, boxpos.y - 0.5f, boxpos.z - 0.5f), Vector3(boxpos.x + 0.5f, boxpos.y - 0.5f, boxpos.z - 0.5f), color, color);
	d3dwnd->DrawLine(Vector3(boxpos.x - 0.5f, boxpos.y + 0.5f, boxpos.z - 0.5f), Vector3(boxpos.x + 0.5f, boxpos.y + 0.5f, boxpos.z - 0.5f), color, color);
	d3dwnd->DrawLine(Vector3(boxpos.x - 0.5f, boxpos.y - 0.5f, boxpos.z - 0.5f), Vector3(boxpos.x - 0.5f, boxpos.y + 0.5f, boxpos.z - 0.5f), color, color);
	d3dwnd->DrawLine(Vector3(boxpos.x + 0.5f, boxpos.y - 0.5f, boxpos.z - 0.5f), Vector3(boxpos.x + 0.5f, boxpos.y + 0.5f, boxpos.z - 0.5f), color, color);

	d3dwnd->DrawLine(Vector3(boxpos.x - 0.5f, boxpos.y - 0.5f, boxpos.z + 0.5f), Vector3(boxpos.x + 0.5f, boxpos.y - 0.5f, boxpos.z + 0.5f), color, color);
	d3dwnd->DrawLine(Vector3(boxpos.x - 0.5f, boxpos.y + 0.5f, boxpos.z + 0.5f), Vector3(boxpos.x + 0.5f, boxpos.y + 0.5f, boxpos.z + 0.5f), color, color);
	d3dwnd->DrawLine(Vector3(boxpos.x - 0.5f, boxpos.y - 0.5f, boxpos.z + 0.5f), Vector3(boxpos.x - 0.5f, boxpos.y + 0.5f, boxpos.z + 0.5f), color, color);
	d3dwnd->DrawLine(Vector3(boxpos.x + 0.5f, boxpos.y - 0.5f, boxpos.z + 0.5f), Vector3(boxpos.x + 0.5f, boxpos.y + 0.5f, boxpos.z + 0.5f), color, color);

	d3dwnd->DrawLine(Vector3(boxpos.x - 0.5f, boxpos.y - 0.5f, boxpos.z - 0.5f), Vector3(boxpos.x - 0.5f, boxpos.y - 0.5f, boxpos.z + 0.5f), color, color);
	d3dwnd->DrawLine(Vector3(boxpos.x + 0.5f, boxpos.y - 0.5f, boxpos.z - 0.5f), Vector3(boxpos.x + 0.5f, boxpos.y - 0.5f, boxpos.z + 0.5f), color, color);
	d3dwnd->DrawLine(Vector3(boxpos.x - 0.5f, boxpos.y + 0.5f, boxpos.z - 0.5f), Vector3(boxpos.x - 0.5f, boxpos.y + 0.5f, boxpos.z + 0.5f), color, color);
	d3dwnd->DrawLine(Vector3(boxpos.x + 0.5f, boxpos.y + 0.5f, boxpos.z - 0.5f), Vector3(boxpos.x + 0.5f, boxpos.y + 0.5f, boxpos.z + 0.5f), color, color);
}
float CastRay(const Vector3& vpos, const Vector3& vdir, UINT *boxpos)
{
	LPENTITY hitentity;
	UINT32 shapekeys[8];
	float dist = hvk->CastRayDir(vpos, vdir, &hitentity, &shapekeys);
	for(UINT chunkidx = 0; chunkidx < numchunks; chunkidx++)
		if(hitentity == pblevel)
		{
			if(boxpos)
			{
				BoxedLevelChunk* hitchunk = pblevel->ChunkFromShapeKey(shapekeys[0]);
				pblevel->BoxPosFromShapeKey(shapekeys[1], (UINT(*)[3])boxpos);
				boxpos[0] += hitchunk->chunkpos[0] * chunksize[0];
				boxpos[1] += hitchunk->chunkpos[1] * chunksize[1];
				boxpos[2] += hitchunk->chunkpos[2] * chunksize[2];
				//cle->PrintD3D(String("HIT: [") << String((int)boxpos[0]) << String(", ") << String((int)boxpos[1]) << String(", ") << String((int)boxpos[2]) << String("]"));
			}
			return dist;
		}
	return 1e20f;
}
void AddBox(const UINT (&pos)[3], BoxedLevelBoxDesc boxdesc)
{
BoxedLevelChunk& chunk = chunks[0];

	UINT boxpos[] = {pos[0] % chunksize[0], pos[1] % chunksize[1], pos[2] % chunksize[2]};
	UINT boxidx = boxpos[2] * chunksize[0] * chunksize[1] + boxpos[1] * chunksize[0] + boxpos[0];
	if(chunk.boxmask[boxidx] != boxdesc)
	{
		chunk.boxmask[boxidx] = boxdesc; // Update boxmask
		chunk.numboxes++; // Update numboxes
		blevel->UpdateChunk(&chunk, boxpos); // Sync changed boxmask with GPU
	}
}

void RemoveBox(const UINT (&pos)[3])
{
BoxedLevelChunk& chunk = chunks[0];

	UINT boxpos[] = {pos[0] % chunksize[0], pos[1] % chunksize[1], pos[2] % chunksize[2]};
	UINT boxidx = boxpos[2] * chunksize[0] * chunksize[1] + boxpos[1] * chunksize[0] + boxpos[0];
	if(chunk.boxmask[boxidx] != 0)
	{
		chunk.boxmask[boxidx] = 0; // Update boxmask
		chunk.numboxes--; // Update numboxes
		blevel->UpdateChunk(&chunk, boxpos); // Sync changed boxmask with GPU
	}
}

Vector3 a(0.0f, 1.0f, 0.0f);
void __stdcall Cyclic()
{
	Result rlt;

	if(dip)
		dip->Update();

	if(hud)
		hud->Update();

	// Update physics
	if(hvk)
	{
		vehicle->acceleration = 0.5f;

		static int framecounter = 0;
		if(framecounter < 20)
			framecounter++;
		else
			hvk->Update();
	}

	if(d3d)
		d3d->Render();

Vector3 b(0.0f, 10.0f, 0.0f);
d3dwnd->DrawLine(a, b, Color(0xFF0000FF), Color(0xFF0000FF));


/*static BYTE oldmousebuttons[8] = {0};

	POINT mousepos;
	GetCursorPos(&mousepos);
	ScreenToClient(fms->GetHwnd(0), &mousepos);
	Vector3 vdir;
	d3dwnd->GetCamera()->ComputePixelDir(Point<int>(mousepos.x, mousepos.y), &vdir);
	UINT hitboxpos[3];
	float dist = CastRay(cam->pos, vdir, hitboxpos);
	if(dist < 1e20f)
	{
Vector3 hitpos = cam->pos + vdir * dist;
//cle->PrintD3D(String("HIT: [") << String(hitpos.x - (float)hitboxpos[0]) << String(", ") << String(hitpos.y - (float)hitboxpos[1]) << String(", ") << String(hitpos.z - (float)hitboxpos[2]) << String("]"));
Vector3 hitboxdelta(hitpos.x - (float)hitboxpos[0], hitpos.y - (float)hitboxpos[1], hitpos.z - (float)hitboxpos[2]);
Vector3 abshitboxdelta(abs(hitboxdelta.x), abs(hitboxdelta.y), abs(hitboxdelta.z));

bool hitboxadd;
if(hitboxdelta.x > abshitboxdelta.y + abshitboxdelta.z)
	{hitboxpos[0]++; hitboxadd = true;}
else if(hitboxdelta.x < -abshitboxdelta.y - abshitboxdelta.z)
	{hitboxpos[0]--; hitboxadd = true;}
else if(hitboxdelta.y > abshitboxdelta.x + abshitboxdelta.z)
	{hitboxpos[1]++; hitboxadd = true;}
else if(hitboxdelta.y < -abshitboxdelta.x - abshitboxdelta.z)
	{hitboxpos[1]--; hitboxadd = true;}
else if(hitboxdelta.z > abshitboxdelta.x + abshitboxdelta.y)
	{hitboxpos[2]++; hitboxadd = true;}
else if(hitboxdelta.z < -abshitboxdelta.x - abshitboxdelta.y)
	{hitboxpos[2]--; hitboxadd = true;}
else
	hitboxadd = false;

DrawSelection(Vector3((float)hitboxpos[0], (float)hitboxpos[1], (float)hitboxpos[2]), hitboxadd ? Color(0xFF0000FF) : Color(0xFFFF0000));

		Vector3 vpos = cam->pos + vdir * (dist - 0.01f);

		INT16 x, y, z;
		Vector3 boxpos;
		x = round(vpos.x), y = round(vpos.y), z = round(vpos.z);
		boxpos = Vector3((float)x, (float)y, (float)z);

if(dip->mouse.rgbButtons[0] && dip->mouse.rgbButtons[0] != oldmousebuttons[0])
{
	if(hitboxadd)
		AddBox(hitboxpos, BoxedLevelBoxDesc(1));
	else
		RemoveBox(hitboxpos);
}
	}

memcpy(oldmousebuttons, dip->mouse.rgbButtons, ARRAYSIZE(oldmousebuttons));*/
}
CYCLICFUNC cyclic = Cyclic;

void __stdcall OnSpace(int key, LPVOID user)
{
	if(numchunks > 1)
	{
		blevel->AssignChunk(&chunks[1]);
		pblevel->AssignChunk((BoxedLevelChunk*)&chunks[1]);
	}

	/*POINT mousepos;
	GetCursorPos(&mousepos);
	ScreenToClient(fms->GetHwnd(0), &mousepos);
	Vector3 vdir;
	d3dwnd->GetCamera()->ComputePixelDir(Point<int>(mousepos.x, mousepos.y), &vdir);
	Vector3 vpos = d3dwnd->GetCamera()->pos;
	float dist = hvk->CastRayDir(vpos, vdir);
	if(dist < 1e20f)
		a = cam->pos + vdir * dist;*/
}

void __stdcall End()
{
	RELEASE(d3d);
	RELEASE(dip);
	RELEASE(hvk);
	RELEASE(cle);
	RELEASE(fms);

	if(chunks)
	{
		for(UINT i = 0; i < numchunks; i++)
			delete[] chunks[i].boxmask;
		delete[] chunks;
	}
}