#ifndef __ISRAYTRACER_H
#define __ISRAYTRACER_H

#include <ISEngine.h>
#include <ISMath.h>
#include <ISImage.h>

class IOutputWindow;
struct SceneDefinition;
struct MeshDefinition;
namespace ISRayTracer
{
	class IRaySpace;
}


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_RAYTRACER
	#define RAYTRACER_EXTERN_FUNC		__declspec(dllexport)
#else
	#define RAYTRACER_EXTERN_FUNC		__declspec(dllimport)
#endif
typedef unsigned int VOXEL;


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
typedef void (*ONSETPIXEL_CALLBACK)(const Point<>& pos, unsigned long color);
typedef void (*ONCASTRAY_CALLBACK)(const float* from, const float* to);
typedef void (*ONFRAMEDONE_CALLBACK)(int frame, int numframes);


struct RayTracingOptions
{
	enum Technique
	{
		RTT_DEBUG,
		RTT_SHOW_COLORS,
		RTT_SHOW_NORMALS,
		RTT_SHOW_UV,
		RTT_SHOW_REFLECT_DIRS,
		RTT_SHOW_REFRACT_DIRS,
		RTT_SHOW_CURVATURE,
		RTT_WHITTED_RT,
		RTT_VDA_GI
	} technique;
	bool quietmodeenabled, usethinrefractions, batchviewrays;
	unsigned char supersamplingfactor;
	enum TextureSampling
	{
		TS_POINT, TS_LINEAR
	} texsampling;
	Rect<> region;
	int startframe;
	ISRayTracer::IRaySpace* rayspace;
	ONSETPIXEL_CALLBACK onsetpixel;
	ONCASTRAY_CALLBACK oncastray;
	ONFRAMEDONE_CALLBACK onframedone;
	volatile bool keepalive; // Boolean value that can be switched to false from another thread to request algorithm termination

	RayTracingOptions() : quietmodeenabled(false), usethinrefractions(false), batchviewrays(false), supersamplingfactor(1),
						  texsampling(TS_LINEAR), region(0, 0, -1, -1), startframe(1), rayspace(NULL), onsetpixel(NULL), oncastray(NULL), onframedone(NULL), keepalive(true) {}
	unsigned char IsBlendingRequired() const {return supersamplingfactor > 1;} // Choose whether to use blending based on the combination of algorithms used
};


namespace ISRayTracer
{
	//-----------------------------------------------------------------------------
	// Name: ICamera
	// Desc: Interface to the Camera class
	//-----------------------------------------------------------------------------
	typedef class ICamera
	{
	public:
		float fovx, fovy;
		Size<> size;
		Rect<> region;
		int maxbounces;

		virtual const Matrix* GetTransform() = 0;
		virtual const Matrix* SetTransform(const Matrix* transform) = 0;
		virtual void SetTransform(const Vector3* translation, const Vector3* rotation) = 0;
		virtual void SetTransform(const Vector3* translation, const Quaternion* rotation) = 0;
		virtual void SetTransform(const Vector3* pos, const Vector3* target, const Vector3* up) = 0;
		virtual void Release() = 0;
	}* LPCAMERA;

	//-----------------------------------------------------------------------------
	// Name: ILight
	// Desc: Interface to the Light class
	//-----------------------------------------------------------------------------
	typedef class ILight
	{
	public:
		Color color;
		virtual void Release() = 0;
	}* LPLIGHT;

	//-----------------------------------------------------------------------------
	// Name: IPointLight
	// Desc: Interface to the PointLight class
	//-----------------------------------------------------------------------------
	typedef class IPointLight : public ILight
	{
	public:
		Vector3 pos;
	}* LPPOINTLIGHT;

	//-----------------------------------------------------------------------------
	// Name: IDirLight
	// Desc: Interface to the PointLight class
	//-----------------------------------------------------------------------------
	typedef class IDirLight : public ILight
	{
	public:
		Vector3 dir;
	}* LPDIRLIGHT;

	//-----------------------------------------------------------------------------
	// Name: ISpotLight
	// Desc: Interface to the SpotLight class
	//-----------------------------------------------------------------------------
	typedef class ISpotLight : public ILight
	{
	public:
		Vector3 pos, dir, attenuation;
		float spotpower;
	}* LPSPOTLIGHT;

	//-----------------------------------------------------------------------------
	// Name: IMaterial
	// Desc: Interface to the IMaterial class
	//-----------------------------------------------------------------------------
	typedef class IMaterial
	{
	public:
		Color ambientclr, diffuseclr, specularclr, emissiveclr;
		Image *texture, *normalmap;
		float specularpwr;
		float reflectance, transmittance, absorbance;
		float refraction;

		virtual void Release() = 0;
	}* LPMATERIAL;

	//-----------------------------------------------------------------------------
	// Name: ISurface
	// Desc: Interface to the Surface class
	//-----------------------------------------------------------------------------
	typedef class ISurface
	{
	public:
		virtual IMaterial* GetMaterial(unsigned int idx = 0) = 0;
		virtual IMaterial* SetMaterial(IMaterial* material, unsigned int idx = 0) = 0;
		virtual const Matrix* GetTransform() = 0;
		virtual const Matrix* SetTransform(const Matrix* transform) = 0;
		virtual void SetTransform(const Vector3* translation, const Vector3* rotation, const Vector3* scale) = 0;
		virtual void SetTransform(const Vector3* translation, const Quaternion* rotation, const Vector3* scale) = 0;
		__declspec(property(get=GetTransform, put=SetTransform)) const Matrix* Transform;
		virtual void Release() = 0;
	}* LPSURFACE;

	//-----------------------------------------------------------------------------
	// Name: ISphereSurface
	// Desc: Interface to the SphereSurface class
	//-----------------------------------------------------------------------------
	typedef class ISphereSurface : public ISurface
	{
	public:
	}* LPSPHERESURFACE;

	//-----------------------------------------------------------------------------
	// Name: IMeshSurface
	// Desc: Interface to the MeshSurface class
	//-----------------------------------------------------------------------------
	typedef class IMeshSurface : public ISurface
	{
	public:
		unsigned int numfaces;

		virtual void SetVertexPositions(LPVOID positions, size_t stride) = 0;
		virtual void SetVertexNormals(LPVOID normals, size_t stride) = 0;
		virtual void SetVertexTangents(LPVOID tangents, size_t stride) = 0;
		virtual void SetVertexBinormals(LPVOID binormals, size_t stride) = 0;
		virtual void SetVertexTexcoords(LPVOID texcoords, size_t stride) = 0;
		virtual void SetVertexPositionIndices(LPVOID positionindices, size_t stride) = 0;
		virtual void SetVertexNormalIndices(LPVOID normalindices, size_t stride) = 0;
		virtual void SetVertexTexcoordIndices(LPVOID texcoordindices, size_t stride) = 0;
		virtual void SetFaceAttributes(LPVOID attributes, size_t stride) = 0;
virtual void CreateOctree() = 0;
	}* LPMESHSURFACE;

	//-----------------------------------------------------------------------------
	// Name: IVMeshSurface
	// Desc: Interface to the VMeshSurface class
	//-----------------------------------------------------------------------------
	typedef class IVMeshSurface : public ISurface
	{
	public:
		VOXEL* voxelmask;
int size[3];
Vector3 bounds_min, bounds_max;

		virtual void SetVoxelAttributes(LPVOID attributes, size_t stride) = 0;
	}* LPVMESHSURFACE;

	//-----------------------------------------------------------------------------
	// Name: IScene
	// Desc: Interface to the Scene class
	//-----------------------------------------------------------------------------
	typedef class IScene
	{
	public:
		Color bgclr, ambientclr;

		virtual Result CreateRaySpace(bool multireslattice, bool is2d, size_t numsamples_u, size_t numsamples_v, size_t numsamples_theta, size_t numsamples_phi, IRaySpace** rayspace) = 0;
		virtual Result CreateRaySpace(const FilePath& filename, IRaySpace** rayspace) = 0;
		virtual ICamera* GetCamera() = 0;
		virtual ICamera* SetCamera(ICamera* cam) = 0;
		__declspec(property(get=GetCamera, put=SetCamera)) ICamera* Camera;
		virtual void AddSurface(ISRayTracer::ISurface* sfc) = 0;
		virtual void RemoveSurface(ISRayTracer::ISurface* sfc) = 0;
		virtual void AddLight(ISRayTracer::ILight* light) = 0;
		virtual void RemoveLight(ISRayTracer::ILight* light) = 0;
		virtual void Release() = 0;
	}* LPSCENE;
}

//-----------------------------------------------------------------------------
// Name: IRayTracer
// Desc: Interface to the RayTracer class
//-----------------------------------------------------------------------------
typedef class IRayTracer
{
public:
	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result CreateScene(ISRayTracer::IScene** scene) = 0;
	virtual Result CreateScene(IOutputWindow* wnd, ISRayTracer::IScene** scene) = 0;
	virtual Result CreateScene(const SceneDefinition* scenedef, const std::map<FilePath, MeshDefinition*>* meshdefs, std::map<FilePath, Image*>* textures, ISRayTracer::IScene** scene) = 0;
	virtual Result CreateCamera(ISRayTracer::ICamera** cam) = 0;
	virtual Result CreatePointLight(ISRayTracer::IPointLight** light) = 0;
	virtual Result CreateDirLight(ISRayTracer::IDirLight** light) = 0;
	virtual Result CreateSpotLight(ISRayTracer::ISpotLight** light) = 0;
	virtual Result CreateSphereSurface(const Vector3* pos, float radius, ISRayTracer::ISphereSurface** sfc) = 0;
	virtual Result CreateMeshSurface(ISRayTracer::IMeshSurface** sfc) = 0;
	virtual Result CreateVMeshSurface(ISRayTracer::IVMeshSurface** sfc) = 0;
	virtual Result CreateSolidMaterial(ISRayTracer::IMaterial** mat) = 0;
	virtual void Render(const ISRayTracer::IScene* scene, const RayTracingOptions& options) = 0;
	virtual Result ParseXmlScene(FilePath filename, SceneDefinition** scenedef) const = 0;
	virtual Result ParseObjMesh(FilePath filename, MeshDefinition** meshdef) const = 0;
	virtual Result ComputeTangentsAndBinormals(FilePath filename, MeshDefinition* meshdef) const = 0;
	virtual void Release() = 0;
}* LPRAYTRACER;

extern "C" RAYTRACER_EXTERN_FUNC LPRAYTRACER __cdecl CreateRayTracer();

#endif