#include "RayTracer.h"


Color* Material::Sample(Color* clr, const Vector2* uv, TextureSampler* sampler) const
{
	if(texture)
		sampler->Sample(clr, texture, uv);
	else
		ClrSet(clr, 1.0f, 1.0f, 1.0f);
	return clr;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Material::Release()
{
	parent->materials.remove(this);

	delete this;
}