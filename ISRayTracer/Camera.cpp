#include "RayTracer.h"


const Matrix* Camera::SetTransform(const Matrix* newtransform)
{
	MatrixInverse(&transform, newtransform);
	Vec3Set(&pos, 0.0f, 0.0f, 0.0f);
	Vec3TransformCoord(&pos, &pos, &transform);
	return newtransform;
}
void Camera::SetTransform(const Vector3* translation, const Vector3* rotation)
{
	pos = *translation;

	Matrix r1, r2, r3, t;
	MatrixRotationX(&r1, -rotation->x);
	MatrixRotationY(&r2, -rotation->y);
	MatrixRotationZ(&r3, -rotation->z);
	MatrixTranslation(&t, -translation->x, -translation->y, -translation->z);
	transform = t * r1 * r2 * r3;
}
void Camera::SetTransform(const Vector3* translation, const Quaternion* rotation)
{
	pos = *translation;

	Matrix r, t;
	Quaternion invrotation;
	QuaternionInverse(&invrotation, rotation);
	MatrixRotationQuaternion(&r, &invrotation);
	MatrixTranslation(&t, -translation->x, -translation->y, -translation->z);
	transform = r * t;
}
void Camera::SetTransform(const Vector3* pos, const Vector3* target, const Vector3* up)
{
	MatrixLookAtRH(&transform, pos, target, up);
	this->pos = *pos;
}

/*Camera::RayIterator Camera::begin()
{
	return RayIterator(this, region.left(), region.top());
}
Camera::RayIterator Camera::end()
{
	return RayIterator(this, region.left(), region.bottom());
}*/
Vector3* Camera::SetRayDir(Vector3* vDir, const Point<>& pixelpos)
{
	// Create dir vector pointing towards the pixel at [x, y] for a camera facing towards [0, 0, 1] with up-vector [0, 1, 0]
	Vector3 vpixeldir((2.0f * (float)pixelpos.x / (float)size.width - 1.0f) * tan(fovx), (-2.0f * (float)pixelpos.y / (float)size.height + 1.0f) * tan(fovy), 1.0f);
	Vec3Normalize(&vpixeldir);

	// params.raydir = vpixeldir rotated according to cam.transform
	Vec3TransformNormal(vDir, &vpixeldir, &transform);
	return vDir;
}
Vector3* Camera::SetRayDir(Vector3* vDir, const Point<>& pixelpos, const Vector2* pixeloffset)
{
	// Create dir vector pointing towards the pixel at [x, y] for a camera facing towards [0, 0, 1] with up-vector [0, 1, 0]
	Vector3 vpixeldir((2.0f * ((float)pixelpos.x + pixeloffset->x) / (float)size.width - 1.0f) * tan(fovx), (-2.0f * (float)(pixelpos.y + pixeloffset->y) / (float)size.height + 1.0f) * tan(fovy), 1.0f);
	Vec3Normalize(&vpixeldir);

	// params.raydir = vpixeldir rotated according to cam.transform
	Vec3TransformNormal(vDir, &vpixeldir, &transform);
	return vDir;
}
/*void Camera::RayIterator::operator++()
{
	x++;
	if(cam)
	{
		x -= cam->region.location.x;
		y += x / cam->region.size.width;
		x %= cam->region.size.width;
		x += cam->region.location.x;
	}
}
void Camera::RayIterator::operator++(int)
{
	x++;
	if(cam)
	{
		x -= cam->region.location.x;
		y += x / cam->region.size.width;
		x %= cam->region.size.width;
		x += cam->region.location.x;
	}
}*/

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Camera::Release()
{
	parent->cameras.remove(this);

	delete this;
}