#ifndef __DIRECT3D_H
#define __DIRECT3D_H

#include "ISDirect3D.h"
#include "..\\global\\HResult.h"
#include <map>
#include <vector>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "D3DCompiler.lib") // For compiling Direct3D 11 effects
#ifdef _DEBUG
	#pragma comment (lib, "d3dx11d.lib")
	#pragma comment (lib, "Effects11d.lib")
	#pragma comment (lib, "ISMath_d.lib")
#else
	#pragma comment (lib, "d3dx11.lib")
	#pragma comment (lib, "Effects11.lib")
	#pragma comment (lib, "ISMath.lib")
#endif

struct _object;
#ifndef PyObject
typedef _object PyObject;
#endif


//-----------------------------------------------------------------------------
// D3D 10 TYPEDEFS
//-----------------------------------------------------------------------------
typedef ID3D11Device				ID3DDevice, *LPD3DDEVICE;
typedef ID3D11DeviceContext			ID3DDeviceContext, *LPD3DDEVICECONTEXT;
typedef ID3D11Texture2D				ID3DTexture2D, *LPD3DTEXTURE2D;
/*typedef ID3DX10Font					ID3DXFont, *LPD3DXFONT;
#define D3DXCreateFont				D3DX10CreateFont*/


struct RenderQuad
{
	static D3D11_INPUT_ELEMENT_DESC VTX_DECL[2];

	struct VertexFormat
	{
		Vector3 pos; float tu, tv;
		void SetFormat(Vector3& pos, float tu, float tv)
		{
			this->pos = pos;
			this->tu = tu;
			this->tv = tv;
		}
	};
	VertexFormat vertices[4];
	UINT indices[6];
	ID3D11Buffer *vb, *ib;

	RenderQuad()
	{
		vertices[0].SetFormat(Vector3(-1.0f, -1.0f, 0.0f), 0.0f, 1.0f);
		vertices[1].SetFormat(Vector3(-1.0f,  1.0f, 0.0f), 0.0f, 0.0f);
		vertices[2].SetFormat(Vector3( 1.0f, -1.0f, 0.0f), 1.0f, 1.0f);
		vertices[3].SetFormat(Vector3( 1.0f,  1.0f, 0.0f), 1.0f, 0.0f);
		indices[0] = 0; indices[1] = 1; indices[2] = 2; indices[3] = 1; indices[4] = 3; indices[5] = 2;
		UINT ids[] = {0, 1, 2, 1, 3, 2};
		vb = ib = NULL;
	}
	~RenderQuad()
	{
		vb->Release();
		ib->Release();
	}

	Result Init(LPD3DDEVICE device);
	Result PrepareRendering(LPD3DDEVICECONTEXT devicecontext);
};

struct EffectScalarVariable : public IEffectScalarVariable
{
private:
	LPD3D11EFFECTSCALARVARIABLE var;
public:
	EffectScalarVariable(LPD3D11EFFECTSCALARVARIABLE var) : var(var) {}
	void SetBool(BOOL b) {var->SetBool(b);}
	void SetBoolArray(BOOL* b, UINT count) {var->SetBoolArray(b, 0, count);}
	void SetFloat(float f) {var->SetFloat(f);}
	void SetFloatArray(float* f, UINT count) {var->SetFloatArray(f, 0, count);}
	void SetInt(int i) {var->SetInt(i);}
	void SetIntArray(int* i, UINT count) {var->SetIntArray(i, 0, count);}
};
struct EffectVectorVariable : public IEffectVectorVariable
{
private:
	LPD3D11EFFECTVECTORVARIABLE var;
public:
	EffectVectorVariable(LPD3D11EFFECTVECTORVARIABLE var) : var(var) {}
	void SetVector(Vector3& v) {var->SetFloatVector(v);}
	void SetVectorArray(Vector3* v, UINT count) {var->SetFloatVectorArray(*v, 0, count);}
};
struct EffectMatrixVariable : public IEffectMatrixVariable
{
private:
	LPD3D11EFFECTMATRIXVARIABLE var;
public:
	EffectMatrixVariable(LPD3D11EFFECTMATRIXVARIABLE var) : var(var) {}
	void SetMatrix(Matrix& m) {var->SetMatrix(m);}
	void SetMatrixArray(Matrix* m, UINT count) {var->SetMatrixArray(*m, 0, count);}
};


class OutputWindow;
class Camera;

//-----------------------------------------------------------------------------
// Name: Texture
// Desc: Class holding data, properties and methodes of a single 2D texture
//-----------------------------------------------------------------------------
class Texture : public ITexture
{
private:
	OutputWindow* wnd;

public:
	struct Pixel
	{
		BYTE r, g, b, a;
		bool operator==(const Pixel& other) const
			{return *(UINT32*)this == *(UINT32*)&other;}
	};
	std::vector<Pixel>* mappedpixeldata;

	ID3D11Texture2D* tex;
	ID3D11ShaderResourceView* srview;
	ID3D11RenderTargetView* rtview;
	FilePath filename;
	UINT width, height, miplevels;

	UINT GetWidth() {return width;}
	UINT GetHeight() {return height;}
	UINT GetNumMipLevels() {return miplevels;}
	FilePath GetFilename() {return filename;}

	Texture();

	void CreateEmpty(OutputWindow* wnd) {this->wnd = wnd;}
	Result CreateNew(OutputWindow* wnd, UINT width, UINT height, Usage usage,
					 bool deviceindependent, DXGI_FORMAT format, UINT miplevels = 0xFFFFFFFF, LPVOID data = NULL);
	Result CreateCopy(Texture* source, Usage usage, bool deviceindependent);
	Result Load(OutputWindow* wnd, const FilePath& filename, Usage usage, bool deviceindependent);
	Result Lock(D3D11_MAPPED_SUBRESOURCE* data); // Deprecated
	void Unlock(); // Deprecated
	Result MapPixelData(bool read, bool write, UINT* stride, LPVOID* data);
	std::vector<Pixel>* MapPixelDataWrapper(bool read, bool write)
	{
		Result rlt;
		Pixel* data;
		IFFAILED(MapPixelData(read, write, NULL, (LPVOID*)&data))
			throw rlt.GetLastResult();
		return mappedpixeldata = new std::vector<Pixel>(data, data + (width * height));
	}
	void UnmapData();
	void UnmapDataWrapper()
	{
		UnmapData();
		delete mappedpixeldata;
	}
	Result Save(const FilePath& filename, D3DX_IMAGE_FILEFORMAT format);
	Result Save(LPD3D10BLOB* datablob, D3DX_IMAGE_FILEFORMAT format);
	ID3D11Texture2D* GetD3DTexture() {return tex;}
	void Render(LPD3DDEVICE device);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: RenderShader
// Desc: Class holding a single SM 4.0 shader for rendering
//-----------------------------------------------------------------------------
class RenderShader : public IRenderShader
{
private:
	LPD3D11EFFECT effect;
	LPD3D11EFFECTTECHNIQUE tech;
	LPD3DDEVICE device;
	LPD3DDEVICECONTEXT devicecontext;
	D3D11_INPUT_ELEMENT_DESC* vtxdecl;
	ID3D11InputLayout **layouts;
	UINT vtxdeclcount, passcount;

	Result ReCreateInputLayouts();

public:
	struct DefaultParameters
	{
		LPD3D11EFFECTMATRIXVARIABLE world, worldarray, view, proj, viewproj, worldviewproj, worldviewprojarray, worldinvtrans, worldinvtransarray; // Matrices
		LPD3D11EFFECTVECTORVARIABLE viewpos, ambient, diffuse, specular; // Vectors
		LPD3D11EFFECTSCALARVARIABLE numlights; // Light parameters
		LPD3D11EFFECTVARIABLE lightparams; // Light parameters
		LPD3D11EFFECTSCALARVARIABLE power, time, hastex; // Misc
		LPD3D11EFFECTSHADERRESOURCEVARIABLE tex; // Textures
	} defparams;

	RenderShader();

	Result LoadFX(const FilePath filename, LPD3DDEVICE device, LPD3DDEVICECONTEXT devicecontext);
	LPD3D11EFFECTVARIABLE GetVariableBySemantic(LPCSTR semantic) {return effect ? effect->GetVariableBySemantic(semantic) : NULL;}
	LPD3D11EFFECTVARIABLE GetVariableByName(LPCSTR name) {return effect ? effect->GetVariableByName(name) : NULL;}
	IEffectScalarVariable* GetScalarVariable(const String& semantic);
	IEffectVectorVariable* GetVectorVariable(const String& semantic);
	IEffectMatrixVariable* GetMatrixVariable(const String& semantic);
	void PrepareRendering(Camera* cam, const Matrix* worldmatrix, const Matrix* worldmatrixarray, UINT numblendparts, UINT* passcount);
	void PrepareRendering(UINT* passcount);
	void PreparePass(UINT passindex, UINT techidx = 0xFFFFFFFF);
	void PrepareSubset(const Texture* tex, const Color* ambient, const Color* diffuse, const Color* specular, float power, UINT passindex);
	void ApplyStateChanges(UINT passindex = (UINT)-1);
	Result SetTechnique(UINT techidx);
	Result SetVtxDecl(D3D11_INPUT_ELEMENT_DESC* vtxdecl, UINT vtxdeclcount);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Object
// Desc: Class holding data, properties and methodes of a single 3D mesh
//-----------------------------------------------------------------------------
class Mesh;
struct MeshBuffer;
class Object : public IObject
{
private:
	UINT* refcounter, *matrefcounter;
	Mesh* mesh;
	OutputWindow* wnd;
	RenderShader* shader;
	bool visible;

	BoundingSphere boundingsphere;
	BoundingBox boundingbox;

	Vector3 *bonepos;
	UINT numblendparts;

	const D3dShapeDesc** underlyingshapes;
	int numunderlyingshapes;

	RENDER_CALLBACK beforerendercbk;
	LPVOID beforerendercbkuser;

	std::map<LPVOID, MeshBuffer*> mappedbuffers;

	void ComputeBoundingSphere(IObject::Vertex* vertices, UINT numvertices);
	Result AddShape(const D3dShapeDesc& shapedesc, IObject::Vertex* vertices, UINT* indices, UINT startidx);

public:

	Object();

	bool GetRefPos(Vector3& pos) const;
	LPOBJECT GetObject() {return this;}
	void SetTransform(const Matrix* transform);

	UINT GetNumBones() {return numblendparts;}

	std::vector<D3DMaterial>& GetMatsWrapper() {return *mats;}

	Result LoadX(const FilePath& filename, bool loadtextures, OutputWindow* wnd);
	Result Create(IObject* srcobj, bool newmaterials);
	Result Create(UINT numvertices, UINT numfaces, OutputWindow* wnd);
	Result Create(const D3dShapeDesc& shapedesc, OutputWindow* wnd);
	Result Create(const D3dShapeDesc* shapedescs[], UINT numshapedescs, OutputWindow* wnd);
	int GetUnderlyingShapeCount() {return numunderlyingshapes;}
	const D3dShapeDesc* GetUnderlyingShape(int shapeidx) {return shapeidx < numunderlyingshapes ? underlyingshapes[shapeidx] : NULL;}
	void Render(RenderType rendertype);
	float RayTrace(const Vector3* pos, const Vector3* dir, Vector3* normal, UINT* subset, UINT* blendpart);
	Result CreateCollisionMesh(LPREGULARENTITY* entity);
	Result CreateConvexCollisionHull(LPREGULARENTITY* entity);
	Result CreateCollisionCapsule(float radius, LPREGULARENTITY* entity);
	Result CreateCollisionRagdoll(float radius, bool usedynamicmotion, LPRAGDOLLENTITY* entity);
	std::shared_ptr<void> CreateCollisionRagdollWrapper(float radius, bool usedynamicmotion)
	{
		Result rlt;
		LPRAGDOLLENTITY entity;
		IFFAILED(CreateCollisionRagdoll(radius, usedynamicmotion, &entity))
			throw rlt.GetLastResult();
		return std::shared_ptr<void>((LPVOID)entity);
	}
	Result SetShader(IRenderShader* shader);
	IRenderShader* GetShader() const {return shader;}
	Result SetVertexData(const Vertex* vertices);
	Result SetIndexData(const UINT* indices);
	UINT GetVertexCount();
	UINT GetFaceCount();
	UINT GetIndexCount();
	Result MapVertexData(Vertex** vertices);
	class VertexBuffer : public std::vector<Object::Vertex>
	{
	public:
		VertexBuffer() : std::vector<Object::Vertex>() {}
		template<class _Iter> VertexBuffer(_Iter _First, _Iter _Last) : std::vector<Object::Vertex>(_First, _Last) {}
		PtrWrapper GetPointer() {return PtrWrapper(this->size() ? &this->operator[](0) : NULL);}
		UINT GetVertexCount() {return this->size();}
	};
	class IndexBuffer : public std::vector<UINT>
	{
	public:
		IndexBuffer() : std::vector<UINT>() {}
		template<class _Iter> IndexBuffer(_Iter _First, _Iter _Last) : std::vector<UINT>(_First, _Last) {}
		PtrWrapper GetPointer() {return PtrWrapper(this->size() ? &this->operator[](0) : NULL);}
		UINT GetFaceCount() {return this->size() / 3;}
		UINT GetIndexCount() {return this->size();}
	};
	VertexBuffer* MapVertexDataWrapper()
	{
		Result rlt;
		Vertex* vertices;
		IFFAILED(MapVertexData(&vertices))
			throw rlt.GetLastResult();
		return new VertexBuffer(vertices, vertices + GetVertexCount());
	}
	Result MapIndexData(UINT** indices);
	IndexBuffer* MapIndexDataWrapper()
	{
		Result rlt;
		UINT* indices;
		IFFAILED(MapIndexData(&indices))
			throw rlt.GetLastResult();
		return new IndexBuffer(indices, indices + 3 * GetFaceCount());
	}
	Result UnmapData(LPVOID vertices);
	void UnmapVertexDataWrapper(VertexBuffer* buffer)
	{
		Result rlt = UnmapData(buffer->GetPointer().ptr);
		delete buffer;
		IFFAILED(rlt)
			throw rlt.GetLastResult();
	}
	void UnmapIndexDataWrapper(IndexBuffer* buffer)
	{
		Result rlt = UnmapData(buffer->GetPointer().ptr);
		delete buffer;
		IFFAILED(rlt)
			throw rlt.GetLastResult();
	}
	void GetAttributeTable(AttributeRange* attributetable, UINT* attributetablesize);
	Result SetAttributeTable(const AttributeRange* attributetable, UINT attributetablesize);
	Result CommitChanges();
	Result ComputeNormals();
	Result Split(const Plane* splitplane, LPOBJECT* first, LPOBJECT* second);
	Result Break(int targetnumparts, LPOBJECT* parts, int* numparts);
	const BoundingSphere& GetBoundingSphere() const {return boundingsphere;}
	const Vector3& GetBoundingSphereCenter() const {return boundingsphere.center;}
	float GetBoundingSphereRadius() {return boundingsphere.radius;}
	const BoundingBox& GetBoundingBox() const {return boundingbox;}
	const Vector3& GetBoundingBoxCenter() const {return boundingbox.center;}
	const Vector3& GetBoundingBoxSize() {return boundingbox.size;}
	void SetBeforeRenderCallback(RENDER_CALLBACK cbk, LPVOID user) {beforerendercbk = cbk; beforerendercbkuser = user;}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Camera
// Desc: Interface for view releated parameters and opearations
//-----------------------------------------------------------------------------
class Camera : public ICamera
{
private:
	enum ProjectionType
		{PT_PERSPECTIVE, PT_ORTHOGONAL} ptype;
	Size<float> backbuffersize;
	float aspectratio, fov;
	Frustum viewfrustum;
	Object* playermodel;
	LPRAGDOLLENTITY playerentity;

	void RebuildProjectionMatrix();

public:
	Matrix viewmatrix, projmatrix, viewprojmatrix;
	float clipnear, clipfar, otvwidth, otvheight;
	/*READONLY_BYREF(Matrix, viewmatrix, GetViewMatrix);
	READONLY_BYREF(Matrix, projmatrix, GetProjMatrix);
	READONLY_BYREF(Matrix, viewprojmatrix, GetViewProjMatrix);
	READONLY(float, clipnear, GetClipNear);
	READONLY(float, clipfar, GetClipFar);
	READONLY(float, otvwidth, GetOrthoViewWidth);
	READONLY(float, otvheight, GetOrthoViewHeight);*/

	float GetFOV() const
		{return ptype == PT_PERSPECTIVE ? fov : 0.0f;}

	Camera(float fov, float clipnear, float clipfar);

	void SetTransform(const Matrix* transform);
	void OnOutputWindowResize(Size<UINT> backbuffersize);
	void UpdateMatrices();
	void Update();
	void Render(RenderType rendertype);
	bool DoFrustumCulling(const Matrix* worldmatrix, const Matrix* scalingmatrix, const Matrix* rotationmatrix,
						  const Vector3* center, const Vector3* radius);
	void SetClipNear(float clipnear);
	void SetClipFar(float clipfar);
	void SetPlayerModel(LPOBJECT obj, LPRAGDOLLENTITY ragdoll);
	void SetPerspectiveProjection(float fov);
	void SetOrthogonalProjection(float width, float height);
	Frustum& GetViewFrustum() {UpdateMatrices(); return viewfrustum;}
	Matrix& GetViewMatrix() {return viewmatrix;}
	Matrix& GetProjMatrix() {return projmatrix;}
	Matrix& GetViewProjMatrix() {return viewprojmatrix;}
	Vector3* ComputeViewDir(Vector3* vdir) const;
	Vector3* ComputePixelDir(const Point<int>& pixelpos, Vector3* vdir) const;
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Light
// Desc: Class representing a light source
//-----------------------------------------------------------------------------
class Light : public ILight
{
public:
	#pragma pack(push, 4)
	struct LightParams // Must match corresponding struct in shader
	{
		::Color lightcolor;
		Vector3 lightpos;
		int lighttype;
		Vector3 lightdir;
		int padding0;
		Vector3 lightattenuation;
		float lightspotpower;
	};
	#pragma pack(pop)
private:
	/*mutable*/ std::vector<LightParams>& lightparams;
	LightParams* lightparamsentry;
	LightParams locallightparams; // Used while the light is disabled

	LightParams* CreateLightParamsEntry() const;
	void RemoveLightParamsEntry(Light::LightParams* entry) const;

public:
	bool enabled;

	Light(LightType type);
	void SetTransform(const Matrix* transform);

	const Vector3& GetPos();
	void SetPos(const Vector3& val);
	const Vector3& GetDir();
	void SetDir(const Vector3& val);
	const ::Color& GetColor();
	void SetColor(const ::Color& val);
	LightType GetType();
	void SetType(LightType val);
	const Vector3& GetAttenuation();
	void SetAttenuation(const Vector3& val);
	float GetSpotPower();
	void SetSpotPower(float val);

	bool GetEnabled();
	void SetEnabled(bool val);

	void Release();
};

//-----------------------------------------------------------------------------
// Name: SpriteContainer
// Desc: Class rendering a set of textures by drawing quads via geometry shader
//-----------------------------------------------------------------------------
class SpriteContainer : public ISpriteContainer
{
private:
	struct TextureAndCount
	{
		ID3D11ShaderResourceView* tex;
		UINT count;
		TextureAndCount(ID3D11ShaderResourceView* tex, UINT count) : tex(tex), count(count) {}
	};
	struct GpuSprite
	{
		Matrix worldmatrix;
		Vector2 texcoord;
		Vector2 texsize;
		Color color;
	};

	OutputWindow* wnd;
	RenderShader* spriteshader;
	LPD3D11EFFECTVARIABLE spritesvar;
	LPD3D11EFFECTMATRIXVARIABLE viewprojvar;
	LPD3D11EFFECTSHADERRESOURCEVARIABLE texvar;
	Matrix *viewmatrix, *projmatrix;
	bool usecamviewmatrix, usecamprojmatrix;

	// Variables for DrawSpriteImmediate()
	UINT immediate_buffercapacity, immediate_buffersize;
	ID3D11ShaderResourceView* immediate_tex;
	GpuSprite* immediate_spritebuffer;

	// Variables for DrawSpriteDelayed()
	std::list<TextureAndCount> delayed_textures;
	std::vector<GpuSprite> delayed_spritebuffer;

	void SetMatrices();

public:
	SpriteContainer(UINT buffersize);

	Result Init(OutputWindow* wnd, const FilePath filename);
	Result Init(OutputWindow* wnd, SpriteContainer* other);

	void SetViewTransform(const Matrix* viewmatrix);
	void SetIdentityViewTransform();
	void SetCameraViewTransform();
	void SetProjectionTransform(const Matrix* projmatrix);
	void SetIdentityProjectionTransform();
	void SetCameraProjectionTransform();

	// Methods for DrawSpriteImmediate()
	void Begin();
	void DrawSpriteImmediate(const Sprite* sprite);
	void End();

	// Methods for DrawSpriteDelayed()
	void DrawSpriteDelayed(const Sprite* sprite);
	void Render(RenderType rendertype);

	void Release();
};

//-----------------------------------------------------------------------------
// Name: TextRenderer
// Desc: Class rendering text using a DirectWrite with a sprite container
//-----------------------------------------------------------------------------
struct IDWriteFactory;
struct IDWriteGdiInterop;
struct DWriteRenderer;
class TextRenderer //: public ITextRenderer
{
public:
	class Font : public ID3dFont
	{
	private:
		TextRenderer* parent;
		FontType type;

		void DrawTextInternal(LPSPRITECONTAINER spritecontainer, const String text, const Rect<float>& bounds, const Color& clr, UINT flags, BYTE drawtype, Size<float>* measuredsize = 0);

	public:
		LPTEXTURE tex;
		float linespacing;
		struct Character
		{
			bool isprintable;
			Vector2 texcoord, texsize;
			float charspacing;
		} chars[256];

		Font(TextRenderer* parent, const FontType& type) : parent(parent), type(type) {}
		const FontType& GetType() {return type;}
		void DrawTextImmediate(LPSPRITECONTAINER spritecontainer, const String text, const Rect<float>& bounds, const Color& clr, UINT flags);
		void DrawTextDelayed(LPSPRITECONTAINER spritecontainer, const String text, const Rect<float>& bounds, const Color& clr, UINT flags);
		void MeasureText(const String text, float fixedwidth, UINT flags, Size<float>* measuredsize);
		void Release() {parent->fonts.erase(type); delete this;}
	};

private:
	static IDWriteFactory* dwtfactory;
	static IDWriteGdiInterop* dwtgdiinterop;
	static DWriteRenderer* dwtrenderer;

	OutputWindow* wnd;

public:
	std::map<FontType, Font*> fonts;

	~TextRenderer();
	Result Init(OutputWindow* wnd);
	Result CreateD3dFont(const FontType& type, ID3dFont** font);
	//Result DrawTextImmediate(LPSPRITECONTAINER spritecontainer, const String text, const String family, float size, bool italic = false, bool bold = false);
};

//-----------------------------------------------------------------------------
// Name: SceneNode
// Desc: ?
//-----------------------------------------------------------------------------
class SceneManager;
class SceneNode : public ISceneNode
{
private:
	SceneManager* parent;
	SceneNode* parentnode;
	std::list<SceneNode*> childnodes;
	std::list<IMoveable*> moveables;
	std::list<IRenderable*> renderables[4];

	// Position and orientation in parent space
	Vector3 pos, scl;
    Quaternion qrot;

	Matrix worldtransform, parenttransform;

public:
	SceneNode(SceneManager* parent);
	SceneNode(SceneManager* parent, SceneNode* src);
	void SetTransform(const Matrix* transform);
	void Update();
	void Render(RenderType rendertype);
	Result GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype);

	void AddChild(ISceneNode* node);
	void AddMoveable(IMoveable* moveable);
	void AddRenderable(IRenderable* renderable, RenderType rendertype);

void Translate(const Vector3* deltapos, TransformSpace ts);
	void TranslateX(const float deltapos, TransformSpace ts);
	void TranslateY(const float deltapos, TransformSpace ts);
	void TranslateZ(const float deltapos, TransformSpace ts);
void SetPosition(const Vector3* pos, TransformSpace ts);
/*XMVECTOR getPosition(TransformSpace ts = TS_PARENT); //get the position in desired space*/
	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);
	void RotateAxis(const Vector3* axis, float angle);
	void RotateQuaternion(const Quaternion* qrot);
void SetOrientation(const Quaternion* qrot, TransformSpace ts);
Quaternion* GetOrientation(Quaternion* qrot, TransformSpace ts);
void SetScale(const Vector3* scale);
/*XMMATRIX getScaling(); //get scaling matrix*/
Matrix* GetTransform(Matrix* mout, TransformSpace ts);
/*XMVECTOR getDirectionZ(TransformSpace ts = TS_PARENT); //return z-axis vector in desired space*/

/*void rotateAxis(const Vector3* axis, FLOAT deg, TransformSpace ts = TS_PARENT); //rotate the scene node
void roll(FLOAT deg); //rotate via z-axis
void pitch(FLOAT deg); //rotate via x-axis
void yaw(FLOAT deg); //rotate via y-axis
void lookAt(const Vector3* target, TransformSpace ts = TS_WORLD); //look at a point in space*/
Vector3* TransformToParentSpace(Vector3* vout, const Vector3* v0, TransformSpace ts);
Vector3* TransformToLocalSpace(Vector3* vout, const Vector3* v0, TransformSpace ts);

	void Release();
};

//-----------------------------------------------------------------------------
// Name: SceneManager
// Desc: ?
//-----------------------------------------------------------------------------
class SceneManager : public ISceneManager
{
private:
	OutputWindow* wnd;
	SceneNode rootnode;
	std::list<SceneNode> scenenodes; // All scene nodes except root (used for garbage collection)

public:
	ISceneNode* GetRoot() {return (ISceneNode*)&rootnode;}
	ISceneNode* CreateNode();
	ISceneNode* CloneNode(ISceneNode* srcnode);

	SceneManager();

	Result Init(OutputWindow* wnd);
	void Update();
	void Render(RenderType rendertype);
	Result GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: SkyBox
// Desc: Class holding one or more sky layers, hovering around the camera
//-----------------------------------------------------------------------------
class SkyBox : public ISkyBox
{
public:
	struct Layer
	{
		Sprite* faces;
		Texture* textures;
		BYTE numfaces;
		float distance;
	};
private:
	OutputWindow* wnd;
	LPSPRITECONTAINER spritecontainer;
	std::list<Layer*> layers;

public:

	SkyBox();

	Result Init(OutputWindow* wnd);
	Result CreateLayer(const FilePath& north, const FilePath& east, const FilePath& south, const FilePath& west, const FilePath& top, const FilePath& bottom, UINT distance);
	void Render(RenderType rendertype);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: BoxedLevel
// Desc: Class rendering a level composed of boxes and drawn with a geometry shader
//-----------------------------------------------------------------------------
class BoxedLevel : public IBoxedLevel
{
private:
/*	UINT buffersize;
	BoxedLevelBoxDesc* boxes;
	ID3D11Buffer* vbboxes;
	std::map<BoxLocation, BoxedLevelBoxDesc*> boxregister;*/
	UINT chunksize[3], totalchunksize; // totalchunksize = chunksize[0] * chunksize[1] * chunksize[2]

	struct ChunkBuffer
	{
		ID3D11Buffer* buffer;
	};
	std::stack<ChunkBuffer*> unusedchunkbuffers;
	std::map<BoxedLevelChunk*, ChunkBuffer*> usedchunkbuffers;

	Texture* texboxes;

	BoxedLevelBoxType* boxtypes;
	UINT16 numboxtypes;

	OutputWindow* wnd;
	RenderShader* blevelshader;
	LPD3D11EFFECTVARIABLE boxtypesvar;
	LPD3D11EFFECTVECTORVARIABLE chunkposvar;
	LPD3D11EFFECTMATRIXVARIABLE worldviewprojvar;

	Result Init();

public:
	BoxedLevel(const UINT (&chunksize)[3], LPTEXTURE texboxes, LPRENDERSHADER blevelshader);

	Result Init(OutputWindow* wnd, UINT numchunkbuffers);
	void SetBoxTypes(BoxedLevelBoxType* boxtypes, UINT32 numtypes);
	void SetBoxTypesWrapper(std::vector<BoxedLevelBoxType>& list)
	{
		if(list.size())
			SetBoxTypes(&list[0], list.size());
	}
	void AssignChunk(BoxedLevelChunk* chunk);
	void UnassignChunk(BoxedLevelChunk* chunk);
	void UpdateChunk(BoxedLevelChunk* chunk, const UINT (&boxpos)[3]);
	void UpdateChunk(BoxedLevelChunk* chunk, const UINT (&boxpos_min)[3], const UINT (&boxpos_max)[3]);
	void GetAssignedChunks(std::list<BoxedLevelChunk*>* chunks);
	void GetChunkSize(UINT (*chunksize)[3]) {memcpy(chunksize, this->chunksize, sizeof(this->chunksize));}
	LPTEXTURE GetTexture() {return texboxes;}
	void Render(RenderType rendertype);
	void Release();
	std::shared_ptr<void> GetPyPtr();
};

/*//-----------------------------------------------------------------------------
// Name: Landscape
// Desc: Class rendering a heightmap
//-----------------------------------------------------------------------------
class Landscape : public ILandscape
{
private:
	OutputWindow* wnd;
	Texture* tex;
	Object* obj;

public:

	Landscape();

	Result Load(FilePath& filename, LPRENDERSHADER shader, OutputWindow* wnd);
	void Render(RenderType rendertype);
	void Release();
};*/

//-----------------------------------------------------------------------------
// Name: Laser
// Desc: Class rendering a laser ray and spot
//-----------------------------------------------------------------------------
class Laser : public ILaser
{
private:
	RenderShader* shader;
	Texture* tex;
	ID3D11Buffer *rayvertexbuffer, *spotvertexbuffer;

public:
	Vector3 pos, dir;

	Laser();

	Result Init(const FilePath& spotfilename, Color color, float diameter, float spotsize, float strength, int falloff, OutputWindow* wnd);
	void Render(RenderType rendertype);
	void Update();
	void Release();
};

//-----------------------------------------------------------------------------
// Name: HUD
// Desc: Class managing foreground sprites
//-----------------------------------------------------------------------------
class HUD : public IHUD
{
public:
	enum ElementType {ET_IMAGE, ET_TEXT};
	struct ImageElement : public IHUD::IImageElement
	{
		Sprite sprt;
		Texture* tex;
		int _x, _y;
		DockType _dock;
		Rect<int> subregion;
		bool poschanged;

		PyObject* GetPyUser() {return (PyObject*)user;}
		void SetPyUser(PyObject* pyobject) {user = pyobject;}
		int GetX() {return _x;}
		int SetX(int val) {poschanged = true; return _x = val;}
		int GetY() {return _y;}
		int SetY(int val) {poschanged = true; return _y = val;}
		DockType GetDock() {return _dock;}
		DockType SetDock(DockType val) {poschanged = true; return _dock = val;}

		ImageElement(int zorder) : IHUD::IImageElement(zorder) {}
	};
	struct TextElement : public IHUD::ITextElement
	{
		int _x, _y;
		DockType _dock;
		Size<int> size;
		Rect<float> textbounds;
		bool poschanged;

		PyObject* GetPyUser() {return (PyObject*)user;}
		void SetPyUser(PyObject* pyobject) {user = pyobject;}
		int GetX() {return _x;}
		int SetX(int val) {poschanged = true; return _x = val;}
		int GetY() {return _y;}
		int SetY(int val) {poschanged = true; return _y = val;}
		DockType GetDock() {return _dock;}
		DockType SetDock(DockType val) {poschanged = true; return _dock = val;}

		TextElement(int zorder) : IHUD::ITextElement(zorder) {}
	};
private:
	OutputWindow* wnd;
	LPSPRITECONTAINER spritecontainer;
	PriorityQueue<IElement*, IElement::CompareFunctor> elements;
	struct MouseEventListener
	{
		bool mouseoverlastframe;
		MouseEventListener() : mouseoverlastframe(false) {}
		virtual void RaiseMouseDown(IHUD::LPELEMENT element) = 0;
		virtual void RaiseMouseUp(IHUD::LPELEMENT element) = 0;
		virtual void RaiseMouseMove(IHUD::LPELEMENT element) = 0;
		virtual void RaiseMouseEnter(IHUD::LPELEMENT element) = 0;
		virtual void RaiseMouseLeave(IHUD::LPELEMENT element) = 0;
	};
	struct CMouseEventListener : public MouseEventListener
	{
		MOUSEEVENT_CALLBACK mousedowncbk, mouseupcbk, mousemovecbk, mouseentercbk, mouseleavecbk;
		CMouseEventListener() : mousedowncbk(NULL), mouseupcbk(NULL), mousemovecbk(NULL), mouseentercbk(NULL), mouseleavecbk(NULL) {}
		void RaiseMouseDown(IHUD::LPELEMENT element) {if(mousedowncbk) mousedowncbk(element);}
		void RaiseMouseUp(IHUD::LPELEMENT element) {if(mouseupcbk) mouseupcbk(element);}
		void RaiseMouseMove(IHUD::LPELEMENT element) {if(mousemovecbk) mousemovecbk(element);}
		void RaiseMouseEnter(IHUD::LPELEMENT element) {if(mouseentercbk) mouseentercbk(element);}
		void RaiseMouseLeave(IHUD::LPELEMENT element) {if(mouseleavecbk) mouseleavecbk(element);}
	};
	struct PyMouseEventListener : public MouseEventListener
	{
		PyObject *mousedownpycbk, *mouseuppycbk, *mousemovepycbk, *mouseenterpycbk, *mouseleavepycbk;
		PyMouseEventListener() : mousedownpycbk(NULL), mouseuppycbk(NULL), mousemovepycbk(NULL), mouseenterpycbk(NULL), mouseleavepycbk(NULL) {}
		void RaiseMouseDown(IHUD::LPELEMENT element);
		void RaiseMouseUp(IHUD::LPELEMENT element);
		void RaiseMouseMove(IHUD::LPELEMENT element);
		void RaiseMouseEnter(IHUD::LPELEMENT element);
		void RaiseMouseLeave(IHUD::LPELEMENT element);
	};
	std::map<std::pair<IElement*, bool>, MouseEventListener*> mouselisteners; // bool ... true=PyMouseEventListener, false=CMouseEventListener
	BYTE mousebuttonslastframe[8];

	CMouseEventListener* FindCMouseEventListener(LPELEMENT element);
	PyMouseEventListener* FindPyMouseEventListener(LPELEMENT element);

public:
	Vector3 pos, rot;

	HUD();

	Result Init(OutputWindow* wnd);
	Result CreateElement(ITexture* tex, int x, int y, DockType dock, Color color, int zorder, LPIMAGEELEMENT* element, LPVOID elementuser);
	Result CreateElement(ITexture* tex, int x, int y, DockType dock, Color color, const Rect<int>& subregion, int zorder, LPIMAGEELEMENT* element, LPVOID elementuser);
	Result CreateElement(String text, LPD3DFONT font, int x, int y, DockType dock, Color color, const Size<int>& size,
						 int zorder = 0, UINT textflags = 0, LPTEXTELEMENT* element = NULL, LPVOID elementuser = NULL);
	ImageElement* CreateElementWrapper1(ITexture* tex, int x, int y, DockType dock, Color color, int zorder, PyObject* elementuser)
	{
		ImageElement* element;
		CreateElement(tex, x, y, dock, color, zorder, (LPIMAGEELEMENT*)&element, elementuser);
		return element;
	}
	ImageElement* CreateElementWrapper2(ITexture* tex, int x, int y, DockType dock, Color color, const Rect<int>& subregion, int zorder, PyObject* elementuser)
	{
		ImageElement* element;
		CreateElement(tex, x, y, dock, color, subregion, zorder, (LPIMAGEELEMENT*)&element, elementuser);
		return element;
	}
	TextElement* CreateElementWrapper3(String text, LPD3DFONT font, int x, int y, DockType dock, Color color, const Size<int>& size,
									   int zorder, UINT textflags, PyObject* elementuser)
	{
		TextElement* element;
		CreateElement(text, font, x, y, dock, color, size, zorder, textflags, (LPTEXTELEMENT*)&element, elementuser);
		return element;
	}
	void RemoveElement(LPELEMENT element);
	void RegisterMouseDownListener(IElement* element, MOUSEEVENT_CALLBACK cbk);
	void RegisterPyMouseDownListener(IElement* element, PyObject* pycbk);
	void RegisterMouseUpListener(IElement* element, MOUSEEVENT_CALLBACK cbk);
	void RegisterPyMouseUpListener(IElement* element, PyObject* pycbk);
	void RegisterMouseMoveListener(IElement* element, MOUSEEVENT_CALLBACK cbk);
	void RegisterPyMouseMoveListener(IElement* element, PyObject* pycbk);
	void RegisterMouseEnterListener(IElement* element, MOUSEEVENT_CALLBACK cbk);
	void RegisterPyMouseEnterListener(IElement* element, PyObject* pycbk);
	void RegisterMouseLeaveListener(IElement* element, MOUSEEVENT_CALLBACK cbk);
	void RegisterPyMouseLeaveListener(IElement* element, PyObject* pycbk);
	void Render(RenderType rendertype);
	void Release();
	void Update();
	LPSPRITECONTAINER GetSpriteContainer() {return spritecontainer;}
};

//-----------------------------------------------------------------------------
// Name: D3dCursor
// Desc: A texture moved by mouse move events
//-----------------------------------------------------------------------------
class D3dCursor : public ID3dCursor
{
private:
	OutputWindow* wnd;
	LPSPRITECONTAINER spritecontainer;
	Sprite sprtcursor;

public:
	D3dCursor()
	{
		tex = NULL;
		animated = false;
		hotspot.x = hotspot.y = 0;
	}
	Result Init(OutputWindow* wnd);
	void Render(RenderType rendertype);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: D3DConsole
// Desc: Extends Console with Direct3D support
//-----------------------------------------------------------------------------
class D3DConsole : public Console, public IRenderable
{
private:
	OutputWindow* wnd;
	LPSPRITECONTAINER spritecontainer;
	LPD3DFONT font;
	String* msghistory;
	String** visiblemsgs;
	UINT msghistorysize, numlines, writemsgidx;
	bool historyfull;

	// For fps computation
	float fpstimer;
	UINT framecounter;

public:
	D3DConsole(UINT numlines, UINT historysize);

	Result Init(OutputWindow* wnd);
	void PrintD3D(const String& message);
	void PrintD3D(int message);
	void PrintD3D(double message);
	void ScrollTo(UINT idx);
	inline UINT GetBottomScrollBound();
	inline UINT GetTopScrollBound();
	void Render(RenderType rendertype);
	void OnLostDevice(){};
	void OnResetDevice(){};
	void Release();

	void ShowFps() {framecounter = 0;}
	void HideFps() {framecounter = 0xFFFFFFFF;}
};

//-----------------------------------------------------------------------------
// Name: OutputWindow
// Desc: Interface to a single Direct3D output
//-----------------------------------------------------------------------------
class Direct3D;
class LineDrawer;
class SpriteDrawer;
class OutputWindow : public IOutputWindow
{
protected:
	Direct3D* parent;
	IDXGISwapChain* swapchain;
	Texture* texbackbuffer;
	ID3D11Texture2D* texdepthstencil;
	ID3D11DepthStencilView *dsview;
	D3D11_VIEWPORT viewport;
	HWND wnd;
	D3DConsole* d3dcle;
	LineDrawer* linedrawer[4];
	RenderShader* spriteshader;
	SpriteContainer* defaultspritecontainer;
	TextRenderer textrenderer;

	// Screenshot
	bool takescreenshot;
	FilePath screenshotfilename;
	D3DX_IMAGE_FILEFORMAT screenshotfileformat;

	struct RenderDelegate
	{
		union
		{
			RENDERFUNC func;
			PyObject* pyfunc;
			IRenderable* cls;
		} ptr;
		const enum DelegateType
			{DT_FUNC, DT_PYFUNC, DT_CLASS} type;
		RenderDelegate(DelegateType type) : type(type) {}
		virtual void operator()(OutputWindow* wnd, RenderType rendertype) = 0;
	};
	struct FuncRenderDelegate : RenderDelegate
	{
		LPVOID user;
		FuncRenderDelegate(RENDERFUNC func, LPVOID user) : RenderDelegate(DT_FUNC), user(user) {ptr.func = func;}
		void operator()(OutputWindow* wnd, RenderType rendertype) {ptr.func(wnd, user);}
	};
	struct PyFuncRenderDelegate : RenderDelegate
	{
		PyFuncRenderDelegate(PyObject* pyfunc) : RenderDelegate(DT_PYFUNC) {ptr.pyfunc = pyfunc;}
		void operator()(OutputWindow* wnd, RenderType rendertype) {wnd->CallPyRenderFunc(ptr.pyfunc);}
	};
	struct ClassRenderDelegate : RenderDelegate
	{
		ClassRenderDelegate(IRenderable* cls) : RenderDelegate(DT_CLASS) {ptr.cls = cls;}
		void operator()(OutputWindow* wnd, RenderType rendertype) {if(ptr.cls->autorenderenabled) ptr.cls->Render(rendertype);}
	};
	std::list<RenderDelegate*> renderlist[4];
	std::list<IUpdateable*> updatelist;

	Result OnReset();
	void CallPyRenderFunc(PyObject* pyfunc);

public:
	std::list<Texture*> textures;
	std::list<RenderShader*> rendershaders;
	std::list<Object*> objects;
	std::list<HUD*> huds;
	std::list<D3dCursor*> cursors;
	std::list<SpriteContainer*> spritecontainers;
	std::list<SceneManager*> scenemanagers;
	std::list<SkyBox*> skyboxes;
	std::list<BoxedLevel*> blevels;
/*	std::list<Landscape*> landscapes;*/

	struct DeviceSpecific
	{
		LPD3DDEVICE device;
		LPD3DDEVICECONTEXT devicecontext;
		ID3D11DepthStencilState *dssdefault, *dssnodepth;
		DXGI_SAMPLE_DESC multisampling;
		ID3D11RasterizerState* rasterizer;
		UINT refcount;

		DeviceSpecific()
		{
			device = NULL;
			devicecontext = NULL;
			dssdefault = dssnodepth = NULL;
			multisampling.Count = 1;
			multisampling.Quality = 0;
			rasterizer = NULL;
			refcount = 1;
		}
		~DeviceSpecific()
		{
			RELEASE(device);
		}
	}* devspec;
	Size<UINT> backbuffersize;
	Camera* cam;

	OutputWindow(Direct3D* parent, IOutputWindow::Settings* wndsettings);

	LPD3DDEVICE GetDevice()
		{return devspec ? devspec->device : NULL;}

	Result CreateDevice(IDXGIFactory* dxgifty, IOutputWindow::Settings* settings);
	Result AdoptDevice(IDXGIFactory* dxgifty, IOutputWindow::Settings* settings, OutputWindow* sourcewnd);
	Size<UINT> GetBackbufferSize() {return backbuffersize;}
	UINT GetBackbufferWidth() {return backbuffersize.width;}
	UINT GetBackbufferHeight() {return backbuffersize.height;}
	HWND GetHwnd() {return wnd;}
	ITexture* GetBackbuffer() {return texbackbuffer;}
	ITexture* SetBackbuffer(ITexture* texbackbuffer);
	Result Resize(UINT width, UINT height);
	void Update();
	void Render();
	void RegisterForRendering(RENDERFUNC func, RenderType rendertype, LPVOID user);
	void RegisterForRendering(PyObject* pyfunc, RenderType rendertype);
	void RegisterForRendering(IRenderable* cls, RenderType rendertype);
	void DeregisterFromRendering(RENDERFUNC func, RenderType rendertype);
	void DeregisterFromRendering(PyObject* pyfunc, RenderType rendertype);
	void DeregisterFromRendering(IRenderable* cls, RenderType rendertype);
	void DeregisterFromRendering(IRenderable* cls);
	void RegisterForUpdating(IUpdateable* cls);
	void DeregisterFromUpdating(IUpdateable* cls);
	void SortRegisteredClasses(RenderType rendertype, const Vector3& campos);
	Result GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype);

	void DrawLine(const Vector3& v0, const Vector3& v1, Color c0, Color c1, RenderType rendertype);
	void DrawLine(const Vector2& v0, const Vector2& v1, Color c0, Color c1, RenderType rendertype);
	Result EnableSprites(const FilePath spriteshaderfilename);
	Result EnableLineDrawing(const FilePath lineshaderfilename);
	void DrawSprite(const Sprite* sprite);
	void ShowFps();
	void HideFps();
	void TakeScreenShot(const FilePath& filename, D3DX_IMAGE_FILEFORMAT fileformat);
	void SetCamera(ICamera* cam);
	ICamera* GetCamera() const {return cam;}
	D3D11_VIEWPORT& GetViewport() {return viewport;}
	ID3D11Device* GetDevice() const {return devspec->device;}
	ID3D11DeviceContext* GetDeviceContext() const {return devspec->devicecontext;}
	Result MakeConsoleTarget(UINT numlines, UINT historysize);
	Result CreateTexture(const FilePath& filename, ITexture::Usage usage, bool deviceindependent, ITexture** tex);
	Texture* CreateTextureFromFileWrapper(const FilePath& filename, ITexture::Usage usage, bool deviceindependent)
	{
		Result rlt;
		Texture* tex;
		IFFAILED(CreateTexture(filename, usage, deviceindependent, (LPTEXTURE*)&tex))
			throw rlt.GetLastResult();
		return tex;
	}
	Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, ITexture** tex);
	Texture* CreateNewTextureWrapper(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format)
	{
		Result rlt;
		Texture* tex;
		IFFAILED(CreateTexture(width, height, usage, deviceindependent, format, (LPTEXTURE*)&tex))
			throw rlt.GetLastResult();
		return tex;
	}
	Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, LPVOID data, ITexture** tex);
	Result CreateTexture(ITexture* source, ITexture::Usage usage, bool deviceindependent, ITexture** tex);
	Texture* CopyTextureWrapper(ITexture* source, ITexture::Usage usage, bool deviceindependent)
	{
		Result rlt;
		Texture* tex;
		IFFAILED(CreateTexture(source, usage, deviceindependent, (LPTEXTURE*)&tex))
			throw rlt.GetLastResult();
		return tex;
	}
	Result CreateRenderShader(const FilePath filename, IRenderShader** shader);
	RenderShader* CreateRenderShaderWrapper(const FilePath filename)
	{
		Result rlt;
		RenderShader* sdr;
		IFFAILED(CreateRenderShader(filename, (LPRENDERSHADER*)&sdr))
			throw rlt.GetLastResult();
		return sdr;
	}
	Result CreateObject(const FilePath& filename, bool loadtextures, IObject** obj);
	Object* CreateObjectFromFileWrapper(const FilePath& filename, bool loadtextures)
	{
		Result rlt;
		Object* obj;
		IFFAILED(CreateObject(filename, loadtextures, (LPOBJECT*)&obj))
			throw rlt.GetLastResult();
		return obj;
	}
	Result CreateObject(IObject* srcobj, bool newmaterials, IObject** obj);
	Object* CreateObjectFromObjectWrapper(IObject* srcobj, bool newmaterials)
	{
		Result rlt;
		Object* obj;
		IFFAILED(CreateObject(srcobj, newmaterials, (LPOBJECT*)&obj))
			throw rlt.GetLastResult();
		return obj;
	}
	Result CreateObject(UINT numvertices, UINT numfaces, IObject** obj);
	Result CreateObject(const D3dShapeDesc& shapedesc, IObject** obj);
	Object* CreateObjectFromShapeWrapper(const D3dShapeDesc& shapedesc)
	{
		Result rlt;
		Object* obj;
		IFFAILED(CreateObject(shapedesc, (LPOBJECT*)&obj))
			throw rlt.GetLastResult();
		return obj;
	}
	Result CreateObject(const D3dShapeDesc* shapedescs[], UINT numshapedescs, IObject** obj);
	void RemoveObject(IObject* obj);
	void RemoveAllObjects();
	Result CreateCursor(ID3dCursor** cursor);
	D3dCursor* CreateCursorWrapper()
	{
		Result rlt;
		D3dCursor* cursor;
		IFFAILED(CreateCursor((LPD3DCURSOR*)&cursor))
			throw rlt.GetLastResult();
		return cursor;
	}
	Result CreateHUD(IHUD** hud);
	HUD* CreateHUDWrapper()
	{
		Result rlt;
		HUD* hud;
		IFFAILED(CreateHUD((LPHUD*)&hud))
			throw rlt.GetLastResult();
		return hud;
	}
	Result CreateSpriteContainer(UINT buffersize, ISpriteContainer** spc);
	Result CreateD3dFont(const FontType& type, ID3dFont** font);
	TextRenderer::Font* CreateD3dFontWrapper(const FontType& type)
	{
		Result rlt;
		TextRenderer::Font* font;
		IFFAILED(CreateD3dFont(type, (LPD3DFONT*)&font))
			throw rlt.GetLastResult();
		return font;
	}
	Result CreateSceneManager(ISceneManager** mgr);
	SceneManager* CreateSceneManagerWrapper()
	{
		Result rlt;
		SceneManager* mgr;
		IFFAILED(CreateSceneManager((LPSCENEMANAGER*)&mgr))
			throw rlt.GetLastResult();
		return mgr;
	}
	Result CreateSkyBox(ISkyBox** sky);
	SkyBox* CreateSkyBoxWrapper()
	{
		Result rlt;
		SkyBox* sky;
		IFFAILED(CreateSkyBox((LPSKYBOX*)&sky))
			throw rlt.GetLastResult();
		return sky;
	}
	Result CreateBoxedLevel(const UINT (&chunksize)[3], UINT numchunkbuffers, LPTEXTURE texboxes, LPRENDERSHADER blevelshader, IBoxedLevel** blevel);
/*	Result CreateLandscape(FilePath& filename, LPRENDERSHADER shader, ILandscape** landscape);*/ //EDIT11
	void Release();
};

//-----------------------------------------------------------------------------
// Name: WindowlessDevice
// Desc: Interface to a Direct3D device without swapchain
//-----------------------------------------------------------------------------
//static int counter = 0;
class WindowlessDevice : public IWindowlessDevice
{
public:
	struct TextureAndVariables
	{
		Texture* t;
		LPD3D11EFFECTSHADERRESOURCEVARIABLE vartex;
		LPD3D11EFFECTVECTORVARIABLE varsize;
		String texsemantic, sizesemantic;
		TextureAndVariables(Texture* tex, LPD3D11EFFECTSHADERRESOURCEVARIABLE vartex, LPD3D11EFFECTVECTORVARIABLE varsize, String& texsemantic, String& sizesemantic)
		{
			this->t = tex;
			this->vartex = vartex;
			this->varsize = varsize;
			this->texsemantic = String(texsemantic);
			this->sizesemantic = String(sizesemantic);
		}
	};

private:
	RenderQuad renderquad;
	std::list<Texture*> textures;
	std::list<RenderShader*> rendershaders;
	LPD3DDEVICE device;
	LPD3DDEVICECONTEXT devicecontext;
	RenderShader* shader;
	std::list<TextureAndVariables*> srctextures;
	TextureAndVariables* desttexture;

public:

	WindowlessDevice();

	Result Create();
	Result AddSourceTexture(ITexture* tex, String& texsemantic, String& sizesemantic);
	Result SetDestinationTexture(ITexture* tex, String& sizesemantic);
	Result SetShader(IRenderShader* shader);
	void Render(float bg_r, float bg_g, float bg_b);
	void Render();
	Result CreateTexture(FilePath& filename, ITexture::Usage usage, bool deviceindependent, ITexture** tex);
	Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, ITexture** tex);
	void RemoveTexture(ITexture* tex);
	Result CreateRenderShader(FilePath& filename, IRenderShader** shader);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Direct3D
// Desc: Basic Direct3D interface
//-----------------------------------------------------------------------------
class Direct3D : public IDirect3D
{
	//friend void IRenderable::RegisterForRendering(IOutputWindow* wnd, RenderType rendertype);
	//friend void IRenderable::DeregisterFromRendering(IOutputWindow* wnd, RenderType rendertype);
private:
	bool isfullscreen;
	std::list<Camera*> cameras;
	std::list<WindowlessDevice*> wldevs;

public:
	IDXGIFactory* dxgifty;
	std::list<IOutputWindow*> wnds;
	std::list<Light*> lights;
	std::vector<Light::LightParams> lightparams; // Only modified/used by Light class

	/*OutputWindow* GetWnd(int wndidx) const
		{return wndidx < numwnds ? wnds[wndidx] : NULL;}
	LPD3DDEVICE GetDevice(int wndidx) const
		{return wndidx < numwnds ? wnds[wndidx]->GetDevice() : NULL;}
	Size<UINT> GetBackbufferSize(int wndidx) const
		{return wndidx < numwnds ? wnds[wndidx]->GetBackbufferSize() : Size<UINT>();}*/
	bool IsFullscreen() const
		{return isfullscreen;}
	/*void DispFps(int wndidx)
		{if(wndidx < numwnds) wnds[wndidx]->DispFps();}
	void RegisterForRendering(int wndidx, RENDERFUNC func, RenderType rendertype)
		{if(wndidx < numwnds) wnds[wndidx]->RegisterForRendering(func, rendertype);}
	void DeregisterFromRendering(int wndidx, RENDERFUNC func, RenderType rendertype)
		{if(wndidx < numwnds) wnds[wndidx]->DeregisterFromRendering(func, rendertype);}
	Camera* GetCamera(int wndidx) const
		{return wndidx < numwnds ? wnds[wndidx]->GetCamera() : NULL;}
	void SetCamera(int wndidx, ICamera* cam)
		{if(wndidx < numwnds) wnds[wndidx]->SetCamera(cam);}
	void SetBackColor(int wndidx, bool enabled, const float colorRGBA[4])
		{if(wndidx < numwnds) wnds[wndidx]->SetBackColor(enabled, colorRGBA);}
	void SetBackColor(int wndidx, bool enabled, const BYTE colorRGBA[4])
		{if(wndidx < numwnds) wnds[wndidx]->SetBackColor(enabled, colorRGBA);}
	void SetBackColor(int wndidx, bool enabled, UINT color)
		{if(wndidx < numwnds) wnds[wndidx]->SetBackColor(enabled, color);}*/

	Direct3D();

	void Sync(GLOBALVARDEF_LIST);
	Result Init(const D3dStartupSettings* settings);
	void Render();
	bool CastRayTo(const Vector3& src, const Vector3& dest, float* dist);
	bool CastRayDir(const Vector3& src, const Vector3& dir, float* dist);
	void TakeScreenShots(const FilePath& filename, D3DX_IMAGE_FILEFORMAT fileformat);
	void GetEnabledLights(std::list<ILight*>* lights);
	void RemoveAllLights();
	const std::list<IOutputWindow*>& GetOutputWindows() const {return this->wnds;}
	Result CreateOutputWindow(IOutputWindow::Settings* settings, IOutputWindow** wnd);
	Result CreateWindowlessDevice(IWindowlessDevice** wldev);
	Result CreateCamera(float fov, float clipnear, float clipfar, ICamera** cam);
	Camera* CreateCameraWrapper(float fov, float clipnear, float clipfar)
	{
		Result rlt;
		Camera* cam;
		IFFAILED(CreateCamera(fov, clipnear, clipfar, (LPCAMERA*)&cam))
			throw rlt.GetLastResult();
		return cam;
	}
	Result CreateLight(ILight::LightType type, ILight** light);
	Light* CreateLightWrapper(ILight::LightType type)
	{
		Result rlt;
		Light* light;
		IFFAILED(CreateLight(type, (LPLIGHT*)&light))
			throw rlt.GetLastResult();
		return light;
	}
	void Release();
};

void SyncWithPython();

#endif