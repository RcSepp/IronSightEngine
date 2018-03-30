#ifndef __ISDIRECT3D_H
#define __ISDIRECT3D_H

#ifndef PARENT_NAMESPACE
#define PARENT_NAMESPACE
#endif

#include "ISEngine.h"
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
//#endif
#include "ISMath.h"
#include <ISHvkD3dInterop.h>

/*//#include <dxgi.h>
#include "ISHavok.h"
//#include "ISMath.h"*/
#include <d3dx11effect.h>


//-----------------------------------------------------------------------------
// D3D 11 TYPEDEFS
//-----------------------------------------------------------------------------
typedef D3DX11_IMAGE_FILE_FORMAT	D3DX_IMAGE_FILEFORMAT;


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_DIRECT3D
	#define DIRECT3D_EXTERN_FUNC		__declspec(dllexport)
#else
	#define DIRECT3D_EXTERN_FUNC		__declspec(dllimport)
#endif
#ifndef D3DERR_DEVICELOST
#define D3DERR_DEVICELOST	MAKE_D3DHRESULT(2152) // EDIT: D3DERR_DEVICELOST is D3D9
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
enum D3dShapeId
{
	DSID_MESH, DSID_PLANE, DSID_BOX, DSID_SPHERE, DSID_CAPSULE, DSID_CYLINDER, DSID_CONE
};

//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct D3dStartupSettings
{
	bool isfullscreen;
};
struct Frustum
{
	Plane pleft, pright, ptop, pbottom, pnear, pfar;
};
struct D3DMaterial
{
	Color ambient, diffuse, specular;
	float power;
	ITexture* tex;
	bool operator==(const D3DMaterial& other) const
		{return this->tex == other.tex && this->power == other.power && this->ambient == other.ambient && this->diffuse == other.diffuse && this->specular == other.specular;}
};
 struct Sprite
{
	Matrix worldmatrix;
	Vector2 texcoord;
	Vector2 texsize;
	Color color;
	ITexture* tex;
	Sprite() : texcoord(0.0f, 0.0f), texsize(1.0f, 1.0f), color(1.0f, 1.0f, 1.0f, 1.0f)
	{
		MatrixIdentity(&worldmatrix);
	}
};
struct FontType
{
	String family;
	float size;
	bool bold, italic;
	FontType(const String family, float size, bool bold = false, bool italic = false) : family(family), size(size), bold(bold), italic(italic) {}
	/*bool operator==(const FontMapId& other) const
		{return this->bold == other.bold && this->italic == other.italic && this->size == other.size && this->family.Equals(other.family);}*/
	bool operator<(const FontType& other) const
	{
		char cmp1 = this->bold - other.bold;
		char cmp2 = this->italic - other.italic;
		float cmp3 = this->size - other.size;
		return (cmp1 < 0) || (!cmp1 && cmp2 < 0) || (!cmp1 && !cmp2 && cmp3 < 0) || (!cmp1 && !cmp2 && !cmp3 && (other.family < this->family));
	}
};
struct D3dShapeDesc
{
protected:
	const D3dShapeId id;
	D3dShapeDesc(D3dShapeId id) : id(id), texcoordscale(1.0f, 1.0f) {}
public:
	Vector2 texcoordscale;
	D3dShapeId GetId() const {return id;}
	virtual UINT GetVertexCount() const = 0;
	virtual UINT GetFaceCount() const = 0;
	virtual D3dShapeDesc* Copy() const = 0;
};
struct D3dMeshShapeDesc : public D3dShapeDesc
{
	D3dMeshShapeDesc() : D3dShapeDesc(DSID_MESH), numvertices(0), numfaces(0) {}
	FilePath filename;
	UINT numvertices, numfaces; // Will be set by Object::LoadX

	UINT GetVertexCount() const {return numvertices;}
	UINT GetFaceCount() const {return numfaces;}
	D3dShapeDesc* Copy() const {return new D3dMeshShapeDesc(*this);}
};
struct D3dPlaneShapeDesc : public D3dShapeDesc
{
	D3dPlaneShapeDesc() : D3dShapeDesc(DSID_PLANE), pos(0.0f, 0.0f, 0.0f), nml(0.0f, 1.0f, 0.0f), size(1.0f, 1.0f) {}
	Vector3 pos, nml;
	Vector2 size;

	UINT GetVertexCount() const {return 4;}
	UINT GetFaceCount() const {return 2;}
	D3dShapeDesc* Copy() const {return new D3dPlaneShapeDesc(*this);}
};
struct D3dBoxShapeDesc : public D3dShapeDesc
{
	D3dBoxShapeDesc() : D3dShapeDesc(DSID_BOX) {}
	Vector3 pos, size;

	UINT GetVertexCount() const {return 24;}
	UINT GetFaceCount() const {return 12;}
	D3dShapeDesc* Copy() const {return new D3dBoxShapeDesc(*this);}
};
struct D3dSphereShapeDesc : public D3dShapeDesc
{
	D3dSphereShapeDesc() : D3dShapeDesc(DSID_SPHERE), pos(0.0f, 0.0f, 0.0f), rot_y(0.0f), radius(1.0f), stacks(8), slices(8), theta(PI), phi(2.0f * PI) {}
	Vector3 pos;
	float rot_y, radius;
	UINT stacks, slices;
	float theta, phi;

	UINT GetVertexCount() const {return (slices + (UINT)(phi != 2.0f * PI)) * (stacks + 1);}
	UINT GetFaceCount() const {return slices * stacks * 2;}
	D3dShapeDesc* Copy() const {return new D3dSphereShapeDesc(*this);}
};
struct D3dCapsuleShapeDesc : public D3dShapeDesc
{
	D3dCapsuleShapeDesc() : D3dShapeDesc(DSID_CAPSULE) {}
	Vector3 v0, v1;
	float radius;
	UINT stacks, slices;

	UINT GetVertexCount() const {return slices * (stacks + 2);}
	UINT GetFaceCount() const {return slices * (stacks + 1) * 2;}
	D3dShapeDesc* Copy() const {return new D3dCapsuleShapeDesc(*this);}
};
struct D3dCylinderShapeDesc : public D3dShapeDesc
{
	D3dCylinderShapeDesc() : D3dShapeDesc(DSID_CYLINDER), hasfrontfaces(true) {}
	Vector3 v0, v1;
	float radius;
	UINT slices;
	bool hasfrontfaces;

	UINT GetVertexCount() const {return (hasfrontfaces ? 4 : 2) * slices;}
	UINT GetFaceCount() const {return 2 * slices + (hasfrontfaces ? 2 * (slices - 2) : 0);}
	D3dShapeDesc* Copy() const {return new D3dCylinderShapeDesc(*this);}
};
struct D3dConeShapeDesc : public D3dShapeDesc
{
	D3dConeShapeDesc() : D3dShapeDesc(DSID_CONE), hasgroundfaces(true) {}
	Vector3 v0, v1;
	float radius;
	UINT slices;
	bool hasgroundfaces;

	UINT GetVertexCount() const {return (hasgroundfaces ? 3 : 2) * slices;}
	UINT GetFaceCount() const {return 2 * slices + (hasgroundfaces ? slices - 2 : 0);}
	D3dShapeDesc* Copy() const {return new D3dConeShapeDesc(*this);}
};

struct IEffectScalarVariable
{
	virtual void SetBool(BOOL b) = 0;
	virtual void SetBoolArray(BOOL* b, UINT count) = 0;
	virtual void SetFloat(float f) = 0;
	virtual void SetFloatArray(float* f, UINT count) = 0;
	virtual void SetInt(int i) = 0;
	virtual void SetIntArray(int* i, UINT count) = 0;
};
struct IEffectVectorVariable
{
	virtual void SetVector(Vector3& v) = 0;
	virtual void SetVectorArray(Vector3* v, UINT count) = 0;
};
struct IEffectMatrixVariable
{
	virtual void SetMatrix(Matrix& m) = 0;
	virtual void SetMatrixArray(Matrix* m, UINT count) = 0;
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
	virtual LPOBJECT GetObject() {return NULL;}
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

	virtual UINT GetWidth() = 0;
	virtual UINT GetHeight() = 0;
	virtual UINT GetNumMipLevels() = 0;
	virtual FilePath GetFilename() = 0;
	virtual Result Lock(D3D11_MAPPED_SUBRESOURCE* data) = 0;
	virtual void Unlock() = 0;
	virtual Result MapPixelData(bool read, bool write, UINT* stride, LPVOID* data) = 0;
	virtual void UnmapData() = 0;
	virtual Result Save(const FilePath& filename, D3DX_IMAGE_FILEFORMAT format) = 0;
	virtual Result Save(LPD3D10BLOB* datablob, D3DX_IMAGE_FILEFORMAT format) = 0;
	virtual ID3D11Texture2D* GetD3DTexture() = 0;
	virtual void Release() = 0;
}* LPTEXTURE;

//-----------------------------------------------------------------------------
// Name: IRenderShader
// Desc: Interface to the RenderShader class
//-----------------------------------------------------------------------------
typedef class IRenderShader
{
public:
	virtual LPD3D11EFFECTVARIABLE GetVariableBySemantic(LPCSTR semantic) = 0;
	virtual LPD3D11EFFECTVARIABLE GetVariableByName(LPCSTR name) = 0;
	virtual IEffectScalarVariable* GetScalarVariable(const String& semantic) = 0;
	virtual IEffectVectorVariable* GetVectorVariable(const String& semantic) = 0;
	virtual IEffectMatrixVariable* GetMatrixVariable(const String& semantic) = 0;
	virtual Result SetTechnique(UINT techidx) = 0;
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
	std::vector<D3DMaterial>* mats;

	virtual int GetUnderlyingShapeCount() = 0;
	virtual const D3dShapeDesc* GetUnderlyingShape(int shapeidx = 0) = 0;
	virtual float RayTrace(const Vector3* pos, const Vector3* dir, Vector3* normal = NULL, UINT* subset = NULL, UINT* blendpart = NULL) = 0;
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
// Name: ILight
// Desc: Interface to the Light class
//-----------------------------------------------------------------------------
typedef class ILight : public IMoveable
{
public:
	enum LightType
		{LT_DIRECTIONAL, LT_POINT, LT_SPOT};

	virtual const Vector3& GetPos() = 0;
	virtual void SetPos(const Vector3& val) = 0;
	__declspec(property(get=GetPos, put=SetPos)) const Vector3& Pos;
	virtual const Vector3& GetDir() = 0;
	virtual void SetDir(const Vector3& val) = 0;
	__declspec(property(get=GetDir, put=SetDir)) const Vector3& Dir;
	virtual const PARENT_NAMESPACE::Color& GetColor() = 0;
	virtual void SetColor(const PARENT_NAMESPACE::Color& val) = 0;
	__declspec(property(get=GetColor, put=SetColor)) const PARENT_NAMESPACE::Color& Color;
	virtual LightType GetType() = 0;
	virtual void SetType(LightType val) = 0;
	__declspec(property(get=GetType, put=SetType)) LightType Type;
	virtual const Vector3& GetAttenuation() = 0;
	virtual void SetAttenuation(const Vector3& val) = 0;
	__declspec(property(get=GetAttenuation, put=SetAttenuation)) const Vector3& Attenuation;
	virtual float GetSpotPower() = 0;
	virtual void SetSpotPower(float val) = 0;
	__declspec(property(get=GetSpotPower, put=SetSpotPower)) float SpotPower;

	virtual bool GetEnabled() = 0;
	virtual void SetEnabled(bool val) = 0;
	__declspec(property(get=GetEnabled, put=SetEnabled)) bool Enabled;

	virtual void Release() = 0;
}* LPLIGHT;

//-----------------------------------------------------------------------------
// Name: ISpriteContainer
// Desc: Interface to the Object class
//-----------------------------------------------------------------------------
typedef class ISpriteContainer : public IRenderable
{
public:
	virtual void SetViewTransform(const Matrix* viewmatrix) = 0;
	virtual void SetIdentityViewTransform() = 0;
	virtual void SetCameraViewTransform() = 0;
	virtual void SetProjectionTransform(const Matrix* projmatrix) = 0;
	virtual void SetIdentityProjectionTransform() = 0;
	virtual void SetCameraProjectionTransform() = 0;

	virtual void Begin() = 0;
	virtual void DrawSpriteImmediate(const Sprite* sprite) = 0;
	virtual void End() = 0;

	virtual void DrawSpriteDelayed(const Sprite* sprite) = 0;

	virtual void Release() = 0;
}* LPSPRITECONTAINER;

//-----------------------------------------------------------------------------
// Name: ID3dFont
// Desc: Interface to the Font class
//-----------------------------------------------------------------------------
typedef class ID3dFont
{
public:
	static const UINT DF_NOCLIP = 0x1;
	static const UINT DF_SINGLELINE = 0x2;
	static const UINT DF_BREAKWORDS = 0x4;

	virtual const FontType& GetType() = 0;
	virtual void DrawTextImmediate(LPSPRITECONTAINER spritecontainer, const String text, const Rect<float>& bounds, const Color& clr, UINT flags) = 0;
	virtual void DrawTextDelayed(LPSPRITECONTAINER spritecontainer, const String text, const Rect<float>& bounds, const Color& clr, UINT flags) = 0;
	virtual void MeasureText(const String text, float fixedwidth, UINT flags, Size<float>* measuredsize) = 0;
	virtual void Release() = 0;
}* LPD3DFONT;

//-----------------------------------------------------------------------------
// Name: ISceneNode
// Desc: Interface to the SceneNode class
//-----------------------------------------------------------------------------
typedef class ISceneNode : public IRenderable, public IUpdateable
{
public:
	enum TransformSpace
		{TS_LOCAL, TS_PARENT, TS_WORLD};

	virtual void AddChild(ISceneNode* node) = 0;
	virtual void AddMoveable(IMoveable* moveable) = 0;
	virtual void AddRenderable(IRenderable* renderable, RenderType rendertype = RT_DEFAULT) = 0;
	virtual Result GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype) = 0;

	virtual void Translate(const Vector3* deltapos, TransformSpace ts = TS_PARENT) = 0;
	virtual void TranslateX(const float deltapos, TransformSpace ts = TS_PARENT) = 0;
	virtual void TranslateY(const float deltapos, TransformSpace ts = TS_PARENT) = 0;
	virtual void TranslateZ(const float deltapos, TransformSpace ts = TS_PARENT) = 0;
	virtual void SetPosition(const Vector3* pos, TransformSpace ts = TS_PARENT) = 0;
	virtual void SetScale(const Vector3* scale) = 0;
	virtual void RotateX(float angle) = 0;
	virtual void RotateY(float angle) = 0;
	virtual void RotateZ(float angle) = 0;
	inline void Yaw(float angle) {RotateY(angle);}
	inline void Pitch(float angle) {RotateX(angle);}
	inline void Roll(float angle) {RotateZ(angle);}
	virtual void RotateAxis(const Vector3* axis, float angle) = 0;
	virtual void RotateQuaternion(const Quaternion* qrot) = 0;
	virtual void SetOrientation(const Quaternion* qrot, TransformSpace ts = TS_PARENT) = 0;
	virtual Quaternion* GetOrientation(Quaternion* qrot, TransformSpace ts = TS_PARENT) = 0;
	virtual Matrix* GetTransform(Matrix* mout, TransformSpace ts = TS_PARENT) = 0;

	virtual Vector3* TransformToParentSpace(Vector3* vout, const Vector3* v0, TransformSpace ts) = 0;
	virtual Vector3* TransformToLocalSpace(Vector3* vout, const Vector3* v0, TransformSpace ts) = 0;
}* LPSCENENODE;

//-----------------------------------------------------------------------------
// Name: ISceneManager
// Desc: Interface to the SceneManager class
//-----------------------------------------------------------------------------
typedef class ISceneManager : public IRenderable, public IUpdateable
{
public:
	virtual ISceneNode* GetRoot() = 0;
	virtual ISceneNode* CreateNode() = 0;
	virtual ISceneNode* CloneNode(ISceneNode* srcnode) = 0;
	virtual Result GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype) = 0;
	virtual void Release() = 0;
}* LPSCENEMANAGER;

//-----------------------------------------------------------------------------
// Name: ISkyBox
// Desc: Interface to the SkyBox class
//-----------------------------------------------------------------------------
typedef class ISkyBox : public IRenderable
{
public:
	virtual Result CreateLayer(const FilePath& north, const FilePath& east, const FilePath& south, const FilePath& west, const FilePath& top, const FilePath& bottom, UINT distance) = 0;
}* LPSKYBOX;

//-----------------------------------------------------------------------------
// Name: IBoxedLevel
// Desc: Interface to the BoxedLevel class
//-----------------------------------------------------------------------------
typedef class IBoxedLevel : public IRenderable
{
public:
	virtual void SetBoxTypes(BoxedLevelBoxType* boxtypes, UINT32 numtypes) = 0;

	virtual void AssignChunk(BoxedLevelChunk* chunk) = 0;
	virtual void UnassignChunk(BoxedLevelChunk* chunk) = 0;
	virtual void UpdateChunk(BoxedLevelChunk* chunk, const UINT (&boxpos)[3]) = 0;
	virtual void UpdateChunk(BoxedLevelChunk* chunk, const UINT (&boxpos_min)[3], const UINT (&boxpos_max)[3]) = 0;
	virtual void GetAssignedChunks(std::list<BoxedLevelChunk*>* chunks) = 0;
	virtual void GetChunkSize(UINT (*chunksize)[3]) = 0;
	virtual LPTEXTURE GetTexture() = 0;
	virtual std::shared_ptr<void> GetPyPtr() = 0;
}* LPBOXEDLEVEL;

//-----------------------------------------------------------------------------
// Name: ILandscape
// Desc: Interface to the Landscape class
//-----------------------------------------------------------------------------
/*typedef class ILandscape : public IRenderable
{
public:
}* LPLANDSCAPE;*/ //EDIT11

//-----------------------------------------------------------------------------
// Name: ILaser
// Desc: Interface to the Laser class
//-----------------------------------------------------------------------------
typedef class ILaser : public IRenderable
{
public:
	virtual void Update() = 0;
}* LPLASER;

//-----------------------------------------------------------------------------
// Name: IHUD
// Desc: Interface to the HUD class
//-----------------------------------------------------------------------------
typedef class IHUD : public IRenderable
{
public:
	enum ElementType {ET_IMAGE, ET_TEXT};
	typedef struct IElement
	{
		bool visible;
		Color color;
		const int zorder;
		const ElementType type;
		LPVOID user;

		virtual int GetX() = 0;
		virtual int SetX(int val) = 0;
		__declspec(property(get=GetX, put=SetX)) int x;
		virtual int GetY() = 0;
		virtual int SetY(int val) = 0;
		__declspec(property(get=GetY, put=SetY)) int y;
		virtual DockType GetDock() = 0;
		virtual DockType SetDock(DockType val) = 0;
		__declspec(property(get=GetDock, put=SetDock)) DockType dock;

		IElement(int zorder, ElementType type) : zorder(zorder), type(type) {}
		struct CompareFunctor
		{
			bool operator()(const IElement* left, const IElement* right) const
			{return left->zorder < right->zorder;}
		};
	}* LPELEMENT;
	typedef struct IImageElement : public IElement
	{
		IImageElement(int zorder) : IElement(zorder, ET_IMAGE) {}
	}* LPIMAGEELEMENT;
	typedef struct ITextElement : public IElement
	{
		String text;
		LPD3DFONT font;
		UINT textflags;

		ITextElement(int zorder) : IElement(zorder, ET_TEXT) {}
	}* LPTEXTELEMENT;

	typedef void (__stdcall* MOUSEEVENT_CALLBACK)(LPELEMENT element);

	virtual Result CreateElement(ITexture* tex, int x, int y, DockType dock, Color color, int zorder = 0, LPIMAGEELEMENT* element = NULL, LPVOID elementuser = NULL) = 0;
	virtual Result CreateElement(ITexture* tex, int x, int y, DockType dock, Color color, const Rect<int>& subregion, int zorder = 0, LPIMAGEELEMENT* element = NULL, LPVOID elementuser = NULL) = 0;
	virtual Result CreateElement(String text, LPD3DFONT font, int x, int y, DockType dock, Color color, const Size<int>& size,
								 int zorder = 0, UINT textflags = 0, LPTEXTELEMENT* element = NULL, LPVOID elementuser = NULL) = 0;
	virtual void RemoveElement(LPELEMENT element) = 0;
	virtual void RegisterMouseDownListener(IElement* element, MOUSEEVENT_CALLBACK cbk) = 0;
	virtual void RegisterMouseUpListener(IElement* element, MOUSEEVENT_CALLBACK cbk) = 0;
	virtual void RegisterMouseMoveListener(IElement* element, MOUSEEVENT_CALLBACK cbk) = 0;
	virtual void RegisterMouseEnterListener(IElement* element, MOUSEEVENT_CALLBACK cbk) = 0;
	virtual void RegisterMouseLeaveListener(IElement* element, MOUSEEVENT_CALLBACK cbk) = 0;
	virtual void Update() = 0;
	virtual LPSPRITECONTAINER GetSpriteContainer() = 0;
	virtual void Release() = 0;
}* LPHUD;

//-----------------------------------------------------------------------------
// Name: ID3dCursor
// Desc: Interface to the D3dCursor class
//-----------------------------------------------------------------------------
typedef class ID3dCursor : public IRenderable
{
public:
	LPTEXTURE tex;
	bool animated;
	Point<> hotspot;

	virtual void Release() = 0;
}* LPD3DCURSOR;

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
		HWND wnd;
		bool enablemultisampling;
		int screenidx;
	};
	const bool ismultisampled;
	const int screenidx;

	Color backcolor;
	bool backcolorenabled;

	virtual Size<UINT> GetBackbufferSize() = 0;
	virtual UINT GetBackbufferWidth() = 0;
	virtual UINT GetBackbufferHeight() = 0;
	virtual HWND GetHwnd() = 0;
	virtual ITexture* GetBackbuffer() = 0;
	virtual ITexture* SetBackbuffer(ITexture* texbackbuffer) = 0;
	__declspec(property(get=GetBackbuffer, put=SetBackbuffer)) ITexture* Backbuffer;
	virtual Result Resize(UINT width, UINT height) = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void RegisterForRendering(RENDERFUNC func, RenderType rendertype, LPVOID user = NULL) = 0;
	virtual void RegisterForRendering(IRenderable* cls, RenderType rendertype) = 0;
	virtual void DeregisterFromRendering(RENDERFUNC func, RenderType rendertype) = 0;
	virtual void DeregisterFromRendering(IRenderable* cls, RenderType rendertype) = 0;
	virtual void DeregisterFromRendering(IRenderable* cls) = 0;
	virtual void RegisterForUpdating(IUpdateable* cls) = 0;
	virtual void DeregisterFromUpdating(IUpdateable* cls) = 0;
	virtual void SortRegisteredClasses(RenderType rendertype, const Vector3& campos) = 0;
	virtual Result GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype) = 0;
	virtual void DrawLine(const Vector3& v0, const Vector3& v1, Color c0, Color c1, RenderType rendertype = RT_DEFAULT) = 0;
	virtual void DrawLine(const Vector2& v0, const Vector2& v1, Color c0, Color c1, RenderType rendertype = RT_DEFAULT) = 0;
	virtual void DrawSprite(const Sprite* sprite) = 0;
	virtual Result EnableSprites(const FilePath spriteshaderfilename) = 0;
	virtual Result EnableLineDrawing(const FilePath lineshaderfilename) = 0;
	virtual void ShowFps() = 0;
	virtual void HideFps() = 0;
	virtual void TakeScreenShot(const FilePath& filename, D3DX_IMAGE_FILEFORMAT fileformat) = 0;
	virtual void SetCamera(ICamera* cam) = 0;
	virtual ICamera* GetCamera() const = 0;
	virtual D3D11_VIEWPORT& GetViewport() = 0;
	virtual ID3D11Device* GetDevice() const = 0;
	virtual ID3D11DeviceContext* GetDeviceContext() const = 0;
	virtual Result MakeConsoleTarget(UINT numlines, UINT historysize) = 0;
	virtual Result CreateTexture(const FilePath& filename, ITexture::Usage usage, bool deviceindependent, ITexture** tex) = 0;
	virtual Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, ITexture** tex) = 0;
	virtual Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, LPVOID data, ITexture** tex) = 0;
	virtual Result CreateTexture(ITexture* source, ITexture::Usage usage, bool deviceindependent, ITexture** tex) = 0;
	virtual Result CreateRenderShader(const FilePath filename, IRenderShader** shader) = 0;
	virtual Result CreateObject(const FilePath& filename, bool loadtextures, IObject** obj) = 0;
	virtual Result CreateObject(IObject* srcobj, bool newmaterials, IObject** obj) = 0;
	virtual Result CreateObject(UINT numvertices, UINT numfaces, IObject** obj) = 0;
	virtual Result CreateObject(const D3dShapeDesc& shapedesc, IObject** obj) = 0;
	virtual Result CreateObject(const D3dShapeDesc* shapedescs[], UINT numshapedescs, IObject** obj) = 0;
	virtual void RemoveObject(IObject* obj) = 0;
	virtual void RemoveAllObjects() = 0;
	virtual Result CreateHUD(IHUD** hud) = 0;
	virtual Result CreateCursor(ID3dCursor** cursor) = 0;
	virtual Result CreateSpriteContainer(UINT buffersize, ISpriteContainer** spc) = 0;
	virtual Result CreateD3dFont(const FontType& type, ID3dFont** font) = 0;
	virtual Result CreateSceneManager(ISceneManager** mgr) = 0;
	virtual Result CreateSkyBox(ISkyBox** sky) = 0;
	virtual Result CreateBoxedLevel(const UINT (&chunksize)[3], UINT numchunkbuffers, LPTEXTURE texboxes, LPRENDERSHADER blevelshader, IBoxedLevel** blevel) = 0;
/*	virtual Result CreateLandscape(FilePath& filename, LPRENDERSHADER shader, ILandscape** landscape) = 0;*/ //EDIT11
	virtual void Release() = 0;

	IOutputWindow(const Settings& wndsettings) : ismultisampled(wndsettings.enablemultisampling), screenidx(wndsettings.screenidx) {}
}* LPOUTPUTWINDOW;

//-----------------------------------------------------------------------------
// Name: IWindowlessDevice
// Desc: Interface to the WindowlessDevice class
//-----------------------------------------------------------------------------
typedef class IWindowlessDevice
{
public:
	virtual Result AddSourceTexture(ITexture* tex, String& texsemantic, String& sizesemantic) = 0;
	virtual Result SetDestinationTexture(ITexture* tex, String& sizesemantic) = 0;
	virtual Result SetShader(IRenderShader* shader) = 0;
	virtual void Render(float bg_r, float bg_g, float bg_b) = 0;
	virtual void Render() = 0;
	virtual Result CreateTexture(FilePath& filename, ITexture::Usage usage, bool deviceindependent, ITexture** tex) = 0;
	virtual Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, ITexture** tex) = 0;
	virtual void RemoveTexture(ITexture* tex) = 0;
	virtual Result CreateRenderShader(FilePath& filename, IRenderShader** shader) = 0;
}* LPWINDOWLESSDEVICE;

//-----------------------------------------------------------------------------
// Name: IDirect3D
// Desc: Interface to the Direct3D class
//-----------------------------------------------------------------------------
typedef class IDirect3D
{
public:
	virtual bool IsFullscreen() const = 0;

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init(const D3dStartupSettings* settings) = 0;
	virtual void Render() = 0;
	virtual bool CastRayTo(const Vector3& src, const Vector3& dest, float* dist) = 0;
	virtual bool CastRayDir(const Vector3& src, const Vector3& dir, float* dist) = 0;
	virtual void TakeScreenShots(const FilePath& filename, D3DX_IMAGE_FILEFORMAT fileformat) = 0;
	virtual void GetEnabledLights(std::list<ILight*>* lights) = 0;
	virtual void RemoveAllLights() = 0;
	virtual const std::list<IOutputWindow*>& GetOutputWindows() const = 0;
	virtual Result CreateOutputWindow(IOutputWindow::Settings* settings, IOutputWindow** wnd) = 0;
	virtual Result CreateWindowlessDevice(IWindowlessDevice** wldev) = 0;
	virtual Result CreateCamera(float fov, float clipnear, float clipfar, ICamera** cam) = 0;
	virtual Result CreateLight(ILight::LightType type, ILight** light) = 0;
	virtual void Release() = 0;
}* LPDIRECT3D;

extern "C" DIRECT3D_EXTERN_FUNC LPDIRECT3D __cdecl CreateDirect3D();
extern "C" DIRECT3D_EXTERN_FUNC bool __cdecl CheckDirect3DSupport(LPTSTR* missingdllname);
extern "C" DIRECT3D_EXTERN_FUNC void __cdecl RegisterScriptableDirect3DClasses(const IPythonScriptEngine* pse);

#endif