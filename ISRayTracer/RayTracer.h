#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include "ISRayTracer.h"
#include "Octree.h"
#include "RayCastParams.h"
#include <ISSceneDefinition.h>
#include <ISMeshDefinition.h>
#ifndef _WIN32
	#include <time.h>
#endif

#ifdef _DEBUG
	#pragma comment (lib, "ISMath_d.lib")
#else
	#pragma comment (lib, "ISMath.lib")
#endif


#define REFRACT_IDX_AIR		1.000293f


class RayTracer;
class RaySpace;

class TextureSampler
{
private:
	float rgb[3];

protected:
	float* GetPixel(float* fResult, const Image* texture, const Point<int>* pos);

public:
	Color* Sample(Color* cResult, const Image* texture, const Vector2* uv);
	virtual float* Sample(float* fResult, const Image* texture, const Vector2* uv) = 0;
};
class PointTextureSampler : public TextureSampler
{
private:
	Point<int> pt;

public:
	float* Sample(float* fResult, const Image* texture, const Vector2* uv);
};
class LinearTextureSampler : public TextureSampler
{
private:
	Vector2 fpos, f;
	Point<int> pt[4];
	float rgba[4][4];
	//Color c[4], lerpclr1, lerpclr2;

public:
	float* Sample(float* fResult, const Image* texture, const Vector2* uv);
};

//-----------------------------------------------------------------------------
// Name: Camera
// Desc: A camera to be used by one or more scenes as viewpoint for rendering
//-----------------------------------------------------------------------------
class Camera : public ISRayTracer::ICamera
{
public:
	RayTracer* parent;
	Matrix transform;
	Vector3 pos;

	Camera(RayTracer* parent) : parent(parent)
	{
		MatrixIdentity(&transform);
		Vec3Set(&pos, 0.0f, 0.0f, 0.0f);
		fovx = 0.25f * PI;
		size = Size<>(256, 256);
		maxbounces = 8;

		fovy = fovx * size.height / size.width;
		region = Rect<>(0, 0, size.width, size.height);
	};
	Matrix* GetTransform() {return &transform;}
	const Matrix* SetTransform(const Matrix* transform);
	void SetTransform(const Vector3* translation, const Vector3* rotation);
	void SetTransform(const Vector3* translation, const Quaternion* rotation);
	void SetTransform(const Vector3* pos, const Vector3* target, const Vector3* up);

	/*struct RayIterator : public Point<>
	{
		friend class Camera;
	private:
		Camera* cam;
		RayIterator(Camera* cam, int x, int y) : Point<>(x, y), cam(cam) {}

	public:
		RayIterator() : cam(0) {}
		void operator++();
		void operator++(int);
	};
	RayIterator begin();
	RayIterator end();*/
	Vector3* SetRayDir(Vector3* vDir, const Point<>& pixelpos);
	Vector3* SetRayDir(Vector3* vDir, const Point<>& pixelpos, const Vector2* pixeloffset);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Light
// Desc: Base class for all types of light sources
//-----------------------------------------------------------------------------
class Light
{
public:
	const Color* lightcolor;

	Light(const Color* lightcolor) : lightcolor(lightcolor) {}
	virtual float GetExposure(const Vector3* pos, RayCastParams& params) = 0;
};

//-----------------------------------------------------------------------------
// Name: PointLight
// Desc: A point light source
//-----------------------------------------------------------------------------
class PointLight : public Light, public ISRayTracer::IPointLight
{
public:
	RayTracer* parent;

	PointLight(RayTracer* parent) : Light(&color), parent(parent)
	{
		Vec3Set(&pos, 0.0f, 0.0f, 0.0f);
		color = Color(1.0f, 1.0f, 1.0f);
	};

	float GetExposure(const Vector3* pos, RayCastParams& params);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: DirLight
// Desc: A directional light source
//-----------------------------------------------------------------------------
class DirLight : public Light, public ISRayTracer::IDirLight
{
public:
	RayTracer* parent;

	DirLight(RayTracer* parent) : Light(&color), parent(parent)
	{
		Vec3Set(&dir, 0.0f, 0.0f, 1.0f);
		color = Color(1.0f, 1.0f, 1.0f);
	};

	float GetExposure(const Vector3* pos, RayCastParams& params);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: SpotLight
// Desc: A directional spot source
//-----------------------------------------------------------------------------
class SpotLight : public Light, public ISRayTracer::ISpotLight
{
public:
	RayTracer* parent;

	SpotLight(RayTracer* parent) : Light(&color), parent(parent)
	{
		Vec3Set(&pos, 0.0f, 0.0f, 0.0f);
		Vec3Set(&dir, 0.0f, 0.0f, 1.0f);
		Vec3Set(&attenuation, 1.0f, 0.0f, 0.0f);
		spotpower = 100.0f;
		color = Color(1.0f, 1.0f, 1.0f);
	};

	float GetExposure(const Vector3* pos, RayCastParams& params);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Material
// Desc: Collection of material properties for surfaces
//-----------------------------------------------------------------------------
class Material : public ISRayTracer::IMaterial
{
public:
	RayTracer* parent;

	Material(RayTracer* parent) : parent(parent)
	{
		ambientclr = Color(0.0f, 0.0f, 0.0f);
		diffuseclr = Color(0.0f, 0.0f, 0.0f);
		specularclr = Color(0.0f, 0.0f, 0.0f);
		emissiveclr = Color(0.0f, 0.0f, 0.0f);
		texture = normalmap = NULL;
		specularpwr = 1.0f;
		reflectance = transmittance = 0.0f;
		absorbance = 1.0f;
		refraction = 1.0f;
	};

	Color* Sample(Color* clr, const Vector2* uv, TextureSampler* sampler) const;
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Surface
// Desc: Base class for all renderable entities
//-----------------------------------------------------------------------------
class Surface
{
public:
	std::vector<::Material*> materials;
	Matrix *transform, *invtransform;
	Matrix *transform_invtrans, *invtransform_invtrans;
	Vector3 translation, rotation, scale;

	Surface() : transform(NULL)
	{
		materials.push_back(NULL);
	}
	~Surface()
	{
		if(transform)
		{
			delete transform;
			delete transform;
			delete transform_invtrans;
			delete invtransform_invtrans;
		}
	}

	Matrix* GetTransform() {return transform;}
	const Matrix* SetTransform(const Matrix* transform);
	void SetTransform(const Vector3* translation, const Vector3* rotation, const Vector3* scale);
	void SetTransform(const Vector3* translation, const Quaternion* rotation, const Vector3* scale);
	ISRayTracer::IMaterial* GetMaterial(unsigned int idx);
	ISRayTracer::IMaterial* SetMaterial(ISRayTracer::IMaterial* material, unsigned int idx);
	virtual void CastRay(RayCastParams& params) const = 0;
	virtual void BatchCastRay(BatchRayCastParams& batchparams) const = 0;
	virtual void GetRayCastHitDetails(RayCastParams& params, TextureSampler* sampler) const = 0;
	virtual bool GetPosition(Vector3* pos, float u, float v) const = 0;
	virtual bool GetNormal(Vector3* nml, float u, float v) const = 0;
	virtual bool GetPositionAndNormal(Vector3* pos, Vector3* nml, float u, float v) const {return GetPosition(pos, u, v) && GetNormal(nml, u, v);}
	virtual float GetSampleArea(float u, float v, float numsamples_u, float numsamples_v) const = 0;
	void Release();
};

//-----------------------------------------------------------------------------
// Name: SphereSurface
// Desc: A spherical object
//-----------------------------------------------------------------------------
class SphereSurface : public Surface, public ISRayTracer::ISphereSurface
{
public:
	RayTracer* parent;
	Vector3 pos;
	float radius;

	SphereSurface(RayTracer* parent, const Vector3* pos, float radius) : Surface(), parent(parent), pos(*pos), radius(radius)
	{
	};

	Matrix* GetTransform() {return Surface::GetTransform();}
	const Matrix* SetTransform(const Matrix* transform) {return Surface::SetTransform(transform);}
	void SetTransform(const Vector3* translation, const Vector3* rotation, const Vector3* scale) {Surface::SetTransform(translation, rotation, scale);}
	void SetTransform(const Vector3* translation, const Quaternion* rotation, const Vector3* scale) {Surface::SetTransform(translation, rotation, scale);}
	ISRayTracer::IMaterial* GetMaterial(unsigned int idx) {return Surface::GetMaterial(idx);}
	ISRayTracer::IMaterial* SetMaterial(ISRayTracer::IMaterial* material, unsigned int idx) {return Surface::SetMaterial(material, idx);}
	void CastRay(RayCastParams& params) const;
	void BatchCastRay(BatchRayCastParams& batchparams) const;
	void GetRayCastHitDetails(RayCastParams& params, TextureSampler* sampler) const;
	bool GetPosition(Vector3* pos, float u, float v) const;
	bool GetNormal(Vector3* nml, float u, float v) const;
	bool GetPositionAndNormal(Vector3* pos, Vector3* nml, float u, float v) const;
	float GetSampleArea(float u, float v, float numsamples_u, float numsamples_v) const;
	void Release();
};

//-----------------------------------------------------------------------------
// Name: MeshSurface
// Desc: A triangulated object
//-----------------------------------------------------------------------------
class MeshSurface : public Surface, public ISRayTracer::IMeshSurface
{
public:
	RayTracer* parent;
	Array<const Vector3> positions, normals, tangents, binormals;
	Array<const Vector2> texcoords;
	Array<const unsigned int> positionindices, normalindices, texcoordindices;
	Array<const unsigned int> attributes;
	Octree octree;

	MeshSurface(RayTracer* parent) : Surface(), parent(parent)
	{
		numfaces = 0;
	};

	Matrix* GetTransform() {return Surface::GetTransform();}
	const Matrix* SetTransform(const Matrix* transform) {return Surface::SetTransform(transform);}
	void SetTransform(const Vector3* translation, const Vector3* rotation, const Vector3* scale) {Surface::SetTransform(translation, rotation, scale);}
	void SetTransform(const Vector3* translation, const Quaternion* rotation, const Vector3* scale) {Surface::SetTransform(translation, rotation, scale);}
	ISRayTracer::IMaterial* GetMaterial(unsigned int idx) {return Surface::GetMaterial(idx);}
	ISRayTracer::IMaterial* SetMaterial(ISRayTracer::IMaterial* material, unsigned int idx) {return Surface::SetMaterial(material, idx);}
	void SetVertexPositions(LPVOID positions, size_t stride) {this->positions.data = (unsigned char*)positions; this->positions.stride = stride;}
	void SetVertexNormals(LPVOID normals, size_t stride) {this->normals.data = (unsigned char*)normals; this->normals.stride = stride;}
	void SetVertexTangents(LPVOID tangents, size_t stride) {this->tangents.data = (unsigned char*)tangents; this->tangents.stride = stride;}
	void SetVertexBinormals(LPVOID binormals, size_t stride) {this->binormals.data = (unsigned char*)binormals; this->binormals.stride = stride;}
	void SetVertexTexcoords(LPVOID texcoords, size_t stride) {this->texcoords.data = (unsigned char*)texcoords; this->texcoords.stride = stride;}
	void SetVertexPositionIndices(LPVOID positionindices, size_t stride) {this->positionindices.data = (unsigned char*)positionindices; this->positionindices.stride = stride;}
	void SetVertexNormalIndices(LPVOID normalindices, size_t stride) {this->normalindices.data = (unsigned char*)normalindices; this->normalindices.stride = stride;}
	void SetVertexTexcoordIndices(LPVOID texcoordindices, size_t stride) {this->texcoordindices.data = (unsigned char*)texcoordindices; this->texcoordindices.stride = stride;}
	void SetFaceAttributes(LPVOID attributes, size_t stride) {this->attributes.data = (unsigned char*)attributes; this->attributes.stride = stride;}
void CreateOctree() {octree.Create(positions, positionindices, numfaces, 50, 100);}
	void CastRay(RayCastParams& params) const;
	void BatchCastRay(BatchRayCastParams& batchparams) const;
	void GetRayCastHitDetails(RayCastParams& params, TextureSampler* sampler) const;
	bool GetPosition(Vector3* pos, float u, float v) const;
	bool GetNormal(Vector3* nml, float u, float v) const;
	bool GetPositionAndNormal(Vector3* pos, Vector3* nml, float u, float v) const;
	float GetSampleArea(float u, float v, float numsamples_u, float numsamples_v) const;
	void Release();
};

//-----------------------------------------------------------------------------
// Name: VMeshSurface
// Desc: A voxelized object
//-----------------------------------------------------------------------------
class VMeshSurface : public Surface, public ISRayTracer::IVMeshSurface
{
public:
	RayTracer* parent;
	Array<const unsigned int> attributes;

	VMeshSurface(RayTracer* parent) : Surface(), parent(parent)
	{
	};

	Matrix* GetTransform() {return Surface::GetTransform();}
	const Matrix* SetTransform(const Matrix* transform) {return Surface::SetTransform(transform);}
	void SetTransform(const Vector3* translation, const Vector3* rotation, const Vector3* scale) {Surface::SetTransform(translation, rotation, scale);}
	void SetTransform(const Vector3* translation, const Quaternion* rotation, const Vector3* scale) {Surface::SetTransform(translation, rotation, scale);}
	ISRayTracer::IMaterial* GetMaterial(unsigned int idx) {return Surface::GetMaterial(idx);}
	ISRayTracer::IMaterial* SetMaterial(ISRayTracer::IMaterial* material, unsigned int idx) {return Surface::SetMaterial(material, idx);}
	void SetVoxelAttributes(LPVOID attributes, size_t stride) {this->attributes.data = (unsigned char*)attributes; this->attributes.stride = stride;}
	void CastRay(RayCastParams& params) const;
	void BatchCastRay(BatchRayCastParams& batchparams) const;
	void GetRayCastHitDetails(RayCastParams& params, TextureSampler* sampler) const;
	bool GetPosition(Vector3* pos, float u, float v) const {Result::PrintLogMessage("NOT IMPLEMENTED"); return false;}
	bool GetNormal(Vector3* nml, float u, float v) const {Result::PrintLogMessage("NOT IMPLEMENTED"); return false;}
	float GetSampleArea(float u, float v, float numsamples_u, float numsamples_v) const {Result::PrintLogMessage("NOT IMPLEMENTED"); return 0.0f;}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Scene
// Desc: A collection of all components required for rendering a ray-traced image
//-----------------------------------------------------------------------------
class Scene : public ISRayTracer::IScene
{
public:
	// References to created classes (for garbage collection)
	std::list<RaySpace*> rayspaces;

	RayTracer* parent;
	::Camera* cam;
	std::list<Surface*> surfaces;
	std::list<Light*> lights;

	Scene(RayTracer* parent) : parent(parent), cam(NULL)
	{
		bgclr = Color(0.0f, 0.0f, 0.0f, 0.0f);
		ambientclr = Color(1.0f, 1.0f, 1.0f);
	};
	~Scene()
	{
/*		if(sampler)
			delete sampler;*/
	}

	Result CreateRaySpace(bool multireslattice, bool is2d, size_t numsamples_u, size_t numsamples_v, size_t numsamples_theta, size_t numsamples_phi, ISRayTracer::IRaySpace** rayspace);
	Result CreateRaySpace(const FilePath& filename, ISRayTracer::IRaySpace** rayspace);
	ISRayTracer::ICamera* GetCamera() {return cam;}
	ISRayTracer::ICamera* SetCamera(ISRayTracer::ICamera* cam) {this->cam = (::Camera*)cam; return cam;}
	void AddSurface(ISRayTracer::ISurface* sfc) {surfaces.push_back(dynamic_cast<Surface*>(sfc));}
	void RemoveSurface(ISRayTracer::ISurface* sfc) {surfaces.remove(dynamic_cast<Surface*>(sfc));}
	void AddLight(ISRayTracer::ILight* light) {lights.push_back(dynamic_cast<Light*>(light));}
	void RemoveLight(ISRayTracer::ILight* light) {lights.remove(dynamic_cast<Light*>(light));}

	bool CastRay(RayCastParams& params, TextureSampler* sampler) const;
	void BatchCastRay(BatchRayCastParams& batchparams, TextureSampler* sampler, float frameblendfactor, ONRAYHIT_CALLBACK hitcbk, ONRAYHIT_CALLBACK misscbk) const;
	void Release();
};


//-----------------------------------------------------------------------------
// Name: RayTracer
// Desc: ?
//-----------------------------------------------------------------------------
class RayTracer : public IRayTracer
{
public:
	// References to created classes (for garbage collection)
	std::list<Scene*> scenes;
	std::list<Camera*> cameras;
	std::list<Light*> lights;
	std::list<Surface*> surfaces;
	std::list<Material*> materials;

	// Methods and variables used during a call to Render()
	struct RenderSequence
	{
		const RayTracingOptions& options;
		const Scene* scene;
		Rect<> region;
		TextureSampler* sampler;
		Color* blendbuffer;
		int numpixelsdrawn;
#ifdef _WIN32
		LARGE_INTEGER framestarttime, frameendtime, timerfreq;
#else
		timespec framestarttime, frameendtime, timerfreq;
#endif
		RenderSequence(const RayTracingOptions& options, const Scene* scene) : options(options), scene(scene)
		{
			region.x = options.region.x == -1 ? scene->cam->region.x : options.region.x;
			region.y = options.region.y == -1 ? scene->cam->region.y : options.region.y;
			region.width = options.region.width == -1 ? scene->cam->region.width : options.region.width;
			region.height = options.region.height == -1 ? scene->cam->region.height : options.region.height;

			switch(options.texsampling)
			{
			case RayTracingOptions::TS_POINT: sampler = new PointTextureSampler(); break;
			case RayTracingOptions::TS_LINEAR: sampler = new LinearTextureSampler(); break;
			default:
				Result::PrintLogMessage(String("Unknown texture sampler id: ") << String((int)options.texsampling));
				sampler = new PointTextureSampler();
			}
		}
		~RenderSequence()
		{
			delete sampler;
		}

		void RayTraceFrame(float frameblendfactor);
		void RayTraceFrame(float frameblendfactor, const Vector2* pixeloffset);
		static void OnRayHit_RTT_SHOW_NORMALS(RayCastParams& params, Point<int>& pos, float frameblendfactor);
		static void OnRayHit_RTT_SHOW_UV(RayCastParams& params, Point<int>& pos, float frameblendfactor);
		static void OnRayHit_RTT_SHOW_COLORS(RayCastParams& params, Point<int>& pos, float frameblendfactor);
		static void OnRayHit_RTT_SHOW_REFLECT_DIRS(RayCastParams& params, Point<int>& pos, float frameblendfactor);
		static void OnRayHit_RTT_SHOW_REFRACT_DIRS(RayCastParams& params, Point<int>& pos, float frameblendfactor);
		static void OnRayHit_RTT_SHOW_CURVATURE(RayCastParams& params, Point<int>& pos, float frameblendfactor);
		static void OnRayHit_RTT_WHITTED_RT(RayCastParams& params, Point<int>& pos, float frameblendfactor);
		static void OnRayMiss_RTT_WHITTED_RT(RayCastParams& params, Point<int>& pos, float frameblendfactor);
		void BeginFrame(int frame, int numframes);
		void EndFrame(int frame, int numframes);
		void SetPixel(const Point<>& pos, const Color* clr, float pixelblendfactor);
		bool CastRay(RayCastParams& params);
		void RecursiveWhittedRT(RayCastParams& params, Color& finalclr, float factor, int maxbounces);
	};

	RayTracer()
	{
	};

	void Sync(GLOBALVARDEF_LIST);
	Result CreateScene(ISRayTracer::IScene** scene);
	Result CreateScene(IOutputWindow* wnd, ISRayTracer::IScene** scene);
	Result CreateScene(const SceneDefinition* scenedef, const std::map<FilePath, MeshDefinition*>* meshdefs, std::map<FilePath, Image*>* textures, ISRayTracer::IScene** scene);
	Result CreateCamera(ISRayTracer::ICamera** cam);
	Result CreatePointLight(ISRayTracer::IPointLight** light);
	Result CreateDirLight(ISRayTracer::IDirLight** light);
	Result CreateSpotLight(ISRayTracer::ISpotLight** light);
	Result CreateSphereSurface(const Vector3* pos, float radius, ISRayTracer::ISphereSurface** sfc);
	Result CreateMeshSurface(ISRayTracer::IMeshSurface** sfc);
	Result CreateVMeshSurface(ISRayTracer::IVMeshSurface** sfc);
	Result CreateSolidMaterial(ISRayTracer::IMaterial** mat);
	void Render(const ISRayTracer::IScene* scene, const RayTracingOptions& options);
	Result ParseXmlScene(FilePath filename, SceneDefinition** scenedef) const {return ::ParseXmlScene(filename, scenedef);}
	Result ParseObjMesh(FilePath filename, MeshDefinition** meshdef) const {return ::ParseObjMesh(filename, meshdef);}
	Result ComputeTangentsAndBinormals(FilePath filename, MeshDefinition* meshdef) const {return ::ComputeTangentsAndBinormals(filename, meshdef);}
	void Release();
};

#endif