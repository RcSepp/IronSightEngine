#ifdef DIRECT3D_SCRIPTABLE
#include "Direct3D.h"
#include "ISPythonScriptEngine.h"
#include <vector>

//#define BOOST_PYTHON_STATIC_LIB

/*#include <boost/python/return_opaque_pointer.hpp>//EDIT
#include <boost/python/def.hpp>//EDIT
#include <boost/python/module.hpp>//EDIT
#include <boost/python/return_value_policy.hpp>//EDIT*/

#include <boost\python.hpp>
#include <boost\python\suite\indexing\vector_indexing_suite.hpp>
using namespace boost::python;

//BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(IRagdollEntity)//EDIT

void OutputWindow::CallPyRenderFunc(PyObject* pyfunc)
{
	pse->Call(pyfunc, "(O)", ((object)ptr(this)).ptr());
	//PyObject_CallObject(pyfunc, NULL);
	/*object _pyfunc = object(handle<>(pyfunc));
	Py_INCREF(pyfunc);
	_pyfunc();*/
}

void HUD::PyMouseEventListener::RaiseMouseDown(IHUD::LPELEMENT element)
{
	if(mousedownpycbk)
	{
		switch(element->type)
		{
		case ET_IMAGE: pse->Call(mousedownpycbk, "(O)", ((object)ptr((ImageElement*)element)).ptr()); break;
		case ET_TEXT: pse->Call(mousedownpycbk, "(O)", ((object)ptr((TextElement*)element)).ptr()); break;
		}
	}
}
void HUD::PyMouseEventListener::RaiseMouseUp(IHUD::LPELEMENT element)
{
	if(mouseuppycbk)
	{
		switch(element->type)
		{
		case ET_IMAGE: pse->Call(mouseuppycbk, "(O)", ((object)ptr((ImageElement*)element)).ptr()); break;
		case ET_TEXT: pse->Call(mouseuppycbk, "(O)", ((object)ptr((TextElement*)element)).ptr()); break;
		}
	}
}
void HUD::PyMouseEventListener::RaiseMouseMove(IHUD::LPELEMENT element)
{
	if(mousemovepycbk)
	{
		switch(element->type)
		{
		case ET_IMAGE: pse->Call(mousemovepycbk, "(O)", ((object)ptr((ImageElement*)element)).ptr()); break;
		case ET_TEXT: pse->Call(mousemovepycbk, "(O)", ((object)ptr((TextElement*)element)).ptr()); break;
		}
	}
}
void HUD::PyMouseEventListener::RaiseMouseEnter(IHUD::LPELEMENT element)
{
	if(mouseenterpycbk)
	{
		switch(element->type)
		{
		case ET_IMAGE: pse->Call(mouseenterpycbk, "(O)", ((object)ptr((ImageElement*)element)).ptr()); break;
		case ET_TEXT: pse->Call(mouseenterpycbk, "(O)", ((object)ptr((TextElement*)element)).ptr()); break;
		}
	}
}
void HUD::PyMouseEventListener::RaiseMouseLeave(IHUD::LPELEMENT element)
{
	if(mouseleavepycbk)
	{
		switch(element->type)
		{
		case ET_IMAGE: pse->Call(mouseleavepycbk, "(O)", ((object)ptr((ImageElement*)element)).ptr()); break;
		case ET_TEXT: pse->Call(mouseleavepycbk, "(O)", ((object)ptr((TextElement*)element)).ptr()); break;
		}
	}
}

class SimpleVertexBuffer : public std::vector<Vector3>
{
public:
	SimpleVertexBuffer() : std::vector<Vector3>() {}
	template<class _Iter> SimpleVertexBuffer(_Iter _First, _Iter _Last) : std::vector<Vector3>(_First, _Last) {}
	PtrWrapper GetPointer() {return PtrWrapper(this->size() ? &this->operator[](0) : NULL);}
	UINT GetVertexCount() {return this->size();}
};

void SetBoxTypeTextureIndex(BoxedLevelBoxType& boxtype, int idx, BYTE textureidx) {boxtype.textureindex[idx] = textureidx;}
BYTE GetBoxTypeTextureIndex(const BoxedLevelBoxType& boxtype, int idx) {return boxtype.textureindex[idx];}

struct PyChunk : public BoxedLevelChunk
{
	struct BoxMask
	{
		BoxedLevelChunk* parent;
		BoxedLevelBoxDesc* boxmask;

		BoxMask(const BoxMask& other) : parent(other.parent), boxmask(other.boxmask) {}
		BoxMask(BoxedLevelChunk* parent) : parent(parent), boxmask(parent->boxmask) {}
		BoxedLevelBoxDesc* Get(int idx) const {return &boxmask[idx];}
		void Set(int idx, BoxedLevelBoxDesc* boxdesc) {boxmask[idx] = *boxdesc;}
		void Allocate(int size) {parent->boxmask = boxmask = new BoxedLevelBoxDesc[size];}
		void Free() {delete[] boxmask;}
		boost::python::object GetBytes(int numboxes) {return object(handle<>(PyMemoryView_FromMemory((char*)boxmask, numboxes * sizeof(BoxedLevelBoxDesc), PyBUF_READ)));}
		void SetBytes(object pyobject)
		{
			if(PyObject_CheckBuffer(pyobject.ptr()) != 1)
			{
				PyErr_SetString(PyExc_IndexError, "Object is not a buffer");
				throw_error_already_set();
			}
			Py_buffer pybuffer;
			PyObject_GetBuffer(pyobject.ptr(), &pybuffer, 0);
			memcpy(boxmask, pybuffer.buf, pybuffer.len);
			PyBuffer_Release(&pybuffer);
		}
	};
};
PyChunk::BoxMask GetBoxMask(BoxedLevelChunk* self) {return PyChunk::BoxMask(self);}
void SetBoxMask(BoxedLevelChunk* self, PyChunk::BoxMask& boxmask)
{
	self->boxmask = boxmask.boxmask;
	boxmask.parent = self;
}
tuple GetChunkPos(const BoxedLevelChunk* self) {return make_tuple(self->chunkpos[0], self->chunkpos[1], self->chunkpos[2]);}
void SetChunkPos(BoxedLevelChunk* self, tuple chunkpos)
{
	self->chunkpos[0] = extract<int>(chunkpos[0]);
	self->chunkpos[1] = extract<int>(chunkpos[1]);
	self->chunkpos[2] = extract<int>(chunkpos[2]);
}

class PyBoxedLevel : public BoxedLevel
{
public:
	void AssignChunkWrapper(PyChunk* chunk) {AssignChunk(chunk);}
};

class PyOutputWindow : public OutputWindow
{
public:
	PyBoxedLevel* CreateBoxedLevelWrapper(tuple chunksize, UINT numchunkbuffers, LPTEXTURE texboxes, LPRENDERSHADER blevelshader)
	{
		Result rlt;
		PyBoxedLevel* blevel;
		UINT _chunksize[3];
		_chunksize[0] = extract<int>(chunksize[0]);
		_chunksize[1] = extract<int>(chunksize[1]);
		_chunksize[2] = extract<int>(chunksize[2]);
		IFFAILED(CreateBoxedLevel(_chunksize, numchunkbuffers, texboxes, blevelshader, (LPBOXEDLEVEL*)&blevel))
			throw rlt.GetLastResult();
		return blevel;
	}
	HWND GetHwndWrapper() {return wnd;}
};

class PyDirect3D : public Direct3D
{
public:
	PyOutputWindow* CreateOutputWindowWrapper(IOutputWindow::Settings* settings)
	{
		Result rlt;
		PyOutputWindow* wnd;
		IFFAILED(CreateOutputWindow(settings, (LPOUTPUTWINDOW*)&wnd))
			throw rlt.GetLastResult();
		return wnd;
	}
};

BOOST_PYTHON_MODULE(Direct3D)
{
	// >>> Enums

	enum_<D3DX_IMAGE_FILEFORMAT>("ImageFileFormat")
		.value("BMP", D3DX11_IFF_BMP)
		.value("JPG", D3DX11_IFF_JPG)
		.value("PNG", D3DX11_IFF_PNG)
		.value("DDS", D3DX11_IFF_DDS)
		.value("TIFF", D3DX11_IFF_TIFF)
		.value("GIF", D3DX11_IFF_GIF)
		.value("WMP", D3DX11_IFF_WMP)
	;

	enum_<RenderType>("RenderType")
		.value("Background", RT_BACKGROUND)
		.value("Default", RT_DEFAULT)
		.value("Fast", RT_FAST)
		.value("Foreground", RT_FOREGROUND)
	;

	enum_<OrientationMode>("OrientationMode")
		.value("Euler", OM_EULER)
		.value("Quaternion", OM_QUAT)
		.value("Matrix", OM_MATRIX)
	;

	enum_<ITexture::Usage>("TextureUsage")
		.value("Skin", ITexture::TU_SKIN)
		.value("Skin_NoMipmap", ITexture::TU_SKIN_NOMIPMAP)
		.value("Source", ITexture::TU_SOURCE)
		.value("RenderTarget", ITexture::TU_RENDERTARGET)
		.value("RenderTargetSource", ITexture::TU_RENDERTARGETORSOURCE)
		.value("AllAccess", ITexture::TU_ALLACCESS)
	;

	enum_<ICamera::ViewMode>("ViewMode")
		.value("Default", ICamera::VM_DEFAULT)
		.value("Fly", ICamera::VM_FLY)
		.value("FirstPerson", ICamera::VM_FIRSTPERSON)
		.value("ThirdPerson", ICamera::VM_THIRDPERSON)
	;

	enum_<ILight::LightType>("LightType")
		.value("Directional", ILight::LT_DIRECTIONAL)
		.value("Point", ILight::LT_POINT)
		.value("Spot", ILight::LT_SPOT)
	;

	enum_<BoxedLevelBoxShape>("BoxShape")
		.value("Cubic", SHAPE_CUBIC)
		.value("Stairs", SHAPE_STAIRS)
	;

	// >>> Structs

	class_<Texture::Pixel>("Pixel")
		.def_readwrite("r", &Texture::Pixel::r)
		.def_readwrite("g", &Texture::Pixel::r)
		.def_readwrite("b", &Texture::Pixel::r)
		.def_readwrite("a", &Texture::Pixel::r)
	;
	class_<std::vector<Texture::Pixel>>("PixelBuffer")
        .def(vector_indexing_suite<std::vector<Texture::Pixel>>())
    ;

	class_<D3dStartupSettings>("D3dStartupSettings")
		.def_readwrite("isfullscreen", &D3dStartupSettings::isfullscreen)
	;

	class_<D3DMaterial>("D3DMaterial")
		.def_readwrite("ambient", &D3DMaterial::ambient)
		.def_readwrite("diffuse", &D3DMaterial::diffuse)
		.def_readwrite("specular", &D3DMaterial::specular)
		.def_readwrite("power", &D3DMaterial::power)
		.def_readwrite("tex", &D3DMaterial::tex)
	;

	class_<PyOutputWindow::Settings>("OutputWindowSettings")
		.def_readwrite("backbuffersize", &PyOutputWindow::Settings::backbuffersize)
		.def_readwrite("wnd", &PyOutputWindow::Settings::wnd)
		.def_readwrite("enablemultisampling", &PyOutputWindow::Settings::enablemultisampling)
		.def_readwrite("screenidx", &PyOutputWindow::Settings::screenidx)
	;

	class_<Sprite>("Sprite")
		.def_readwrite("worldmatrix", &Sprite::worldmatrix)
		.def_readwrite("texcoord", &Sprite::texcoord)
		.def_readwrite("texsize", &Sprite::texsize)
		.def_readwrite("color", &Sprite::color)
		.def_readwrite("tex", &Sprite::tex)
	;

	class_<FontType>("FontType", no_init)
		.def(init<const String, float, optional<bool, bool>>())
		.def_readwrite("family", &FontType::family)
		.def_readwrite("size", &FontType::size)
		.def_readwrite("bold", &FontType::bold)
		.def_readwrite("italic", &FontType::italic)
	;

	class_<D3dPlaneShapeDesc>("D3dPlaneShapeDesc")
		.def_readwrite("pos", &D3dPlaneShapeDesc::pos)
		.def_readwrite("nml", &D3dPlaneShapeDesc::nml)
		.def_readwrite("size", &D3dPlaneShapeDesc::size)
		.def("GetVertexCount", &D3dPlaneShapeDesc::GetVertexCount)
		.def("GetFaceCount", &D3dPlaneShapeDesc::GetFaceCount)
	;
	objects::register_conversion<D3dPlaneShapeDesc, D3dShapeDesc>();
	class_<D3dBoxShapeDesc>("D3dBoxShapeDesc")
		.def_readwrite("pos", &D3dBoxShapeDesc::pos)
		.def_readwrite("size", &D3dBoxShapeDesc::size)
		.def("GetVertexCount", &D3dBoxShapeDesc::GetVertexCount)
		.def("GetFaceCount", &D3dBoxShapeDesc::GetFaceCount)
	;
	objects::register_conversion<D3dBoxShapeDesc, D3dShapeDesc>();
	class_<D3dSphereShapeDesc>("D3dSphereShapeDesc")
		.def_readwrite("pos", &D3dSphereShapeDesc::pos)
		.def_readwrite("rot_y", &D3dSphereShapeDesc::rot_y)
		.def_readwrite("radius", &D3dSphereShapeDesc::radius)
		.def_readwrite("stacks", &D3dSphereShapeDesc::stacks)
		.def_readwrite("slices", &D3dSphereShapeDesc::slices)
		.def_readwrite("theta", &D3dSphereShapeDesc::theta)
		.def_readwrite("phi", &D3dSphereShapeDesc::phi)
		.def("GetVertexCount", &D3dSphereShapeDesc::GetVertexCount)
		.def("GetFaceCount", &D3dSphereShapeDesc::GetFaceCount)
	;
	objects::register_conversion<D3dSphereShapeDesc, D3dShapeDesc>();
	class_<D3dCapsuleShapeDesc>("D3dCapsuleShapeDesc")
		.def_readwrite("v0", &D3dCapsuleShapeDesc::v0)
		.def_readwrite("v1", &D3dCapsuleShapeDesc::v1)
		.def_readwrite("radius", &D3dCapsuleShapeDesc::radius)
		.def_readwrite("stacks", &D3dCapsuleShapeDesc::stacks)
		.def_readwrite("slices", &D3dCapsuleShapeDesc::slices)
		.def("GetVertexCount", &D3dCapsuleShapeDesc::GetVertexCount)
		.def("GetFaceCount", &D3dCapsuleShapeDesc::GetFaceCount)
	;
	objects::register_conversion<D3dCapsuleShapeDesc, D3dShapeDesc>();
	class_<D3dCylinderShapeDesc>("D3dCylinderShapeDesc")
		.def_readwrite("v0", &D3dCylinderShapeDesc::v0)
		.def_readwrite("v1", &D3dCylinderShapeDesc::v1)
		.def_readwrite("radius", &D3dCylinderShapeDesc::radius)
		.def_readwrite("slices", &D3dCylinderShapeDesc::slices)
		.def_readwrite("hasfrontfaces", &D3dCylinderShapeDesc::hasfrontfaces)
		.def("GetVertexCount", &D3dCylinderShapeDesc::GetVertexCount)
		.def("GetFaceCount", &D3dCylinderShapeDesc::GetFaceCount)
	;
	objects::register_conversion<D3dCylinderShapeDesc, D3dShapeDesc>();

	class_<BoxedLevelBoxType>("BoxType")
		.add_property("front", &BoxedLevelBoxType::GetFront, &BoxedLevelBoxType::SetFront)
		.add_property("back", &BoxedLevelBoxType::GetBack, &BoxedLevelBoxType::SetBack)
		.add_property("left", &BoxedLevelBoxType::GetLeft, &BoxedLevelBoxType::SetLeft)
		.add_property("right", &BoxedLevelBoxType::GetRight, &BoxedLevelBoxType::SetRight)
		.add_property("top", &BoxedLevelBoxType::GetTop, &BoxedLevelBoxType::SetTop)
		.add_property("bottom", &BoxedLevelBoxType::GetBottom, &BoxedLevelBoxType::SetBottom)
		.def_readwrite("shape", &BoxedLevelBoxType::shape)
		.def("SetTextureIndices", &BoxedLevelBoxType::SetTextureIndices)
		.def("__getitem__", &GetBoxTypeTextureIndex)
		.def("__setitem__", &SetBoxTypeTextureIndex)
	;
	class_<BoxedLevelBoxDesc>("BoxDesc")
		.def(init<>())
		.def(init<UINT32, optional<UINT32>>())
		.def_readwrite("id", &BoxedLevelBoxDesc::id)
	;
	{
		scope s = class_<BoxedLevelChunk>("Chunk")
			.def_readwrite("numboxes", &PyChunk::numboxes)
			.add_property("chunkpos", &GetChunkPos, &SetChunkPos)
			.add_property("boxmask", &GetBoxMask, &SetBoxMask)
		;
		objects::register_conversion<PyChunk, BoxedLevelChunk>();
		class_<PyChunk::BoxMask>("BoxMask", no_init)
			.def("__getitem__", &PyChunk::BoxMask::Get, POLICY_MANAGED_BY_C)
			.def("__setitem__", &PyChunk::BoxMask::Set)
			.def("Allocate", &PyChunk::BoxMask::Allocate)
			.def("Free", &PyChunk::BoxMask::Free)
			.def("GetBytes", &PyChunk::BoxMask::GetBytes)
			.def("SetBytes", &PyChunk::BoxMask::SetBytes)
		;
	}

	class_<Object::Vertex>("Vertex")
		.def_readwrite("pos", &Object::Vertex::pos)
		.def_readwrite("nml", &Object::Vertex::nml)
		.def_readwrite("texcoord", &Object::Vertex::texcoord)
		.def_readwrite("blendidx", &Object::Vertex::blendidx)
	;

	class_<Object::BoundingSphere>("BoundingSphere")
		.def_readwrite("center", &Object::BoundingSphere::center)
		.def_readwrite("radius", &Object::BoundingSphere::radius)
	;

	class_<Object::BoundingBox>("BoundingBox")
		.def_readwrite("center", &Object::BoundingBox::center)
		.def_readwrite("size", &Object::BoundingBox::size)
	;

	class_<HUD::ImageElement, boost::noncopyable>("ImageElement", no_init)
		.def_readwrite("visible", &HUD::ImageElement::visible)
		.def_readwrite("color", &HUD::ImageElement::color)
		.add_property("user", &HUD::TextElement::GetPyUser, &HUD::TextElement::SetPyUser)
		.add_property("x", &HUD::ImageElement::GetX, &HUD::ImageElement::SetX)
		.add_property("y", &HUD::ImageElement::GetY, &HUD::ImageElement::SetY)
		.add_property("dock", &HUD::ImageElement::GetDock, &HUD::ImageElement::SetDock)
	;
	objects::register_conversion<IHUD::IImageElement, HUD::ImageElement>();
	objects::register_conversion<HUD::ImageElement, IHUD::IImageElement>();

	objects::register_conversion<IHUD::IImageElement, IHUD::IElement>();
	objects::register_conversion<IHUD::IElement, IHUD::IImageElement>();

	class_<HUD::TextElement, boost::noncopyable>("TextElement", no_init)
		.def_readwrite("visible", &HUD::TextElement::visible)
		.def_readwrite("color", &HUD::TextElement::color)
		.add_property("user", &HUD::TextElement::GetPyUser, &HUD::TextElement::SetPyUser)
		.add_property("x", &HUD::TextElement::GetX, &HUD::TextElement::SetX)
		.add_property("y", &HUD::TextElement::GetY, &HUD::TextElement::SetY)
		.add_property("dock", &HUD::TextElement::GetDock, &HUD::TextElement::SetDock)
		.def_readwrite("text", &HUD::TextElement::text)
		.def_readwrite("font", &HUD::TextElement::font)
		.def_readwrite("textflags", &HUD::TextElement::textflags)
	;
	objects::register_conversion<IHUD::ITextElement, HUD::TextElement>();
	objects::register_conversion<HUD::TextElement, IHUD::ITextElement>();

	objects::register_conversion<IHUD::ITextElement, IHUD::IElement>();
	objects::register_conversion<IHUD::IElement, IHUD::ITextElement>();

	// >>> Classes

	class_<IMoveable, boost::noncopyable>("IMoveable", no_init)
	;

	class_<IRenderable, boost::noncopyable>("IRenderable", no_init)
	;

	class_<IUpdateable, boost::noncopyable>("IUpdateable", no_init)
	;

	class_<RenderShader, boost::noncopyable>("RenderShader", no_init)
	;
	objects::register_conversion<IRenderShader, RenderShader>();
	objects::register_conversion<RenderShader, IRenderShader>();

	Result (Texture::*SaveToFile)(const FilePath&, D3DX_IMAGE_FILEFORMAT) = &Texture::Save;
	Result (Texture::*SaveToMemory)(LPD3D10BLOB* datablob, D3DX_IMAGE_FILEFORMAT) = &Texture::Save;
	class_<Texture, boost::noncopyable>("Texture", no_init)
		.def("GetWidth", &Texture::GetWidth)
		.def("GetHeight", &Texture::GetHeight)
		.def("GetFilename", &Texture::GetFilename)
		.def("MapPixelData", &Texture::MapPixelDataWrapper, POLICY_MANAGED_BY_C)
		.def("UnmapData", &Texture::UnmapDataWrapper)
		.def("Save", SaveToFile)
		.def("Save", SaveToMemory)
	;
	objects::register_conversion<ITexture, Texture>();
	objects::register_conversion<Texture, ITexture>();

	class_<std::vector<D3DMaterial>>("MaterialArray")
        .def(vector_indexing_suite<std::vector<D3DMaterial>>())
    ;
	class_<std::vector<Matrix>>("MatrixArray")
        .def(vector_indexing_suite<std::vector<Matrix>>())
    ;
	class_<Object::VertexBuffer, boost::noncopyable>("VertexBuffer")
        .def(vector_indexing_suite<Object::VertexBuffer>())
		.def("GetPointer", &Object::VertexBuffer::GetPointer)
		.def("GetVertexCount", &Object::VertexBuffer::GetVertexCount)
    ;
	class_<SimpleVertexBuffer, boost::noncopyable>("SimpleVertexBuffer")
        .def(vector_indexing_suite<SimpleVertexBuffer>())
		.def("GetPointer", &SimpleVertexBuffer::GetPointer)
		.def("GetVertexCount", &SimpleVertexBuffer::GetVertexCount)
    ;
	class_<Object::IndexBuffer, boost::noncopyable>("IndexBuffer")
        .def(vector_indexing_suite<Object::IndexBuffer>())
		.def("GetPointer", &Object::IndexBuffer::GetPointer)
		.def("GetFaceCount", &Object::IndexBuffer::GetFaceCount)
		.def("GetIndexCount", &Object::IndexBuffer::GetIndexCount)
    ;

	class_<std::shared_ptr<void>>("VoidPtr");

	class_<Object, bases<IRenderable>, boost::noncopyable>("Object", no_init)
		.def_readwrite("orientmode", &Object::orientmode)
		.def_readwrite("bonetransform", &Object::bonetransform)
		.def_readwrite("pos", &Object::pos)
		.def_readwrite("rot", &Object::rot)
		.def_readwrite("qrot", &Object::qrot)
		.def_readwrite("scl", &Object::scl)
		.def("GetMats", &Object::GetMatsWrapper, POLICY_MANAGED_BY_C)
		.def("GetNumBones", &Object::GetNumBones)
		.def("CreateCollisionMesh", &Object::CreateCollisionMesh)
		.def("CreateConvexCollisionHull", &Object::CreateConvexCollisionHull)
		.def("CreateCollisionCapsule", &Object::CreateCollisionCapsule)
		.def("CreateCollisionRagdoll", &Object::CreateCollisionRagdollWrapper/*, boost::python::return_value_policy<boost::python::return_opaque_pointer>()*/)//EDIT
		.def("SetShader", &Object::SetShader)
		.def("SetVertexData", &Object::SetVertexData)
		.def("SetIndexData", &Object::SetIndexData)
		.def("GetVertexCount", &Object::GetVertexCount)
		.def("GetIndexCount", &Object::GetIndexCount)
		.def("MapVertexData", &Object::MapVertexDataWrapper, POLICY_MANAGED_BY_C)
		.def("MapIndexData", &Object::MapIndexDataWrapper, POLICY_MANAGED_BY_C)
		.def("UnmapData", &Object::UnmapVertexDataWrapper)
		.def("UnmapData", &Object::UnmapIndexDataWrapper)
		.def("CommitChanges", &Object::CommitChanges)
		.def("ComputeNormals", &Object::ComputeNormals)
		.def("GetBoundingSphere", &Object::GetBoundingSphere, POLICY_MANAGED_BY_C)
		.def("GetBoundingSphereCenter", &Object::GetBoundingSphereCenter, POLICY_MANAGED_BY_C)
		.def("GetBoundingSphereRadius", &Object::GetBoundingSphereRadius)
		.def("GetBoundingBox", &Object::GetBoundingBox, POLICY_MANAGED_BY_C)
		.def("GetBoundingBoxCenter", &Object::GetBoundingBoxCenter, POLICY_MANAGED_BY_C)
		.def("GetBoundingBoxSize", &Object::GetBoundingBoxSize, POLICY_MANAGED_BY_C)
	;
	objects::register_conversion<IObject, Object>();
	objects::register_conversion<Object, IObject>();

	class_<Camera, bases<IUpdateable>, boost::noncopyable>("Camera", no_init)
		.def_readwrite("viewmode", &Camera::viewmode)
		.def_readwrite("orientmode", &Camera::orientmode)
		.def_readwrite("pos", &Camera::pos)
		.def_readwrite("rot", &Camera::rot)
		.def_readwrite("qrot", &Camera::qrot)
	;
	objects::register_conversion<ICamera, Camera>();
	objects::register_conversion<Camera, ICamera>();

	class_<Light, bases<IMoveable>, boost::noncopyable>("Light", no_init)
		.def("GetPos", &Light::GetPos, POLICY_MANAGED_BY_C)
		.def("SetPos", &Light::SetPos)
		.def("GetDir", &Light::GetDir, POLICY_MANAGED_BY_C)
		.def("SetDir", &Light::SetDir)
		/*.def("GetColor", &Light::GetColor)
		.def("SetColor", &Light::SetColor)
		.def("GetType", &Light::GetType)
		.def("SetType", &Light::SetType)
		.def("GetAttenuation", &Light::GetAttenuation)
		.def("SetAttenuation", &Light::SetAttenuation)
		.def("GetSpotPower", &Light::GetSpotPower)
		.def("SetSpotPower", &Light::SetSpotPower)
		.def("GetEnabled", &Light::GetEnabled)
		.def("SetEnabled", &Light::SetEnabled)*/
	;
	objects::register_conversion<ILight, Light>();
	objects::register_conversion<Light, ILight>();

	object d3dfont = class_<TextRenderer::Font, boost::noncopyable>("D3dFont", no_init)
		.def("GetType", &TextRenderer::Font::GetType, POLICY_MANAGED_BY_C)
		.def("DrawTextImmediate", &TextRenderer::Font::DrawTextImmediate)
		.def("DrawTextDelayed", &TextRenderer::Font::DrawTextDelayed)
		.def("MeasureText", &TextRenderer::Font::MeasureText)
		.def("Release", &TextRenderer::Font::Release)
	;
	d3dfont.attr("DF_NOCLIP") = 0x1;
	d3dfont.attr("DF_SINGLELINE") = 0x2;
	d3dfont.attr("DF_BREAKWORDS") = 0x4;
	objects::register_conversion<TextRenderer::Font, ID3dFont>();

	class_<SkyBox, bases<IRenderable>, boost::noncopyable>("SkyBox", no_init)
		.def("CreateLayer", &SkyBox::CreateLayer)
	;

	void (std::vector<BoxedLevelBoxType>::*ResizeBoxTypeList)(std::vector<BoxedLevelBoxType>::size_type) = &std::vector<BoxedLevelBoxType>::resize;
	class_<std::vector<BoxedLevelBoxType>>("BoxTypeList")
        .def(vector_indexing_suite<std::vector<BoxedLevelBoxType>>())
		.def("resize", ResizeBoxTypeList)
		.def("size", &std::vector<BoxedLevelBoxType>::size)
    ;

	class_<PyBoxedLevel, bases<IRenderable>, boost::noncopyable>("BoxedLevel", no_init)
		.def("SetBoxTypes", &PyBoxedLevel::SetBoxTypesWrapper)
		.def("AssignChunk", &PyBoxedLevel::AssignChunk)
		.def("UnassignChunk", &PyBoxedLevel::UnassignChunk)
	;

	class_<HUD, bases<IRenderable>, boost::noncopyable>("HUD", no_init)
		.def("CreateElement", &HUD::CreateElementWrapper1, POLICY_MANAGED_BY_C)
		.def("CreateElement", &HUD::CreateElementWrapper2, POLICY_MANAGED_BY_C)
		.def("CreateElement", &HUD::CreateElementWrapper3, POLICY_MANAGED_BY_C)
		.def("RemoveElement", &HUD::RemoveElement)
		.def("RegisterMouseDownListener", &HUD::RegisterPyMouseDownListener)
		.def("RegisterMouseUpListener", &HUD::RegisterPyMouseUpListener)
		.def("RegisterMouseMoveListener", &HUD::RegisterPyMouseMoveListener)
		.def("RegisterMouseEnterListener", &HUD::RegisterPyMouseEnterListener)
		.def("RegisterMouseLeaveListener", &HUD::RegisterPyMouseLeaveListener)
		.def("Update", &HUD::Update)
	;

	class_<D3dCursor, bases<IRenderable>, boost::noncopyable>("Cursor", no_init)
		.def_readwrite("tex", &D3dCursor::tex)
		.def_readwrite("animated", &D3dCursor::animated)
		.def_readwrite("hotspot", &D3dCursor::hotspot)
		.def("Release", &D3dCursor::Release)
	;

	void (PyOutputWindow::*RegisterClassForRendering)(IRenderable*, RenderType) = &PyOutputWindow::RegisterForRendering;
	void (PyOutputWindow::*RegisterPyFuncForRendering)(PyObject*, RenderType) = &PyOutputWindow::RegisterForRendering;
	void (PyOutputWindow::*DeregisterClassFromRendering)(IRenderable*, RenderType) = &PyOutputWindow::DeregisterFromRendering;
	void (PyOutputWindow::*DeregisterPyFuncFromRendering)(PyObject*, RenderType) = &PyOutputWindow::DeregisterFromRendering;
	void (PyOutputWindow::*DrawLineV3)(const Vector3&, const Vector3&, Color, Color, RenderType rendertype) = &PyOutputWindow::DrawLine;
	void (PyOutputWindow::*DrawLineV2)(const Vector2&, const Vector2&, Color, Color, RenderType rendertype) = &PyOutputWindow::DrawLine;
	class_<PyOutputWindow, boost::noncopyable>("OutputWindow", no_init)
		.def_readwrite("backcolor", &PyOutputWindow::backcolor)
		.def_readwrite("backcolorenabled", &PyOutputWindow::backcolorenabled)
		.def_readonly("size", &PyOutputWindow::GetBackbufferSize)
		.def_readonly("width", &PyOutputWindow::GetBackbufferWidth)
		.def_readonly("height", &PyOutputWindow::GetBackbufferHeight)
		.add_property("hwnd", make_function(&PyOutputWindow::GetHwndWrapper, POLICY_MANAGED_BY_C))
		.def("RegisterClassForRendering", RegisterClassForRendering)
		.def("RegisterFunctionForRendering", RegisterPyFuncForRendering)
		.def("DeregisterClassFromRendering", DeregisterClassFromRendering)
		.def("DeregisterPyFuncFromRendering", DeregisterPyFuncFromRendering)
		.def("RegisterForUpdating", &PyOutputWindow::RegisterForUpdating)
		.def("DeregisterFromUpdating", &PyOutputWindow::DeregisterFromUpdating)
		.def("SetCamera", &PyOutputWindow::SetCamera)
		.def("MakeConsoleTarget", &PyOutputWindow::MakeConsoleTarget)
		.def("DrawLine", DrawLineV3)
		.def("DrawLine", DrawLineV2)
		.def("EnableSprites", &PyOutputWindow::EnableSprites)
		.def("EnableLineDrawing", &PyOutputWindow::EnableLineDrawing)
		.def("ShowFps", &PyOutputWindow::ShowFps)
		.def("HideFps", &PyOutputWindow::HideFps)
		.def("CreateRenderShader", &PyOutputWindow::CreateRenderShaderWrapper, POLICY_MANAGED_BY_C)
		.def("CreateTexture", &PyOutputWindow::CreateTextureFromFileWrapper, POLICY_MANAGED_BY_C)
		.def("CreateTexture", &PyOutputWindow::CreateNewTextureWrapper, POLICY_MANAGED_BY_C)
		.def("CreateObject", &PyOutputWindow::CreateObjectFromObjectWrapper, POLICY_MANAGED_BY_C)
		.def("CreateObject", &PyOutputWindow::CreateObjectFromFileWrapper, POLICY_MANAGED_BY_C)
		.def("CreateObject", &PyOutputWindow::CreateObjectFromShapeWrapper, POLICY_MANAGED_BY_C)
		.def("CreateCursor", &PyOutputWindow::CreateCursorWrapper, POLICY_MANAGED_BY_C)
		.def("CreateHUD", &PyOutputWindow::CreateHUDWrapper, POLICY_MANAGED_BY_C)
		.def("CreateD3dFont", &PyOutputWindow::CreateD3dFontWrapper, POLICY_MANAGED_BY_C)
		.def("CreateSkyBox", &PyOutputWindow::CreateSkyBoxWrapper, POLICY_MANAGED_BY_C)
		.def("CreateBoxedLevel", &PyOutputWindow::CreateBoxedLevelWrapper, POLICY_MANAGED_BY_C)
	;
	objects::register_conversion<IOutputWindow, PyOutputWindow>();
	objects::register_conversion<PyOutputWindow, IOutputWindow>();

	class_<PyDirect3D, boost::noncopyable>("Direct3D")
		.def("Init", &PyDirect3D::Init)
		.def("Render", &PyDirect3D::Render)
		.def("TakeScreenShots", &PyDirect3D::TakeScreenShots)
		.def("CreateOutputWindow", &PyDirect3D::CreateOutputWindowWrapper, POLICY_MANAGED_BY_C)
		.def("CreateCamera", &PyDirect3D::CreateCameraWrapper, POLICY_MANAGED_BY_C)
		.def("CreateLight", &PyDirect3D::CreateLightWrapper, POLICY_MANAGED_BY_C)
	;
}

DIRECT3D_EXTERN_FUNC void __cdecl RegisterScriptableDirect3DClasses(const IPythonScriptEngine* pse)
{
	if(pse)
		pse->AddModule("Direct3D", &PyInit_Direct3D);
}
void SyncWithPython()
{
	AddToBuiltins("d3d", (PyDirect3D*)&*d3d);
}

std::shared_ptr<void> BoxedLevel::GetPyPtr()
{
	return pse->GetHandle(api::object(ptr(this)).ptr());
}
#else
#include "Direct3D.h"
#include <memory>
void OutputWindow::CallPyRenderFunc(PyObject* pyfunc) {}
void HUD::PyMouseEventListener::RaiseMouseDown(IHUD::LPELEMENT element) {}
void HUD::PyMouseEventListener::RaiseMouseUp(IHUD::LPELEMENT element) {}
void HUD::PyMouseEventListener::RaiseMouseMove(IHUD::LPELEMENT element) {}
void HUD::PyMouseEventListener::RaiseMouseEnter(IHUD::LPELEMENT element) {}
void HUD::PyMouseEventListener::RaiseMouseLeave(IHUD::LPELEMENT element) {}
std::shared_ptr<void> BoxedLevel::GetPyPtr() {return NULL;}
void SyncWithPython() {}
#endif