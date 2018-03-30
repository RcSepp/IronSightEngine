#include "main.h"
#include <ISRayTracer.h>
//#include <ISDirect3D.h>
#include <ISSceneDefinition.h>
#include <ISMeshDefinition.h>
#include <ISRaySpace.h>
#include <ISImage.h>

#ifdef _DEBUG
	#pragma comment (lib, "ISEngine_d.lib")
	#pragma comment (lib, "ISForms_d.lib")
	#pragma comment (lib, "ISMath_d.lib")
	#pragma comment (lib, "ISImage_d.lib")
	#pragma comment (lib, "ISRayTracer_d.lib")
#else
	#pragma comment (lib, "ISEngine.lib")
	#pragma comment (lib, "ISForms.lib")
	#pragma comment (lib, "ISMath.lib")
	#pragma comment (lib, "ISImage.lib")
	#pragma comment (lib, "ISRayTracer.lib")
#endif

/*// Direct 3D variables
LPOUTPUTWINDOW d3dwnd;*/

// Raytracer variables
//using namespace ISRayTracer;
//Image* outputimage = NULL;
LPWINFORM rtrform;
ISRayTracer::LPSCENE scene;
Image* outputimage;
void OnFrameDone(int frame, int numframes);
void OnSetPixel(const Point<>& pos, unsigned long color);

Result ParseMeshes(const SceneDefinition* scenedef, FilePath rootpath, std::map<FilePath, MeshDefinition*>& meshdefs);
Result ParseTextures(const SceneDefinition* scenedef, const std::map<FilePath, MeshDefinition*>& meshdefs, FilePath rootpath, std::map<FilePath, Image*>& textures);


Result __stdcall Init()
{
	Result rlt;

	// >>> Init Forms

	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create forms
	//LPWINFORM d3dform;
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(256, 256);
	frmsettings.caption = "D3D";
	frmsettings.windowpos = Point<int>(100, 100);
	//CHKRESULT(fms->CreateForm(&frmsettings, NULL, &d3dform));

	frmsettings.caption = "RayTracer";
	frmsettings.windowpos = Point<int>(612, 100);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &rtrform));

	/*// >>> Init Direct3D

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
	//d3dwnd->EnableSprites("K:\\Resources\\Shader\\Common\\Sprite.fx");
	//d3dwnd->EnableLineDrawing("K:\\Resources\\Shader\\Common\\Line.fx");
	d3dwnd->MakeConsoleTarget(11, 11);
	d3dwnd->ShowFps();
	//d3dwnd->backcolor = Color(0.0f, 0.0f, 0.0f);

	// Create perspective view
	LPCAMERA d3dcam;
	CHKRESULT(d3d->CreateCamera(90.0f * PI / 180.0f, 0.001f, 200.0f, &d3dcam));
	Vec3Set(&d3dcam->pos, 0.0f, 0.0f, 3.0f);
	d3dcam->rot.y = PI;
	d3dwnd->SetCamera(d3dcam);

	// Create sphere
	LPRENDERSHADER sdrDefault;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("K:\\Resources\\Shader\\Common\\Default.fx"), &sdrDefault));
	LPOBJECT objSphere;
	D3dSphereShapeDesc spheredesc;
	Vec3Set(&spheredesc.pos, 0.0f, 0.0f, 0.0f);
	spheredesc.radius = 1.0f;
	spheredesc.slices = spheredesc.stacks = 32;
	d3dwnd->CreateObject(spheredesc, &objSphere);
	d3dwnd->RegisterForRendering(objSphere, RT_DEFAULT);
	objSphere->SetShader(sdrDefault);
	Vec3Set(&objSphere->pos, 0.0f, 0.0f, -1.0f);
	objSphere->mats[0].ambient = Color(0.17f * 0.3f, 0.18f * 0.3f, 0.50f * 0.3f);
	objSphere->mats[0].diffuse = Color(0.17f * 0.9f, 0.18f * 0.9f, 0.50f * 0.9f);
	objSphere->mats[0].specular = Color(0.17f * 1.0f, 0.18f * 1.0f, 0.50f * 1.0f);
	objSphere->mats[0].power = 200.0f;

	// Create teapot
	LPRENDERSHADER sdrDefault;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("K:\\Resources\\Shader\\Common\\Default.fx"), &sdrDefault));
	LPOBJECT objTeapot;
	CHKRESULT(d3dwnd->CreateObject(FilePath("Debug_teapot.x"), false, &objTeapot));
	d3dwnd->RegisterForRendering(objTeapot, RT_DEFAULT);
	objTeapot->SetShader(sdrDefault);
	Vec3Set(&objTeapot->pos, 0.0f, 0.0f, -2.0f);
QuaternionRotationX(&objTeapot->qrot, PI / 4.0f);
objTeapot->orientmode = OM_QUAT;
	objTeapot->mats[0].ambient = Color(0.17f * 0.3f, 0.18f * 0.3f, 0.50f * 0.3f);
	objTeapot->mats[0].diffuse = Color(0.17f * 0.9f, 0.18f * 0.9f, 0.50f * 0.9f);
	objTeapot->mats[0].specular = Color(0.17f * 1.0f, 0.18f * 1.0f, 0.50f * 1.0f);
	objTeapot->mats[0].power = 200.0f;

	// Create lights
	LPLIGHT d3dpointlight;
	CHKRESULT(d3d->CreateLight(ILight::LT_POINT, &d3dpointlight));
	d3dpointlight->SetPos(Vector3( 1.0f, 1.0f, 1.0f));
	CHKRESULT(d3d->CreateLight(ILight::LT_POINT, &d3dpointlight));
	d3dpointlight->SetPos(Vector3(-1.0f, 1.0f, 1.0f));*/

	// >>> Init RayTracer

	CHKALLOC(rtr = CreateRayTracer()); rtr->Sync(GLOBALVAR_LIST);

	//CHKRESULT(rtr->CreateScene(d3dwnd, &scene));

	SceneDefinition* scenedef;
	CHKRESULT(rtr->ParseXmlScene("K:\\RayTracer\\scenes\\transparency2d.xml", &scenedef));
	std::map<FilePath, MeshDefinition*> meshdefs;
	CHKRESULT(ParseMeshes(scenedef, "K:\\RayTracer\\scenes\\", meshdefs));
	std::map<FilePath, Image*> textures;
	CHKRESULT(ParseTextures(scenedef, meshdefs, "K:\\RayTracer\\scenes\\", textures));
	CHKRESULT(rtr->CreateScene(scenedef, &meshdefs, &textures, &scene));

	//scene->Camera->size = Size<>(256, 256);
	//scene->Camera->region = Rect<>(0, 0, scene->Camera->size.width, scene->Camera->size.height);
	rtrform->SetClientSize(scene->Camera->size);
	outputimage = Image::New(scene->Camera->size.width, scene->Camera->size.height, 3);

	/*ISRayTracer::LPCAMERA cam;
	CHKRESULT(rtr->CreateCamera(&cam));
	scene->Camera = cam;*/

	/*ISRayTracer::LPSPHERESURFACE sphere;
	CHKRESULT(rtr->CreateSphereSurface(&Vector3(0.0f, 0.0f, -1.0f), 1.0f, &sphere));
	scene->AddSurface(sphere);*/

	/*ISRayTracer::LPMESHSURFACE mesh;
	CHKRESULT(rtr->CreateMeshSurface(&mesh));
	scene->AddSurface(mesh);

	UINT* indices;
	objTeapot->MapIndexData(&indices);
	mesh->SetVertexPositionIndices(indices, sizeof(UINT));
	mesh->SetVertexNormalIndices(indices, sizeof(UINT));
	mesh->SetVertexTexcoordIndices(indices, sizeof(UINT));
	objTeapot->UnmapData(indices);

	IObject::Vertex* vertices;
	objTeapot->MapVertexData(&vertices);
	mesh->SetVertexPositions(&vertices->pos, sizeof(IObject::Vertex));
	mesh->SetVertexNormals(&vertices->nml, sizeof(IObject::Vertex));
	mesh->SetVertexTexcoords(&vertices->texcoord, sizeof(IObject::Vertex));
	objTeapot->UnmapData(vertices);
	mesh->numfaces = objTeapot->GetFaceCount();
mesh->CreateOctree();*/

	/*ISRayTracer::LPPOINTLIGHT pointlight;
	CHKRESULT(rtr->CreatePointLight(&pointlight));
	Vec3Set(&pointlight->pos, 0.0f, 1.0f, 1.0f);
	scene->AddLight(pointlight);*/

	/*ISRayTracer::LPMATERIAL spherematerial;
	CHKRESULT(rtr->CreateSolidMaterial(&spherematerial));
	spherematerial->reflectance = 0.0f;
	spherematerial->transmittance = 0.0f;
	spherematerial->absorbance = 1.0f;
	spherematerial->refraction = 2.3f;
	spherematerial->ambientclr =  Color(0.17f * 0.3f, 0.18f * 0.3f, 0.50f * 0.3f);
	spherematerial->diffuseclr =  Color(0.17f * 0.9f, 0.18f * 0.9f, 0.50f * 0.9f);
	spherematerial->specularclr = Color(0.17f * 1.0f, 0.18f * 1.0f, 0.50f * 1.0f);
	spherematerial->specularpwr = 200.0f;
	//sphere->Material = spherematerial;
	mesh->Material = spherematerial;*/

	return R_OK;
}

Result __stdcall __PostInit()
{
	ISRayTracer::LPRAYSPACE rayspace1, rayspace2;
	scene->CreateRaySpace("10M_1.rayspace", &rayspace1);
	scene->CreateRaySpace("10M_2.rayspace", &rayspace2);
	//cle->PrintLine(rayspace1->Compare(rayspace2));
	//cle->PrintLine(rayspace2->Compare(rayspace1));

Image *img;
rayspace1->Draw2DRaySpace(32, 64, &img);
SaveImage("out.png", img);
/*SaveImage("10M_1.png", img);
delete img;
rayspace2->Draw2DRaySpace(32, 64, &img);
SaveImage("10M_2.png", img);*/
delete img;

	return R_OK;
}

Result __stdcall ___PostInit()
{
	/*if(d3d)
		d3d->Render();*/

	Result rlt;

	// Create comparison ray space
	ISRayTracer::LPRAYSPACE rayspace_comp;
	CHKRESULT(scene->CreateRaySpace(false, true, 32, 32, 32, 64, &rayspace_comp));//CHKRESULT(scene->CreateRaySpace("ref_groundtruth.rayspace", &rayspace_comp));

	// Create ray space
	ISRayTracer::LPRAYSPACE rayspace;
	CHKRESULT(scene->CreateRaySpace(false, true, 32, 32, 32, 64, &rayspace));

	//cle->PrintLine(rayspace->Compare(rayspace_comp));

	// Sample lattice
	RaySpaceSamplingOptions rssoptions;
	//rssoptions.castbackwards = false; // Cast FROM random entries
	//rssoptions.constraints = RaySpaceSamplingOptions::REC_EMMISSIVE_SFC; // Random entries belong to emissive surfaces
	//rayspace->SampleLattice(100000, rssoptions);
	rssoptions.castbackwards = true; // Cast TO random entries
	rssoptions.constraints = RaySpaceSamplingOptions::REC_NONE; // Random entries belong to any surface

//	for(int i = 0; i < 10; i++)
//		rayspace->SampleLattice(1000000, rssoptions);
	rayspace_comp->SampleLattice(10000000, rssoptions);

rayspace_comp->Save("test.rayspace");
rayspace_comp->Release();
CHKRESULT(scene->CreateRaySpace("test.rayspace", &rayspace_comp));

	cle->PrintLine(rayspace->Compare(rayspace_comp));
	cle->PrintLine(rayspace_comp->Compare(rayspace));

Image *img;
CHKRESULT(rayspace->Draw2DRaySpace(32, 64, &img));
CHKRESULT(SaveImage("rayspace.png", img));
delete img;
CHKRESULT(rayspace_comp->Draw2DRaySpace(32, 64, &img));
CHKRESULT(SaveImage("rayspace_comp.png", img));
delete img;

	/*for(int i = 0; i < 10; i++)
	{
		rayspace->SampleLattice(1000, rssoptions);
		cle->PrintLine(rayspace->Compare(rayspace_comp));
	}*/

	/*Image *img, *splitimg;
	CHKRESULT(rayspace->Draw2DRaySpace(32, 128, &img, &splitimg));
	CHKRESULT(SaveImage("lattice.png", img));
	delete img;
	CHKRESULT(SaveImage("lattice_splits.png", splitimg));
	delete splitimg;*/

	//CHKRESULT(rayspace->Save("ref_groundtruth.rayspace"));

	/*// Perform ray-tracing
	RayTracingOptions options;
options.technique = RayTracingOptions::RTT_VDA_GI;//RTT_WHITTED_RT;//RTT_SHOW_NORMALS;//cmdline.technique;
options.rayspace = rayspace;
//options.quietmodeenabled = cmdline.quietmodeenabled;
//options.usethinrefractions = cmdline.usethinrefractions;
//options.batchviewrays = cmdline.batchviewrays;
options.supersamplingfactor = 1;//4;//cmdline.supersamplingfactor;
//options.texsampling = cmdline.texsampling;
//options.startframe = cmdline.startframe;
options.onsetpixel = OnSetPixel;
options.onframedone = OnFrameDone;
	rtr->Render(scene, options);*/

	return R_OK;
}

int SplitCondition_MAXDEPTH, SplitCondition_MINRAYCOUNT;
float SplitCondition_THRESHOLD;
bool SplitCondition(const Color3* oldlum, const Color3* newlum, int oldnumrays, int splitdepth)
{
	if(splitdepth >= SplitCondition_MAXDEPTH || oldnumrays < SplitCondition_MINRAYCOUNT)
		return false;

	auto Luma = [](const Color3* clr) -> float {return 0.3f * clr->r + 0.59f * clr->g + 0.11f * clr->b;};
	float oldluma = Luma(oldlum);
	float newluma = Luma(newlum);

	return abs(oldluma - newluma) >= SplitCondition_THRESHOLD;
}
Result __stdcall PostInit()
{
	Result rlt;

	// Create ray space
	ISRayTracer::LPRAYSPACE rayspace;
	//CHKRESULT(scene->CreateRaySpace(true, false, 1, 1, 4, 8, &rayspace));
	//CHKRESULT(scene->CreateRaySpace(false, false, 8, 8, 8, 16, &rayspace));
CHKRESULT(scene->CreateRaySpace(false, true, 32, 1, 1, 128, &rayspace));

	// Sample lattice
	RaySpaceSamplingOptions rssoptions;
//	rssoptions.bfstracing = true;
	rssoptions.castbackwards = true;
//rssoptions.castbackwards = false;
//rssoptions.constraints = RaySpaceSamplingOptions::REC_EMMISSIVE_SFC;
	rssoptions.splitconditioncbk = SplitCondition;
	SplitCondition_THRESHOLD = 10.0f;
	SplitCondition_MINRAYCOUNT = 10;
	SplitCondition_MAXDEPTH = 4;

	rayspace->SampleLattice(1000000, rssoptions);

	Image *img;
	CHKRESULT(rayspace->Draw2DRaySpace(32, 128, &img));
	CHKRESULT(SaveImage("lattice.png", img));
	delete img;
	CHKRESULT(rayspace->DrawIntegratedRaySpace(32, 128, &img));
	CHKRESULT(SaveImage("lattice integrated.png", img));
	delete img;

	/*// Perform ray-tracing
	RayTracingOptions options;
	options.technique = RayTracingOptions::RTT_VDA_GI;
	options.rayspace = rayspace;
	options.supersamplingfactor = 1;
	options.onsetpixel = OnSetPixel;
	options.onframedone = OnFrameDone;
	rtr->Render(scene, options);*/

	return R_OK;
}

//#define USE_CYCLIC
#ifdef USE_CYCLIC
void __stdcall Cyclic()
{
	d3d->Render();
}
CYCLICFUNC cyclic = Cyclic;
#else
CYCLICFUNC cyclic = NULL;
#endif

FilePath outfilename = "out", outfilenameext = ".png";
void OnFrameDone(int frame, int numframes)
{
	// Save final image
	if(outputimage)
	{
		if(numframes == 1)
			SaveImage((FilePath)((String)outfilename << (String)outfilenameext), outputimage);
		else
		{
			String strframe = String(frame);
			int strframelen = String(frame).length(), targetstrframelen = String(numframes).length();
			for(int i = strframelen; i < targetstrframelen; i++)
				strframe = String('0') << strframe;
			SaveImage((FilePath)((String)outfilename << strframe << (String)outfilenameext), outputimage);
		}
	}
}

void OnSetPixel(const Point<>& pos, unsigned long color)
{
	unsigned char* colorptr = outputimage->data + ((pos.y * outputimage->width + pos.x) * outputimage->bpp);
	*colorptr = (unsigned char)(color >> 0);
	*(++colorptr) = (unsigned char)(color >> 8);
	*(++colorptr) = (unsigned char)(color >> 16);
	if(outputimage->bpp == 4)
		*(++colorptr) = 0xFF;

	HDC hdc = GetDC(rtrform->GetHwnd());
	SetPixel(hdc, pos.x, pos.y, color);
	ReleaseDC(rtrform->GetHwnd(), hdc);
}

Result ParseMeshes(const SceneDefinition* scenedef, FilePath rootpath, std::map<FilePath, MeshDefinition*>& meshdefs)
{
	MeshDefinition* meshdef;
	Result rlt;

	std::list<SceneDefinition::Surface*>::const_iterator surfaceiter;
	LIST_FOREACH(scenedef->surfaces, surfaceiter)
	{
		if((*surfaceiter)->type == SceneDefinition::ST_MESH)
		{
			FilePath filename = ((SceneDefinition::Mesh*)*surfaceiter)->mesh_file;
			if(meshdefs.find(filename) == meshdefs.end())
			{
				CHKRESULT(rtr->ParseObjMesh((FilePath)(rootpath << filename), &meshdef));
				if((*surfaceiter)->material->type == SceneDefinition::MT_TEXTURED && !((SceneDefinition::TexturedMaterial*)(*surfaceiter)->material)->normalmap_file.IsEmpty())
					CHKRESULT(rtr->ComputeTangentsAndBinormals((FilePath)(rootpath << filename), meshdef));
				meshdefs[filename] = meshdef;
			}
		}
	}

	return R_OK;
}

Result ParseTextures(const SceneDefinition* scenedef, const std::map<FilePath, MeshDefinition*>& meshdefs, FilePath rootpath, std::map<FilePath, Image*>& textures)
{
	Image* texture;
	Result rlt;

	std::list<SceneDefinition::Surface*>::const_iterator surfaceiter;
	LIST_FOREACH(scenedef->surfaces, surfaceiter)
	{
		if((*surfaceiter)->material->type == SceneDefinition::MT_TEXTURED)
		{
			FilePath filename = ((SceneDefinition::TexturedMaterial*)((*surfaceiter)->material))->texture_file;
			if(textures.find(filename) == textures.end())
			{
				CHKRESULT(LoadImage((FilePath)(rootpath << filename), &texture));
				textures[filename] = texture;
			}
			filename = ((SceneDefinition::TexturedMaterial*)((*surfaceiter)->material))->normalmap_file;
			if(!filename.IsEmpty() && textures.find(filename) == textures.end())
			{
				CHKRESULT(LoadImage((FilePath)(rootpath << filename), &texture));
				textures[filename] = texture;
			}
		}
	}

	std::map<FilePath, MeshDefinition*>::const_iterator meshiter;
	LIST_FOREACH(meshdefs, meshiter)
	{
		std::vector<MeshDefinition::Surface*>::const_iterator meshsurfaceiter;
		LIST_FOREACH(meshiter->second->surfaces, meshsurfaceiter)
		{
			FilePath filename = (*meshsurfaceiter)->material.texfilename;
			if(!filename.IsEmpty() && textures.find(filename) == textures.end())
			{
				CHKRESULT(LoadImage((FilePath)(rootpath << filename), &texture));
				textures[filename] = texture;
			}
		}
	}

	return R_OK;
}

void __stdcall End()
{
	RELEASE(rtr);
	//RELEASE(d3d);
	RELEASE(cle);
	RELEASE(fms);
}