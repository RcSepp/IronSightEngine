#include "RayTracer.h"
#include "RaySpace.h"
#include <fstream>

//-----------------------------------------------------------------------------
// Name: CreateRaySpace()
// Desc: Create a 2D or 4D lattice covering all surfaces of the scene
//-----------------------------------------------------------------------------
Result Scene::CreateRaySpace(bool multireslattice, bool is2d, size_t numsamples_u, size_t numsamples_v, size_t numsamples_theta, size_t numsamples_phi, ISRayTracer::IRaySpace** rayspace)
{
#define NUMSAMPLES_THETA 4
#define NUMSAMPLES_PHI 4 * 2
	Result rlt;

	*rayspace = NULL;

	RaySpace* newrayspace;
	CHKALLOC(newrayspace = new RaySpace(this, multireslattice ? (RaySpace::Lattice*)new RaySpace::MultiResolutionLattice(is2d) : (RaySpace::Lattice*)new RaySpace::SingleResolutionLattice(is2d), is2d));

	std::list<Surface*>::iterator iter;
	Surface* sfc;
	int i;
	LIST_FOREACH_I(surfaces, iter, i)
	{
		sfc = (*iter);
		//newrayspace->AddSurface(sfc, numsamples_u * (i == 0 ? 4 : 1), numsamples_v * 1, numsamples_theta, numsamples_phi); // vdatest.obj
		//newrayspace->AddSurface(sfc, numsamples_u * (i == 1 ? 14 : 1), numsamples_v * 1, numsamples_theta, numsamples_phi); // cornell_box.obj
		/*if(i == 4) // cornell_box_tr.obj
			newrayspace->AddSurface(sfc, numsamples_u * 2, numsamples_v * 1, 2 * numsamples_theta, 2 * numsamples_phi); // cornell_box_tr.obj
		else // cornell_box_tr.obj
			newrayspace->AddSurface(sfc, numsamples_u * (i == 1 ? 3 : 1), numsamples_v * 1, numsamples_theta, numsamples_phi); // cornell_box_tr.obj*/

		newrayspace->AddSurface(sfc, numsamples_u, numsamples_v, numsamples_theta, numsamples_phi);
	}
	CHKRESULT(newrayspace->Allocate());

	rayspaces.push_back(newrayspace);
	*rayspace = newrayspace;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateRaySpace()
// Desc: Create a 2D or 4D lattice from file
//-----------------------------------------------------------------------------
Result Scene::CreateRaySpace(const FilePath& filename, ISRayTracer::IRaySpace** rayspace)
{
	Result rlt;

	*rayspace = NULL;

	std::ifstream fs(filename, std::ios::binary);
	if(!fs.good())
	{
		fs.close();
		return ERR("File not found or not readable");
	}
	RaySpace* newrayspace;
	IFFAILED(RaySpace::FromFile(fs, this, surfaces, &newrayspace))
	{
		fs.close();
		return rlt;
	}
	fs.close();

	rayspaces.push_back(newrayspace);
	*rayspace = newrayspace;

	return R_OK;
}

bool Scene::CastRay(RayCastParams& params, TextureSampler* sampler) const
{
	// Perform ray cast on every surface of the scene
	params.hitdist = FLOAT_INFINITY;
	std::list<Surface*>::const_iterator iter;
	Surface* sfc;
	LIST_FOREACH(surfaces, iter)
	{
		sfc = (Surface*)(*iter);
		if(sfc->transform)
		{
			Vec3TransformCoord(&params.transraypos, &params.raypos, sfc->invtransform);
			Vec3TransformNormal(&params.transraydir, &params.raydir, sfc->invtransform);
			Vec3Normalize(&params.transraydir); //EDIT: May not be necessary
		}
		else
		{
			params.transraypos = params.raypos;
			params.transraydir = params.raydir;
		}
		sfc->CastRay(params);
	}

	if(params.hitdist < FLOAT_INFINITY) // If a collision occured ...
	{
		if(params.flags) // If any hit details are required ...
		{
params.hituv.x = saturate(params.hituv.x);
params.hituv.y = saturate(params.hituv.y);
			// Compute hit details
			params.hitsfc->GetRayCastHitDetails(params, sampler);

			// Invert normal if front faces are rendered
			if(params.IsFrontFaceCullingEnabled())
				Vec3Invert(&params.hitnml);
		}
		return true;
	}
	else
		return false;
}
void Scene::BatchCastRay(BatchRayCastParams& batchparams, TextureSampler* sampler, float frameblendfactor, ONRAYHIT_CALLBACK hitcbk, ONRAYHIT_CALLBACK misscbk) const
{
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Scene::Release()
{
	parent->scenes.remove(this);

	while(rayspaces.size())
		rayspaces.back()->Release();

	delete this;
}