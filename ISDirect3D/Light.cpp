#include "Direct3D.h"


//-----------------------------------------------------------------------------
// Name: Light()
// Desc: Constructor
//-----------------------------------------------------------------------------
Light::Light(LightType type) : lightparams(((Direct3D*)&*d3d)->lightparams)
{
	this->enabled = true;

	// Create light params entry inside Direct3D::lightparams
	lightparamsentry = CreateLightParamsEntry();

	lightparamsentry->lightcolor = ::Color(0xFFFFFFFF);
	lightparamsentry->lighttype = type;
	Vec3Set(&lightparamsentry->lightpos, 0.0f, 0.0f, 0.0f);
	Vec3Set(&lightparamsentry->lightdir, 0.0f, 0.0f, 1.0f);
	Vec3Set(&lightparamsentry->lightattenuation, 1.0f, 0.0f, 0.0f);
	lightparamsentry->lightspotpower = 100.0f;
}

//-----------------------------------------------------------------------------
// Name: CreateLightParamsEntry()
// Desc: Adds an entry to the Direct3D::lightparams vector and returns a pointer to it.
//		 If lightparams.push_back() causes the internal array to be moved, the lightparams entry pointers for all enabled lights are updated
//-----------------------------------------------------------------------------
Light::LightParams* Light::CreateLightParamsEntry() const
{
	if(lightparams.size())
	{
		// Update lightparams entry pointers for all enabled lights
		LightParams* oldbase = &lightparams[0];
		lightparams.reserve(lightparams.size() + 1);
		size_t arraybaseoffset = (BYTE*)&lightparams[0] - (BYTE*)oldbase;
		if(arraybaseoffset)
		{
			std::list<Light*>::iterator iter;
			LIST_FOREACH(((Direct3D*)&*d3d)->lights, iter)
				if((*iter)->enabled)
					(*iter)->lightparamsentry = (LightParams*)((BYTE*)(*iter)->lightparamsentry + arraybaseoffset);
		}
	}

	// Create lightparams entry inside Direct3D::lightparams
	lightparams.push_back(LightParams());
	return &lightparams.back();
}
void Light::RemoveLightParamsEntry(Light::LightParams* entry) const
{
	// Move down lightparams entries pointers above entry for all enabled lights
	std::list<Light*>::iterator iter;
	LIST_FOREACH(((Direct3D*)&*d3d)->lights, iter)
	if((*iter)->enabled && (*iter)->lightparamsentry > entry)
		(*iter)->lightparamsentry -= 1;

	// Remove entry
	lightparams.erase(lightparams.begin() + (entry - &lightparams[0]));
}

void Light::SetTransform(const Matrix* transform)
{
	Vec3Set(&lightparamsentry->lightpos, transform->_41, transform->_42, transform->_43);
	Vector4 dirvec(0.0f, 0.0f, 1.0f, 0.0f);
	dirvec = *transform * dirvec;
	Vec3Set(&lightparamsentry->lightdir, dirvec.x, dirvec.y, dirvec.z); //EDIT
}

const Vector3& Light::GetPos()
{
	return lightparamsentry->lightpos;
}
void Light::SetPos(const Vector3& val)
{
	lightparamsentry->lightpos = val;
}
const Vector3& Light::GetDir()
{
	return lightparamsentry->lightdir;
}
void Light::SetDir(const Vector3& val)
{
	lightparamsentry->lightdir = val;
	Vec3Normalize(&lightparamsentry->lightdir);
}
const ::Color& Light::GetColor()
{
	return lightparamsentry->lightcolor;
}
void Light::SetColor(const ::Color& val)
{
	lightparamsentry->lightcolor = val;
}
ILight::LightType Light::GetType()
{
	return (ILight::LightType)lightparamsentry->lighttype;
}
void Light::SetType(LightType val)
{
	lightparamsentry->lighttype = val;
}
const Vector3& Light::GetAttenuation()
{
	return lightparamsentry->lightattenuation;
}
void Light::SetAttenuation(const Vector3& val)
{
	lightparamsentry->lightattenuation = val;
	//EDIT: normalize attenuation
}
float Light::GetSpotPower()
{
	return lightparamsentry->lightspotpower;
}
void Light::SetSpotPower(float val)
{
	lightparamsentry->lightspotpower = val;
}

bool Light::GetEnabled()
{
	return enabled;
}
void Light::SetEnabled(bool val)
{
	if(val == enabled)
		return;
	enabled = val;

	if(enabled)
	{
		// Create light params entry inside Direct3D::lightparams
		lightparamsentry = CreateLightParamsEntry();

		// Set pointer to new lightparams entry
		lightparamsentry = &lightparams.back();

		// Copy lightparams from local entry to Direct3D::lightparams entry
		memcpy(lightparamsentry, &locallightparams, sizeof(LightParams));
	}
	else
	{
		// Copy lightparams from Direct3D::lightparams entry to local entry
		memcpy(&locallightparams, lightparamsentry, sizeof(LightParams));

		// Remove lightparamsentry from Direct3D::lightparams
		RemoveLightParamsEntry(lightparamsentry);

		// Set pointer to temporary local lightparams
		lightparamsentry = &locallightparams;
	}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Light::Release()
{
	((Direct3D*)&*d3d)->lights.remove(this);

	if(enabled)
		// Remove lightparamsentry from Direct3D::lightparams
		RemoveLightParamsEntry(lightparamsentry);

	delete this;
}