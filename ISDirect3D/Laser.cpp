#include "Direct3D.h"


//-----------------------------------------------------------------------------
// Name: Laser()
// Desc: Constructor
//-----------------------------------------------------------------------------
Laser::Laser()
{
	shader = NULL;
	tex = NULL;
	/*vbray = vbspot = NULL;*/
	pos = Vector3(0.0f, 0.0f, 0.0f);
	dir = Vector3(0.0f, 0.0f, 1.0f);
}

//-----------------------------------------------------------------------------
/// Initialize laser
/// \param spotfilename [in] File name of laser spot
/// \param diameter [in] Laser ray thickness in world units
/// \param spotsize [in] Laser spot diameter in world units
/// \param strength [in] Factor for color computation for HDR effects
/// \param falloff [in] Factor for computing view angle dependence
/// \return S_OK if successful, else error code
//-----------------------------------------------------------------------------
Result Laser::Init(const FilePath& spotfilename, Color color, float diameter, float spotsize, float strength, int falloff, OutputWindow* wnd)
{
	Result rlt;

	// Compile shader
	LPRENDERSHADER lpshd;
	CHKRESULT(wnd->CreateRenderShader(FilePath("Laser.fx"), &lpshd)); //EDIT: Use only one shader for multiple lasers (i.e. make shader var static)
	shader = (RenderShader*)lpshd;

	// Load spot texture
	LPTEXTURE lptex;
	CHKRESULT(wnd->CreateTexture(spotfilename, ITexture::TU_SKIN, false, &lptex));
	tex = (Texture*)lptex;
	/*shader->effect->SetTexture(shader->defparam.ObjTexture, tex);

	// Create laser ray vertexbuffer
	ERRCHECK(app->d3d->device->CreateVertexBuffer(4 * sizeof(vfLaser),
												  D3DUSAGE_WRITEONLY, vfLaser::FVF_FLAGS,
												  D3DPOOL_MANAGED, &vbray, NULL))
	vfLaser *vray;
	ERRCHECK(vbray->Lock(0, 0, (void**)&vray, 0))
	{
		vray[0].pos = vector3(diameter, 0.0f, 0.0f);
		vray[1].pos = vector3(-diameter, 0.0f, 0.0f);
		vray[2].pos = vector3(diameter, 0.0f, 1.0f);
		vray[3].pos = vector3(-diameter, 0.0f, 1.0f);
		vray[0].col = color;
		vray[1].col = color;
		vray[2].col = color;
		vray[3].col = color;
		vray[0].uv = vector2(0.0f, 0.0f);
		vray[1].uv = vector2(1.0f, 0.0f);
		vray[2].uv = vector2(1.0f, 0.0f);
		vray[3].uv = vector2(0.0f, 0.0f);
	}
	vbray->Unlock();

	// Create laser spot vertexbuffer
	ERRCHECK(app->d3d->device->CreateVertexBuffer(4 * sizeof(vfLaser),
												  D3DUSAGE_WRITEONLY, vfLaser::FVF_FLAGS,
												  D3DPOOL_MANAGED, &vbspot, NULL))
	vfLaser *vspot;
	ERRCHECK(vbspot->Lock(0, 0, (void**)&vspot, 0))
	{
		vspot[0].pos = vector3(spotsize, -spotsize, 0.0f);
		vspot[1].pos = vector3(spotsize, spotsize, 0.0f);
		vspot[2].pos = vector3(-spotsize, -spotsize, 0.0f);
		vspot[3].pos = vector3(-spotsize, spotsize, 0.0f);
		vspot[0].col = color;
		vspot[1].col = color;
		vspot[2].col = color;
		vspot[3].col = color;
		vspot[0].uv = vector2(0.0f, 0.0f);
		vspot[1].uv = vector2(0.0f, 1.0f);
		vspot[2].uv = vector2(1.0f, 1.0f);
		vspot[3].uv = vector2(1.0f, 0.0f);
	}
	vbspot->Unlock();

	// Set parameters for the laser ray
	shader->effect->SetFloat("strength", strength);
	shader->effect->SetInt("falloff", falloff);

	app->RegisterForRendering(this, __FILE__, RT_FAST);
	app->RegisterForUpdating(this, __FILE__, app->UCID_Laser);*/

const UINT numvertices =  4;
const UINT numfaces =  2;

struct Vertex
{
	Vector3 pos;
	Vector2 texcoord;
};

Vertex* vertices;
CHKALLOC(vertices = new Vertex[numvertices]);
D3D11_BUFFER_DESC bufferdesc;
D3D11_SUBRESOURCE_DATA data;

// Create laser ray vertices
{
	vertices[0].pos = Vector3(diameter, 0.0f, 0.0f);
	vertices[1].pos = Vector3(-diameter, 0.0f, 0.0f);
	vertices[2].pos = Vector3(diameter, 0.0f, 1.0f);
	vertices[3].pos = Vector3(-diameter, 0.0f, 1.0f);
	vertices[0].texcoord = Vector2(0.0f, 0.0f);
	vertices[1].texcoord = Vector2(1.0f, 0.0f);
	vertices[2].texcoord = Vector2(1.0f, 0.0f);
	vertices[3].texcoord = Vector2(0.0f, 0.0f);

	// Create vertex buffer
	bufferdesc.Usage = D3D11_USAGE_DEFAULT;
	bufferdesc.ByteWidth = sizeof(Vertex) * numvertices;
	bufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdesc.CPUAccessFlags = 0;
	bufferdesc.MiscFlags = 0;
	data.pSysMem = vertices;
	CHKRESULT(wnd->devspec->device->CreateBuffer(&bufferdesc, &data, &rayvertexbuffer));

	// Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	wnd->devspec->devicecontext->IASetVertexBuffers(0, 1, &rayvertexbuffer, &stride, &offset);
}

// Create laser spot vertices
{
	vertices[0].pos = Vector3(spotsize, -spotsize, 0.0f);
	vertices[1].pos = Vector3(spotsize, spotsize, 0.0f);
	vertices[2].pos = Vector3(-spotsize, -spotsize, 0.0f);
	vertices[3].pos = Vector3(-spotsize, spotsize, 0.0f);
	vertices[0].texcoord = Vector2(0.0f, 0.0f);
	vertices[1].texcoord = Vector2(0.0f, 1.0f);
	vertices[2].texcoord = Vector2(1.0f, 1.0f);
	vertices[3].texcoord = Vector2(1.0f, 0.0f);

	// Create vertex buffer
	data.pSysMem = vertices;
	CHKRESULT(wnd->devspec->device->CreateBuffer(&bufferdesc, &data, &spotvertexbuffer));

	// Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	wnd->devspec->devicecontext->IASetVertexBuffers(0, 1, &spotvertexbuffer, &stride, &offset);
}

	return R_OK;
}

/*matrix* GetMatrixLookAt_(const vector3* vpos, const vector3* vtarget, const vector3* vup, matrix* mresult)
{
	vector3 _vup, vlook, vleft;

	Vec3Normalize(&vlook, &(*vtarget - *vpos));
	Vec3Cross(&vleft, &vlook, &(*vup - *vpos));
	Vec3Normalize(&vleft, &vleft);
	Vec3Cross(&_vup, &vlook, &vleft);
	Vec3Normalize(&_vup, &_vup);

	mresult->_11 = vlook.x;	mresult->_12 = vlook.y;	mresult->_13 = vlook.z;	mresult->_14 = 0.0f;
	mresult->_21 = vleft.x;	mresult->_22 = vleft.y;	mresult->_23 = vleft.z;	mresult->_24 = 0.0f;
	mresult->_31 = _vup.x;	mresult->_32 = _vup.y;	mresult->_33 = _vup.z;	mresult->_34 = 0.0f;
	mresult->_41 = vpos->x;	mresult->_42 = vpos->y;	mresult->_43 = vpos->z;	mresult->_44 = 1.0f;

	return mresult;
}*/

//-----------------------------------------------------------------------------
/// Update ray coordinates
//-----------------------------------------------------------------------------
void Laser::Update()
{
	if(!autorenderenabled)
		return;

	/*Vector3 vTargetNormal;
	Vector3 vTarget = pos + dir * min(app->lvl->RayTrace(&pos, &dir, &vTargetNormal, NULL), app->d3d->CLIPFAR);

//matrix mTarget;
//GetMatrixLookAt_(&vTarget, &app->mtr->pos, &vector3(0.0f, 1.0f, 0.0f), &mTarget);

	// Set shader parameters
	shader->effect->SetVector("vStart", &Vector4(pos, 1.0f));
	shader->effect->SetVector("vEnd", &Vector4(vTarget, 1.0f));
	shader->effect->SetVector("vTargetNormal", &Vector4(vTargetNormal, 1.0f));*/
}

//-----------------------------------------------------------------------------
/// Render
/// \param rendertype [in] Type of rendering. Use this value for deciding what
/// to render if more than one render type is registered
//-----------------------------------------------------------------------------
void Laser::Render(RenderType rendertype)
{
	/*UINT passcount, pass;
	HRESULT hr;

	// Set parameters for the laser in total
	shader->effect->SetMatrix(shader->defparam.ViewProj, &app->mtr->viewprojmatrix);
	app->d3d->device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	app->d3d->device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	app->d3d->device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Set parameters for the laser ray
	app->d3d->device->SetStreamSource(0, vbray, 0, sizeof(vfLaser));
	app->d3d->device->SetFVF(vfLaser::FVF_FLAGS);
	shader->effect->SetTechnique("techPixelRay");

	shader->effect->Begin(&passcount, 0);
	for(pass = 0; pass < passcount; pass++)
	{
		shader->effect->BeginPass(pass);
		ASSERT(app->d3d->device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2))
        shader->effect->EndPass();
	}
	shader->effect->End();

	// Set parameters for the laser spot
	app->d3d->device->SetStreamSource(0, vbspot, 0, sizeof(vfLaser));
	app->d3d->device->SetFVF(vfLaser::FVF_FLAGS);

	shader->effect->SetTechnique("techSpot");
shader->effect->SetTexture(shader->defparam.ObjTexture, tex);
	shader->effect->Begin(&passcount, 0);
	for(pass = 0; pass < passcount; pass++)
	{
		shader->effect->BeginPass(pass);
		ASSERT(app->d3d->device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2))
        shader->effect->EndPass();
	}
	shader->effect->End();

	app->d3d->device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);*/

/*wnd->devspec->device->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

UINT numpasses;
//shader->PrepareRendering(wnd->cam, &mWorld, &mTrans, &mRot, worldmatrixarray, numblendparts, &numpasses);

for(UINT passidx = 0; passidx < numpasses; passidx++)
{
	shader->PreparePass(passidx);
	//shader->PrepareSubset((Texture*)mats[subset].tex, &mats[subset].ambient, &mats[subset].diffuse, &mats[subset].specular, mats[subset].power, passidx);
	wnd->devspec->device->Draw(4, 0);
}*/
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Laser::Release()
{
	/*app->UnregisterFromRendering(this, RT_FAST);
	app->UnregisterFromUpdating(this);

	RELEASE(vbray)
	RELEASE(vbspot)*/
	RELEASE(tex)
	REMOVE(shader)

	delete this;
};