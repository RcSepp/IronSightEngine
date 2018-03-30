#include "Havok.h"

//#include <Physics/ConstraintSolver/Simplex/hkpSimplexSolver.h>


Result PlayerRBEntity::Create(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot)
{
	if(shapedesc->mtype == MT_FIXED)
		LOG("Player entities can't be static (property ignored)");

	this->vpos = vpos;
	this->qrot = qrot;

	// Create shape
	hkpShape* shape;
	Result rlt;
	CHKRESULT(Havok::CreateShape(shapedesc, &shape));
	shape->setUserData((hkUlong)this);

//hkpCharacterRigidBodyListener* listener = new hkpCharacterRigidBodyListener();

// Create character rigid body info
hkpCharacterRigidBodyCinfo info;
info.m_shape = shape;
info.m_mass = shapedesc->mass;
//if(layer != IHavok::LAYER_DEFAULT)
//	info.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(layer);

info.m_maxForce = 1000.0f * info.m_mass; //EDIT
hkVector4 up(0.0f, 1.0f, 0.0f);
info.m_up = up;

// Set initial position and rotation
hkVector4 hkvpos = *vpos;
info.m_position = hkvpos;
hkQuaternion hkqrot = *qrot;
info.m_rotation = hkqrot;
		
// Create the rigid body
characterrigidbody = new hkpCharacterRigidBody(info);
//characterrigidbody->setListener( listener );
shape->removeReference();

// Add rigidbody to the world
parent->world->addEntity(characterrigidbody->getRigidBody());

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
	manager->registerState(state, HK_CHARACTER_JUMPING);
	state->removeReference();

	state = new hkpCharacterStateClimbing();
	manager->registerState(state, HK_CHARACTER_CLIMBING);
	state->removeReference();

	characterctx = new hkpCharacterContext(manager, HK_CHARACTER_ON_GROUND);
	manager->removeReference();

	// Set character type
	characterctx->setCharacterType(hkpCharacterContext::HK_CHARACTER_RIGIDBODY);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Update vpos & vrot if the entity isn't static
//-----------------------------------------------------------------------------
void PlayerRBEntity::Update(float dx, float dy, float rot, bool jump)
{
	hkVector4 up;
	up.setZero4();
	up(1) = 1;
	hkQuaternion orient;
	orient.setAxisAngle(up, rot);

	hkpCharacterInput input;
	hkpCharacterOutput output;

	input.m_inputUD = dx;
	input.m_inputLR = dy;

	input.m_wantJump =  jump;
	input.m_atLadder = false;

	input.m_up = up;
	input.m_forward.set(0.0f, 0.0f, 1.0f);
	input.m_forward.setRotatedDir(orient, input.m_forward);

	hkStepInfo& stepInfo = input.m_stepInfo;
	stepInfo.m_deltaTime = parent->dt;
	stepInfo.m_invDeltaTime = 1.0f / parent->dt;
	stepInfo.m_endTime = parent->t;

	input.m_characterGravity.set(0, -16, 0);
	input.m_velocity = characterrigidbody->getRigidBody()->getLinearVelocity();
	input.m_position = characterrigidbody->getRigidBody()->getPosition();

	hkVector4 down;	down.setNeg4(up);
	characterrigidbody->checkSupport(stepInfo, input.m_surfaceInfo);

	// Apply character state machine
	characterctx->update(input, output);

	// Feed output from state machine into character rigid body
	characterrigidbody->setLinearVelocity(output.m_velocity, parent->dt);
	//characterrigidbody->getRigidBody()->setRotation(orient);

	hkVector4 pos = characterrigidbody/*m_characterProxy*/->getPosition();
	vpos->x = pos.getComponent(0);
	vpos->y = pos.getComponent(1);
	vpos->z = pos.getComponent(2);

	qrot->x = orient(0);
	qrot->y = orient(1);
	qrot->z = orient(2);
	qrot->w = orient(3);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void PlayerRBEntity::Release()
{
	if(characterrigidbody->getRigidBody()->getWorld())
		parent->world->removeEntity(characterrigidbody->getRigidBody());
	characterrigidbody->removeReference();
	characterctx->removeReference();
//	parent->entities.remove(this);

	delete this;
}