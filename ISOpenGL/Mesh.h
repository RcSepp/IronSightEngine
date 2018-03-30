#ifndef GLuint
typedef unsigned int GLuint;
#endif

struct MeshBuffer
{
private:
	unsigned char* buffer;
	UINT bufferlen;

public:
	MeshBuffer(unsigned char* buffer, UINT bufferlen) : buffer(buffer), bufferlen(bufferlen) {}
	UINT GetSize() {return bufferlen;}
	Result Map(void **data, UINT *datalen);
	Result Unmap();
};

struct IntersectInfo
{
	UINT faceidx;
	float u, v, dist;
};

class Mesh
{
private:
	unsigned char *cpu_vb, *cpu_ib;
	GLuint gpu_vb, gpu_ib;
	IObject::AttributeRange* attributetable;
	UINT numvertices, numfaces, attributetablesize;
	UINT vertexstride, indexstride;

public:
	Mesh();
	~Mesh();

	Result Init(UINT vertexsize,  UINT numvertices, UINT numfaces);

	UINT GetFaceCount() {return numfaces;}
	UINT GetVertexCount() {return numvertices;}

	Result SetVertexData(const LPVOID data);
	Result SetIndexData(const LPVOID data, UINT numindices);
	void GetAttributeTable(IObject::AttributeRange* attributetable, UINT* attributetablesize);
	Result SetAttributeTable(const IObject::AttributeRange* attributetable, UINT attributetablesize);
	Result CommitToDevice();

	Result GetVertexBuffer(MeshBuffer** vertexbuffer);
	Result GetIndexBuffer(MeshBuffer** indexbuffer);

	void DrawSubset(UINT attrid);
};