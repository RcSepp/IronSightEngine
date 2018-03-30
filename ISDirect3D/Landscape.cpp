#include "Direct3D.h"


//-----------------------------------------------------------------------------
// Name: Landscape()
// Desc: Constructor
//-----------------------------------------------------------------------------
Landscape::Landscape()
{
	wnd = NULL;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize
//-----------------------------------------------------------------------------
Result Landscape::Load(FilePath& filename, LPRENDERSHADER shader, OutputWindow* wnd)
{
	Result rlt;

	this->wnd = wnd;
	if(!wnd)
		return ERR("Parameter wnd is NULL");
return ERR("WARNING: THIS CODE CRASHES THE GRAPHICS CARD!");

	// Load heightmap texture
	LPTEXTURE lptex;
	CHKRESULT(wnd->CreateTexture(filename, ITexture::TU_ALLACCESS, false, &lptex));
	tex = (Texture*)lptex;

	UINT width = tex->width;
	UINT height = tex->height;
	UINT numvertices = width * height;
	UINT numfaces = (width - 1) * (height - 1) * 2;

	// Create heightmap object
	LPOBJECT lpobj;
	CHKRESULT(wnd->CreateObject(numvertices, numfaces, &lpobj));
	obj = (Object*)lpobj;
obj->scl = Vector3(10.0f, 2.0f, 10.0f);

	D3D11_MAPPED_SUBRESOURCE texdata;
	tex->Lock(&texdata);

	IObject::Vertex* vertices = new IObject::Vertex[numvertices];
	CHKALLOC(vertices);
	IObject::Vertex* lpvtx = vertices;
	for(UINT y = 0; y < height; y++)
		for(UINT x = 0; x < width; x++)
		{
			lpvtx->pos = Vector3((float)x - (float)width / 2.0f, 0.0f, (float)y * 1.0f - (float)height / 2.0f);
			lpvtx->nml = Vector3(0.0f, 1.0f, 0.0f);
			lpvtx->texcoord = Vector2(0.0f, 0.0f); //EDIT
			lpvtx->blendidx = 0;
			byte height = ((byte*)texdata.pData)[4 * x + 0 + texdata.RowPitch * y];
			lpvtx->pos.y = (float)height / 10.0f;
			lpvtx++;
		}

	// Create normals
	Vector3 v0, v1, v2, v3, v01, v23;
	for(UINT y = 1; y < height - 1; y++)
		for(UINT x = 1; x < width - 1; x++)
		{
			v0 = vertices[width * (y + 0) + (x + 0)].pos - vertices[width * (y + 1) + (x + 0)].pos;
			v1 = vertices[width * (y + 0) + (x + 0)].pos - vertices[width * (y + 0) + (x + 1)].pos;
			Vec3Cross(&v01, &v0, &v1);
			v2 = vertices[width * (y + 0) + (x + 0)].pos - vertices[width * (y - 1) + (x + 0)].pos;
			v3 = vertices[width * (y + 0) + (x + 0)].pos - vertices[width * (y + 0) + (x - 1)].pos;
			Vec3Cross(&v23, &v2, &v3);
			Vec3Normalize(&vertices[width * y + x].nml, &(v01 + v23));
		}

	CHKRESULT(obj->SetVertexData(vertices));
	delete[] vertices;

	UINT* indices = new UINT[numfaces * 3];
	CHKALLOC(indices);
	UINT* lpidx = indices;
	int i = 0;
	for(UINT y = 0; y < height - 1; y++)
		for(UINT x = 0; x < width - 1; x++)
		{
			i += 6;
			*lpidx++ = (x + 0) + width * (y + 0);
			*lpidx++ = (x + 0) + width * (y + 1);
			*lpidx++ = (x + 1) + width * (y + 1);

			*lpidx++ = (x + 1) + width * (y + 1);
			*lpidx++ = (x + 1) + width * (y + 0);
			*lpidx++ = (x + 0) + width * (y + 0);
		}
	CHKRESULT(obj->SetIndexData(indices));
	delete[] indices;

	tex->Unlock();

	CHKRESULT(obj->CommitChanges());
	obj->SetShader(shader);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Draw Heightmap to screen
//-----------------------------------------------------------------------------
void Landscape::Render(RenderType rendertype)
{
	obj->Render(wnd, rendertype);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Landscape::Release()
{
	RELEASE(obj);

	delete this;
}