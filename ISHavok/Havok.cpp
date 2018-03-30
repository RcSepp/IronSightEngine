#include "Havok.h"

#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>

#include <Common/Visualize/hkProcessFactory.h>

#include <Physics/Collide/Dispatch/hkpCollisionDispatcher.h>

// Viewer
#include <Physics/Utilities/VisualDebugger/Viewer/Collide/hkpBroadphaseViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Dynamics/hkpConstraintViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Collide/hkpActiveContactPointViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Collide/hkpInactiveContactPointViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Collide/hkpConvexRadiusViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Collide/hkpInconsistentWindingViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Collide/hkpMidphaseViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Utilities/hkpMousePickingViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Dynamics/hkpPhantomDisplayViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Dynamics/hkpRigidBodyCentreOfMassViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Dynamics/hkpRigidBodyInertiaViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Collide/hkpShapeDisplayViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Dynamics/hkpSimulationIslandViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Dynamics/hkpSingleBodyConstraintViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Dynamics/hkpSweptTransformDisplayViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Collide/hkpToiContactPointViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Collide/hkpToiCountViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Vehicle/hkpVehicleViewer.h>
#include <Physics/Utilities/VisualDebugger/Viewer/Collide/hkpWeldingViewer.h>

// Shapes
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/Convex/Cylinder/hkpCylinderShape.h>
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics/Internal/Collide/StaticCompound/hkpStaticCompoundShape.h>
//#include <Common/Internal/GeometryProcessing/ConvexHull/hkgpConvexHull.h>

#include <Common/Base/Types/Geometry/hkStridedVertices.h>

// Constraints
#include <Physics/Dynamics/Constraint/Bilateral/BallAndSocket/hkpBallAndSocketConstraintData.h>
#include <Physics/Dynamics/Constraint/Bilateral/Hinge/hkpHingeConstraintData.h>
#include <Physics/Dynamics/Constraint/Bilateral/LimitedHinge/hkpLimitedHingeConstraintData.h>
#include <Physics/Dynamics/Constraint/Bilateral/Ragdoll/hkpRagdollConstraintData.h>

// Keycode
#include <Common/Base/keycode.cxx>

// Productfeatures
// We're using only physics - we undef products even if the keycode is present so
// that we don't get the usual initialization for these products.
#undef HK_FEATURE_PRODUCT_AI
//#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR

#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
#define HK_EXCLUDE_FEATURE_RegisterVersionPatches
// Vdb needs the reflected classes
//#define HK_EXCLUDE_FEATURE_RegisterReflectedClasses
//#define HK_EXCLUDE_FEATURE_MemoryTracker

#include <Common/Base/Config/hkProductFeatures.cxx>

Vector3::Vector3(const hkVector4& hkvec)
{
	memcpy(&x, &hkvec.m_quad, 3 * sizeof(float));
}
Vector3::operator hkVector4() const
{
	return hkVector4(x, y, z);
}

Quaternion::Quaternion(const hkQuaternion& hkquat)
{
	memcpy(&x, &hkquat.m_vec.m_quad, 4 * sizeof(float));
}
Quaternion::operator hkQuaternion() const
{
	return hkQuaternion(x, y, z, w);
}

static void HK_CALL errorReport(const char* msg, void* userArgGivenToInit)
{
	cle->PrintLine(msg);
}

//-----------------------------------------------------------------------------
// Name: CreateHavok()
// Desc: DLL API for creating an instance of Havok
//-----------------------------------------------------------------------------
HAVOK_EXTERN_FUNC LPHAVOK __cdecl CreateHavok()
{
	return new Havok();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void Havok::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
	if(pse)
		SyncWithPython();
}

//-----------------------------------------------------------------------------
// Name: Havok()
// Desc: Constructor
//-----------------------------------------------------------------------------
Havok::Havok() : memutilinitialized(false)
{
	world = NULL;
	groupfilter = NULL;
	visualdebugger = NULL;
	localvisualizer = NULL;
	mousespring = NULL;
	time.t = time.dt = t = dt = 0.1f;
}

/*class MemAlloc : public hkMemoryAllocator
{
public:
	MemAlloc() {}

	void* blockAlloc(int numBytes)
	{
		return malloc(numBytes);
	}

	void blockFree(void* p, int numBytes)
	{
		free(p);
	}

	void getMemoryStatistics(MemoryStatistics& u)
	{
	}

	int getAllocatedSize(const void* obj, int nbytes)
	{
		return 0;
	}
};*/

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: ?
//-----------------------------------------------------------------------------
Result Havok::Init(const Vector3& worldsize_min, const Vector3& worldsize_max, VisualizationType vtype, LPOUTPUTWINDOW localvizwnd, LPRENDERSHADER localvizshader)
{
	Result rlt;

/*hkMallocAllocator mallocBase;
hkMemoryRouter* memoryRouter;
hkMemorySystem::FrameInfo frameInfo(6*1024*1024);	// the bridge in Havok destruction needs a large buffer
memoryRouter = hkMemoryInitUtil::initFreeListLargeBlock(&mallocBase, frameInfo);
extAllocator::initDefault();
CHKRESULT(hkBaseSystem::init(memoryRouter, errorReport));*/

/*MemAlloc* baseMalloc = new(malloc(sizeof(MemAlloc))) MemAlloc();
hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024));*/
hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(1024 * 1024));
hkBaseSystem::init(memoryRouter, errorReport);
memutilinitialized = true;

hkError::getInstance().setEnabled(0xF0123245, false); // Disable asserts on extreme velocities

	/*// Need to have memory allocated for the solver. Allocate 1mb for it.
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(1024 * 1024));
	CHKRESULT(hkBaseSystem::init(memoryRouter, errorReport));




	hkpWorldCinfo winfo = hkpWorldCinfo();
	winfo.m_gravity.set(0.0f, -9.807f, 0.0f);	
	winfo.setBroadPhaseWorldSize(300.0f);
	world = new hkpWorld(winfo);
	world->lock();

	// Register all collision agents
	// It's important to register collision agents before adding any entities to the world.
	hkpAgentRegisterUtil::registerAllAgents(world->getCollisionDispatcher());*/






	/*hkMallocAllocator baseMalloc;
	// Need to have memory allocated for the solver. Allocate 1mb for it.
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( &baseMalloc, hkMemorySystem::FrameInfo(2048 * 1024) );
	hkBaseSystem::init( memoryRouter, errorReport );*/

		hkpWorldCinfo winfo = hkpWorldCinfo();
		winfo.m_gravity.set(0.0f, -9.807f, 0.0f);
		//winfo.setBroadPhaseWorldSize(1000.0f);
		winfo.m_broadPhaseWorldAabb.m_min = worldsize_min;
		winfo.m_broadPhaseWorldAabb.m_max = worldsize_max;
		winfo.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_8ITERS_MEDIUM);//SOLVER_TYPE_4ITERS_SOFT);
		winfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;//BROADPHASE_BORDER_DO_NOTHING;//BROADPHASE_BORDER_REMOVE_ENTITY;
		//winfo.m_deactivationReferenceDistance = 0.01f;//0.005f;
		world = new hkpWorld(winfo);

		// Register all collision agents
		// It's important to register collision agents before adding any entities to the world.
		hkpCollisionDispatcher* dispatcher = world->getCollisionDispatcher();
		dispatcher->registerAlternateShapeType(hkcdShapeType::USER0, hkcdShapeType::BV_TREE); // Register BoxedLevelShape as tree shape
		dispatcher->registerAlternateShapeType(hkcdShapeType::USER1, hkcdShapeType::BV_TREE); // Register BoxedLevelChunkShape as tree shape
		hkpAgentRegisterUtil::registerAllAgents(dispatcher);

/*hkpCollisionDispatcher::ShapeInheritance foo[100];
ZeroMemory(foo, sizeof(foo));
memcpy(foo, &dispatcher->m_shapeInheritance[0], dispatcher->m_shapeInheritance.getSize() * sizeof(hkpCollisionDispatcher::ShapeInheritance));
int abc = 0;*/

	// >>> Init collision filter

	groupfilter = new hkpGroupFilter();

	groupfilter->disableCollisionsBetween(LAYER_LANDSCAPE, LAYER_RAGDOLL_KEYFRAMED);
	groupfilter->disableCollisionsBetween(LAYER_PROXY, LAYER_DEBRIS);
	groupfilter->disableCollisionsBetween(LAYER_PROXY, LAYER_RAGDOLL_KEYFRAMED);
	groupfilter->disableCollisionsBetween(LAYER_PROXY, LAYER_RAYCAST);
	groupfilter->disableCollisionsBetween(LAYER_PROXY, LAYER_RAGDOLL_DYNAMIC);
	groupfilter->disableCollisionsBetween(LAYER_DEBRIS, LAYER_RAYCAST);
	groupfilter->disableCollisionsBetween(LAYER_MOVABLE_ENVIRONMENT, LAYER_RAGDOLL_KEYFRAMED);
	groupfilter->disableCollisionsBetween(LAYER_RAGDOLL_KEYFRAMED, LAYER_RAYCAST);
	groupfilter->disableCollisionsBetween(LAYER_RAYCAST, LAYER_RAGDOLL_DYNAMIC);
	groupfilter->disableCollisionsBetween(LAYER_PICKING, LAYER_LANDSCAPE);
	groupfilter->disableCollisionsBetween(LAYER_PICKING, LAYER_PROXY);
	groupfilter->disableCollisionsBetween(LAYER_PICKING, LAYER_RAGDOLL_KEYFRAMED);

	groupfilter->disableCollisionsBetween(LAYER_RAGDOLL_KEYFRAMED, LAYER_HANDHELD);
	groupfilter->disableCollisionsBetween(LAYER_RAGDOLL_DYNAMIC, LAYER_HANDHELD);
	groupfilter->disableCollisionsBetween(LAYER_PROXY, LAYER_HANDHELD);

	world->setCollisionFilter(groupfilter);
	groupfilter->removeReference();

	// >>> Init visualizer (local visualizer or Havok Visual Debugger)

	if(vtype != VT_NONE)
	{
		// Register all the physics viewers
		hkpPhysicsContext::registerAllPhysicsProcesses();

		// Set up a physics context containing the world
		hkpPhysicsContext* context = new hkpPhysicsContext;
		context->addWorld(world);
		contexts.pushBack(context);

		// Set up the visual debugger
		if(vtype == VT_VISUALDEBUGGER)
		{
			// Setup the visual debugger
			visualdebugger = new hkVisualDebugger(contexts);
			visualdebugger->serve();
		}
		else // vtype == VT_LOCAL
		{
			// Create local visualizer
			if(!localvizwnd)
				return ERR("An output window must be provided when using the local visualizer");
			if(!localvizshader)
				return ERR("A render shader must be provided when using the local visualizer");
			localvisualizer = new LocalVisualizer(localvizwnd, localvizshader);
		}
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: InitDone()
// Desc: Finalize initial content creation. Should be called after entities, etc. are created and before the main game loop takes over
//-----------------------------------------------------------------------------
void Havok::InitDone()
{
	/*//world->unlock();

	// Register all the physics viewers
	hkpPhysicsContext::registerAllPhysicsProcesses(); 

	// Set up a physics context containing the world for the use in the visual debugger
	context = new hkpPhysicsContext;
	context->addWorld(world); 

	// Set up the visual debugger
	{
		// Setup the visual debugger
		hkArray<hkProcessContext*> contexts;
		contexts.pushBack(context);

		visualdebugger = new hkvisualdebugger(contexts);
		visualdebugger->serve();
	}*/

	// Enable stabilized solver and pre-stabilize inertias (needed for constraints)
	hkpConstraintStabilizationUtil::setConstraintsSolvingMethod(world, hkpConstraintAtom::METHOD_STABILIZED);
	hkpConstraintStabilizationUtil::stabilizePhysicsWorldInertias(world);
}

//-----------------------------------------------------------------------------
// Name: CreateXXXEntity()
// Desc: Create and register a collidable entity
//-----------------------------------------------------------------------------
Result Havok::CreateRegularEntity(const HvkShapeDesc* shapedesc, Layer layer, Vector3* vpos, Quaternion* qrot, IRegularEntity** entity)
{
	*entity = NULL;
	Result rlt;

	RegularEntity* newentity;
	CHKALLOC(newentity = new RegularEntity(this));
	CHKRESULT(newentity->Create(shapedesc, layer, vpos, qrot));

	entities.push_back(newentity);
	*entity = newentity;

	return R_OK;
}

Result Havok::CreateLevelEntity(const HvkStaticCompoundShapeDesc* shapedesc, const Vector3& vpos, const Quaternion& qrot, ILevelEntity** entity)
{
	*entity = NULL;
	Result rlt;

	LevelEntity* newentity;
	CHKALLOC(newentity = new LevelEntity(this));
	CHKRESULT(newentity->Create(shapedesc, vpos, qrot));

	entities.push_back(newentity);
	*entity = newentity;

	return R_OK;
}

Result Havok::CreateBoxedLevelEntity(const UINT (&chunksize)[3], IBoxedLevelEntity** entity)
{
	*entity = NULL;
	Result rlt;

	BoxedLevelEntity* newentity;
	CHKALLOC(newentity = new BoxedLevelEntity(this));
	CHKRESULT(newentity->Create(chunksize));

	entities.push_back(newentity);
	*entity = newentity;

	return R_OK;
}

Result Havok::CreateRagdollEntity(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot, IRagdollEntity** entity)
{
	*entity = NULL;
	Result rlt;

	RagdollEntity* newentity;
	CHKALLOC(newentity = new RagdollEntity(this));
	CHKRESULT(newentity->Create(shapedesc, vpos, qrot));

	entities.push_back(newentity);
	*entity = newentity;

	return R_OK;
}

Result Havok::CreatePlayerEntity(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot, bool isrigidbody, IPlayerEntity** entity)
{
	*entity = NULL;
	Result rlt;

	if(isrigidbody)
	{
		PlayerRBEntity* newentity;
		CHKALLOC(newentity = new PlayerRBEntity(this));
		CHKRESULT(newentity->Create(shapedesc, vpos, qrot));

//		entities.push_back(newentity);
		*entity = newentity;
	}
	else
	{
		PlayerEntity* newentity;
		CHKALLOC(newentity = new PlayerEntity(this));
		CHKRESULT(newentity->Create(shapedesc, vpos, qrot));

//		entities.push_back(newentity);
		*entity = newentity;
	}

	return R_OK;
}

Result Havok::CreateWeapon(IWeapon** weapon)
{
	*weapon = NULL;
	Result rlt;

	Weapon* newweapon;
	CHKALLOC(newweapon = new Weapon(this));
	CHKRESULT(newweapon->Init());

	weapons.push_back(newweapon);
	*weapon = newweapon;

	return R_OK;
}

Result Havok::CreateVehicle(const VehicleDesc* vehicledesc, Vector3* vpos, Quaternion* qrot, IVehicle** vehicle)
{
	*vehicle = NULL;
	Result rlt;

	Vehicle* newvehicle;
	CHKALLOC(newvehicle = new Vehicle(this));
	CHKRESULT(newvehicle->Init(vehicledesc, vpos, qrot));

	vehicles.push_back(newvehicle);
	*vehicle = newvehicle;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CastRay()
// Desc: Cast a ray into world
//-----------------------------------------------------------------------------
float Havok::CastRayTo(const Vector3& src, const Vector3& dest, LPENTITY* hitentity, UINT32 (*shapekeys)[8], Vector3* hitnml)
{
	return CastRay(hkVector4(src[0], src[1], src[2]), hkVector4(dest[0], dest[1], dest[2]), hitentity, shapekeys, hitnml);
}
float Havok::CastRayDir(const Vector3& src, const Vector3& dir, LPENTITY* hitentity, UINT32 (*shapekeys)[8], Vector3* hitnml)
{
	hkVector4 hkSrc = hkVector4(src[0], src[1], src[2]);
	hkVector4 hkDir = hkVector4(dir[0], dir[1], dir[2]);

	hkVector4 v = world->m_broadPhaseExtents[0];
	v.sub(world->m_broadPhaseExtents[1]);
	hkReal worldsize = v.length3();

	hkDir.normalize3();
	hkDir.mul(worldsize);
	hkDir.add(hkSrc);

	return CastRay(hkSrc, hkDir, hitentity, shapekeys, hitnml);
}
float Havok::CastRay(hkVector4& src, hkVector4& dest, LPENTITY* hitentity, UINT32 (*shapekeys)[8], Vector3* hitnml)
{
	hkpWorldRayCastInput input;
	input.m_from = src;
	input.m_to = dest;

	hkpWorldRayCastOutput output;
	world->castRay(input, output);

	if(output.hasHit())
	{
		if(hitentity)
			*hitentity = (LPENTITY)output.m_rootCollidable->getShape()->getUserData();
		if(shapekeys)
			memcpy(shapekeys, output.m_shapeKeys, 8 * sizeof(UINT32));
		if(hitnml)
			*hitnml = output.m_normal;
		dest.sub(src);
		return output.m_hitFraction * dest.length3();
	}
	else
	{
		if(hitentity)
			*hitentity = NULL;
		if(shapekeys)
			ZeroMemory(shapekeys, 8 * sizeof(UINT32));
		return 1e20f;
	}
}

//-----------------------------------------------------------------------------
// Name: Enable/DisableViewer()
// Desc: Add/Remove viewer process to be used by the local visualizer
//---------------------------------------------------------------------------
void Havok::EnableViewer(HvkViewer viewer)
{
	if(!localvisualizer)
	{
		LOG("Can't enable viewer: Local visualizer not running");
		return;
	}

	String viewername = GetViewerName(viewer);

	if(viewerprocesses.find(viewer) != viewerprocesses.end())
		return;

	hkProcess* process = hkProcessFactory::getInstance().createProcess(viewername, contexts);
	HK_ASSERT2(0x30675a6b, process, "Failed to create - did you register all the processes for the product(s) you are using?");
	process->m_inStream = HK_NULL;
	process->m_outStream = HK_NULL;
	process->m_processHandler = HK_NULL;
	process->m_displayHandler = localvisualizer;
	process->init();
	viewerprocesses[viewer] = process;
}
void Havok::DisableViewer(HvkViewer viewer)
{
	std::map<HvkViewer, hkProcess*>::iterator pair = viewerprocesses.find(viewer);

	if(pair != viewerprocesses.end())
	{
		delete pair->second;
		viewerprocesses.erase(pair);
	}
}
String Havok::GetViewerName(HvkViewer viewer)
{
	switch(viewer)
	{
	case HV_BROADPHASE: return hkpBroadphaseViewer::getName();
	case HV_CONSTRAINT: return hkpConstraintViewer::getName();
	case HV_ACTIVECONTACTPOINT: return hkpActiveContactPointViewer::getName();
	case HV_INACTIVECONTACTPOINT: return hkpInactiveContactPointViewer::getName();
	case HV_CONVEXRADIUS: return hkpConvexRadiusViewer::getName();
	case HV_INCONSISTENDWINDING: return hkpInconsistentWindingViewer::getName();
	case HV_MIDPHASE: return hkpMidphaseViewer::getName();
	case HV_MOUSEPICKING: return hkpMousePickingViewer::getName();
	case HV_PHANTOMDISPLAY: return hkpPhantomDisplayViewer::getName();
	case HV_RIGIDBODYCENTEROFMASS: return hkpRigidBodyCentreOfMassViewer::getName();
	case HV_RIGIDBODYINERTIA: return hkpRigidBodyInertiaViewer::getName();
	case HV_SHAPEDISPLAY: return hkpShapeDisplayViewer::getName();
	case HV_SIMULATIONISLAND: return hkpSimulationIslandViewer::getName();
	case HV_SINGLEBODYCONSTRAINT: return hkpSingleBodyConstraintViewer::getName();
	case HV_SWEPTTRANSFORMDISPLAY: return hkpSweptTransformDisplayViewer::getName();
	case HV_TOICONTACTPOINT: return hkpToiContactPointViewer::getName();
	case HV_TOICOUNT: return hkpToiCountViewer::getName();
	case HV_VEHICLE: return hkpVehicleViewer::getName();
	case HV_WELDING: return hkpWeldingViewer::getName();
	default: return String("");
	}
}

void Havok::MousePickGrab(float* campos, float* camdir)
{
	hkVector4 hkSrc = hkVector4(campos[0], campos[1], campos[2]);
	hkVector4 hkDest = hkVector4(camdir[0], camdir[1], camdir[2]);

	hkVector4 v = world->m_broadPhaseExtents[0];
	v.sub(world->m_broadPhaseExtents[1]);
	hkReal worldsize = v.length3();

	hkDest.normalize3();
	hkDest.mul(worldsize);
	hkDest.add(hkSrc);

	hkpWorldRayCastInput input;
	input.m_from = hkSrc;
	input.m_to = hkDest;
	hkpWorldRayCastOutput output;
	world->castRay(input, output);

	if(!output.hasHit())
		return;

	// dist = ((dest - src) * hitfraction).length
	// targetpos = src + (dest - src) * hitfraction
	hkDest.sub(hkSrc);
	hkDest.mul(output.m_hitFraction);
	mousepickdist = hkDest.length3();
	hkSrc.add(hkDest);
	hkVector4 targetpos = hkSrc;

	if(!output.m_rootCollidable->getOwner())
		return;

	hkpRigidBody* rigidbody = hkpGetRigidBody(output.m_rootCollidable);
	if(!rigidbody || rigidbody->isFixed())
		return;

	const hkReal springDamping = 0.5f;
	const hkReal springElasticity = 0.3f;
	const hkReal objectDamping = 0.95f;
	hkVector4 positionAinA;
	positionAinA.setTransformedInversePos(rigidbody->getTransform(), targetpos);
	world->lock();
	mousespring = new hkpMouseSpringAction(positionAinA, targetpos, springDamping, springElasticity, objectDamping, rigidbody, NULL);
	mousespring->setMaxRelativeForce(1000.0f);
	world->addAction(mousespring);
	world->unlock();
}
void Havok::MousePickDrag(float* campos, float* camdir)
{
	if(!mousespring)
		return;

	hkVector4 hkSrc = hkVector4(campos[0], campos[1], campos[2]);
	hkVector4 hkDir = hkVector4(camdir[0], camdir[1], camdir[2]);
	hkDir.mul(mousepickdist);
	hkSrc.add(hkDir);
	hkVector4 targetpos = hkSrc;

	world->lock();
	mousespring->setMousePosition(targetpos);
	world->unlock();
}
void Havok::MousePickRelease()
{
	if(!mousespring)
		return;
	world->lock();

	world->removeAction(mousespring);
	hkpRigidBody* rigidbody = (hkpRigidBody*)mousespring->getEntity();
	rigidbody->activate();
	mousespring->removeReference();

	world->unlock();
	mousespring = NULL;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Tell physics engine to progress dynamics
//-----------------------------------------------------------------------------
#define MAX_UPDATES_PER_FRAME	8
#define MAX_UPDATE_TIME			1.0f
void Havok::Update()
{
//// Delay-start physics engine
//if(eng->time.t < 2.0f)
//	return;

	static float skippedframe_dt = 0.0f;
	dt = eng->time.dt + skippedframe_dt;
	t += dt;
	if(dt < 0.005f) // Skip frame if fps > 200
	{
		skippedframe_dt += dt;
		return;
	}
	else
		skippedframe_dt = 0.0f;

	float total_dt = dt;
	for(int updatenum = 0; updatenum < MAX_UPDATES_PER_FRAME && total_dt > 0.0f; updatenum++)
	{
		time.t += time.dt = min(total_dt, MAX_UPDATE_TIME);
		_Update();
		total_dt -= time.dt;
	}
}
void Havok::_Update()
{
	std::list<Entity*>::iterator entityiter;
	std::list<Vehicle*>::iterator vehicleiter;

	// Perform pre-updates (setting forces, ...)
	LIST_FOREACH(entities, entityiter)
		(*entityiter)->PreUpdate();
	LIST_FOREACH(vehicles, vehicleiter)
		(*vehicleiter)->PreUpdate();

	if(world)
		world->stepDeltaTime(time.dt);

	std::map<HvkViewer, hkProcess*>::iterator mapiter;
	LIST_FOREACH(viewerprocesses, mapiter)
		mapiter->second->step(time.dt * 1000.0f);

	if(visualdebugger)
		visualdebugger->step();

	// Perform post-updates (applying changes, ...)
	LIST_FOREACH(entities, entityiter)
		(*entityiter)->Update();
	std::list<Weapon*>::iterator weaponiter;
	LIST_FOREACH(weapons, weaponiter)
		(*weaponiter)->Update();
	LIST_FOREACH(vehicles, vehicleiter)
		(*vehicleiter)->Update();
}

//-----------------------------------------------------------------------------
// Name: CreateShape()
// Desc: Static function that holds all the code to generate any kind of shape from the corresponding shape description struct
//-----------------------------------------------------------------------------
Result Havok::CreateShape(const HvkShapeDesc* shapedesc, hkpShape** shape)
{
	switch(shapedesc->GetId())
	{
	case HSID_CAPSULE:
		{
			HvkCapsuleShapeDesc desc = *(HvkCapsuleShapeDesc*)shapedesc;

			*shape = new hkpCapsuleShape(desc.v0, desc.v1, desc.radius);
		}
		return R_OK;

	case HSID_CYLINDER:
		{
			HvkCylinderShapeDesc desc = *(HvkCylinderShapeDesc*)shapedesc;

			*shape = new hkpCylinderShape(desc.v0, desc.v1, desc.radius);
		}
		return R_OK;

	case HSID_SPHERE:
		{
			HvkSphereShapeDesc desc = *(HvkSphereShapeDesc*)shapedesc;

			*shape = new hkpSphereShape(desc.radius);
		}
		return R_OK;

	case HSID_BOX:
		{
			HvkBoxShapeDesc desc = *(HvkBoxShapeDesc*)shapedesc;

			*shape = new hkpBoxShape(hkVector4(desc.size.x / 2.0f, desc.size.y / 2.0f, desc.size.z / 2.0f));
		}
		return R_OK;

	case HSID_PLANE:
		{
			HvkPlaneShapeDesc desc = *(HvkPlaneShapeDesc*)shapedesc;

			*shape = new hkpPlaneShape(desc.vdir, desc.vcenter, desc.vhalfext);
		}
		return R_OK;

	case HSID_CONVEXVERTICES:
		{
			HvkConvexVerticesShapeDesc desc = *(HvkConvexVerticesShapeDesc*)shapedesc;
			hkStridedVertices vertices;
			vertices.m_vertices = (const hkReal*)desc.vertices;
			vertices.m_numVertices = desc.numvertices;
			vertices.m_striding = desc.fvfstride;
			hkpConvexVerticesShape::BuildConfig config;
			config.m_maxVertices = desc.numvertices >> 1; // Reduce number of vertices by a factor of 2 (optimization)
			*shape = new hkpConvexVerticesShape(vertices, config);
		}
		return R_OK;

	case HSID_COMPRESSEDMESH:
		{
			HvkCompressedMeshShapeDesc desc = *(HvkCompressedMeshShapeDesc*)shapedesc;

			hkGeometry terrainGeometry;
			hkArray<hkVector4>& vertices = terrainGeometry.m_vertices;
			hkArray<hkGeometry::Triangle>& triangles = terrainGeometry.m_triangles;

			vertices.setSize(desc.numvertices);
			triangles.setSize(desc.numfaces);

			hkVector4* vts = vertices.begin();
			for(UINT i = 0; i < desc.numvertices; i++)
			{
				vts[i].set(((float*)desc.vertexdata)[0], ((float*)desc.vertexdata)[1], ((float*)desc.vertexdata)[2]);
				desc.vertexdata += desc.fvfstride;
			}

			hkGeometry::Triangle* trs = triangles.begin();
			for(UINT i = 0; i < desc.numfaces; i++)
				trs[i].set(desc.indices[i * 3 + 0], desc.indices[i * 3 + 1], desc.indices[i * 3 + 2]);

			hkpDefaultBvCompressedMeshShapeCinfo landscapeinfo(&terrainGeometry);
			Result rlt;
			for(UINT i = 0; i < desc.numshapes; i++)
			{
				hkpShape* hkshape;
				CHKRESULT(CreateShape(desc.convexshapes[i].shape, &hkshape));
				landscapeinfo.addConvexShape((hkpConvexShape*)hkshape, hkQsTransform(desc.convexshapes[i].pos, desc.convexshapes[i].qrot, desc.convexshapes[i].scale));
			}
			*shape = new hkpBvCompressedMeshShape(landscapeinfo);
		}
		return R_OK;

		case HSID_STATICCOMPOUND:
		{
			HvkStaticCompoundShapeDesc desc = *(HvkStaticCompoundShapeDesc*)shapedesc;

			hkpStaticCompoundShape* scshape = new hkpStaticCompoundShape();
			Result rlt;
			for(UINT i = 0; i < desc.numshapes; i++)
			{
				hkpShape* hkshape;
				CHKRESULT(CreateShape(desc.shapes[i].shape, &hkshape));
				scshape->addInstance((hkpConvexShape*)hkshape, hkQsTransform(desc.shapes[i].pos, desc.shapes[i].qrot, desc.shapes[i].scale));
			}
			scshape->bake();
			*shape = scshape;
		}
		return R_OK;

	default:
		return ERR("Shape type not implemented");
	}
}

//-----------------------------------------------------------------------------
// Name: CreateConstraint()
// Desc: Static function that holds all the code to generate any kind of constraint from the corresponding constraint description struct
//-----------------------------------------------------------------------------
hkpRagdollConstraintData* HK_CALL createRagdollContraint( //EDIT: Cleanup
	const hkpRigidBody* rbReference,
	const hkpRigidBody* rbAttached,
	hkReal coneMin,
	hkReal coneMax,
	hkReal planeMin,
	hkReal planeMax,
	hkReal twistMin,
	hkReal twistMax,
	const hkVector4 &pivot,
	const hkVector4 &twistAxis,
	const hkVector4 &planeAxis )
{
	hkVector4 pivotReference;
	hkVector4 twistReference;
	hkVector4 planeReference;
	pivotReference.setTransformedInversePos( rbReference->getTransform(), pivot );
	twistReference.setRotatedInverseDir( rbReference->getRotation(), twistAxis ); twistReference.normalize3();
	planeReference.setRotatedInverseDir( rbReference->getRotation(), planeAxis ); planeReference.normalize3();

	hkVector4 pivotAttached;
	hkVector4 twistAttached;
	hkVector4 planeAttached;
	pivotAttached.setTransformedInversePos( rbAttached->getTransform(), pivot );
	twistAttached.setRotatedInverseDir( rbAttached->getRotation(), twistAxis );	twistAttached.normalize3();
	planeAttached.setRotatedInverseDir( rbAttached->getRotation(), planeAxis ); planeAttached.normalize3();

	// check for unsymmetrical cone axes
	hkReal centerCone	= (coneMax + coneMin) * 0.5f;
	hkReal diffCone		= (coneMax - coneMin) * 0.5f;

	if( hkMath::fabs( centerCone ) > HK_REAL_EPSILON )
	{
		twistReference._setRotatedDir( hkQuaternion( planeReference, -centerCone ), twistReference );
	}

	hkpRagdollConstraintData* bob = new hkpRagdollConstraintData( );
	bob->setInBodySpace(	pivotReference,	pivotAttached,
		planeReference,	planeAttached,
		twistReference,	twistAttached );

	bob->setConeAngularLimit( diffCone );
	bob->setPlaneMinAngularLimit( planeMin );
	bob->setPlaneMaxAngularLimit( planeMax );
	bob->setTwistMinAngularLimit( twistMin );
	bob->setTwistMaxAngularLimit( twistMax );
	bob->setMaxFrictionTorque( 0.0f );

	return bob;
}
Result Havok::CreateConstraint(const ConstraintDesc* constraintdesc, hkpRigidBody* anchorbody, hkpRigidBody* connectedbody, hkpConstraintInstance** constraint)
{
	switch(constraintdesc->GetId())
	{
	case CID_BALLANDSOCKET:
		{
			BallAndSocketConstraintDesc desc = *(BallAndSocketConstraintDesc*)constraintdesc;

			hkVector4 hkPivot(desc.pivot[0], desc.pivot[1], desc.pivot[2]);
			//hkPivot.setRotatedDir(anchorbody->getRotation(), hkPivot);
			//hkPivot.add(anchorbody->getPosition());
/*hkPivot.setRotatedDir(connectedbody->getRotation(), hkPivot);
hkPivot.add(connectedbody->getPosition());*/

Vector3* userdata = new Vector3[2];
connectedbody->setPosition(hkPivot);
userdata[1].x = hkPivot.getComponent(0); userdata[1].y = hkPivot.getComponent(1); userdata[1].z = hkPivot.getComponent(2);
hkPivot.sub(anchorbody->getPosition());
userdata[0].x = hkPivot.getComponent(0); userdata[0].y = hkPivot.getComponent(1); userdata[0].z = hkPivot.getComponent(2);

			// Create constraint data
			hkpBallAndSocketConstraintData* data = new hkpBallAndSocketConstraintData();
			data->setInBodySpace(hkVector4(0.0f, 0.0f, 0.0f), hkPivot);
			//data->setInWorldSpace(connectedbody->getTransform(), anchorbody->getTransform(), hkPivot);

			// Create constraint instance
			*constraint = new hkpConstraintInstance(connectedbody, anchorbody, data);
			(*constraint)->setUserData((hkUlong)userdata);//new hkVector4(desc.pivot[0], desc.pivot[1], desc.pivot[2]));
			data->removeReference();
		}
		return R_OK;

	case CID_HINGE:
		{
			HingeConstraintDesc desc = *(HingeConstraintDesc*)constraintdesc;

			hkVector4 hkPivot(desc.pivot[0], desc.pivot[1], desc.pivot[2]);
			//hkPivot.setRotatedDir(anchorbody->getRotation(), hkPivot);
			//hkPivot.add(anchorbody->getPosition());
/*hkPivot.setRotatedDir(connectedbody->getRotation(), hkPivot);
hkPivot.add(connectedbody->getPosition());*/

Vector3* userdata = new Vector3[2];
connectedbody->setPosition(hkPivot);
userdata[1].x = hkPivot.getComponent(0); userdata[1].y = hkPivot.getComponent(1); userdata[1].z = hkPivot.getComponent(2);
hkPivot.sub(anchorbody->getPosition());
userdata[0].x = hkPivot.getComponent(0); userdata[0].y = hkPivot.getComponent(1); userdata[0].z = hkPivot.getComponent(2);

			hkVector4 hkAxis(desc.axis[0], desc.axis[1], desc.axis[2]);

			// Create constraint data
			hkpHingeConstraintData* data = new hkpHingeConstraintData();
			data->setInWorldSpace(connectedbody->getTransform(), anchorbody->getTransform(), hkPivot, hkAxis);

			// Create constraint instance
			*constraint = new hkpConstraintInstance(connectedbody, anchorbody, data);
			(*constraint)->setUserData((hkUlong)userdata);//new hkVector4(desc.pivot[0], desc.pivot[1], desc.pivot[2]));
			data->removeReference();
		}
		return R_OK;

	case CID_LIMITEDHINGE:
		{
			LimitedHingeConstraintDesc desc = *(LimitedHingeConstraintDesc*)constraintdesc;

			hkVector4 hkPivot(desc.pivot[0], desc.pivot[1], desc.pivot[2]);
			//hkPivot.setRotatedDir(anchorbody->getRotation(), hkPivot);
			//hkPivot.add(anchorbody->getPosition());
/*hkPivot.setRotatedDir(connectedbody->getRotation(), hkPivot);
hkPivot.add(connectedbody->getPosition());*/

Vector3* userdata = new Vector3[2];
connectedbody->setPosition(hkPivot);
userdata[1].x = hkPivot.getComponent(0); userdata[1].y = hkPivot.getComponent(1); userdata[1].z = hkPivot.getComponent(2);
hkPivot.sub(anchorbody->getPosition());
userdata[0].x = hkPivot.getComponent(0); userdata[0].y = hkPivot.getComponent(1); userdata[0].z = hkPivot.getComponent(2);

			hkVector4 hkAxis(desc.axis[0], desc.axis[1], desc.axis[2]);

			// Create constraint data
			hkpLimitedHingeConstraintData* data = new hkpLimitedHingeConstraintData();
			data->setMinAngularLimit(desc.angleMin);
			data->setMaxAngularLimit(desc.angleMax);
			data->setInWorldSpace(connectedbody->getTransform(), anchorbody->getTransform(), hkPivot, hkAxis);

			// Create constraint instance
			*constraint = new hkpConstraintInstance(connectedbody, anchorbody, data);
			(*constraint)->setUserData((hkUlong)userdata);//new hkVector4(desc.pivot[0], desc.pivot[1], desc.pivot[2]));
			data->removeReference();
		}
		return R_OK;

	case CID_RAGDOLL:
		{
			RagdollConstraintDesc desc = *(RagdollConstraintDesc*)constraintdesc;

			hkVector4 hkPivot(desc.pivot[0], desc.pivot[1], desc.pivot[2]);
			//hkPivot.setRotatedDir(anchorbody->getRotation(), hkPivot);
			//hkPivot.add(anchorbody->getPosition());
/*hkPivot.setRotatedDir(connectedbody->getRotation(), hkPivot);
hkPivot.add(connectedbody->getPosition());*/

Vector3* userdata = new Vector3[2];
connectedbody->setPosition(hkPivot);
userdata[1].x = hkPivot.getComponent(0); userdata[1].y = hkPivot.getComponent(1); userdata[1].z = hkPivot.getComponent(2);
hkPivot.sub(anchorbody->getPosition());
userdata[0].x = hkPivot.getComponent(0); userdata[0].y = hkPivot.getComponent(1); userdata[0].z = hkPivot.getComponent(2);

			hkVector4 twistAxis(desc.twistAxis[0], desc.twistAxis[1], desc.twistAxis[2]);
			hkVector4 planeAxis(desc.planeAxis[0], desc.planeAxis[1], desc.planeAxis[2]);

			// Create constraint data
			hkpRagdollConstraintData* data = createRagdollContraint(connectedbody, anchorbody, desc.coneMin, desc.coneMax, desc.planeMin, desc.planeMax, desc.twistMin, desc.twistMax, hkPivot, twistAxis, planeAxis);

			// Create constraint instance
			*constraint = new hkpConstraintInstance(connectedbody, anchorbody, data);
			(*constraint)->setUserData((hkUlong)userdata);//new hkVector4(desc.pivot[0], desc.pivot[1], desc.pivot[2]));
			data->removeReference();
		}
		return R_OK;

	default:
		return ERR("Constraint type not implemented");
	}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Havok::Release()
{
	Result rlt;

	std::map<HvkViewer, hkProcess*>::iterator iter;
	LIST_FOREACH(viewerprocesses, iter)
		delete iter->second;
	viewerprocesses.clear();

	while(entities.size())
		entities.front()->Release();
	while(weapons.size())
		weapons.front()->Release();
	while(vehicles.size())
		vehicles.front()->Release();

	HAVOK_RELEASE(visualdebugger);
	REMOVE(localvisualizer);
	HAVOK_RELEASE(world);
	if(!contexts.isEmpty())
		((hkpPhysicsContext*)contexts[0])->removeReference();

	delete this; // Remove local havok data types (i.e. havok arrays) before the memory util gets released

	LOGERRORS(hkBaseSystem::quit());
//	if(memutilinitialized)
//		LOGERRORS(hkMemoryInitUtil::quit()); //EDIT: Throws NULL-pointer exception
}