#define ISTYPES_NO_POINT
#include "Pbrt.h"

using namespace std;
#define PBRT_PROBES_NONE
//#define PBRT_HAS_OPENEXR
#include <core\paramset.h>
#include <core\filter.h>
#include <filters\box.h>
#include <film\image.h>
#include <cameras\perspective.h>
#include <renderers\metropolis.h>
#include <renderers\samplerrenderer.h>
#include <samplers\lowdiscrepancy.h>
#include <integrators\directlighting.h>
#include <integrators\path.h>
#include <integrators\emission.h>
#include <lights\point.h>
#include <shapes\trianglemesh.h>
#include <shapes\sphere.h>
#include <materials\plastic.h>
#include <textures\constant.h>
#include <textures\imagemap.h>
#include <accelerators\bvh.h>


Filter* MakeFilter(const FilterDesc* desc)
{
	ParamSet params;
	switch(desc->id)
	{
	case PFID_BOX:
		{
			const BoxFilterDesc* bfdesc = (BoxFilterDesc*)desc;
			params.AddFloat("xwidth", &bfdesc->xwidth, 1);
			params.AddFloat("ywidth", &bfdesc->ywidth, 1);
			return (Filter*)CreateBoxFilter(params);
		}
	}
	return NULL;
}

Film* MakeFilm(const FilmDesc* desc, Filter* filter)
{
	ParamSet params;
	switch(desc->id)
	{
	case PFID_IMAGE:
		{
			const ImageFilmDesc* ifdesc = (ImageFilmDesc*)desc;
			params.AddString("filename", ifdesc->filename.IsEmpty() ? &std::string("") : &std::string(ifdesc->filename), 1);
			params.AddInt("xresolution", &ifdesc->xresolution, 1);
			params.AddInt("yresolution", &ifdesc->yresolution, 1);
			params.AddFloat("cropwindow", ifdesc->cropwindow, 4);
			return (Film*)CreateImageFilm(params, filter);
		}
	}
	return NULL;
}

Camera* MakeCamera(const CameraDesc* desc, const Transform* tstart, const Transform* tend, Film *film)
{
	ParamSet params;

	// Camera common parameters
	params.AddFloat("shutteropen", &desc->shutteropen, 1);
	params.AddFloat("shutterclose", &desc->shutterclose, 1);
	float aspectratio = desc->frameaspectratio == 0.0f ? (float)film->xResolution / (float)film->yResolution : desc->frameaspectratio;
	params.AddFloat("frameaspectratio", &aspectratio, 1);
	if(desc->screenwindow[0] == 0.0f && desc->screenwindow[1] == 0.0f && desc->screenwindow[2] == 0.0f && desc->screenwindow[3] == 0.0f)
	{
		float screen[4];
		if (aspectratio > 1.0f) {
			screen[0] = -aspectratio;
			screen[1] =  aspectratio;
			screen[2] = -1.0f;
			screen[3] =  1.0f;
		}
		else {
			screen[0] = -1.0f;
			screen[1] =  1.0f;
			screen[2] = -1.0f / aspectratio;
			screen[3] =  1.0f / aspectratio;
		}
		params.AddFloat("screenwindow", screen, 4);
	}
	else
		params.AddFloat("screenwindow", desc->screenwindow, 4);

	switch(desc->id)
	{
	case PCID_PERSPECTIVE:
		{
			const PerspectiveCameraDesc* pcdesc = (PerspectiveCameraDesc*)desc;
			params.AddFloat("lensradius", &pcdesc->lensradius, 1);
			params.AddFloat("focaldistance", &pcdesc->focaldistance, 1);
			params.AddFloat("fov", &pcdesc->fov, 1);
		}
		return (Camera*)CreatePerspectiveCamera(params, AnimatedTransform(tstart, 0.0f, tend, 1.0f), film);
	}
	return NULL;
}

Sampler* MakeSampler(const SamplerDesc* desc, const Camera* camera)
{
	ParamSet params;
	switch(desc->id)
	{
	case PSID_LOWDISCREPANCY:
		{
			const LowDiscrepancySamplerDesc* ldsdesc = (LowDiscrepancySamplerDesc*)desc;
			params.AddInt("pixelsamples", &ldsdesc->pixelsamples, 1);
			return CreateLowDiscrepancySampler(params, camera->film, camera);
		}
	}
	return NULL;
}

SurfaceIntegrator* MakeSurfaceIntegrator(const SurfaceIntegratorDesc* desc)
{
	ParamSet params;
	switch(desc->id)
	{
	case PSIID_DIRECTLIGHTING:
		{
			const DirectLightingSurfaceIntegratorDesc* dlsidesc = (DirectLightingSurfaceIntegratorDesc*)desc;
			params.AddInt("maxdepth", &dlsidesc->maxdepth, 1);
			switch(dlsidesc->strategy)
			{
			case DirectLightingSurfaceIntegratorDesc::S_ALL: params.AddString("strategy", &std::string("all"), 1); break;
			case DirectLightingSurfaceIntegratorDesc::S_ONE: params.AddString("strategy", &std::string("one"), 1); break;
			}
			return CreateDirectLightingIntegrator(params);
		}
	case PSIID_PATH:
		{
			const PathSurfaceIntegratorDesc* psidesc = (PathSurfaceIntegratorDesc*)desc;
			params.AddInt("maxdepth", &psidesc->maxdepth, 1);
			return CreatePathSurfaceIntegrator(params);
		}
	}
	return NULL;
}

VolumeIntegrator* MakeVolumeIntegrator(const VolumeIntegratorDesc* desc)
{
	ParamSet params;
	params.AddFloat("stepsize", &desc->stepsize, 1);
	switch(desc->id)
	{
	case PVIID_EMISSION:
		{
			return CreateEmissionVolumeIntegrator(params);
		}
	}
	return NULL;
}

Renderer* MakeRenderer(const RendererDesc* desc, Camera* camera)
{
	ParamSet params;
	switch(desc->id)
	{
	case PRID_METROPOLIS:
		{
			const MetropolisRendererDesc* mrdesc = (MetropolisRendererDesc*)desc;
			params.AddFloat("largestepprobability", &mrdesc->largestepprobability, 1);
			params.AddInt("samplesperpixel", &mrdesc->samplesperpixel, 1);
			params.AddInt("bootstrapsamples", &mrdesc->bootstrapsamples, 1);
			params.AddInt("directsamples", &mrdesc->directsamples, 1);
			params.AddBool("dodirectseparately", &mrdesc->dodirectseparately, 1);
			params.AddInt("maxconsecutiverejects", &mrdesc->maxconsecutiverejects, 1);
			params.AddInt("maxdepth", &mrdesc->maxdepth, 1);
			params.AddBool("bidirectional", &mrdesc->bidirectional, 1);
			return (Renderer*)CreateMetropolisRenderer(params, camera);
		}
	case PRID_SAMPLER:
		{
			SamplerRendererDesc* srdesc = (SamplerRendererDesc*)desc;
			return (Renderer*)new SamplerRenderer(MakeSampler(srdesc->sampler, camera), camera, MakeSurfaceIntegrator(srdesc->surfaceintegrator), MakeVolumeIntegrator(srdesc->volumeintegrator), srdesc->visualizeobjectids);
		}
	}
	return NULL;
}

Light* PointLightDesc::MakeLight(const Transform& transform) const
{
	ParamSet params;
	params.AddRGBSpectrum("scale", scale, 3);
	params.AddRGBSpectrum("I", I, 3);
	params.AddPoint("from", (Point*)from, 1);
	return (Light*)CreatePointLight(transform, params);
}

Shape* TriangleMeshShapeDesc::MakeShape(const Transform& transform) const
{
	ParamSet params;
	params.AddInt("indices", indices, 3 * numfaces);
	params.AddPoint("P", (Point*)P, numvertices);
	if(N) params.AddNormal("N", (Normal*)N, numvertices);
	if(S) params.AddVector("S", (Vector*)S, numvertices);
	if(uv) params.AddFloat("uv", uv, 2 * numvertices);
	return (Shape*)CreateTriangleMeshShape(new Transform(transform), new Transform(Inverse(transform)), false, params);
}
Shape* SphereShapeDesc::MakeShape(const Transform& transform) const
{
	ParamSet params;
	params.AddFloat("radius", &radius, 1);
	float _zmin = min(radius, max(-radius, zmin)); params.AddFloat("zmin", &_zmin, 1);
	float _zmax = min(radius, max(-radius, zmax)); params.AddFloat("zmax", &_zmax, 1);
	params.AddFloat("phimax", &phimax, 1);
	return (Shape*)CreateSphereShape(new Transform(transform), new Transform(Inverse(transform)), false, params);
}

Material* PlasticMaterialDesc::MakeMaterial() const
{
	/*ParamSet params;
	//EDIT
	ParamSet matparams;
	//EDIT
float diffuse[] = {1.0f, 0.0f, 1.0f};
float specular[] = {1.0f, 1.0f, 1.0f};
matparams.AddRGBSpectrum("Kd", (float*)diffuse, 3);
matparams.AddRGBSpectrum("Ks", (float*)specular, 3);
float roughness = 0.1f;
matparams.AddFloat("roughness", &roughness, 1);

map<string, Reference<Texture<float>>> floatTextures;
map<string, Reference<Texture<Spectrum>>> spectrumTextures;

	TextureParams mp(params, matparams, floatTextures, spectrumTextures);
	//EDIT
	return (Material*)CreatePlasticMaterial(Transform(), mp);*/

	return new PlasticMaterial(Kd ? Kd : new ConstantTexture<Spectrum>(Spectrum(0.25f)),
							   Ks ? Ks : new ConstantTexture<Spectrum>(Spectrum(0.25f)),
							   roughness ? roughness : new ConstantTexture<float>(0.25f),
							   bumpMap ? bumpMap : new ConstantTexture<float>(0.25f));
}

Texture<Spectrum>* ConstantSpectrumTextureDesc::MakeTexture() const
{
	return new ConstantTexture<Spectrum>(Spectrum::FromRGB(value));
}
Texture<Spectrum>* ImageMapSpectrumTextureDesc::MakeTexture() const
{
	return new ImageTexture<RGBSpectrum, Spectrum>(new UVMapping2D(1.0f, 1.0f, 0.0f, 0.0f), std::string(filename), false, 8.0f, TEXTURE_REPEAT, 1.0f, 1.0f);
}

Texture<float>* ConstantFloatTextureDesc::MakeTexture() const
{
	return new ConstantTexture<float>(value);
}
Texture<float>* ImageMapFloatTextureDesc::MakeTexture() const
{
	return new ImageTexture<float, float>(new UVMapping2D(1.0f, 1.0f, 0.0f, 0.0f), std::string(filename), false, 8.0f, TEXTURE_REPEAT, 1.0f, 1.0f);
}

Primitive* BvhAcceleratorDesc::MakeAccelerator(const std::vector<Reference<Primitive>>& primitives) const
{
	ParamSet params;
	//EDIT
	return CreateBVHAccelerator(primitives, params);
}