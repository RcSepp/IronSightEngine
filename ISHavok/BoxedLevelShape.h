#include <Physics/Collide/Shape/Misc/Transform/hkpTransformShape.h>

struct ChunkIndex
{
	UINT x, y, z;
	ChunkIndex() {}
	ChunkIndex(UINT x, UINT y, UINT z) : x(x), y(y), z(z) {}
	ChunkIndex(const UINT (&chunkpos)[3]) : x(chunkpos[0]), y(chunkpos[1]), z(chunkpos[2]) {}
	bool operator<(const ChunkIndex& other) const
		{return this->x < other.x || (this->x == other.x && (this->y < other.y || (this->y == other.y && this->z < other.z)));}
};

class BoxedLevelChunkShapeContainer : public hkpShapeContainer
{
public:
	BoxedLevelShape* parent;
	UINT chunksize[3];
	const Vector3 chunkpos;
	BoxedLevelChunk* chunk;

	BoxedLevelChunkShapeContainer(BoxedLevelShape* parent, const UINT (&chunksize)[3], BoxedLevelChunk* chunk);
	~BoxedLevelChunkShapeContainer();
	const hkpShape* getChildShape(hkpShapeKey key, hkpShapeBuffer& buffer) const;
	const hkpShape* getChildShape(hkpShapeKey key) const;
	int getNumChildShapes() const {return chunk->numboxes;}
	hkpShapeKey getFirstKey() const {Result::PrintLogMessage("BoxedLevelChunkShapeContainer::getFirstKey() NOT IMPLEMENTED"); return 0;}
	hkpShapeKey getNextKey(hkpShapeKey oldKey) const {Result::PrintLogMessage("BoxedLevelChunkShapeContainer::getNextKey() NOT IMPLEMENTED"); return 0;}
	//hkUint32 getCollisionFilterInfo() const {Result::PrintLogMessage("BoxedLevelChunkShapeContainer::getNextKey() NOT IMPLEMENTED"); return 0;}
	//bool isWeldingEnabled() const {Result::PrintLogMessage("NOT IMPLEMENTED"); return false;}
};

class BoxedLevelChunkShape : public hkpBvTreeShape
{
public:
	BoxedLevelChunkShapeContainer container;
	UINT chunksize[3];
	hkVector4 bounds_min, bounds_max;

	BoxedLevelChunkShape(BoxedLevelShape* parent, const UINT (&chunksize)[3], BoxedLevelChunk* chunk);
	void getAabb(const hkTransform& localToWorld, hkReal tolerance, hkAabb& aabbOut) const;
	hkBool castRay(const hkpShapeRayCastInput& input, hkpShapeRayCastOutput& output) const;
	void queryAabb(const hkAabb& aabb, hkArray<hkpShapeKey>& hits) const;
	hkUint32 queryAabbImpl(HKP_SHAPE_VIRTUAL_THIS const hkAabb& aabb, hkpShapeKey* hits, int maxNumKeys) HKP_SHAPE_VIRTUAL_CONST;
	const hkpShapeContainer* getContainer() const {return &container;}

	void castAabbImpl(HKP_SHAPE_VIRTUAL_THIS const hkAabb& from, hkVector4Parameter to, hkpAabbCastCollector& collector) HKP_SHAPE_VIRTUAL_CONST {Result::PrintLogMessage("BoxedLevelChunkShape::castAabbImpl() NOT IMPLEMENTED");}
	hkReal getMaximumProjection(const hkVector4& direction) const {Result::PrintLogMessage("BoxedLevelChunkShape::getMaximumProjection() NOT IMPLEMENTED"); return 0.0f;}
	int calcSizeForSpu(const CalcSizeForSpuInput& input, int spuBufferSizeLeft) const {Result::PrintLogMessage("BoxedLevelChunkShape::calcSizeForSpu() NOT IMPLEMENTED"); return 0;}
	void castRayWithCollector(const hkpShapeRayCastInput& input, const hkpCdBody& cdBody, hkpRayHitCollector& collector) const {Result::PrintLogMessage("BoxedLevelChunkShape::castRayWithCollector() NOT IMPLEMENTED");}
	void getSupportingVertex(hkVector4Parameter direction, hkcdVertex& supportingVertexOut) const {Result::PrintLogMessage("BoxedLevelChunkShape::getSupportingVertex() NOT IMPLEMENTED");}
	void convertVertexIdsToVertices(const hkpVertexId* ids, int numIds, hkcdVertex* vertexArrayOut) const {Result::PrintLogMessage("BoxedLevelChunkShape::convertVertexIdsToVertices() NOT IMPLEMENTED");}
	void getCentre(hkVector4& centreOut) const {Result::PrintLogMessage("BoxedLevelChunkShape::getCentre() NOT IMPLEMENTED");}
	int getNumCollisionSpheres() const {Result::PrintLogMessage("BoxedLevelChunkShape::getNumCollisionSpheres() NOT IMPLEMENTED"); return 0;}
	const hkSphere* getCollisionSpheres(hkSphere* sphereBuffer) const {Result::PrintLogMessage("BoxedLevelChunkShape::getCollisionSpheres() NOT IMPLEMENTED"); return NULL;}
	int weldContactPoint(hkpVertexId* featurePoints, hkUint8& numFeaturePoints, hkVector4& contactPointWs, const hkTransform* thisObjTransform,
						 const class hkpConvexShape* collidingConvexShape, const hkTransform* collidingTransform, hkVector4& separatingNormalInOut) const {Result::PrintLogMessage("BoxedLevelChunkShape::weldContactPoint() NOT IMPLEMENTED"); return 0;}
	const hkClass* getClassType() const {Result::PrintLogMessage("BoxedLevelChunkShape::getClassType() NOT IMPLEMENTED"); return HK_NULL;}
};

class BoxedLevelShapeContainer : public hkpShapeContainer
{
public:
	BoxedLevelShape* parent;
	UINT chunksize[3];
	std::map<ChunkIndex, BoxedLevelChunkShape*> chunks;

	BoxedLevelShapeContainer(const UINT (&chunksize)[3]);
	~BoxedLevelShapeContainer();
	void AssignChunk(BoxedLevelChunk* chunk);
	void UnassignChunk(BoxedLevelChunk* chunk);
	const hkpShape* getChildShape(hkpShapeKey key, hkpShapeBuffer& buffer) const;
	const hkpShape* getChildShape(hkpShapeKey key) const;
	int getNumChildShapes() const {return chunks.size();}
	hkpShapeKey getFirstKey() const {Result::PrintLogMessage("hkpShapeContainer::getFirstKey() NOT IMPLEMENTED"); return 0;}
	hkpShapeKey getNextKey(hkpShapeKey oldKey) const {Result::PrintLogMessage("hkpShapeContainer::getNextKey() NOT IMPLEMENTED"); return 0;}
	//bool isWeldingEnabled() const {Result::PrintLogMessage("NOT IMPLEMENTED"); return false;}
};

class BoxedLevelShape : public hkpBvTreeShape
{
public:
	BoxedLevelShapeContainer container;
	UINT chunksize[3];
	hkVector4 bounds_min, bounds_max;
	hkRotation boxrotations[4];

	// Box types (operator[] returns index into shapes)
	struct BoxTypes
	{
		const BYTE* btarray;
		UINT32 btsize, btshapeoffset;
		BYTE operator[](UINT32 idx) const {return btarray[idx * btsize + btshapeoffset];}
	} boxtypes;
	UINT32 numboxtypes;

	// Box shapes
	hkpTransformShape** boxshapes;
	UINT32 numboxshapes;

	BoxedLevelShape(const UINT (&chunksize)[3], const Vector3& maxlevelsize);
	~BoxedLevelShape();
	void AssignChunk(BoxedLevelChunk* chunk) {container.AssignChunk(chunk);}
	void UnassignChunk(BoxedLevelChunk* chunk) {container.UnassignChunk(chunk);}
	void SetBoxTypes(BoxedLevelBoxType* boxtypes, UINT32 numtypes);
	Result CreateBoxShapes(HvkShapeDesc** boxshapes, UINT32 numboxshapes);
	BoxedLevelChunk* ChunkFromShapeKey(UINT32 chunkshapekey) {return ((BoxedLevelChunkShape*)chunkshapekey)->container.chunk;}
	void BoxPosFromShapeKey(UINT32 boxshapekey, UINT32 (*boxpos)[3]);
	void getAabb(const hkTransform& localToWorld, hkReal tolerance, hkAabb& aabbOut) const;
	hkBool castRay(const hkpShapeRayCastInput& input, hkpShapeRayCastOutput& output) const;
	void queryAabb(const hkAabb& aabb, hkArray<hkpShapeKey>& hits) const;
	hkUint32 queryAabbImpl(HKP_SHAPE_VIRTUAL_THIS const hkAabb& aabb, hkpShapeKey* hits, int maxNumKeys) HKP_SHAPE_VIRTUAL_CONST;
	const hkpShapeContainer* getContainer() const {return &container;}

	void castAabbImpl(HKP_SHAPE_VIRTUAL_THIS const hkAabb& from, hkVector4Parameter to, hkpAabbCastCollector& collector) HKP_SHAPE_VIRTUAL_CONST {Result::PrintLogMessage("BoxedLevelShape::castAabbImpl() NOT IMPLEMENTED");}
	hkReal getMaximumProjection(const hkVector4& direction) const {Result::PrintLogMessage("BoxedLevelShape::getMaximumProjection() NOT IMPLEMENTED"); return 0.0f;}
	int calcSizeForSpu(const CalcSizeForSpuInput& input, int spuBufferSizeLeft) const {Result::PrintLogMessage("BoxedLevelShape::calcSizeForSpu() NOT IMPLEMENTED"); return 0;}
	void castRayWithCollector(const hkpShapeRayCastInput& input, const hkpCdBody& cdBody, hkpRayHitCollector& collector) const {Result::PrintLogMessage("BoxedLevelShape::castRayWithCollector() NOT IMPLEMENTED");}
	void getSupportingVertex(hkVector4Parameter direction, hkcdVertex& supportingVertexOut) const {Result::PrintLogMessage("BoxedLevelShape::getSupportingVertex() NOT IMPLEMENTED");}
	void convertVertexIdsToVertices(const hkpVertexId* ids, int numIds, hkcdVertex* vertexArrayOut) const {Result::PrintLogMessage("BoxedLevelShape::convertVertexIdsToVertices() NOT IMPLEMENTED");}
	void getCentre(hkVector4& centreOut) const {Result::PrintLogMessage("BoxedLevelShape::getCentre() NOT IMPLEMENTED");}
	int getNumCollisionSpheres() const {Result::PrintLogMessage("BoxedLevelShape::getNumCollisionSpheres() NOT IMPLEMENTED"); return 0;}
	const hkSphere* getCollisionSpheres(hkSphere* sphereBuffer) const {Result::PrintLogMessage("BoxedLevelShape::getCollisionSpheres() NOT IMPLEMENTED"); return NULL;}
	int weldContactPoint(hkpVertexId* featurePoints, hkUint8& numFeaturePoints, hkVector4& contactPointWs, const hkTransform* thisObjTransform,
						 const class hkpConvexShape* collidingConvexShape, const hkTransform* collidingTransform, hkVector4& separatingNormalInOut) const {Result::PrintLogMessage("BoxedLevelShape::weldContactPoint() NOT IMPLEMENTED"); return 0;}
	const hkClass* getClassType() const {Result::PrintLogMessage("BoxedLevelShape::getClassType() NOT IMPLEMENTED"); return HK_NULL;}
};