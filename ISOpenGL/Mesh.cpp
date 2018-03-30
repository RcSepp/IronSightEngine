#include "OpenGL.h"
#include "Mesh.h"

Mesh::Mesh() : cpu_vb(NULL), cpu_ib(NULL), gpu_vb(0), gpu_ib(0), attributetable(NULL), attributetablesize(0)
{
}

Result Mesh::Init(UINT vertexsize, UINT numvertices, UINT numfaces)
{
	Result rlt;

	this->numvertices = numvertices;
	this->numfaces = numfaces;

	this->vertexstride = vertexsize;
	this->indexstride = sizeof(UINT);

	// Create vertex buffer on the CPU
	cpu_vb = new unsigned char[numvertices * vertexstride];

	// Create index buffer on the CPU
	cpu_ib = new unsigned char[3 * numfaces * indexstride];

	// Generate buffers on the GPU
	GLuint gpu_buffers[2];
	GLX.glGenBuffers(ARRAYSIZE(gpu_buffers), gpu_buffers);

	// Create vertex buffer on the GPU
	GLX.glBindBuffer(GL_ARRAY_BUFFER, gpu_vb = gpu_buffers[0]);

	// Create index buffer on the GPU
	GLX.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_ib = gpu_buffers[1]);

	return R_OK;
}

Result Mesh::SetVertexData(const LPVOID data)
{
	memcpy(cpu_vb, data, numvertices * vertexstride);
	return R_OK;
}
Result Mesh::SetIndexData(const LPVOID data, UINT numindices)
{
	if(numindices > 3 * this->numfaces)
		return ERR("Trying to set more indices than the mesh was originally created with.");

	memcpy(cpu_ib, data, numindices * indexstride);
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
	{
		GLX.glBindBuffer(GL_ARRAY_BUFFER, gpu_vb);
		GLX.glBufferData(GL_ARRAY_BUFFER, numvertices * vertexstride, cpu_vb, GL_STATIC_DRAW);
	}
	if(gpu_ib && cpu_ib)
	{
		GLX.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_ib);
		GLX.glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * numfaces * indexstride, cpu_ib, GL_STATIC_DRAW);
	}
	return R_OK;
}

Result Mesh::GetVertexBuffer(MeshBuffer** vertexbuffer)
{
	CHKALLOC(*vertexbuffer = new MeshBuffer(cpu_vb, numvertices * vertexstride));
	return R_OK;
}
Result Mesh::GetIndexBuffer(MeshBuffer** indexbuffer)
{
	CHKALLOC(*indexbuffer = new MeshBuffer(cpu_ib, 3 * numfaces * indexstride));
	return R_OK;
}

void Mesh::DrawSubset(UINT attrid)
{
	// Set vertex and index buffers
	GLX.glBindBuffer(GL_ARRAY_BUFFER, gpu_vb);
	GLX.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_ib);

	if(attributetable)
	{
		// Draw sections of attribute table with matching attribute id
		for(UINT i = 0; i < attributetablesize; i++)
			if(attributetable[i].attrid == attrid)
				glDrawElements(GL_TRIANGLES, 3 * attributetable[attrid].facecount, GL_UNSIGNED_INT, (void*)(3 * attributetable[attrid].facestart));
	}
	else
		// Draw whole mesh
		glDrawElements(GL_TRIANGLES, 3 * numfaces, GL_UNSIGNED_INT, (void*)0);
}

Mesh::~Mesh()
{
	REMOVE_ARRAY(cpu_vb);
	REMOVE_ARRAY(cpu_ib);
	if(gpu_vb)
		GLX.glDeleteBuffers(1, &gpu_vb);
	if(gpu_vb)
		GLX.glDeleteBuffers(1, &gpu_ib);
	if(attributetable)
		delete[] attributetable;
}


Result MeshBuffer::Map(void **data, UINT *datalen)
{
	*data = buffer;
	*datalen = bufferlen;

	return R_OK;
}
Result MeshBuffer::Unmap()
{
	return R_OK;
}
