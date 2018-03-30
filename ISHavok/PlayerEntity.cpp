#include "Havok.h"

#include <Physics/ConstraintSolver/Simplex/hkpSimplexSolver.h>


Result PlayerEntity::Create(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot)
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

	// Construct a Shape Phantom
	hkpShapePhantom* m_phantom;
	m_phantom = new hkpSimpleShapePhantom(shape, hkTransform::getIdentity(), hkpGroupFilter::calcFilterInfo(Havok::LAYER_PROXY));
		
	// Add the phantom to the world
	parent->world->addPhantom(m_phantom);
	m_phantom->removeReference();

	// Construct a character proxy
	hkpCharacterProxyCinfo cpci;
	cpci.m_position = *vpos;
	cpci.m_staticFriction = 0.0f;
	cpci.m_dynamicFriction = 1.0f;
	cpci.m_up.setNeg4(parent->world->getGravity());
	cpci.m_up.normalize3();	
	cpci.m_userPlanes = 4;
	cpci.m_keepDistance = 0.0f;
	cpci.m_maxSlope = HK_REAL_PI / 3.f;

	cpci.m_shapePhantom = m_phantom;
	m_characterProxy = new hkpCharacterProxy(cpci);

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

	m_characterContext = new hkpCharacterContext(manager, HK_CHARACTER_ON_GROUND);
	manager->removeReference();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Update vpos & vrot if the entity isn't static
//-----------------------------------------------------------------------------
void PlayerEntity::Update(float dx, float dy, float rot, bool jump)
{
if(m_characterContext)
{

hkVector4 up;
int m_upAxisIndex = 1;
up.setZero4();
up(m_upAxisIndex) = 1;

hkQuaternion orient;
//CharacterUtils::getUserInputForCharacter(m_env, deltaAngle, posX, posY);
orient.setAxisAngle(up, rot);

	hkpCharacterInput input;
	hkpCharacterOutput output;

	input.m_inputLR = dx;
	input.m_inputUD = dy;

	input.m_wantJump =  jump;
	input.m_atLadder = false;//m_listener->m_atLadder;

	input.m_up = up;
	input.m_forward.set(1,0,0);
	input.m_forward.setRotatedDir(orient, input.m_forward);

	input.m_stepInfo.m_deltaTime = parent->dt;
	input.m_stepInfo.m_invDeltaTime = 1.0f / parent->dt;
	input.m_characterGravity.set(0, -16, 0);
	input.m_velocity = m_characterProxy->getLinearVelocity();
	input.m_position = m_characterProxy->getPosition();

	hkVector4 down;	down.setNeg4(up);
	m_characterProxy->checkSupport(down, input.m_surfaceInfo);

	m_characterContext->update(input, output);

	// Feed output from state machine into character proxy
	m_characterProxy->setLinearVelocity(output.m_velocity);

	hkStepInfo si;
	si.m_deltaTime = parent->dt;
	si.m_invDeltaTime = 1.0f/parent->dt;
	m_characterProxy->integrate( si, parent->world->getGravity() );


/*	hkpCharacterStateType state = m_characterContext->getState();
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

	hkVector4 pos = m_characterProxy->getPosition();
	vpos->x = pos.getComponent(0);
	vpos->y = pos.getComponent(1);
	vpos->z = pos.getComponent(2);

	qrot->x = orient(0);
	qrot->y = orient(1);
	qrot->z = orient(2);
	qrot->w = orient(3);
}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void PlayerEntity::Release()
{
//	parent->entities.remove(this);

	delete this;
}