#define ISTYPES_NO_POINT
#include "Pbrt.h"
#ifdef _DEBUG
#include <vld.h>
#endif

#ifdef NO_D3D_INTEROP
Result Pbrt::Render(IOutputWindow* wnd)
{
	return ERR("ISPbrt.dll was build without Direct3D interop support");
}
#else

#include <ISImage.h>

using namespace std;
#define PBRT_PROBES_NONE
//#define PBRT_HAS_OPENEXR
#include <core\geometry.h>
#include <core\api.h>
#include <core\paramset.h>
#include <core\primitive.h>//DELETE
#include <textures\imagemap.h>

//#include <ISMath.h>
namespace D3D
{
	#undef ISTYPES_NO_POINT
	#undef __ISTYPES_H
	#define PARENT_NAMESPACE D3D
	class IOutputWindow;
	#include <ISDirect3D.h>
}

Image* TextureToImage(D3D::LPOUTPUTWINDOW wnd, D3D::LPTEXTURE tex)
{
	Image* img;
	D3D::LPTEXTURE localtex;
	Result rlt;

	IFFAILED(wnd->CreateTexture(tex, D3D::ITexture::TU_ALLACCESS, false, &localtex))
	{
		LOG("Error acquiring Direct3D texture. Ignoring texture");
		return NULL;
	}
	else
	{
		UINT texstride;
		img = new Image();
		img->width = localtex->GetWidth();
		img->height = localtex->GetHeight();
		localtex->MapPixelData(true, false, &texstride, (LPVOID*)&img->data);
		img->bpp = texstride / localtex->GetWidth();
		img->width = localtex->GetWidth();
		localtex->UnmapData();
		return img;
	}
}

/*#include <set>
std::set<D3D::ITexture*> textures;
void DefineMaterial(D3D::IOutputWindow* wnd, D3D::D3DMaterial& mat) //DEPRECATED
{
	ParamSet params;

	if(mat.tex)
	{
		const std::string texture_id(String((int)mat.tex));
		if(textures.find(mat.tex) == textures.end())
		{
			Image* img = TextureToImage(wnd, mat.tex); //EDIT: Free image after rendering
			char buf[64];
			std::string imgdesc = "|";
			imgdesc += itoa(img->width, buf, 10);
			imgdesc += "|";
			imgdesc += itoa(img->height, buf, 10);
			imgdesc += "|";
			imgdesc += itoa(img->bpp, buf, 10);
			imgdesc += "|";
			imgdesc += itoa((int)img->data, buf, 10);
			imgdesc += "|";
			imgdesc += ".***";
			params.AddString("filename", &imgdesc, 1);
			pbrtTexture(texture_id, "color", "imagemap", params);
			params.Clear();
			textures.insert(mat.tex);
		}
		params.AddTexture("Kd", texture_id);
	}
	else
		params.AddRGBSpectrum("Kd", (float*)mat.diffuse, 3);
	params.AddRGBSpectrum("Ks", (float*)mat.specular, 3);
float roughness = 0.000001f;
params.AddFloat("Ks", &roughness, 1);
	pbrtMaterial("plastic", params);
}*/

/*Result __stdcall AddRenderableToScene(D3D::IRenderable* renderable, LPVOID user) //DEPRECATED
{
	D3D::IOutputWindow* wnd = (D3D::IOutputWindow*)user;

	if(D3D::LPOBJECT obj = renderable->GetObject())
	{
		// Define shape from LPOBJECT

		ParamSet params;

		pbrtAttributeBegin();

		switch(obj->orientmode)
		{
		case D3D::OM_EULER:
			pbrtTranslate(obj->pos.x, obj->pos.y, obj->pos.z);
			pbrtRotate(obj->rot.z * 180.0f / PI, 0.0f, 0.0f, 1.0f);
			pbrtRotate(-obj->rot.y * 180.0f / PI, 0.0f, 1.0f, 0.0f);
			pbrtRotate(obj->rot.x * 180.0f / PI, 1.0f, 0.0f, 0.0f);
			pbrtScale(obj->scl.x, obj->scl.y, obj->scl.z);
			break;

		case D3D::OM_QUAT:
{
static D3D::Vector3 oldpos = obj->pos;
static D3D::Quaternion oldqrot = obj->qrot;
pbrtActiveTransformStartTime();
pbrtTranslate(oldpos.x, oldpos.y, oldpos.z);
pbrtRotate(oldqrot.w * 180.0f / PI, oldqrot.x, oldqrot.y, oldqrot.z);
pbrtScale(obj->scl.x, obj->scl.y, obj->scl.z);
oldpos = obj->pos;
oldqrot = obj->qrot;
pbrtActiveTransformEndTime();
pbrtTranslate(obj->pos.x, obj->pos.y, obj->pos.z);
pbrtRotate(obj->qrot.w * 180.0f / PI, obj->qrot.x, obj->qrot.y, obj->qrot.z);
pbrtScale(obj->scl.x, obj->scl.y, obj->scl.z);
pbrtActiveTransformAll();
}


//			pbrtTranslate(obj->pos.x, obj->pos.y, obj->pos.z);
//			pbrtRotate(obj->qrot.w * 180.0f / PI, obj->qrot.x, obj->qrot.y, obj->qrot.z);
//			pbrtScale(obj->scl.x, obj->scl.y, obj->scl.z);
			break;

		case D3D::OM_MATRIX:
			pbrtTransform((float*)obj->transform);
			break;

		default:
			Result::PrintLogMessage("IObject::orientmode not supported for PBRT interop");
		}

		D3D::IObject::Vertex* vertices;
		UINT numvertices = obj->GetVertexCount();
		Point* pos = new Point[numvertices]; //EDIT: Free this buffer after rendering
		Normal* nml = new Normal[numvertices]; //EDIT: Free this buffer after rendering
		float* uv = new float[2 * numvertices]; //EDIT: Free this buffer after rendering
		obj->MapVertexData(&vertices);
		for(UINT i = 0; i < numvertices; i++)
		{
			pos[i] = Point(vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z);
			nml[i] = Normal(vertices[i].nml.x, vertices[i].nml.y, vertices[i].nml.z);
			uv[2 * i + 0] = vertices[i].texcoord.x;
			uv[2 * i + 1] = vertices[i].texcoord.y;
		}
		params.AddPoint("P", pos, numvertices);
		params.AddNormal("N", nml, numvertices);
		params.AddFloat("uv", uv, 2 * numvertices);
		obj->UnmapData(vertices);

		UINT attributetablesize;
		obj->GetAttributeTable(NULL, &attributetablesize);
		UINT* indices;
		obj->MapIndexData(&indices);
		if(attributetablesize > 1)
		{
			D3D::IObject::AttributeRange* attributetable = new D3D::IObject::AttributeRange[attributetablesize];
			obj->GetAttributeTable(attributetable, &attributetablesize);
			for(UINT i = 0; i < attributetablesize; i++)
				for(UINT face = attributetable[i].facestart; face < attributetable[i].facestart + attributetable[i].facecount; face++)
				{
					params.AddInt("indices", (int*)(indices + 3 * attributetable[i].facestart), 3 * attributetable[i].facecount);
					DefineMaterial(wnd, (*obj->mats)[attributetable[i].attrid]);
					pbrtShape("trianglemesh", params);
				}
			delete[] attributetable;
		}
		else
		{
			params.AddInt("indices", (int*)indices, obj->GetIndexCount());
			DefineMaterial(wnd, (*obj->mats)[0]);
			pbrtShape("trianglemesh", params);
		}
		obj->UnmapData(indices);

		pbrtAttributeEnd();

//MessageBox(NULL, String((int)textures.size()), String((int)attributetablesize), 0);
	}

	return R_OK;
}*/

std::map<D3D::ITexture*, Texture<RGBSpectrum>*> textures;
Material* DefineMaterial(D3D::IOutputWindow* wnd, D3D::D3DMaterial& mat)
{
	/*if(mat.tex)
	{
		const std::string texture_id(String((int)mat.tex));
		if(textures.find(mat.tex) == textures.end())
		{
			Image* img = TextureToImage(wnd, mat.tex); //EDIT: Free image after rendering
			char buf[64];
			std::string imgdesc = "|";
			imgdesc += itoa(img->width, buf, 10);
			imgdesc += "|";
			imgdesc += itoa(img->height, buf, 10);
			imgdesc += "|";
			imgdesc += itoa(img->bpp, buf, 10);
			imgdesc += "|";
			imgdesc += itoa((int)img->data, buf, 10);
			imgdesc += "|";
			imgdesc += ".***";
			params.AddString("filename", &imgdesc, 1);
			pbrtTexture(texture_id, "color", "imagemap", params);
			params.Clear();
			textures.insert(mat.tex);
		}
		params.AddTexture("Kd", texture_id);
	}
	else
		params.AddRGBSpectrum("Kd", (float*)mat.diffuse, 3);
	params.AddRGBSpectrum("Ks", (float*)mat.specular, 3);
float roughness = 0.000001f;
params.AddFloat("Ks", &roughness, 1);
	pbrtMaterial("plastic", params);*/

	PlasticMaterialDesc materialdesc;
	ConstantSpectrumTextureDesc constspectrumdesc;

	if(mat.tex)
	{
		std::map<D3D::ITexture*, Texture<RGBSpectrum>*>::iterator pair = textures.find(mat.tex);
		if(pair == textures.end())
		{
			Image* img = TextureToImage(wnd, mat.tex); //EDIT: Free image after rendering
			char buf[64];
			std::string imgdesc = "|";
			imgdesc += itoa(img->width, buf, 10);
			imgdesc += "|";
			imgdesc += itoa(img->height, buf, 10);
			imgdesc += "|";
			imgdesc += itoa(img->bpp, buf, 10);
			imgdesc += "|";
			imgdesc += itoa((int)img->data, buf, 10);
			imgdesc += "|";
			imgdesc += ".***";

			ImageMapSpectrumTextureDesc texturedesc;
			texturedesc.filename = FilePath(imgdesc.c_str());
			textures[mat.tex] = materialdesc.Kd = texturedesc.MakeTexture();
		}
		else
			materialdesc.Kd = pair->second;
	}
	else
	{
		memcpy(constspectrumdesc.value, mat.diffuse, 3 * sizeof(float));
		materialdesc.Kd = constspectrumdesc.MakeTexture();
	}

	memcpy(constspectrumdesc.value, mat.specular, 3 * sizeof(float));
	materialdesc.Ks = constspectrumdesc.MakeTexture();

	ConstantFloatTextureDesc constfloatdesc;
	constfloatdesc.value = 1.0f / pow(mat.power, 1.3f); //EDIT: Approximisation
	materialdesc.roughness = constfloatdesc.MakeTexture();

	return materialdesc.MakeMaterial();
}

struct AddRenderableToSceneParams
{
	std::vector<Reference<Primitive>> primitives;
	D3D::IOutputWindow* wnd;
};
Result __stdcall AddRenderableToScene(D3D::IRenderable* renderable, LPVOID user)
{
	AddRenderableToSceneParams* arts_params = (AddRenderableToSceneParams*)user;

	if(D3D::LPOBJECT obj = renderable->GetObject())
	{
		// Define shape from LPOBJECT

		Transform transform;//(Inverse(*(Matrix4x4*)(float*)arts_params->wnd->GetCamera()->GetViewMatrix()));
		switch(obj->orientmode)
		{
		case D3D::OM_EULER:
			transform = Translate(Vector(obj->pos.x, obj->pos.y, obj->pos.z));
			transform = transform * Rotate( obj->rot.z * 180.0f / PI, Vector(0.0f, 0.0f, 1.0f));
			transform = transform * Rotate(-obj->rot.y * 180.0f / PI, Vector(0.0f, 1.0f, 0.0f));
			transform = transform * Rotate( obj->rot.x * 180.0f / PI, Vector(1.0f, 0.0f, 0.0f));
			transform = transform * Scale(obj->scl.x, obj->scl.y, obj->scl.z);
			break;

		case D3D::OM_QUAT:
/*{
static D3D::Vector3 oldpos = obj->pos;
static D3D::Quaternion oldqrot = obj->qrot;
pbrtActiveTransformStartTime();
pbrtTranslate(oldpos.x, oldpos.y, oldpos.z);
pbrtRotate(oldqrot.w * 180.0f / PI, oldqrot.x, oldqrot.y, oldqrot.z);
pbrtScale(obj->scl.x, obj->scl.y, obj->scl.z);
oldpos = obj->pos;
oldqrot = obj->qrot;
pbrtActiveTransformEndTime();
pbrtTranslate(obj->pos.x, obj->pos.y, obj->pos.z);
pbrtRotate(obj->qrot.w * 180.0f / PI, obj->qrot.x, obj->qrot.y, obj->qrot.z);
pbrtScale(obj->scl.x, obj->scl.y, obj->scl.z);
pbrtActiveTransformAll();
}*/

			transform = Translate(Vector(obj->pos.x, obj->pos.y, obj->pos.z));
			transform = transform * ((Quaternion*)(float*)obj->qrot)->ToTransform();
			transform = transform * Scale(obj->scl.x, obj->scl.y, obj->scl.z);
			break;

		case D3D::OM_MATRIX:
			transform = Transform(Inverse(*(Matrix4x4*)(float*)obj->transform));
			break;

		default:
			Result::PrintLogMessage("IObject::orientmode not supported for PBRT interop");
		}
		//transform = transform * Transform(Inverse(*(Matrix4x4*)(float*)arts_params->wnd->GetCamera()->GetViewMatrix()));

		for(int i = 0; i < obj->GetUnderlyingShapeCount(); ++i) //EDIT: obj->GetUnderlyingShapeCount() == 0 for objects created empty. Create a single TriangleMeshShapeDesc over the whole mesh in that case
		{
			switch(obj->GetUnderlyingShape(i)->GetId())
			{
			case D3D::DSID_SPHERE:
				{
					D3D::D3dSphereShapeDesc* desc = (D3D::D3dSphereShapeDesc*)obj->GetUnderlyingShape(i);
					SphereShapeDesc shapedesc;
					shapedesc.radius = desc->radius;
					shapedesc.phimax = desc->phi * 180.0f / PI;

					Reference<Shape> foo = shapedesc.MakeShape(transform);
					GeometricPrimitive* bar = new GeometricPrimitive(foo, DefineMaterial(arts_params->wnd, (*obj->mats)[0]), NULL);
					arts_params->primitives.push_back(bar);
				}
				break;

			default:
				{
					//EDIT: Only create TriangleMeshShapeDesc for the mesh-shape portion of obj

					TriangleMeshShapeDesc shapedesc;
					D3D::IObject::Vertex* vertices;
					shapedesc.numvertices = obj->GetVertexCount();
					Point* pos = new Point[shapedesc.numvertices]; 
					Normal* nml = new Normal[shapedesc.numvertices];
					float* uv = new float[2 * shapedesc.numvertices];
					obj->MapVertexData(&vertices);
					for(int i = 0; i < shapedesc.numvertices; i++)
					{
						pos[i] = Point(vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z);
						nml[i] = Normal(vertices[i].nml.x, vertices[i].nml.y, vertices[i].nml.z);
						uv[2 * i + 0] = vertices[i].texcoord.x;
						uv[2 * i + 1] = vertices[i].texcoord.y;
					}
					shapedesc.P = (float*)pos;
					shapedesc.N = (float*)nml;
					shapedesc.uv = (float*)uv;
					obj->UnmapData(vertices);

					UINT attributetablesize;
					obj->GetAttributeTable(NULL, &attributetablesize);
					UINT* indices;
					obj->MapIndexData(&indices);
					if(attributetablesize > 1)
					{
						D3D::IObject::AttributeRange* attributetable = new D3D::IObject::AttributeRange[attributetablesize];
						obj->GetAttributeTable(attributetable, &attributetablesize);
						for(UINT i = 0; i < attributetablesize; i++)
							for(UINT face = attributetable[i].facestart; face < attributetable[i].facestart + attributetable[i].facecount; face++)
							{
								shapedesc.numfaces = (int)attributetable[i].facecount;
								shapedesc.indices = (int*)(indices + 3 * attributetable[i].facestart);
								arts_params->primitives.push_back(new GeometricPrimitive(shapedesc.MakeShape(transform), DefineMaterial(arts_params->wnd, (*obj->mats)[attributetable[i].attrid]), NULL));
							}
						delete[] attributetable;
					}
					else
					{
						shapedesc.numfaces = (int)obj->GetFaceCount();
						shapedesc.indices = (int*)indices;
						Reference<Shape> foo = shapedesc.MakeShape(transform);
						GeometricPrimitive* bar = new GeometricPrimitive(foo, DefineMaterial(arts_params->wnd, (*obj->mats)[0]), NULL);
						arts_params->primitives.push_back(bar);
					}
					obj->UnmapData(indices);
					delete[] pos;
					delete[] nml;
					delete[] uv;
				}
				break;
			}
		}

//MessageBox(NULL, String((int)textures.size()), String((int)attributetablesize), 0);
	}

	return R_OK;
}

#include <core\scene.h>//DELETE

struct RenderThreadParams
{
	Pbrt* pbrt;
	Renderer* renderer;
	const Scene* scene;
	Transform* viewtransform;
	RenderThreadParams(Pbrt* pbrt, Renderer* renderer, const Scene* scene, Transform* viewtransform)
		: pbrt(pbrt), renderer(renderer), scene(scene), viewtransform(viewtransform) {}
	~RenderThreadParams()
	{
		pbrt->UnhookFilm();
		TasksCleanup();
		//transformCache.Clear();
		ImageTexture<float, float>::ClearCache();
		ImageTexture<RGBSpectrum, Spectrum>::ClearCache();
		delete renderer;
		delete scene;
		delete viewtransform;
	}
};
DWORD WINAPI RenderThread(LPVOID lpparams)
{
	RenderThreadParams* params = (RenderThreadParams*)lpparams;
	params->renderer->Render(params->scene);
	delete params;
	return 0;
}

Result Pbrt::Render(IOutputWindow* _wnd, const FilterDesc* filterdesc, const RendererDesc* rendererdesc)
{
printf("THIS IS A TEST\n");
fprintf(stdout, "FOO\n");

	D3D::IOutputWindow* wnd = (D3D::IOutputWindow*)_wnd;
	D3D::IDirect3D* d3d = (D3D::IDirect3D*)&*(::d3d);
	Result rlt;

	D3D::LPCAMERA cam = wnd->GetCamera();
	if(!cam)
		return ERR("No camera associated to output window");

static int frameidx = 0;
String filename = "frames\\";
filename <<= frameidx++;
filename <<= ".tga";
//textures.clear(); //DELETE

	/*BoxFilterDesc filterdesc;
	Filter* filter = filterdesc.MakeFilter();*/

	ImageFilmDesc filmdesc;
	filmdesc.filename = filename;
	filmdesc.xresolution = (int)wnd->GetBackbufferWidth();
	filmdesc.yresolution = (int)wnd->GetBackbufferHeight();
	Film* film = this->MakeFilm(&filmdesc, MakeFilter(filterdesc));

	PerspectiveCameraDesc cameradesc;
	cameradesc.fov = 90.0f;
	Transform* viewtransform = new Transform(Inverse(Transpose(*(Matrix4x4*)(float*)cam->GetViewMatrix())));
	Camera* camera = MakeCamera(&cameradesc, viewtransform, viewtransform, film);

	/*//MetropolisRendererDesc rendererdesc;
	SamplerRendererDesc rendererdesc;
	//rendererdesc.visualizeobjectids = true;
	LowDiscrepancySamplerDesc samplerdesc;
	samplerdesc.pixelsamples = 16;//1;
	rendererdesc.sampler = samplerdesc.MakeSampler(camera);
	//DirectLightingSurfaceIntegratorDesc surfaceintegratordesc;
	PathSurfaceIntegratorDesc surfaceintegratordesc;
	rendererdesc.surfaceintegrator = surfaceintegratordesc.MakeSurfaceIntegrator();
	EmissionVolumeIntegratorDesc volumeintegratordesc;
	rendererdesc.volumeintegrator = volumeintegratordesc.MakeVolumeIntegrator();*/
	Renderer* renderer = MakeRenderer(rendererdesc , camera);

	// Define light sources
	std::vector<Light*> lights;
	std::list<D3D::LPLIGHT> d3dlights;
	std::list<D3D::LPLIGHT>::const_iterator liter;
	d3d->GetEnabledLights(&d3dlights);
	LIST_FOREACH(d3dlights, liter)
	{
		D3D::LPLIGHT light = *liter;
		switch(light->Type)
		{
		case D3D::ILight::LT_DIRECTIONAL:
			break;

		case D3D::ILight::LT_POINT:
			{
				PointLightDesc pointlightdesc;
				pointlightdesc.I[0] = light->Color.r * 10.0f;
				pointlightdesc.I[1] = light->Color.g * 10.0f;
				pointlightdesc.I[2] = light->Color.b * 10.0f;
				//memcpy(pointlightdesc.I, light->Color, 3 * sizeof(float));
				memcpy(pointlightdesc.from, light->GetPos(), 3 * sizeof(float));
				lights.push_back(pointlightdesc.MakeLight(Matrix4x4::Matrix4x4()));
			}
			break;

		case D3D::ILight::LT_SPOT:
			break;
		}
	}

	// Define shapes
	AddRenderableToSceneParams arts_params;
	arts_params.wnd = wnd;
	CHKRESULT(wnd->GetRegisteredClasses(AddRenderableToScene, &arts_params, D3D::RT_DEFAULT));

	BvhAcceleratorDesc acceleratordesc;
	Primitive* accelerator = acceleratordesc.MakeAccelerator(arts_params.primitives);

	Scene* scene = new Scene(accelerator, lights, NULL);

	//renderer->Render(scene);
	CreateThread(NULL, 0, &RenderThread, new RenderThreadParams(this, renderer, scene, viewtransform), 0, NULL);

	return R_OK;
}

#endif