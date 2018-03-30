#ifdef HAVOK_SCRIPTABLE
#include "Havok.h"
#include "ISPythonScriptEngine.h"

//#define BOOST_PYTHON_STATIC_LIB
#include <boost\python.hpp>
#include <boost\python\suite\indexing\vector_indexing_suite.hpp>
using namespace boost::python;




/*struct pystring_converter
{
	/// @note Registers converter from a python interable type to the
	///       provided type.
	template<typename Container> pystring_converter& from_python()
	{
		boost::python::converter::registry::push_back(&pystring_converter::convertible, &pystring_converter::construct<Container>, boost::python::type_id<Container>());
		return *this;
	}

	/// @brief Check if PyObject is a string.
	static void* convertible(PyObject* object)
	{
		return object;
	}

	/// @brief Convert PyString to Container.
	///
	/// Container Concept requirements:
	///
	///   * Container::value_type is CopyConstructable from char.
	///   * Container can be constructed and populated with two iterators.
	///     I.e. Container(begin, end)
	template<typename Container> static void construct(PyObject* object, boost::python::converter::rvalue_from_python_stage1_data* data)
	{
		namespace python = boost::python;
		// Object is a borrowed reference, so create a handle indicting it is borrowed for proper reference counting.
		python::handle<> handle(python::borrowed(object));

		// Obtain a handle to the memory block that the converter has allocated for the C++ type.
		typedef python::converter::rvalue_from_python_storage<Container> storage_type;
		void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

		// Allocate the C++ type into the converter's memory block, and assign
		// its handle to the converter's convertible variable. The C++
		// container is populated by passing the begin and end iterators of
		// the python object to the container's constructor.
		data->convertible = PyCapsule_GetPointer(object, PyCapsule_GetName(object));
	}
};*/

template<class T> struct vector_from_python
{
	vector_from_python()
	{
		boost::python::converter::registry::push_back(&convertible, &construct, boost::python::type_id<std::vector<T>>());
	}

	static void* convertible(PyObject* obj_ptr)
	{
		if(!(PyList_Check(obj_ptr) || PyTuple_Check(obj_ptr) || PyIter_Check(obj_ptr) || PyRange_Check(obj_ptr))) {
			return 0;
		}

		PyObject * first_obj = PyObject_GetItem(obj_ptr,
		PyLong_FromLong(0));
		if(!PyObject_TypeCheck(first_obj, &PyFloat_Type) ) {
			return 0;
		}

		return obj_ptr;
	}


	static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data)
	{
		void* storage = ((boost::python::converter::rvalue_from_python_storage<std::vector<T>>*)data)->storage.bytes;
		new (storage) std::vector<T>();

		data->convertible = storage;

		std::vector<T>& result = *((std::vector<T>*) storage);

		boost::python::handle<> obj_iter(PyObject_GetIter(obj_ptr));

		while(1) {
		boost::python::handle<>
		py_elem_hdl(boost::python::allow_null(PyIter_Next(obj_iter.get())));
		if (PyErr_Occurred()) {
		boost::python::throw_error_already_set();
		}

		if (!py_elem_hdl.get()) {
		break;
		}

		boost::python::object py_elem_obj(py_elem_hdl);
		boost::python::extract<T> elem_proxy(py_elem_obj);
		result.push_back(elem_proxy());
		}
	}
};




struct PyVehicleDesc : public VehicleDesc
{
	struct Wheel : public VehicleDesc::Wheel
	{
		void SetDefault1() {SetDefault();}
		void SetDefault2(int axle) {SetDefault(axle);}
		void SetDefault3(int axle, float radius) {SetDefault(axle, radius);}
		void SetDefault4(int axle, float radius, float width) {SetDefault(axle, radius, width);}
		void SetDefault5(int axle, float radius, float width, float mass) {SetDefault(axle, radius, width, mass);}
		void SetDefault6(int axle, float radius, float width, float mass, float friction) {SetDefault(axle, radius, width, mass, friction);}
		bool operator==(const PyVehicleDesc::Wheel& other) const
			{return memcmp(this, &other, sizeof(PyVehicleDesc::Wheel)) == 0;}
	};
	std::vector<Wheel>* GetWheels() {return (std::vector<Wheel>*)&wheels;}
	struct Engine : public VehicleDesc::Engine
	{
		void SetDefault1() {SetDefault();}
		void SetDefault2(float maxrpm) {SetDefault(maxrpm);}
		void SetDefault3(float maxrpm, float maxtorque) {SetDefault(maxrpm, maxtorque);}
		void SetDefault4(float maxrpm, float maxtorque, float optrpm) {SetDefault(maxrpm, maxtorque, optrpm);}
		void SetDefault5(float maxrpm, float maxtorque, float optrpm, float minrpm) {SetDefault(maxrpm, maxtorque, optrpm, minrpm);}
	};
	Engine* GetEngine() {return (Engine*)&engine;}
};

class PyHavok : public Havok
{
public:
	BoxedLevelEntity* CreateBoxedLevelEntityWrapper(tuple chunksize)
	{
		Result rlt;
		BoxedLevelEntity* entity;
		UINT _chunksize[3];
		_chunksize[0] = extract<int>(chunksize[0]);
		_chunksize[1] = extract<int>(chunksize[1]);
		_chunksize[2] = extract<int>(chunksize[2]);
		IFFAILED(CreateBoxedLevelEntity(_chunksize, (IBoxedLevelEntity**)&entity))
			throw rlt.GetLastResult();
		return entity;
	}
	Vehicle* CreateVehicleWrapper(const PyVehicleDesc* vehicledesc, Vector3* vpos, Quaternion* qrot)
	{
		Result rlt;
		Vehicle* vehicle;
		IFFAILED(CreateVehicle((VehicleDesc*)vehicledesc, vpos, qrot, (IVehicle**)&vehicle))
			throw rlt.GetLastResult();
		return vehicle;
	}
	RegularEntity* CreateCollisionMeshWrapper(LPOBJECT obj)
	{
		Result rlt;
		RegularEntity* entity;
		IFFAILED(CreateCollisionMesh(obj, (IRegularEntity**)&entity))
			throw rlt.GetLastResult();
		return entity;
	}
};

BOOST_PYTHON_MODULE(Havok)
{
	// >>> Enums

	enum_<VisualizationType>("VisualizationType")
		.value("Disabled", VT_NONE)
		.value("Local", VT_LOCAL)
		.value("VisualDebugger", VT_VISUALDEBUGGER)
	;

	enum_<HvkViewer>("HvkViewer")
		.value("BroadPhase", HV_BROADPHASE)
		.value("Constraint", HV_CONSTRAINT)
		.value("ActiveContactPoint", HV_ACTIVECONTACTPOINT)
		.value("InactiveContactPoint", HV_INACTIVECONTACTPOINT)
		.value("ConvexRadius", HV_CONVEXRADIUS)
		.value("InconsistendWinding", HV_INCONSISTENDWINDING)
		.value("MidPhase", HV_MIDPHASE)
		.value("MousePicking", HV_MOUSEPICKING)
		.value("PhantomDisplay", HV_PHANTOMDISPLAY)
		.value("RigidBodyCenterOfMass", HV_RIGIDBODYCENTEROFMASS)
		.value("RigidBodyInertia", HV_RIGIDBODYINERTIA)
		.value("ShapeDisplay", HV_SHAPEDISPLAY)
		.value("SimulationIsland", HV_SIMULATIONISLAND)
		.value("SingleBodyConstraint", HV_SINGLEBODYCONSTRAINT)
		.value("SweptTransformDisplay", HV_SWEPTTRANSFORMDISPLAY)
		.value("ToiContactPoint", HV_TOICONTACTPOINT)
		.value("ToiCount", HV_TOICOUNT)
		.value("Vehicle", HV_VEHICLE)
		.value("Welding", HV_WELDING)
	;

	enum_<MotionType>("MotionType")
		.value("Invalid", MT_INVALID)
		.value("Dynamic", MT_DYNAMIC)
		.value("SphereInertia", MT_SPHERE_INERTIA)
		.value("BoxInertia", MT_BOX_INERTIA)
		.value("Keyframed", MT_KEYFRAMED)
		.value("Fixed", MT_FIXED)
		.value("ThinBoxInertia", MT_THIN_BOX_INERTIA)
		.value("Character", MT_CHARACTER)
	;

	enum_<CollidableQuality>("CollidableQuality")
		.value("Invalid", CQ_INVALID)
		.value("Fixed", CQ_FIXED)
		.value("Keyframed", CQ_KEYFRAMED)
		.value("Debris", CQ_DEBRIS)
		.value("SimpleTOI", CQ_DEBRIS_SIMPLE_TOI)
		.value("Moving", CQ_MOVING)
		.value("Critical", CQ_CRITICAL)
		.value("Bullet", CQ_BULLET)
		.value("User", CQ_USER)
		.value("Character", CQ_CHARACTER)
		.value("KeyframedReporting", CQ_KEYFRAMED_REPORTING)
	;

	enum_<IHavok::Layer>("Layer")
		.value("Default", IHavok::LAYER_DEFAULT)
		.value("Landscape", IHavok::LAYER_LANDSCAPE) // Contains static geometry for the level
		.value("Proxy", IHavok::LAYER_PROXY) // Contains the proxy for the character controller
		.value("Debris", IHavok::LAYER_DEBRIS) // Contains small dynamic objects in the scene. Each of these have different masses
		.value("MovableEnvironment", IHavok::LAYER_MOVABLE_ENVIRONMENT) // Contains large dynamic objects in the scene
		.value("RagdollKeyframed", IHavok::LAYER_RAGDOLL_KEYFRAMED) // Contains the bodies of the ragdoll that are currently keyframed
		.value("Raycast", IHavok::LAYER_RAYCAST) // This layer is used to control which items to raycast against
		.value("RagdollDynamic", IHavok::LAYER_RAGDOLL_DYNAMIC) // Contains the rigid bodies for the ragdoll which are not keyframed. When the ragdoll falls all ragdoll bodies are switched into this layer
		.value("Picking", IHavok::LAYER_PICKING) // A layer that control which items are effected by the mouse pointer
		.value("Handheld", IHavok::LAYER_HANDHELD) // An attachable item
	;

	// >>> Structs

	class_<HvkGeneralShapeDesc>("HvkGeneralShapeDesc")
		.def_readwrite("mtype", &HvkGeneralShapeDesc::mtype)
		.def_readwrite("qual", &HvkGeneralShapeDesc::qual)
		.def_readwrite("mass", &HvkGeneralShapeDesc::mass)
	;
	class_<HvkShapeDesc, bases<HvkGeneralShapeDesc>>("HvkShapeDesc", no_init)
		/*.def_readwrite("mtype", &HvkShapeDesc::mtype)
		.def_readwrite("mass", &HvkShapeDesc::mass)*/
		.def("GetId", &HvkShapeDesc::GetId)
	;
	class_<std::vector<HvkShapeDesc*>>("HvkShapeDescArray")
		.def(vector_indexing_suite<std::vector<HvkShapeDesc*>>())
	;
	class_<HvkCapsuleShapeDesc, bases<HvkShapeDesc>>("HvkCapsuleShapeDesc")
		.def_readwrite("v0", &HvkCapsuleShapeDesc::v0)
		.def_readwrite("v1", &HvkCapsuleShapeDesc::v1)
		.def_readwrite("radius", &HvkCapsuleShapeDesc::radius)
	;
	class_<HvkCylinderShapeDesc, bases<HvkShapeDesc>>("HvkCylinderShapeDesc")
		.def_readwrite("v0", &HvkCylinderShapeDesc::v0)
		.def_readwrite("v1", &HvkCylinderShapeDesc::v1)
		.def_readwrite("radius", &HvkCylinderShapeDesc::radius)
	;
	class_<HvkSphereShapeDesc, bases<HvkShapeDesc>>("HvkSphereShapeDesc")
		.def_readwrite("radius", &HvkSphereShapeDesc::radius)
	;
	class_<HvkBoxShapeDesc, bases<HvkShapeDesc>>("HvkBoxShapeDesc")
		.def_readwrite("size", &HvkBoxShapeDesc::size)
	;
	class_<HvkPlaneShapeDesc, bases<HvkShapeDesc>>("HvkPlaneShapeDesc")
		.def_readwrite("vcenter", &HvkPlaneShapeDesc::vcenter)
		.def_readwrite("vdir", &HvkPlaneShapeDesc::vdir)
		.def_readwrite("vhalfext", &HvkPlaneShapeDesc::vhalfext)
	;
	class_<HvkConvexVerticesShapeDesc, bases<HvkShapeDesc>>("HvkConvexVerticesShapeDesc")
		.add_property("vertices", &HvkConvexVerticesShapeDesc::GetVertexPtr, &HvkConvexVerticesShapeDesc::SetVerticesFromVertexPtr)
		.def_readwrite("numvertices", &HvkConvexVerticesShapeDesc::numvertices)
		.def_readwrite("fvfstride", &HvkConvexVerticesShapeDesc::fvfstride)
		.def_readwrite("stride", &HvkConvexVerticesShapeDesc::fvfstride)
	;
	class_<HvkCompressedMeshShapeDesc, bases<HvkShapeDesc>>("HvkCompressedMeshShapeDesc")
		.add_property("vertices", &HvkCompressedMeshShapeDesc::GetVertexPtr, &HvkCompressedMeshShapeDesc::SetVerticesFromVertexPtr)
		.add_property("indices", &HvkCompressedMeshShapeDesc::GetIndexPtr, &HvkCompressedMeshShapeDesc::SetIndicesFromIndexPtr)
		.def_readwrite("numvertices", &HvkCompressedMeshShapeDesc::numvertices)
		.def_readwrite("numfaces", &HvkCompressedMeshShapeDesc::numfaces)
		.def_readwrite("fvfstride", &HvkCompressedMeshShapeDesc::fvfstride)
	;

	class_<ConstraintDesc>("ConstraintDesc", no_init)
	;
	class_<BallAndSocketConstraintDesc, bases<ConstraintDesc>>("BallAndSocketConstraintDesc")
		.def_readwrite("pivot", &BallAndSocketConstraintDesc::pivot)
	;
	class_<LimitedHingeConstraintDesc, bases<ConstraintDesc>>("LimitedHingeConstraintDesc")
		.def_readwrite("angleMin", &LimitedHingeConstraintDesc::angleMin)
		.def_readwrite("angleMax", &LimitedHingeConstraintDesc::angleMax)
		.def_readwrite("pivot", &LimitedHingeConstraintDesc::pivot)
		.def_readwrite("axis", &LimitedHingeConstraintDesc::axis)
	;
	class_<RagdollConstraintDesc, bases<ConstraintDesc>>("RagdollConstraintDesc")
		.def_readwrite("coneMin", &RagdollConstraintDesc::coneMin)
		.def_readwrite("coneMax", &RagdollConstraintDesc::coneMax)
		.def_readwrite("planeMin", &RagdollConstraintDesc::planeMin)
		.def_readwrite("planeMax", &RagdollConstraintDesc::planeMax)
		.def_readwrite("twistMin", &RagdollConstraintDesc::twistMin)
		.def_readwrite("twistMax", &RagdollConstraintDesc::twistMax)
		.def_readwrite("pivot", &RagdollConstraintDesc::pivot)
		.def_readwrite("twistAxis", &RagdollConstraintDesc::twistAxis)
		.def_readwrite("planeAxis", &RagdollConstraintDesc::planeAxis)
	;

	{
		scope vehicledescscope = class_<PyVehicleDesc>("VehicleDesc")
			.def_readwrite("chassis", &PyVehicleDesc::chassis)
			.add_property("wheels", make_function(&PyVehicleDesc::GetWheels, POLICY_MANAGED_BY_C))
			.add_property("engine", make_function(&PyVehicleDesc::GetEngine, POLICY_MANAGED_BY_C))
		;
		class_<std::vector<PyVehicleDesc::Wheel>>("WheelArray")
			.def(vector_indexing_suite<std::vector<PyVehicleDesc::Wheel>>())
		;
		class_<PyVehicleDesc::Wheel>("Wheel")
			.def_readwrite("axle", &PyVehicleDesc::Wheel::axle)
			.def_readwrite("friction", &PyVehicleDesc::Wheel::friction)
			.def_readwrite("slipangle", &PyVehicleDesc::Wheel::slipangle)
			.def_readwrite("radius", &PyVehicleDesc::Wheel::radius)
			.def_readwrite("width", &PyVehicleDesc::Wheel::width)
			.def_readwrite("viscosityfriction", &PyVehicleDesc::Wheel::viscosityfriction)
			.def_readwrite("maxfriction", &PyVehicleDesc::Wheel::maxfriction)
			.def_readwrite("forcefeedbackmultiplier", &PyVehicleDesc::Wheel::forcefeedbackmultiplier)
			.def("SetDefault", &PyVehicleDesc::Wheel::SetDefault1)
			.def("SetDefault", &PyVehicleDesc::Wheel::SetDefault2)
			.def("SetDefault", &PyVehicleDesc::Wheel::SetDefault3)
			.def("SetDefault", &PyVehicleDesc::Wheel::SetDefault4)
			.def("SetDefault", &PyVehicleDesc::Wheel::SetDefault5)
			.def("SetDefault", &PyVehicleDesc::Wheel::SetDefault6)
		;
		//objects::register_conversion<PyVehicleDesc::Wheel, VehicleDesc::Wheel>();
		////objects::register_conversion<VehicleDesc::Wheel, PyVehicleDesc::Wheel>();
		class_<PyVehicleDesc::Engine>("Engine")
			.def_readwrite("minrpm", &PyVehicleDesc::Engine::minrpm)
			.def_readwrite("optrpm", &PyVehicleDesc::Engine::optrpm)
			.def_readwrite("maxrpm", &PyVehicleDesc::Engine::maxrpm)
			.def_readwrite("maxtorque", &PyVehicleDesc::Engine::maxtorque)
			.def_readwrite("torquefactor_minrpm", &PyVehicleDesc::Engine::torquefactor_minrpm)
			.def_readwrite("torquefactor_maxrpm", &PyVehicleDesc::Engine::torquefactor_maxrpm)
			.def_readwrite("resistencefactor_minrpm", &PyVehicleDesc::Engine::resistencefactor_minrpm)
			.def_readwrite("resistencefactor_optrpm", &PyVehicleDesc::Engine::resistencefactor_optrpm)
			.def_readwrite("resistencefactor_maxrpm", &PyVehicleDesc::Engine::resistencefactor_maxrpm)
			.def("SetDefault", &PyVehicleDesc::Engine::SetDefault1)
			.def("SetDefault", &PyVehicleDesc::Engine::SetDefault2)
			.def("SetDefault", &PyVehicleDesc::Engine::SetDefault3)
			.def("SetDefault", &PyVehicleDesc::Engine::SetDefault4)
			.def("SetDefault", &PyVehicleDesc::Engine::SetDefault5)
		;
		//objects::register_conversion<PyVehicleDesc::Engine, VehicleDesc::Engine>();
		////objects::register_conversion<VehicleDesc::Engine, PyVehicleDesc::Engine>();
	}

	// >>> Classes

	class_<Vehicle, boost::noncopyable>("Vehicle", no_init)
		.def_readwrite("acceleration", &Vehicle::acceleration)
		.def_readwrite("steering", &Vehicle::steering)
		.def_readwrite("handbrakepulled", &Vehicle::handbrakepulled)
	;

	class_<Pose, boost::noncopyable>("Pose", no_init)
		.add_property("numbones", &Pose::GetNumBones)
		.def("GetRotation", &Pose::GetRotation)
		.def("SetRotation", &Pose::SetRotation)
		.def("GetPosition", &Pose::GetPosition)
		.def("SetPosition", &Pose::SetPosition)
	;
	objects::register_conversion<IPose, Pose>();
	objects::register_conversion<Pose, IPose>();

	class_<Animation, Animation*, boost::noncopyable>("Animation", no_init)
		.add_property("enabled", &Animation::GetEnabled, &Animation::SetEnabled)
		.add_property("time", &Animation::GetTime, &Animation::SetTime)
		.add_property("loops", &Animation::GetLoops, &Animation::SetLoops)
		.add_property("duration", &Animation::GetDuration, &Animation::SetDuration)
		.add_property("weight", &Animation::GetWeight, &Animation::SetWeight)
		.def("AddFrame", &Animation::AddFrame)
		.def("Serialize", &Animation::Serialize)
	;

	class_<Entity, boost::noncopyable>("Entity", no_init)
	;

	class_<PlayerEntity, bases<Entity>, boost::noncopyable>("PlayerEntity", no_init)
	;

	class_<RegularEntity, bases<Entity>, boost::noncopyable>("RegularEntity", no_init)
		.def("applyForce", &RegularEntity::applyForce)
		.def("applyAngularImpulse", &RegularEntity::applyAngularImpulse)
		.def("applyLinearImpulse", &RegularEntity::applyLinearImpulse)
		.def("applyPointImpulse", &RegularEntity::applyPointImpulse)
		.def("setMotionType", &RegularEntity::setMotionType)
	;

	class_<BoxedLevelEntity, bases<Entity>, boost::noncopyable>("BoxedLevelEntity", no_init)
		.def("SetBoxTypes", &BoxedLevelEntity::SetBoxTypesWrapper)
		.def("CreateBoxShapes", &BoxedLevelEntity::CreateBoxShapesWrapper)
		.def("AssignChunk", &BoxedLevelEntity::AssignChunk)
		.def("UnassignChunk", &BoxedLevelEntity::UnassignChunk)
		.def("ChunkFromShapeKey", &BoxedLevelEntity::ChunkFromShapeKey, POLICY_MANAGED_BY_C)
		.def("BoxPosFromShapeKey", &BoxedLevelEntity::BoxPosFromShapeKey)
	;

	//class_<std::shared_ptr<void>>("VoidPtr");

	class_<RagdollEntity, bases<Entity>, boost::noncopyable>("RagdollEntity", no_init)
		.def_readwrite("dx", &RagdollEntity::dx)
		.def_readwrite("dz", &RagdollEntity::dz)
		.def_readwrite("rot", &RagdollEntity::rot)
		.def_readwrite("jump", &RagdollEntity::jump)
		.def_readwrite("onladder", &RagdollEntity::onladder)
		.def("SetRootBone", &RagdollEntity::SetRootBone)
		.def("AttachShape", &RagdollEntity::AttachShape)
		.def("AssembleRagdoll", &RagdollEntity::AssembleRagdoll)
		.def("CreatePose", &RagdollEntity::CreatePoseWrapper, POLICY_MANAGED_BY_C)
		.def("CreateAnimation", &RagdollEntity::CreateAnimationWrapper, POLICY_MANAGED_BY_C)
	;

/*pystring_converter()
	.from_python<std::shared_ptr<void>>()
	.from_python<RagdollEntity>()
;*/

	void (PyHavok::*Update)() = &PyHavok::Update;
	class_<PyHavok, boost::noncopyable>("Havok")
		.def("Init", &PyHavok::Init)
		.def("InitDone", &PyHavok::InitDone)
		.def("CreateRegularEntity", &PyHavok::CreateRegularEntityWrapper, POLICY_MANAGED_BY_C)
		.def("CreateBoxedLevelEntity", &PyHavok::CreateBoxedLevelEntityWrapper, POLICY_MANAGED_BY_C)
		.def("CreateRagdollEntity", &PyHavok::CreateRagdollEntityWrapper, POLICY_MANAGED_BY_C)
		.def("CreateVehicle", &PyHavok::CreateVehicleWrapper, POLICY_MANAGED_BY_C)
		.def("CreateCollisionMesh", &PyHavok::CreateCollisionMeshWrapper, POLICY_MANAGED_BY_C)
		.def("CreateConvexCollisionHull", &PyHavok::CreateConvexCollisionHullWrapper, POLICY_MANAGED_BY_C)
		.def("CreateCollisionRagdoll", &PyHavok::CreateCollisionRagdollWrapper, POLICY_MANAGED_BY_C)
		.def("EnableViewer", &PyHavok::EnableViewer)
		.def("DisableViewer", &PyHavok::DisableViewer)
		.def("Update", Update)
	;
}

HAVOK_EXTERN_FUNC void __cdecl RegisterScriptableHavokClasses(const IPythonScriptEngine* pse)
{
	if(pse)
		pse->AddModule("Havok", &PyInit_Havok);
}
void SyncWithPython()
{
	AddToBuiltins("hvk", (PyHavok*)&*hvk);
}

std::shared_ptr<void> Animation::GetPyPtr()
{
	return pse->GetHandle(api::object(ptr(this)).ptr());
}
HAVOK_EXTERN_FUNC void __cdecl PyGet(LPVOID pyobject, LPANIMATION* ani)
{
	*ani = extract<Animation*>(api::object(handle<>((PyObject*)pyobject))); Py_DECREF(pyobject);
}
std::shared_ptr<void> RagdollEntity::GetPyPtr()
{
/*try
{
	object pyFunPlxMsgWrapper = import("human_animations").attr("CreateAnimations");
	pyFunPlxMsgWrapper(handle<>(boost::python::api::object(ptr(this)).ptr()));
}
catch(boost::python::error_already_set const &)
{
	LOG(String("Error in Python: ") << PyExceptionToString<0>() << '\n');
}*/



/*LPPYTHONSCRIPT script;
pse->LoadScript("human_animations.py", &script);
int numfunctions;
PyObject** scriptfunctions;
String* scriptfuncnames;
script->GetFunctions(&numfunctions, &scriptfunctions, &scriptfuncnames);
for(int i = 0; i < numfunctions; i++)
	if(scriptfuncnames[i].Equals("CreateAnimations"))
		pse->Call(scriptfunctions[i], "(O)", boost::python::api::object(ptr(this)).ptr());*/





//return NULL;
	return pse->GetHandle(api::object(ptr(this)).ptr());//incref(api::object(ptr(this)).ptr());
}
#else
#include "Havok.h"
#include <memory>
std::shared_ptr<void> Animation::GetPyPtr() {return NULL;}
std::shared_ptr<void> RagdollEntity::GetPyPtr() {return NULL;}
void SyncWithPython() {}
#endif