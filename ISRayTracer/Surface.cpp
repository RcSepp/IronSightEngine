#include "RayTracer.h"
#include "IntersectRayAABB.h"
#include "RayCastCube.h"
#include "RaySpace.h"

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif


const Matrix* Surface::SetTransform(const Matrix* newtransform)
{
	if(newtransform)
	{
		if(!transform)
		{
			transform = new Matrix(*newtransform);
			invtransform = new Matrix();
			transform_invtrans = new Matrix();
			invtransform_invtrans = new Matrix();
		}
		else
			*transform = *newtransform;

		MatrixInverse(invtransform, transform);

		MatrixTranspose(transform_invtrans, invtransform);
		MatrixTranspose(invtransform_invtrans, transform);
	}
	else if(transform)
	{
		delete transform; transform = NULL;
		delete invtransform; invtransform = NULL;
		delete transform_invtrans; transform_invtrans = NULL;
		delete invtransform_invtrans; invtransform_invtrans = NULL;
	}

	return newtransform;
}
void Surface::SetTransform(const Vector3* translation, const Vector3* rotation, const Vector3* scale)
{
	if(translation->x != 0.0f || translation->y != 0.0f || translation->z != 0.0f ||
	   rotation->x != 0.0f || rotation->y != 0.0f || rotation->z != 0.0f ||
	   scale->x != 1.0f || scale->y != 1.0f || scale->z != 1.0f)
	{
		if(!transform)
		{
			transform = new Matrix();
			invtransform = new Matrix();
			transform_invtrans = new Matrix();
			invtransform_invtrans = new Matrix();
		}

		Matrix s, r1, r2, r3, t;
		MatrixScaling(&s, scale);
		MatrixRotationX(&r1, rotation->x);
		MatrixRotationY(&r2, rotation->y);
		MatrixRotationZ(&r3, rotation->z);
		MatrixTranslation(&t, translation);
		*transform = s * r1 * r2 * r3 * t;
		MatrixScaling(&s, 1.0f / scale->x, 1.0f / scale->y, 1.0f / scale->z);
		MatrixRotationX(&r1, -rotation->x);
		MatrixRotationY(&r2, -rotation->y);
		MatrixRotationZ(&r3, -rotation->z);
		MatrixTranslation(&t, -translation->x, -translation->y, -translation->z);
		*invtransform = t * r3 * r2 * r1 * s;

		MatrixTranspose(transform_invtrans, invtransform);
		MatrixTranspose(invtransform_invtrans, transform);
	}
	else if(transform)
	{
		delete transform; transform = NULL;
		delete invtransform; invtransform = NULL;
		delete transform_invtrans; transform_invtrans = NULL;
		delete invtransform_invtrans; invtransform_invtrans = NULL;
	}
}
void Surface::SetTransform(const Vector3* translation, const Quaternion* rotation, const Vector3* scale)
{
	if(translation->x != 0.0f || translation->y != 0.0f || translation->z != 0.0f ||
	   rotation->x != 0.0f || rotation->y != 0.0f || rotation->z != 0.0f ||
	   scale->x != 1.0f || scale->y != 1.0f || scale->z != 1.0f)
	{
		if(!transform)
		{
			transform = new Matrix();
			invtransform = new Matrix();
			transform_invtrans = new Matrix();
			invtransform_invtrans = new Matrix();
		}

		Matrix s, r, t;
		Quaternion invrotation;
		MatrixScaling(&s, scale);
		MatrixRotationQuaternion(&r, rotation);
		MatrixTranslation(&t, translation);
		*transform = s * r * t;
		MatrixScaling(&s, 1.0f / scale->x, 1.0f / scale->y, 1.0f / scale->z);
		QuaternionInverse(&invrotation, rotation);
		MatrixRotationQuaternion(&r, &invrotation);
		MatrixTranslation(&t, -translation->x, -translation->y, -translation->z);
		*invtransform = t * r * s;

		MatrixTranspose(transform_invtrans, invtransform);
		MatrixTranspose(invtransform_invtrans, transform);
	}
	else if(transform)
	{
		delete transform; transform = NULL;
		delete invtransform; invtransform = NULL;
		delete transform_invtrans; transform_invtrans = NULL;
		delete invtransform_invtrans; invtransform_invtrans = NULL;
	}
}

ISRayTracer::IMaterial* Surface::GetMaterial(unsigned int idx)
{
	if(idx >= materials.size())
	{
		Result::PrintLogMessage("Material index out of bounds");
		return NULL;
	}

	return materials[idx];
}
ISRayTracer::IMaterial* Surface::SetMaterial(ISRayTracer::IMaterial* material, unsigned int idx)
{
	if(idx >= materials.size())
		materials.resize(idx + 1, NULL);
	materials[idx] = (::Material*)material;
	return material;
}


void SphereSurface::CastRay(RayCastParams& params) const
{
	float newhitdist = params.RaySphere(&params.transraypos, &params.transraydir, &pos, radius);

	if(transform && newhitdist != FLOAT_INFINITY)
	{
		// Transform newhitdist
		Vec3Scale(&params.hitnml, &params.transraydir, newhitdist);
		Vec3TransformNormal(&params.hitnml, &params.hitnml, transform);
		newhitdist = Vec3Length(&params.hitnml);
	}

	if(newhitdist < params.hitdist)
	{
		params.hitdist = newhitdist;
		params.hitsfc = this;
	}
}
void SphereSurface::BatchCastRay(BatchRayCastParams& batchparams) const
{
	Vector3 centerdir;
	Vec3Sub(&centerdir, &pos, &batchparams.params.transraypos);
	Vec3Normalize(&centerdir);

	float newhitdist;
	float dot, maxmiss = 0.0f;
	BatchRayCastParams::Hit* hit;
	for(int y = 0; y < batchparams.resolution.height; y++)
		for(int x = 0; x < batchparams.resolution.width; x++)
		{
			batchparams.GetRayDir(&batchparams.params.transraydir, x, y);
			if(transform)
				Vec3TransformNormal(&batchparams.params.transraydir, &batchparams.params.transraydir, invtransform);
			Vec3Normalize(&batchparams.params.transraydir);

			dot = Vec3Dot(&batchparams.params.transraydir, &centerdir);
			if(dot <= maxmiss)
				continue; // Early out

			newhitdist = batchparams.params.RaySphere(&batchparams.params.transraypos, &batchparams.params.transraydir, &pos, radius);
			if(newhitdist != FLOAT_INFINITY)
			{
				if(transform)
				{
					// Transform newhitdist
					Vec3Scale(&batchparams.params.hitnml, &batchparams.params.transraydir, newhitdist);
					Vec3TransformNormal(&batchparams.params.hitnml, &batchparams.params.hitnml, transform);
					newhitdist = Vec3Length(&batchparams.params.hitnml);
				}

				hit = &batchparams.hits[y * batchparams.resolution.width + x];
				if(newhitdist < hit->hitdist)
				{
					hit->hitdist = newhitdist;
					hit->hitsfc = this;
				}
			}
			else if(dot > maxmiss)
				maxmiss = dot;
		}
}
void SphereSurface::GetRayCastHitDetails(RayCastParams& params, TextureSampler* sampler) const
{
	// Compute hitmat
	params.hitmat = materials[0];

	// Compute hitpos
	if(params.flags & (RCP_GET_HIT_POS | RCP_GET_HIT_NML | RCP_GET_HIT_UV)) // RCP_GET_HIT_NML and RCP_GET_HIT_UV depend on RCP_GET_HIT_POS
	{
		// hitpos = raypos + raydir * hitdist
		Vec3Scale(&params.hitpos, &params.raydir, params.hitdist);
		Vec3Add(&params.hitpos, &params.raypos, &params.hitpos);
	}

	// Compute hituv
	if(params.flags & RCP_GET_HIT_UV || (params.flags & RCP_GET_HIT_NML && params.hitmat->normalmap)) // Bump mapping depends on RCP_GET_HIT_UV
	{
		if(transform)
		{
			// hitnml = normalize(hitpos - pos)
			Vec3TransformCoord(&params.hitnml, &params.hitpos, invtransform); // hitpos = transformed_hitpos * invtransform
			Vec3Sub(&params.hitnml, &params.hitnml, &pos);
			Vec3Normalize(&params.hitnml);
		}
		else
		{
			// hitnml = normalize(hitpos - pos)
			Vec3Sub(&params.hitnml, &params.hitpos, &pos);
			Vec3Normalize(&params.hitnml);
		}

		params.hituv.x = 0.5f - atan2(params.hitnml.x, -params.hitnml.z) / (2.0f * PI);
		params.hituv.y = 0.5f - asin(params.hitnml.y) / PI;
	}

	// Compute hitnml [and hittgt/hitbnm]
	if(params.flags & RCP_GET_HIT_NML)
	{
		if(params.flags & RCP_GET_HIT_UV) // If hitnml = normalize(hitpos - pos) has already been computed
		{
			if(params.hitmat->normalmap)
			{
				// hittgt, hitbnm = orthogonal to hitnml, with hitbnm pointing up
				if(params.hitnml.y < 1.0f - FLOAT_TOLERANCE && params.hitnml.y > -1.0f + FLOAT_TOLERANCE)
					Vec3Set(&params.hitbnm, 0.0f, 1.0f, 0.0f);
				else
					Vec3Set(&params.hitbnm, 0.0f, 0.0f, -1.0f);
				Vec3Cross(&params.hittgt, &params.hitbnm, &params.hitnml);
				Vec3Cross(&params.hitbnm, &params.hitnml, &params.hittgt);
			}

			if(transform)
			{
				// hitnml = normalize(hitnml * transform_invtrans)
				Vec3TransformNormal(&params.hitnml, &params.hitnml, transform_invtrans);
				Vec3Normalize(&params.hitnml);

				if(params.hitmat->normalmap)
				{
					Vec3TransformNormal(&params.hittgt, &params.hittgt, transform_invtrans);
					Vec3Normalize(&params.hittgt);
					Vec3TransformNormal(&params.hitbnm, &params.hitbnm, transform_invtrans);
					Vec3Normalize(&params.hitbnm);
				}
			}
		}
		else // If hitnml = normalize(hitpos - pos) hasn't been computed yet
		{
			if(transform)
			{
				// hitnml = normalize((hitpos - pos) * transform_invtrans)
				Vec3TransformCoord(&params.hitnml, &params.hitpos, invtransform); // hitpos = transformed_hitpos * invtransform
				Vec3Sub(&params.hitnml, &params.hitnml, &pos);
				Vec3TransformNormal(&params.hitnml, &params.hitnml, transform_invtrans);
				Vec3Normalize(&params.hitnml);
			}
			else
			{
				// hitnml = normalize(hitpos - pos)
				Vec3Sub(&params.hitnml, &params.hitpos, &pos);
				Vec3Normalize(&params.hitnml);
			}
		}
	}

	// Compute hitcurv
	if(params.flags & RCP_GET_CURVATURE)
		params.hitcurv = 1.0f / radius; //EDIT

	// Compute hitnml based on normal map
	if(params.flags & RCP_GET_HIT_NML && params.hitmat->normalmap)
	{
		float spl[4];
		sampler->Sample(spl, params.hitmat->normalmap, &params.hituv); // Sample normal from normalmap
		Vector3 splvec = Vector3(2.0f * spl[0] - 1.0f, 2.0f * spl[1] - 1.0f, 2.0f * spl[2] - 1.0f); // Transform normal components from [0 -> 1] to [-1 -> 1]
		Vec3Normalize(&splvec); // Assure normal stays normalized after sampling
		Vector3 hitnml(splvec.x * params.hittgt.x + splvec.y * params.hitbnm.x + splvec.z * params.hitnml.x,
					   splvec.x * params.hittgt.y + splvec.y * params.hitbnm.y + splvec.z * params.hitnml.y,
					   splvec.x * params.hittgt.z + splvec.y * params.hitbnm.z + splvec.z * params.hitnml.z); // Transform normal by TBN matrix
		Vec3Normalize(&params.hitnml, &hitnml);
	}
}

bool SphereSurface::GetPosition(Vector3* pos, float u, float v) const
{
	RaySpace::DirFromLattice(pos, v, u);
	Vec3Scale(pos, pos, radius + FLOAT_TOLERANCE);
	Vec3Add(pos, pos, &this->pos);
	if(transform)
		Vec3TransformCoord(pos, pos, transform);

	return true;
}

bool SphereSurface::GetNormal(Vector3* nml, float u, float v) const
{
	RaySpace::DirFromLattice(nml, v, u);
	if(transform)
		Vec3TransformNormal(nml, nml, transform);
	return true;
}
bool SphereSurface::GetPositionAndNormal(Vector3* pos, Vector3* nml, float u, float v) const
{
	RaySpace::DirFromLattice(nml, v, u);
	Vec3Scale(pos, nml, radius + FLOAT_TOLERANCE);
	Vec3Add(pos, pos, &this->pos);
	if(transform)
	{
		Vec3TransformCoord(pos, pos, transform);
		Vec3TransformNormal(nml, nml, transform);
	}
	return true;
}
float SphereSurface::GetSampleArea(float u, float v, float numsamples_u, float numsamples_v) const // [u, v] ... lower bounds of bin ... center of bin
{
	// Subtract spherical cap with h=v-0.5*delta_v (lower bound of bin) from spherical cap with h=v+0.5*delta_v (upper bound of bin)
	auto h = [&](float alpha) {return radius - cos(alpha * PI) * radius;};
	float h0 = h(v - 0.5f / numsamples_v); // delta_v = 1 / numsamples_v
	float h1 = h(v + 0.5f / numsamples_v); // delta_v = 1 / numsamples_v
	float A = 2.0f * PI * radius * (h1 - h0);

	A /= numsamples_u; // Divide by number of sphere stacks

	return A;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void SphereSurface::Release()
{
	parent->surfaces.remove(this);

	delete this;
}


void MeshSurface::CastRay(RayCastParams& params) const
{
	float newhitdist;
	Vector2 newuv;
#define USE_OCTREE
#ifdef USE_OCTREE
	unsigned int newhitidx;
	newhitdist = octree.RayCast(&params.transraypos, &params.transraydir, params.RayTriangle, &newuv, &newhitidx);

	if(transform && newhitdist != FLOAT_INFINITY)
	{
		// Transform newhitdist
		Vec3Scale(&params.hitnml, &params.transraydir, newhitdist);
		Vec3TransformNormal(&params.hitnml, &params.hitnml, transform);
		newhitdist = Vec3Length(&params.hitnml);
	}

	if(newhitdist < params.hitdist && newhitdist > FLOAT_TOLERANCE)
	{
		params.hitdist = newhitdist;
		params.hituv = newuv;
		params.hitsfc = this;
		params.hitidx = newhitidx;
	}
#else
	for(unsigned int i = 0; i < numfaces; i++)
	{
		newhitdist = params.RayTriangle(&params.transraypos, &params.transraydir, &positions[positionindices[3 * i + 0]], &positions[positionindices[3 * i + 1]], &positions[positionindices[3 * i + 2]], &newuv);

		if(transform && newhitdist != FLOAT_INFINITY)
		{
			// Transform newhitdist
			Vec3Scale(&params.hitnml, &params.transraydir, newhitdist);
			Vec3TransformNormal(&params.hitnml, &params.hitnml, transform);
			newhitdist = Vec3Length(&params.hitnml);
		}

		if(newhitdist < params.hitdist && newhitdist > FLOAT_TOLERANCE)
		{
			params.hitdist = newhitdist;
			params.hituv = newuv;
			params.hitsfc = this;
			params.hitidx = i;
		}
	}
#endif
}
void MeshSurface::BatchCastRay(BatchRayCastParams& batchparams) const
{
	//EDIT: >>> Placeholder >>>
	BatchRayCastParams::Hit* hit;
	float oldhitdist;
	for(int y = 0; y < batchparams.resolution.height; y++)
		for(int x = 0; x < batchparams.resolution.width; x++)
		{
			batchparams.GetRayDir(&batchparams.params.transraydir, x, y);
			if(transform)
				Vec3TransformNormal(&batchparams.params.transraydir, &batchparams.params.transraydir, invtransform);
			Vec3Normalize(&batchparams.params.transraydir);

			oldhitdist = batchparams.params.hitdist = batchparams.hits[y * batchparams.resolution.width + x].hitdist;
			CastRay(batchparams.params);
			if(batchparams.params.hitdist < oldhitdist)
			{
				hit = &batchparams.hits[y * batchparams.resolution.width + x];
				hit->hitdist = batchparams.params.hitdist;
				hit->hituv = batchparams.params.hituv;
				hit->hitsfc = batchparams.params.hitsfc;
				hit->hitidx = batchparams.params.hitidx;
			}
		}
	//EDIT: <<< Placeholder <<<

/*batchparams.currentsfc = this;
batchparams.currenttransform = transform;
batchparams.currentinvtransform = invtransform;
octree.BatchRayCast(batchparams);*/
}
void MeshSurface::GetRayCastHitDetails(RayCastParams& params, TextureSampler* sampler) const
{
	// Compute hitmat
	params.hitmat = materials[attributes.data ? attributes[params.hitidx] : 0];

	// Compute hitpos
	if(params.flags & RCP_GET_HIT_POS)
	{
		// hitpos = raypos + raydir * hitdist
		Vec3Scale(&params.hitpos, &params.raydir, params.hitdist);
		Vec3Add(&params.hitpos, &params.raypos, &params.hitpos);
	}

	// Compute hitnml [and hittgt/hitbnm]
	if(params.flags & RCP_GET_HIT_NML)
	{
		// hitnml = n0 * (1 - u - v) + n1 * u + n2 * v
		Vec3BaryCentric(&params.hitnml, &normals[normalindices[3 * params.hitidx + 0]], &normals[normalindices[3 * params.hitidx + 1]], &normals[normalindices[3 * params.hitidx + 2]], &params.hituv);
Vec3Normalize(&params.hitnml);
		if(transform)
		{
			Vec3TransformNormal(&params.hitnml, &params.hitnml, transform_invtrans);
			Vec3Normalize(&params.hitnml);
		}

		if(params.hitmat->normalmap)
		{
			Vec3BaryCentric(&params.hittgt, &tangents[normalindices[3 * params.hitidx + 0]], &tangents[normalindices[3 * params.hitidx + 1]],
							&tangents[normalindices[3 * params.hitidx + 2]], &params.hituv);
Vec3Normalize(&params.hittgt);
			Vec3BaryCentric(&params.hitbnm, &binormals[normalindices[3 * params.hitidx + 0]], &binormals[normalindices[3 * params.hitidx + 1]],
							&binormals[normalindices[3 * params.hitidx + 2]], &params.hituv);
Vec3Normalize(&params.hitbnm);
			if(transform)
			{
				Vec3TransformNormal(&params.hittgt, &params.hittgt, transform_invtrans);
				Vec3Normalize(&params.hittgt);
				Vec3TransformNormal(&params.hitbnm, &params.hitbnm, transform_invtrans);
				Vec3Normalize(&params.hitbnm);
			}
		}
	}

	// Compute hitcurv
	if(params.flags & RCP_GET_CURVATURE)
	{
		// Compute absolute mean curvature
		// Algorithm based on the paper "Normal Based Estimation of the Curvature Tensor for Triangular Meshes"
		// Authors: Holger Theisel, Christian Rossl, Rhaleb Zayer and Hans-Peter Seidel
		// ISSN: 1550-4085
		// Print ISBN: 0-7695-2234-3
		// Digital Object Identifier: 10.1109/PCCGA.2004.1348359
const Vector3* n0 = &normals[normalindices[3 * params.hitidx + 0]];
const Vector3* n1 = &normals[normalindices[3 * params.hitidx + 1]];
const Vector3* n2 = &normals[normalindices[3 * params.hitidx + 2]];

const Vector3* x0 = &positions[positionindices[3 * params.hitidx + 0]];
const Vector3* x1 = &positions[positionindices[3 * params.hitidx + 1]];
const Vector3* x2 = &positions[positionindices[3 * params.hitidx + 2]];

float u = params.hituv.x, v = params.hituv.y;
float w = 1.0f - u - v;

Vector3 tmp[3];

Vector3 n;
Vec3Scale(&tmp[0], n0, w);
Vec3Scale(&tmp[1], n1, u);
Vec3Scale(&tmp[2], n2, v);
Vec3Add(&n, &tmp[0], &tmp[1]);
Vec3Add(&n, &n, &tmp[2]);

Vector3 r0, r1, r2;
Vec3Sub(&r0, x2, x1);
Vec3Sub(&r1, x0, x2);
Vec3Sub(&r2, x1, x0);

Vector3 m;
Vec3Cross(&m, &r2, &r0);

Vector3 h;
Vec3Cross(&tmp[0], n0, &r0);
Vec3Cross(&tmp[1], n1, &r1);
Vec3Cross(&tmp[2], n2, &r2);
Vec3Add(&h, &tmp[0], &tmp[1]);
Vec3Add(&h, &h, &tmp[2]);

params.hitcurv = abs(Vec3Dot(&n, &h) / (2.0f * Vec3Length(&n) * Vec3Dot(&n, &m)));
//EDIT: Needs cleanup
	}

	// Compute hituv
	if(params.flags & RCP_GET_HIT_UV || (params.flags & RCP_GET_HIT_NML && params.hitmat->normalmap)) // Bump mapping depends on RCP_GET_HIT_UV
	{
		// hituv = tc0 * (1 - u - v) + tc1 * u + tc2 * v
		Vec2BaryCentric(&params.hituv, &texcoords[texcoordindices[3 * params.hitidx + 0]],
						&texcoords[texcoordindices[3 * params.hitidx + 1]], &texcoords[texcoordindices[3 * params.hitidx + 2]], &params.hituv);
	}

	// Compute hitnml based on normal map
	if(params.flags & RCP_GET_HIT_NML && params.hitmat->normalmap)
	{
		float spl[4];
		sampler->Sample(spl, params.hitmat->normalmap, &params.hituv); // Sample normal from normalmap
		Vector3 splvec = Vector3(2.0f * spl[0] - 1.0f, 2.0f * spl[1] - 1.0f, 2.0f * spl[2] - 1.0f); // Transform normal components from [0 -> 1] to [-1 -> 1]
		Vec3Normalize(&splvec); // Assure normal stays normalized after sampling
		Vector3 hitnml(splvec.x * params.hittgt.x + splvec.y * params.hitbnm.x + splvec.z * params.hitnml.x,
					   splvec.x * params.hittgt.y + splvec.y * params.hitbnm.y + splvec.z * params.hitnml.y,
					   splvec.x * params.hittgt.z + splvec.y * params.hitbnm.z + splvec.z * params.hitnml.z); // Transform normal by TBN matrix
		Vec3Normalize(&params.hitnml, &hitnml);
	}
}

bool MeshSurface::GetPosition(Vector3* pos, float u, float v) const
{
	Vector2 triangle_uv, sfc_uv(u, v);
	for(unsigned int f = 0; f < numfaces; f++)
	{
		Vec2InvBaryCentric(&triangle_uv, &texcoords[texcoordindices[3 * f + 0]], &texcoords[texcoordindices[3 * f + 1]], &texcoords[texcoordindices[3 * f + 2]], &sfc_uv);

		if(triangle_uv.x >= 0.0f && triangle_uv.y >= 0.0f && triangle_uv.x + triangle_uv.y <= 1.0f) //EDIT: Check "<="
		{
			Vec3BaryCentric(pos, &positions[positionindices[3 * f + 0]], &positions[positionindices[3 * f + 1]], &positions[positionindices[3 * f + 2]], &triangle_uv);
			if(transform)
				Vec3TransformCoord(pos, pos, transform);
			return true;
		}
	}
	return false;
}
bool MeshSurface::GetNormal(Vector3* nml, float u, float v) const
{
	Vector2 triangle_uv, sfc_uv(u, v);
	for(unsigned int f = 0; f < numfaces; f++)
	{
		Vec2InvBaryCentric(&triangle_uv, &texcoords[texcoordindices[3 * f + 0]], &texcoords[texcoordindices[3 * f + 1]], &texcoords[texcoordindices[3 * f + 2]], &sfc_uv);

		if(triangle_uv.x >= 0.0f && triangle_uv.y >= 0.0f && triangle_uv.x + triangle_uv.y <= 1.0f) //EDIT: Check "<="
		{
			Vec3BaryCentric(nml, &normals[normalindices[3 * f + 0]], &normals[normalindices[3 * f + 1]], &normals[normalindices[3 * f + 2]], &triangle_uv);
			if(transform)
				Vec3TransformNormal(nml, nml, transform);
			return true;
		}
	}
	return false;
}
bool MeshSurface::GetPositionAndNormal(Vector3* pos, Vector3* nml, float u, float v) const
{
	Vector2 triangle_uv, sfc_uv(u, v);
	for(unsigned int f = 0; f < numfaces; f++)
	{
		Vec2InvBaryCentric(&triangle_uv, &texcoords[texcoordindices[3 * f + 0]], &texcoords[texcoordindices[3 * f + 1]], &texcoords[texcoordindices[3 * f + 2]], &sfc_uv);

		if(triangle_uv.x >= 0.0f && triangle_uv.y >= 0.0f && triangle_uv.x + triangle_uv.y <= 1.0f) //EDIT: Check "<="
		{
			Vec3BaryCentric(pos, &positions[positionindices[3 * f + 0]], &positions[positionindices[3 * f + 1]], &positions[positionindices[3 * f + 2]], &triangle_uv);
			Vec3BaryCentric(nml, &normals[normalindices[3 * f + 0]], &normals[normalindices[3 * f + 1]], &normals[normalindices[3 * f + 2]], &triangle_uv);
			if(transform)
			{
				Vec3TransformCoord(pos, pos, transform);
				Vec3TransformNormal(nml, nml, transform);
			}
			return true;
		}
	}
	return false;
}

float MeshSurface::GetSampleArea(float u, float v, float numsamples_u, float numsamples_v) const
{
	// Fast version: area = parallelogram_area(pos[0], pos[1], pos[2])
	float halfdelta_u = 1.0f / (2.0f * numsamples_u);
	float halfdelta_v = 1.0f / (2.0f * numsamples_v);
	Vector3 pos[3];
	if(!GetPosition(&pos[0], MAX(0.0f, u - halfdelta_u), MAX(0.0f, v - halfdelta_v)))
		return 0.0f;
	if(!GetPosition(&pos[1], MIN(1.0f - FLOAT_TOLERANCE, u + halfdelta_u), MAX(0.0f, v - halfdelta_v)))
		return 0.0f;
	if(!GetPosition(&pos[2], MAX(0.0f, u - halfdelta_u), MIN(1.0f - FLOAT_TOLERANCE, v + halfdelta_v)))
		return 0.0f;
	
	Vec3Sub(&pos[1], &pos[1], &pos[0]); // pos[1] = pos[1] - pos[0]
	Vec3Sub(&pos[2], &pos[2], &pos[0]); // pos[2] = pos[2] - pos[0]
	Vec3Cross(&pos[0], &pos[1], &pos[2]); // pos[0] = (pos[1] - pos[0]) x (pos[2] - pos[0])
	return Vec3Length(&pos[0]);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void MeshSurface::Release()
{
	parent->surfaces.remove(this);

	delete this;
}


void VMeshSurface::CastRay(RayCastParams& params) const
{
	// Set raypos to level bounds
	/*static*/ Vector3 transraypos;
	RayCastCubeResult rccr;
	if(!IntersectRayAABB(&bounds_min.x, &bounds_max.x, &params.transraypos.x, &params.transraydir.x, &transraypos.x, (IntersectRayAABBResult*)&rccr))
		return;

	// Transform to local space (with origin at bounds_min)
	Vec3Sub(&transraypos, &transraypos, &bounds_min);

	int boxpos[] = {MIN(size[0] - 1, (int)transraypos.x), MIN(size[1] - 1, (int)transraypos.y), MIN(size[2] - 1, (int)transraypos.z)};
	int hitvoxelidx = -1;
	do
	{
		// Query box mask at boxpos
		int voxelidx = boxpos[0] * (size[1] * size[2]) + boxpos[2] * size[1] + boxpos[1];
		if(voxelmask[voxelidx]) // If a voxel was hit
		{
			if(params.IsFrontFaceCullingEnabled())
				hitvoxelidx = voxelidx; // Mark that the ray had already passed through a voxel for future hit tests
			else
			{
Vec3Sub(&params.transraypos, &params.transraypos, &bounds_min); // Transform to local space (with origin at bounds_min)
				Vector3 vdelta = transraypos - params.transraypos;
				float newhitdist = Vec3Length(&vdelta);
				if(transform)
				{
					// Transform newhitdist
					Vec3Scale(&params.hitnml, &params.transraydir, newhitdist);
					Vec3TransformNormal(&params.hitnml, &params.hitnml, transform);
					newhitdist = Vec3Length(&params.hitnml);
				}

				if(newhitdist > FLOAT_TOLERANCE)
				{
					if(newhitdist < params.hitdist)
					{
						params.hitdist = newhitdist;
						Vec3Add(&params.hitpos, &transraypos, &bounds_min);
						params.hitsfc = this;

						// Store ray cast result in hitidx
						params.hitidx = voxelidx & 0x1FFFFFFF | ((rccr & 0x7) << 29);
					}
					return;
				}
			}
		}
		else if(hitvoxelidx != -1) // If the ray had passed through voxels and is now exiting the voxel mesh
			break; // Stop tracing ray

		// Advance raypos and boxpos to next cube
		rccr = RayCastCube(&transraypos.x, &params.transraydir.x, boxpos);
		if(!params.IsFrontFaceCullingEnabled())
			rccr = (RayCastCubeResult)(rccr & 0xFFFFFFFE | !(rccr & 0x1)); // Flip ray cast result, since the hit normal is the inverse of the outgoing face normal of the empoty neighbour-cube
	} while(boxpos[0] >= 0 && boxpos[0] < size[0] && boxpos[1] >= 0 && boxpos[1] < size[1] && boxpos[2] >= 0 && boxpos[2] < size[2]);

	if(hitvoxelidx != -1) // If the ray had passed through voxel mesh (implies params.IsFrontFaceCullingEnabled())
	{
Vec3Sub(&params.transraypos, &params.transraypos, &bounds_min); // Transform to local space (with origin at bounds_min)
		Vector3 vdelta = transraypos - params.transraypos;
		float newhitdist = Vec3Length(&vdelta);
		if(transform)
		{
			// Transform newhitdist
			Vec3Scale(&params.hitnml, &params.transraydir, newhitdist);
			Vec3TransformNormal(&params.hitnml, &params.hitnml, transform);
			newhitdist = Vec3Length(&params.hitnml);
		}

		if(newhitdist < params.hitdist && newhitdist > FLOAT_TOLERANCE)
		{
			params.hitdist = newhitdist;
			Vec3Add(&params.hitpos, &transraypos, &bounds_min);
			params.hitsfc = this;

			// Store ray cast result in hitidx
			params.hitidx = hitvoxelidx & 0x1FFFFFFF | ((rccr & 0x7) << 29);
		}
		return;
	}
}
void VMeshSurface::BatchCastRay(BatchRayCastParams& batchparams) const
{
	//EDIT: >>> Placeholder >>>
	BatchRayCastParams::Hit* hit;
	float oldhitdist;
	for(int y = 0; y < batchparams.resolution.height; y++)
		for(int x = 0; x < batchparams.resolution.width; x++)
		{
			batchparams.GetRayDir(&batchparams.params.transraydir, x, y);
			if(transform)
				Vec3TransformNormal(&batchparams.params.transraydir, &batchparams.params.transraydir, invtransform);
			Vec3Normalize(&batchparams.params.transraydir);

			oldhitdist = batchparams.params.hitdist = batchparams.hits[y * batchparams.resolution.width + x].hitdist;
			CastRay(batchparams.params);
			if(batchparams.params.hitdist < oldhitdist)
			{
				hit = &batchparams.hits[y * batchparams.resolution.width + x];
				hit->hitdist = batchparams.params.hitdist;
				hit->hituv = batchparams.params.hituv;
				hit->hitsfc = batchparams.params.hitsfc;
				hit->hitidx = batchparams.params.hitidx;
			}
		}
	//EDIT: <<< Placeholder <<<
}
void VMeshSurface::GetRayCastHitDetails(RayCastParams& params, TextureSampler* sampler) const
{
	// Compute hitmat
	int hitvoxelidx = params.hitidx & 0x1FFFFFFF;
	params.hitmat = materials[attributes.data ? attributes[hitvoxelidx] : 0];

	/*// Transform hitpos
	if(transform && params.flags & RCP_GET_HIT_POS)
	{
		Vec3TransformCoord(&params.hitpos, &params.hitpos, transform);
	}*/

	// Transform hitpos
	if(params.flags & RCP_GET_HIT_POS)
	{
		// hitpos = raypos + raydir * hitdist
		Vec3Scale(&params.hitpos, &params.raydir, params.hitdist);
		Vec3Add(&params.hitpos, &params.raypos, &params.hitpos);
	}

	RayCastCubeResult rccr = (RayCastCubeResult)(params.hitidx >> 29); // Retreive ray cast result from params.hitnml

	// Compute hitnml [and hittgt/hitbnm]
	if(params.flags & RCP_GET_HIT_NML)
	{
		switch(rccr)
		{
		case RCCR_NORTH:  Vec3Set(&params.hitnml, 0.0f, 1.0f, 0.0f); break;
		case RCCR_EAST:   Vec3Set(&params.hitnml, 1.0f, 0.0f, 0.0f); break;
		case RCCR_SOUTH:  Vec3Set(&params.hitnml, 0.0f, -1.0f, 0.0f); break;
		case RCCR_WEST:   Vec3Set(&params.hitnml, -1.0f, 0.0f, 0.0f); break;
		case RCCR_TOP:    Vec3Set(&params.hitnml, 0.0f, 0.0f, 1.0f); break;
		case RCCR_BOTTOM: Vec3Set(&params.hitnml, 0.0f, 0.0f, -1.0f); break;
		default: Vec3Set(&params.hitnml, 1.0f, 1.0f, 1.0f);
		}

		if(params.hitmat->normalmap)
		{
			// hittgt, hitbnm = orthogonal to hitnml, with hitbnm pointing up
			if(rccr != RCCR_NORTH && rccr != RCCR_SOUTH)
				Vec3Set(&params.hitbnm, 0.0f, 1.0f, 0.0f);
			else
				Vec3Set(&params.hitbnm, 0.0f, 0.0f, -1.0f);
			Vec3Cross(&params.hittgt, &params.hitbnm, &params.hitnml);
			Vec3Cross(&params.hitbnm, &params.hitnml, &params.hittgt);
		}

		if(transform)
		{
			// hitnml = normalize(hitnml * transform_invtrans)
			Vec3TransformNormal(&params.hitnml, &params.hitnml, transform_invtrans);
			Vec3Normalize(&params.hitnml);

			if(params.hitmat->normalmap)
			{
				Vec3TransformNormal(&params.hittgt, &params.hittgt, transform_invtrans);
				Vec3Normalize(&params.hittgt);
				Vec3TransformNormal(&params.hitbnm, &params.hitbnm, transform_invtrans);
				Vec3Normalize(&params.hitbnm);
			}
		}
	}

	// Compute hituv
	if(params.flags & RCP_GET_HIT_UV || (params.flags & RCP_GET_HIT_NML && params.hitmat->normalmap)) // Bump mapping depends on RCP_GET_HIT_UV
	{
		int boxpos[3];
		boxpos[2] = hitvoxelidx / size[1];
		boxpos[1] = hitvoxelidx - boxpos[2] * size[1];
		boxpos[0] = boxpos[2] / size[2];
		boxpos[2] -= boxpos[0] * size[2];
//Vec2Set(&params.hituv, (float)boxpos[0] / (float)size[0], (float)boxpos[1] / (float)size[1]);
		Vector3 cubecenter((float)boxpos[0], (float)boxpos[1], (float)boxpos[2]);
		switch(rccr)
		{
		case RCCR_NORTH:  Vec2Set(&params.hituv, 0.5f + params.hitpos.x - cubecenter.x, 0.5f + params.hitpos.z - cubecenter.z); break;
		case RCCR_EAST:   Vec2Set(&params.hituv, 0.5f + params.hitpos.z - cubecenter.z, 0.5f + params.hitpos.y - cubecenter.y); break;
		case RCCR_SOUTH:  Vec2Set(&params.hituv, 0.5f + cubecenter.x - params.hitpos.x, 0.5f + params.hitpos.z - cubecenter.z); break;
		case RCCR_WEST:   Vec2Set(&params.hituv, 0.5f + cubecenter.z - params.hitpos.z, 0.5f + params.hitpos.y - cubecenter.y); break;
		case RCCR_TOP:    Vec2Set(&params.hituv, 0.5f + params.hitpos.x - cubecenter.x, 0.5f + params.hitpos.y - cubecenter.y); break;
		case RCCR_BOTTOM: Vec2Set(&params.hituv, 0.5f + cubecenter.x - params.hitpos.x, 0.5f + params.hitpos.y - cubecenter.y); break;
		default: Vec2Set(&params.hituv, 0.0f, 0.0f);
		}
		params.hituv.x = saturate(params.hituv.x);
		params.hituv.y = saturate(params.hituv.y);
	}
	
	// Compute hitcurv
	if(params.flags & RCP_GET_CURVATURE)
		params.hitcurv = 0.0f;

	// Compute hitnml based on normal map
	if(params.flags & RCP_GET_HIT_NML && params.hitmat->normalmap)
	{
		float spl[4];
		sampler->Sample(spl, params.hitmat->normalmap, &params.hituv); // Sample normal from normalmap
		Vector3 splvec = Vector3(2.0f * spl[0] - 1.0f, 2.0f * spl[1] - 1.0f, 2.0f * spl[2] - 1.0f); // Transform normal components from [0 -> 1] to [-1 -> 1]
		Vec3Normalize(&splvec); // Assure normal stays normalized after sampling
		Vector3 hitnml(splvec.x * params.hittgt.x + splvec.y * params.hitbnm.x + splvec.z * params.hitnml.x,
					   splvec.x * params.hittgt.y + splvec.y * params.hitbnm.y + splvec.z * params.hitnml.y,
					   splvec.x * params.hittgt.z + splvec.y * params.hitbnm.z + splvec.z * params.hitnml.z); // Transform normal by TBN matrix
		Vec3Normalize(&params.hitnml, &hitnml);
	}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void VMeshSurface::Release()
{
	parent->surfaces.remove(this);

	delete this;
}