#include <ISRayTracer.h>
#include "RayTracer.h"
#ifdef NO_D3D_INTEROP
Result RayTracer::CreateScene(IOutputWindow* wnd, ISRayTracer::IScene** _scene)
{
	return ERR("ISRaytracer.dll was build without Direct3D interop support");
}
#else
#include <ISDirect3D.h>

Image* TextureToImage(LPOUTPUTWINDOW wnd, LPTEXTURE tex)
{
	Image* img;
	LPTEXTURE localtex;
	Result rlt;

	IFFAILED(wnd->CreateTexture(tex, ITexture::TU_ALLACCESS, false, &localtex))
	{
		LOG("Error acquiring Direct3D texture. Ignoring texture");
		return NULL;
	}
	else
	{
		UINT texstride;
		img = new Image();
		img->width = localtex->GetWidth();
		img->height = localtex->GetHeight();
		localtex->MapPixelData(true, false, &texstride, (LPVOID*)&img->data);
		img->bpp = texstride;
		img->width = localtex->GetWidth();
		localtex->UnmapData();
		return img;
	}
}
Image* TextureToImage2(LPOUTPUTWINDOW wnd, LPTEXTURE tex) //DELETE
{
	Image* img;
	LPTEXTURE localtex;
	Result rlt;

	IFFAILED(wnd->CreateTexture(tex, ITexture::TU_ALLACCESS, false, &localtex))
	{
		LOG("Error acquiring Direct3D texture. Ignoring texture");
		return NULL;
	}
	else
	{
		UINT texstride;
		unsigned char* texdata;
		localtex->MapPixelData(true, false, &texstride, (LPVOID*)&texdata);
		img = Image::New(32, 32, texstride);
		for(UINT y = 0; y < 32; y++)
			for(UINT x = 0; x < 32; x++)
				for(UINT b = 0; b < texstride; b++)
					img->data[b + x * texstride + y * (32 * texstride)] = texdata[b + (x + 3 * 32) * texstride + y * (tex->GetWidth() * texstride)];
		localtex->UnmapData();
		return img;
	}
}

struct AddRenderableToSceneParams
{
	IOutputWindow* wnd;
	ISRayTracer::IScene* scene;
};
Result __stdcall AddRenderableToScene(IRenderable* renderable, LPVOID user)
{
	ISRayTracer::IScene* scene = ((AddRenderableToSceneParams*)user)->scene;
	IOutputWindow* wnd = ((AddRenderableToSceneParams*)user)->wnd;
	LPOBJECT d3dobj;
	LPBOXEDLEVEL blevel;
	Result rlt;

	if((d3dobj = dynamic_cast<LPOBJECT>(renderable)) != NULL)
	{
		// Create ISRayTracer::LPMESHSURFACE from LPOBJECT

		ISRayTracer::LPMESHSURFACE mesh;
		CHKRESULT(rtr->CreateMeshSurface(&mesh));
		scene->AddSurface(mesh);

		UINT* indices;
		d3dobj->MapIndexData(&indices);
		mesh->SetVertexPositionIndices(indices, sizeof(UINT));
		mesh->SetVertexNormalIndices(indices, sizeof(UINT));
		mesh->SetVertexTexcoordIndices(indices, sizeof(UINT));
		d3dobj->UnmapData(indices);

		IObject::Vertex* vertices;
		d3dobj->MapVertexData(&vertices);
		mesh->SetVertexPositions(&vertices->pos, sizeof(IObject::Vertex));
		mesh->SetVertexNormals(&vertices->nml, sizeof(IObject::Vertex));
		mesh->SetVertexTexcoords(&vertices->texcoord, sizeof(IObject::Vertex));
		d3dobj->UnmapData(vertices);

		UINT attributetablesize;
		d3dobj->GetAttributeTable(NULL, &attributetablesize);
		if(attributetablesize > 1)
		{
			IObject::AttributeRange* attributetable = new IObject::AttributeRange[attributetablesize];
			d3dobj->GetAttributeTable(attributetable, &attributetablesize);
			UINT* attributes = new UINT[d3dobj->GetFaceCount()]; //EDIT: Delete this when releasing the mesh surface
			for(UINT i = 0; i < attributetablesize; i++)
				for(UINT face = attributetable[i].facestart; face < attributetable[i].facestart + attributetable[i].facecount; face++)
					attributes[face] = attributetable[i].attrid;
			delete[] attributetable;
			mesh->SetFaceAttributes(attributes, sizeof(UINT));
		}

		mesh->numfaces = d3dobj->GetFaceCount();
mesh->CreateOctree();

		switch(d3dobj->orientmode)
		{
		case OM_EULER:
			mesh->SetTransform(&d3dobj->pos, &d3dobj->rot, &d3dobj->scl);
			break;

		case OM_QUAT:
			mesh->SetTransform(&d3dobj->pos, &d3dobj->qrot, &d3dobj->scl);
			break;

		case OM_MATRIX:
			mesh->SetTransform(&d3dobj->transform);
			break;

		default:
			Result::PrintLogMessage("IObject::orientmode not supported for RayTracer interop");
		}

		for(UINT i = 0; i < d3dobj->mats->size(); i++)
		{
			ISRayTracer::LPMATERIAL material;
			CHKRESULT(rtr->CreateSolidMaterial(&material));
			material->reflectance = 0.0f;
			material->transmittance = 0.0f;
			material->absorbance = 1.0f;
			material->ambientclr =  (*d3dobj->mats)[i].ambient;
			material->diffuseclr =  (*d3dobj->mats)[i].diffuse;
			material->specularclr = (*d3dobj->mats)[i].specular;
			material->specularpwr = (*d3dobj->mats)[i].power;

			if((*d3dobj->mats)[i].tex)
				material->texture = TextureToImage(wnd, (*d3dobj->mats)[i].tex);

			mesh->SetMaterial(material, i);
		}
	}
	else if((blevel = dynamic_cast<LPBOXEDLEVEL>(renderable)) != NULL)
	{
		// Create ISRayTracer::LPVMESHSURFACEs from LPBOXEDLEVEL

		std::list<BoxedLevelChunk*> chunks;
		std::list<BoxedLevelChunk*>::const_iterator iter;
		blevel->GetAssignedChunks(&chunks);
		LIST_FOREACH(chunks, iter)
		{
			ISRayTracer::LPVMESHSURFACE vmesh;
			CHKRESULT(rtr->CreateVMeshSurface(&vmesh));
			scene->AddSurface(vmesh);

			/*VOXEL* v = new VOXEL[16 * 16 * 16];
			for(int i = 0; i < 16 * 16 * 16; i++)
				v[i] = rand() % 2;

			vmesh->voxelmask = v;
			vmesh->size[0] = vmesh->size[1] = vmesh->size[2] = 16;
			Vec3Scale(&vmesh->bounds_max, &Vector3((float)vmesh->size[0], (float)vmesh->size[1], (float)vmesh->size[2]), 0.5f);
			Vec3Inverse(&vmesh->bounds_min, &vmesh->bounds_max);*/

			vmesh->voxelmask = (UINT*)(*iter)->boxmask;

			UINT chunksize[3];
			blevel->GetChunkSize(&chunksize);
			vmesh->size[0] = chunksize[0];
			vmesh->size[1] = chunksize[1];
			vmesh->size[2] = chunksize[2];
			Vec3Sub(&vmesh->bounds_min, &(*iter)->GetPos(chunksize), &Vector3(0.5f, 0.5f, 0.5f));
			Vec3Add(&vmesh->bounds_max, &vmesh->bounds_min, &Vector3((float)chunksize[0], (float)chunksize[1], (float)chunksize[2]));

			ISRayTracer::LPMATERIAL material;
			CHKRESULT(rtr->CreateSolidMaterial(&material));
			material->reflectance = 0.0f;
			material->transmittance = 0.0f;
			material->absorbance = 1.0f;
			material->refraction = 2.3f;
			material->ambientclr =  Color(0.3f, 0.3f, 0.3f);
			material->diffuseclr =  Color(0.9f, 0.9f, 0.9f);
			material->specularclr = Color(1.0f, 1.0f, 1.0f);
			material->specularpwr = 10.0f;
material->texture = TextureToImage2(wnd, blevel->GetTexture());
			vmesh->SetMaterial(material, 0);
		}
	}

	return R_OK;
}

Result RayTracer::CreateScene(IOutputWindow* wnd, ISRayTracer::IScene** _scene)
{
	Result rlt;
	CHKRESULT(CreateScene(_scene));

	// Scene
	ISRayTracer::IScene* scene = *_scene;
	scene->bgclr = wnd->backcolor;

	// Camera
	LPCAMERA d3dcam = wnd->GetCamera();
	if(d3dcam)
	{
		ISRayTracer::LPCAMERA cam;
		CreateCamera(&cam);
		scene->SetCamera(cam);
		switch(d3dcam->orientmode)
		{
		case OM_EULER:
			cam->SetTransform(&d3dcam->pos, &d3dcam->rot);
			break;

		case OM_QUAT:
			cam->SetTransform(&d3dcam->pos, &d3dcam->qrot);
			break;

		case OM_MATRIX:
			cam->SetTransform(&d3dcam->transform);
			break;

		default:
			Result::PrintLogMessage("ICamera::orientmode not supported for RayTracer interop");
		}
cam->size = Size<>(wnd->GetBackbufferWidth(), wnd->GetBackbufferHeight());
cam->region = Rect<>(0, 0, wnd->GetBackbufferWidth(), wnd->GetBackbufferHeight());
	}

	// Lights
	std::list<LPLIGHT> d3dlights;
	std::list<LPLIGHT>::const_iterator liter;
	d3d->GetEnabledLights(&d3dlights);
	LIST_FOREACH(d3dlights, liter)
	{
		LPLIGHT d3dlight = *liter;
		ISRayTracer::LPLIGHT light;
		switch(d3dlight->Type)
		{
		case ILight::LT_DIRECTIONAL:
			ISRayTracer::LPDIRLIGHT dirlight;
			CHKRESULT(rtr->CreateDirLight(&dirlight));
			dirlight->dir = d3dlight->GetDir();
			scene->AddLight(dirlight);
			light = dirlight;
			break;

		case ILight::LT_POINT:
			ISRayTracer::LPPOINTLIGHT pointlight;
			CHKRESULT(rtr->CreatePointLight(&pointlight));
			pointlight->pos = d3dlight->GetPos();
			scene->AddLight(pointlight);
			light = pointlight;
			break;

		case ILight::LT_SPOT:
			ISRayTracer::LPSPOTLIGHT spotlight;
			CHKRESULT(rtr->CreateSpotLight(&spotlight));
			spotlight->pos = d3dlight->GetPos();
			spotlight->dir = d3dlight->GetDir();
			spotlight->attenuation = d3dlight->GetAttenuation();
			spotlight->spotpower = d3dlight->GetSpotPower();
			scene->AddLight(spotlight);
			light = spotlight;
			break;
		}
		if(light)
			light->color = d3dlight->GetColor();
	}

	// Renderable classes
	AddRenderableToSceneParams artsparams;
	artsparams.scene = scene;
	artsparams.wnd = wnd;
	CHKRESULT(wnd->GetRegisteredClasses(AddRenderableToScene, &artsparams, RT_DEFAULT));

	return R_OK;
}
#endif