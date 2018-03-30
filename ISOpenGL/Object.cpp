#include "OpenGL.h"
#include "Mesh.h"
#include "XFile.h"
#include <ISHavok.h>


//-----------------------------------------------------------------------------
// Name: Object()
// Desc: Constructor
//-----------------------------------------------------------------------------
Object::Object()
{
	refcounter = matrefcounter = NULL;
	mesh = NULL;
	shader = NULL;
	wnd = NULL;
	mats = NULL;
	visible = true;

	orientmode = OM_EULER;
	MatrixIdentity(&transform);
	pos = rot = Vector3(0.0f, 0.0f, 0.0f);
	scl = Vector3(1.0f, 1.0f, 1.0f);
	QuaternionIdentity(&qrot);
	iscameradependent = true;

	bonepos = NULL;
	numblendparts = 0;

	boundingsphere.center = Vector3(0.0f, 0.0f, 0.0f);
	boundingsphere.radius = 0.0f;
	boundingbox.center = Vector3(0.0f, 0.0f, 0.0f);
	boundingbox.size = Vector3(0.0f, 0.0f, 0.0f);

	beforerendercbk = NULL;
}

bool Object::GetRefPos(Vector3& pos) const
{
	Matrix mRot, mRotX, mRotY, mRotZ;
	MatrixRotationX(&mRotX, rot.x);
	MatrixRotationY(&mRotY, -rot.y);
	MatrixRotationZ(&mRotZ, rot.z);
	mRot = mRotY * mRotX * mRotZ;

	Vector3 vboundscenter;
	Vec3TransformNormal(&vboundscenter, &this->boundingsphere.center, &mRot);
	pos = this->pos + vboundscenter;
	return true;
}

void Object::SetTransform(const Matrix* transform)
{
	orientmode = OM_MATRIX;
	this->transform = *transform;
}

//-----------------------------------------------------------------------------
// Name: LoadX()
// Desc: Load object from DirectX object file
//-----------------------------------------------------------------------------
Result Object::LoadX(const FilePath& filename, bool loadtextures, OutputWindow* wnd)
{
	MeshData mdata;
	AttributeRange* attrs = NULL;
	XMaterial* xmats = NULL;
	Result rlt;

	if(!wnd)
		return ERR("Parameter wnd is NULL");
	this->wnd = wnd;

	UINT nummats;
	CHKRESULT(LoadMeshFromX(filename, &mdata, &attrs, &xmats, &nummats));

	// Create mesh
	mesh = new Mesh();
	CHKRESULT(mesh->Init(sizeof(Vertex), mdata.numvertices, mdata.numfaces));

	// Create world matrix array
	numblendparts = mdata.numblendparts;
	bonetransform.resize(numblendparts);
	bonepos = new Vector3[numblendparts];
	for(UINT i = 0; i < numblendparts; i++)
	{
		MatrixIdentity(&bonetransform[i]);
		bonepos[i] = Vector3(0.0f, 0.0f, 0.0f);
	}

	FilePath dir;
	filename.GetParentDir(&dir);

	// Convert XMaterial to OGLMaterial
	mats = new std::vector<OGLMaterial>();
	mats->resize(nummats);
	for(UINT i = 0; i < nummats; i++)
	{
		(*mats)[i].ambient = Color(xmats[i].emissiveColor[0], xmats[i].emissiveColor[1], xmats[i].emissiveColor[2], 1.0f);
		(*mats)[i].diffuse = Color(xmats[i].faceColor);
		if((*mats)[i].diffuse.a == 0.0f)
			(*mats)[i].diffuse.a = 1.0f; //EDIT
		(*mats)[i].specular = Color(xmats[i].specularColor[0], xmats[i].specularColor[1], xmats[i].specularColor[2], 1.0f);
		(*mats)[i].power = xmats[i].power;
		if(loadtextures && !xmats[i].textureFilename.IsEmpty())
		{
			CHKRESULT(wnd->CreateTexture((FilePath)(dir << xmats[i].textureFilename), ITexture::TU_SKIN, &(*mats)[i].tex));
		}
		else
			(*mats)[i].tex = NULL;
	}
	REMOVE_ARRAY(xmats);

	// Set vertex data
	CHKRESULT(mesh->SetVertexData(mdata.vertices));

	// Set index data
	CHKRESULT(mesh->SetIndexData(mdata.indices, mdata.numfaces * 3));

	// Set attribute table
	CHKRESULT(mesh->SetAttributeTable(attrs, nummats));
	REMOVE_ARRAY(attrs);

	// Commit data
	CHKRESULT(mesh->CommitToDevice());

	// Compute bounding box
	Vector3 vMin(1e20f, 1e20f, 1e20f), vMax(-1e20f, -1e20f, -1e20f);
	for(UINT i = 0; i < mdata.numvertices; i++)
	{
		vMin.x = min(vMin.x, mdata.vertices[i].pos.x);
		vMin.y = min(vMin.y, mdata.vertices[i].pos.y);
		vMin.z = min(vMin.z, mdata.vertices[i].pos.z);

		vMax.x = max(vMax.x, mdata.vertices[i].pos.x);
		vMax.y = max(vMax.y, mdata.vertices[i].pos.y);
		vMax.z = max(vMax.z, mdata.vertices[i].pos.z);
	}
	boundingbox.size = vMax - vMin;
	boundingbox.center /*= boundingsphere.center*/ = (vMin + vMax) / 2.0f;

	// Compute bounding sphere
	/*D3DXComputeBoundingSphere(&mdata.vertices[0].pos, mdata.numvertices, sizeof(MeshData::Vertex), &boundingsphere.center, &boundingsphere.radius);*/ //EDIT11

	/*// Compute bounding sphere radius
	Vector3 vRelative;
	boundingsphere.radius = 0.0f;
	for(UINT i = 0; i < mdata.numvertices; i++)
	{
		vRelative = boundingsphere.center - mdata.vertices[i].pos;
		boundingsphere.radius = max(boundingsphere.radius, vRelative.x*vRelative.x + vRelative.y*vRelative.y + vRelative.z*vRelative.z);
	}
	boundingsphere.radius = sqrt(boundingsphere.radius);*/





	/*bool isnewobject = true;
	char *fname;
	HRESULT hr;

	LPD3DXBUFFER bufmat;
	UINT dwsubsetcount;

	this->autorender = autorender;

	// Load the mesh
	ERRCHECK(FindFile(filename, &fname))
	ERRCHECK(D3DXLoadMeshFromX(fname, D3DXMESH_MANAGED, app->d3d->device, NULL, &bufmat, NULL, &dwsubsetcount, &mesh))

	// Get extra info
	subsetcount = (int)dwsubsetcount;
	mat = (D3DXMATERIAL*) bufmat->GetBufferPointer();
	fvf = mesh->GetFVF();
	fvfstride = mesh->GetNumBytesPerVertex();
	numvertices = (int)mesh->GetNumVertices();
	numfaces = (int)mesh->GetNumFaces();
	longindices = mesh->GetOptions() & D3DXMESH_32BIT;

	// Create normals
	if(!(fvf & D3DFVF_NORMAL))
	{
		fvf |= D3DFVF_NORMAL;
		ERRCHECK(mesh->CloneMeshFVF(D3DXMESH_MANAGED, fvf, app->d3d->device,&mesh))
		ERRCHECK(D3DXComputeNormals(mesh, NULL))
	}

	// Check fvf offset to normals
	normaloffset = -1;
	D3DVERTEXELEMENT9 *decl = new D3DVERTEXELEMENT9[MAX_FVF_DECL_SIZE];
	ERRCHECK(mesh->GetDeclaration(decl))

	for(int i = 0; decl[i].Stream != 0xFF; i++)
	{
		if(decl[i].Usage == D3DDECLUSAGE_NORMAL)
		{
			normaloffset = decl[i].Offset;
			break;
		}
	}

	// Load Textures
	tex = new LPDIRECT3DTEXTURE9[subsetcount];
	ZeroMemory(tex, subsetcount * sizeof(LPDIRECT3DTEXTURE9));
	for(int i = 0; i < subsetcount; i++)
	{
		// Skip if Texturename is empty
		if(mat[i].pTextureFilename == NULL || strcmp(mat[i].pTextureFilename, "") == 0)
			continue;

		ERRCHECK(D3DXCreateTextureFromFile(app->d3d->device, mat[i].pTextureFilename, &tex[i]))
	}

	// Compute bounding ellipsoid
	vector3* vertices;
	float sphereradius;

	ERRCHECK(mesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&vertices))
	hr = D3DXComputeBoundingSphere(vertices, numvertices, fvfstride, &boundingsphere.center, &sphereradius);
	if(FAILED(hr))
	{
		mesh->UnlockVertexBuffer();
		ERRCHECK(hr)
	}
	hr = CalculateBoundingEllipsoid(vertices, numvertices, fvfstride, &boundingsphere.center, &boundingsphere.radius);
	if(FAILED(hr))
	{
		mesh->UnlockVertexBuffer();
		ERRCHECK(hr)
	}
	ERRCHECK(mesh->UnlockVertexBuffer())

	if(autorender)
		// Register for rendering
		app->renderlist.push_back(this);*/

	refcounter = new UINT(1);
	matrefcounter = new UINT(1);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create a new instance of the given object
//-----------------------------------------------------------------------------
Result Object::Create(IObject* srcobj, bool newmaterials)
{
	Object* src = (Object*)srcobj;
	Result rlt;

	this->wnd = src->wnd;
	this->mesh = src->mesh;

	this->numblendparts = src->numblendparts;
	this->bonetransform.resize(numblendparts);
	this->bonepos = new Vector3[numblendparts];
	for(UINT i = 0; i < this->numblendparts; i++)
	{
		MatrixIdentity(&this->bonetransform[i]);
		this->bonepos[i] = Vector3(0.0f, 0.0f, 0.0f);
	}

	this->orientmode = src->orientmode;
	this->transform = src->transform;
	this->pos = src->pos;
	this->rot = src->rot;
	this->scl = src->scl;
	this->qrot = src->qrot;

	this->boundingsphere = src->boundingsphere;
	this->boundingbox = src->boundingbox;

	src->refcounter[0]++;
	this->refcounter = src->refcounter;

	if(newmaterials)
	{
		// Create default material
		mats = new std::vector<OGLMaterial>();
		mats->resize(srcobj->mats->size());
		for(UINT i = 0; i < mats->size(); i++)
		{
			(*mats)[i].ambient = Color(0.1f, 0.1f, 0.1f, 1.0f);
			(*mats)[i].diffuse = Color(0.8f, 0.8f, 0.8f, 1.0f);
			(*mats)[i].specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
			(*mats)[i].power = 10.0f;
			(*mats)[i].tex = NULL;
		}
		this->matrefcounter = new UINT(1);
	}
	else
	{
		// Take over source material
		this->mats = src->mats;
		src->matrefcounter[0]++;
		this->matrefcounter = src->matrefcounter;
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create object with numvertices and numfaces
//-----------------------------------------------------------------------------
Result Object::Create(UINT numvertices, UINT numfaces, OutputWindow* wnd)
{
	Result rlt;

	if(!wnd)
		return ERR("Parameter wnd is NULL");
	this->wnd = wnd;

	// Create mesh
	mesh = new Mesh();
	CHKRESULT(mesh->Init(sizeof(Vertex), numvertices, numfaces));

	// Create world matrix array
	numblendparts = 1;
	bonetransform.resize(numblendparts);
	bonepos = new Vector3[numblendparts];
	for(UINT i = 0; i < numblendparts; i++)
	{
		MatrixIdentity(&bonetransform[i]);
		bonepos[i] = Vector3(0.0f, 0.0f, 0.0f);
	}

	// Create default material
	mats = new std::vector<OGLMaterial>();
	mats->push_back(OGLMaterial());
	(*mats)[0].ambient = Color(0.1f, 0.1f, 0.1f, 1.0f);
	(*mats)[0].diffuse = Color(0.8f, 0.8f, 0.8f, 1.0f);
	(*mats)[0].specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	(*mats)[0].power = 10.0f;
	(*mats)[0].tex = NULL;

	// Set attribute table
	AttributeRange attr;
	attr.attrid = 0;
	attr.vertexstart = 0;
	attr.vertexcount = numvertices;
	attr.facestart = 0;
	attr.facecount = numfaces;
	CHKRESULT(mesh->SetAttributeTable(&attr, 1));

	refcounter = new UINT(1);
	matrefcounter = new UINT(1);

	return R_OK;
}

/*//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create object of given shape
//-----------------------------------------------------------------------------
Result Object::Create(const OglShapeDesc& shapedesc, OutputWindow* wnd)
{
	Result rlt;

	switch(shapedesc.GetId())
	{
	case OSID_SPHERE:
		{
			OglSphereShapeDesc desc = *(OglSphereShapeDesc*)&shapedesc;

			bool closedbeta = desc.phi == 2.0f * PI;
			UINT desc_slices2 = desc.slices + !closedbeta;

			UINT numvertices = desc_slices2 * (desc.stacks + 1);
			UINT numfaces = desc.slices * desc.stacks * 2;

			CHKRESULT(Create(numvertices, numfaces, wnd));

			IObject::Vertex* vertices = new IObject::Vertex[numvertices];
			CHKALLOC(vertices);
			IObject::Vertex* lpvtx = vertices;

			// Set unused vertex properties
			for(UINT i = 0; i < numvertices; i++)
				(lpvtx++)->blendidx = 0;
			lpvtx = vertices;

			// Create vertices
			float tx, ty, alpha, beta;
			for(UINT i = 0; i <= desc.stacks; i++)
			{
				ty = (float)i / (float)desc.stacks;
				alpha = ty * desc.theta;
				for(UINT j = 0; j < desc_slices2; j++)
				{
					tx = (float)j / (float)desc.slices;
					beta = tx * desc.phi;
					lpvtx->nml = Vector3(sin(alpha) * sin(beta), cos(alpha), sin(alpha) * cos(beta));
					lpvtx->texcoord = Vector2(ty, tx);
					(lpvtx++)->pos = lpvtx->nml * desc.radius;
				}
			}

			CHKRESULT(SetVertexData(vertices));
			delete[] vertices;

			UINT* indices = new UINT[numfaces * 3];
			CHKALLOC(indices);
			UINT* lpidx = indices;
			UINT stack_offset_top, stack_offset_bottom;

			// Create faces (quads)
			stack_offset_top = 0;
			stack_offset_bottom = desc_slices2;
			for(UINT i = 0; i < desc.stacks; i++)
			{
				for(UINT j = 0; j < desc.slices; j++)
				{
					*lpidx++ = stack_offset_top + ((j + 1) % desc_slices2);
					*lpidx++ = stack_offset_top + (j + 0);
					*lpidx++ = stack_offset_bottom + (j + 0);

					*lpidx++ = stack_offset_top + ((j + 1) % desc_slices2);
					*lpidx++ = stack_offset_bottom + (j + 0);
					*lpidx++ = stack_offset_bottom + ((j + 1) % desc_slices2);
				}
				stack_offset_top += desc_slices2;
				stack_offset_bottom += desc_slices2;
			}

			CHKRESULT(SetIndexData(indices));
			delete[] indices;

			CHKRESULT(CommitChanges());
		}
		break;

	case OSID_CAPSULE:
		{
			// (Difference to sphere: one stack extra for side faces & transformation to fit (v0, v1))
			OglCapsuleShapeDesc desc = *(OglCapsuleShapeDesc*)&shapedesc;

			if((desc.stacks & 0x1) != 0)
				return ERR("Factor desc.stacks must be even for capsules");

			UINT numvertices = desc.slices * (desc.stacks + 2);
			UINT numfaces = desc.slices * (desc.stacks + 1) * 2;

			CHKRESULT(Create(numvertices, numfaces, wnd));

			IObject::Vertex* vertices = new IObject::Vertex[numvertices];
			CHKALLOC(vertices);
			IObject::Vertex* lpvtx = vertices;

			// Create transformation (billboard matrix)
			float l01 = Vec3Length(&(desc.v1 - desc.v0));

			Vector3 vray, vcam, vcamtoray, vcross, vfinal;

			vray = desc.v1 - desc.v0;
			if(vray.x != 0.0f || vray.y != 0.0f)
				vcam = desc.v0 + Vector3(0.0f, 0.0f, 1.0f);
			else
				vcam = desc.v0 + Vector3(0.0f, 1.0f, 0.0f);

			vcamtoray = ((desc.v0 + desc.v1) / 2.0f) - vcam;
			Vec3Cross(&vcross, &vcamtoray, &vray);
			Vec3Cross(&vfinal, &vcross, &vray);

			Vector3 vup, vlook, vleft;
			Vec3Normalize(&vup, &vray);
			Vec3Normalize(&vlook, &vfinal);
			Vec3Normalize(&vleft, &vcross);

			Matrix mBillboard;
			mBillboard._11 = vleft.x;		mBillboard._12 = vleft.y;		mBillboard._13 = vleft.z;		mBillboard._14 = 0.0f;
			mBillboard._21 = vup.x;			mBillboard._22 = vup.y;			mBillboard._23 = vup.z;			mBillboard._24 = 0.0f;
			mBillboard._31 = vlook.x;		mBillboard._32 = vlook.y;		mBillboard._33 = vlook.z;		mBillboard._34 = 0.0f;
			mBillboard._41 = desc.v0.x;		mBillboard._42 = desc.v0.y;		mBillboard._43 = desc.v0.z;		mBillboard._44 = 1.0f;

			// Set unused vertex properties
			for(UINT i = 0; i < numvertices; i++)
				(lpvtx++)->blendidx = 0;
			lpvtx = vertices;

			// Create upper half sphere vertices
			float tx, ty, alpha, beta;
			for(UINT i = 0; i <= desc.stacks >> 1; i++)
			{
				ty = (float)i / (float)desc.stacks;
				alpha = ty * PI;
				for(UINT j = 0; j < desc.slices; j++)
				{
					tx = (float)j / (float)desc.slices;
					beta = tx * 2 * PI;
					lpvtx->nml = Vector3(sin(alpha) * sin(beta), cos(alpha), sin(alpha) * cos(beta));
					lpvtx->texcoord = Vector2(ty, tx);
					lpvtx->pos = lpvtx->nml * desc.radius;
					lpvtx->pos.y += l01;
					Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
					lpvtx++;
				}
			}

			// Create lower half sphere vertices
			for(UINT i = desc.stacks >> 1; i <= desc.stacks; i++)
			{
				ty = (float)i / (float)desc.stacks;
				alpha = ty * PI;
				for(UINT j = 0; j < desc.slices; j++)
				{
					tx = (float)j / (float)desc.slices;
					beta = tx * 2 * PI;
					lpvtx->nml = Vector3(sin(alpha) * sin(beta), cos(alpha), sin(alpha) * cos(beta));
					lpvtx->texcoord = Vector2(ty, tx);
					lpvtx->pos = lpvtx->nml * desc.radius;
					Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
					lpvtx++;
				}
			}

			CHKRESULT(SetVertexData(vertices));
			delete[] vertices;

			UINT* indices = new UINT[numfaces * 3];
			CHKALLOC(indices);
			UINT* lpidx = indices;
			UINT stack_offset_top, stack_offset_bottom;

			// Create faces (quads)
			stack_offset_top = 0;
			stack_offset_bottom = desc.slices;
			for(UINT i = 0; i < desc.stacks + 1; i++)
			{
				for(UINT j = 0; j < desc.slices; j++)
				{
					*lpidx++ = stack_offset_top + ((j + 1) % desc.slices);
					*lpidx++ = stack_offset_top + (j + 0);
					*lpidx++ = stack_offset_bottom + (j + 0);

					*lpidx++ = stack_offset_top + ((j + 1) % desc.slices);
					*lpidx++ = stack_offset_bottom + (j + 0);
					*lpidx++ = stack_offset_bottom + ((j + 1) % desc.slices);
				}
				stack_offset_top += desc.slices;
				stack_offset_bottom += desc.slices;
			}

			CHKRESULT(SetIndexData(indices));
			delete[] indices;

			CHKRESULT(CommitChanges());
		}
		break;

	case OSID_CYLINDER:
		{
			OglCylinderShapeDesc desc = *(OglCylinderShapeDesc*)&shapedesc;

			UINT numvertices = (desc.hasfrontfaces ? 4 : 2) * desc.slices;
			UINT numfaces = 2 * desc.slices + (desc.hasfrontfaces ? 2 * (desc.slices - 2) : 0);

			CHKRESULT(Create(numvertices, numfaces, wnd));

			IObject::Vertex* vertices = new IObject::Vertex[numvertices];
			CHKALLOC(vertices);
			IObject::Vertex* lpvtx = vertices;

			// Create transformation (billboard matrix)
			float l01 = Vec3Length(&(desc.v1 - desc.v0));

			Vector3 vray, vcam, vcamtoray, vcross, vfinal;

			vray = desc.v1 - desc.v0;
			if(vray.x != 0.0f || vray.y != 0.0f)
				vcam = desc.v0 + Vector3(0.0f, 0.0f, 1.0f);
			else
				vcam = desc.v0 + Vector3(0.0f, 1.0f, 0.0f);

			vcamtoray = ((desc.v0 + desc.v1) / 2.0f) - vcam;
			Vec3Cross(&vcross, &vcamtoray, &vray);
			Vec3Cross(&vfinal, &vcross, &vray);

			Vector3 vup, vlook, vleft;
			Vec3Normalize(&vup, &vray);
			Vec3Normalize(&vlook, &vfinal);
			Vec3Normalize(&vleft, &vcross);

			Matrix mBillboard;
			mBillboard._11 = vleft.x;		mBillboard._12 = vleft.y;		mBillboard._13 = vleft.z;		mBillboard._14 = 0.0f;
			mBillboard._21 = vup.x;			mBillboard._22 = vup.y;			mBillboard._23 = vup.z;			mBillboard._24 = 0.0f;
			mBillboard._31 = vlook.x;		mBillboard._32 = vlook.y;		mBillboard._33 = vlook.z;		mBillboard._34 = 0.0f;
			mBillboard._41 = desc.v0.x;		mBillboard._42 = desc.v0.y;		mBillboard._43 = desc.v0.z;		mBillboard._44 = 1.0f;

			// Set unused vertex properties
			for(UINT i = 0; i < numvertices; i++)
				(lpvtx++)->blendidx = 0;
			lpvtx = vertices;

			// Create upper circle (normals facing sideways)
			float tx, beta;
			for(UINT j = 0; j < desc.slices; j++, lpvtx++)
			{
				tx = (float)j / (float)desc.slices;
				beta = tx * 2 * PI;
				lpvtx->nml = Vector3(sin(beta), 0.0f, cos(beta));
				lpvtx->texcoord = Vector2(1.0f, tx);
				lpvtx->pos = lpvtx->nml * desc.radius;
				lpvtx->pos.y += l01;
				Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
			}
			// Create lower circle (normals facing sideways)
			for(UINT j = 0; j < desc.slices; j++, lpvtx++)
			{
				tx = (float)j / (float)desc.slices;
				beta = tx * 2 * PI;
				lpvtx->nml = Vector3(sin(beta), 0.0f, cos(beta));
				lpvtx->texcoord = Vector2(0.0f, tx);
				lpvtx->pos = lpvtx->nml * desc.radius;
				Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
			}
			if(desc.hasfrontfaces)
			{
				// Create upper circle (normals facing up)
				for(UINT j = 0; j < desc.slices; j++, lpvtx++)
				{
					tx = (float)j / (float)desc.slices;
					beta = tx * 2 * PI;
					lpvtx->nml = Vector3(0.0f, 1.0f, 0.0f);
					lpvtx->texcoord = Vector2(0.0f, 0.0f);
					lpvtx->pos = Vector3(sin(beta), 0.0f, cos(beta)) * desc.radius;
					lpvtx->pos.y += l01;
					Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
				}
				// Create lower circle (normals facing down)
				for(UINT j = 0; j < desc.slices; j++, lpvtx++)
				{
					tx = (float)j / (float)desc.slices;
					beta = tx * 2 * PI;
					lpvtx->nml = Vector3(0.0f, -1.0f, 0.0f);
					lpvtx->texcoord = Vector2(0.0f, 0.0f);
					lpvtx->pos = Vector3(sin(beta), 0.0f, cos(beta))* desc.radius;
					Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
				}
			}

			CHKRESULT(SetVertexData(vertices));
			delete[] vertices;

			UINT* indices = new UINT[numfaces * 3];
			CHKALLOC(indices);
			UINT* lpidx = indices;

			// Create side faces (quads)
			UINT offset_uppercircle = 0 * desc.slices, offset_lowercircle = 1 * desc.slices;
			for(UINT j = 0; j < desc.slices; j++)
			{
				*lpidx++ = (j + 0) % desc.slices + offset_lowercircle;
				*lpidx++ = (j + 1) % desc.slices + offset_lowercircle;
				*lpidx++ = (j + 0) % desc.slices + offset_uppercircle;

				*lpidx++ = (j + 1) % desc.slices + offset_uppercircle;
				*lpidx++ = (j + 0) % desc.slices + offset_uppercircle;
				*lpidx++ = (j + 1) % desc.slices + offset_lowercircle;
			}
			if(desc.hasfrontfaces)
			{
				// Create upper circle faces (quads)
				offset_uppercircle = 2 * desc.slices;
				UINT lowidx = 1 + offset_uppercircle;
				UINT highidx = desc.slices - 1 + offset_uppercircle;
				UINT thirdidx = 0 + offset_uppercircle;
				for(UINT j = 2; j < desc.slices; j++)
				{
					*lpidx++ = lowidx;
					*lpidx++ = highidx;
					*lpidx++ = thirdidx;
					if(j & 1)
						// Odd step
						thirdidx = highidx--;
					else
						// Even step
						thirdidx = lowidx++;
				}
				// Create lower circle faces (quads)
				offset_lowercircle = 3 * desc.slices;
				lowidx = 1 + offset_lowercircle;
				highidx = desc.slices - 1 + offset_lowercircle;
				thirdidx = 0 + offset_lowercircle;
				for(UINT j = 2; j < desc.slices; j++)
				{
					*lpidx++ = highidx;
					*lpidx++ = lowidx;
					*lpidx++ = thirdidx;
					if(j & 1)
						// Odd step
						thirdidx = highidx--;
					else
						// Even step
						thirdidx = lowidx++;
				}
			}

			CHKRESULT(SetIndexData(indices));
			delete[] indices;

			CHKRESULT(CommitChanges());
		}
	}

	return R_OK;
}*/

//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create object of given shape
//-----------------------------------------------------------------------------
Result Object::Create(const OglShapeDesc& shapedesc, OutputWindow* wnd)
{
	Result rlt;

	UINT numvertices = shapedesc.GetVertexCount();
	UINT numfaces = shapedesc.GetFaceCount();

	CHKRESULT(Create(numvertices, numfaces, wnd));

	IObject::Vertex* vertices = new IObject::Vertex[numvertices];
	CHKALLOC(vertices);
	UINT* indices = new UINT[numfaces * 3];
	CHKALLOC(indices);

	CHKRESULT(AddShape(shapedesc, vertices, indices, 0));

	ComputeBoundingSphere(vertices, numvertices);

	CHKRESULT(SetVertexData(vertices));
	delete[] vertices;
	CHKRESULT(SetIndexData(indices));
	delete[] indices;

	CHKRESULT(CommitChanges());

	return R_OK;
}

Result Object::Create(const OglShapeDesc* shapedescs[], UINT numshapedescs, OutputWindow* wnd)
{
	Result rlt;

	UINT totalnumvertices = 0;
	UINT totalnumfaces = 0;
	for(UINT i = 0; i < numshapedescs; i++)
	{
		totalnumvertices += shapedescs[i]->GetVertexCount();
		totalnumfaces += shapedescs[i]->GetFaceCount();
	}

	CHKRESULT(Create(totalnumvertices, totalnumfaces, wnd));

	IObject::Vertex* vertices = new IObject::Vertex[totalnumvertices];
	CHKALLOC(vertices);
	UINT* indices = new UINT[totalnumfaces * 3];
	CHKALLOC(indices);

	IObject::Vertex* shapevertices = vertices;
	UINT* shapeindices = indices;
	UINT startidx = 0;
	for(UINT i = 0; i < numshapedescs; i++)
	{
		CHKRESULT(AddShape(*shapedescs[i], shapevertices, shapeindices, startidx));
		startidx += shapedescs[i]->GetVertexCount();
		shapevertices = vertices + startidx;
		shapeindices += shapedescs[i]->GetFaceCount() * 3;
	}

	ComputeBoundingSphere(vertices, totalnumvertices);

	CHKRESULT(SetVertexData(vertices));
	delete[] vertices;
	CHKRESULT(SetIndexData(indices));
	delete[] indices;

	CHKRESULT(CommitChanges());

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: ComputeBoundingSphere()
// Desc: Compute center and radius of the bounding sphere
//-----------------------------------------------------------------------------
void Object::ComputeBoundingSphere(IObject::Vertex* vertices, UINT numvertices)
{
	// Compute bounding box
	Vector3 vMin(1e20f, 1e20f, 1e20f), vMax(-1e20f, -1e20f, -1e20f);
	for(UINT i = 0; i < numvertices; i++)
	{
		vMin.x = min(vMin.x, vertices[i].pos.x);
		vMin.y = min(vMin.y, vertices[i].pos.y);
		vMin.z = min(vMin.z, vertices[i].pos.z);

		vMax.x = max(vMax.x, vertices[i].pos.x);
		vMax.y = max(vMax.y, vertices[i].pos.y);
		vMax.z = max(vMax.z, vertices[i].pos.z);
	}
	boundingbox.size = vMax - vMin;
	boundingbox.center /*= boundingsphere.center*/ = (vMin + vMax) / 2.0f;

	/*// Compute bounding sphere radius
	Vector3 vRelative;
	boundingsphere.radius = 0.0f;
	for(UINT i = 0; i < numvertices; i++)
	{
		vRelative = boundingsphere.center - vertices[i].pos;
		boundingsphere.radius = max(boundingsphere.radius, vRelative.x*vRelative.x + vRelative.y*vRelative.y + vRelative.z*vRelative.z);
	}
	boundingsphere.radius = sqrt(boundingsphere.radius);*/

	// Compute bounding sphere
	/*D3DXComputeBoundingSphere(&vertices[0].pos, numvertices, sizeof(MeshData::Vertex), &boundingsphere.center, &boundingsphere.radius);*/ //EDIT11
}

//-----------------------------------------------------------------------------
// Name: AddShape()
// Desc: Add vertices and indices of the given shape to this object's mesh
//-----------------------------------------------------------------------------
Result Object::AddShape(const OglShapeDesc& shapedesc, IObject::Vertex* vertices, UINT* indices, UINT startidx)
{
	Result rlt;

	switch(shapedesc.GetId())
	{
	case OSID_PLANE:
		{
			OglPlaneShapeDesc desc = *(OglPlaneShapeDesc*)&shapedesc;

			// Set unused vertex properties
			UINT numvertices = desc.GetVertexCount();
			for(UINT i = 0; i < numvertices; i++)
				vertices[i].blendidx = 0;

			// Create vertices
			Vector3 vup = desc.nml.y >= 1.0f - FLOAT_TOLERANCE ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(0.0f, 1.0f, 0.0f);
			Vector3 u, v;
			Vec3Cross(&u, &desc.nml, &vup);
			Vec3Cross(&v, &u, &desc.nml);
			Vec3Scale(&u, &u, desc.size.x);
			Vec3Scale(&v, &v, desc.size.y);
			Vec3Sub(&vertices[0].pos, &desc.pos, &u); Vec3Add(&vertices[0].pos, &vertices[0].pos, &v); vertices[0].texcoord = Vector2(0.0f, desc.texcoordscale.y);
			Vec3Sub(&vertices[1].pos, &desc.pos, &u); Vec3Sub(&vertices[1].pos, &vertices[1].pos, &v); vertices[1].texcoord = Vector2(0.0f, 0.0f);
			Vec3Add(&vertices[2].pos, &desc.pos, &u); Vec3Add(&vertices[2].pos, &vertices[2].pos, &v); vertices[2].texcoord = Vector2(desc.texcoordscale.x, desc.texcoordscale.y);
			Vec3Add(&vertices[3].pos, &desc.pos, &u); Vec3Sub(&vertices[3].pos, &vertices[3].pos, &v); vertices[3].texcoord = Vector2(desc.texcoordscale.x, 0.0f);
			vertices[0].nml = vertices[1].nml = vertices[2].nml = vertices[3].nml = desc.nml;

			// Create faces (quads)
			indices[0] = 2; indices[1] = 1; indices[2] = 0;
			indices[3] = 1; indices[4] = 2; indices[5] = 3;
		}
		break;

	case OSID_BOX:
		{
			OglBoxShapeDesc desc = *(OglBoxShapeDesc*)&shapedesc;
			Vector3 halfsize; Vec3Scale(&halfsize, &desc.size, 0.5f);

			// Set unused vertex properties
			UINT numvertices = desc.GetVertexCount();
			for(UINT i = 0; i < numvertices; i++)
				vertices[i].blendidx = 0;

			// Create vertices

			// Front face
			vertices[ 0].pos = Vector3(-halfsize.x, -halfsize.y, -halfsize.z); vertices[ 0].texcoord = Vector2(0.0f, desc.texcoordscale.y);
			vertices[ 1].pos = Vector3(-halfsize.x,  halfsize.y, -halfsize.z); vertices[ 1].texcoord = Vector2(0.0f, 0.0f);
			vertices[ 2].pos = Vector3( halfsize.x, -halfsize.y, -halfsize.z); vertices[ 2].texcoord = Vector2(desc.texcoordscale.x, desc.texcoordscale.y);
			vertices[ 3].pos = Vector3( halfsize.x,  halfsize.y, -halfsize.z); vertices[ 3].texcoord = Vector2(desc.texcoordscale.x, 0.0f);
			vertices[0].nml = vertices[1].nml = vertices[2].nml = vertices[3].nml = Vector3(0.0f, 0.0f, -1.0f);

			// Back face
			vertices[ 4].pos = Vector3( halfsize.x, -halfsize.y,  halfsize.z); vertices[ 4].texcoord = Vector2(0.0f, desc.texcoordscale.y);
			vertices[ 5].pos = Vector3( halfsize.x,  halfsize.y,  halfsize.z); vertices[ 5].texcoord = Vector2(0.0f, 0.0f);
			vertices[ 6].pos = Vector3(-halfsize.x, -halfsize.y,  halfsize.z); vertices[ 6].texcoord = Vector2(desc.texcoordscale.x, desc.texcoordscale.y);
			vertices[ 7].pos = Vector3(-halfsize.x,  halfsize.y,  halfsize.z); vertices[ 7].texcoord = Vector2(desc.texcoordscale.x, 0.0f);
			vertices[4].nml = vertices[5].nml = vertices[6].nml = vertices[7].nml = Vector3(0.0f, 0.0f, 1.0f);

			// Left face
			vertices[ 8].pos = Vector3(-halfsize.x, -halfsize.y,  halfsize.z); vertices[ 8].texcoord = Vector2(0.0f, desc.texcoordscale.y);
			vertices[ 9].pos = Vector3(-halfsize.x,  halfsize.y,  halfsize.z); vertices[ 9].texcoord = Vector2(0.0f, 0.0f);
			vertices[10].pos = Vector3(-halfsize.x, -halfsize.y, -halfsize.z); vertices[10].texcoord = Vector2(desc.texcoordscale.x, desc.texcoordscale.y);
			vertices[11].pos = Vector3(-halfsize.x,  halfsize.y, -halfsize.z); vertices[11].texcoord = Vector2(desc.texcoordscale.x, 0.0f);
			vertices[8].nml = vertices[9].nml = vertices[10].nml = vertices[11].nml = Vector3(-1.0f, 0.0f, 0.0f);

			// Right face
			vertices[12].pos = Vector3( halfsize.x, -halfsize.y, -halfsize.z); vertices[12].texcoord = Vector2(0.0f, desc.texcoordscale.y);
			vertices[13].pos = Vector3( halfsize.x,  halfsize.y, -halfsize.z); vertices[13].texcoord = Vector2(0.0f, 0.0f);
			vertices[14].pos = Vector3( halfsize.x, -halfsize.y,  halfsize.z); vertices[14].texcoord = Vector2(desc.texcoordscale.x, desc.texcoordscale.y);
			vertices[15].pos = Vector3( halfsize.x,  halfsize.y,  halfsize.z); vertices[15].texcoord = Vector2(desc.texcoordscale.x, 0.0f);
			vertices[12].nml = vertices[13].nml = vertices[14].nml = vertices[15].nml = Vector3(1.0f, 0.0f, 0.0f);

			// Top face
			vertices[16].pos = Vector3(-halfsize.x,  halfsize.y, -halfsize.z); vertices[16].texcoord = Vector2(0.0f, desc.texcoordscale.y);
			vertices[17].pos = Vector3(-halfsize.x,  halfsize.y,  halfsize.z); vertices[17].texcoord = Vector2(0.0f, 0.0f);
			vertices[18].pos = Vector3( halfsize.x,  halfsize.y, -halfsize.z); vertices[18].texcoord = Vector2(desc.texcoordscale.x, desc.texcoordscale.y);
			vertices[19].pos = Vector3( halfsize.x,  halfsize.y,  halfsize.z); vertices[19].texcoord = Vector2(desc.texcoordscale.x, 0.0f);
			vertices[16].nml = vertices[17].nml = vertices[18].nml = vertices[19].nml = Vector3(0.0f, 1.0f, 0.0f);

			// Bottom face
			vertices[20].pos = Vector3( halfsize.x, -halfsize.y, -halfsize.z); vertices[20].texcoord = Vector2(0.0f, desc.texcoordscale.y);
			vertices[21].pos = Vector3( halfsize.x, -halfsize.y,  halfsize.z); vertices[21].texcoord = Vector2(0.0f, 0.0f);
			vertices[22].pos = Vector3(-halfsize.x, -halfsize.y, -halfsize.z); vertices[22].texcoord = Vector2(desc.texcoordscale.x, desc.texcoordscale.y);
			vertices[23].pos = Vector3(-halfsize.x, -halfsize.y,  halfsize.z); vertices[23].texcoord = Vector2(desc.texcoordscale.x, 0.0f);
			vertices[20].nml = vertices[21].nml = vertices[22].nml = vertices[23].nml = Vector3(0.0f, -1.0f, 0.0f);

			// Offset vertices by desc.pos
			for(UINT i = 0; i < numvertices; i++)
				Vec3Add(&vertices[i].pos, &vertices[i].pos, &desc.pos);

			// Create faces (quads)
			UINT* lpidx = indices;
			for(UINT i = startidx; i < startidx + 24; i += 4)
			{
				*lpidx++ = i + 0;
				*lpidx++ = i + 1;
				*lpidx++ = i + 2;

				*lpidx++ = i + 2;
				*lpidx++ = i + 1;
				*lpidx++ = i + 3;
			}
		}
		break;
	case OSID_SPHERE:
		{
			OglSphereShapeDesc desc = *(OglSphereShapeDesc*)&shapedesc;

			bool closedbeta = desc.phi == 2.0f * PI;
			UINT desc_slices2 = desc.slices + !closedbeta;

			IObject::Vertex* lpvtx = vertices;

			// Set unused vertex properties
			UINT numvertices = desc.GetVertexCount();
			for(UINT i = 0; i < numvertices; i++)
				(lpvtx++)->blendidx = 0;
			lpvtx = vertices;

			// Create vertices
			float tx, ty, alpha, beta;
			for(UINT i = 0; i <= desc.stacks; i++)
			{
				ty = (float)i / (float)desc.stacks;
				alpha = ty * desc.theta;
				for(UINT j = 0; j < desc_slices2; j++)
				{
					tx = (float)j / (float)desc.slices;
					beta = desc.rot_y + tx * desc.phi;
					lpvtx->nml = Vector3(sin(alpha) * sin(beta), cos(alpha), sin(alpha) * cos(beta));
					lpvtx->texcoord = Vector2(tx * desc.texcoordscale.x, ty * desc.texcoordscale.y);
					(lpvtx++)->pos = desc.pos + lpvtx->nml * desc.radius;
				}
			}

			UINT* lpidx = indices;
			UINT stack_offset_top, stack_offset_bottom;

			// Create faces (quads)
			stack_offset_top = startidx + 0;
			stack_offset_bottom = startidx + desc_slices2;
			for(UINT i = 0; i < desc.stacks; i++)
			{
				for(UINT j = 0; j < desc.slices; j++)
				{
					*lpidx++ = stack_offset_top + ((j + 1) % desc_slices2);
					*lpidx++ = stack_offset_top + (j + 0);
					*lpidx++ = stack_offset_bottom + (j + 0);

					*lpidx++ = stack_offset_top + ((j + 1) % desc_slices2);
					*lpidx++ = stack_offset_bottom + (j + 0);
					*lpidx++ = stack_offset_bottom + ((j + 1) % desc_slices2);
				}
				stack_offset_top += desc_slices2;
				stack_offset_bottom += desc_slices2;
			}
		}
		break;

	case OSID_CAPSULE:
		{
			// (Difference to sphere: one stack extra for side faces & transformation to fit (v0, v1))
			OglCapsuleShapeDesc desc = *(OglCapsuleShapeDesc*)&shapedesc;

			if((desc.stacks & 0x1) != 0)
				return ERR("Factor desc.stacks must be even for capsules");

			IObject::Vertex* lpvtx = vertices;

			// Create transformation (billboard matrix)
			Vector3 vray, vcam, vcamtoray, vcross, vfinal;

			vray = desc.v1 - desc.v0;
			if(vray.x != 0.0f || vray.y != 0.0f)
				vcam = desc.v0 + Vector3(0.0f, 0.0f, 1.0f);
			else
				vcam = desc.v0 + Vector3(0.0f, 1.0f, 0.0f);
			float l01 = Vec3Length(&vray);

			vcamtoray = ((desc.v0 + desc.v1) / 2.0f) - vcam;
			Vec3Cross(&vcross, &vcamtoray, &vray);
			Vec3Cross(&vfinal, &vcross, &vray);

			Vector3 vup, vlook, vleft;
			Vec3Normalize(&vup, &vray);
			Vec3Normalize(&vlook, &vfinal);
			Vec3Normalize(&vleft, &vcross);

			Matrix mBillboard;
			mBillboard._11 = vleft.x;		mBillboard._12 = vleft.y;		mBillboard._13 = vleft.z;		mBillboard._14 = 0.0f;
			mBillboard._21 = vup.x;			mBillboard._22 = vup.y;			mBillboard._23 = vup.z;			mBillboard._24 = 0.0f;
			mBillboard._31 = vlook.x;		mBillboard._32 = vlook.y;		mBillboard._33 = vlook.z;		mBillboard._34 = 0.0f;
			mBillboard._41 = desc.v0.x;		mBillboard._42 = desc.v0.y;		mBillboard._43 = desc.v0.z;		mBillboard._44 = 1.0f;

			// Set unused vertex properties
			UINT numvertices = desc.GetVertexCount();
			for(UINT i = 0; i < numvertices; i++)
				(lpvtx++)->blendidx = 0;
			lpvtx = vertices;

			// Create upper half sphere vertices
			float tx, ty, alpha, beta;
			for(UINT i = 0; i <= desc.stacks >> 1; i++)
			{
				ty = (float)i / (float)desc.stacks;
				alpha = ty * PI;
				for(UINT j = 0; j < desc.slices; j++)
				{
					tx = (float)j / (float)desc.slices;
					beta = tx * 2 * PI;
					lpvtx->nml = Vector3(sin(alpha) * sin(beta), cos(alpha), sin(alpha) * cos(beta));
					lpvtx->texcoord = Vector2(tx * desc.texcoordscale.x, ty * desc.texcoordscale.y);
					lpvtx->pos = lpvtx->nml * desc.radius;
					lpvtx->pos.y += l01;
					Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
					lpvtx++;
				}
			}

			// Create lower half sphere vertices
			for(UINT i = desc.stacks >> 1; i <= desc.stacks; i++)
			{
				ty = (float)i / (float)desc.stacks;
				alpha = ty * PI;
				for(UINT j = 0; j < desc.slices; j++)
				{
					tx = (float)j / (float)desc.slices;
					beta = tx * 2 * PI;
					lpvtx->nml = Vector3(sin(alpha) * sin(beta), cos(alpha), sin(alpha) * cos(beta));
					lpvtx->texcoord = Vector2(tx * desc.texcoordscale.x, ty * desc.texcoordscale.y);
					lpvtx->pos = lpvtx->nml * desc.radius;
					Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
					lpvtx++;
				}
			}

			UINT* lpidx = indices;
			UINT stack_offset_top, stack_offset_bottom;

			// Create faces (quads)
			stack_offset_top = startidx + 0;
			stack_offset_bottom = startidx + desc.slices;
			for(UINT i = 0; i < desc.stacks + 1; i++)
			{
				for(UINT j = 0; j < desc.slices; j++)
				{
					*lpidx++ = stack_offset_top + ((j + 1) % desc.slices);
					*lpidx++ = stack_offset_top + (j + 0);
					*lpidx++ = stack_offset_bottom + (j + 0);

					*lpidx++ = stack_offset_top + ((j + 1) % desc.slices);
					*lpidx++ = stack_offset_bottom + (j + 0);
					*lpidx++ = stack_offset_bottom + ((j + 1) % desc.slices);
				}
				stack_offset_top += desc.slices;
				stack_offset_bottom += desc.slices;
			}
		}
		break;

	case OSID_CYLINDER:
		{
			OglCylinderShapeDesc desc = *(OglCylinderShapeDesc*)&shapedesc;

			IObject::Vertex* lpvtx = vertices;

			// Create transformation (billboard matrix)
			Vector3 vray, vcam, vcamtoray, vcross, vfinal;

			vray = desc.v1 - desc.v0;
			if(vray.x != 0.0f || vray.y != 0.0f)
				vcam = desc.v0 + Vector3(0.0f, 0.0f, 1.0f);
			else
				vcam = desc.v0 + Vector3(0.0f, 1.0f, 0.0f);
			float l01 = Vec3Length(&vray);

			vcamtoray = ((desc.v0 + desc.v1) / 2.0f) - vcam;
			Vec3Cross(&vcross, &vcamtoray, &vray);
			Vec3Cross(&vfinal, &vcross, &vray);

			Vector3 vup, vlook, vleft;
			Vec3Normalize(&vup, &vray);
			Vec3Normalize(&vlook, &vfinal);
			Vec3Normalize(&vleft, &vcross);

			Matrix mBillboard;
			mBillboard._11 = vleft.x;		mBillboard._12 = vleft.y;		mBillboard._13 = vleft.z;		mBillboard._14 = 0.0f;
			mBillboard._21 = vup.x;			mBillboard._22 = vup.y;			mBillboard._23 = vup.z;			mBillboard._24 = 0.0f;
			mBillboard._31 = vlook.x;		mBillboard._32 = vlook.y;		mBillboard._33 = vlook.z;		mBillboard._34 = 0.0f;
			mBillboard._41 = desc.v0.x;		mBillboard._42 = desc.v0.y;		mBillboard._43 = desc.v0.z;		mBillboard._44 = 1.0f;

			// Set unused vertex properties
			UINT numvertices = desc.GetVertexCount();
			for(UINT i = 0; i < numvertices; i++)
				(lpvtx++)->blendidx = 0;
			lpvtx = vertices;

			// Create upper circle (normals facing sideways)
			float tx, beta;
			for(UINT j = 0; j < desc.slices; j++, lpvtx++)
			{
				tx = (float)j / (float)desc.slices;
				beta = tx * 2 * PI;
				lpvtx->nml = Vector3(sin(beta), 0.0f, cos(beta));
				lpvtx->texcoord = Vector2(tx * desc.texcoordscale.x, desc.texcoordscale.y);
				lpvtx->pos = lpvtx->nml * desc.radius;
				lpvtx->pos.y += l01;
				Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
			}
			// Create lower circle (normals facing sideways)
			for(UINT j = 0; j < desc.slices; j++, lpvtx++)
			{
				tx = (float)j / (float)desc.slices;
				beta = tx * 2 * PI;
				lpvtx->nml = Vector3(sin(beta), 0.0f, cos(beta));
				lpvtx->texcoord = Vector2(tx * desc.texcoordscale.x, 0.0f);
				lpvtx->pos = lpvtx->nml * desc.radius;
				Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
			}
			if(desc.hasfrontfaces)
			{
				// Create upper circle (normals facing up)
				for(UINT j = 0; j < desc.slices; j++, lpvtx++)
				{
					tx = (float)j / (float)desc.slices;
					beta = tx * 2 * PI;
					lpvtx->nml = Vector3(0.0f, 1.0f, 0.0f);
					lpvtx->texcoord = Vector2(0.0f, 0.0f);
					lpvtx->pos = Vector3(sin(beta), 0.0f, cos(beta)) * desc.radius;
					lpvtx->pos.y += l01;
					Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
				}
				// Create lower circle (normals facing down)
				for(UINT j = 0; j < desc.slices; j++, lpvtx++)
				{
					tx = (float)j / (float)desc.slices;
					beta = tx * 2 * PI;
					lpvtx->nml = Vector3(0.0f, -1.0f, 0.0f);
					lpvtx->texcoord = Vector2(0.0f, 0.0f);
					lpvtx->pos = Vector3(sin(beta), 0.0f, cos(beta))* desc.radius;
					Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
				}
			}

			UINT* lpidx = indices;

			// Create side faces (quads)
			UINT offset_uppercircle = startidx + 0 * desc.slices, offset_lowercircle = startidx + 1 * desc.slices;
			for(UINT j = 0; j < desc.slices; j++)
			{
				*lpidx++ = (j + 0) % desc.slices + offset_lowercircle;
				*lpidx++ = (j + 1) % desc.slices + offset_lowercircle;
				*lpidx++ = (j + 0) % desc.slices + offset_uppercircle;

				*lpidx++ = (j + 1) % desc.slices + offset_uppercircle;
				*lpidx++ = (j + 0) % desc.slices + offset_uppercircle;
				*lpidx++ = (j + 1) % desc.slices + offset_lowercircle;
			}
			if(desc.hasfrontfaces)
			{
				// Create upper circle faces (quads)
				offset_uppercircle = startidx + 2 * desc.slices;
				UINT lowidx = 1 + offset_uppercircle;
				UINT highidx = desc.slices - 1 + offset_uppercircle;
				UINT thirdidx = 0 + offset_uppercircle;
				for(UINT j = 2; j < desc.slices; j++)
				{
					*lpidx++ = lowidx;
					*lpidx++ = highidx;
					*lpidx++ = thirdidx;
					if(j & 1)
						// Odd step
						thirdidx = highidx--;
					else
						// Even step
						thirdidx = lowidx++;
				}
				// Create lower circle faces (quads)
				offset_lowercircle = startidx + 3 * desc.slices;
				lowidx = 1 + offset_lowercircle;
				highidx = desc.slices - 1 + offset_lowercircle;
				thirdidx = 0 + offset_lowercircle;
				for(UINT j = 2; j < desc.slices; j++)
				{
					*lpidx++ = highidx;
					*lpidx++ = lowidx;
					*lpidx++ = thirdidx;
					if(j & 1)
						// Odd step
						thirdidx = highidx--;
					else
						// Even step
						thirdidx = lowidx++;
				}
			}
		}
	case OSID_CONE:
		{
			OglConeShapeDesc desc = *(OglConeShapeDesc*)&shapedesc;

			IObject::Vertex* lpvtx = vertices;

			// Create transformation (billboard matrix)
			Vector3 vray, vcam, vcamtoray, vcross, vfinal;

			vray = desc.v1 - desc.v0;
			if(vray.x != 0.0f || vray.y != 0.0f)
				vcam = desc.v0 + Vector3(0.0f, 0.0f, 1.0f);
			else
				vcam = desc.v0 + Vector3(0.0f, 1.0f, 0.0f);
			float l01 = Vec3Length(&vray);

			vcamtoray = ((desc.v0 + desc.v1) / 2.0f) - vcam;
			Vec3Cross(&vcross, &vcamtoray, &vray);
			Vec3Cross(&vfinal, &vcross, &vray);

			Vector3 vup, vlook, vleft;
			Vec3Normalize(&vup, &vray);
			Vec3Normalize(&vlook, &vfinal);
			Vec3Normalize(&vleft, &vcross);

			Matrix mBillboard;
			mBillboard._11 = vleft.x;		mBillboard._12 = vleft.y;		mBillboard._13 = vleft.z;		mBillboard._14 = 0.0f;
			mBillboard._21 = vup.x;			mBillboard._22 = vup.y;			mBillboard._23 = vup.z;			mBillboard._24 = 0.0f;
			mBillboard._31 = vlook.x;		mBillboard._32 = vlook.y;		mBillboard._33 = vlook.z;		mBillboard._34 = 0.0f;
			mBillboard._41 = desc.v0.x;		mBillboard._42 = desc.v0.y;		mBillboard._43 = desc.v0.z;		mBillboard._44 = 1.0f;

			// Set unused vertex properties
			UINT numvertices = desc.GetVertexCount();
			for(UINT i = 0; i < numvertices; i++)
				(lpvtx++)->blendidx = 0;
			lpvtx = vertices;

			// Create top vertex (normals facing sideways)
			float tx, beta;
			float nl =  sqrt(l01*l01 + desc.radius*desc.radius);
			float ny = desc.radius / nl, nxz = l01 / nl;
			for(UINT j = 0; j < desc.slices; j++, lpvtx++)
			{
				tx = (float)j / (float)desc.slices;
				beta = tx * 2 * PI;
				lpvtx->nml = Vector3(nxz * sin(beta), ny, nxz * cos(beta));
				lpvtx->texcoord = Vector2(tx * desc.texcoordscale.x, desc.texcoordscale.y);
				lpvtx->pos = Vector3(0.0f, l01, 0.0f);
				Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
			}
			// Create lower circle (normals facing sideways)
			for(UINT j = 0; j < desc.slices; j++, lpvtx++)
			{
				tx = (float)j / (float)desc.slices;
				beta = tx * 2 * PI;
				lpvtx->nml = Vector3(sin(beta), 0.0f, cos(beta));
				lpvtx->texcoord = Vector2(tx * desc.texcoordscale.x, 0.0f);
				lpvtx->pos = lpvtx->nml * desc.radius;
				Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
			}
			if(desc.hasgroundfaces)
			{
				// Create lower circle (normals facing down)
				for(UINT j = 0; j < desc.slices; j++, lpvtx++)
				{
					tx = (float)j / (float)desc.slices;
					beta = tx * 2 * PI;
					lpvtx->nml = Vector3(0.0f, -1.0f, 0.0f);
					lpvtx->texcoord = Vector2(0.0f, 0.0f);
					lpvtx->pos = Vector3(sin(beta), 0.0f, cos(beta))* desc.radius;
					Vec3TransformCoord(&lpvtx->pos, &lpvtx->pos, &mBillboard);
				}
			}

			UINT* lpidx = indices;

			// Create side faces (quads)
			UINT offset_uppercircle = startidx + 0 * desc.slices, offset_lowercircle = startidx + 1 * desc.slices;
			for(UINT j = 0; j < desc.slices; j++)
			{
				*lpidx++ = (j + 0) % desc.slices + offset_lowercircle;
				*lpidx++ = (j + 1) % desc.slices + offset_lowercircle;
				*lpidx++ = (j + 0) % desc.slices + offset_uppercircle;

				*lpidx++ = (j + 1) % desc.slices + offset_uppercircle;
				*lpidx++ = (j + 0) % desc.slices + offset_uppercircle;
				*lpidx++ = (j + 1) % desc.slices + offset_lowercircle;
			}
			if(desc.hasgroundfaces)
			{
				// Create lower circle faces (quads)
				offset_lowercircle = startidx + 2 * desc.slices;
				UINT lowidx = 1 + offset_lowercircle;
				UINT highidx = desc.slices - 1 + offset_lowercircle;
				UINT thirdidx = 0 + offset_lowercircle;
				for(UINT j = 2; j < desc.slices; j++)
				{
					*lpidx++ = highidx;
					*lpidx++ = lowidx;
					*lpidx++ = thirdidx;
					if(j & 1)
						// Odd step
						thirdidx = highidx--;
					else
						// Even step
						thirdidx = lowidx++;
				}
			}
		}
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Split()
// Desc: Split the mesh along a given plane into two new meshes
//-----------------------------------------------------------------------------
Result Object::Split(const Plane* splitplane, LPOBJECT* first, LPOBJECT* second)
{
	Vector3 planenml(splitplane->a, splitplane->b, splitplane->c);
	Result rlt;

	auto VerticesOverlap = [](const Vector3* vtx0, const Vector3* vtx1) -> bool
	{
		float x = vtx0->x - vtx1->x, y = vtx0->y - vtx1->y, z = vtx0->z - vtx1->z;
		return x < 1e-5f && x > -1e-5f && y < 1e-5f && y > -1e-5f && z < 1e-5f && z > -1e-5f;
	};

	// >>> Get vertices and indices

	IObject::Vertex* vertices;
	UINT* indices;
	CHKRESULT(MapVertexData(&vertices));
	CHKRESULT(MapIndexData(&indices));
	UINT numvertices = mesh->GetVertexCount();
	UINT numfaces = mesh->GetFaceCount();

	// >>> Create arrays for new vertices and indices

	std::vector<IObject::Vertex> firstvertices/*(vertices, vertices + numvertices)*/, secondvertices/*(vertices, vertices + numvertices)*/;
	std::vector<UINT> firstindices, secondindices;

	// >>> Add vertices on each side

	UINT *firstvtxremap = new UINT[3 * numfaces], *secondvtxremap = new UINT[3 * numfaces];
	CHKALLOC(firstvtxremap);
	CHKALLOC(secondvtxremap);
	for(UINT idx = 0; idx < numvertices; idx++)
	{
		if(Vec3Dot(&planenml, &vertices[idx].pos) + splitplane->d < 1e-5f)
		{
			firstvtxremap[idx] = firstvertices.size();
			firstvertices.push_back(vertices[idx]);
		}
		else
			firstvtxremap[idx] = (UINT)-1;
		if(Vec3Dot(&planenml, &vertices[idx].pos) + splitplane->d > -1e-5f)
		{
			secondvtxremap[idx] = secondvertices.size();
			secondvertices.push_back(vertices[idx]);
		}
		else
			secondvtxremap[idx] = (UINT)-1;
	}

	// >>> Create a list to store open edges

	struct Edge
	{
		IObject::Vertex vtx0, vtx1;
		Edge(const IObject::Vertex* vtx0, const IObject::Vertex* vtx1) : vtx0(*vtx0), vtx1(*vtx1) {}
	};
	std::list<Edge> openedges[2]; // Open edges of first ([0]) and second ([1]) mesh

	// >>> Define a lambda function to split a triangle into three triangles

	IObject::Vertex* v[3];
	IObject::Vertex splitvtx01, splitvtx02;
	auto SplitTrinagle = [&](UINT faceidx, std::vector<IObject::Vertex>& vertices0, std::vector<UINT>& indices0, const UINT* vtxremap0, UINT idx0,
							 std::vector<IObject::Vertex>& vertices12, std::vector<UINT>& indices12, const UINT* vtxremap12, UINT idx1, UINT idx2)
	{
		// Find vertices at the v[idx0]/v[idx1]- and v[idx0]/v[idx2]-intersections with the splitplane
		auto GetSplitVertex = [&](IObject::Vertex* vtxout, const Plane* splitplane, const IObject::Vertex* vtx0, const IObject::Vertex* vtx1)
		{
			vtxout->blendidx = vtx0->blendidx;
			PlaneIntersectLine(&vtxout->pos, splitplane, &vtx0->pos, &vtx1->pos);
			Vector3 v0out = vtxout->pos - vtx0->pos;
			Vector3 v01 = vtx1->pos - vtx0->pos;
			float s = sqrt(Vec3LengthSq(&v0out) / Vec3LengthSq(&v01));
			Vec3Lerp(&vtxout->nml, &vtx0->nml, &vtx1->nml, s);
			Vec2Lerp(&vtxout->texcoord, &vtx0->texcoord, &vtx1->texcoord, s);
		};
		GetSplitVertex(&splitvtx01, splitplane, v[idx0], v[idx1]);
		GetSplitVertex(&splitvtx02, splitplane, v[idx0], v[idx2]);

		// Create triangle between v[idx0], v[idx0]/v[idx1]-intersection and v[idx0]/v[idx2]-intersection
		indices0.push_back(vtxremap0[indices[faceidx * 3 + idx0]]);
		indices0.push_back(vertices0.size()); vertices0.push_back(splitvtx01);
		indices0.push_back(vertices0.size()); vertices0.push_back(splitvtx02);

		// Create triangle between v[idx1], v[idx0]/v[idx2]-intersection and v[idx0]/v[idx1]-intersection
		indices12.push_back(vtxremap12[indices[faceidx * 3 + idx1]]);
		indices12.push_back(vertices12.size()); vertices12.push_back(splitvtx02);
		indices12.push_back(vertices12.size()); vertices12.push_back(splitvtx01);

		// Create triangle between v[idx2], v[idx0]/v[idx2]-intersection and v[idx1]
		indices12.push_back(vtxremap12[indices[faceidx * 3 + idx2]]);
		indices12.push_back(vertices12.size()); vertices12.push_back(splitvtx02);
		indices12.push_back(vtxremap12[indices[faceidx * 3 + idx1]]);
	};

	// >>> Split triangles

	bool onfirst[3], onsecond[3];
	for(UINT idx = 0; idx < numfaces; idx++)
	{
		v[0] = &vertices[indices[idx * 3 + 0]];
		v[1] = &vertices[indices[idx * 3 + 1]];
		v[2] = &vertices[indices[idx * 3 + 2]];
		if(VerticesOverlap(&v[0]->pos, &v[1]->pos) || VerticesOverlap(&v[0]->pos, &v[2]->pos) || VerticesOverlap(&v[1]->pos, &v[2]->pos))
			continue; // Ignore degenerated triangle //EDIT: Why does this happen for a sphere?

		onfirst[0] = firstvtxremap[indices[idx * 3 + 0]] != (UINT)-1;//Vec3Dot(&planenml, &v[0]->pos) + splitplane->d < 1e-5f;
		onfirst[1] = firstvtxremap[indices[idx * 3 + 1]] != (UINT)-1;//Vec3Dot(&planenml, &v[1]->pos) + splitplane->d < 1e-5f;
		onfirst[2] = firstvtxremap[indices[idx * 3 + 2]] != (UINT)-1;//Vec3Dot(&planenml, &v[2]->pos) + splitplane->d < 1e-5f;
		onsecond[0] = secondvtxremap[indices[idx * 3 + 0]] != (UINT)-1;//Vec3Dot(&planenml, &v[0]->pos) + splitplane->d > -1e-5f;
		onsecond[1] = secondvtxremap[indices[idx * 3 + 1]] != (UINT)-1;//Vec3Dot(&planenml, &v[1]->pos) + splitplane->d > -1e-5f;
		onsecond[2] = secondvtxremap[indices[idx * 3 + 2]] != (UINT)-1;//Vec3Dot(&planenml, &v[2]->pos) + splitplane->d > -1e-5f;
		if(onfirst[0] && onfirst[1] && onfirst[2])
		{
			firstindices.push_back(firstvtxremap[indices[idx * 3 + 0]]);
			firstindices.push_back(firstvtxremap[indices[idx * 3 + 1]]);
			firstindices.push_back(firstvtxremap[indices[idx * 3 + 2]]);
			if(onsecond[0] && onsecond[1])
				openedges[0].push_back(Edge(v[0], v[1]));
			else if(onsecond[1] && onsecond[2])
				openedges[0].push_back(Edge(v[1], v[2]));
			else if(onsecond[0] && onsecond[2])
				openedges[0].push_back(Edge(v[0], v[2]));
		}
		else if(onsecond[0] && onsecond[1] && onsecond[2])
		{
			secondindices.push_back(secondvtxremap[indices[idx * 3 + 0]]);
			secondindices.push_back(secondvtxremap[indices[idx * 3 + 1]]);
			secondindices.push_back(secondvtxremap[indices[idx * 3 + 2]]);
			if(onfirst[0] && onfirst[1])
				openedges[1].push_back(Edge(v[0], v[1]));
			else if(onfirst[1] && onfirst[2])
				openedges[1].push_back(Edge(v[1], v[2]));
			else if(onfirst[0] && onfirst[2])
				openedges[1].push_back(Edge(v[0], v[2]));
		}
		else if(onfirst[0] && onsecond[1] && onsecond[2])
		{
			SplitTrinagle(idx, firstvertices, firstindices, firstvtxremap, 0, secondvertices, secondindices, secondvtxremap, 1, 2);

			// Add edge between newly created vertices to open edges of both sides 
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[0].push_back(Edge(&splitvtx01, &splitvtx02));
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[1].push_back(Edge(&splitvtx01, &splitvtx02));
		}
		else if(onsecond[0] && onfirst[1] && onsecond[2])
		{
			SplitTrinagle(idx, firstvertices, firstindices, firstvtxremap, 1, secondvertices, secondindices, secondvtxremap, 2, 0);

			// Add edge between newly created vertices to open edges of both sides 
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[0].push_back(Edge(&splitvtx01, &splitvtx02));
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[1].push_back(Edge(&splitvtx01, &splitvtx02));
		}
		else if(onsecond[0] && onsecond[1] && onfirst[2])
		{
			SplitTrinagle(idx, firstvertices, firstindices, firstvtxremap, 2, secondvertices, secondindices, secondvtxremap, 0, 1);

			// Add edge between newly created vertices to open edges of both sides 
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[0].push_back(Edge(&splitvtx01, &splitvtx02));
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[1].push_back(Edge(&splitvtx01, &splitvtx02));
		}
		else if(onsecond[0] && onfirst[1] && onfirst[2])
		{
			SplitTrinagle(idx, secondvertices, secondindices, secondvtxremap, 0, firstvertices, firstindices, firstvtxremap, 1, 2);

			// Add edge between newly created vertices to open edges of both sides 
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[0].push_back(Edge(&splitvtx01, &splitvtx02));
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[1].push_back(Edge(&splitvtx01, &splitvtx02));
		}
		else if(onfirst[0] && onsecond[1] && onfirst[2])
		{
			SplitTrinagle(idx, secondvertices, secondindices, secondvtxremap, 1, firstvertices, firstindices, firstvtxremap, 2, 0);

			// Add edge between newly created vertices to open edges of both sides 
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[0].push_back(Edge(&splitvtx01, &splitvtx02));
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[1].push_back(Edge(&splitvtx01, &splitvtx02));
		}
		else if(onfirst[0] && onfirst[1] && onsecond[2])
		{
			SplitTrinagle(idx, secondvertices, secondindices, secondvtxremap, 2, firstvertices, firstindices, firstvtxremap, 0, 1);

			// Add edge between newly created vertices to open edges of both sides 
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[0].push_back(Edge(&splitvtx01, &splitvtx02));
			if(!VerticesOverlap(&splitvtx01.pos, &splitvtx02.pos)) openedges[1].push_back(Edge(&splitvtx01, &splitvtx02));
		}
	}

	// Unmap source vertices and indices

	CHKRESULT(UnmapData(vertices));
	CHKRESULT(UnmapData(indices));

	// Triangulate loops of open edges

	std::vector<Edge> edgeloop;
	std::list<Edge>::iterator iter;
	size_t oldopenedgessize;
	IObject::Vertex newvtx;
	for(int i = 0; i < 2; i++)
	{
		std::vector<IObject::Vertex>* _vertices = i == 0 ? &firstvertices : &secondvertices;
		std::vector<UINT>* _indices = i == 0 ? &firstindices : &secondindices;
		while((oldopenedgessize = openedges[i].size()) != 0)
		{
			if(edgeloop.empty()) // If this is a new loop
			{
				// Add any edge from openedges as first edge of loop
				edgeloop.push_back(openedges[i].back());
				openedges[i].pop_back();
			}

			LIST_FOREACH(openedges[i], iter)
			{
				if(VerticesOverlap(&iter->vtx0.pos, &edgeloop.back().vtx1.pos))
				{
					if(VerticesOverlap(&iter->vtx1.pos, &edgeloop.back().vtx0.pos))
					{
						// Ignore identical edge
						openedges[i].erase(iter);
						break; // The iterated list has been altered. Reenter loop
					}

					// Attache Edge(vtx0, vtx1)
					edgeloop.push_back(Edge(&iter->vtx0, &iter->vtx1));
					openedges[i].erase(iter);
				}
				else if(VerticesOverlap(&iter->vtx1.pos, &edgeloop.back().vtx1.pos))
				{
					if(VerticesOverlap(&iter->vtx0.pos, &edgeloop.back().vtx0.pos))
					{
						// Ignore identical edge
						openedges[i].erase(iter);
						break; // The iterated list has been altered. Reenter loop
					}

					// Attache Edge(vtx1, vtx0)
					edgeloop.push_back(Edge(&iter->vtx1, &iter->vtx0));
					openedges[i].erase(iter);
				}
				else
					continue;

				// Is the loop already closed?
				if(VerticesOverlap(&edgeloop.front().vtx0.pos, &edgeloop.back().vtx1.pos))
				{
					// Find out loop direction
					Vector3 cross, nml;

					auto AddVertex = [&](const IObject::Vertex* oldvertex, const Vector3& nml)
					{
						newvtx.pos = oldvertex->pos;
						newvtx.blendidx = oldvertex->blendidx;
						newvtx.texcoord = oldvertex->texcoord;
						newvtx.nml = nml;//Vector3(1.0f, 0.0f, 0.0f);
						_vertices->push_back(newvtx);
					};

					// Triangulate loop //EDIT: Works only for convex loops
					size_t edgeloopsize = edgeloop.size();
					size_t edgeloopstartidx = _vertices->size();
					bool firstvertexdefined = false;
					for(size_t e = 1; e < edgeloopsize - 1; e++)
					{
						Vector3 v0eplus0 = edgeloop[e + 0].vtx0.pos - edgeloop[0].vtx0.pos;
						Vector3 v0eplus1 = edgeloop[e + 1].vtx0.pos - edgeloop[0].vtx0.pos;
						float dot = Vec3Dot(&planenml, Vec3Cross(&cross, &v0eplus0, &v0eplus1));
						if(dot < 1e-5f && dot > -1e-5f)
							continue; // Skip triangle (i.e. if all three vertices lie in a streight line)
						bool inverse = i != (int)(dot > 0.0f);
						if(i)
							Vec3Scale(&nml, &planenml, -1.0f);
						else
							nml = planenml;

						if(!firstvertexdefined)
						{
							AddVertex(&edgeloop[0].vtx0, nml);
							firstvertexdefined = true;
						}

						_indices->push_back(edgeloopstartidx);
						_indices->push_back(_vertices->size()); AddVertex(&edgeloop[e + !inverse].vtx0, nml);
						_indices->push_back(_vertices->size()); AddVertex(&edgeloop[e + inverse].vtx0, nml);
					}

					edgeloop.clear();
				}
				break; // The iterated list has been altered. Reenter loop
			}

			// Avoid endless loops
			if(openedges[i].size() == oldopenedgessize)
			{
				Result::PrintLogMessage("Warning: Open loop couldn't be triangulated");
				edgeloop.clear();
				break;
			}
		}
	}

	// >>> Create first object

	if(firstindices.empty())
		*first = NULL;
	else
	{
		Object* firstobj;
		CHKALLOC(*first = firstobj = new Object());
		CHKRESULT(firstobj->Create(this, false));
		(*firstobj->refcounter)--; firstobj->refcounter = new UINT(1);
		firstobj->mesh = new Mesh();
		CHKRESULT(firstobj->mesh->Init(sizeof(Vertex), firstvertices.size(), firstindices.size() / 3));
		firstobj->ComputeBoundingSphere(&firstvertices[0], firstvertices.size()); //EDIT: DO after optimization to exclude unused vertices
		CHKRESULT(firstobj->SetVertexData(&firstvertices[0]));
		CHKRESULT(firstobj->SetIndexData(&firstindices[0]));
UINT foo_old = firstobj->GetVertexCount();
		/*CHKRESULT(firstobj->mesh->Optimize(D3DX10_MESHOPT_COMPACT, NULL, NULL)); // Remove unused vertices*/ //EDIT11
UINT foo_new = firstobj->GetVertexCount();
		CHKRESULT(firstobj->MapVertexData(&vertices)); // Get optimized vertices
		firstobj->ComputeBoundingSphere(vertices, firstobj->GetVertexCount());
		CHKRESULT(firstobj->UnmapData(vertices));
		CHKRESULT(firstobj->CommitChanges());
	}

	// >>> Create second object

	if(secondindices.empty())
		*second = NULL;
	else
	{
		Object* secondobj;
		CHKALLOC(*second = secondobj = new Object());
		CHKRESULT(secondobj->Create(this, false));
		(*secondobj->refcounter)--; secondobj->refcounter = new UINT(1);
		secondobj->mesh = new Mesh();
		CHKRESULT(secondobj->mesh->Init(sizeof(Vertex), secondvertices.size(), secondindices.size() / 3));
		CHKRESULT(secondobj->SetVertexData(&secondvertices[0]));
		CHKRESULT(secondobj->SetIndexData(&secondindices[0]));
		/*CHKRESULT(secondobj->mesh->Optimize(D3DX10_MESHOPT_COMPACT, NULL, NULL)); // Remove unused vertices */ //EDIT11
		CHKRESULT(secondobj->MapVertexData(&vertices)); // Get optimized vertices
		secondobj->ComputeBoundingSphere(vertices, secondobj->GetVertexCount());
		CHKRESULT(secondobj->UnmapData(vertices));
		CHKRESULT(secondobj->CommitChanges());
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Break()
// Desc: Create numparts new objects by splitting the source object numsplits times by a random plane
//-----------------------------------------------------------------------------
Result Object::Break(int targetnumparts, LPOBJECT* parts, int* numparts)
{
	LPOBJECT biggestpart, splitparts[2];
	int biggestpartidx;
	Plane splitplane;
	Vector3 planenml;
	Result rlt;

	parts[0] = this;

	int numpartsdone = 1;
	int retries = targetnumparts; // Can be any number
	while(numpartsdone < targetnumparts && retries)//for(int i = 0; numpartsdone < targetnumparts; i++)
	{
		// Find biggest part
		float biggestvolume = parts[0]->GetBoundingBoxSize().x * parts[0]->GetBoundingBoxSize().y * parts[0]->GetBoundingBoxSize().z;
		biggestpartidx = 0;
		for(int j = 1; j < numpartsdone; j++)
			if(parts[j]->GetBoundingBoxSize().x * parts[j]->GetBoundingBoxSize().y * parts[j]->GetBoundingBoxSize().z > biggestvolume)
			{
				biggestvolume = parts[j]->GetBoundingBoxSize().x * parts[j]->GetBoundingBoxSize().y * parts[j]->GetBoundingBoxSize().z;
				biggestpartidx = j;
			}
		biggestpart = parts[biggestpartidx];

		// Find split plane
		float a = (float)rand() / (float)RAND_MAX - 0.5f; //a *= 0.5f;
		float b = (float)rand() / (float)RAND_MAX - 0.5f; //b *= 0.5f;
		float c = (float)rand() / (float)RAND_MAX - 0.5f; c *= 0.5f * biggestpart->GetBoundingSphereRadius();
		if(biggestpart->GetBoundingBoxSize().x > biggestpart->GetBoundingBoxSize().y)
		{
			if(biggestpart->GetBoundingBoxSize().x > biggestpart->GetBoundingBoxSize().z)
				{planenml.x = 1.0f; planenml.y = a; planenml.z = b;}
			else
				{planenml.x = a; planenml.y = b; planenml.z = 1.0f;}
		}
		else
		{
			if(biggestpart->GetBoundingBoxSize().y > biggestpart->GetBoundingBoxSize().z)
				{planenml.x = a; planenml.y = 1.0f; planenml.z = b;}
			else
				{planenml.x = a; planenml.y = b; planenml.z = 1.0f;}
		}
		PlaneFromPointNormal(&splitplane, &biggestpart->GetBoundingBoxCenter(), &planenml);
		PlaneNormalize(&splitplane, &splitplane); //EDIT: Is the plane from PlaneFromPointNormal() already normalized?
		splitplane.d += c;

		// Split part
		CHKRESULT(biggestpart->Split(&splitplane, &splitparts[0], &splitparts[1]));
float factor = (float)(targetnumparts - numpartsdone) / (float)targetnumparts;
if(splitparts[0])
{
	Vector3 scaledplanenml;
	Vec3Scale(&scaledplanenml, &planenml, -factor);
	Vec3Add(&splitparts[0]->pos, &splitparts[0]->pos, &scaledplanenml);
}
if(splitparts[1])
{
	Vector3 scaledplanenml;
	Vec3Scale(&scaledplanenml, &planenml, factor);
	Vec3Add(&splitparts[1]->pos, &splitparts[1]->pos, &scaledplanenml);
}

		if(splitparts[0] && splitparts[1])
		{
			// Splitting successful
			parts[biggestpartidx] = splitparts[0];
			parts[numpartsdone++] = splitparts[1];
			if(biggestpart != this)
				wnd->RemoveObject(biggestpart);
		}
		else
		{
			// Splitting failed
			if(splitparts[0])
				wnd->RemoveObject(splitparts[0]);
			if(splitparts[0])
				wnd->RemoveObject(splitparts[1]);
			retries--;
		}
	}

	if(numpartsdone == 1)
		CHKRESULT(wnd->CreateObject(this, false, &parts[0]));

	*numparts = numpartsdone;
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Draw object to the device
//-----------------------------------------------------------------------------
void Object::Render(RenderType rendertype)
{
	if(!shader || !visible)
		return;

	// Raise event
	if(beforerendercbk)
	{
		bool usercancel = false;
		beforerendercbk(this, usercancel, beforerendercbkuser);
		if(usercancel)
			return;
	}

	if(orientmode != OM_MATRIX)
	{
		Matrix mScale, mRot, mTrans;
		MatrixScaling(&mScale, scl.x, scl.y, scl.z);
		MatrixTranslation(&mTrans, pos.x, pos.y, pos.z);
		if(orientmode == OM_EULER)
		{
			Matrix mRotX, mRotY, mRotZ;
			MatrixRotationX(&mRotX, rot.x);
			MatrixRotationY(&mRotY, -rot.y);
			MatrixRotationZ(&mRotZ, rot.z);
			mRot = mRotY * mRotX * mRotZ;
		}
		else // orientmode == OM_QUAT
			MatrixRotationQuaternion(&mRot, &qrot);
		transform = mScale * mRot * mTrans;
	}

	if(numblendparts == 1)
		bonetransform[0] = transform;

	/*wnd->devspec->devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);*/

	UINT numpasses;
	shader->PrepareRendering(iscameradependent ? wnd->cam : NULL, &transform, &bonetransform[0], numblendparts, &numpasses);

	for(UINT subset = 0; subset < mats->size(); subset++)
	{
		for(UINT passidx = 0; passidx < numpasses; passidx++)
		{
			//shader->PreparePass(passidx);
			shader->PrepareSubset((Texture*)(*mats)[subset].tex, &(*mats)[subset].ambient, &(*mats)[subset].diffuse, &(*mats)[subset].specular, (*mats)[subset].power, passidx);
			mesh->DrawSubset(subset);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: CreateCollisionMesh()
// Desc: ?
//-----------------------------------------------------------------------------
Result Object::CreateCollisionMesh(LPREGULARENTITY* entity)
{
	if(!hvk)
		return ERR("Havok Physics not initialized");

	Result rlt;

	MeshBuffer *vertexbuffer, *indexbuffer;
	BYTE* vertices;
	UINT* indices;
	UINT size;

	CHKRESULT(mesh->GetVertexBuffer(&vertexbuffer));
	CHKRESULT(vertexbuffer->Map((LPVOID*)&vertices, &size));

	CHKRESULT(mesh->GetIndexBuffer(&indexbuffer));
	CHKRESULT(indexbuffer->Map((LPVOID*)&indices, &size));

	HvkCompressedMeshShapeDesc desc;
	desc.mtype = MT_FIXED;
	desc.qual = CQ_FIXED;
	desc.indices = indices;
	desc.numvertices = mesh->GetVertexCount();
	desc.numfaces = mesh->GetFaceCount();
	if(scl.x == 1.0f && scl.y == 1.0f && scl.z == 1.0f)
	{
		desc.vertexdata = (BYTE*)vertices;
		desc.fvfstride = 36; //EDIT: Remove hard-coded stride
		CHKRESULT(hvk->CreateRegularEntity(&desc, IHavok::LAYER_LANDSCAPE, &pos, &qrot, entity));
	}
	else
	{
		float* scaledvertices;
		CHKALLOC(scaledvertices = new float[desc.numvertices * 3]);
		for(UINT i = 0; i < desc.numvertices; i++)
		{
			scaledvertices[3 * i + 0] = ((float*)&vertices[36 * i])[0] * scl.x; //EDIT: Remove hard-coded stride
			scaledvertices[3 * i + 1] = ((float*)&vertices[36 * i])[1] * scl.y;
			scaledvertices[3 * i + 2] = ((float*)&vertices[36 * i])[2] * scl.z;
		}
		desc.vertexdata = (BYTE*)scaledvertices;
		desc.fvfstride = 3 * sizeof(float);
		CHKRESULT(hvk->CreateRegularEntity(&desc, IHavok::LAYER_LANDSCAPE, &pos, &qrot, entity));
		delete[] scaledvertices;
	}

	CHKRESULT(vertexbuffer->Unmap());
	CHKRESULT(indexbuffer->Unmap());

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateConvexCollisionHull()
// Desc: ?
//-----------------------------------------------------------------------------
Result Object::CreateConvexCollisionHull(LPREGULARENTITY* entity)
{
	if(!hvk)
		return ERR("Havok Physics not initialized");

	Result rlt;

	MeshBuffer* vertexbuffer;
	BYTE* vertices;
	UINT size;

	CHKRESULT(mesh->GetVertexBuffer(&vertexbuffer));
	CHKRESULT(vertexbuffer->Map((LPVOID*)&vertices, &size));

	HvkConvexVerticesShapeDesc desc;
	desc.mtype = MT_DYNAMIC;
	desc.qual = CQ_MOVING;
	desc.mass = 5.0f;
	desc.numvertices = mesh->GetVertexCount();

	orientmode = OM_QUAT;
	if(scl.x == 1.0f && scl.y == 1.0f && scl.z == 1.0f)
	{
		desc.vertices = (BYTE*)vertices;
		desc.fvfstride = 36; //EDIT: Remove hard-coded stride
		CHKRESULT(hvk->CreateRegularEntity(&desc, IHavok::LAYER_HANDHELD, &pos, &qrot, entity));
	}
	else
	{
		float* scaledvertices;
		CHKALLOC(scaledvertices = new float[desc.numvertices * 3]);
		for(int i = 0; i < desc.numvertices; i++)
		{
			scaledvertices[3 * i + 0] = ((float*)&vertices[36 * i])[0] * scl.x; //EDIT: Remove hard-coded stride
			scaledvertices[3 * i + 1] = ((float*)&vertices[36 * i])[1] * scl.y;
			scaledvertices[3 * i + 2] = ((float*)&vertices[36 * i])[2] * scl.z;
		}
		desc.vertices = (BYTE*)scaledvertices;
		desc.fvfstride = 3 * sizeof(float);
		CHKRESULT(hvk->CreateRegularEntity(&desc, IHavok::LAYER_HANDHELD, &pos, &qrot, entity));
		delete[] scaledvertices;
	}

	CHKRESULT(vertexbuffer->Unmap());

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateCollisionEllipsoid()
// Desc: ?
//-----------------------------------------------------------------------------
Result Object::CreateCollisionCapsule(float radius, LPREGULARENTITY* entity)
{
	if(!hvk)
		return ERR("Havok Physics not initialized");

	Result rlt;

	MeshBuffer *vertexbuffer;
	UINT numvertices = mesh->GetVertexCount();
	Vertex* vertices;
	UINT size;

	CHKRESULT(mesh->GetVertexBuffer(&vertexbuffer));
	CHKRESULT(vertexbuffer->Map((LPVOID*)&vertices, &size));

	float ymin = 1e20f, ymax = -1e20f;
	for(UINT i = 0; i < numvertices; i++)
	{
		ymin = min(ymin, vertices[i].pos.y);
		ymax = max(ymax, vertices[i].pos.y);
	}
	ymin *= scl.y;
	ymax *= scl.y;
	Vector3 v0(boundingsphere.center.x, ymin + radius, boundingsphere.center.z), v1(boundingsphere.center.x, ymax - radius, boundingsphere.center.z);

	CHKRESULT(vertexbuffer->Unmap());

	HvkCapsuleShapeDesc desc;
	desc.mtype = MT_DYNAMIC;
	desc.qual = CQ_MOVING;
	desc.mass = 10;
	desc.v0 = v0;
	desc.v1 = v1;
	desc.radius = radius;

	orientmode = OM_QUAT;
	CHKRESULT(hvk->CreateRegularEntity(&desc, IHavok::LAYER_HANDHELD, &pos, &qrot, entity));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateCollisionRagdoll()
// Desc: ?
//-----------------------------------------------------------------------------
Result Object::CreateCollisionRagdoll(float radius, bool usedynamicmotion, LPRAGDOLLENTITY* entity)
{
	if(!hvk)
		return ERR("Havok Physics not initialized");

	HvkCapsuleShapeDesc desc;
	LimitedHingeConstraintDesc lhcdesc;
	RagdollConstraintDesc rcdesc;
	BallAndSocketConstraintDesc bsdesc;
	Result rlt;

MeshBuffer *vertexbuffer;
UINT numvertices = mesh->GetVertexCount();
Vertex* vertices;
UINT size;

CHKRESULT(mesh->GetVertexBuffer(&vertexbuffer));
CHKRESULT(vertexbuffer->Map((LPVOID*)&vertices, &size));

float ymin = 1e20f, ymax = -1e20f;
for(UINT i = 0; i < numvertices; i++)
{
	ymin = min(ymin, vertices[i].pos.y);
	ymax = max(ymax, vertices[i].pos.y);
}
ymin *= scl.y;
ymax *= scl.y;
Vector3 v0(boundingsphere.center.x, ymin + radius, boundingsphere.center.z), v1(boundingsphere.center.x, ymax - radius, boundingsphere.center.z);

CHKRESULT(vertexbuffer->Unmap());

desc.mtype = MT_DYNAMIC;
desc.qual = CQ_MOVING;
desc.mass = 70;
desc.v0 = v0;
desc.v1 = v1;
desc.radius = radius;

	orientmode = OM_QUAT;
	CHKRESULT(hvk->CreateRagdollEntity(&desc, &pos, &qrot, entity));
	LPRAGDOLLENTITY _entity = (LPRAGDOLLENTITY)*entity;

	// >>> Torsor

	desc.mtype = MT_KEYFRAMED; //MT_DYNAMIC;
	desc.qual = CQ_KEYFRAMED; //CQ_MOVING;
	desc.radius = 0.2f;

	desc.mass = 0.43f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.31f, 0.0f);
	desc.v1 = Vector3(0.0f, -0.3f, 0.0f);
	desc.v0[1] -= desc.radius;
	desc.v1[1] += desc.radius;

	CHKRESULT(_entity->SetRootBone(&desc, String("Torsor"), &bonetransform[0]));

	// >>> Upper left arm

	desc.mtype = usedynamicmotion ? MT_DYNAMIC : MT_KEYFRAMED;
	desc.qual = usedynamicmotion ? CQ_MOVING : CQ_KEYFRAMED;
	desc.radius = 0.05f;

	desc.mass = 0.03f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.0f, 0.0f);//Vector3(-0.16f, 0.31f, 0.0f);
	desc.v1 = Vector3(-0.27f, 0.0f, 0.0f);//Vector3(-0.43f, 0.31f, 0.0f);
	bonepos[1] = Vector3(-0.16f, 0.31f, 0.0f);//Vector3(desc.v0);
	rcdesc.pivot = bonepos[1];
bsdesc.pivot = bonepos[1];
	desc.v0[0] -= desc.radius;
	desc.v1[0] += desc.radius;

	lhcdesc.angleMin = 10.0f * PI / 180.0f;
	lhcdesc.angleMax = 80.0f * PI / 180.0f;

	rcdesc.coneMin = -86.0f * PI / 180.0f;
	rcdesc.coneMax =  75.0f * PI / 180.0f;
	rcdesc.planeMin = -65.0f * PI / 180.0f;
	rcdesc.planeMax =   5.0f * PI / 180.0f;
	rcdesc.twistMin = -5.0f * PI / 180.0f;
	rcdesc.twistMax =  5.0f * PI / 180.0f;
	rcdesc.twistAxis = Vector3(1.0f,  0.0f, 0.0f);
	rcdesc.planeAxis = Vector3(0.0f, -1.0f, 0.0f);

	CHKRESULT(_entity->AttachShape(&desc, &bsdesc, String("UpperLeftArm"), String("Torsor"), &bonetransform[1]));

	// >>> Lower left arm

	desc.mass = 0.02f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.0f, 0.0f);//Vector3(-0.43f, 0.31f, 0.0f);
	desc.v1 = Vector3(-0.27f, 0.0f, 0.0f);//Vector3(-0.70f, 0.31f, 0.0f);
	bonepos[2] = Vector3(-0.43f, 0.31f, 0.0f);//Vector3(desc.v0);
	lhcdesc.pivot = bonepos[2];
bsdesc.pivot = bonepos[2];
	lhcdesc.axis = Vector3(0.0f, 0.0f, 1.0f);
	desc.v0[0] -= desc.radius;
	desc.v1[0] += desc.radius;

	CHKRESULT(_entity->AttachShape(&desc, &bsdesc, String("LowerLeftArm"), String("UpperLeftArm"), &bonetransform[2]));

	// >>> Upper right arm

	desc.mass = 0.03f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.0f, 0.0f);//Vector3(0.16f, 0.31f, 0.0f);
	desc.v1 = Vector3(0.27f, 0.0f, 0.0f);//Vector3(0.43f, 0.31f, 0.0f);
	bonepos[3] = Vector3(0.16f, 0.31f, 0.0f);//Vector3(desc.v0);
	rcdesc.pivot = bonepos[3];
bsdesc.pivot = bonepos[3];
	desc.v0[0] += desc.radius;
	desc.v1[0] -= desc.radius;

	rcdesc.twistAxis = Vector3(-1.0f,  0.0f, 0.0f);
	rcdesc.planeAxis = Vector3( 0.0f, -1.0f, 0.0f);

	CHKRESULT(_entity->AttachShape(&desc, &bsdesc, String("UpperRightArm"), String("Torsor"), &bonetransform[3]));

	// >>> Lower right arm

	desc.mass = 0.02f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.0f, 0.0f);//Vector3(0.43f, 0.31f, 0.0f);
	desc.v1 = Vector3(0.27f, 0.0f, 0.0f);//Vector3(0.70f, 0.31f, 0.0f);
	bonepos[4] = Vector3(0.43f, 0.31f, 0.0f);//Vector3(desc.v0);
	lhcdesc.pivot = bonepos[4];
bsdesc.pivot = bonepos[4];
	lhcdesc.axis = Vector3(0.0f, 0.0f, -1.0f);
	desc.v0[0] += desc.radius;
	desc.v1[0] -= desc.radius;

	CHKRESULT(_entity->AttachShape(&desc, &bsdesc, String("LowerRightArm"), String("UpperRightArm"), &bonetransform[4]));

	// >>> Upper left leg

	desc.mass = 0.12f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.0f, 0.0f);//Vector3(-0.1f, -0.3f, 0.0f);
	desc.v1 = Vector3(0.0f, -0.37f, 0.0f);//Vector3(-0.1f, -0.67f, 0.0f);
	rcdesc.pivot = bonepos[5] = Vector3(-0.1f, -0.3f, 0.0f);//Vector3(desc.v0);
bsdesc.pivot = bonepos[5];
	desc.v0[1] -= desc.radius;
	desc.v1[1] += desc.radius;

	rcdesc.coneMin = -45.0f * PI / 180.0f;
	rcdesc.coneMax = 10.0f * PI / 180.0f;
	rcdesc.planeMin = -15.0f * PI / 180.0f;
	rcdesc.planeMax = 15.0f * PI / 180.0f;
	rcdesc.twistMin = -5.0f * PI / 180.0f;
	rcdesc.twistMax = 5.0f * PI / 180.0f;
	rcdesc.twistAxis = Vector3(0.0f, 0.0f, 1.0f);
	rcdesc.planeAxis = Vector3(1.0f, 0.0f, 0.0f);

	CHKRESULT(_entity->AttachShape(&desc, &bsdesc, String("UpperLeftLeg"), String("Torsor"), &bonetransform[5]));

	desc.mass = 0.05f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.0f, 0.0f);//Vector3(-0.1f, -0.67f, 0.0f);
	desc.v1 = Vector3(0.0f, -0.37f, 0.0f);//Vector3(-0.1f, -1.04f, 0.0f);
	rcdesc.pivot = bonepos[6] = Vector3(-0.1f, -0.67f, 0.0f);//Vector3(desc.v0);
bsdesc.pivot = bonepos[6];
	desc.v0[1] -= desc.radius;
	desc.v1[1] += desc.radius;

	CHKRESULT(_entity->AttachShape(&desc, &bsdesc, String("LowerLeftLeg"), String("UpperLeftLeg"), &bonetransform[6]));

	desc.mass = 0.12f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.0f, 0.0f);//Vector3(0.1f, -0.3f, 0.0f);
	desc.v1 = Vector3(0.0f, -0.37f, 0.0f);//Vector3(0.1f, -0.67f, 0.0f);
	rcdesc.pivot = bonepos[7] = Vector3(0.1f, -0.3f, 0.0f);//Vector3(desc.v0);
bsdesc.pivot = bonepos[7];
	desc.v0[1] -= desc.radius;
	desc.v1[1] += desc.radius;

	CHKRESULT(_entity->AttachShape(&desc, &bsdesc, String("UpperRightLeg"), String("Torsor"), &bonetransform[7]));

	desc.mass = 0.05f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.0f, 0.0f);//Vector3(0.1f, -0.67f, 0.0f);
	desc.v1 = Vector3(0.0f, -0.37f, 0.0f);//Vector3(0.1f, -1.04f, 0.0f);
	rcdesc.pivot = bonepos[8] = Vector3(0.1f, -0.67f, 0.0f);//Vector3(desc.v0);
bsdesc.pivot = bonepos[8];
	desc.v0[1] -= desc.radius;
	desc.v1[1] += desc.radius;

	CHKRESULT(_entity->AttachShape(&desc, &bsdesc, String("LowerRightLeg"), String("UpperRightLeg"), &bonetransform[8]));

	desc.mass = 0.01f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.0f, 0.0f);//Vector3(0.0f, 0.31f, 0.0f);
	desc.v1 = Vector3(0.0f, 0.13f, 0.0f);//Vector3(0.0f, 0.44f, 0.0f);
	rcdesc.pivot = bonepos[10] = Vector3(0.0f, 0.31f, 0.0f);//Vector3(desc.v0);

	CHKRESULT(_entity->AttachShape(&desc, &rcdesc, String("Neck"), String("Torsor"), &bonetransform[10]));

	desc.mass = 0.07f * 70.0f;
	desc.v0 = Vector3(0.0f, 0.0f, 0.0f);//Vector3(0.0f, 0.44f, 0.0f);
	desc.v1 = Vector3(0.0f, 0.04f, 0.0f);//Vector3(0.0f, 0.48f, 0.0f);
	rcdesc.pivot = bonepos[9] = Vector3(0.0f, 0.44f, 0.0f);//Vector3(desc.v0);

	CHKRESULT(_entity->AttachShape(&desc, &rcdesc, String("Head"), String("Neck"), &bonetransform[9]));

	CHKRESULT(_entity->AssembleRagdoll());

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetShader()
// Desc: Apply shader and sync vertex declaration with it
//-----------------------------------------------------------------------------
Result Object::SetShader(IRenderShader* shader)
{
	if(!shader)
		return ERR("Parameter shader is NULL");
	this->shader = reinterpret_cast<RenderShader*>(shader);

	return this->shader->SetVtxDecl(/*VTX_DECL, 4*/); //EDIT: Vertex declaration may be changed if this shader is used by multiple objects
}

//-----------------------------------------------------------------------------
// Name: SetVertexData()
// Desc: Set vertices (call CommitChanges() to apply changes)
//-----------------------------------------------------------------------------
Result Object::SetVertexData(const Vertex* vertices)
{
	return CheckResult(mesh->SetVertexData((LPVOID)vertices));
}

//-----------------------------------------------------------------------------
// Name: SetIndexData()
// Desc: Set indices (call CommitChanges() to apply changes)
//-----------------------------------------------------------------------------
Result Object::SetIndexData(const UINT* indices)
{
	return CheckResult(mesh->SetIndexData((LPVOID)indices, mesh->GetFaceCount() * 3));
}

//-----------------------------------------------------------------------------
// Name: GetVertexCount()
// Desc: Get number of vertices in the mesh
//-----------------------------------------------------------------------------
UINT Object::GetVertexCount()
{
	return mesh->GetVertexCount();
}

//-----------------------------------------------------------------------------
// Name: GetFaceCount()
// Desc: Get number of triangles in the mesh (== GetIndexCount() / 3)
//-----------------------------------------------------------------------------
UINT Object::GetFaceCount()
{
	return mesh->GetFaceCount();
}

//-----------------------------------------------------------------------------
// Name: GetIndexCount()
// Desc: Get number of indices in the mesh (== GetFaceCount() * 3)
//-----------------------------------------------------------------------------
UINT Object::GetIndexCount()
{
	return 3 * mesh->GetFaceCount();
}

//-----------------------------------------------------------------------------
// Name: MapVertexData()
// Desc: Get vertices (call UnmapVertexData() when done)
//-----------------------------------------------------------------------------
Result Object::MapVertexData(Vertex** vertices)
{
	MeshBuffer* vtxbuffer;
	UINT buffersize;
	Result rlt;

	*vertices = NULL;

	CHKRESULT(mesh->GetVertexBuffer(&vtxbuffer));
	CHKRESULT(vtxbuffer->Map((LPVOID*)vertices, &buffersize));
	mappedbuffers[*vertices] = vtxbuffer;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: MapIndexData()
// Desc: Get indices (call UnmapData() when done)
//-----------------------------------------------------------------------------
Result Object::MapIndexData(UINT** indices)
{
	MeshBuffer* idxbuffer;
	UINT buffersize;
	Result rlt;

	*indices = NULL;

	CHKRESULT(mesh->GetIndexBuffer(&idxbuffer));
	CHKRESULT(idxbuffer->Map((LPVOID*)indices, &buffersize));
	mappedbuffers[*indices] = idxbuffer;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: UnmapData()
// Desc: Release mapped data
//-----------------------------------------------------------------------------
Result Object::UnmapData(LPVOID buffer)
{
	std::map<LPVOID, MeshBuffer*>::const_iterator pair = mappedbuffers.find(buffer);
	if(pair != mappedbuffers.end())
	{
		Result rlt = CheckResult(pair->second->Unmap());
		mappedbuffers.erase(pair);
		return rlt;
	}
	return R_OK;
}

void Object::GetAttributeTable(AttributeRange* attributetable, UINT* attributetablesize)
{
	mesh->GetAttributeTable(attributetable, attributetablesize);
}
Result Object::SetAttributeTable(const AttributeRange* attributetable, UINT attributetablesize)
{
	if(attributetablesize > mats->size())
	{
		OGLMaterial defmat;
		defmat.ambient = Color(0.1f, 0.1f, 0.1f, 1.0f);
		defmat.diffuse = Color(0.8f, 0.8f, 0.8f, 1.0f);
		defmat.specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
		defmat.power = 10.0f;
		defmat.tex = NULL;
		mats->resize(attributetablesize, defmat);
	}
	else
		for(UINT i = mats->size() - attributetablesize; i; i--)
			mats->pop_back();
	return mesh->SetAttributeTable(attributetable, attributetablesize);
}

//-----------------------------------------------------------------------------
// Name: CommitChanges()
// Desc: Commit changes in vertex and index buffer to the GPU and generate adjacency
//-----------------------------------------------------------------------------
Result Object::CommitChanges()
{
	Result rlt;
	CHKRESULT(mesh->CommitToDevice());
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: ComputeNormals()
// Desc: Compute face normals based on vertex and index data
//-----------------------------------------------------------------------------
Result Object::ComputeNormals()
{
	MeshBuffer *vtxbuffer, *idxbuffer;
	IObject::Vertex* vertices;
	UINT *indices;
	UINT size;
	Result rlt;

	// Lock vertex, index and ajacency buffer
	CHKRESULT(mesh->GetVertexBuffer(&vtxbuffer));
	CHKRESULT(vtxbuffer->Map((LPVOID*)&vertices, &size));
	CHKRESULT(mesh->GetIndexBuffer(&idxbuffer));
	CHKRESULT(idxbuffer->Map((LPVOID*)&indices, &size));

	/*// Compute normals
	Vector3 v0, v1, v2, vN, vNa1, vNa2, vNa3;
	for(int i = 0; i < mesh->GetFaceCount() * 3; i += 3)
	{
		v0 = vertices[indices[i + 0]].pos;
		v1 = vertices[indices[i + 1]].pos;
		v2 = vertices[indices[i + 2]].pos;
		Vec3Cross(&vN, &Vector3(v1 - v0), &Vector3(v2 - v0));
		Vec3Normalize(&vN, &vN);

		if(adjacency[i + 0] != 0xFFFFFFFF)
		{
			v0 = vertices[indices[adjacency[i + 0] + 0]].pos;
			v1 = vertices[indices[adjacency[i + 0] + 1]].pos;
			v2 = vertices[indices[adjacency[i + 0] + 2]].pos;
			Vec3Cross(&vNa1, &Vector3(v1 - v0), &Vector3(v2 - v0));
			Vec3Normalize(&vNa1, &vNa1);
			Vec3Normalize(&vNa1, &(vNa1 + vN));
		}
		else
			vNa1 = vN;

		if(adjacency[i + 1] != 0xFFFFFFFF)
		{
			v0 = vertices[indices[adjacency[i + 1] + 0]].pos;
			v1 = vertices[indices[adjacency[i + 1] + 1]].pos;
			v2 = vertices[indices[adjacency[i + 1] + 2]].pos;
			Vec3Cross(&vNa2, &Vector3(v1 - v0), &Vector3(v2 - v0));
			Vec3Normalize(&vNa2, &vNa2);
			Vec3Normalize(&vNa2, &(vNa2 + vN));
		}
		else
			vNa2 = vN;

		if(adjacency[i + 2] != 0xFFFFFFFF)
		{
			v0 = vertices[indices[adjacency[i + 2] + 0]].pos;
			v1 = vertices[indices[adjacency[i + 2] + 1]].pos;
			v2 = vertices[indices[adjacency[i + 2] + 2]].pos;
			Vec3Cross(&vNa3, &Vector3(v1 - v0), &Vector3(v2 - v0));
			Vec3Normalize(&vNa3, &vNa3);
			Vec3Normalize(&vNa3, &(vNa3 + vN));
		}
		else
			vNa3 = vN;


		vertices[indices[i + 0]].nml = vNa1;
		vertices[indices[i + 1]].nml = vNa2;
		vertices[indices[i + 2]].nml = vNa3;
	}*/

Vector3 v[3], vN, v01, v12;
for(UINT i = 0; i < mesh->GetFaceCount() * 3; i += 3)
{
	v[0] = vertices[indices[i + 0]].pos;
	v[1] = vertices[indices[i + 1]].pos;
	v[2] = vertices[indices[i + 2]].pos;
	Vec3Sub(&v01, &v[1], &v[0]);
	Vec3Sub(&v12, &v[2], &v[1]);
	Vec3Cross(&vN, &v01, &v12);

for(UINT j = 0; j < 3; j++)
{
	Vector3 a = v[(j+1) % 3] - v[j];
	Vector3 b = v[(j+2) % 3] - v[j];
	float weight = acos(Vec3Dot(&a, &b) / (Vec3Length(&a) * Vec3Length(&b)));
	vertices[indices[i + j]].nml = vN * weight;
}


	//vertices[indices[i + 0]].nml = vN;
	//vertices[indices[i + 1]].nml = vN;
	//vertices[indices[i + 2]].nml = vN;
}
for(UINT i = 0; i < mesh->GetVertexCount(); i++)
	Vec3Normalize(&vertices[i].nml, &vertices[i].nml);

	// Unlock vertex, index and ajacency buffer
	CHKRESULT(vtxbuffer->Unmap());
	CHKRESULT(idxbuffer->Unmap());

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Object::Release()
{
	// Remove shared data
	if(--(*refcounter) == 0)
		REMOVE(mesh);
	if(--(*matrefcounter) == 0)
		REMOVE(mats);

	// Remove per-instance data

	delete this;
}

LPOBJECT IRenderable::GetObject()
{
	return dynamic_cast<LPOBJECT>(this);
}