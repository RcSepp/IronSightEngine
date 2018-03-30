#ifndef __SCENEDEFINITION_H
#define	__SCENEDEFINITION_H

#include <ISTypes.h>


struct SceneDefinition
{
	FilePath output_file;
	float background_color[3];

	struct Camera
	{
		float position[3], lookat[3], up[3];
		float horizontal_fov;
		int resolution[2];
		int max_bounces;
	} camera;

	enum LightType {LT_AMBIENT, LT_POINT, LT_PARALLEL, LT_SPOT};
	struct Light
	{
		const LightType type;
		float color[3];

		Light(LightType type) : type(type) {}
	};
	struct AmbientLight : public Light
	{
		AmbientLight() : Light(LT_AMBIENT) {}
	};
	struct PointLight : public Light
	{
		float position[3];

		PointLight() : Light(LT_POINT) {}
	};
	struct ParallelLight : public Light
	{
		float direction[3];

		ParallelLight() : Light(LT_PARALLEL) {}
	};
	std::list<Light*> lights;

	enum MaterialType {MT_SOLID, MT_TEXTURED};
	struct Material
	{
		const MaterialType type;
		float phong[5];
		float reflectance, transmittance, refraction;

		Material(MaterialType type) : type(type)
		{
			phong[0] = phong[1] = phong[2] = phong[3] = phong[4] = 0.0f;
		}
	};
	struct SolidMaterial : public Material
	{
		float color[3];

		SolidMaterial() : Material(MT_SOLID) {}
	};
	struct TexturedMaterial : public Material
	{
		FilePath texture_file, normalmap_file;

		TexturedMaterial() : Material(MT_TEXTURED) {}
	};

	struct Transform
	{
		float translate[3], scale[3], rotate[3];

		Transform()
		{
			translate[0] = translate[1] = translate[2] = 0.0f;
			scale[0] = scale[1] = scale[2] = 1.0f;
			rotate[0] = rotate[1] = rotate[2] = 0.0f;
		}
	};

	enum SurfaceType {ST_SPHERE, ST_MESH, ST_VOXEL};
	struct Surface
	{
		const SurfaceType type;
		Material* material;
		Transform* transform;
		String key;

		Surface(SurfaceType type) : type(type), material(NULL), transform(NULL) {}
		~Surface()
		{
			if(material)
				delete material;
			if(transform)
				delete transform;
		}
	};
	struct Sphere : public Surface
	{
		float radius;
		float position[3];

		Sphere() : Surface(ST_SPHERE) {}
	};
	struct Mesh : public Surface
	{
		FilePath mesh_file;

		Mesh() : Surface(ST_MESH) {}
	};
	struct VMesh : public Surface
	{
		FilePath voxel_file;

		VMesh() : Surface(ST_VOXEL) {}
	};
	std::list<Surface*> surfaces;

	struct AnimationScript
	{
		float starttime, duration, endtime;
		String type, script;
		AnimationScript() : duration(-1e20f), endtime(-1e20f) {}
	};
	struct Animation
	{
		float interval;
		std::list<AnimationScript> scripts;
	}* animation;

	SceneDefinition() : animation(NULL) {}
	~SceneDefinition()
	{
		std::list<Light*>::iterator lightiter;
		LIST_FOREACH(lights, lightiter)
			delete *lightiter;
		std::list<Surface*>::iterator surfaceiter;
		LIST_FOREACH(surfaces, surfaceiter)
			delete *surfaceiter;
		if(animation)
			delete animation;
	}
};

Result ParseXmlScene(FilePath filename, SceneDefinition** scenedef);

#endif