#include "Havok.h"
#include <Animation/Ragdoll/Utils/hkaRagdollUtils.h>
#include <Animation/Animation/Ik/LookAt/hkaLookAtIkSolver.h>
#include <Physics/Utilities/Collide/Filter/GroupFilter/hkpGroupFilterUtil.h>

#include <Physics/Utilities/Constraint/Chain/hkpPoweredChainMapper.h>
#include <Physics/Utilities/Constraint/Chain/hkpPoweredChainMapperUtil.h>

#include <Common/Base/Reflection/hkClass.h>

class MyCharacterListener2 : public hkReferencedObject, public hkpCharacterProxyListener
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO);

		MyCharacterListener2( ) 
		: m_atLadder(false), contactpointcounter(0)
		{}

		// Ladder handling code goes here
		void contactPointAddedCallback( const hkpCharacterProxy* proxy, const hkpRootCdPoint& point )
		{
			contactpointcounter++;

			hkReal ny = point.getContact().getNormal().getComponent(1);
			if(ny > -0.7f && ny < 0.1f) // If non-ground contact point added
			{
				m_atLadder = true;
				m_ladderNorm = point.m_contact.getNormal();
				hkpRigidBody* body = hkpGetRigidBody(point.m_rootCollidableB);
				body->getPointVelocity(point.m_contact.getPosition(), m_ladderVelocity);
			}
			else // If ground contact point added
				m_atLadder = false;
		}

		void contactPointRemovedCallback( const hkpCharacterProxy* proxy, const hkpRootCdPoint& point)
		{
			if(--contactpointcounter == 0) // If no contact points (player climbed into air)
				m_atLadder = false;
		}

	public:

		hkBool m_atLadder;

		hkVector4 m_ladderNorm;

		hkVector4 m_ladderVelocity;

		int contactpointcounter;

};

Result RagdollEntity::Create(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot)
{
	if(shapedesc->mtype == MT_FIXED)
		LOG("Player entities can't be static (property ignored)");

	// Store position and rotation pointers for physics driven character movement (inside Update())
	this->vpos = vpos;
	this->qrot = qrot;

	// Store initial transform
	transform->setIdentity();
	transform->setTranslation(*vpos);
	transform->setRotation(*qrot);

	// Create shape
	hkpShape* shape;
	Result rlt;
	CHKRESULT(Havok::CreateShape(shapedesc, &shape));
	shape->setUserData((hkUlong)this);

	// Construct a Shape Phantom
	hkpShapePhantom* m_phantom;
	hkTransform shapetransform;
	transform->copyToTransformNoScale(shapetransform);
	m_phantom = new hkpSimpleShapePhantom(shape, shapetransform, hkpGroupFilter::calcFilterInfo(Havok::LAYER_PROXY));
		
	// Add the phantom to the world
	parent->world->addPhantom(m_phantom);
	m_phantom->removeReference();

	// Construct a character proxy
	hkpCharacterProxyCinfo cpci;
	cpci.m_position = hkVector4(transform->getTranslation());
	cpci.m_staticFriction = 0.0f;
	cpci.m_dynamicFriction = 1.0f;
	cpci.m_up.setNeg4(parent->world->getGravity());
	cpci.m_up.normalize3();	
	cpci.m_userPlanes = 4;
	cpci.m_maxSlope = HK_REAL_PI / 2.0f; // 90° => Let the character walk up stairs
	cpci.m_characterMass = 70.0f;

	cpci.m_shapePhantom = m_phantom;
	m_characterProxy = new hkpCharacterProxy(cpci);
	
	// Add in a custom friction model
	hkVector4 up(0.0f, 1.0f, 0.0f);
	m_listener = new MyCharacterListener2();
	m_characterProxy->addCharacterProxyListener(m_listener);

	// >>> Create the Character state machine and context

	hkpCharacterState* state;
	hkpCharacterStateManager* manager = new hkpCharacterStateManager();

	state = new hkpCharacterStateOnGround();
	manager->registerState(state, HK_CHARACTER_ON_GROUND);
	state->removeReference();

	state = new hkpCharacterStateInAir();
	manager->registerState(state, HK_CHARACTER_IN_AIR);
	state->removeReference();

	state = new hkpCharacterStateJumping();
((hkpCharacterStateJumping*)state)->setJumpHeight(1.0f);
	manager->registerState(state, HK_CHARACTER_JUMPING);
	state->removeReference();

	state = new hkpCharacterStateClimbing();
	manager->registerState(state, HK_CHARACTER_CLIMBING);
	state->removeReference();

	m_characterContext = new hkpCharacterContext(manager, HK_CHARACTER_ON_GROUND);
	manager->removeReference();

	return R_OK;
}

Result RagdollEntity::SetRootBone(const HvkShapeDesc* shapedesc, const String& shapekey, const Matrix* worldmatrix)
{
	if(shapedesc->mtype == MT_FIXED)
		worldmatrix = NULL;

	// Create system group to disable collisions between bones of the same ragdoll
	collisiongroup = parent->groupfilter->getNewSystemGroup();

	// Create physics system
	ragdoll = new hkpPhysicsSystem();

	// Create shape
	hkpShape* shape;
	Result rlt;
	CHKRESULT(Havok::CreateShape(shapedesc, &shape));
	shape->setUserData((hkUlong)this);

	// Create rigid body info
	hkpRigidBodyCinfo binfo;
	binfo.m_shape = shape;
	binfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(Havok::LAYER_RAGDOLL_KEYFRAMED, collisiongroup);
	binfo.m_motionType = (hkpMotion::MotionType)shapedesc->mtype;

	// Set initial position and rotation
	/*hkVector4 hkvpos(vpos[0], vpos[1], vpos[2]);
	binfo.m_position = hkvpos;
	hkQuaternion hkqrot(qrot[0], qrot[1], qrot[2], qrot[3]);
	binfo.m_rotation = hkqrot;*/
/*	binfo.m_position = transform->getTranslation();
	binfo.m_rotation = transform->getRotation();*/
binfo.m_position.setZero();
binfo.m_rotation.setIdentity();

	if(shapedesc->mtype == MT_DYNAMIC)
	{
		//  Calculate the mass properties for the shape
		const hkReal mass = shapedesc->mass;
		hkpMassProperties massprops;
		hkpInertiaTensorComputer::computeShapeVolumeMassProperties(shape, mass, massprops);
		binfo.setMassProperties(massprops);
	}

	// Create the rigid body
	hkpRigidBody* rigidbody = new hkpRigidBody(binfo);
	rigidbody->setName(shapekey.ToCharString());
	parts.insert(std::pair<String, Part*>(shapekey, new Part(rigidbody, worldmatrix, 0)));
	shape->removeReference();

	// Add rigidbody to ragdoll
	ragdoll->addRigidBody(rigidbody);
	rigidbody->removeReference();

	return R_OK;
}

Result RagdollEntity::AttachShape(const HvkShapeDesc* shapedesc, const ConstraintDesc* constraintdesc, const String& newshapekey, const String& anchorshapekey, const Matrix* worldmatrix)
{
	// Create shape
	hkpShape* shape;
	Result rlt;
	CHKRESULT(Havok::CreateShape(shapedesc, &shape));
	shape->setUserData((hkUlong)this);

	// Get anchor body
	hkpRigidBody* anchorbody = parts.find(anchorshapekey)->second->rigidbody;

	// Create rigid body info
	hkpRigidBodyCinfo binfo;
	binfo.m_shape = shape;
	binfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(Havok::LAYER_RAGDOLL_KEYFRAMED, collisiongroup);
	binfo.m_motionType = (hkpMotion::MotionType)shapedesc->mtype;

	// Set initial position and rotation
//	binfo.setTransform(anchorbody->getTransform());
/*binfo.m_position = transform->getTranslation();
binfo.m_rotation = transform->getRotation();*/

binfo.m_position.setZero();
binfo.m_rotation.setIdentity();

	/*//  Calculate the mass properties for the shape
	const hkReal mass = shapedesc->mass;
	hkpMassProperties massprops;
	hkpInertiaTensorComputer::computeShapeVolumeMassProperties(shape, mass, massprops);
	binfo.setMassProperties(massprops);*/

	// Create the rigid body
	hkpRigidBody* rigidbody = new hkpRigidBody(binfo);
	rigidbody->setName(newshapekey.ToCharString());
	parts.insert(std::pair<String, Part*>(newshapekey, new Part(rigidbody, worldmatrix, parts.size())));
	shape->removeReference();

	// Create constraint
	hkpConstraintInstance* constraint;
	CHKRESULT(Havok::CreateConstraint(constraintdesc, anchorbody, rigidbody, &constraint));

	// Add constraint instance to ragdoll
	ragdoll->addConstraint(constraint);
	constraint->removeReference();

	// Add rigidbody to ragdoll
	ragdoll->addRigidBody(rigidbody);
	rigidbody->removeReference();

	return R_OK;
}

void RagdollEntity::AttachEntity(IRegularEntity* entity, const String& targetshapekey, const float* localtransform)
{
	hkaBoneAttachment* newattachment = new hkaBoneAttachment();

	hkpRigidBody* rigidbody = ((RegularEntity*)entity)->rigidbody;

	newattachment->m_attachment.set(rigidbody, &hkpRigidBodyClass);
	newattachment->m_boneIndex = parts.find(targetshapekey)->second->index;
	newattachment->m_boneFromAttachment.set4x4RowMajor(localtransform);

	rigidbody->enableDeactivation(false);
	rigidbody->setMotionType(hkpMotion::MOTION_KEYFRAMED);

	attachments.pushBack(newattachment);
}

void RagdollEntity::DetachEntity(const IRegularEntity* entity)
{
	for(int i = 0; i < attachments.getSize(); i++)
	{
		hkaBoneAttachment* attachment = attachments[i];
		hkpRigidBody* rigidbody = ((RegularEntity*)entity)->rigidbody;
		if(attachment->m_attachment.val() != rigidbody)
			continue;

		rigidbody->enableDeactivation(true);
		rigidbody->setMotionType(hkpMotion::MOTION_DYNAMIC);
		attachments.removeAt(i);
		break;
	}
}

Result RagdollEntity::AssembleRagdoll()
{
	// Disable constrains
hkpGroupFilterUtil::disableCollisionsBetweenConstraintBodies(ragdoll->getConstraints().begin(), ragdoll->getConstraints().getSize(), collisiongroup);
if(true)
	hkpInertiaTensorComputer::optimizeInertiasOfConstraintTree(ragdoll->getConstraints().begin(), ragdoll->getConstraints().getSize(), ragdoll->getRigidBodies()[0]);




	// Create the ragdoll and its skeleton
//hkaRagdollUtils::reorderAndAlignForRagdoll(ragdoll->getRigidBodies(), ragdoll->getConstraints(), true); //EDIT

/*// Create constraint chains and a mapper. This can be done right after the constraints are properly ordered.
{
	hkpPoweredChainMapper::Config config;
	config.m_createLimitConstraints = false;
	hkArray<hkpPoweredChainMapper::ChainEndpoints> pairs;

	hkpRigidBody* leftHand = physicsData->findRigidBodyByName("Ragdoll L Hand"); HK_ASSERT(0xab000000, leftHand);
	hkpRigidBody* leftFoot = physicsData->findRigidBodyByName("Ragdoll L Foot"); HK_ASSERT(0xab000001, leftFoot);
	hkpRigidBody* rightHand = physicsData->findRigidBodyByName("Ragdoll R Hand"); HK_ASSERT(0xab000002, rightHand);
	hkpRigidBody* rightFoot = physicsData->findRigidBodyByName("Ragdoll R Foot"); HK_ASSERT(0xab000003, rightFoot);
	hkpRigidBody* headBone = physicsData->findRigidBodyByName("Ragdoll Head"); HK_ASSERT(0xab000003, headBone);
	hkpRigidBody* pelvisBone = physicsData->findRigidBodyByName("Ragdoll Pelvis"); HK_ASSERT(0xab000003, pelvisBone);

	pairs.setSize(3);
	pairs[0].m_start = leftHand;
	pairs[0].m_end   = rightFoot;
	pairs[1].m_start = rightHand;
	pairs[1].m_end   = leftFoot;
	pairs[2].m_start = headBone;
	pairs[2].m_end   = pelvisBone;

	hkArray<hkpConstraintInstance*> unusedConstraints;

	m_mapper = hkpPoweredChainMapper::buildChainMapper(config, newConstraints, pairs, &unusedConstraints);
int foo;
}*/


	ragdollskeleton = hkaRagdollUtils::constructSkeletonForRagdoll(ragdoll->getRigidBodies(), ragdoll->getConstraints());

	ragdollinstance = new hkaRagdollInstance(ragdoll->getRigidBodies(), ragdoll->getConstraints(), ragdollskeleton);

//hkpInertiaTensorComputer::optimizeInertiasOfConstraintTree(ragdoll->getConstraints().begin(), ragdoll->getConstraints().getSize(), ragdollinstance->getRigidBodyOfBone(0));

for(int i = 0; i < ragdollinstance->getNumBones(); i++)
{
	ragdollinstance->getRigidBodyOfBone(i)->setAngularDamping(0.0f);
}

	// Add ragdoll to world
	ragdollinstance->addToWorld(parent->world, true);

// Create animated skeleton
animatedskeleton = new hkaAnimatedSkeleton(ragdollskeleton);
animatedskeleton->setReferencePoseWeightThreshold(0.5f);

/*// animation controller
{
	m_control = new hkaDefaultAnimationControl(m_binding);
	m_control->setPlaybackSpeed(1.0f);

	animatedskeleton->addAnimationControl(m_control);
	m_control->removeReference();
}*/

controller = new hkaRagdollRigidBodyController(ragdollinstance);
/*hkaKeyFrameHierarchyUtility::ControlData& controllerdata = controller->m_controlDataPalette[0];
controllerdata.m_positionMaxLinearVelocity = 1.0f;
controllerdata.isValid();*/

	// >>> Create currentpose (The pose used in PreUpdate() for combining animations, IK solvers, ...)
	int numbones = ragdollskeleton->m_bones.getSize();
	hkLocalArray<hkQsTransform> transforms(numbones);
	hkQsTransform identitytransform(hkQsTransform::IDENTITY);
	for(int i = 0; i < numbones; i++)
		transforms.pushBack(identitytransform);
	currentpose = new hkaPose(hkaPose::MODEL_SPACE, ragdollskeleton, transforms);








/*Result rlt;
LPPOSE pose;
CHKRESULT(CreatePose(&pose));

hkQsTransform transform;

transform = ((Pose*)pose)->getBoneLocalSpace(1);
transform.setRotation(hkQuaternion(hkVector4(0.0f, 0.0f, 1.0f), 60.0f * PI / 180.0f));
((Pose*)pose)->setBoneLocalSpace(1, transform);

transform = ((Pose*)pose)->getBoneLocalSpace(3);
transform.setRotation(hkQuaternion(hkVector4(0.0f, 0.0f, 1.0f), -60.0f * PI / 180.0f));
((Pose*)pose)->setBoneLocalSpace(3, transform);

transform = ((Pose*)pose)->getBoneLocalSpace(4);
transform.setRotation(hkQuaternion(hkVector4(0.0f, 1.0f, 0.0f), -80.0f * PI / 180.0f));
((Pose*)pose)->setBoneLocalSpace(4, transform);*/



/*ApplyPose(pose);*/



/*LPANIMATION ani;
CHKRESULT(CreateAnimation(pose, &ani));

ApplyPose(ani);



LPPOSE pose2;
CHKRESULT(CreatePose(&pose2));

transform = ((Pose*)pose)->getBoneLocalSpace(4);
transform.setRotation(hkQuaternion(hkVector4(0.0f, 0.0f, 1.0f), -1.0f));
((Pose*)pose2)->setBoneLocalSpace(4, transform);

ani->AddFrame(pose2);


ani->AddFrame(pose);*/


/*// We start with a pose in local space
transforms.setSizeUnchecked(ragdollinstance->getNumBones());
animatedskeleton->sampleAndCombineAnimations(transforms.begin(), NULL);*/

//addRagdoll(parent->world, ragdoll, 1.0f);

	return R_OK;
}

float* RagdollEntity::GetPivot(const String& shapekey)
{
	std::map<String, Part*, StringComparator>::iterator pair = parts.find(shapekey);

	if(pair != parts.end())
		return (float*)ragdoll->getConstraints()[pair->second->index - 1]->getUserData();

	return NULL;
}
float* RagdollEntity::GetPivot(int boneidx)
{
	if(boneidx >= 1 || boneidx < ragdollskeleton->m_bones.getSize())
		return (float*)ragdoll->getConstraints()[boneidx - 1]->getUserData();
	else
		return NULL;
}

Result RagdollEntity::CreatePose(IPose** pose)
{
	*pose = NULL;
	Result rlt;

	Pose* newpose;
	CHKALLOC(newpose = new Pose(ragdollskeleton, ragdoll->getConstraints()));

//	poses.push_back(newpose);
	*pose = newpose;

	return R_OK;
}

Result RagdollEntity::CreateAnimation(float duration, IAnimation** ani)
{
	*ani = NULL;
	Result rlt;

	Animation* newani;
	CHKALLOC(newani = new Animation(this, ragdollskeleton->m_bones.getSize(), duration));

	animations.push_back(newani);
	*ani = newani;

	return R_OK;
}

void RagdollEntity::LookAt(const float* vtargetpos, float gain)
{
	if(!lookatdesc)
		lookatdesc = new LookAtDesc();

	lookatdesc->targetpos.set(vtargetpos[0], vtargetpos[1], vtargetpos[2]);
	lookatdesc->gain = gain;
	lookatdesc->neckboneidx = 8;
	lookatdesc->headboneidx = 9;
}

void RagdollEntity::EnableFootIk()
{
/*	if(!footikdesc)
		delete footikdesc;
	footikdesc = new FootIkDesc();
hkaFootPlacementIkSolver::Setup setupData;

// General setup (left and right leg)
setupData.m_skeleton = ragdollskeleton;
setupData.m_kneeAxisLS.set(0,0,1); // Z
setupData.m_footEndLS.set(0.0f, 0.0f, 0.0f);
setupData.m_worldUpDirectionWS.set(0,0,1);
setupData.m_modelUpDirectionMS.set(0,0,1);
setupData.m_originalGroundHeightMS = -0.82f;
setupData.m_minAnkleHeightMS = -0.9f;
setupData.m_maxAnkleHeightMS = -0.1f;
setupData.m_cosineMaxKneeAngle = -0.99f;
setupData.m_cosineMinKneeAngle = 0.99f;
setupData.m_footPlantedAnkleHeightMS = -0.8f;
setupData.m_footRaisedAnkleHeightMS = -0.2f;
setupData.m_raycastDistanceUp = 1.0f;
setupData.m_raycastDistanceDown = 1.0f;

// Left leg setup
setupData.m_hipIndex = 5;
setupData.m_kneeIndex = 6;
setupData.m_ankleIndex = hkaSkeletonUtils::findBoneWithName(*ragdollskeleton, "HavokBipedRig L Foot" );
footikdesc->leftlegsolver = new hkaFootPlacementIkSolver(setupData);

// Right leg setup
setupData.m_hipIndex = 7;
setupData.m_kneeIndex = 8;
setupData.m_ankleIndex = hkaSkeletonUtils::findBoneWithName(*ragdollskeleton, "HavokBipedRig R Foot" );
footikdesc->rightlegsolver = new hkaFootPlacementIkSolver(setupData);*/
}

//-----------------------------------------------------------------------------
// Name: PreUpdate()
// Desc: Update forces
//-----------------------------------------------------------------------------
void RagdollEntity::PreUpdate()
{
	if(animatedskeleton->getNumAnimationControls()) // If any animations are enabled
	{
		// Advance active animations
		animatedskeleton->stepDeltaTime(parent->dt);

		// Sample animations to currentpose
		animatedskeleton->sampleAndCombineAnimations(currentpose->accessUnsyncedPoseLocalSpace().begin(), currentpose->getFloatSlotValues().begin());
	}

	// Apply look-at IK solver to currentpose
	if(lookatdesc)
	{
		hkaLookAtIkSolver::Setup setup;
		setup.m_fwdLS.set(0.0f, 0.0f, 1.0f);
		setup.m_eyePositionLS.set(0.0f, 0.1f, 0.05f);
		setup.m_limitAxisMS.setRotatedDir(currentpose->getBoneModelSpace(lookatdesc->neckboneidx).getRotation(), hkVector4(0.0f, 0.0f, 1.0f));
		setup.m_limitAngle = HK_REAL_PI;

		hkVector4 targetpos;
		targetpos.setTransformedInversePos(*transform, lookatdesc->targetpos);
		hkaLookAtIkSolver::solve(setup, targetpos, lookatdesc->gain, currentpose->accessBoneModelSpace(lookatdesc->headboneidx, hkaPose::PROPAGATE), HK_NULL);
	}

	// Force ragdoll into current pose
	// (This will slow down any mouse spiring action, because the pose target is in world space and it
	// gets computed before the mouse spring action is applied, therefore pushing to the old ragdoll position)
	controller->driveToPose(parent->dt, currentpose->getSyncedPoseLocalSpace().begin(), *transform, HK_NULL);
//HK_DISPLAY_LOCAL_SPACE_POSE(ragdollskeleton->m_bones.getSize(), ragdollskeleton->m_parentIndices.begin(), currentpose->getSyncedPoseModelSpace().begin(), *transform, 0xFFFFFFFF);
//parent->localvisualizer->displayLocalSpacePose(ragdollskeleton->m_bones.getSize(), ragdollskeleton->m_parentIndices.begin(), currentpose->getSyncedPoseModelSpace().begin(), *transform, 0xFFFFFFFF,0,0);
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Update vpos & vrot if the entity isn't static
//-----------------------------------------------------------------------------
void RagdollEntity::Update()
{
if(m_characterContext)
{

hkVector4 up;
up.setZero4();
up(1) = 1;

hkQuaternion orient;
//CharacterUtils::getUserInputForCharacter(m_env, deltaAngle, posX, posY);
orient.setAxisAngle(up, rot);

	hkpCharacterInput input;
	hkpCharacterOutput output;

	input.m_inputUD = dx;
	input.m_inputLR = dz;

	input.m_wantJump =  jump;
	input.m_atLadder = onladder && m_listener->m_atLadder;

	input.m_up = up;
	input.m_forward.set(0.0f, 0.0f, 1.0f);
	input.m_forward.setRotatedDir(orient, input.m_forward);

	input.m_stepInfo.m_deltaTime = parent->dt;
	input.m_stepInfo.m_invDeltaTime = 1.0f / parent->dt;
	input.m_characterGravity.set(0, -16, 0);
	input.m_velocity = m_characterProxy->getLinearVelocity();
	input.m_position = m_characterProxy->getPosition();

	hkVector4 down;	down.setNeg4(up);
	m_characterProxy->checkSupport(down, input.m_surfaceInfo);




if(input.m_atLadder &&(input.m_inputUD < 0.0f || input.m_surfaceInfo.m_supportedState != hkpSurfaceInfo::SUPPORTED))
{
	hkVector4 right, ladderUp;
	right.setCross(up, m_listener->m_ladderNorm);
	ladderUp.setCross(m_listener->m_ladderNorm, right);
	// Calculate the up vector for the ladder
	if(ladderUp.lengthSquared3() > HK_REAL_EPSILON)
		ladderUp.normalize3();

	// Reorient the forward vector so it points up along the ladder
	input.m_forward.addMul4(-m_listener->m_ladderNorm.dot3(input.m_forward), m_listener->m_ladderNorm);
	input.m_forward.add4(ladderUp);
	input.m_forward.normalize3();

	input.m_surfaceInfo.m_supportedState = hkpSurfaceInfo::UNSUPPORTED;
	input.m_surfaceInfo.m_surfaceNormal = m_listener->m_ladderNorm;
	input.m_surfaceInfo.m_surfaceVelocity = m_listener->m_ladderVelocity;
}





	m_characterContext->update(input, output);
//cle->PrintD3D(output.m_velocity.getComponent(2));

	// Feed output from state machine into character proxy
	m_characterProxy->setLinearVelocity(output.m_velocity);

	hkStepInfo si;
	si.m_deltaTime = parent->dt;
	si.m_invDeltaTime = 1.0f/parent->dt;
	m_characterProxy->integrate( si, parent->world->getGravity() );


	/*hkpCharacterStateType state = m_characterContext->getState();
	switch(state)
	{
	case HK_CHARACTER_ON_GROUND:
		cle->PrintD3D("On Ground");	break;
	case HK_CHARACTER_JUMPING:
		cle->PrintD3D("Jumping"); break;
	case HK_CHARACTER_IN_AIR:
		cle->PrintD3D("In Air"); break;
	case HK_CHARACTER_CLIMBING:
		cle->PrintD3D("Climbing"); break;
	default:
		cle->PrintD3D("Other");	break;
	}*/

	/*hkVector4 pos = m_characterProxy->getPosition();
	vpos[0] = pos.getComponent(0);
	vpos[1] = pos.getComponent(1);
	vpos[2] = pos.getComponent(2);

	vrot[0] = orient(0);
	vrot[1] = orient(1);
	vrot[2] = orient(2);
	vrot[3] = orient(3);*/

	//transform.setIdentity();
	transform->setTranslation(m_characterProxy->getPosition());
//cle->PrintD3D(m_characterProxy->getPosition().getComponent(2));
	//transform.setTranslation(hkVector4(0.0f, 4.3f, m_characterProxy->getPosition().getComponent(2)));
	transform->setRotation(orient);

	if(vpos)
	{
		vpos->x = m_characterProxy->getPosition()(0);
		vpos->y = m_characterProxy->getPosition()(1);
		vpos->z = m_characterProxy->getPosition()(2);

		qrot->x = orient(0);
		qrot->y = orient(1);
		qrot->z = orient(2);
		qrot->w = orient(3);
	}
}

	// Update attachments
	for(int i = 0; i < attachments.getSize(); i++)
	{
		hkaBoneAttachment* attachment = attachments[i];

		HK_ALIGN16(hkReal) _attachmentworldtransform[16];
		attachment->m_boneFromAttachment.get4x4RowMajor(_attachmentworldtransform);
		hkQsTransform attachmentworldtransform;
		attachmentworldtransform.set4x4ColumnMajor(_attachmentworldtransform);

		hkQsTransform poseworld(ragdollinstance->getRigidBodyArray()[attachment->m_boneIndex]->getPosition(), ragdollinstance->getRigidBodyArray()[attachment->m_boneIndex]->getRotation());

		hkQsTransform worldtransform;
		worldtransform.setMul(poseworld, attachmentworldtransform);

		((hkpRigidBody*)attachment->m_attachment.val())->setPositionAndRotation(worldtransform.getTranslation(), worldtransform.getRotation());
	}

	// Update transformation matrices for every body part outside ISHavok
	std::map<String, Part*, StringComparator>::iterator iter;
	LIST_FOREACH(parts, iter)
		if(iter->second->vtransform)
		{
			hkTransform w = iter->second->rigidbody->getTransform();
			if(iter->second->index >= 1 && ragdoll->getConstraints()[iter->second->index - 1]->getUserData())
			{
				hkTransform centerpivot;
				centerpivot.setIdentity();
				centerpivot.getTranslation().sub(((Vector3*)ragdoll->getConstraints()[iter->second->index - 1]->getUserData())[1]);
				hkTransform foo;
				foo.setMul(w, centerpivot);
				w = foo;
			}
			float* f = (float*)iter->second->vtransform;

			/*for(int c = 0; c < 3; c++)
			{
				for(int r = 0; r < 3; r++)
					f[c * 4 + r] = w(r, c);
				f[c * 4 + 3] = 0.0f;
			}
			Vector3 translation(w.getTranslation());
			f[3 * 4 + 0] = translation.x;
			f[3 * 4 + 1] = translation.y;
			f[3 * 4 + 2] = translation.z;
			f[3 * 4 + 3] = 1.0f;*/

			for(int r = 0; r < 4; r++)
				for(int c = 0; c < 4; c++)
					f[r + c * 4] = w(r, c);
			f[15] = 1.0f;
		}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void RagdollEntity::Release()
{
	parent->entities.remove(this);

	if(ragdoll)
		for(int i = 0; i < ragdoll->getConstraints().getSize(); i++)
			if(ragdoll->getConstraints()[i]->getUserData())
				delete (hkVector4*)ragdoll->getConstraints()[i]->getUserData();

	//REMOVE_ARRAY(currentanimationframe);
	REMOVE(controller);
	HAVOK_RELEASE(ragdollinstance);
	HAVOK_RELEASE(ragdollskeleton);
	attachments.clear();
	REMOVE(lookatdesc);
	REMOVE(footikdesc);

	std::map<String, Part*, StringComparator>::iterator iter;
	LIST_FOREACH(parts, iter)
		delete iter->second;

	delete this;
}