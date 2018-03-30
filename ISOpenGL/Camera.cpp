#include "OpenGL.h"
/*#include <ISDirectInput.h>
#include <ISHavok.h>


#define FREECHASECAM_SPEED		3.0f // [m / s]*/


//-----------------------------------------------------------------------------
// Name: Camera()
// Desc: Constructor
//-----------------------------------------------------------------------------
Camera::Camera(float fov, float clipnear, float clipfar)
{
	ptype = PT_PERSPECTIVE;
	viewmode = VM_DEFAULT;
	aspectratio = 1.0f;
	this->fov = fov ? fov : DEFFOV;
	this->clipnear = clipnear ? clipnear : DEFCLIPNEAR;
	this->clipfar = clipfar ? clipfar : DEFCLIPFAR;
	playermodel = NULL;
	playerentity = NULL;

	orientmode = OM_EULER;
	MatrixIdentity(&transform);
	pos = rot = Vector3(0.0f, 0.0f, 0.0f);
	QuaternionIdentity(&qrot);

	MatrixIdentity(&viewmatrix);
	RebuildProjectionMatrix();
	viewprojmatrix = viewmatrix * projmatrix;
}

void Camera::SetTransform(const Matrix* transform)
{
	viewmode = VM_DEFAULT;
	orientmode = OM_MATRIX;
	Vec3Set(&this->pos, transform->_41, transform->_42, transform->_43);
	MatrixInverse(&this->transform, transform);
}

//-----------------------------------------------------------------------------
// Name: OnOutputWindowResize()
// Desc: Should be called from OutputWindow on SetCamera() and Resize(). Updates projection matrix to the new output window size //EDIT: Resize()...
//-----------------------------------------------------------------------------
void Camera::OnOutputWindowResize(Size<UINT> backbuffersize)
{
	this->backbuffersize.width = (float)backbuffersize.width;
	this->backbuffersize.height = (float)backbuffersize.height;
	this->aspectratio = (float)backbuffersize.width / (float)backbuffersize.height;

	RebuildProjectionMatrix();
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Control camera with mouse and keyboard
//-----------------------------------------------------------------------------
void Camera::Update()
{
	if(viewmode == VM_DEFAULT) {}
	/*else if(dip)
		switch(viewmode)
		{
		case VM_FLY:
			{
orientmode = OM_EULER; //DELETE
				if(dip->mouse.rgbButtons[2])
				{
					rot.y -= dip->mouse.lX * 0.01f;
					if(rot.y > 2.0f * PI)
						rot.y -= 2.0f * PI;
					if(rot.y < -2.0f * PI)
						rot.y += 2.0f * PI;

					rot.x = min(PI / 2.0f, max(-PI / 2.0f, rot.x - dip->mouse.lY * 0.01f));
				}
				float keyz = (dip->keys[DIK_W] != 0)	  * FREECHASECAM_SPEED * eng->time.dt - (dip->keys[DIK_S] != 0)		   * FREECHASECAM_SPEED * eng->time.dt;
				float keyx = (dip->keys[DIK_D] != 0)	  * FREECHASECAM_SPEED * eng->time.dt - (dip->keys[DIK_A] != 0)		   * FREECHASECAM_SPEED * eng->time.dt;
				float keyy = (dip->keys[DIK_LSHIFT] != 0) * FREECHASECAM_SPEED * eng->time.dt - (dip->keys[DIK_LCONTROL] != 0) * FREECHASECAM_SPEED * eng->time.dt;
				pos.z += keyx * sin(rot.y) + keyz * cos(rot.x) * cos(rot.y) - keyy * sin(rot.x) * cos(rot.y);
				pos.x += keyx * cos(rot.y) - keyz * cos(rot.x) * sin(rot.y) + keyy * sin(rot.x) * sin(rot.y);
				pos.y += keyy * cos(rot.x) + keyz * sin(rot.x);
			}
			break;

		case VM_FIRSTPERSON:
			if(playerentity)
			{
				static float rot = PI; //EDIT: Remove static
				rot += dip->mouse.lX * 0.01f;

				float fz = 0.5f * ((dip->keys[DIK_W] != 0) - (dip->keys[DIK_S] != 0));
				float fx = 0.5f * ((dip->keys[DIK_A] != 0) - (dip->keys[DIK_D] != 0));
playerentity->dx = -fz;
playerentity->dz = -fx;
playerentity->rot = rot;
playerentity->jump = (dip->keys[DIK_LSHIFT] > 0);

				//playerentity->Update(-fz, fx, rot, (dip->keys[DIK_LSHIFT] > 0));

				QuaternionInverse(&qrot, &playermodel->qrot);
				Vector3 axis;
				float angle;
				QuaternionToAxisAngle(&qrot, &axis, &angle);
				angle = axis.y >= 0.0f ? angle : -angle;
				pos = playermodel->pos + Vector3(-0.2f * sin(angle), 0.5f, 0.2f * cos(angle));

				static float rotY = 0.0f; //EDIT: Remove static
				rotY = max(-PI / 3.0f, min(PI / 3.0f, rotY - dip->mouse.lY * 0.01f));
				Quaternion qupdown;
				QuaternionRotationAxis(&qupdown, &Vector3(1.0f, 0.0f, 0.0f), rotY);
				qrot *= qupdown; //EDIT11
			}
			else
			{
				//TODO: First person controls without collision detection
			}
			break;

		case VM_THIRDPERSON:
			if(playerentity)
			{
				static float rot = PI; //EDIT: Remove static
				rot += dip->mouse.lX * 0.01f;

				float fz = 0.5f * ((dip->keys[DIK_W] != 0) - (dip->keys[DIK_S] != 0));
				float fx = 0.5f * ((dip->keys[DIK_A] != 0) - (dip->keys[DIK_D] != 0));
playerentity->dx = -fz;
playerentity->dz = -fx;
playerentity->rot = rot;
playerentity->jump = (dip->keys[DIK_LSHIFT] > 0);
				//playerentity->Update(-fz, fx, rot, (dip->keys[DIK_LSHIFT] > 0));

				QuaternionInverse(&qrot, &playermodel->qrot);
				Vector3 axis;
				float angle;
				QuaternionToAxisAngle(&qrot, &axis, &angle);
				angle = axis.y >= 0.0f ? angle : -angle;
				pos = playermodel->pos + Vector3(2.0f * sin(angle), 0.0f, -2.0f * cos(angle));

#define LOCK_THIRDPERSON_Y_ROT
#ifndef LOCK_THIRDPERSON_Y_ROT
				static float rotY = 0.0f; //EDIT: Remove static
				rotY = max(-PI / 2.2f, min(PI / 2.2f, rotY - dip->mouse.lY * 0.01f));
				Quaternion qupdown;
				::QuaternionRotationAxis(&qupdown, &Vector3(1.0f, 0.0f, 0.0f), rotY);
				qrot *= qupdown;

				pos = playermodel->pos + Vector3(2.0f * sin(angle) * cos(rotY), 0.5f - 1.0f * sin(rotY), -2.0f * cos(angle) * cos(rotY));
#endif
			}
			else
			{
				//TODO: Third person controls without collision detection
			}
		}*/
	else
		LOG("viewmode other than VM_DEFAULT, but DirectInput is not used");
}

//-----------------------------------------------------------------------------
// Name: UpdateMatrices()
// Desc: Update view matrix, projection matrix and view frustum (should only be called by OutputWindow)
//-----------------------------------------------------------------------------
void Camera::UpdateMatrices()
{
	if(orientmode != OM_MATRIX)
	{
		static Vector3 oldpos = Vector3(1e20f, 1e20f, 1e20f), oldrot = Vector3(1e20f, 1e20f, 1e20f); //EDIT: Static variables wouldn't work for multiple cameras
		static Quaternion oldqrot = Quaternion(1e20f, 1e20f, 1e20f, 1e20f);
		if(pos == oldpos && ((orientmode == OM_EULER && rot == oldrot) || (orientmode == OM_QUAT && qrot == oldqrot)))
			return;
		oldpos = pos;
		oldrot = rot;

		Matrix mRot, mTrans;
		if(orientmode == OM_EULER)
		{
			Matrix mRx, mRy, mRz;
			MatrixRotationY(&mRy, rot.y);
			MatrixRotationX(&mRx, rot.x);
			MatrixRotationZ(&mRz, rot.z);
			mRot = mRy * mRx * mRz;
		}
		else // orientmode == OM_QUAT
			MatrixRotationQuaternion(&mRot, &qrot);
		MatrixTranslation(&mTrans, -pos.x, -pos.y, -pos.z);
		transform = transform = mTrans * mRot;
	}

	viewprojmatrix = transform * projmatrix;

	// Only do calculations if needed
	if(transform == viewmatrix)
		return;
	viewmatrix = transform;

	// Rebuild view frustum

	// pleft plane
	viewfrustum.pleft.a = viewprojmatrix._14 + viewprojmatrix._11;
	viewfrustum.pleft.b = viewprojmatrix._24 + viewprojmatrix._21;
	viewfrustum.pleft.c = viewprojmatrix._34 + viewprojmatrix._31;
	viewfrustum.pleft.d = viewprojmatrix._44 + viewprojmatrix._41;
	PlaneNormalize(&viewfrustum.pleft, &viewfrustum.pleft);

	// Right plane
	viewfrustum.pright.a = viewprojmatrix._14 - viewprojmatrix._11;
	viewfrustum.pright.b = viewprojmatrix._24 - viewprojmatrix._21;
	viewfrustum.pright.c = viewprojmatrix._34 - viewprojmatrix._31;
	viewfrustum.pright.d = viewprojmatrix._44 - viewprojmatrix._41;
	PlaneNormalize(&viewfrustum.pright, &viewfrustum.pright);

	// Top plane
	viewfrustum.ptop.a = viewprojmatrix._14 - viewprojmatrix._12;
	viewfrustum.ptop.b = viewprojmatrix._24 - viewprojmatrix._22;
	viewfrustum.ptop.c = viewprojmatrix._34 - viewprojmatrix._32;
	viewfrustum.ptop.d = viewprojmatrix._44 - viewprojmatrix._42;
	PlaneNormalize(&viewfrustum.ptop, &viewfrustum.ptop);

	// Bottom plane
	viewfrustum.pbottom.a = viewprojmatrix._14 + viewprojmatrix._12;
	viewfrustum.pbottom.b = viewprojmatrix._24 + viewprojmatrix._22;
	viewfrustum.pbottom.c = viewprojmatrix._34 + viewprojmatrix._32;
	viewfrustum.pbottom.d = viewprojmatrix._44 + viewprojmatrix._42;
	PlaneNormalize(&viewfrustum.pbottom, &viewfrustum.pbottom);

	// Near plane
	viewfrustum.pnear.a = viewprojmatrix._13;
	viewfrustum.pnear.b = viewprojmatrix._23;
	viewfrustum.pnear.c = viewprojmatrix._33;
	viewfrustum.pnear.d = viewprojmatrix._43;
	PlaneNormalize(&viewfrustum.pnear, &viewfrustum.pnear);

	// Far plane
	viewfrustum.pfar.a = viewprojmatrix._14 - viewprojmatrix._13;
	viewfrustum.pfar.b = viewprojmatrix._24 - viewprojmatrix._23;
	viewfrustum.pfar.c = viewprojmatrix._34 - viewprojmatrix._33;
	viewfrustum.pfar.d = viewprojmatrix._44 - viewprojmatrix._43;
	PlaneNormalize(&viewfrustum.pfar, &viewfrustum.pfar);
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Render player model
//-----------------------------------------------------------------------------
void Camera::Render(RenderType rendertype)
{
	if(playermodel)
		playermodel->Render(rendertype);
}

//-----------------------------------------------------------------------------
// Name: DoFrustumCulling()
// Desc: Return whether a bounding ellipsoid intersects the viewing frustrum
//-----------------------------------------------------------------------------
bool Camera::DoFrustumCulling(const Matrix* worldmatrix, const Matrix* scalingmatrix, const Matrix* rotationmatrix,
							  const Vector3* center, const Vector3* radius)
{
	Matrix mScaleBounds, mScaleRot;
	Vector3 vBoundCenterWorld, vPlaneNormal, vPlaneNormalWorld;

	Vec3TransformNormal(&vBoundCenterWorld, center, worldmatrix);
	MatrixScaling(&mScaleBounds, radius->x, radius->y, radius->z);
	mScaleRot = *scalingmatrix * mScaleBounds * *rotationmatrix;

	Plane* vfrustum = (Plane*)&viewfrustum.pleft;
	for(byte i = 0; i < 6; i++)
	{
		Vec3Set(&vPlaneNormal, vfrustum[i].a, vfrustum[i].b, vfrustum[i].c);
		Vec3TransformNormal(&vPlaneNormalWorld, &vPlaneNormal, &mScaleRot);

		if(PlaneDotCoord(&vfrustum[i], &vBoundCenterWorld) + Vec3Length(&vPlaneNormalWorld) < 0)
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: RebuildProjectionMatrix()
// Desc: Create projection matrix using D3DX methodes
//-----------------------------------------------------------------------------
void Camera::RebuildProjectionMatrix()
{
	if(ptype == PT_PERSPECTIVE)
		MatrixPerspectiveFovLH(&projmatrix, fov, aspectratio, clipnear, clipfar);
	else
		MatrixOrthographicLH(&projmatrix, otvwidth, otvheight, clipnear, clipfar);
}

//-----------------------------------------------------------------------------
// Name: SetClipNear()
// Desc: Assign distance of the near clipping plane
//-----------------------------------------------------------------------------
void Camera::SetClipNear(float clipnear)
{
	this->clipnear = clipnear;

	RebuildProjectionMatrix();
}

//-----------------------------------------------------------------------------
// Name: SetClipFar()
// Desc: Assign distance of the far clipping plane
//-----------------------------------------------------------------------------
void Camera::SetClipFar(float clipfar)
{
	this->clipfar = clipfar;

	RebuildProjectionMatrix();
}

//-----------------------------------------------------------------------------
// Name: SetPlayerModel()
// Desc: ?
//-----------------------------------------------------------------------------
void Camera::SetPlayerModel(LPOBJECT obj, LPRAGDOLLENTITY ragdoll)
{
	playermodel = (Object*)obj;

	if(ragdoll)
	{
		this->orientmode = OM_QUAT;
		playerentity = ragdoll;
	}
}

//-----------------------------------------------------------------------------
// Name: SetPerspectiveProjection()
// Desc: Establish a perspective view by setting the projection matrix accordingly
//-----------------------------------------------------------------------------
void Camera::SetPerspectiveProjection(float fov)
{
	this->fov = fov;
	ptype = PT_PERSPECTIVE;

	RebuildProjectionMatrix();
}

//-----------------------------------------------------------------------------
// Name: SetOrthogonalProjection()
// Desc: Establish an orthogonal view by setting the projection matrix accordingly
//-----------------------------------------------------------------------------
void Camera::SetOrthogonalProjection(float width, float height)
{
	this->otvwidth = width;
	this->otvheight = height;
	ptype = PT_ORTHOGONAL;

	RebuildProjectionMatrix();
}

//-----------------------------------------------------------------------------
// Name: ComputeViewDir()
// Desc: Return a normalized vector pointing in view direction
//-----------------------------------------------------------------------------
Vector3* Camera::ComputeViewDir(Vector3* vdir) const
{
	// Get rotation part of inverse view matrix
	Matrix mInvRot;
	if(orientmode == OM_EULER)
	{
		Matrix mRx, mRy, mRz;
		MatrixRotationY(&mRy, -rot.y);
		MatrixRotationX(&mRx, -rot.x);
		MatrixRotationZ(&mRz, -rot.z);
		mInvRot = mRz * mRx * mRy;
	}
	else
		MatrixInverse(&mInvRot, &viewmatrix);

	// Get view direction
	*vdir = Vector3(0.0f, 0.0f, 1.0f);
	Vec3TransformNormal(vdir, vdir, &mInvRot);

	return vdir;
}

//-----------------------------------------------------------------------------
// Name: ComputePixelDir()
// Desc: Return a normalized vector pointing in the direction of a given pixel
//-----------------------------------------------------------------------------
Vector3* Camera::ComputePixelDir(const Point<int>& pixelpos, Vector3* vdir) const
{
	// Get rotation part of inverse view matrix
	Matrix mInvRot;
	if(orientmode == OM_EULER)
	{
		Matrix mRx, mRy, mRz;
		MatrixRotationY(&mRy, -rot.y);
		MatrixRotationX(&mRx, -rot.x);
		MatrixRotationZ(&mRz, -rot.z);
		mInvRot = mRz * mRx * mRy;
	}
	else
		MatrixInverse(&mInvRot, &viewmatrix);

	// Get destination in screen space
	Vector3 v;
	v.x =  (((2.0f * pixelpos.x ) / backbuffersize.width  ) - 1 ) / projmatrix._11;
	v.y = -(((2.0f * pixelpos.y ) / backbuffersize.height ) - 1 ) / projmatrix._22;
	v.z =  1.0f;

	// Get pixel direction
	vdir->x = v.x*mInvRot._11 + v.y*mInvRot._21 + v.z*mInvRot._31;
	vdir->y = v.x*mInvRot._12 + v.y*mInvRot._22 + v.z*mInvRot._32;
	vdir->z = v.x*mInvRot._13 + v.y*mInvRot._23 + v.z*mInvRot._33;
	Vec3Normalize(vdir, vdir);

	return vdir;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Camera::Release()
{

	delete this;
}