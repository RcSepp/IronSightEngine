#include <ISRayTracer.h>
#include <ISSceneDefinition.h>
#include "RayTracer.h"

Result CreateMaterial(const SceneDefinition::Material* matdef, const std::map<FilePath, Image*>* textures, ISRayTracer::LPMATERIAL* _material)
{
	Result rlt;

	CHKRESULT(rtr->CreateSolidMaterial(_material));
	ISRayTracer::LPMATERIAL material = *_material;

	material->reflectance = matdef->reflectance;
	material->transmittance = matdef->transmittance;
	material->absorbance = 1.0f - matdef->reflectance - matdef->transmittance;
	material->refraction = matdef->refraction;
	material->specularclr = Color(matdef->phong[2], matdef->phong[2], matdef->phong[2]);
	material->specularpwr = matdef->phong[3];
	switch(matdef->type)
	{
	case SceneDefinition::MT_SOLID:
		ClrScale(&material->ambientclr, &Color(((SceneDefinition::SolidMaterial*)matdef)->color), matdef->phong[0]);
		ClrScale(&material->diffuseclr, &Color(((SceneDefinition::SolidMaterial*)matdef)->color), matdef->phong[1]);
		ClrScale(&material->emissiveclr, &Color(((SceneDefinition::SolidMaterial*)matdef)->color), matdef->phong[4]);
		break;
	case SceneDefinition::MT_TEXTURED:
		material->ambientclr = Color(matdef->phong[0], matdef->phong[0], matdef->phong[0]);
		material->diffuseclr = Color(matdef->phong[1], matdef->phong[1], matdef->phong[1]);
		material->emissiveclr = Color(matdef->phong[4], matdef->phong[4], matdef->phong[4]);

		if(textures)
		{
			// Set color texture
			SceneDefinition::TexturedMaterial* texmaterial = (SceneDefinition::TexturedMaterial*)matdef;
			std::map<FilePath, Image*>::const_iterator pair = textures->find(texmaterial->texture_file);
			if(pair == textures->end())
				Result::PrintLogMessage(String("No texture loaded for ") << texmaterial->texture_file << String(". Ignoring texture"));
			else
				material->texture = pair->second;

			if(!texmaterial->normalmap_file.IsEmpty())
			{
				// Set normal map
				pair = textures->find(texmaterial->normalmap_file);
				if(pair == textures->end())
					Result::PrintLogMessage(String("No texture loaded for ") << texmaterial->normalmap_file << String(". Ignoring normal map"));
				else
					material->normalmap = pair->second;
			}
		}
		else
			Result::PrintLogMessage(String("No textures provided. Ignoring texture"));

		break;
	}

	return R_OK;
}

void SetTransform(ISRayTracer::LPSURFACE sfc, const SceneDefinition::Surface* sfcdef)
{
	if(sfcdef->transform)
	{
		Vector3 rot;
		Vec3Scale(&rot, &Vector3(sfcdef->transform->rotate), PI / 180.0f);
		sfc->SetTransform(&Vector3(sfcdef->transform->translate), &rot, &Vector3(sfcdef->transform->scale));
	}
}

void SetObjMaterialProperties(ISRayTracer::LPMATERIAL material, const SceneDefinition::Material* xmlmatdef,
							  const MeshDefinition::Surface::Material* objmatdef, std::map<FilePath, Image*>* textures)
{
	// Overwrite XML material properties with material properties of OBJ material
	if(objmatdef->ambient[0] != -1.0f)
		ClrSet(&material->ambientclr, objmatdef->ambient[0] * xmlmatdef->phong[0], objmatdef->ambient[1] * xmlmatdef->phong[0], objmatdef->ambient[2] * xmlmatdef->phong[0]);
	else if(objmatdef->diffuse[0] != -1.0f) // Some OBJ material files only set diffuse color. If that is the case use it also as ambient color
		ClrSet(&material->ambientclr, objmatdef->diffuse[0] * xmlmatdef->phong[0], objmatdef->diffuse[1] * xmlmatdef->phong[0], objmatdef->diffuse[2] * xmlmatdef->phong[0]);
	if(objmatdef->diffuse[0] != -1.0f)
		ClrSet(&material->diffuseclr, objmatdef->diffuse[0] * xmlmatdef->phong[1], objmatdef->diffuse[1] * xmlmatdef->phong[1], objmatdef->diffuse[2] * xmlmatdef->phong[1]);
	if(objmatdef->specular[0] != -1.0f)
		ClrSet(&material->specularclr, objmatdef->specular[0] * xmlmatdef->phong[2], objmatdef->specular[1] * xmlmatdef->phong[2], objmatdef->specular[2] * xmlmatdef->phong[2]);
	else if(objmatdef->diffuse[0] != -1.0f) // Some OBJ material files only set diffuse color. If that is the case use it also as specular color
		ClrSet(&material->specularclr, objmatdef->diffuse[0] * xmlmatdef->phong[2], objmatdef->diffuse[1] * xmlmatdef->phong[2], objmatdef->diffuse[2] * xmlmatdef->phong[2]);
	if(objmatdef->emissive[0] != -1.0f)
		ClrSet(&material->emissiveclr, objmatdef->emissive[0], objmatdef->emissive[1], objmatdef->emissive[2]);
	if(objmatdef->power != -1.0f)
		material->specularpwr = objmatdef->power;
	if(objmatdef->transparency != -1.0f)
	{
		material->transmittance = objmatdef->transparency;
		if(!material->refraction)
			material->refraction = 1.57f; // Default to a refractive index of 1.57 (glass)
	}
	if(!objmatdef->texfilename.IsEmpty())
	{
		if(textures)
		{
			std::map<FilePath, Image*>::const_iterator pair = textures->find(objmatdef->texfilename);
			if(pair == textures->end())
				Result::PrintLogMessage(String("No texture loaded for ") << objmatdef->texfilename << String(". Ignoring texture"));
			else
				material->texture = pair->second;
		}
		else
			Result::PrintLogMessage(String("No textures provided. Ignoring texture"));
	}
}

Result RayTracer::CreateScene(const SceneDefinition* scenedef, const std::map<FilePath, MeshDefinition*>* meshdefs, std::map<FilePath, Image*>* textures, ISRayTracer::IScene** _scene)
{
	Result rlt;
	CHKRESULT(CreateScene(_scene));

	// Scene
	ISRayTracer::IScene* scene = *_scene;
	scene->bgclr = scenedef->background_color;

	// Camera
	ISRayTracer::LPCAMERA cam;
	CreateCamera(&cam);
	scene->SetCamera(cam);
	cam->SetTransform(&Vector3(scenedef->camera.position), &Vector3(scenedef->camera.lookat), &Vector3(scenedef->camera.up));
	cam->fovx = scenedef->camera.horizontal_fov * PI / 180.0f;
	cam->maxbounces = scenedef->camera.max_bounces;
	cam->size = Size<>(scenedef->camera.resolution[0], scenedef->camera.resolution[1]);
	cam->fovy = cam->fovx * cam->size.height / cam->size.width;
	cam->region = Rect<>(0, 0, cam->size.width, cam->size.height);


	scene->ambientclr = Color(0.0f, 0.0f, 0.0f);
	std::list<SceneDefinition::Light*>::const_iterator liter;
		LIST_FOREACH(scenedef->lights, liter)
			switch((*liter)->type)
			{
			case SceneDefinition::LT_AMBIENT:
				{
					// Sum up all ambient lights
					Color ambclr((*liter)->color);
					ClrAdd(&scene->ambientclr, &scene->ambientclr, &ambclr);
				}
				break;
			case SceneDefinition::LT_POINT:
				{
					ISRayTracer::LPPOINTLIGHT pointlight;
					SceneDefinition::PointLight* pointlightdef = (SceneDefinition::PointLight*)*liter;
					CreatePointLight(&pointlight);
					pointlight->color = pointlightdef->color;
					pointlight->pos = pointlightdef->position;
					scene->AddLight(pointlight);
				}
				break;
			case SceneDefinition::LT_PARALLEL:
				{
					ISRayTracer::LPDIRLIGHT dirlight;
					SceneDefinition::ParallelLight* pointlightdef = (SceneDefinition::ParallelLight*)*liter;
					CreateDirLight(&dirlight);
					dirlight->color = pointlightdef->color;
					Vec3Normalize(&dirlight->dir, &Vector3(pointlightdef->direction));
					scene->AddLight(dirlight);
				}
				break;
			case SceneDefinition::LT_SPOT:
				//EDIT
				break;
			}

		ISRayTracer::LPMATERIAL material;
		std::list<SceneDefinition::Surface*>::const_iterator siter;
		LIST_FOREACH(scenedef->surfaces, siter)
		{
			switch((*siter)->type)
			{
			case SceneDefinition::ST_SPHERE:
				{
					ISRayTracer::LPSPHERESURFACE sphere;
					SceneDefinition::Sphere* spheredef = (SceneDefinition::Sphere*)*siter;
					CreateSphereSurface(&Vector3(spheredef->position), spheredef->radius, &sphere);
					scene->AddSurface(sphere);

					CHKRESULT(CreateMaterial(spheredef->material, textures, &material));
					sphere->SetMaterial(material);
					SetTransform(sphere, spheredef);
				}
				//surfaces.insert((*siter)->key.IsEmpty() ? String("Sphere") << String(sphereidx++) : (*siter)->key, (PyObject*)new Sphere((SceneDefinition::Sphere*)*siter, textures));
				break;
			case SceneDefinition::ST_MESH:
				{
					if(meshdefs)
					{
						SceneDefinition::Mesh* meshdef = (SceneDefinition::Mesh*)*siter;
						std::map<FilePath, MeshDefinition*>::const_iterator pair = meshdefs->find(meshdef->mesh_file);
						if(pair == meshdefs->end())
							Result::PrintLogMessage(String("No mesh definition found for ") << meshdef->mesh_file << String(". Ignoring mesh"));
						else
						{
							MeshDefinition* _meshdef = pair->second;
							std::vector<MeshDefinition::Surface*>::const_iterator sfciter;
							LIST_FOREACH(_meshdef->surfaces, sfciter)
							{
								ISRayTracer::LPMESHSURFACE mesh;
								CreateMeshSurface(&mesh);
								mesh->SetVertexPositionIndices(&(*sfciter)->positionindices[0], sizeof((*sfciter)->positionindices[0]));
								if(!(*sfciter)->normalindices.empty()) mesh->SetVertexNormalIndices(&(*sfciter)->normalindices[0], sizeof((*sfciter)->normalindices[0]));
								if(!(*sfciter)->texcoordindices.empty()) mesh->SetVertexTexcoordIndices(&(*sfciter)->texcoordindices[0], sizeof((*sfciter)->texcoordindices[0]));
								mesh->SetVertexPositions(&_meshdef->positions[0], sizeof(Vector3));
								if(!_meshdef->normals.empty()) mesh->SetVertexNormals(&_meshdef->normals[0], sizeof(Vector3));
								if(!_meshdef->binormals.empty()) mesh->SetVertexBinormals(&_meshdef->binormals[0], sizeof(Vector3));
								if(!_meshdef->tangents.empty()) mesh->SetVertexTangents(&_meshdef->tangents[0], sizeof(Vector3));
								if(!_meshdef->texcoords.empty()) mesh->SetVertexTexcoords(&_meshdef->texcoords[0], sizeof(Vector2));
								mesh->numfaces = (*sfciter)->positionindices.size() / 3;
mesh->CreateOctree();
								scene->AddSurface(mesh);

								CHKRESULT(CreateMaterial(meshdef->material, textures, &material));
								SetObjMaterialProperties(material, meshdef->material, &(*sfciter)->material, textures);
								mesh->SetMaterial(material);
								SetTransform(mesh, meshdef);
							}
						}
					}
					else
						Result::PrintLogMessage(String("No mesh definitions provided. Ignoring mesh"));
				}
				{
					/*SceneDefinition::Mesh* mesh = (SceneDefinition::Mesh*)*siter;
					std::map<FilePath, MeshDefinition*>::const_iterator pair = meshdefs.find(mesh->mesh_file);
					if(pair == meshdefs.end())
						Result::PrintLogMessage(String("No mesh definition found for ") << mesh->mesh_file << String(". Ignoring mesh"));
					else
					{
						MeshDefinition* meshdef = pair->second;
						std::vector<MeshDefinition::Surface*>::const_iterator sfciter;
						subsetidx = 0;
						LIST_FOREACH(meshdef->surfaces, sfciter)
							if(!(*sfciter)->positionindices.empty())
								surfaces.insert((*siter)->key.IsEmpty() ? String("Mesh") << String(meshidx) << String(".") << String(subsetidx++) : (*siter)->key,
												(PyObject*)new Mesh(mesh, meshdef, (MeshDefinition::Surface*)*sfciter, textures));
						if((*siter)->key.IsEmpty())
							meshidx++;
					}*/
				}
				break;
			case SceneDefinition::ST_VOXEL:
				{
					/*SceneDefinition::VMesh* mesh = (SceneDefinition::VMesh*)*siter;
					std::map<FilePath, VMeshDefinition*>::const_iterator pair = vmeshdefs.find(mesh->voxel_file);
					if(pair == vmeshdefs.end())
						Result::PrintLogMessage(String("No mesh definition found for ") << mesh->voxel_file << String(". Ignoring voxel mesh"));
					else
						surfaces.insert((*siter)->key.IsEmpty() ? String("VMesh") << String(vmeshidx++) : (*siter)->key, (PyObject*)new VMesh((SceneDefinition::VMesh*)*siter, pair->second, textures));*/
				}
				break;
			}
		}




	return R_OK;
}