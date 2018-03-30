#ifndef __ISHAVOK_H
#define __ISHAVOK_H

//#ifndef __cplusplus_cli
#include "ISEngine.h"
//#include <d3d11.h>
//#include <dxgi.h>
//#include <d3dx11.h>
//#endif
#include "ISMath.h"
#include "ISHvkD3dInterop.h"


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_HAVOK
	#define HAVOK_EXTERN_FUNC	__declspec(dllexport)
#else
	#define HAVOK_EXTERN_FUNC	__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum VisualizationType
{
	VT_NONE, VT_LOCAL, VT_VISUALDEBUGGER
};
enum HvkViewer
{
	HV_BROADPHASE, HV_CONSTRAINT, HV_ACTIVECONTACTPOINT, HV_INACTIVECONTACTPOINT, HV_CONVEXRADIUS, HV_INCONSISTENDWINDING, HV_MIDPHASE, HV_MOUSEPICKING, HV_PHANTOMDISPLAY, HV_RIGIDBODYCENTEROFMASS,
	HV_RIGIDBODYINERTIA, HV_SHAPEDISPLAY, HV_SIMULATIONISLAND, HV_SINGLEBODYCONSTRAINT, HV_SWEPTTRANSFORMDISPLAY, HV_TOICONTACTPOINT, HV_TOICOUNT, HV_VEHICLE, HV_WELDING
};
enum MotionType // Order matches hkpMotion::MotionType; don't change!
{
	MT_INVALID, MT_DYNAMIC, MT_SPHERE_INERTIA, MT_BOX_INERTIA,
	MT_KEYFRAMED, MT_FIXED, MT_THIN_BOX_INERTIA, MT_CHARACTER
};
enum CollidableQuality // Order matches hkpCollidableQualityType; don't change!
{
	CQ_INVALID = -1, CQ_FIXED, CQ_KEYFRAMED, CQ_DEBRIS, CQ_DEBRIS_SIMPLE_TOI, CQ_MOVING, CQ_CRITICAL,
	CQ_BULLET, CQ_USER, CQ_CHARACTER, CQ_KEYFRAMED_REPORTING
};
enum HvkShapeId
{
	HSID_CONVEX, HSID_SPHERE, HSID_BOX, HSID_CAPSULE, HSID_CYLINDER, HSID_PLANE, HSID_TRIANGLE, HSID_CONVEXVERTICES, HSID_CONVEXTRANSFORM,
	HSID_CONVEXTRANSLATE, HSID_MESH, HSID_EXTENDEDMESH, HSID_COMPRESSEDMESH, HSID_TRANSFORM, HSID_LIST, HSID_STATICCOMPOUND
};
enum ConstraintId
{
	CID_BALLANDSOCKET, CID_STIFFSPRING, CID_HINGE, CID_LIMITEDHINGE, CID_WHEEL, CID_PULLEY, CID_RAGDOLL
};

//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct HvkGeneralShapeDesc
{
	MotionType mtype;
	CollidableQuality qual;
	float mass;
	HvkGeneralShapeDesc() : mtype(MT_INVALID), qual(CQ_INVALID) {}
};

struct HvkShapeDesc : public HvkGeneralShapeDesc
{
protected:
	const HvkShapeId id;
	HvkShapeDesc(HvkShapeId id) : id(id) {}
public:
	HvkShapeId GetId() const {return id;}
};
struct HvkConvexShapeDesc : public HvkShapeDesc // Specialization of HvkShapeDesc
{
	HvkConvexShapeDesc(HvkShapeId id) : HvkShapeDesc(id) {}
};

struct HvkCapsuleShapeDesc : public HvkConvexShapeDesc
{
	HvkCapsuleShapeDesc() : HvkConvexShapeDesc(HSID_CAPSULE) {}
	Vector3 v0, v1;
	float radius;
};
struct HvkCylinderShapeDesc : public HvkConvexShapeDesc
{
	HvkCylinderShapeDesc() : HvkConvexShapeDesc(HSID_CYLINDER) {}
	Vector3 v0, v1;
	float radius;
};
struct HvkSphereShapeDesc : public HvkConvexShapeDesc
{
	HvkSphereShapeDesc() : HvkConvexShapeDesc(HSID_SPHERE) {}
	float radius;
};
struct HvkBoxShapeDesc : public HvkConvexShapeDesc
{
	HvkBoxShapeDesc() : HvkConvexShapeDesc(HSID_BOX) {}
	Vector3 size;
};
struct HvkPlaneShapeDesc : public HvkShapeDesc
{
	HvkPlaneShapeDesc() : HvkShapeDesc(HSID_PLANE) {}
	Vector3 vcenter, vdir, vhalfext;
};
struct HvkConvexVerticesShapeDesc : public HvkConvexShapeDesc
{
	HvkConvexVerticesShapeDesc() : HvkConvexShapeDesc(HSID_CONVEXVERTICES) {}
	BYTE* vertices;
	int numvertices, fvfstride;
	PtrWrapper GetVertexPtr() {return PtrWrapper(vertices);}
	void SetVerticesFromVertexPtr(PtrWrapper bytes) {vertices = (BYTE*)bytes.ptr;}
};
struct HvkCompressedMeshShapeDesc : public HvkShapeDesc
{
	HvkCompressedMeshShapeDesc() : HvkShapeDesc(HSID_COMPRESSEDMESH), numvertices(0), numfaces(0), numshapes(0) {}
	BYTE* vertexdata;
	UINT* indices;
	UINT numvertices, numfaces, fvfstride;
	struct TransformedShape
	{
		const HvkConvexShapeDesc* shape;
		Vector3 pos, scale;
		Quaternion qrot;
		TransformedShape() : shape(NULL), pos(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), qrot(0.0f, 0.0f, 0.0f, 1.0f) {}
	}* convexshapes;
	UINT numshapes;
	PtrWrapper GetVertexPtr() {return PtrWrapper(vertexdata);}
	void SetVerticesFromVertexPtr(PtrWrapper bytes) {vertexdata = (BYTE*)bytes.ptr;}
	PtrWrapper GetIndexPtr() {return PtrWrapper(indices);}
	void SetIndicesFromIndexPtr(PtrWrapper bytes) {indices = (UINT*)bytes.ptr;}
};
struct HvkStaticCompoundShapeDesc : public HvkShapeDesc
{
	HvkStaticCompoundShapeDesc() : HvkShapeDesc(HSID_STATICCOMPOUND), numshapes(0) {}
	struct TransformedShape
	{
		const HvkShapeDesc* shape;
		Vector3 pos, scale;
		Quaternion qrot;
		TransformedShape() : shape(NULL), pos(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), qrot(0.0f, 0.0f, 0.0f, 1.0f) {}
	}* shapes;
	UINT numshapes;
};

struct ConstraintDesc
{
protected:
	const ConstraintId id;
	ConstraintDesc(ConstraintId id) : id(id) {}
public:
	ConstraintId GetId() const {return id;}
};

struct BallAndSocketConstraintDesc : public ConstraintDesc
{
	BallAndSocketConstraintDesc() : ConstraintDesc(CID_BALLANDSOCKET) {}
	Vector3 pivot;
};
struct HingeConstraintDesc : public ConstraintDesc
{
	HingeConstraintDesc() : ConstraintDesc(CID_HINGE) {}
	Vector3 pivot, axis;
};
struct LimitedHingeConstraintDesc : public ConstraintDesc
{
	LimitedHingeConstraintDesc() : ConstraintDesc(CID_LIMITEDHINGE) {}
	float angleMin, angleMax;
	Vector3 pivot, axis;
};
struct RagdollConstraintDesc : public ConstraintDesc
{
	RagdollConstraintDesc() : ConstraintDesc(CID_RAGDOLL) {}
	float coneMin, coneMax;
	float planeMin, planeMax;
	float twistMin, twistMax;
	Vector3 pivot, twistAxis, planeAxis;
};

struct VehicleDesc
{
	const HvkShapeDesc* chassis;
	struct Wheel
	{
		char axle;
		float friction, slipangle, radius, width, mass;
		float viscosityfriction, maxfriction, forcefeedbackmultiplier;

		void SetDefault(char axle = 0, float radius = 0.4f, float width = 0.2f, float mass = 10.0f, float friction = 1.5f)
		{
			this->axle = axle;
			this->friction = friction; this->slipangle = 0.0f; this->radius = radius; this->width = width; this->mass = mass;
			this->viscosityfriction = 0.25f; this->maxfriction = friction * 2.0f; this->forcefeedbackmultiplier = 0.1f;
		}
	};
	std::vector<Wheel> wheels;
	struct Engine
	{
		float minrpm, optrpm, maxrpm;
		float maxtorque, torquefactor_minrpm, torquefactor_maxrpm;
		float resistencefactor_minrpm, resistencefactor_optrpm, resistencefactor_maxrpm;

		void SetDefault(float maxrpm = 7500.0f, float maxtorque = 500.0f, float optrpm = 5500.0f, float minrpm = 1000.0f)
		{
			this->minrpm = minrpm; this->optrpm = optrpm; this->maxrpm = maxrpm;
			this->maxtorque = maxtorque; this->torquefactor_minrpm = 0.8f; this->torquefactor_maxrpm = 0.8f;
			this->resistencefactor_minrpm = 0.05f; this->resistencefactor_optrpm = 0.1f; this->resistencefactor_maxrpm = 0.3f;
		}
	} engine;
};

//-----------------------------------------------------------------------------
// Name: IWeapon
// Desc: Interface to the Weapon class
//-----------------------------------------------------------------------------
typedef class IWeapon
{
public:
	virtual void Fire(Vector3* vpos, Quaternion* qrot) = 0;
}* LPWEAPON;

//-----------------------------------------------------------------------------
// Name: IVehicle
// Desc: Interface to the Vehicle class
//-----------------------------------------------------------------------------
typedef class IVehicle
{
public:
	float acceleration, steering;
	bool handbrakepulled;
}* LPVEHICLE;

//-----------------------------------------------------------------------------
// Name: IPose
// Desc: Interface to the Pose class
//-----------------------------------------------------------------------------
typedef class IPose
{
public:
	virtual int GetNumBones() = 0;
	virtual void GetRotation(Quaternion* qrot, int boneidx) = 0;
	virtual void SetRotation(const Quaternion* qrot, int boneidx) = 0;
	virtual void GetPosition(Vector3* vpos, int boneidx) = 0;
	virtual void SetPosition(const Vector3* vpos, int boneidx) = 0;
}* LPPOSE;

//-----------------------------------------------------------------------------
// Name: IAnimation
// Desc: Interface to the Animation class
//-----------------------------------------------------------------------------
typedef class IAnimation
{
public:
	virtual bool GetEnabled() {int abc = 0; return false;}//= 0;
	virtual bool SetEnabled(bool val) {int abc = 0; return false;}//= 0;
	__declspec(property(get=GetEnabled, put=SetEnabled)) bool Enabled;
	virtual float GetTime() = 0;
	virtual float SetTime(float val) = 0;
	__declspec(property(get=GetTime, put=SetTime)) float Time;
	virtual UINT GetLoops() = 0;
	virtual UINT SetLoops(UINT val) = 0;
	__declspec(property(get=GetLoops, put=SetLoops)) UINT Loops;
	virtual float GetDuration() = 0;
	virtual float SetDuration(float val) = 0;
	__declspec(property(get=GetDuration, put=SetDuration)) float Duration;
	virtual float GetWeight() = 0;
	virtual float SetWeight(float val) = 0;
	__declspec(property(get=GetWeight, put=SetWeight)) float Weight;

	virtual void AddFrame(IPose* frame) = 0;
	virtual Result Serialize(const FilePath& filename) = 0;
	virtual std::shared_ptr<void> GetPyPtr() = 0;
}* LPANIMATION;

//-----------------------------------------------------------------------------
// Name: IEntity
// Desc: Base class for all entity interfaces
//-----------------------------------------------------------------------------
typedef class IEntity
{
public:
	virtual void Release() = 0;
}* LPENTITY;

//-----------------------------------------------------------------------------
// Name: IPlayerEntity
// Desc: Interface to the PlayerEntity class
//-----------------------------------------------------------------------------
typedef class IPlayerEntity : public IEntity
{
public:

	virtual void Update(float dx, float dy, float rot, bool jump) = 0;
}* LPPLAYERENTITY;

//-----------------------------------------------------------------------------
// Name: IRegularEntity
// Desc: Interface to the RegularEntity class
//-----------------------------------------------------------------------------
typedef class IRegularEntity : public IEntity
{
public:

	virtual void applyForce(const Vector3* force) = 0;
	virtual void applyAngularImpulse(const Vector3* impulse) = 0;
	virtual void applyLinearImpulse(const Vector3* impulse) = 0;
	virtual void applyPointImpulse(const Vector3* impulse, const Vector3* pos) = 0;
	virtual void setMotionType(MotionType mtype) = 0;
}* LPREGULARENTITY;

//-----------------------------------------------------------------------------
// Name: ILevelEntity
// Desc: Interface to the LevelEntity class
//-----------------------------------------------------------------------------
typedef class ILevelEntity : public IEntity
{
public:

	virtual void EnableAll() = 0;
	virtual void DisableAll() = 0;
	virtual void EnableShape(UINT32 shapekey) = 0;
	virtual void DisableShape(UINT32 shapekey) = 0;
}* LPLEVELENTITY;

//-----------------------------------------------------------------------------
// Name: IBoxedLevelEntity
// Desc: Interface to the BoxedLevelEntity class
//-----------------------------------------------------------------------------
typedef class IBoxedLevelEntity : public IEntity
{
public:
	virtual void SetBoxTypes(BoxedLevelBoxType* boxtypes, UINT32 numtypes) = 0;
	virtual Result CreateBoxShapes(HvkShapeDesc** boxshapes, UINT32 numboxshapes) = 0;
	virtual void AssignChunk(BoxedLevelChunk* chunk) = 0;
	virtual void UnassignChunk(BoxedLevelChunk* chunk) = 0;
	virtual BoxedLevelChunk* ChunkFromShapeKey(UINT32 chunkshapekey) = 0;
	virtual void BoxPosFromShapeKey(UINT32 boxshapekey, UINT32 (*boxpos)[3]) = 0;
}* LPBOXEDLEVELENTITY;

//-----------------------------------------------------------------------------
// Name: IRagdollEntity
// Desc: Interface to the RagdollEntity class
//-----------------------------------------------------------------------------
typedef class IRagdollEntity : public IEntity
{
public:
	float dx, dz, rot;
	bool jump, onladder;

	virtual Result SetRootBone(const HvkShapeDesc* shapedesc, const String& shapekey, const Matrix* worldmatrix) = 0;
	virtual Result AttachShape(const HvkShapeDesc* shapedesc, const ConstraintDesc* constraintdesc, const String& newshapekey, const String& anchorshapekey, const Matrix* worldmatrix) = 0;
	virtual void AttachEntity(IRegularEntity* entity, const String& targetshapekey, const float* localtransform) = 0;
	virtual void DetachEntity(const IRegularEntity* entity) = 0;
	virtual Result AssembleRagdoll() = 0;
	virtual Result CreatePose(IPose** pose) = 0;
	virtual Result CreateAnimation(float duration, IAnimation** ani) = 0;
	virtual float* GetPivot(const String& shapekey) = 0;
	virtual float* GetPivot(int boneidx) = 0;
	virtual void LookAt(const float* vtargetpos, float gain) = 0;
	virtual void EnableFootIk() = 0;
	virtual void Update() = 0;
	virtual std::shared_ptr<void> GetPyPtr() = 0;
}* LPRAGDOLLENTITY;

//-----------------------------------------------------------------------------
// Name: IHavok
// Desc: Interface to the Havok class
//-----------------------------------------------------------------------------
typedef class IHavok
{
public:
	struct Time {float t, dt;} time;

	enum Layer
	{
		LAYER_DEFAULT,
		LAYER_LANDSCAPE, // Contains static geometry for the level
		LAYER_PROXY, // Contains the proxy for the character controller
		LAYER_DEBRIS, // Contains small dynamic objects in the scene. Each of these have different masses
		LAYER_MOVABLE_ENVIRONMENT, // Contains large dynamic objects in the scene
		LAYER_RAGDOLL_KEYFRAMED, // Contains the bodies of the ragdoll that are currently keyframed
		LAYER_RAYCAST, // This layer is used to control which items to raycast against
		LAYER_RAGDOLL_DYNAMIC, // Contains the rigid bodies for the ragdoll which are not keyframed. When the ragdoll falls all ragdoll bodies are switched into this layer
		LAYER_PICKING, // A layer that control which items are effected by the mouse pointer
		LAYER_HANDHELD // An attachable item
	};

	virtual void Sync(GLOBALVARDEF_LIST) = 0;

	virtual Result Init(const Vector3& worldsize_min, const Vector3& worldsize_max, VisualizationType vtype, LPOUTPUTWINDOW localvizwnd = NULL, LPRENDERSHADER localvizshader = NULL) = 0;
	virtual void InitDone() = 0;

	virtual Result CreateRegularEntity(const HvkShapeDesc* shapedesc, Layer layer, Vector3* vpos, Quaternion* qrot, IRegularEntity** entity) = 0;
	virtual Result CreateLevelEntity(const HvkStaticCompoundShapeDesc* shapedesc, const Vector3& vpos, const Quaternion& qrot, ILevelEntity** entity) = 0;
	virtual Result CreateBoxedLevelEntity(const UINT (&chunksize)[3], IBoxedLevelEntity** entity) = 0;
	virtual Result CreateRagdollEntity(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot, IRagdollEntity** entity) = 0;
	virtual Result CreatePlayerEntity(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot, bool isrigidbody, IPlayerEntity** entity) = 0;
	virtual Result CreateWeapon(IWeapon** weapon) = 0;
	virtual Result CreateVehicle(const VehicleDesc* vehicledesc, Vector3* vpos, Quaternion* qrot, IVehicle** vehicle) = 0;

	virtual Result CreateCollisionMesh(LPOBJECT obj, IRegularEntity** entity) = 0;
	virtual Result CreateConvexCollisionHull(const HvkGeneralShapeDesc* shapedesc, LPOBJECT obj, IRegularEntity** entity) = 0;
	virtual Result CreateCollisionCapsule(LPOBJECT obj, float radius, IRegularEntity** entity) = 0;
	virtual Result CreateCollisionRagdoll(LPOBJECT obj, float radius, IRagdollEntity** entity) = 0;

	virtual float CastRayTo(const Vector3& src, const Vector3& dest, LPENTITY* hitentity = NULL, UINT32 (*shapekeys)[8] = NULL, Vector3* hitnml = NULL) = 0;
	virtual float CastRayDir(const Vector3& src, const Vector3& dir, LPENTITY* hitentity = NULL, UINT32 (*shapekeys)[8] = NULL, Vector3* hitnml = NULL) = 0;

	virtual void EnableViewer(HvkViewer viewer) = 0;
	virtual void DisableViewer(HvkViewer viewer) = 0;
	virtual String GetViewerName(HvkViewer viewer) = 0;

	virtual void MousePickGrab(float* campos, float* camdir) = 0;
	virtual void MousePickDrag(float* campos, float* camdir) = 0;
	virtual void MousePickRelease() = 0;

	virtual void Update() = 0;
	virtual void Release() = 0;
}* LPHAVOK;

HAVOK_EXTERN_FUNC LPHAVOK __cdecl CreateHavok();
HAVOK_EXTERN_FUNC void __cdecl RegisterScriptableHavokClasses(const IPythonScriptEngine* pse);
HAVOK_EXTERN_FUNC void __cdecl PyGet(LPVOID pyobject, LPANIMATION* ani);

#endif