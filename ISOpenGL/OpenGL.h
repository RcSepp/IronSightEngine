#ifndef __OPENGL_H
#define __OPENGL_H

#include "ISOpenGL.h"
#include "GLX.h"

#pragma comment(lib, "opengl32.lib")
#ifdef _DEBUG
	#pragma comment (lib, "ISMath_d.lib")
#else
	#pragma comment (lib, "ISMath.lib")
#endif

struct _object;
#ifndef PyObject
typedef _object PyObject;
#endif


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
	struct Caps
	{
		bool genmipmaps, gpu_write, cpu_read, cpu_write, bind_resource, bind_rendertarget, force_rgba;
		Caps()
		{
			memset(this, 0, sizeof(Caps));
		}
	} caps;

public:
	struct Pixel
	{
		BYTE r, g, b, a;
		bool operator==(const Pixel& other) const
			{return *(UINT32*)this == *(UINT32*)&other;}
	};
	std::vector<Pixel>* mappedpixeldata;

	GLuint tex;
	/*ID3D11Texture2D* tex;
	ID3D11ShaderResourceView* srview;
	ID3D11RenderTargetView* rtview;*/
	FilePath filename;
	int width, height, miplevels;

	int GetWidth() {return width;}
	int GetHeight() {return height;}
	int GetNumMipLevels() {return miplevels;}
	FilePath GetFilename() {return filename;}

	Texture();

	void CreateEmpty(OutputWindow* wnd) {this->wnd = wnd;}
	Result CreateNew(OutputWindow* wnd, UINT width, UINT height, Usage usage,
					 UINT miplevels = 0xFFFFFFFF, LPVOID data = NULL);
	Result CreateCopy(Texture* source, Usage usage);
	Result Load(OutputWindow* wnd, const FilePath& filename, Usage usage);
	Result MapPixelData(bool read, bool write, UINT* stride, LPVOID* data);
	void UnmapData();
	/*Result Save(const FilePath& filename, D3DX_IMAGE_FILEFORMAT format);
	Result Save(LPD3D10BLOB* datablob, D3DX_IMAGE_FILEFORMAT format);*/
	void Render();
	void Release();
};

//-----------------------------------------------------------------------------
// Name: RenderShader
// Desc: Class holding a single SM 4.0 shader for rendering
//-----------------------------------------------------------------------------
class RenderShader : public IRenderShader
{
private:
	GLuint vertexshader, fragmentshader, shaderprogram;
	/*LPD3D11EFFECTTECHNIQUE tech;
	D3D11_INPUT_ELEMENT_DESC* vtxdecl;
	ID3D11InputLayout **layouts;
	UINT vtxdeclcount, passcount;*/

	Result ReCreateInputLayouts();

public:
	struct DefaultParameters
	{
		GLint world, worldarray, view, proj, viewproj, worldviewproj, worldviewprojarray, worldinvtrans, worldinvtransarray; // Matrices
		GLint viewpos, ambient, diffuse, specular; // Vectors
		GLint numlights; // Light parameters
		GLint lightparams; // Light parameters
		GLint power, time, hastex; // Misc
		GLint tex; // Textures

		DefaultParameters()
		{
			GLint* params = (GLint*)this;
			for(int i = 0, numparams = sizeof(DefaultParameters) / sizeof(GLint); i < numparams; ++i)
				params[i] = -1;
		}
	} defparams;

	struct DefaultVertexAttributes
	{
		GLint pos, nml, texcoord, blendidx;

		DefaultVertexAttributes()
		{
			GLint* params = (GLint*)this;
			for(int i = 0, numparams = sizeof(DefaultVertexAttributes) / sizeof(GLint); i < numparams; ++i)
				params[i] = -1;
		}
	} defvtxattrs;

	RenderShader();

	Result LoadFX(const FilePath filename);
	/*LPD3D11EFFECTVARIABLE GetVariableBySemantic(LPCSTR semantic) {return effect ? effect->GetVariableBySemantic(semantic) : NULL;}
	LPD3D11EFFECTVARIABLE GetVariableByName(LPCSTR name) {return effect ? effect->GetVariableByName(name) : NULL;}
	IEffectScalarVariable* GetScalarVariable(const String& semantic);
	IEffectVectorVariable* GetVectorVariable(const String& semantic);
	IEffectMatrixVariable* GetMatrixVariable(const String& semantic);*/
	void PrepareRendering(Camera* cam, const Matrix* worldmatrix, const Matrix* worldmatrixarray, UINT numblendparts, UINT* passcount);
	void PrepareRendering(UINT* passcount);
	void PreparePass(UINT passindex, UINT techidx = 0xFFFFFFFF);
	void PrepareSubset(const Texture* tex, const Color* ambient, const Color* diffuse, const Color* specular, float power, UINT passindex);
	void ApplyStateChanges(UINT passindex = (UINT)-1);
	Result SetVtxDecl(/*D3D11_INPUT_ELEMENT_DESC* vtxdecl, UINT vtxdeclcount*/);
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

	RENDER_CALLBACK beforerendercbk;
	LPVOID beforerendercbkuser;

	std::map<LPVOID, MeshBuffer*> mappedbuffers;

	void ComputeBoundingSphere(IObject::Vertex* vertices, UINT numvertices);
	Result AddShape(const OglShapeDesc& shapedesc, IObject::Vertex* vertices, UINT* indices, UINT startidx);

public:

	Object();

	bool GetRefPos(Vector3& pos) const;
	void SetTransform(const Matrix* transform);

	UINT GetNumBones() {return numblendparts;}

	std::vector<OGLMaterial>& GetMatsWrapper() {return *mats;}

	Result LoadX(const FilePath& filename, bool loadtextures, OutputWindow* wnd);
	Result Create(IObject* srcobj, bool newmaterials);
	Result Create(UINT numvertices, UINT numfaces, OutputWindow* wnd);
	Result Create(const OglShapeDesc& shapedesc, OutputWindow* wnd);
	Result Create(const OglShapeDesc* shapedescs[], UINT numshapedescs, OutputWindow* wnd);
	void Render(RenderType rendertype);
	Result CreateCollisionMesh(LPREGULARENTITY* entity);
	Result CreateConvexCollisionHull(LPREGULARENTITY* entity);
	Result CreateCollisionCapsule(float radius, LPREGULARENTITY* entity);
	Result CreateCollisionRagdoll(float radius, bool usedynamicmotion, LPRAGDOLLENTITY* entity);
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
	Result MapIndexData(UINT** indices);
	Result UnmapData(LPVOID vertices);
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
// Name: OutputWindow
// Desc: Interface to a single OpenGL output
//-----------------------------------------------------------------------------
class OutputWindow : public IOutputWindow
{
private:
	DisplayHandle display;
	WindowHandle wnd;
	DeviceContext devicectx;
	RenderContext renderctx;

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

	inline void CallPyRenderFunc(PyObject* pyfunc) {}

public:
	std::list<Texture*> textures;
	std::list<RenderShader*> rendershaders;
	std::list<Object*> objects;

	Size<UINT> backbuffersize;
	Camera* cam;

	OutputWindow(IOutputWindow::Settings* wndsettings);

	Result CreateDevice(IOutputWindow::Settings* settings);
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
	void SetCamera(ICamera* cam);
	ICamera* GetCamera() const {return cam;}
	Result CreateTexture(const FilePath& filename, ITexture::Usage usage, ITexture** tex);
	Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, ITexture** tex);
	Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, LPVOID data, ITexture** tex);
	Result CreateTexture(ITexture* source, ITexture::Usage usage, ITexture** tex);
	Result CreateRenderShader(const FilePath filename, IRenderShader** shader);
	Result CreateObject(const FilePath& filename, bool loadtextures, IObject** obj);
	Result CreateObject(IObject* srcobj, bool newmaterials, IObject** obj);
	Result CreateObject(UINT numvertices, UINT numfaces, IObject** obj);
	Result CreateObject(const OglShapeDesc& shapedesc, IObject** obj);
	Result CreateObject(const OglShapeDesc* shapedescs[], UINT numshapedescs, IObject** obj);
	void RemoveObject(IObject* obj);
	void RemoveAllObjects();
	void Release();
};

//-----------------------------------------------------------------------------
// Name: OpenGL
// Desc: Basic OpenGL interface
//-----------------------------------------------------------------------------
class OpenGL : public IOpenGL
{
private:
	bool isfullscreen;
	std::list<Camera*> cameras;
	std::list<IOutputWindow*> wnds;

public:
	int oglversion_major, oglversion_minor;

	OpenGL();
	void Sync(GLOBALVARDEF_LIST);
	Result Init(const OglStartupSettings* settings);
	void Render();
	Result CreateOutputWindow(IOutputWindow::Settings* settings, IOutputWindow** wnd);
	Result CreateCamera(float fov, float clipnear, float clipfar, ICamera** cam);
	void Release();
};

#endif