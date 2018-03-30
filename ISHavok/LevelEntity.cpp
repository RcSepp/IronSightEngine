#include "Havok.h"
#include <Physics/Internal/Collide/StaticCompound/hkpStaticCompoundShape.h>


Result LevelEntity::Create(const HvkStaticCompoundShapeDesc* shapedesc, const Vector3& vpos, const Quaternion& qrot)
{
	// Create shape
	Result rlt;
	CHKRESULT(Havok::CreateShape(shapedesc, (hkpShape**)&compoundshape));
	compoundshape->setUserData((hkUlong)this);

	// Create rigid body info
	hkpRigidBodyCinfo binfo;
	binfo.m_shape = compoundshape;
	binfo.m_motionType = hkpMotion::MOTION_FIXED;
	binfo.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
	binfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(IHavok::LAYER_LANDSCAPE);

	// Set initial position and rotation
	binfo.m_position = vpos;
	binfo.m_rotation = qrot;

	// Create the rigid body
	rigidbody = new hkpRigidBody(binfo);
	compoundshape->removeReference();

	// Add rigidbody to the world
	parent->world->addEntity(rigidbody);
	rigidbody->removeReference();

	return R_OK;
}

void LevelEntity::EnableAll()
{
	compoundshape->enableAllInstancesAndShapeKeys();
}
void LevelEntity::DisableAll()
{
	for(hkpShapeKey key = compoundshape->getFirstKey(); key != HK_INVALID_SHAPE_KEY; key = compoundshape->getNextKey(key))
		compoundshape->setShapeKeyEnabled(key, false);
	for(int i = 0; i < compoundshape->getInstances().getSize(); i++)
		compoundshape->setInstanceEnabled(i, false);
}

void LevelEntity::EnableShape(UINT32 shapekey)
{
	compoundshape->setShapeKeyEnabled(shapekey, true);
}
void LevelEntity::DisableShape(UINT32 shapekey)
{
	compoundshape->setShapeKeyEnabled(shapekey, false);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void LevelEntity::Release()
{
	if(rigidbody->getWorld())
		parent->world->removeEntity(rigidbody);
	parent->entities.remove(this);

	delete this;
}