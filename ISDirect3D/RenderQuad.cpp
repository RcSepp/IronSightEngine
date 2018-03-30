#include "Direct3D.h"

D3D11_INPUT_ELEMENT_DESC RenderQuad::VTX_DECL[2] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Create buffers
//-----------------------------------------------------------------------------
Result RenderQuad::Init(LPD3DDEVICE device)
{
	D3D11_SUBRESOURCE_DATA initdata;
	Result rlt;

	// Init vertex buffer
	D3D11_BUFFER_DESC vbdesc = {4 * sizeof(VertexFormat), D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER, 0, 0};
	initdata.pSysMem = vertices;
	initdata.SysMemPitch = 0;
	initdata.SysMemSlicePitch = 0;
	CHKRESULT(device->CreateBuffer(&vbdesc, &initdata, &vb));

	// Init index buffer
	D3D11_BUFFER_DESC ibdesc = {6 * sizeof(UINT), D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER, 0, 0};
	initdata.pSysMem = indices;
	initdata.SysMemPitch = 0;
	initdata.SysMemSlicePitch = 0;
	CHKRESULT(device->CreateBuffer(&ibdesc, &initdata, &ib));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: PrepareRendering()
// Desc: Set buffers
//-----------------------------------------------------------------------------
Result RenderQuad::PrepareRendering(LPD3DDEVICECONTEXT devicecontext)
{
	// Set input params
	UINT offsets = 0;
	UINT strides[] = {sizeof(RenderQuad::VertexFormat)};
	devicecontext->IASetVertexBuffers(0, 1, &vb, strides, &offsets);
	devicecontext->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return R_OK;
}