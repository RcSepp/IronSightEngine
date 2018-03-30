#ifndef __ISPBRT_H
#define __ISPBRT_H

#include <windows.h>
#include <ISTypes.h>
#include <ISInterfaces.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_PBRT
	#define PBRT_EXTERN_FUNC		__declspec(dllexport)
#else
	#define PBRT_EXTERN_FUNC		__declspec(dllimport)
#endif


class IOutputWindow;
class Transform; // Transform class from PBRT
class Light; // Light class from PBRT
class Shape; // Shape class from PBRT
class Material; // Material class from PBRT
template<typename T> class Texture; // Texture class from PBRT
class RGBSpectrum; // RGBSpectrum class from PBRT
class Primitive; // Primitive class from PBRT
template<typename T> class Reference; // Reference class from PBRT


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum PbrtFilterId {
	PFID_BOX
};
enum PbrtFilmId {
	PFID_IMAGE
};
enum PbrtCameraId {
	PCID_PERSPECTIVE
};
enum PbrtSamplerId {
	PSID_LOWDISCREPANCY
};
enum PbrtSurfaceIntegratorId {
	PSIID_DIRECTLIGHTING, PSIID_PATH
};
enum PbrtVolumeIntegratorId {
	PVIID_EMISSION
};
enum PbrtRendererId {
	PRID_METROPOLIS, PRID_SAMPLER
};
enum PbrtLightId {
	PLID_POINT
};
enum PbrtShapeId {
	PSID_TRIANGLEMESH, PSID_SPHERE
};
enum PbrtMaterialId {
	PMID_PLASTIC
};
enum PbrtTextureId {
	PTID_CONSTANT, PTID_IMAGEMAP
};
enum PbrtAcceleratorId {
	PAID_BVH
};

struct FilterDesc
{
protected:
	FilterDesc(PbrtFilterId id) : id(id) {}
public:
	const PbrtFilterId id;
};
struct BoxFilterDesc : public FilterDesc
{
	BoxFilterDesc() : FilterDesc(PFID_BOX), xwidth(0.5f), ywidth(0.5f) {}
	float xwidth, ywidth;
};

struct FilmDesc
{
protected:
	FilmDesc(PbrtFilmId id) : id(id) {}
public:
	const PbrtFilmId id;
};
struct ImageFilmDesc : public FilmDesc
{
	ImageFilmDesc() : FilmDesc(PFID_IMAGE), xresolution(640), yresolution(480), display(false), filename("")
	{
		cropwindow[0] = cropwindow[2] = 0.0f;
		cropwindow[1] = cropwindow[3] = 1.0f;
	}
	FilePath filename;
	int xresolution, yresolution;
	bool display;
	float cropwindow[4];
};

struct CameraDesc
{
protected:
	CameraDesc(PbrtCameraId id) : id(id), shutteropen(0.0f), shutterclose(1.0f), frameaspectratio(0.0f)
	{
		screenwindow[0] = screenwindow[2] = screenwindow[1] = screenwindow[3] = 0.0f;
	}
public:
	const PbrtCameraId id;
	float shutteropen, shutterclose, frameaspectratio;
	float screenwindow[4];
};
struct PerspectiveCameraDesc : public CameraDesc
{
	PerspectiveCameraDesc() : CameraDesc(PCID_PERSPECTIVE), lensradius(0.0f), focaldistance(1e30f), fov(90.0f) {}
	float lensradius, focaldistance, fov;
};

struct SamplerDesc
{
protected:
	SamplerDesc(PbrtSamplerId id) : id(id) {}
public:
	const PbrtSamplerId id;
};
struct LowDiscrepancySamplerDesc : public SamplerDesc
{
	LowDiscrepancySamplerDesc() : SamplerDesc(PSID_LOWDISCREPANCY), pixelsamples(4) {}
	int pixelsamples;
};

struct SurfaceIntegratorDesc
{
protected:
	SurfaceIntegratorDesc(PbrtSurfaceIntegratorId id) : id(id) {}
public:
	const PbrtSurfaceIntegratorId id;
};
struct DirectLightingSurfaceIntegratorDesc : public SurfaceIntegratorDesc
{
	DirectLightingSurfaceIntegratorDesc() : SurfaceIntegratorDesc(PSIID_DIRECTLIGHTING), maxdepth(5), strategy(S_ALL) {}
	int maxdepth;
	enum Strategy {
		S_ALL, S_ONE
	} strategy;
};
struct PathSurfaceIntegratorDesc : public SurfaceIntegratorDesc
{
	PathSurfaceIntegratorDesc() : SurfaceIntegratorDesc(PSIID_PATH), maxdepth(5) {}
	int maxdepth;
};

struct VolumeIntegratorDesc
{
protected:
	VolumeIntegratorDesc(PbrtVolumeIntegratorId id) : id(id), stepsize(1.0f) {}
public:
	const PbrtVolumeIntegratorId id;
	float stepsize;
};
struct EmissionVolumeIntegratorDesc : public VolumeIntegratorDesc
{
	EmissionVolumeIntegratorDesc() : VolumeIntegratorDesc(PVIID_EMISSION) {}
};

struct RendererDesc
{
protected:
	RendererDesc(PbrtRendererId id) : id(id) {}
public:
	const PbrtRendererId id;
};
struct MetropolisRendererDesc : public RendererDesc
{
	MetropolisRendererDesc() : RendererDesc(PRID_METROPOLIS), largestepprobability(0.25f), samplesperpixel(100), bootstrapsamples(100000), directsamples(4), dodirectseparately(true), maxconsecutiverejects(512), maxdepth(7), bidirectional(true) {}
	float largestepprobability;
	int samplesperpixel, bootstrapsamples, directsamples, maxconsecutiverejects, maxdepth;
	bool dodirectseparately, bidirectional;
};
struct SamplerRendererDesc : public RendererDesc
{
	SamplerRendererDesc() : RendererDesc(PRID_SAMPLER), sampler(NULL), surfaceintegrator(NULL), volumeintegrator(NULL), visualizeobjectids(false) {}
	SamplerDesc* sampler;
	SurfaceIntegratorDesc* surfaceintegrator;
	VolumeIntegratorDesc* volumeintegrator;
	bool visualizeobjectids;
};

struct LightDesc
{
protected:
	const PbrtLightId id;
	LightDesc(PbrtLightId id) : id(id)
	{
		scale[0] = scale[1] = scale[2] = 1.0f;
	}
public:
	float scale[3];

	virtual Light* MakeLight(const Transform& transform) const = 0;
};
struct PointLightDesc : public LightDesc
{
	PointLightDesc() : LightDesc(PLID_POINT)
	{
		I[0] = I[1] = I[2] = 1.0f;
		from[0] = from[1] = from[2] = 0.0f;
	}
	float I[3], from[3];

	Light* MakeLight(const Transform& transform) const;
};

struct ShapeDesc
{
protected:
	const PbrtShapeId id;
	ShapeDesc(PbrtShapeId id) : id(id) {}
public:
	virtual Shape* MakeShape(const Transform& transform) const = 0;
};
struct TriangleMeshShapeDesc : public ShapeDesc
{
	TriangleMeshShapeDesc() : ShapeDesc(PSID_TRIANGLEMESH), indices(NULL), P(NULL), N(NULL), S(NULL), uv(NULL), numvertices(0), numfaces(0) {}
	int* indices;
	float *P, *N, *S, *uv;
	int numvertices, numfaces;

	Shape* MakeShape(const Transform& transform) const;
};
struct SphereShapeDesc : public ShapeDesc
{
	SphereShapeDesc() : ShapeDesc(PSID_SPHERE), radius(1.0f), zmin(-1e20f), zmax(1e20f), phimax(360.0f) {}
	float radius, zmin, zmax, phimax;

	Shape* MakeShape(const Transform& transform) const;
};

struct MaterialDesc
{
protected:
	const PbrtMaterialId id;
	MaterialDesc(PbrtMaterialId id) : id(id) {}
public:
	virtual Material* MakeMaterial() const = 0;
};
struct PlasticMaterialDesc : public MaterialDesc
{
	PlasticMaterialDesc() : MaterialDesc(PMID_PLASTIC), Kd(NULL), Ks(NULL), roughness(NULL), bumpMap(NULL) {}
	Texture<RGBSpectrum> *Kd, *Ks;
	Texture<float> *roughness, *bumpMap;

	Material* MakeMaterial() const;
};

struct SpectrumTexture
{
protected:
	const PbrtTextureId id;
	SpectrumTexture(PbrtTextureId id) : id(id) {}
public:
	virtual Texture<RGBSpectrum>* MakeTexture() const = 0;
};
struct ConstantSpectrumTextureDesc : public SpectrumTexture
{
	ConstantSpectrumTextureDesc() : SpectrumTexture(PTID_CONSTANT)
	{
		value[0] = value[1] = value[2] = 1.0f;
	}
	float value[3];

	Texture<RGBSpectrum>* MakeTexture() const;
};
struct ImageMapSpectrumTextureDesc : public SpectrumTexture
{
	ImageMapSpectrumTextureDesc() : SpectrumTexture(PTID_IMAGEMAP), filename("") {}
	FilePath filename;

	Texture<RGBSpectrum>* MakeTexture() const;
};

struct FloatTexture
{
protected:
	const PbrtTextureId id;
	FloatTexture(PbrtTextureId id) : id(id) {}
public:
	virtual Texture<float>* MakeTexture() const = 0;
};
struct ConstantFloatTextureDesc : public FloatTexture
{
	ConstantFloatTextureDesc() : FloatTexture(PTID_CONSTANT), value(1.0f) {}
	float value;

	Texture<float>* MakeTexture() const;
};
struct ImageMapFloatTextureDesc : public FloatTexture
{
	ImageMapFloatTextureDesc() : FloatTexture(PTID_IMAGEMAP), filename("") {}
	FilePath filename;

	Texture<float>* MakeTexture() const;
};

struct AcceleratorDesc
{
protected:
	const PbrtAcceleratorId id;
	AcceleratorDesc(PbrtAcceleratorId id) : id(id) {}
public:
	virtual Primitive* MakeAccelerator(const std::vector<Reference<Primitive>>& primitives) const = 0;
};
struct BvhAcceleratorDesc : public AcceleratorDesc
{
	BvhAcceleratorDesc() : AcceleratorDesc(PAID_BVH) {}

	Primitive* MakeAccelerator(const std::vector<Reference<Primitive>>& primitives) const;
};


//-----------------------------------------------------------------------------
// Name: IPbrtRenderer
// Desc: Interface to the PbrtRenderer class
//-----------------------------------------------------------------------------
typedef class IPbrtRenderer
{
public:
	virtual void Release() = 0;
}* LPPBRTRENDERER;

//-----------------------------------------------------------------------------
// Name: IPbrt
// Desc: Interface to the Pbrt class
//-----------------------------------------------------------------------------
typedef class IPbrt
{
public:
	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	/*virtual Result CreateScene(ISPbrt::IScene** scene) = 0;
	virtual Result CreateScene(IOutputWindow* wnd, ISPbrt::IScene** scene) = 0;
	virtual Result CreateScene(const SceneDefinition* scenedef, const std::map<FilePath, MeshDefinition*>* meshdefs, std::map<FilePath, Image*>* textures, ISPbrt::IScene** scene) = 0;
	virtual Result CreateCamera(ISPbrt::ICamera** cam) = 0;
	virtual Result CreatePointLight(ISPbrt::IPointLight** light) = 0;
	virtual Result CreateDirLight(ISPbrt::IDirLight** light) = 0;
	virtual Result CreateSpotLight(ISPbrt::ISpotLight** light) = 0;
	virtual Result CreateSphereSurface(const Vector3* pos, float radius, ISPbrt::ISphereSurface** sfc) = 0;
	virtual Result CreateMeshSurface(ISPbrt::IMeshSurface** sfc) = 0;
	virtual Result CreateVMeshSurface(ISPbrt::IVMeshSurface** sfc) = 0;
	virtual Result CreateSolidMaterial(ISPbrt::IMaterial** mat) = 0;
	virtual void Render(const ISPbrt::IScene* scene, const RayTracingOptions& options) = 0;
	virtual Result ParseXmlScene(FilePath filename, SceneDefinition** scenedef) const = 0;
	virtual Result ParseObjMesh(FilePath filename, MeshDefinition** meshdef) const = 0;
	virtual Result ComputeTangentsAndBinormals(FilePath filename, MeshDefinition* meshdef) const = 0;*/

	virtual Result CreateDirectRenderView(IOutputWindow* wnd) = 0;
	virtual void RemoveDirectRenderView() = 0;
	virtual void Render() = 0;
	virtual Result Render(IOutputWindow* wnd, const FilterDesc* filter, const RendererDesc* renderer) = 0;
	virtual void Release() = 0;
}* LPPBRT;

PBRT_EXTERN_FUNC void RedirectStdOut(const String& pipename);
extern "C" PBRT_EXTERN_FUNC LPPBRT __cdecl CreatePbrt();

#endif