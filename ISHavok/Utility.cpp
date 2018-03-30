#include "Havok.h"
#include "ISDirect3D.h"

//-----------------------------------------------------------------------------
// Name: CreateCollisionXXX()
// Desc: Helper functions for building Havok entities around D3D objects
//-----------------------------------------------------------------------------
Result Havok::CreateCollisionMesh(LPOBJECT obj, IRegularEntity** entity)
{
	return obj->CreateCollisionMesh(entity); //EDIT: Decide whether to drop IObject::CreateCollisionMesh or IHavok::CreateCollisionMesh
}
Result Havok::CreateConvexCollisionHull(const HvkGeneralShapeDesc* shapedesc, LPOBJECT obj, IRegularEntity** entity)
{
	Result rlt;

	IObject::Vertex* vertices;

	CHKRESULT(obj->MapVertexData(&vertices));

	HvkConvexVerticesShapeDesc desc;
	memcpy(&desc, shapedesc, sizeof(HvkGeneralShapeDesc));
	desc.numvertices = obj->GetVertexCount();

	obj->orientmode = OM_QUAT;
	if(obj->scl.x == 1.0f && obj->scl.y == 1.0f && obj->scl.z == 1.0f)
	{
		desc.vertices = (BYTE*)vertices;
		desc.fvfstride = sizeof(IObject::Vertex);
		CHKRESULT(CreateRegularEntity(&desc, IHavok::LAYER_HANDHELD, &obj->pos, &obj->qrot, entity));
	}
	else
	{
		float* scaledvertices;
		CHKALLOC(scaledvertices = new float[desc.numvertices * 3]);
		for(int i = 0; i < desc.numvertices; i++)
		{
			scaledvertices[3 * i + 0] = vertices[i].pos.x * obj->scl.x;
			scaledvertices[3 * i + 1] = vertices[i].pos.y * obj->scl.y;
			scaledvertices[3 * i + 2] = vertices[i].pos.z * obj->scl.z;
		}
		desc.vertices = (BYTE*)scaledvertices;
		desc.fvfstride = 3 * sizeof(float);
		CHKRESULT(CreateRegularEntity(&desc, IHavok::LAYER_HANDHELD, &obj->pos, &obj->qrot, entity));
		delete[] scaledvertices;
	}

	CHKRESULT(obj->UnmapData(vertices));

	return R_OK;
}
Result Havok::CreateCollisionCapsule(LPOBJECT obj, float radius, IRegularEntity** entity)
{
	return obj->CreateCollisionCapsule(radius, entity); //EDIT: Decide whether to drop IObject::CreateCollisionCapsule or IHavok::CreateCollisionCapsule
}
Result Havok::CreateCollisionRagdoll(LPOBJECT obj, float radius, IRagdollEntity** entity)
{
	return obj->CreateCollisionRagdoll(radius, false, entity); //EDIT: Decide whether to drop IObject::CreateCollisionRagdoll or IHavok::CreateCollisionRagdoll
}