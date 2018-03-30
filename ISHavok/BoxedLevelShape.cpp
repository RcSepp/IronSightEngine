#include "Havok.h"
#include "BoxedLevelShape.h"
#include "IntersectRayAABB.h"
#include "RayCastCube.h"
//#include <Physics/Collide/Dispatch/hkpCollisionDispatcher.h>
#include <Physics/Collide/Shape/Query/hkpShapeRayCastOutput.h>


#pragma region BoxedLevelChunkShapeContainer

BoxedLevelChunkShapeContainer::BoxedLevelChunkShapeContainer(BoxedLevelShape* parent, const UINT (&chunksize)[3], BoxedLevelChunk* chunk)
	: parent(parent), chunk(chunk), chunkpos(chunk->GetPos(chunksize))
{
	memcpy(this->chunksize, chunksize, 3 * sizeof(UINT));
}

const hkpShape* BoxedLevelChunkShapeContainer::getChildShape(hkpShapeKey key, hkpShapeBuffer& buffer) const
{
	// Format: [[[x]y]z]
	/*int x, y, z;
	y = key / parent->chunksize[0];
	x = key - y * parent->chunksize[0];
	z = y / parent->chunksize[1];
	y -= z * parent->chunksize[1];*/

	// Format: [[[y]z]x] (binvox format)
	int x, y, z;
	z = key / parent->chunksize[1];
	y = key - z * parent->chunksize[1];
	x = z / parent->chunksize[2];
	z -= x * parent->chunksize[2];

	hkTransform trans;
	trans.setIdentity();
	trans.setTranslation(hkVector4(chunkpos.x + (hkReal)x, chunkpos.y + (hkReal)y, chunkpos.z + (hkReal)z));
	trans.setRotation(hkRotation(parent->boxrotations[chunk->boxmask[key].GetOrientation()]));

	hkpTransformShape* boxshape = parent->boxshapes[parent->boxtypes[chunk->boxmask[key].GetTypeIdx()]];
	boxshape->setTransform(trans);
	return boxshape;
}
const hkpShape* BoxedLevelChunkShapeContainer::getChildShape(hkpShapeKey key) const
{
	// Format: [[[x]y]z]
	/*int x, y, z;
	y = key / parent->chunksize[0];
	x = key - y * parent->chunksize[0];
	z = y / parent->chunksize[1];
	y -= z * parent->chunksize[1];*/

	// Format: [[[y]z]x] (binvox format)
	int x, y, z;
	z = key / parent->chunksize[1];
	y = key - z * parent->chunksize[1];
	x = z / parent->chunksize[2];
	z -= x * parent->chunksize[2];

	hkTransform trans;
	trans.setIdentity();
	trans.setTranslation(hkVector4(chunkpos.x + (hkReal)x, chunkpos.y + (hkReal)y, chunkpos.z + (hkReal)z));
	trans.setRotation(hkRotation(parent->boxrotations[chunk->boxmask[key].GetOrientation()]));

	hkpTransformShape* boxshape = parent->boxshapes[parent->boxtypes[chunk->boxmask[key].GetTypeIdx()]];
	boxshape->setTransform(trans);
	return boxshape;
}

BoxedLevelChunkShapeContainer::~BoxedLevelChunkShapeContainer() {}

#pragma endregion

#pragma region BoxedLevelChunkShape

BoxedLevelChunkShape::BoxedLevelChunkShape(BoxedLevelShape* parent, const UINT (&chunksize)[3], BoxedLevelChunk* chunk)
	: hkpBvTreeShape(hkcdShapeType::USER1, hkpBvTreeShape::BVTREE_USER), container(parent, chunksize, chunk),
	  bounds_min(chunk->GetPosX(chunksize) - 0.5f, chunk->GetPosY(chunksize) - 0.5f, chunk->GetPosZ(chunksize) - 0.5f)
{
	memcpy(this->chunksize, chunksize, 3 * sizeof(UINT));
	bounds_max = bounds_min;
	bounds_max.add(hkVector4((hkReal)parent->chunksize[0], (hkReal)parent->chunksize[1], (hkReal)parent->chunksize[2]));
}

void BoxedLevelChunkShape::getAabb(const hkTransform& localToWorld, hkReal tolerance, hkAabb& aabbOut) const
{
	aabbOut.m_min.setTransformedPos(localToWorld, bounds_min);
	aabbOut.m_max.setTransformedPos(localToWorld, bounds_max);
}

//#include <ISDirect3D.h>
hkBool BoxedLevelChunkShape::castRay(const hkpShapeRayCastInput& input, hkpShapeRayCastOutput& output) const
{
	float bounds_min[] = {this->bounds_min(0), this->bounds_min(1), this->bounds_min(2)};
	float bounds_max[] = {this->bounds_max(0), this->bounds_max(1), this->bounds_max(2)};
	float m_from[] = {input.m_from(0), input.m_from(1), input.m_from(2)};
	hkVector4 vraydir(input.m_to);
	vraydir.sub(input.m_from);
	float maxdist = vraydir.length3();
	vraydir.normalize<3>();
	float raydir[] = {vraydir(0), vraydir(1), vraydir(2)};
	float raypos[3];

	// Set raypos to level bounds
	if(!IntersectRayAABB(bounds_min, bounds_max, m_from, raydir, raypos))
		return false;

	// Validate ray distance
	hkVector4 vraypos(raypos[0], raypos[1], raypos[2]);
	vraypos.sub(input.m_from);
	float disttobounds = vraypos.length3();
	if(disttobounds > maxdist)
		return false;

//Vector3 v0(raypos[0], raypos[1], raypos[2]);
//Vector3 vdir = vraydir;
//Vector3 v1(v0.x + vdir.x, v0.y + vdir.y, v0.z + vdir.z);
//(*d3d->GetOutputWindows().begin())->DrawLine(v0, v1, Color(0xFFFF0000), Color(0xFFFF8080));

	// Transform to local space (with origin at bounds_min)
	hkVector4 transformed_vfrom(m_from[0] - bounds_min[0], m_from[1] - bounds_min[1], m_from[2] - bounds_min[2]);
	raypos[0] -= bounds_min[0];
	raypos[1] -= bounds_min[1];
	raypos[2] -= bounds_min[2];

	int boxpos[] = {min((int)chunksize[0] - 1, (int)raypos[0]), min((int)chunksize[1] - 1, (int)raypos[1]), min((int)chunksize[2] - 1, (int)raypos[2])};
	do
	{
		// Query box mask at boxpos
		hkpShapeKey shapekey = boxpos[0] * (chunksize[1] * chunksize[2]) + boxpos[2] * chunksize[1] + boxpos[1];
		if(container.chunk->boxmask[shapekey].IsBox() && container.getChildShape(shapekey)->castRay(input, output))
		{
			output.m_shapeKeys[1] = shapekey;
			output.m_shapeKeys[2] = HK_INVALID_SHAPE_KEY;
			return true;
		}

		// Advance raypos and boxpos to next cube
		RayCastCube(raypos, raydir, boxpos);

		// Validate ray distance
		vraypos.set(raypos[0], raypos[1], raypos[2]);
		vraypos.sub(transformed_vfrom);
		if(vraypos.length3() > maxdist)
			return false;
	} while(boxpos[0] >= 0 && boxpos[0] < (int)chunksize[0] && boxpos[1] >= 0 && boxpos[1] < (int)chunksize[1] && boxpos[2] >= 0 && boxpos[2] < (int)chunksize[2]);

	return false;
}

void BoxedLevelChunkShape::queryAabb(const hkAabb& aabb, hkArray<hkpShapeKey>& hits) const
{
	int xmin = max(0, (int)((float)aabb.m_min.getComponent(0) - bounds_min.getComponent(0))), xmax = min((int)chunksize[0] - 1, (int)ceil((float)aabb.m_max.getComponent(0) - bounds_min.getComponent(0)));
	int ymin = max(0, (int)((float)aabb.m_min.getComponent(1) - bounds_min.getComponent(1))), ymax = min((int)chunksize[1] - 1, (int)ceil((float)aabb.m_max.getComponent(1) - bounds_min.getComponent(1)));
	int zmin = max(0, (int)((float)aabb.m_min.getComponent(2) - bounds_min.getComponent(2))), zmax = min((int)chunksize[2] - 1, (int)ceil((float)aabb.m_max.getComponent(2) - bounds_min.getComponent(2)));
	hkpShapeKey newkey;
	for(int z = zmin; z <= zmax; z++)
		for(int y = ymin; y <= ymax; y++)
			for(int x = xmin; x <= xmax; x++)
				if(container.chunk->boxmask[x * (chunksize[1] * chunksize[2]) + z * chunksize[1] + y].IsBox())
				{
					newkey = x * (chunksize[1] * chunksize[2]) + z * chunksize[1] + y;
					hits.append(&newkey, 1);
				}

	/* // Return all child shapes (for debugging)
	hkpShapeKey newkey;
	for(UINT z = 0; z < chunksize[2]; z++)
		for(UINT y = 0; y < chunksize[1]; y++)
			for(UINT x = 0; x < chunksize[0]; x++)
				if(container.boxmask[x * (chunksize[1] * chunksize[2]) + z * chunksize[1] + y])
				{
					newkey = x * (chunksize[1] * chunksize[2]) + z * chunksize[1] + y;
					hits.append(&newkey, 1);
				}*/
}

hkUint32 BoxedLevelChunkShape::queryAabbImpl(HKP_SHAPE_VIRTUAL_THIS const hkAabb& aabb, hkpShapeKey* hits, int maxNumKeys) HKP_SHAPE_VIRTUAL_CONST
{
	hkpShapeKey* hitsptr = hits;

	int xmin = max(0, (int)((float)aabb.m_min.getComponent(0) - bounds_min.getComponent(0))), xmax = min((int)chunksize[0] - 1, (int)ceil((float)aabb.m_max.getComponent(0) - bounds_min.getComponent(0)));
	int ymin = max(0, (int)((float)aabb.m_min.getComponent(1) - bounds_min.getComponent(1))), ymax = min((int)chunksize[1] - 1, (int)ceil((float)aabb.m_max.getComponent(1) - bounds_min.getComponent(1)));
	int zmin = max(0, (int)((float)aabb.m_min.getComponent(2) - bounds_min.getComponent(2))), zmax = min((int)chunksize[2] - 1, (int)ceil((float)aabb.m_max.getComponent(2) - bounds_min.getComponent(2)));
	register hkpShapeKey newkey;
	for(int z = zmin; z <= zmax; z++)
		for(int y = ymin; y <= ymax; y++)
			for(int x = xmin; x <= xmax; x++)
				if(container.chunk->boxmask[newkey = (hkpShapeKey)(x * (chunksize[1] * chunksize[2]) + z * chunksize[1] + y)].IsBox())
				{
					if(maxNumKeys-- > 0)
						*hitsptr = newkey;
					hitsptr++;
				}

	/* // Return all child shapes (for debugging)
	register hkpShapeKey newkey;
	for(UINT z = 0; z < chunksize[2]; z++)
		for(UINT y = 0; y < chunksize[1]; y++)
			for(UINT x = 0; x < chunksize[0]; x++)
				if(container.boxmask[newkey = (hkpShapeKey)(x * (chunksize[1] * chunksize[2]) + z * chunksize[1] + y)])
				{
					if(maxNumKeys-- > 0)
						*hitsptr = newkey;
					hitsptr++;
				}*/

	return hitsptr - hits;
}

#pragma endregion




#pragma region BoxedLevelShapeContainer

BoxedLevelShapeContainer::BoxedLevelShapeContainer(const UINT (&chunksize)[3])
{
	memcpy(this->chunksize, chunksize, 3 * sizeof(UINT));
}

void BoxedLevelShapeContainer::AssignChunk(BoxedLevelChunk* chunk)
{
	ChunkIndex cidx = ChunkIndex(chunk->chunkpos);

	// Check whether the chunk is already assigned
	std::map<ChunkIndex, BoxedLevelChunkShape*>::iterator pair = chunks.find(cidx);
	if(pair != chunks.end())
	{
		Result::PrintLogMessage("Chunk already bound to a buffer");
		return;
	}

	chunks[cidx] = new BoxedLevelChunkShape(parent, chunksize, chunk);
}
void BoxedLevelShapeContainer::UnassignChunk(BoxedLevelChunk* chunk)
{
	std::map<ChunkIndex, BoxedLevelChunkShape*>::iterator pair = chunks.find(ChunkIndex(chunk->chunkpos));
	if(pair != chunks.end())
	{
		delete pair->second;
		chunks.erase(pair);
	}
}

const hkpShape* BoxedLevelShapeContainer::getChildShape(hkpShapeKey key, hkpShapeBuffer& buffer) const
{
	return (hkpShape*)key;
}
const hkpShape* BoxedLevelShapeContainer::getChildShape(hkpShapeKey key) const
{
	return (hkpShape*)key;
}

BoxedLevelShapeContainer::~BoxedLevelShapeContainer()
{
	std::map<ChunkIndex, BoxedLevelChunkShape*>::iterator iter;
	LIST_FOREACH(chunks, iter)
		delete iter->second;
	chunks.clear();
}

#pragma endregion

#pragma region BoxedLevelShape

BoxedLevelShape::BoxedLevelShape(const UINT (&chunksize)[3], const Vector3& maxlevelsize)
	: hkpBvTreeShape(hkcdShapeType::USER0, hkpBvTreeShape::BVTREE_USER), container(chunksize), bounds_min(-0.5f, -0.5f, -0.5f), numboxtypes(0), boxshapes(NULL), numboxshapes(0)
{
	container.parent = this;
	memcpy(this->chunksize, chunksize, 3 * sizeof(UINT));
	bounds_max = bounds_min;
	bounds_max.add(maxlevelsize);

	boxrotations[0].setIdentity();
	boxrotations[1].setAxisAngle(hkVector4(0.0f, 1.0f, 0.0f), PI / 2.0f);
	boxrotations[2].setAxisAngle(hkVector4(0.0f, 1.0f, 0.0f), PI);
	boxrotations[3].setAxisAngle(hkVector4(0.0f, 1.0f, 0.0f), -PI / 2.0f);
}

void BoxedLevelShape::SetBoxTypes(BoxedLevelBoxType* boxtypes, UINT32 numtypes)
{
	this->boxtypes.btarray = (const BYTE*)boxtypes;
	this->boxtypes.btsize = sizeof(BoxedLevelBoxType);
	this->boxtypes.btshapeoffset = offsetof(BoxedLevelBoxType, shape);
	this->numboxtypes = numtypes;
}

Result BoxedLevelShape::CreateBoxShapes(HvkShapeDesc** boxshapes, UINT32 numboxshapes)
{
	Result rlt;

	this->numboxshapes = 0;

	hkTransform trans;
	trans.setIdentity();
	CHKALLOC(this->boxshapes = (hkpTransformShape**)new LPVOID[numboxshapes]);
	for(UINT32 i = 0; i < numboxshapes; i++)
	{
		hkpShape* boxshape;
		CHKRESULT(((Havok*)&*hvk)->CreateShape(boxshapes[i], &boxshape));
		this->boxshapes[i] = new hkpTransformShape(boxshape, trans);
		boxshape->removeReference();
	}
	this->numboxshapes = numboxshapes;

	return R_OK;
}

void BoxedLevelShape::BoxPosFromShapeKey(UINT32 boxshapekey, UINT32 (*boxpos)[3])
{
	// Format: [[[x]y]z]
	/*(*boxpos)[1] = boxshapekey / chunksize[0];
	(*boxpos)[0] = boxshapekey - (*boxpos)[1] * chunksize[0];
	(*boxpos)[2] = (*boxpos)[1] / chunksize[1];
	(*boxpos)[1] -= (*boxpos)[2] * chunksize[1];*/

	// Format: [[[y]z]x] (binvox format)
	(*boxpos)[2] = boxshapekey / chunksize[1];
	(*boxpos)[1] = boxshapekey - (*boxpos)[2] * chunksize[1];
	(*boxpos)[0] = (*boxpos)[2] / chunksize[2];
	(*boxpos)[2] -= (*boxpos)[0] * chunksize[2];
}

void BoxedLevelShape::getAabb(const hkTransform& localToWorld, hkReal tolerance, hkAabb& aabbOut) const
{
	aabbOut.m_min.setTransformedPos(localToWorld, bounds_min);
	aabbOut.m_max.setTransformedPos(localToWorld, bounds_max);
}

hkBool BoxedLevelShape::castRay(const hkpShapeRayCastInput& input, hkpShapeRayCastOutput& output) const
{
	float m_from[] = {input.m_from(0), input.m_from(1), input.m_from(2)};
	hkVector4 vraydir(input.m_to);
	vraydir.sub(input.m_from);
	float maxsquareddist = vraydir.lengthSquared3();
	vraydir.normalize<3>();
	float raydir[] = {vraydir(0), vraydir(1), vraydir(2)};
	float raypos[3];

//Vector3 v0(input.m_from(0), input.m_from(1), input.m_from(2));
//Vector3 v1(input.m_to(0), input.m_to(1), input.m_to(2));
//(*d3d->GetOutputWindows().begin())->DrawLine(v0, v1, Color(0xFF0000FF), Color(0xFF8080FF));

	struct HitChunk
	{
		BoxedLevelChunkShape* chunk;
		float squaredhitdist;
		HitChunk(BoxedLevelChunkShape* chunk, float squaredhitdist) : chunk(chunk), squaredhitdist(squaredhitdist) {}
	};
	struct HitChunkCompare
	{
		bool operator()(const HitChunk& first, const HitChunk& second) const {return first.squaredhitdist < second.squaredhitdist;}
	};
	PriorityQueue<HitChunk, HitChunkCompare> hitchunks;

	// Find chunks intersecting the ray using IntersectRayAABB() and insert them into the sorted list hitchunks
	std::map<ChunkIndex, BoxedLevelChunkShape*>::const_iterator iter;
	LIST_FOREACH(container.chunks, iter)
	{
		float bounds_min[] = {iter->second->bounds_min(0), iter->second->bounds_min(1), iter->second->bounds_min(2)};
		float bounds_max[] = {iter->second->bounds_max(0), iter->second->bounds_max(1), iter->second->bounds_max(2)};
		if(IntersectRayAABB(bounds_min, bounds_max, m_from, raydir, raypos)) // If ray intersects chunk
		{
			hkVector4 vraypos(raypos[0], raypos[1], raypos[2]);
			vraypos.sub(input.m_from);
			float squareddist = vraypos.lengthSquared3();
			if(squareddist <= maxsquareddist)
				hitchunks.insert(HitChunk(iter->second, squareddist));
		}
	}

	// Traverse hitchunks from closest to furthest and return true as soon as a hit inside a chunk is encountered
	PriorityQueue<HitChunk>::const_iterator hititer;
	LIST_FOREACH(hitchunks, hititer)
		if(hititer->chunk->castRay(input, output))
		{
			output.m_shapeKeys[0] = (hkpShapeKey)hititer->chunk;
			return true;
		}


	return false;
}

void BoxedLevelShape::queryAabb(const hkAabb& aabb, hkArray<hkpShapeKey>& hits) const
{
	int aabb_min[] = {max(0, (int)((float)aabb.m_min.getComponent(0) + 0.5f)), max(0, (int)((float)aabb.m_min.getComponent(1) + 0.5f)), max(0, (int)((float)aabb.m_min.getComponent(2) + 0.5f))};
	int aabb_max[] = {(int)ceil((float)aabb.m_max.getComponent(0) + 0.5f),	   (int)ceil((float)aabb.m_max.getComponent(1) + 0.5f),		(int)ceil((float)aabb.m_max.getComponent(2) + 0.5f)};
	int cmin[] = {aabb_min[0] / (int)chunksize[0], aabb_min[1] / (int)chunksize[1], aabb_min[2] / (int)chunksize[2]};
	int cmax[] = {aabb_max[0] / (int)chunksize[0], aabb_max[1] / (int)chunksize[1], aabb_max[2] / (int)chunksize[2]};
	hkpShapeKey newkey;
	for(int cz = cmin[2]; cz <= cmax[2]; cz++)
		for(int cy = cmin[1]; cy <= cmax[1]; cy++)
			for(int cx = cmin[0]; cx <= cmax[0]; cx++)
			{
				std::map<ChunkIndex, BoxedLevelChunkShape*>::const_iterator pair = container.chunks.find(ChunkIndex(cx, cy, cz));
				if(pair != container.chunks.end() && pair->second->container.chunk->numboxes)
				{
					newkey = (hkpShapeKey)pair->second;
					hits.append(&newkey, 1);
				}
			}

	/* // Return all child shapes (for debugging)
	hkpShapeKey newkey;
	std::map<ChunkIndex, BoxedLevelChunkShape*>::const_iterator iter;
	LIST_FOREACH(container.chunks, iter)
	{
		newkey = (hkpShapeKey)iter->second;
		hits.append(&newkey, 1);
	}*/
}

hkUint32 BoxedLevelShape::queryAabbImpl(HKP_SHAPE_VIRTUAL_THIS const hkAabb& aabb, hkpShapeKey* hits, int maxNumKeys) HKP_SHAPE_VIRTUAL_CONST
{
	hkpShapeKey* hitsptr = hits;

	int aabb_min[] = {max(0, (int)((float)aabb.m_min.getComponent(0) + 0.5f)), max(0, (int)((float)aabb.m_min.getComponent(1) + 0.5f)), max(0, (int)((float)aabb.m_min.getComponent(2) + 0.5f))};
	int aabb_max[] = {(int)ceil((float)aabb.m_max.getComponent(0) + 0.5f),	   (int)ceil((float)aabb.m_max.getComponent(1) + 0.5f),		(int)ceil((float)aabb.m_max.getComponent(2) + 0.5f)};
	int cmin[] = {aabb_min[0] / (int)chunksize[0], aabb_min[1] / (int)chunksize[1], aabb_min[2] / (int)chunksize[2]};
	int cmax[] = {aabb_max[0] / (int)chunksize[0], aabb_max[1] / (int)chunksize[1], aabb_max[2] / (int)chunksize[2]};
	for(int cz = cmin[2]; cz <= cmax[2]; cz++)
		for(int cy = cmin[1]; cy <= cmax[1]; cy++)
			for(int cx = cmin[0]; cx <= cmax[0]; cx++)
			{
				std::map<ChunkIndex, BoxedLevelChunkShape*>::const_iterator pair = container.chunks.find(ChunkIndex(cx, cy, cz));
				if(pair != container.chunks.end() && pair->second->container.chunk->numboxes)
				{
					if(maxNumKeys-- > 0)
						*hitsptr = (hkpShapeKey)pair->second;
					hitsptr++;
				}
			}

	/* // Return all child shapes (for debugging)
	std::map<ChunkIndex, BoxedLevelChunkShape*>::const_iterator iter;
	LIST_FOREACH(container.chunks, iter)
	{
		if(maxNumKeys-- > 0)
			*hitsptr = (hkpShapeKey)iter->second;
		hitsptr++;
	}*/
	return hitsptr - hits;
}

BoxedLevelShape::~BoxedLevelShape()
{
	if(boxshapes)
		delete[] boxshapes;
}

#pragma endregion