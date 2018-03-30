#include "Havok.h"
#include "BoxedLevelShape.h"

Result BoxedLevelEntity::Create(const UINT (&chunksize)[3])
{
	// Create shape
	Result rlt;
	hkVector4& worldsize_max = parent->world->m_broadPhaseExtents[1];
	shape = new BoxedLevelShape(chunksize, Vector3((hkReal)worldsize_max.getComponent(0) - 0.5f, (hkReal)worldsize_max.getComponent(1) - 0.5f, (hkReal)worldsize_max.getComponent(2) - 0.5f));
	shape->setUserData((hkUlong)this);

	// Create rigid body info
	hkpRigidBodyCinfo binfo;
	binfo.m_shape = shape;
	binfo.m_motionType = hkpMotion::MOTION_FIXED;
	binfo.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
	binfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(IHavok::LAYER_LANDSCAPE);

	// Set position and rotation to zero
	binfo.m_position.setZero();
	binfo.m_rotation.setIdentity();

	// Create the rigid body
	rigidbody = new hkpRigidBody(binfo);
	shape->removeReference();

	// Add rigidbody to the world
	parent->world->addEntity(rigidbody);
	rigidbody->removeReference();

	return R_OK;
}

void BoxedLevelEntity::SetBoxTypes(BoxedLevelBoxType* boxtypes, UINT32 numtypes)
{
	shape->SetBoxTypes(boxtypes, numtypes);
}
Result BoxedLevelEntity::CreateBoxShapes(HvkShapeDesc** boxshapes, UINT32 numboxshapes)
{
	return shape->CreateBoxShapes(boxshapes, numboxshapes);
}

void BoxedLevelEntity::AssignChunk(BoxedLevelChunk* chunk)
{
	shape->AssignChunk(chunk);
}
void BoxedLevelEntity::UnassignChunk(BoxedLevelChunk* chunk)
{
	shape->UnassignChunk(chunk);
}

BoxedLevelChunk* BoxedLevelEntity::ChunkFromShapeKey(UINT32 chunkshapekey)
{
	return shape->ChunkFromShapeKey(chunkshapekey);
}
void BoxedLevelEntity::BoxPosFromShapeKey(UINT32 boxshapekey, UINT32 (*boxpos)[3])
{
	shape->BoxPosFromShapeKey(boxshapekey, boxpos);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void BoxedLevelEntity::Release()
{
	if(rigidbody->getWorld())
		parent->world->removeEntity(rigidbody);
	parent->entities.remove(this);

	delete this;
}