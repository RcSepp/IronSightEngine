#ifndef __HAVOK_H
#define __HAVOK_H

#include "ISHavok.h"
#include "..\\global\\HKResult.h"
#include <map>

/*#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>

#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Dynamics/RegularEntity/hkpRigidBody.h>
#include <Physics/Dynamics/RegularEntity/hkpRigidBodyCinfo.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>*/





#pragma comment (lib, "hkInternal.lib")
#pragma comment (lib, "hkGeometryUtilities.lib")
#pragma comment (lib, "hkVisualize.lib")
#pragma comment (lib, "hkSerialize.lib")
#pragma comment (lib, "hkCompat.lib")
#pragma comment (lib, "hkSceneData.lib")
#pragma comment (lib, "hkBase.lib")
#pragma comment (lib, "hkcdCollide.lib")
#pragma comment (lib, "hkcdInternal.lib")

#pragma comment (lib, "hkpUtilities.lib")
#pragma comment (lib, "hkpDynamics.lib")
#pragma comment (lib, "hkpCollide.lib")
#pragma comment (lib, "hkpConstraintSolver.lib")

#pragma comment (lib, "hkpInternal.lib")
#pragma comment (lib, "hkpVehicle.lib")

#pragma comment (lib, "hkaInternal.lib")
#pragma comment (lib, "hkaAnimation.lib")
#pragma comment (lib, "hkaRagdoll.lib")

/*#pragma comment (lib, "hkgBridge.lib")
#pragma comment (lib, "hkgOgls.lib")
#pragma comment (lib, "hkgOglES.lib")
#pragma comment (lib, "hkgOglES2.lib")
#pragma comment (lib, "hkgDx11.lib")
#pragma comment (lib, "hkgDx9s.lib")
#pragma comment (lib, "hkgSoundCommon.lib")
#pragma comment (lib, "hkgSoundXAudio2.lib")
#pragma comment (lib, "hkgCommon.lib")*/

/*// Register Havok classes.
#include <Common/Base/KeyCode.cxx>
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkKeyCodeClasses.h>
//#include <Common/Serialize/Util/hkBuiltinTypeRegistry.cxx>

// Select product features
//#include <Common/Base/keycode.cxx>
#include <Common/Base/Config/hkProductFeatures.cxx>

// No backward compatibility needed
#define HK_SERIALIZE_MIN_COMPATIBLE_VERSION Current*/

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Memory/Allocator/LargeBlock/hkLargeBlockAllocator.h>
#include <Common/Base/Memory/System/FreeList/hkFreeListMemorySystem.h>
#include <Common/Base/Fwd/hkcstdio.h>

#include <Common/Base/Ext/hkBaseExt.h>

#include <Physics/Dynamics/Phantom/hkpSimpleShapePhantom.h>

#include <Physics/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>
#include <Physics/Utilities/CharacterControl/StateMachine/hkpDefaultCharacterStates.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h>
#include <Physics/Utilities/Actions/MouseSpring/hkpMouseSpringAction.h>
#include <Physics/Utilities/Weapons/hkpBallGun.h>

#include <Physics/Vehicle/hkpVehicleInstance.h>

#include <Physics/Collide/Filter/Group/hkpGroupFilter.h>

#include <Physics/Dynamics/Constraint/Util/hkpConstraintStabilizationUtil.h>

// Physics
#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/HeightField/Plane/hkpPlaneShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

#include <Physics/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShape.h>
#include <Physics/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShapeCinfo.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>

// Animation
#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
#include <Animation/Animation/Rig/hkaPose.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Animation/Animation/Animation/Interleaved/hkaInterleavedUncompressedAnimation.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Rig/hkaBoneAttachment.h>
#include <Animation/Animation/Ik/FootPlacement/hkaFootPlacementIkSolver.h>

// Visualization
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>

// Serialization
#include <Common/Serialize/Util/hkSerializeUtil.h>


#define HAVOK_RELEASE(obj)	{if(obj) {(obj)->removeReference(); (obj) = HK_NULL;}}

struct StringComparator
{
	bool operator()(const String& str1, const String& str2) const {return std::strcmp(str1, str2) < 0;}
};

class Entity
{
public:
	virtual void PreUpdate() {}
	virtual void Update() {};
	virtual void Release() = 0;
};

class Havok;
class RagdollEntity;
class hkpStaticCompoundShape;

//-----------------------------------------------------------------------------
// Name: Weapon
// Desc: Uses Havok utility functions to act as a projectile gun
//-----------------------------------------------------------------------------
class Weapon : public IWeapon
{
private:
	Havok* parent;
	hkpBallGun* ballgun;
	Vector3* vpos;
	Quaternion* qrot;

public:

	Weapon(Havok* parent) : parent(parent), ballgun(NULL) {}
	Result Init();
	void Fire(Vector3* vpos, Quaternion* qrot);
	void Update();
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Vehicle
// Desc: A vehicle with Havok car pyhsics
//-----------------------------------------------------------------------------
class Vehicle : public IVehicle
{
private:
	Havok* parent;
	hkpVehicleInstance* vehicle;
	Vector3* vpos;
	Quaternion* qrot;

public:

	Vehicle(Havok* parent);
	Result Init(const VehicleDesc* vehicledesc, Vector3* vpos, Quaternion* qrot);
	void PreUpdate();
	void Update();
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Pose
// Desc: A collection of bone transformations represented in model and local spaces, based on the hierarchy of a given skeleton
//-----------------------------------------------------------------------------
class Pose : public IPose
{
private:
	Vector3** pivots;
	hkArray<hkQsTransform> localtransforms;

public:
	hkaPose* pose; //EDIT: Make private

	int GetNumBones() {return pose->getSkeleton()->m_bones.getSize();}
	const hkQsTransform& getBoneLocalSpace(int boneidx) {return pose->getBoneLocalSpace(boneidx);}
	const hkQsTransform& getBoneModelSpace(int boneidx) {return pose->getBoneModelSpace(boneidx);}
	void setBoneLocalSpace(int boneidx, const hkQsTransform& transform) {pose->setBoneLocalSpace(boneidx, transform);}
	void setBoneModelSpace(int boneidx, const hkQsTransform& transform, bool propagate) {pose->setBoneModelSpace(boneidx, transform, (hkaPose::PropagateOrNot)propagate);}

	void GetRotation(Quaternion* qrot, int boneidx);
	void SetRotation(const Quaternion* qrot, int boneidx);
	void GetPosition(Vector3* vpos, int boneidx);
	void SetPosition(const Vector3* vpos, int boneidx);

	Pose(const hkaSkeleton* skeleton, const hkArrayBase<hkpConstraintInstance*>& constraints);
};

//-----------------------------------------------------------------------------
// Name: Animation
// Desc: A collection of poses with time reference and annotations
//-----------------------------------------------------------------------------
class Animation : public IAnimation
{
private:
	RagdollEntity* parent;
	hkaInterleavedUncompressedAnimation* ani; //EDIT: Make private
	hkaAnimationBinding* anibinding;
	hkaDefaultAnimationControl* anictrl;
	bool enabled;

public:
	Animation(RagdollEntity* parent, int numtracks, float duration);

	bool GetEnabled() {return enabled;}
	bool SetEnabled(bool val);
	float GetTime() {return anictrl->getLocalTime();}
	float SetTime(float val) {anictrl->setLocalTime(val); return val;}
	UINT GetLoops() {return anictrl->getOverflowCount();}
	UINT SetLoops(UINT val) {anictrl->setOverflowCount(val); return val;}
	float GetDuration() {return ani->m_duration;}
	float SetDuration(float val) {return ani->m_duration = val;}
	float GetWeight() {return anictrl->getMasterWeight();}
	float SetWeight(float val) {val = hkMath::clamp(val, 0.0f, 1.0f); anictrl->setMasterWeight(val); return val;}

	void AddFrame(IPose* frame);
	Result Serialize(const FilePath& filename);
	std::shared_ptr<void> GetPyPtr();
};

//-----------------------------------------------------------------------------
// Name: PlayerEntity
// Desc: An object in collision detection context with player characteristics
//-----------------------------------------------------------------------------
class MyCharacterListener;
class PlayerEntity : public IPlayerEntity
{
private:
	Havok* parent;
hkpCharacterContext* m_characterContext;
hkpCharacterProxy* m_characterProxy;
	Vector3* vpos;
	Quaternion* qrot;

public:

	PlayerEntity(Havok* parent) : parent(parent), vpos(NULL), qrot(NULL) {m_characterContext = NULL; m_characterProxy = NULL;}
	Result Create(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot);
	void Update(float dx, float dy, float rot, bool jump);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: PlayerRBEntity
// Desc: PlayerEntity, utilizing a rigid body, instead of a shape phantom
//-----------------------------------------------------------------------------
class MyCharacterListener;
class PlayerRBEntity : public IPlayerEntity
{
private:
	Havok* parent;
	Vector3* vpos;
	Quaternion* qrot;
	hkpCharacterRigidBody* characterrigidbody;
	hkpCharacterContext* characterctx;

public:

	PlayerRBEntity(Havok* parent) : parent(parent), vpos(NULL), qrot(NULL) {}
	Result Create(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot);
	void Update(float dx, float dy, float rot, bool jump);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: RegularEntity
// Desc: An object in collision detection context
//-----------------------------------------------------------------------------
class RegularEntity : public IRegularEntity, public Entity
{
private:
	Havok* parent;
	Vector3* vpos;
	Quaternion* qrot;

public:
	hkpRigidBody* rigidbody;

	RegularEntity(Havok* parent) : parent(parent), vpos(NULL), qrot(NULL) {}
	Result Create(const HvkShapeDesc* shapedesc, IHavok::Layer layer, Vector3* vpos, Quaternion* qrot);
	void Update();
	void applyForce(const Vector3* force);
	void applyAngularImpulse(const Vector3* impulse);
	void applyLinearImpulse(const Vector3* impulse);
	void applyPointImpulse(const Vector3* impulse, const Vector3* pos);
	void setMotionType(MotionType mtype);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: LevelEntity
// Desc: A collection of static shapes
//-----------------------------------------------------------------------------
class LevelEntity : public ILevelEntity, public Entity
{
private:
	Havok* parent;
	hkpStaticCompoundShape* compoundshape;

public:
	hkpRigidBody* rigidbody;

	LevelEntity(Havok* parent) : parent(parent) {}
	Result Create(const HvkStaticCompoundShapeDesc* shapedesc, const Vector3& vpos, const Quaternion& qrot);
	void EnableAll();
	void DisableAll();
	void EnableShape(UINT32 shapekey);
	void DisableShape(UINT32 shapekey);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: BoxedLevelEntity
// Desc: An entity implementing a custom boxed-level-shape
//-----------------------------------------------------------------------------
class BoxedLevelShape;
class hkpContactMgr;
class BoxedLevelEntity : public IBoxedLevelEntity, public Entity
{
private:
	Havok* parent;
	BoxedLevelShape* shape;

public:
	hkpRigidBody* rigidbody;

	static struct CollisionAgentFunctions
	{
		static hkpCollisionAgent* HK_CALL CreateFunc(const hkpCdBody& collA, const hkpCdBody& collB, const hkpCollisionInput& env, hkpContactMgr* mgr);
		static void HK_CALL GetPenetrationsFunc(const hkpCdBody& bodyA, const hkpCdBody& bodyB, const hkpCollisionInput& input, hkpCdBodyPairCollector& collector);
		static void HK_CALL GetClosestPointsFunc(const hkpCdBody& bodyA, const hkpCdBody& bodyB, const hkpCollisionInput& input, hkpCdPointCollector& output);
		static void HK_CALL LinearCastFunc(const hkpCdBody& bodyA, const hkpCdBody& bodyB, const hkpLinearCastCollisionInput& input, hkpCdPointCollector& castCollector, hkpCdPointCollector* startCollector);
	};

	BoxedLevelEntity(Havok* parent) : parent(parent) {}
	Result Create(const UINT (&chunksize)[3]);
	void SetBoxTypes(BoxedLevelBoxType* boxtypes, UINT32 numtypes);
	void SetBoxTypesWrapper(std::vector<BoxedLevelBoxType>& list)
	{
		if(list.size())
			SetBoxTypes(&list[0], list.size());
	}
	Result CreateBoxShapes(HvkShapeDesc** boxshapes, UINT32 numboxshapes);
	void CreateBoxShapesWrapper(std::vector<HvkShapeDesc*>& list)
	{
		if(list.size())
			CreateBoxShapes(&list[0], list.size());
	}
	void AssignChunk(BoxedLevelChunk* chunk);
	void UnassignChunk(BoxedLevelChunk* chunk);
	BoxedLevelChunk* ChunkFromShapeKey(UINT32 chunkshapekey);
	void BoxPosFromShapeKey(UINT32 boxshapekey, UINT32 (*boxpos)[3]);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: RagdollEntity
// Desc: An animatable ragdoll object, composed of multiple connected shapes
//-----------------------------------------------------------------------------
class MyCharacterListener2;
class RagdollEntity : public IRagdollEntity, public Entity
{
private:
	// Global
//	hkQsTransform* transform;

	// Character controller
hkpCharacterContext* m_characterContext;
hkpCharacterProxy* m_characterProxy;
MyCharacterListener2* m_listener;

	// Animation
	hkpPhysicsSystem* ragdoll;
	int collisiongroup;
	hkaSkeleton* ragdollskeleton;
public:
	hkaAnimatedSkeleton* animatedskeleton;
private:
	hkaRagdollInstance* ragdollinstance;
	hkaRagdollRigidBodyController* controller;
	std::list<Animation*> animations;

	// Attachments
	hkArray<hkRefPtr<hkaBoneAttachment>> attachments;

	hkaPose* currentpose;

	// Look at IK solver
	struct LookAtDesc
	{
		hkVector4 targetpos;
		hkReal gain;
		int neckboneidx, headboneidx;
	}* lookatdesc;

	// Foot IK solver
	struct FootIkDesc
	{
		hkaFootPlacementIkSolver *leftlegsolver, *rightlegsolver;
		FootIkDesc() : leftlegsolver(NULL), rightlegsolver(NULL) {}
		~FootIkDesc()
		{
			if(leftlegsolver)
				delete leftlegsolver;
			if(rightlegsolver)
				delete rightlegsolver;
		}
	}* footikdesc;

	struct Part
	{
		hkpRigidBody* rigidbody;
		const Matrix* vtransform;
		hkInt16 index;

		Part(hkpRigidBody* rigidbody, const Matrix* vtransform, hkInt16 index) : rigidbody(rigidbody), vtransform(vtransform), index(index) {}
	};
	std::map<String, Part*, StringComparator> parts;
	Havok* parent;
	Vector3* vpos;
	Quaternion* qrot;
hkQsTransform* transform;

public:

	RagdollEntity(Havok* parent) : parent(parent), vpos(NULL), qrot(NULL), lookatdesc(NULL), footikdesc(NULL)//, transform(hkQsTransform::IDENTITY)
	{
this->transform = new hkQsTransform(hkQsTransform::IDENTITY);
		dx = dz = rot = 0.0f;
		jump = onladder = false;
	}

	Result Create(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot);
	Result SetRootBone(const HvkShapeDesc* shapedesc, const String& shapekey, const Matrix* worldmatrix);
	Result AttachShape(const HvkShapeDesc* shapedesc, const ConstraintDesc* constraintdesc, const String& newshapekey, const String& anchorshapekey, const Matrix* worldmatrix);
	void AttachEntity(IRegularEntity* entity, const String& targetshapekey, const float* localtransform);
	void DetachEntity(const IRegularEntity* entity);
	Result AssembleRagdoll();
	Result CreatePose(IPose** pose);
	Pose* CreatePoseWrapper()
	{
		Pose* pose;
		CreatePose((LPPOSE*)&pose);
		return pose;
	}
	Result CreateAnimation(float duration, IAnimation** ani);
	Animation* CreateAnimationWrapper(float duration)
	{
		Animation* ani;
		CreateAnimation(duration, (LPANIMATION*)&ani);
		return ani;
	}
	float* GetPivot(const String& shapekey);
	float* GetPivot(int boneidx);
	void LookAt(const float* vtargetpos, float gain);
	void EnableFootIk();
	void PreUpdate();
	void Update();
	void Release();
	std::shared_ptr<void> GetPyPtr();
};

//-----------------------------------------------------------------------------
// Name: LocalVisualizer
// Desc: Proxy class for forwarding debug draw calls to Direct 3D
//-----------------------------------------------------------------------------
class LocalVisualizer : public hkDebugDisplayHandler
{
private:
	LPOUTPUTWINDOW d3dwnd;
	LPRENDERSHADER shader;
	std::map<hkUlong, LPVOID> objids;

	hkResult addGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint);
	//hkResult addGeometry(hkDisplayGeometry* geometry, hkUlong id, int tag, hkUlong shapeIdHint);
	hkResult addGeometryInstance(hkUlong origianalGeomId, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint);
	hkResult setGeometryColor(int color, hkUlong id, int tag);
	hkResult setGeometryTransparency(float alpha, hkUlong id, int tag);
	hkResult updateGeometry(const hkTransform& transform, hkUlong id, int tag);
	hkResult updateGeometry( const hkMatrix4& transform, hkUlong id, int tag );
	hkResult skinGeometry(hkUlong* ids, int numIds, const hkMatrix4* poseModel, int numPoseModel, const hkMatrix4& worldFromModel, int tag );
	hkResult removeGeometry(hkUlong id, int tag, hkUlong shapeIdHint);
	hkResult updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name);
	hkResult displayPoint(const hkVector4& position, int colour, int id, int tag);
	hkResult displayLine(const hkVector4& start, const hkVector4& end, int color, int id, int tag);
	hkResult displayTriangle(const hkVector4& a, const hkVector4& b, const hkVector4& c, int colour, int id, int tag);
	hkResult displayText(const char* text, int color, int id, int tag);
	hkResult display3dText(const char* text, const hkVector4& pos, int color, int id, int tag);
	hkResult displayBone(const hkVector4& a, const hkVector4& b, const hkQuaternion& orientation, int color, int tag);
	hkResult displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, int color, int id, int tag);
	hkResult displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, int color, int id, int tag);			
	hkResult sendMemStatsDump(const char* data, int length);
	hkResult holdImmediate();

public:
	LocalVisualizer(LPOUTPUTWINDOW d3dwnd, LPRENDERSHADER shader) : d3dwnd(d3dwnd), shader(shader) {}
};

//-----------------------------------------------------------------------------
// Name: Havok
// Desc: API to the midi functionality of windows multimedia library. Mainly used to open devices.
//-----------------------------------------------------------------------------
class Havok : public IHavok
{
private:
	bool memutilinitialized;
	hkArray<hkProcessContext*> contexts;
	hkVisualDebugger* visualdebugger;
//	hkDebugDisplayHandler* localvisualizer;
	std::map<HvkViewer, hkProcess*> viewerprocesses;
	hkpMouseSpringAction* mousespring;
	hkReal mousepickdist;

	void _Update();
	float CastRay(hkVector4& src, hkVector4& dest, LPENTITY* hitentity, UINT32 (*shapekeys)[8], Vector3* hitnml);

public:
hkDebugDisplayHandler* localvisualizer;
	hkpWorld* world;
	hkpGroupFilter* groupfilter;
	std::list<Entity*> entities;
	std::list<Weapon*> weapons;
	std::list<Vehicle*> vehicles;
	float t, dt;

	Havok();
	void Sync(GLOBALVARDEF_LIST);
	Result Init(const Vector3& worldsize_min, const Vector3& worldsize_max, VisualizationType vtype, LPOUTPUTWINDOW localvizwnd, LPRENDERSHADER localvizshader);
	void InitDone();
	Result CreateRegularEntity(const HvkShapeDesc* shapedesc, Layer layer, Vector3* vpos, Quaternion* qrot, IRegularEntity** entity);
	RegularEntity* CreateRegularEntityWrapper(const HvkShapeDesc* shapedesc, Layer layer, Vector3* vpos, Quaternion* qrot)
	{
		Result rlt;
		RegularEntity* entity;
		IFFAILED(CreateRegularEntity(shapedesc, layer, vpos, qrot, (LPREGULARENTITY*)&entity))
			throw rlt.GetLastResult();
		return entity;
	}
	Result CreateLevelEntity(const HvkStaticCompoundShapeDesc* shapedesc, const Vector3& vpos, const Quaternion& qrot, ILevelEntity** entity);
	Result CreateBoxedLevelEntity(const UINT (&chunksize)[3], IBoxedLevelEntity** entity);
	Result CreateRagdollEntity(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot, IRagdollEntity** entity);
	RagdollEntity* CreateRagdollEntityWrapper(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot)
	{
		Result rlt;
		RagdollEntity* entity;
		IFFAILED(CreateRagdollEntity(shapedesc, vpos, qrot, (IRagdollEntity**)&entity))
			throw rlt.GetLastResult();
		return entity;
	}
	Result CreatePlayerEntity(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot, bool isrigidbody, IPlayerEntity** entity);
	Result CreateWeapon(IWeapon** weapon);
	Result CreateVehicle(const VehicleDesc* vehicledesc, Vector3* vpos, Quaternion* qrot, IVehicle** vehicle);
	Result CreateCollisionMesh(LPOBJECT obj, IRegularEntity** entity);
	Result CreateConvexCollisionHull(const HvkGeneralShapeDesc* shapedesc, LPOBJECT obj, IRegularEntity** entity);
	RegularEntity* CreateConvexCollisionHullWrapper(const HvkGeneralShapeDesc* shapedesc, LPOBJECT obj)
	{
		Result rlt;
		RegularEntity* entity;
		IFFAILED(CreateConvexCollisionHull(shapedesc, obj, (LPREGULARENTITY*)&entity))
			throw rlt.GetLastResult();
		return entity;
	}
	Result CreateCollisionCapsule(LPOBJECT obj, float radius, IRegularEntity** entity);
	Result CreateCollisionRagdoll(LPOBJECT obj, float radius, IRagdollEntity** entity);
	RagdollEntity* CreateCollisionRagdollWrapper(LPOBJECT obj, float radius)
	{
		Result rlt;
		RagdollEntity* entity;
		IFFAILED(CreateCollisionRagdoll(obj, radius, (IRagdollEntity**)&entity))
			throw rlt.GetLastResult();
		return entity;
	}
	float CastRayTo(const Vector3& src, const Vector3& dest, LPENTITY* hitentity, UINT32 (*shapekeys)[8], Vector3* hitnml);
	float CastRayDir(const Vector3& src, const Vector3& dir, LPENTITY* hitentity, UINT32 (*shapekeys)[8], Vector3* hitnml);
	void EnableViewer(HvkViewer viewer);
	void DisableViewer(HvkViewer viewer);
	String GetViewerName(HvkViewer viewer);
	void MousePickGrab(float* campos, float* camdir);
	void MousePickDrag(float* campos, float* camdir);
	void MousePickRelease();
	void Update();

	static Result CreateShape(const HvkShapeDesc* shapedesc, hkpShape** shape);
	static Result CreateConstraint(const ConstraintDesc* constraintdesc, hkpRigidBody* anchorbody, hkpRigidBody* connectedbody, hkpConstraintInstance** constraint);

	void Release();
};

void SyncWithPython();

#endif