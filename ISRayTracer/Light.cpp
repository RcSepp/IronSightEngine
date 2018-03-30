#include "RayTracer.h"


float PointLight::GetExposure(const Vector3* pos, RayCastParams& params)
{
	Vec3Sub(&params.raydir, &this->pos, pos);
	float lightdist = Vec3Length(&params.raydir);
	params.raydir /= lightdist;
	params.raypos = *pos;

	unsigned char oldflags = params.flags;
	params.flags = 0;
	bool isvisible = !params.scene->CastRay(params, NULL) || params.hitdist + FLOAT_TOLERANCE >= lightdist;
	params.flags = oldflags;
	return (float)isvisible;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void PointLight::Release()
{
	parent->lights.remove(this);

	delete this;
}


float DirLight::GetExposure(const Vector3* pos, RayCastParams& params)
{
	Vec3Inverse(&params.raydir, &dir);
	params.raypos = *pos;

	unsigned char oldflags = params.flags;
	params.flags = 0;
	bool isvisible = !params.scene->CastRay(params, NULL);
	params.flags = oldflags;
	return (float)isvisible;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void DirLight::Release()
{
	parent->lights.remove(this);

	delete this;
}


float SpotLight::GetExposure(const Vector3* pos, RayCastParams& params)
{
	Vec3Sub(&params.raydir, &this->pos, pos);
	float lightdist = Vec3Length(&params.raydir);
	params.raydir /= lightdist;
	params.raypos = *pos;

	unsigned char oldflags = params.flags;
	params.flags = 0;
	bool isvisible = !params.scene->CastRay(params, NULL) || params.hitdist + FLOAT_TOLERANCE >= lightdist;
	params.flags = oldflags;
	if(isvisible)
	{
		float dot = Vec3Dot(&params.raydir, &dir);
		float spot = pow(max(0.0f, -dot), spotpower);
		float att = spot / Vec3Dot(&attenuation, &Vector3(1.0f, lightdist, lightdist*lightdist));
		return saturate(att); //EDIT: Is saturate needed?
	}
	else
		return 0.0f;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void SpotLight::Release()
{
	parent->lights.remove(this);

	delete this;
}