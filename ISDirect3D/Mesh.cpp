#include "Direct3D.h"
#include "Mesh.h"

Mesh::Mesh() : cpu_vb(NULL), cpu_ib(NULL), gpu_vb(NULL), gpu_ib(NULL), attributetable(NULL), attributetablesize(0)
{
}

Result Mesh::Init(LPD3DDEVICE device, LPD3DDEVICECONTEXT devicecontext, const D3D11_INPUT_ELEMENT_DESC* decl, UINT declcount, UINT vertexsize, String possemantic, UINT numvertices, UINT numfaces)
{
	Result rlt;

	this->device = device;
	this->devicecontext = devicecontext;

	this->numvertices = numvertices;
	this->numfaces = numfaces;

	this->vertexstride = vertexsize;
	this->indexstride = sizeof(UINT); //EDIT: Support both 16 and 32 bit indices

D3D11_BUFFER_DESC desc;
desc.Usage = D3D11_USAGE_STAGING;
desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
desc.MiscFlags = 0;
desc.BindFlags = 0; // Stagging resources aren't bound

// Create vertex buffer on the CPU
desc.ByteWidth = numvertices * vertexstride;
CHKRESULT(device->CreateBuffer(&desc, NULL, &cpu_vb));

// Create index buffer on the CPU
desc.ByteWidth = 3 * numfaces * indexstride;
CHKRESULT(device->CreateBuffer(&desc, NULL, &cpu_ib));

desc.Usage = D3D11_USAGE_DEFAULT;
desc.CPUAccessFlags = 0;

// Create vertex buffer on the GPU
desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
desc.ByteWidth = numvertices * vertexstride;
CHKRESULT(device->CreateBuffer(&desc, NULL, &gpu_vb));

// Create index buffer on the GPU
desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
desc.ByteWidth = 3 * numfaces * indexstride;
CHKRESULT(device->CreateBuffer(&desc, NULL, &gpu_ib));

	return R_OK;
}

Result Mesh::SetVertexData(const LPVOID data)
{
	D3D11_MAPPED_SUBRESOURCE vertexbuffer;
	Result rlt;

	CHKRESULT(devicecontext->Map(cpu_vb, 0, D3D11_MAP_WRITE, 0, &vertexbuffer));
	memcpy(vertexbuffer.pData, data, numvertices * vertexstride);
	devicecontext->Unmap(cpu_vb, 0);
	return R_OK;
}
Result Mesh::SetIndexData(const LPVOID data, UINT numindices)
{
	D3D11_MAPPED_SUBRESOURCE indexbuffer;
	Result rlt;

	if(numindices > 3 * this->numfaces)
		return ERR("Trying to set more indices than the mesh was originally created with.");

	CHKRESULT(devicecontext->Map(cpu_ib, 0, D3D11_MAP_WRITE, 0, &indexbuffer));
	memcpy(indexbuffer.pData, data, numindices * indexstride);
	devicecontext->Unmap(cpu_ib, 0);
	return R_OK;
}
void Mesh::GetAttributeTable(IObject::AttributeRange* attributetable, UINT* attributetablesize)
{
	if(attributetable)
	{
		if(this->attributetablesize < *attributetablesize)
			*attributetablesize = this->attributetablesize;
		memcpy(attributetable, this->attributetable, *attributetablesize * sizeof(IObject::AttributeRange));
	}
	else
		*attributetablesize = this->attributetablesize;
}
Result Mesh::SetAttributeTable(const IObject::AttributeRange* attributetable, UINT attributetablesize)
{
	// Remove old table
	REMOVE_ARRAY(this->attributetable);

	this->attributetablesize = attributetablesize;
	if(attributetablesize)
	{
		// Copy new table
		this->attributetable = new IObject::AttributeRange[attributetablesize];
		memcpy(this->attributetable, attributetable, attributetablesize * sizeof(IObject::AttributeRange));
	}
	return R_OK;
}
Result Mesh::CommitToDevice()
{
	if(gpu_vb && cpu_vb)
		devicecontext->CopyResource(gpu_vb, cpu_vb);
	if(gpu_ib && cpu_ib)
		devicecontext->CopyResource(gpu_ib, cpu_ib);
	return R_OK;
}

Result Mesh::GetVertexBuffer(MeshBuffer** vertexbuffer)
{
	CHKALLOC(*vertexbuffer = new MeshBuffer(devicecontext, cpu_vb, numvertices * vertexstride));
	return R_OK;
}
Result Mesh::GetIndexBuffer(MeshBuffer** indexbuffer)
{
	CHKALLOC(*indexbuffer = new MeshBuffer(devicecontext, cpu_ib, 3 * numfaces * indexstride));
	return R_OK;
}
Result Mesh::GetAdjacencyBuffer(MeshBuffer** adjacencybuffer)
{
	return ERR("Mesh::GetAdjacencyBuffer() not implemented"); //EDIT11
}

Result Mesh::GenerateAdjacencyAndPointReps(float epsilon)
{
	Result::PrintLogMessage("Mesh::GenerateAdjacencyAndPointReps() not implemented"); //EDIT11
	return R_OK;
}

Result Mesh::Intersect(Vector3* raypos, Vector3* raydir, UINT* numhits, UINT* faceidx, float* u, float* v, float* dist, ID3D10Blob** allhits)
{
	return ERR("Mesh::Intersect() not implemented"); //EDIT11
}
Result Mesh::IntersectSubset(UINT attrid, Vector3* raypos, Vector3* raydir, UINT* numhits, UINT* faceidx, float* u, float* v, float* dist, ID3D10Blob** allhits)
{
	return ERR("Mesh::IntersectSubset() not implemented"); //EDIT11
}

void Mesh::DrawSubset(UINT attrid)
{
	// Set vertex and index buffers
	UINT offset = 0;
	devicecontext->IASetVertexBuffers(0, 1, &gpu_vb, &vertexstride, &offset);
	devicecontext->IASetIndexBuffer(gpu_ib, DXGI_FORMAT_R32_UINT, 0); //EDIT: Support both 16 and 32 bit indices

	if(attributetable)
	{
		// Draw sections of attribute table with matching attribute id
		for(UINT i = 0; i < attributetablesize; i++)
			if(attributetable[i].attrid == attrid)
				devicecontext->DrawIndexed(3 * attributetable[attrid].facecount, 3 * attributetable[attrid].facestart, 0);
	}
	else
		// Draw whole mesh
		devicecontext->DrawIndexed(3 * numfaces, 0, 0);
}

Mesh::~Mesh()
{
	RELEASE(cpu_vb);
	RELEASE(cpu_ib);
	RELEASE(gpu_vb);
	RELEASE(gpu_ib);
	if(attributetable)
		delete[] attributetable;
}


Result MeshBuffer::Map(void **data, UINT *datalen)
{
	D3D11_MAPPED_SUBRESOURCE mappedbuffer;
	Result rlt;

	*data = NULL;
	*datalen = 0;

	CHKRESULT(devicecontext->Map(buffer, 0, D3D11_MAP_READ_WRITE, 0, &mappedbuffer));
	*data = mappedbuffer.pData;
	*datalen = bufferlen;

	return R_OK;
}
Result MeshBuffer::Unmap()
{
	devicecontext->Unmap(buffer, 0);
	return R_OK;
}
