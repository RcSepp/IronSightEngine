#ifndef __OCTREE_H
#define	__OCTREE_H

struct BatchRayCastParams;

template<class T> struct Array
{
	const unsigned char* data;
	size_t stride;
	Array() : data(NULL) {}
	T& operator[](int idx) const
	{
		return *(T*)(data + idx * stride);
	}
};

class Octree
{
	private:
		Array<const Vector3> positions;
		Array<const unsigned int> indices;
		unsigned int numvertices, numfaces;
		struct Node
		{
			unsigned int *facelist;
			unsigned int facecount;
			Vector3 vmin, vmax;
			Node* childnodes[8];
		}* topnode;

		Result CreateChildNode(Node** thisnode, const unsigned int* parentfacelist, unsigned int parentfacecount,
							   const Vector3* vmin, const Vector3* vmax, int maxdepth, unsigned int maxfaces);
		void RayCastNode(Node *n, const Vector3 *pos, const Vector3 *dir, Intersection::RAY_TRIANGLE_INTERSECTION_FUNCTION isctfct, float *dist, Vector2 *uv, unsigned int *hitidx) const;
		void BatchRayCastNode(Node *n, BatchRayCastParams& batchparams, Rect<int> region, bool inbox[4]) const;

	public:
		Result Create(Array<const Vector3> positions, Array<const unsigned int> indices, unsigned int numfaces, const Vector3* vmin, const Vector3* vmax, int maxdepth, unsigned int maxfaces);
		Result Create(Array<const Vector3> positions, Array<const unsigned int> indices, unsigned int numfaces, int maxdepth, unsigned int maxfaces);
		float RayCast(const Vector3 *pos, const Vector3 *dir, Intersection::RAY_TRIANGLE_INTERSECTION_FUNCTION isctfct, Vector2 *uv, unsigned int *hitidx) const;
		void BatchRayCast(BatchRayCastParams& batchparams) const;
};

#endif