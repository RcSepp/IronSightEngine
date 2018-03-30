#ifndef __ISMESHDEFINITION_H
#define	__ISMESHDEFINITION_H

#include <ISTypes.h>


struct MeshDefinition
{
	struct Surface
	{
		struct Material
		{
			float ambient[3], diffuse[3], specular[3], emissive[3];
			float power;
			float transparency;
			bool isspecular;
			FilePath texfilename;
			enum IlluminationType
			{
				IT_COLOR_ON_AND_AMBIENT_OFF,
				IT_COLOR_ON_AND_AMBIENT_ON,
				IT_HIGHLIGHT_ON,
				IT_REFLECTION_ON_AND_RAY_TRACE_ON,
				IT_TRANSPARENCY_GLASS_ON_AND_REFLECTION_RAY_TRACE_ON,
				IT_REFLECTION_FRESNEL_ON_AND_RAY_TRACE_ON,
				IT_TRANSPARENCY_REFRACTION_ON_AND_REFLECTION_FRESNEL_OFF_AND_RAY_TRACE_ON,
				IT_TRANSPARENCY_REFRACTION_ON_AND_REFLECTION_FRESNEL_ON_AND_RAY_TRACE_ON,
				IT_REFLECTION_ON_AND_RAY_TRACE_OFF,
				IT_TRANSPARENCY_GLASS_ON_AND_REFLECTION_RAY_TRACE_OFF,
				IT_CASTS_SHADOWS_ONTO_INVISIBLE_SURFACES,
				IT_NOT_SET
			} illum;

			// Init all material members to unused values
			Material() : power(-1), transparency(-1.0f), texfilename(), illum(IT_NOT_SET)
			{
				ambient[0] = ambient[1] = ambient[2] = -1.0f;
				diffuse[0] = diffuse[1] = diffuse[2] = -1.0f;
				specular[0] = specular[1] = specular[2] = -1.0f;
				emissive[0] = emissive[1] = emissive[2] = -1.0f;
			}
		} material;
		String name;
		std::vector<unsigned int> positionindices, normalindices, texcoordindices;

		Surface(String name) : name(name) {}
	};
	std::vector<float> positions, normals, texcoords;
	std::vector<float> binormals, tangents; // Optional
	std::vector<Surface*> surfaces;

	~MeshDefinition()
	{
		std::vector<Surface*>::iterator sfciter;
		LIST_FOREACH(surfaces, sfciter)
			delete *sfciter;
	}
};

Result ParseObjMesh(FilePath filename, MeshDefinition** meshdef);
Result ComputeTangentsAndBinormals(FilePath filename, MeshDefinition* meshdef); // Filename only used for logging

#endif