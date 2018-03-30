#include <ISTypes.h>
#include <ISMath.h>
#include "Octree.h"
#include "RayCastParams.h"
#include "TriBoxTest.h"

//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Build up an octree for the given mesh
//-----------------------------------------------------------------------------
Result Octree::Create(Array<const Vector3> positions, Array<const unsigned int> indices, unsigned int numfaces, int maxdepth, unsigned int maxfaces)
{
	// Compute mesh bounds
	Vector3 vmin(FLOAT_INFINITY, FLOAT_INFINITY, FLOAT_INFINITY), vmax(-FLOAT_INFINITY, -FLOAT_INFINITY, -FLOAT_INFINITY);
	for(unsigned int i = 0; i < 3 * numfaces; i++)
	{
		Vec3Min(&vmin, &vmin, &positions[indices[i]]);
		Vec3Max(&vmax, &vmax, &positions[indices[i]]);
	}

	return Create(positions, indices, numfaces, &vmin, &vmax, maxdepth, maxfaces);
}

Result Octree::Create(Array<const Vector3> positions, Array<const unsigned int> indices, unsigned int numfaces, const Vector3* vmin, const Vector3* vmax, int maxdepth, unsigned int maxfaces)
{
	Result rlt;

	this->numfaces = numfaces;

	unsigned int* faces = new unsigned int[numfaces];
	CHKALLOC(faces);
for(unsigned int i = 0; i < numfaces; i++)
	faces[i] = i;
	this->positions = positions;
	this->indices = indices;

	// Build octree
	topnode = new Node();
	CHKALLOC(topnode);
	memset(topnode->childnodes, 0, 8 * sizeof(Node*));
	topnode->facelist = faces;
	topnode->facecount = numfaces;
	topnode->vmin = *vmin;
	topnode->vmax = *vmax;

	if(topnode->facecount <= maxfaces || maxdepth == 0)
		return R_OK;

	Vector3 vmid;
	Vec3Lerp(&vmid, vmin, vmax, 0.5f);

	Vector3 cvmin, cvmax;
	CHKRESULT(CreateChildNode(&topnode->childnodes[0], topnode->facelist, topnode->facecount,
							  Vec3Set(&cvmin, vmin->x, vmin->y, vmin->z),
							  Vec3Set(&cvmax, vmid.x, vmid.y, vmid.z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&topnode->childnodes[1], topnode->facelist, topnode->facecount,
							  Vec3Set(&cvmin, vmin->x, vmin->y, vmid.z),
							  Vec3Set(&cvmax, vmid.x, vmid.y, vmax->z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&topnode->childnodes[2], topnode->facelist, topnode->facecount,
							  Vec3Set(&cvmin, vmin->x, vmid.y, vmin->z),
							  Vec3Set(&cvmax, vmid.x, vmax->y, vmid.z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&topnode->childnodes[3], topnode->facelist, topnode->facecount,
							  Vec3Set(&cvmin, vmin->x, vmid.y, vmid.z),
							  Vec3Set(&cvmax, vmid.x, vmax->y, vmax->z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&topnode->childnodes[4], topnode->facelist, topnode->facecount,
							  Vec3Set(&cvmin, vmid.x, vmin->y, vmin->z),
							  Vec3Set(&cvmax, vmax->x, vmid.y, vmid.z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&topnode->childnodes[5], topnode->facelist, topnode->facecount,
							  Vec3Set(&cvmin, vmid.x, vmin->y, vmid.z),
							  Vec3Set(&cvmax, vmax->x, vmid.y, vmax->z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&topnode->childnodes[6], topnode->facelist, topnode->facecount,
							  Vec3Set(&cvmin, vmid.x, vmid.y, vmin->z),
							  Vec3Set(&cvmax, vmax->x, vmax->y, vmid.z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&topnode->childnodes[7], topnode->facelist, topnode->facecount,
							  Vec3Set(&cvmin, vmid.x, vmid.y, vmid.z),
							  Vec3Set(&cvmax, vmax->x, vmax->y, vmax->z), maxdepth - 1, maxfaces));

	// Cleanup
	topnode->facecount = 0;
	delete[] topnode->facelist;
	topnode->facelist = NULL;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateChildNode()
// Desc: Determine facelist for a new childnode
//-----------------------------------------------------------------------------
Result Octree::CreateChildNode(Node** thisnode, const unsigned int* parentfacelist, unsigned int parentfacecount,
							   const Vector3* vmin, const Vector3* vmax, int maxdepth, unsigned int maxfaces)
{
	Result rlt;

	Node* newnode = new Node();
	(*thisnode) = newnode;
	CHKALLOC(newnode);

	memset(newnode->childnodes, 0, 8 * sizeof(Node*));

	newnode->facelist = new unsigned int[parentfacecount];
	CHKALLOC(newnode->facelist);

	newnode->facecount = 0;
	newnode->vmin = *vmin;
	newnode->vmax = *vmax;

	Vector3 vcenter, vhalf;
	Vec3Add(&vcenter, vmin, vmax);
	Vec3Scale(&vcenter, &vcenter, 0.5f);
	Vec3Sub(&vhalf, vmax, vmin);
	Vec3Scale(&vhalf, &vhalf, 0.5f + FLOAT_TOLERANCE); // Expand covered area to include faces on the bounding box's surface

	for(unsigned int i = 0; i < parentfacecount; i++)
	{
		Vector3 v0 = positions[indices[parentfacelist[i] * 3 + 0]];
		Vector3 v1 = positions[indices[parentfacelist[i] * 3 + 1]];
		Vector3 v2 = positions[indices[parentfacelist[i] * 3 + 2]];

		if(TriBoxOverlap(vcenter, vhalf, v0, v1, v2))
		{
			newnode->facelist[newnode->facecount] = parentfacelist[i];
			newnode->facecount++;
		}
	}
	if(newnode->facecount == 0)
	{
		// Empty node can be removed
		delete[] newnode->facelist;
		delete newnode;
		*thisnode = NULL;
		return R_OK;
	}

	if(newnode->facecount <= maxfaces || maxdepth == 0)
	{
		newnode->facelist = (unsigned int*)realloc(newnode->facelist, newnode->facecount * sizeof(unsigned int));
		return R_OK;
	}

	Vector3 vmid;
	Vec3Lerp(&vmid, vmin, vmax, 0.5f);

	Vector3 cvmin, cvmax;
	CHKRESULT(CreateChildNode(&newnode->childnodes[0], newnode->facelist, newnode->facecount,
							  Vec3Set(&cvmin, vmin->x, vmin->y, vmin->z),
							  Vec3Set(&cvmax, vmid.x, vmid.y, vmid.z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&newnode->childnodes[1], newnode->facelist, newnode->facecount,
							  Vec3Set(&cvmin, vmin->x, vmin->y, vmid.z),
							  Vec3Set(&cvmax, vmid.x, vmid.y, vmax->z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&newnode->childnodes[2], newnode->facelist, newnode->facecount,
							  Vec3Set(&cvmin, vmin->x, vmid.y, vmin->z),
							  Vec3Set(&cvmax, vmid.x, vmax->y, vmid.z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&newnode->childnodes[3], newnode->facelist, newnode->facecount,
							  Vec3Set(&cvmin, vmin->x, vmid.y, vmid.z),
							  Vec3Set(&cvmax, vmid.x, vmax->y, vmax->z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&newnode->childnodes[4], newnode->facelist, newnode->facecount,
							  Vec3Set(&cvmin, vmid.x, vmin->y, vmin->z),
							  Vec3Set(&cvmax, vmax->x, vmid.y, vmid.z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&newnode->childnodes[5], newnode->facelist, newnode->facecount,
							  Vec3Set(&cvmin, vmid.x, vmin->y, vmid.z),
							  Vec3Set(&cvmax, vmax->x, vmid.y, vmax->z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&newnode->childnodes[6], newnode->facelist, newnode->facecount,
							  Vec3Set(&cvmin, vmid.x, vmid.y, vmin->z),
							  Vec3Set(&cvmax, vmax->x, vmax->y, vmid.z), maxdepth - 1, maxfaces));
	CHKRESULT(CreateChildNode(&newnode->childnodes[7], newnode->facelist, newnode->facecount,
							  Vec3Set(&cvmin, vmid.x, vmid.y, vmid.z),
							  Vec3Set(&cvmax, vmax->x, vmax->y, vmax->z), maxdepth - 1, maxfaces));

	// Cleanup
	newnode->facecount = 0;
	delete[] newnode->facelist;
	newnode->facelist = NULL;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: RayCastNode()
// Desc: Collision detection for a single octree node
//-----------------------------------------------------------------------------
void Octree::RayCastNode(Node *n, const Vector3 *pos, const Vector3 *dir, Intersection::RAY_TRIANGLE_INTERSECTION_FUNCTION isctfct, float *dist, Vector2 *uv, unsigned int *hitidx) const
{
	if(!RayIntersectsBox(pos, dir, &n->vmin, &n->vmax))
		return;

	const Vector3 *v0, *v1, *v2;
	Vector2 newuv;
	float newdist;

	for(unsigned int i = 0; i < n->facecount; i++)
	{
		v0 = &positions[indices[n->facelist[i] * 3 + 0]];
		v1 = &positions[indices[n->facelist[i] * 3 + 1]];
		v2 = &positions[indices[n->facelist[i] * 3 + 2]];

		newdist = isctfct(pos, dir, v0, v1, v2, &newuv);
		if(newdist < *dist)
		{
			*dist = newdist;
			*uv = newuv;
			*hitidx = n->facelist[i];
		}
	}

	for(int i = 0; i < 8; i++)
		if(n->childnodes[i])
			RayCastNode(n->childnodes[i], pos, dir, isctfct, dist, uv, hitidx);
}

//-----------------------------------------------------------------------------
// Name: RayCast()
// Desc: Collision detection with the octrees mesh
//-----------------------------------------------------------------------------
float Octree::RayCast(const Vector3 *pos, const Vector3 *dir, Intersection::RAY_TRIANGLE_INTERSECTION_FUNCTION isctfct, Vector2 *uv, unsigned int *hitidx) const
{
	float dist = FLOAT_INFINITY;

	RayCastNode(topnode, pos, dir, isctfct, &dist, uv, hitidx);
	return dist;
}

inline bool IsInsideBox(BatchRayCastParams& batchparams, int x, int y, const Vector3* boxmin, const Vector3* boxmax)
{
	batchparams.GetRayDir(&batchparams.params.transraydir, x, y);
	if(batchparams.currentinvtransform)
		Vec3TransformNormal(&batchparams.params.transraydir, &batchparams.params.transraydir, batchparams.currentinvtransform);
	return RayIntersectsBox(&batchparams.params.transraypos, &batchparams.params.transraydir, boxmin, boxmax);
}

//-----------------------------------------------------------------------------
// Name: BatchRayCastNode()
// Desc: Multiple ray collision detection for a single octree node
//-----------------------------------------------------------------------------
void Octree::BatchRayCastNode(Node *n, BatchRayCastParams& batchparams, Rect<int> region, bool inbox[4]) const
{
	/*if(inbox[0] && inbox[1] && inbox[2] && inbox[3])
	{*/
		const Vector3 *v0, *v1, *v2;
		Vector2 newuv;
		float newdist;
		BatchRayCastParams::Hit* hit;

		for(int y = 0; y < batchparams.resolution.height; y++)
			for(int x = 0; x < batchparams.resolution.width; x++)
			{
				batchparams.GetRayDir(&batchparams.params.transraydir, x, y);
				if(batchparams.currentinvtransform)
					Vec3TransformNormal(&batchparams.params.transraydir, &batchparams.params.transraydir, batchparams.currentinvtransform);

				for(unsigned int i = 0; i < n->facecount; i++)
				{
					v0 = &positions[indices[n->facelist[i] * 3 + 0]];
					v1 = &positions[indices[n->facelist[i] * 3 + 1]];
					v2 = &positions[indices[n->facelist[i] * 3 + 2]];

					newdist = batchparams.params.RayTriangle(&batchparams.params.transraypos, &batchparams.params.transraydir, v0, v1, v2, &newuv);
					if(batchparams.currenttransform && newdist != FLOAT_INFINITY)
					{
						// Transform newhitdist
						Vec3Scale(&batchparams.params.hitnml, &batchparams.params.transraydir, newdist);
						Vec3TransformNormal(&batchparams.params.hitnml, &batchparams.params.hitnml, batchparams.currenttransform);
						newdist = Vec3Length(&batchparams.params.hitnml);
					}

					hit = &batchparams.hits[y * batchparams.resolution.width + x];
					if(newdist < hit->hitdist)
					{
						hit = &batchparams.hits[y * batchparams.resolution.width + x];
						hit->hitdist = newdist;
						hit->hitsfc = batchparams.currentsfc;
						hit->hitidx = n->facelist[i];
						hit->hituv = newuv;
					}
				}
			}

		for(int i = 0; i < 8; i++)
			if(n->childnodes[i])
				BatchRayCastNode(n->childnodes[i], batchparams, region, inbox);
	/*}
	else if(region.size.width > 1 || region.size.height > 1)
	{
		int halfwidth, halfheight, halfwidth_, halfheight_;
		halfwidth = region.size.width / 2;
		halfheight = region.size.height / 2;
		halfwidth_ = halfwidth + (region.size.width & 1);
		halfheight_ = halfheight + (region.size.width & 1);

		Rect<int> newregion;
		bool newinbox[4];
		if(halfwidth * halfheight)
		{
			newregion = Rect<int>(region.left(),			 region.top(),				halfwidth, halfheight);
			newinbox[0] = inbox[0];
			newinbox[1] = IsInsideBox(batchparams, newregion.right() - 1, newregion.top(),		  &n->vmin, &n->vmax);
			newinbox[2] = IsInsideBox(batchparams, newregion.left(),	  newregion.bottom() - 1, &n->vmin, &n->vmax);
			newinbox[3] = IsInsideBox(batchparams, newregion.right() - 1, newregion.bottom() - 1, &n->vmin, &n->vmax);
			BatchRayCastNode(n, batchparams, newregion, newinbox);
		}
		if(halfwidth_ * halfheight)
		{
			newregion = Rect<int>(region.left() + halfwidth, region.top(),				halfwidth_, halfheight);
			newinbox[0] = IsInsideBox(batchparams, newregion.left(),	  newregion.top(),		  &n->vmin, &n->vmax);
			newinbox[1] = inbox[1];
			newinbox[2] = IsInsideBox(batchparams, newregion.left(),	  newregion.bottom() - 1, &n->vmin, &n->vmax);
			newinbox[3] = IsInsideBox(batchparams, newregion.right() - 1, newregion.bottom() - 1, &n->vmin, &n->vmax);
			BatchRayCastNode(n, batchparams, newregion, newinbox);
		}
		if(halfwidth * halfheight_)
		{
			newregion = Rect<int>(region.left(),			 region.top() + halfheight, halfwidth, halfheight_);
			newinbox[0] = IsInsideBox(batchparams, newregion.left(),	  newregion.top(),		  &n->vmin, &n->vmax);
			newinbox[1] = IsInsideBox(batchparams, newregion.right() - 1, newregion.top(),		  &n->vmin, &n->vmax);
			newinbox[2] = inbox[2];
			newinbox[3] = IsInsideBox(batchparams, newregion.right() - 1, newregion.bottom() - 1, &n->vmin, &n->vmax);
			BatchRayCastNode(n, batchparams, newregion, newinbox);
		}
		if(halfwidth_ * halfheight_)
		{
			newregion = Rect<int>(region.left() + halfwidth, region.top() + halfheight, halfwidth_, halfheight_);
			newinbox[0] = IsInsideBox(batchparams, newregion.left(),	  newregion.top(),		  &n->vmin, &n->vmax);
			newinbox[1] = IsInsideBox(batchparams, newregion.right() - 1, newregion.top(),		  &n->vmin, &n->vmax);
			newinbox[2] = IsInsideBox(batchparams, newregion.left(),	  newregion.bottom() - 1, &n->vmin, &n->vmax);
			newinbox[3] = inbox[3];
			BatchRayCastNode(n, batchparams, newregion, newinbox);
		}
	}*/
}

//-----------------------------------------------------------------------------
// Name: BatchRayCast()
// Desc: Multiple ray collision detection with the octrees mesh
//-----------------------------------------------------------------------------
void Octree::BatchRayCast(BatchRayCastParams& batchparams) const
{
	Rect<int> region(0, 0, batchparams.resolution.width, batchparams.resolution.height);
	bool inbox[4];
	inbox[0] = IsInsideBox(batchparams, region.left(), region.top(), &topnode->vmin, &topnode->vmax);
	inbox[1] = IsInsideBox(batchparams, region.right() - 1, region.top(), &topnode->vmin, &topnode->vmax);
	inbox[2] = IsInsideBox(batchparams, region.left(), region.bottom() - 1, &topnode->vmin, &topnode->vmax);
	inbox[3] = IsInsideBox(batchparams, region.right() - 1, region.bottom() - 1, &topnode->vmin, &topnode->vmax);

	BatchRayCastNode(topnode, batchparams, region, inbox);
}