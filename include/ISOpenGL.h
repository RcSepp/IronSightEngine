#ifndef __ISOPENGL_H
#define __ISOPENGL_H

#include <ISEngine.h>
#include <ISMath.h>
#include <ISHvkD3dInterop.h>

#ifndef _WIN32
struct _XDisplay;
#ifndef Display
typedef _XDisplay Display;
#endif
#ifndef Window
typedef long unsigned int Window;
#endif
#endif


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_OPENGL
	#define OPENGL_EXTERN_FUNC	__declspec(dllexport)
#else
	#define OPENGL_EXTERN_FUNC	__declspec(dllimport)
#endif


class IRenderable;
class IOutputWindow;
class OutputWindow;
class ITexture;
class IObject;

typedef class IRegularEntity* LPREGULARENTITY;
typedef class IRagdollEntity* LPRAGDOLLENTITY;


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum RenderType
	{RT_BACKGROUND, RT_DEFAULT, RT_FAST, RT_FOREGROUND};
enum OrientationMode
	{OM_EULER, OM_QUAT, OM_MATRIX};
enum OglShapeId
{
	OSID_PLANE, OSID_BOX, OSID_SPHERE, OSID_CAPSULE, OSID_CYLINDER, OSID_CONE
};


//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct OglStartupSettings
{
	bool isfullscreen;
	int oglversion_major, oglversion_minor;
};
struct Frustum
{
	Plane pleft, pright, ptop, pbottom, pnear, pfar;
};
struct OGLMaterial
{
	Color ambient, diffuse, specular;
	float power;
	ITexture* tex;
	bool operator==(const OGLMaterial& other) const
		{return this->tex == other.tex && this->power == other.power && this->ambient == other.ambient && this->diffuse == other.diffuse && this->specular == other.specular;}
};
struct OglShapeDesc
{
protected:
	const OglShapeId id;
	OglShapeDesc(OglShapeId id) : id(id), texcoordscale(1.0f, 1.0f) {}
public:
	Vector2 texcoordscale;
	OglShapeId GetId() const {return id;}
	virtual UINT GetVertexCount() const = 0;
	virtual UINT GetFaceCount() const = 0;
};
struct OglPlaneShapeDesc : public OglShapeDesc
{
	OglPlaneShapeDesc() : OglShapeDesc(OSID_PLANE) {}
	Vector3 pos, nml;
	Vector2 size;

	UINT GetVertexCount() const {return 4;}
	UINT GetFaceCount() const {return 2;}
};
struct OglBoxShapeDesc : public OglShapeDesc
{
	OglBoxShapeDesc() : OglShapeDesc(OSID_BOX) {}
	Vector3 pos, size;

	UINT GetVertexCount() const {return 24;}
	UINT GetFaceCount() const {return 12;}
};
struct OglSphereShapeDesc : public OglShapeDesc
{
	OglSphereShapeDesc() : OglShapeDesc(OSID_SPHERE), pos(0.0f, 0.0f, 0.0f), rot_y(0.0f), theta(PI), phi(2.0f * PI) {}
	Vector3 pos;
	float rot_y, radius;
	UINT stacks, slices;
	float theta, phi;

	UINT GetVertexCount() const {return (slices + (UINT)(phi != 2.0f * PI)) * (stacks + 1);}
	UINT GetFaceCount() const {return slices * stacks * 2;}
};
struct OglCapsuleShapeDesc : public OglShapeDesc
{
	OglCapsuleShapeDesc() : OglShapeDesc(OSID_CAPSULE) {}
	Vector3 v0, v1;
	float radius;
	UINT stacks, slices;

	UINT GetVertexCount() const {return slices * (stacks + 2);}
	UINT GetFaceCount() const {return slices * (stacks + 1) * 2;}
};
struct OglCylinderShapeDesc : public OglShapeDesc
{
	OglCylinderShapeDesc() : OglShapeDesc(OSID_CYLINDER), hasfrontfaces(true) {}
	Vector3 v0, v1;
	float radius;
	UINT slices;
	bool hasfrontfaces;

	UINT GetVertexCount() const {return (hasfrontfaces ? 4 : 2) * slices;}
	UINT GetFaceCount() const {return 2 * slices + (hasfrontfaces ? 2 * (slices - 2) : 0);}
};
struct OglConeShapeDesc : public OglShapeDesc
{
	OglConeShapeDesc() : OglShapeDesc(OSID_CONE), hasgroundfaces(true) {}
	Vector3 v0, v1;
	float radius;
	UINT slices;
	bool hasgroundfaces;

	UINT GetVertexCount() const {return (hasgroundfaces ? 3 : 2) * slices;}
	UINT GetFaceCount() const {return 2 * slices + (hasgroundfaces ? slices - 2 : 0);}
};


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
typedef void (CALLBACK* RENDERFUNC)(IOutputWindow* wnd, LPVOID user);
//typedef Result (CALLBACK* LOADSCREENFUNC)();
typedef void (__stdcall* RENDER_CALLBACK)(IObject* sender, bool& cancel, LPVOID user);
typedef Result (__stdcall* GETREGISTEREDCLASSES_CALLBACK)(IRenderable* renderable, LPVOID user);


//-----------------------------------------------------------------------------
// Name: IMoveable
// Desc: Interface to a class that can be moved by passing a transformation matrix
//-----------------------------------------------------------------------------
class IMoveable
{
public:
	virtual void SetTransform(const Matrix* transform) {}
};

//-----------------------------------------------------------------------------
// Name: IRenderable
// Desc: Interface to a class with render function
//-----------------------------------------------------------------------------
class IRenderable : public IMoveable
{
public:
	bool autorenderenabled;

	IRenderable()
	{
		autorenderenabled = true;
	};
	virtual void Render(RenderType rendertype) = 0;
	virtual bool GetRefPos(Vector3& pos) const {return false;}
	virtual Result GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype) {return cbk(this, cbkuser);};
	virtual IObject* GetObject();
};

//-----------------------------------------------------------------------------
// Name: IUpdateable
// Desc: Interface to a class with update function
//-----------------------------------------------------------------------------
class IUpdateable
{
public:
	virtual void Update() = 0;
};


//-----------------------------------------------------------------------------
// Name: ITexture
// Desc: Interface to the Texture class
//-----------------------------------------------------------------------------
typedef class ITexture
{
public:
	enum Usage
		{TU_SKIN, TU_SKIN_NOMIPMAP, TU_SOURCE, TU_RENDERTARGET, TU_RENDERTARGETORSOURCE, TU_ALLACCESS, TU_CPUWRITE};

	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual int GetNumMipLevels() = 0;
	virtual FilePath GetFilename() = 0;
	virtual Result MapPixelData(bool read, bool write, UINT* stride, LPVOID* data) = 0;
	virtual void UnmapData() = 0;
	/*virtual Result Save(const FilePath& filename, D3DX_IMAGE_FILEFORMAT format) = 0;
	virtual Result Save(LPD3D10BLOB* datablob, D3DX_IMAGE_FILEFORMAT format) = 0;*/
	virtual void Release() = 0;
}* LPTEXTURE;

//-----------------------------------------------------------------------------
// Name: IRenderShader
// Desc: Interface to the RenderShader class
//-----------------------------------------------------------------------------
typedef class IRenderShader
{
/*public:
	virtual LPD3D11EFFECTVARIABLE GetVariableBySemantic(LPCSTR semantic) = 0;
	virtual LPD3D11EFFECTVARIABLE GetVariableByName(LPCSTR name) = 0;
	virtual IEffectScalarVariable* GetScalarVariable(const String& semantic) = 0;
	virtual IEffectVectorVariable* GetVectorVariable(const String& semantic) = 0;
	virtual IEffectMatrixVariable* GetMatrixVariable(const String& semantic) = 0;
	virtual Result SetTechnique(UINT techidx) = 0;*/
}* LPRENDERSHADER;

//-----------------------------------------------------------------------------
// Name: IObject
// Desc: Interface to the Object class
//-----------------------------------------------------------------------------
typedef class IObject : public IRenderable
{
public:
	struct Vertex
	{
		Vector3 pos, nml;
		Vector2 texcoord;
		UINT blendidx;
		bool operator==(const Vertex& other) const
			{return this->pos == other.pos && this->nml == other.nml && this->texcoord == other.texcoord && this->blendidx == other.blendidx;}
	};
	struct AttributeRange
	{
		UINT attrid;
		UINT facestart, facecount;
		UINT vertexstart, vertexcount;
	};
	struct BoundingSphere
	{
		Vector3 center;
		float radius;
	};
	struct BoundingBox
	{
		Vector3 center, size;
	};
	OrientationMode orientmode;
	Matrix transform;
	std::vector<Matrix> bonetransform;
	Vector3 pos, rot, scl;
	Quaternion qrot;
	bool iscameradependent;

	virtual UINT GetNumBones() = 0;
	std::vector<OGLMaterial>* mats;

	virtual Result CreateCollisionMesh(LPREGULARENTITY* entity) = 0;
	virtual Result CreateConvexCollisionHull(LPREGULARENTITY* entity) = 0;
	virtual Result CreateCollisionCapsule(float radius, LPREGULARENTITY* entity) = 0;
	virtual Result CreateCollisionRagdoll(float radius, bool usedynamicmotion, LPRAGDOLLENTITY* entity) = 0;
	virtual Result SetShader(IRenderShader* shader) = 0;
	virtual IRenderShader* GetShader() const = 0;
	virtual Result SetVertexData(const Vertex* vertices) = 0;
	virtual Result SetIndexData(const UINT* indices) = 0;
	virtual UINT GetVertexCount() = 0;
	virtual UINT GetFaceCount() = 0;
	virtual UINT GetIndexCount() = 0;
	virtual Result MapVertexData(Vertex** vertices) = 0;
	virtual Result MapIndexData(UINT** indices) = 0;
	virtual Result UnmapData(LPVOID vertices) = 0;
	virtual void GetAttributeTable(AttributeRange* attributetable, UINT* attributetablesize) = 0;
	virtual Result SetAttributeTable(const AttributeRange* attributetable, UINT attributetablesize) = 0;
	virtual Result CommitChanges() = 0;
	virtual Result ComputeNormals() = 0;
	virtual Result Split(const Plane* splitplane, IObject** first, IObject** second) = 0;
	virtual Result Break(int targetnumparts, IObject** parts, int* numparts) = 0;
	virtual const BoundingSphere& GetBoundingSphere() const = 0;
	virtual const Vector3& GetBoundingSphereCenter() const = 0;
	virtual float GetBoundingSphereRadius() = 0;
	virtual const BoundingBox& GetBoundingBox() const = 0;
	virtual const Vector3& GetBoundingBoxCenter() const = 0;
	virtual const Vector3& GetBoundingBoxSize() = 0;
	virtual void SetBeforeRenderCallback(RENDER_CALLBACK cbk, LPVOID user) = 0;
}* LPOBJECT;

//-----------------------------------------------------------------------------
// Name: ICamera
// Desc: Interface to the Camera class
//-----------------------------------------------------------------------------
typedef class ICamera : public IRenderable, public IUpdateable
{
public:
	enum ViewMode
		{VM_DEFAULT, VM_FLY, VM_FIRSTPERSON, VM_THIRDPERSON} viewmode;
	OrientationMode orientmode;
	Matrix transform;
	Vector3 pos, rot;
	Quaternion qrot;

	virtual void UpdateMatrices() = 0;
	virtual void SetClipNear(float clipnear) = 0;
	virtual void SetClipFar(float clipfar) = 0;
	virtual void SetPlayerModel(LPOBJECT obj, LPRAGDOLLENTITY ragdoll = NULL) = 0;
	virtual void SetPerspectiveProjection(float fov) = 0;
	virtual void SetOrthogonalProjection(float width, float height) = 0;
	virtual Frustum& GetViewFrustum() = 0;
	virtual Matrix& GetViewMatrix() = 0;
	virtual Matrix& GetProjMatrix() = 0;
	virtual Matrix& GetViewProjMatrix() = 0;
	virtual Vector3* ComputeViewDir(Vector3* vdir) const = 0;
	virtual Vector3* ComputePixelDir(const Point<int>& pixelpos, Vector3* vdir) const = 0;
}* LPCAMERA;

//-----------------------------------------------------------------------------
// Name: IOutputWindow
// Desc: Interface to the OutputWindow class
//-----------------------------------------------------------------------------
typedef class IOutputWindow
{
public:
	struct Settings
	{
		Size<UINT> backbuffersize;
#ifdef _WIN32
		LPVOID display;
		HWND wnd;
#else
		Display* display;
		Window wnd;
#endif
		bool enablemultisampling;
		int screenidx;
	};
	const bool ismultisampled;
	const int screenidx;

	Color backcolor;
	bool backcolorenabled;

	/*virtual Size<UINT> GetBackbufferSize() = 0;
	virtual UINT GetBackbufferWidth() = 0;
	virtual UINT GetBackbufferHeight() = 0;
	virtual HWND GetHwnd() = 0;
	virtual ITexture* GetBackbuffer() = 0;
	virtual ITexture* SetBackbuffer(ITexture* texbackbuffer) = 0;
	__declspec(property(get=GetBackbuffer, put=SetBackbuffer)) ITexture* Backbuffer;
	virtual bool IsDeviceLost() = 0;
	virtual Result Resize(UINT width, UINT height) = 0;*/
	virtual void RegisterForRendering(RENDERFUNC func, RenderType rendertype, LPVOID user = NULL) = 0;
	virtual void RegisterForRendering(IRenderable* cls, RenderType rendertype) = 0;
	virtual void DeregisterFromRendering(RENDERFUNC func, RenderType rendertype) = 0;
	virtual void DeregisterFromRendering(IRenderable* cls, RenderType rendertype) = 0;
	virtual void RegisterForUpdating(IUpdateable* cls) = 0;
	virtual void DeregisterFromUpdating(IUpdateable* cls) = 0;
	/*virtual void SortRegisteredClasses(RenderType rendertype, const Vector3& campos) = 0;
	virtual Result GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype) = 0;
	virtual void DrawLine(const Vector3& v0, const Vector3& v1, Color c0, Color c1) = 0;
	virtual void DrawLine(const Vector2& v0, const Vector2& v1, Color c0, Color c1) = 0;
	virtual void DrawSprite(const Sprite* sprite) = 0;
	virtual Result EnableSprites(const FilePath spriteshaderfilename) = 0;
	virtual Result EnableLineDrawing(const FilePath lineshaderfilename) = 0;
	virtual void ShowFps() = 0;
	virtual void HideFps() = 0;
	virtual void TakeScreenShot(const FilePath& filename, D3DX_IMAGE_FILEFORMAT fileformat) = 0;*/
	virtual void SetCamera(ICamera* cam) = 0;
	virtual ICamera* GetCamera() const = 0;
	/*virtual D3D11_VIEWPORT& GetViewport() = 0;
	virtual ID3D11Device* GetDevice() const = 0;
	virtual ID3D11DeviceContext* GetDeviceContext() const = 0;
	virtual Result MakeConsoleTarget(UINT numlines, UINT historysize) = 0;*/
	virtual Result CreateTexture(const FilePath& filename, ITexture::Usage usage, ITexture** tex) = 0;
	virtual Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, ITexture** tex) = 0;
	virtual Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, LPVOID data, ITexture** tex) = 0;
	virtual Result CreateTexture(ITexture* source, ITexture::Usage usage, ITexture** tex) = 0;
	virtual Result CreateRenderShader(const FilePath filename, IRenderShader** shader) = 0;
	virtual Result CreateObject(const FilePath& filename, bool loadtextures, IObject** obj) = 0;
	virtual Result CreateObject(IObject* srcobj, bool newmaterials, IObject** obj) = 0;
	virtual Result CreateObject(UINT numvertices, UINT numfaces, IObject** obj) = 0;
	virtual Result CreateObject(const OglShapeDesc& shapedesc, IObject** obj) = 0;
	virtual Result CreateObject(const OglShapeDesc* shapedescs[], UINT numshapedescs, IObject** obj) = 0;
	virtual void RemoveObject(IObject* obj) = 0;
	/*virtual Result CreateHUD(IHUD** hud) = 0;
	virtual Result CreateCursor(ID3dCursor** cursor) = 0;
	virtual Result CreateSpriteContainer(UINT buffersize, ISpriteContainer** spc) = 0;
	virtual Result CreateD3dFont(const FontType& type, ID3dFont** font) = 0;
	virtual Result CreateSceneManager(ISceneManager** mgr) = 0;
	virtual Result CreateSkyBox(ISkyBox** sky) = 0;
	virtual Result CreateBoxedLevel(const UINT (&chunksize)[3], UINT numchunkbuffers, LPTEXTURE texboxes, LPRENDERSHADER blevelshader, IBoxedLevel** blevel) = 0;*/
	/*virtual Result CreateLandscape(FilePath& filename, LPRENDERSHADER shader, ILandscape** landscape) = 0;*/ //EDIT11

	IOutputWindow(const Settings& wndsettings) : ismultisampled(wndsettings.enablemultisampling), screenidx(wndsettings.screenidx) {}
}* LPOUTPUTWINDOW;

//-----------------------------------------------------------------------------
// Name: IOpenGL
// Desc: Interface to the OpenGL class
//-----------------------------------------------------------------------------
typedef class IOpenGL
{
public:

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init(const OglStartupSettings* settings) = 0;
	virtual void Render() = 0;
	virtual Result CreateOutputWindow(IOutputWindow::Settings* settings, IOutputWindow** wnd) = 0;
	virtual Result CreateCamera(float fov, float clipnear, float clipfar, ICamera** cam) = 0;
	virtual void Release() = 0;
}* LPOPENGL;

extern "C" OPENGL_EXTERN_FUNC LPOPENGL __cdecl CreateOpenGL();

#endif