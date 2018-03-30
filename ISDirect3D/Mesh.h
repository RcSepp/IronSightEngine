struct MeshBuffer
{
private:
	LPD3DDEVICECONTEXT devicecontext;
	ID3D11Buffer* buffer;
	UINT bufferlen;

public:
	MeshBuffer(LPD3DDEVICECONTEXT devicecontext, ID3D11Buffer* buffer, UINT bufferlen) : devicecontext(devicecontext), buffer(buffer), bufferlen(bufferlen) {}
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
	LPD3DDEVICE device;
	LPD3DDEVICECONTEXT devicecontext;
	ID3D11Buffer *cpu_vb, *cpu_ib;
	ID3D11Buffer *gpu_vb, *gpu_ib;
	IObject::AttributeRange* attributetable;
	UINT numvertices, numfaces, attributetablesize;
	UINT vertexstride, indexstride;

public:
	Mesh();
	~Mesh();

	Result Init(LPD3DDEVICE device, LPD3DDEVICECONTEXT devicecontext, const D3D11_INPUT_ELEMENT_DESC* decl, UINT declcount, UINT vertexsize, String possemantic, UINT numvertices, UINT numfaces);

	UINT GetFaceCount() {return numfaces;}
	UINT GetVertexCount() {return numvertices;}

	Result SetVertexData(const LPVOID data);
	Result SetIndexData(const LPVOID data, UINT numindices);
	void GetAttributeTable(IObject::AttributeRange* attributetable, UINT* attributetablesize);
	Result SetAttributeTable(const IObject::AttributeRange* attributetable, UINT attributetablesize);
	Result CommitToDevice();

	Result GetVertexBuffer(MeshBuffer** vertexbuffer);
	Result GetIndexBuffer(MeshBuffer** indexbuffer);
	Result GetAdjacencyBuffer(MeshBuffer** adjacencybuffer);

	Result GenerateAdjacencyAndPointReps(float epsilon);

	Result Intersect(Vector3* raypos, Vector3* raydir, UINT* numhits, UINT* faceidx, float* u, float* v, float* dist, ID3D10Blob** allhits);
	Result IntersectSubset(UINT attrid, Vector3* raypos, Vector3* raydir, UINT* numhits, UINT* faceidx, float* u, float* v, float* dist, ID3D10Blob** allhits);

	void DrawSubset(UINT attrid);
};