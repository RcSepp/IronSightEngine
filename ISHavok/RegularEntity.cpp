#include "Havok.h"


Result RegularEntity::Create(const HvkShapeDesc* shapedesc, IHavok::Layer layer, Vector3* vpos, Quaternion* qrot)
{
	if(shapedesc->mtype == MT_FIXED)
	{
		this->vpos = NULL;
		this->qrot = NULL;
	}
	else
	{
		this->vpos = vpos;
		this->qrot = qrot;
	}

	// Create shape
	hkpShape* shape;
	Result rlt;
	CHKRESULT(Havok::CreateShape(shapedesc, &shape));
	shape->setUserData((hkUlong)this);

	// Create rigid body info
	hkpRigidBodyCinfo binfo;
	binfo.m_shape = shape;
	binfo.m_motionType = (hkpMotion::MotionType)shapedesc->mtype;
	binfo.m_qualityType = (hkpCollidableQualityType)shapedesc->qual;
	if(layer != IHavok::LAYER_DEFAULT)
		binfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(layer);

	// Set initial position and rotation
	hkVector4 hkvpos = *vpos;
	binfo.m_position = hkvpos;
	hkQuaternion hkqrot = *qrot;
	binfo.m_rotation = hkqrot;

	if(shapedesc->mtype == MT_DYNAMIC)
	{
		//  Calculate the mass properties for the shape
		const hkReal mass = shapedesc->mass;
		hkpMassProperties massprops;
		hkpInertiaTensorComputer::computeShapeVolumeMassProperties(shape, mass, massprops);
		binfo.setMassProperties(massprops);
	}

	// Create the rigid body
	rigidbody = new hkpRigidBody(binfo);
	shape->removeReference();

	// Add rigidbody to the world
	parent->world->addEntity(rigidbody);
	rigidbody->removeReference();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Update vpos & vrot if the entity isn't static
//-----------------------------------------------------------------------------
void RegularEntity::Update()
{
	if(vpos)
	{
		hkVector4 pos = rigidbody->getPosition();
		vpos->x = pos.getComponent(0);
		vpos->y = pos.getComponent(1);
		vpos->z = pos.getComponent(2);

		hkQuaternion rot = rigidbody->getRotation();
		qrot->x = rot(0);
		qrot->y = rot(1);
		qrot->z = rot(2);
		qrot->w = rot(3);
	}
}

//-----------------------------------------------------------------------------
// Name: applyForce()
// Desc: ?
//-----------------------------------------------------------------------------
void RegularEntity::applyForce(const Vector3* force)
{
	rigidbody->applyForce(parent->dt, *force);
}

//-----------------------------------------------------------------------------
// Name: applyAngularImpulse()
// Desc: ?
//-----------------------------------------------------------------------------
void RegularEntity::applyAngularImpulse(const Vector3* impulse)
{
	rigidbody->applyAngularImpulse(*impulse);
}

//-----------------------------------------------------------------------------
// Name: applyLinearImpulse()
// Desc: ?
//-----------------------------------------------------------------------------
void RegularEntity::applyLinearImpulse(const Vector3* impulse)
{
	rigidbody->applyLinearImpulse(*impulse);
}

//-----------------------------------------------------------------------------
// Name: applyPointImpulse()
// Desc: ?
//-----------------------------------------------------------------------------
void RegularEntity::applyPointImpulse(const Vector3* impulse, const Vector3* pos)
{
	rigidbody->applyPointImpulse(*impulse, *pos);
}

//-----------------------------------------------------------------------------
// Name: setMotionType()
// Desc: Change motion type. To change a fixed motion type to a non-fixed one, the entity has to be initially created non-fixed!
//-----------------------------------------------------------------------------
void RegularEntity::setMotionType(MotionType mtype)
{
	rigidbody->setMotionType((hkpMotion::MotionType)mtype);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void RegularEntity::Release()
{
	if(rigidbody->getWorld())
		parent->world->removeEntity(rigidbody);
	parent->entities.remove(this);

	delete this;
}