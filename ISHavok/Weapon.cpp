#include "Havok.h"

Result Weapon::Init()
{
	ballgun = new hkpBallGun();
	ballgun->initGun(parent->world);
	return R_OK;
}

void Weapon::Fire(Vector3* vpos, Quaternion* qrot)
{
	this->vpos = vpos;
	this->qrot = qrot;

	hkQuaternion shootinzdir; shootinzdir.setAxisAngle(hkVector4(0.0f, 1.0f, 0.0f), -PI / 2.0f);
	hkQuaternion _qrot = *qrot;
	_qrot.mul(shootinzdir);

	hkTransform viewtransform(_qrot, *vpos);
	ballgun->fireGun(parent->world, viewtransform);
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Update vpos & vrot
//-----------------------------------------------------------------------------
void Weapon::Update()
{
	if(!ballgun)
		return;

	hkpRigidBody* rigidbody = NULL;
	hkTransform transform;
	ballgun->stepGun(parent->dt, parent->world, rigidbody, transform, false, false);

	/*hkVector4 pos = rigidbody->getPosition();
	vpos[0] = pos.getComponent(0);
	vpos[1] = pos.getComponent(1);
	vpos[2] = pos.getComponent(2);

	hkQuaternion rot = rigidbody->getRotation();
	qrot->x = rot(0);
	qrot->y = rot(1);
	qrot->z = rot(2);
	qrot->w = rot(3);*/
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Weapon::Release()
{
	parent->weapons.remove(this);

	if(ballgun)
		ballgun->removeReference();

	delete this;
}