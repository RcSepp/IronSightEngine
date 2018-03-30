#include "RaySpace.h"
#include "RayTracer.h"
#include <ISImage.h>
#include <fstream>


float GetRandomOffset(float delta) {return ((float)rand() / (float)RAND_MAX - 0.5f) * delta;};
void RaySpace::Lattice::AddSurface(Surface* sfc, size_t numsamples_u, size_t numsamples_v, size_t numsamples_theta, size_t numsamples_phi)
{
	latticepartitionmap[sfc] = latticepartitions.size();
	latticepartitions.push_back(Partition());
	Partition& sfcpartition = latticepartitions.back();
	sfcpartition.sfc = sfc;
	sfcpartition.numsamples_u = numsamples_u;
	sfcpartition.numsamples_v = numsamples_v;
	sfcpartition.numsamples_theta = numsamples_theta;
	sfcpartition.numsamples_phi = numsamples_phi;
	sfcpartition.start = numentries;
	sfcpartition.stride = numsamples_theta * numsamples_phi * numsamples_u;
	sfcpartition.length = sfcpartition.stride * numsamples_v;
	numentries += sfcpartition.length;
}
Result RaySpace::Lattice::Allocate(const Scene* scene)
{
	rand_distribution = boost::random::uniform_int_distribution<>(0, numentries - 1);
	//rand_distribution = boost::random::uniform_int_distribution<>(0, latticepartitions.begin()->length - 1);

//size_t holy_number = entries.max_size();
	entries.resize(numentries);

	// Initialize with ambient color
	if(scene->ambientclr.r || scene->ambientclr.g || scene->ambientclr.b)
	{
		std::vector<Partition>::const_iterator iter;
		LIST_FOREACH(latticepartitions, iter)
		{
			const Partition& partition = *iter;
			//for(size_t i = partition.start; i < partition.start + partition.length; i++)
			for(size_t _u = 0; _u < partition.numsamples_u; _u++)
				for(size_t _v = 0; _v < partition.numsamples_v; _v++)
				{
					float u = (float)(_u + 0.5f) / (float)partition.numsamples_u; // +0.5... Use the center of the bin
					float v = (float)(_v + 0.5f) / (float)partition.numsamples_v; // +0.5... Use the center of the bin
					float samplearea = 1.0f / 2.0f * PI;//partition.sfc->GetSampleArea(u, v, (float)partition.numsamples_u, (float)partition.numsamples_v); //EDIT: Arthefacts appear when uncommenting this
					for(size_t _theta = 0; _theta < partition.numsamples_theta; _theta++)
						for(size_t _phi = 0; _phi < partition.numsamples_phi; _phi++)
						{
							// Angular area:
							// When entries[idx].luminance represents PHOTONS, larger angular bins would receive more photons (multiply with GetAngularArea(theta, phi)).
							// Since the VDA algorithm uses ILLUMINANCE (= photons per area), all bins receive the same illuminance.
							// During integration (inside RaySpace::Sample()) larger angular bins are weighted more than smaller ones (multiplied with GetAngularArea(theta, phi), yielding the same results as with photon-based luminance.
							// During random ray casting, statisticly more rays will hit larger angular bins, which asymptotically doesn't produce an even distribution as with ambient light, but the running average should compensate that.

							/*float phi =  (_phi + 0.5f) / (float)partition.numsamples_phi; // +0.5... Use the center of the bin
							float theta =  (_theta + 0.5f) / (float)partition.numsamples_theta; // +0.5... Use the center of the bin*/
							float angulararea = 1.0f;//GetAngularArea(theta, phi) * 10.0f; //EDIT: hardcoded factor

							size_t idx = partition.start + _v * partition.stride + _u * (partition.numsamples_theta * partition.numsamples_phi) + _phi * partition.numsamples_theta + _theta;

							ClrSet(&entries[idx].luminance, 0.0f, 0.0f, 0.0f);
							ClrAddScaled(&entries[idx].luminance, &entries[idx].luminance, (Color3*)&scene->ambientclr, (Color3*)&partition.sfc->materials[0]->ambientclr, samplearea * angulararea);
entries[idx].luminance.r /= partition.sfc->materials[0]->diffuseclr.r;
entries[idx].luminance.g /= partition.sfc->materials[0]->diffuseclr.g;
entries[idx].luminance.b /= partition.sfc->materials[0]->diffuseclr.b;
							entries[idx].raycount = 1;
						}
				}
		}
	}
	else // No ambient light
		for(size_t i = 0; i < numentries; i++)
		{
			ClrSet(&entries[i].luminance, 0.0f, 0.0f, 0.0f);
			entries[i].raycount = 0;
		}
	return R_OK;
}

RaySpace::Lattice::ThetaPhiIterator* RaySpace::SingleResolutionLattice::GetThetaPhiIterator(const Surface* sfc, const Vector2* uv)
{
	const Partition& partition = latticepartitions[latticepartitionmap.find(sfc)->second];

	size_t _u = (size_t)(uv->x * (float)partition.numsamples_u);
	size_t _v = (size_t)(uv->y * (float)partition.numsamples_v);
_u = min(_u, partition.numsamples_u - 1); // This is only neccessary when hituv.x can get 1.0
_v = min(_v, partition.numsamples_v - 1); // This is only neccessary when hituv.y can get 1.0
	size_t lattice_uv = partition.start + _v * partition.stride + _u * (partition.numsamples_theta * partition.numsamples_phi);

	RaySpace::Lattice::ThetaPhiIterator* iter = new ThetaPhiIterator(partition, lattice_uv);
	iter->phi =  0.5f / (float)partition.numsamples_phi; // +0.5... Use the center of the bin
	iter->theta =  0.5f / (float)partition.numsamples_theta; // +0.5... Use the center of the bin
	iter->delta_theta = 1.0f / (float)partition.numsamples_theta;
	iter->delta_phi = 1.0f / (float)partition.numsamples_phi;

#ifdef NEED_PHYSICAL_BIN_CENTER_AND_SIZE_INSIDE_THETA_PHI_ITERATOR
// Find physical bin center and size
iter->delta_u = 1.0f / (float)partition.numsamples_u;
iter->delta_v = 1.0f / (float)partition.numsamples_v;
iter->u = (float)(_u + 0.5f) * iter->delta_u; // +0.5... Use the center of the bin
iter->v = (float)(_v + 0.5f) * iter->delta_v; // +0.5... Use the center of the bin
#endif

	return iter;
}
bool RaySpace::SingleResolutionLattice::IterateThetaPhi(Lattice::ThetaPhiIterator* iter)
{
	ThetaPhiIterator& _iter = *(ThetaPhiIterator*)iter;
	if(++_iter._theta == _iter.partition.numsamples_theta)
	{
		if(++_iter._phi == _iter.partition.numsamples_phi)
			return false;
		_iter.phi =  (_iter._phi + 0.5f) / (float)_iter.partition.numsamples_phi; // +0.5... Use the center of the bin
		_iter._theta = 0;
	}
	_iter.theta =  (_iter._theta + 0.5f) / (float)_iter.partition.numsamples_theta; // +0.5... Use the center of the bin

	_iter.entry = &entries[_iter.lattice_uv + _iter._phi * _iter.partition.numsamples_theta + _iter._theta];
	return true;
}
RaySpace::Entry* RaySpace::SingleResolutionLattice::GetEntry(const Surface* sfc, float u, float v, float theta, float phi, float* pdelta_theta, float* pdelta_phi, char* subentry, int* psplitdepth, float* samplearea, BinDesc* bin)
{
	const Partition& partition = latticepartitions[latticepartitionmap.find(sfc)->second];

	size_t _u = (size_t)(u * (float)partition.numsamples_u);
	size_t _v = (size_t)(v * (float)partition.numsamples_v);
	size_t _theta = (size_t)(theta * (float)partition.numsamples_theta); _theta = min(_theta, partition.numsamples_theta - 1);
	size_t _phi = (size_t)(phi * (float)partition.numsamples_phi); _phi %= partition.numsamples_phi;
	size_t idx = partition.start + _v * partition.stride + _u * (partition.numsamples_theta * partition.numsamples_phi) + _phi * partition.numsamples_theta + _theta;

	if(samplearea)
	{
		*samplearea = GetAngularArea((_theta + 0.5f) / (float)partition.numsamples_theta, (_phi + 0.5f) / (float)partition.numsamples_phi, partition.numsamples_theta, partition.numsamples_phi, is2d); // [theta, phi] must be bin-center!
		//*samplearea += sfc->GetSampleArea((_u + 0.5f) / (float)partition.numsamples_u, (_v + 0.5f) / (float)partition.numsamples_v, (float)partition.numsamples_u, (float)partition.numsamples_v); // [u, v] must be bin-center!
	}

	if(bin)
	{
		bin->sfc = sfc;
		bin->_u = _u;
		bin->_v = _v;
		bin->_theta = _theta;
		bin->_phi = _phi;
//if(_u == 90 && _v == 0 && _theta == 8 && _phi == 5)
//	int abc = 0;
	}

	return &entries[idx];
}
RaySpace::Entry* RaySpace::SingleResolutionLattice::GetRandomEntry(Surface** sfc, float& u, float& v, float& theta, float& phi, float* pdelta_theta, float* pdelta_phi, char* subentry, int* psplitdepth, float* samplearea)
{
	*sfc = NULL;

	const size_t idx = rand_distribution(rand_generator); //size_t idx = (rand() * rand()) % latticelength;

	// Get partition of idx
	const Partition* partition;
	if(random_entry_constraints.sfc == RANDOM_SURFACE)
	{
		std::vector<Partition>::const_iterator iter;
		partition = NULL;
		LIST_FOREACH(latticepartitions, iter)
			if(idx >= iter->start && idx < iter->start + iter->length)
			{
				partition = &*iter;
				break;
			}
		if(!partition)
			return NULL;
	}
	else
		partition = &latticepartitions[latticepartitionmap.find(random_entry_constraints.sfc)->second];

	// Get theta/phi of idx
	const float delta_theta = 1.0f / (float)partition->numsamples_theta;
	const float delta_phi = 1.0f / (float)partition->numsamples_phi;
	if(pdelta_theta)
		*pdelta_theta = delta_theta;
	if(pdelta_phi)
		*pdelta_phi = delta_phi;

	size_t _theta = random_entry_constraints._theta == RANDOM_INDEX ? idx % partition->numsamples_theta : random_entry_constraints._theta;
	size_t _phi = random_entry_constraints._phi == RANDOM_INDEX ? (idx / partition->numsamples_theta) % partition->numsamples_phi : random_entry_constraints._phi;
	phi =  (_phi + 0.5f) * delta_phi; // +0.5... Use the center of the bin
	theta =  (_theta + 0.5f) * delta_theta; // +0.5... Use the center of the bin

	// Get u/v of idx
	const float delta_u = 1.0f / (float)partition->numsamples_u;
	const float delta_v = 1.0f / (float)partition->numsamples_v;

	size_t _u = random_entry_constraints._u == RANDOM_INDEX ? (idx / (partition->numsamples_theta * partition->numsamples_phi)) % partition->numsamples_u : random_entry_constraints._u;
	size_t _v = random_entry_constraints._v == RANDOM_INDEX ? (idx / partition->stride) % partition->numsamples_v : random_entry_constraints._v;
	u = (float)(_u + 0.5f) * delta_u; // +0.5... Cast from the center of the bin
	v = (float)(_v + 0.5f) * delta_v; // +0.5... Cast from the center of the bin

	// Get sample area
	if(samplearea)
	{
		*samplearea = GetAngularArea(theta, phi, partition->numsamples_theta, partition->numsamples_phi, is2d);
		//*samplearea += partition->sfc->GetSampleArea(u, v, (float)partition->numsamples_u, (float)partition->numsamples_v);
	}

	// >>> Randomize inside bin

	float rnd;

	// Offset [u, v] from bin center
	if(random_entry_constraints.offset_u == RANDOM_OFFSET)
		rnd = GetRandomOffset(delta_u);
	else
		rnd = random_entry_constraints.offset_u * delta_u;
	u += rnd;
	if(random_entry_constraints.offset_v == RANDOM_OFFSET)
		rnd = GetRandomOffset(delta_v);
	else
		rnd = random_entry_constraints.offset_v * delta_v;
	v += rnd;

	// Offset [theta, phi] from bin center
	if(random_entry_constraints.offset_theta == RANDOM_OFFSET)
		rnd = GetRandomOffset(delta_theta);
	else
		rnd = random_entry_constraints.offset_theta * delta_theta;
	theta += rnd;
	if(random_entry_constraints.offset_phi == RANDOM_OFFSET)
		rnd = GetRandomOffset(delta_phi);
	else
		rnd = random_entry_constraints.offset_phi * delta_phi;
	phi += rnd;

	*sfc = partition->sfc;
	return &entries[partition->start + _v * partition->stride + _u * (partition->numsamples_theta * partition->numsamples_phi) + _phi * partition->numsamples_theta + _theta]; // Don't use idx here, because it might be incorrect after applying constraints
}

RaySpace::Lattice::ThetaPhiIterator* RaySpace::MultiResolutionLattice::GetThetaPhiIterator(const Surface* sfc, const Vector2* uv)
{
	const Partition& partition = latticepartitions[latticepartitionmap.find(sfc)->second];

	float _ufract = uv->x * (float)partition.numsamples_u, _vfract = uv->y * (float)partition.numsamples_v;
	size_t _u = (size_t)_ufract, _v = (size_t)_vfract;
	_ufract -= (float)_u; _vfract -= (float)_v; // u = fract(u), v = fract(v)
_u = min(_u, partition.numsamples_u - 1); // This is only neccessary when hituv.x can get 1.0
_v = min(_v, partition.numsamples_v - 1); // This is only neccessary when hituv.y can get 1.0
	size_t lattice_uv = partition.start + _v * partition.stride + _u * (partition.numsamples_theta * partition.numsamples_phi);

	ThetaPhiIterator* iter = new ThetaPhiIterator(_ufract, _vfract, partition, lattice_uv);
	iter->phi = 0.5f / (float)partition.numsamples_phi; // +0.5... Use the center of the bin
	iter->theta = 0.5f / (float)partition.numsamples_theta; // +0.5... Use the center of the bin
	iter->delta_theta = 1.0f / (float)partition.numsamples_theta;
	iter->delta_phi = 1.0f / (float)partition.numsamples_phi;

#ifdef NEED_PHYSICAL_BIN_CENTER_AND_SIZE_INSIDE_THETA_PHI_ITERATOR
// Find physical bin center and size
iter->delta_u = 1.0f / (float)partition.numsamples_u;
iter->delta_v = 1.0f / (float)partition.numsamples_v;
iter->u = (float)(_u + 0.5f) * iter->delta_u; // +0.5... Use the center of the bin
iter->v = (float)(_v + 0.5f) * iter->delta_v; // +0.5... Use the center of the bin
size_t idx = lattice_uv + iter->_phi * partition.numsamples_theta + iter->_theta;
Entry* entry = &entries[idx];
int _splitdepth = 0;
iter->delta_u /= 2.0f;
iter->delta_v /= 2.0f;
while(IsSplitEntry(entry))
{
	++_splitdepth;
	iter->delta_u /= 2.0f;
	iter->delta_v /= 2.0f;
	idx = ((SplitEntry*)entry)->subentryidx;

	if(idx & 0x1)
		iter->u += iter->delta_u;
	else
		iter->u -= iter->delta_u;
	if(idx & 0x2)
		iter->v += iter->delta_v;
	else
		iter->v -= iter->delta_v;
	idx += ((SplitEntry*)entry)->subentryidx;
	entry = &entries[idx];
}
iter->delta_u *= 2.0f;
iter->delta_v *= 2.0f;
#endif

	return iter;
}
void RaySpace::MultiResolutionLattice::RecursiveSplitPathExplorer(ThetaPhiIterator& _iter, int idx, float _ufract, float _vfract)
{
	_iter.entry = &entries[idx];
float oldtheta = _iter.theta;
float oldphi = _iter.phi;
float olddelta_theta = _iter.delta_theta;
float olddelta_phi = _iter.delta_phi;
int oldnumsplits = _iter.numsplits;

	while(IsSplitEntry(_iter.entry))
	{
		++_iter.numsplits;
		idx = ((SplitEntry*)_iter.entry)->subentryidx;

		size_t shift = 0x1;
		if(IsSplit_U(_iter.entry))
		{
			_ufract *= 2.0f;
			if(_ufract >= 1.0f)
			{
				idx += shift;
				_ufract -= 1.0f;
			}
			shift <<= 1;
		}
		if(IsSplit_V(_iter.entry))
		{
			_vfract *= 2.0f;
			if(_vfract >= 1.0f)
			{
				idx += shift;
				_vfract -= 1.0f;
			}
			shift <<= 1;
		}

		if(IsSplit_Theta(_iter.entry))
		{
			_iter.delta_theta /= 2.0f;
			if(IsSplit_Phi(_iter.entry))
			{
				_iter.delta_phi /= 2.0f;

				_iter.theta -= _iter.delta_theta / 2.0f;
				_iter.phi -= _iter.delta_phi / 2.0f;
				RecursiveSplitPathExplorer(_iter, idx, _ufract, _vfract);

				_iter.phi += _iter.delta_phi;
				idx += shift << 1;
				RecursiveSplitPathExplorer(_iter, idx, _ufract, _vfract);

				_iter.phi -= _iter.delta_phi;
				_iter.theta += _iter.delta_theta;
				idx -= shift << 1;
				idx += shift;
				RecursiveSplitPathExplorer(_iter, idx, _ufract, _vfract);

				_iter.phi += _iter.delta_phi;
				idx += shift << 1;
				RecursiveSplitPathExplorer(_iter, idx, _ufract, _vfract);

_iter.theta = oldtheta;
_iter.phi = oldphi;
_iter.delta_theta = olddelta_theta;
_iter.delta_phi = olddelta_phi;
_iter.numsplits = oldnumsplits;
				return;
			}
			else
			{
				_iter.theta -= _iter.delta_theta / 2.0f;
				RecursiveSplitPathExplorer(_iter, idx, _ufract, _vfract);

				_iter.theta += _iter.delta_theta;
				idx += shift;
				RecursiveSplitPathExplorer(_iter, idx, _ufract, _vfract);

_iter.theta = oldtheta;
_iter.phi = oldphi;
_iter.delta_theta = olddelta_theta;
_iter.delta_phi = olddelta_phi;
_iter.numsplits = oldnumsplits;
				return;
			}
		}
		else if(IsSplit_Phi(_iter.entry))
		{
			_iter.delta_phi /= 2.0f;

			_iter.phi -= _iter.delta_phi / 2.0f;
			RecursiveSplitPathExplorer(_iter, idx, _ufract, _vfract);

			_iter.phi += _iter.delta_phi;
			idx += shift;
			RecursiveSplitPathExplorer(_iter, idx, _ufract, _vfract);

_iter.theta = oldtheta;
_iter.phi = oldphi;
_iter.delta_theta = olddelta_theta;
_iter.delta_phi = olddelta_phi;
_iter.numsplits = oldnumsplits;
			return;
		}

		_iter.entry = &entries[idx];
	}

	// At this point the lattice has been traveled down to a leaf entry
	_iter.alternativepaths.push(ThetaPhiIterator::AlternativePath(&_iter)); // Store the path to this entry
}
bool RaySpace::MultiResolutionLattice::IterateThetaPhi(Lattice::ThetaPhiIterator* iter)
{
	ThetaPhiIterator& _iter = *(ThetaPhiIterator*)iter;

	if(_iter.alternativepaths.size()) // If alternative paths haven't been traveled yet
	{
		// Travel an alternative path
		_iter.alternativepaths.top().Apply(iter);
		_iter.alternativepaths.pop();
		return true;
	}

	_iter.delta_theta = 1.0f / (float)_iter.partition.numsamples_theta;
	_iter.delta_phi = 1.0f / (float)_iter.partition.numsamples_phi;

	if(++_iter._theta == _iter.partition.numsamples_theta)
	{
		if(++_iter._phi == _iter.partition.numsamples_phi)
		{
assert(_iter.alternativepaths.empty());
			return false;
		}
		_iter.phi = (_iter._phi + 0.5f) * _iter.delta_phi; // +0.5... Use the center of the bin
		_iter._theta = 0;
	}
	_iter.theta = (_iter._theta + 0.5f) * _iter.delta_theta; // +0.5... Use the center of the bin

	_iter.numsplits = 0;
	RecursiveSplitPathExplorer(_iter, _iter.lattice_uv + _iter._phi * _iter.partition.numsamples_theta + _iter._theta, _iter._ufract, _iter._vfract);

	if(_iter.alternativepaths.size()) // If alternative paths have been found inside RecursiveSplitPathExplorer()
	{
		// Travel first alternative path
		_iter.alternativepaths.top().Apply(iter);
		_iter.alternativepaths.pop();
		return true;
	}

	return true;
}
RaySpace::Entry* RaySpace::MultiResolutionLattice::GetEntry(const Surface* sfc, float u, float v, float theta, float phi, float* pdelta_theta, float* pdelta_phi, char* subentry, int* psplitdepth, float* samplearea, BinDesc* bin)
{
	const Partition& partition = latticepartitions[latticepartitionmap.find(sfc)->second];

	float _ufract = u * (float)partition.numsamples_u, _vfract = v * (float)partition.numsamples_v;
	size_t _u = (size_t)_ufract, _v = (size_t)_vfract;
	_ufract -= (float)_u; _vfract -= (float)_v; // _ufract = fract(u), _vfract = fract(v)

	float _thetafract = theta * (float)partition.numsamples_theta, _phifract = phi * (float)partition.numsamples_phi;
	size_t _theta = (size_t)_thetafract, _phi = (size_t)_phifract;
	_thetafract -= (float)_theta; _phifract -= (float)_phi; // _thetafract = fract(theta), _phifract = fract(phi)
_theta = min(_theta, partition.numsamples_theta - 1);
_phi %= partition.numsamples_phi;

	size_t idx = partition.start + _v * partition.stride + _u * (partition.numsamples_theta * partition.numsamples_phi) + _phi * partition.numsamples_theta + _theta;

	Entry* entry = &entries[idx];
	int splitdepth = 0;
	float delta_theta = 1.0f / (float)partition.numsamples_theta;
	float delta_phi = 1.0f / (float)partition.numsamples_phi;
	while(IsSplitEntry(entry))
	{
		++splitdepth;
		idx = ((SplitEntry*)entry)->subentryidx;

		size_t shift = 0x1;
		if(IsSplit_U(entry))
		{
			_ufract *= 2.0f;
			if(_ufract >= 1.0f)
			{
				idx += shift;
				_ufract -= 1.0f;
			}
			shift <<= 1;
		}
		if(IsSplit_V(entry))
		{
			_vfract *= 2.0f;
			if(_vfract >= 1.0f)
			{
				idx += shift;
				_vfract -= 1.0f;
			}
			shift <<= 1;
		}
		if(IsSplit_Theta(entry))
		{
			delta_theta /= 2.0f;
			_thetafract *= 2.0f;
			if(_thetafract >= 1.0f)
			{
				idx += shift;
				_thetafract -= 1.0f;
			}
			shift <<= 1;
		}
		if(IsSplit_Phi(entry))
		{
			delta_phi /= 2.0f;
			_phifract *= 2.0f;
			if(_phifract >= 1.0f)
			{
				idx += shift;
				_phifract -= 1.0f;
			}
			//shift <<= 1; // Not needed, unless more dimensions are added
		}

		entry = &entries[idx];
	}

	if(pdelta_theta)
		*pdelta_theta = delta_theta;
	if(pdelta_phi)
		*pdelta_phi = delta_phi;
	if(psplitdepth)
		*psplitdepth = splitdepth;
	if(bin)
	{
		bin->sfc = sfc;
		bin->_u = _u;
		bin->_v = _v;
		bin->_theta = _theta;
		bin->_phi = _phi;
	}

	if(subentry)
	{
		*subentry = 0x0;

		_ufract *= 2.0f;
		if(_ufract >= 1.0f)
			*subentry |= 0x1;

		_vfract *= 2.0f;
		if(_vfract >= 1.0f)
			*subentry |= 0x2;

		_thetafract *= 2.0f;
		if(_thetafract >= 1.0f)
			*subentry |= 0x4;

		_phifract *= 2.0f;
		if(_phifract >= 1.0f)
			*subentry |= 0x8;
	}

	if(samplearea)
	{
		theta = GetBinCenter(theta, delta_theta);
		phi = GetBinCenter(phi, delta_phi);
		*samplearea = GetAngularArea(theta, phi, delta_theta, delta_phi, is2d); // [theta, phi] must be bin-center!
	}

	return &entries[idx];
}
RaySpace::Entry* RaySpace::MultiResolutionLattice::GetRandomEntry(Surface** sfc, float& u, float& v, float& theta, float& phi, float* pdelta_theta, float* pdelta_phi, char* subentry, int* psplitdepth, float* samplearea)
{
	*sfc = NULL;

	size_t idx = rand_distribution(rand_generator);

	// Get partition of idx
	const Partition* partition;
	if(random_entry_constraints.sfc == RANDOM_SURFACE)
	{
		std::vector<Partition>::const_iterator iter;
		partition = NULL;
		LIST_FOREACH(latticepartitions, iter)
			if(idx >= iter->start && idx < iter->start + iter->length)
			{
				partition = &*iter;
				break;
			}
		if(!partition)
			return NULL;
	}
	else
		partition = &latticepartitions[latticepartitionmap.find(random_entry_constraints.sfc)->second];

	// Get theta/phi of idx
	float delta_theta = 1.0f / (float)partition->numsamples_theta;
	float delta_phi = 1.0f / (float)partition->numsamples_phi;

	size_t _theta = random_entry_constraints._theta == RANDOM_INDEX ? idx % partition->numsamples_theta : random_entry_constraints._theta;
	size_t _phi = random_entry_constraints._phi == RANDOM_INDEX ? (idx / partition->numsamples_theta) % partition->numsamples_phi : random_entry_constraints._phi;
	phi =  (_phi + 0.5f) * delta_phi; // +0.5... Use the center of the bin
	theta =  (_theta + 0.5f) * delta_theta; // +0.5... Use the center of the bin

	// Get u/v of idx
	float delta_u = 1.0f / (float)partition->numsamples_u;
	float delta_v = 1.0f / (float)partition->numsamples_v;

	size_t _u = random_entry_constraints._u == RANDOM_INDEX ? (idx / (partition->numsamples_theta * partition->numsamples_phi)) % partition->numsamples_u : random_entry_constraints._u;
	size_t _v = random_entry_constraints._v == RANDOM_INDEX ? (idx / partition->stride) % partition->numsamples_v : random_entry_constraints._v;
	u = (float)(_u + 0.5f) * delta_u; // +0.5... Cast from the center of the bin
	v = (float)(_v + 0.5f) * delta_v; // +0.5... Cast from the center of the bin

	// >>> Walk down lattice hierarchy

	idx = partition->start + _v * partition->stride + _u * (partition->numsamples_theta * partition->numsamples_phi) + _phi * partition->numsamples_theta + _theta; // Recompute idx here, because it might be incorrect after applying constraints
	Entry* entry = &entries[idx];
	int splitdepth = 0;
	delta_u /= 2.0f;
	delta_v /= 2.0f;
	delta_theta /= 2.0f;
	delta_phi /= 2.0f;
	while(IsSplitEntry(entry))
	{
		++splitdepth;
		idx = ((SplitEntry*)entry)->subentryidx;

		size_t shift = 0x1;
		int irnd = rand(); // Four random bits are required
		if(IsSplit_U(entry))
		{
			delta_u /= 2.0f;
			if(irnd & 0x1) // Get random boolean from first bit of irnd
			{
				idx += shift;
				u += delta_u;
			}
			else
				u -= delta_u;
			shift <<= 1;
		}
		if(IsSplit_V(entry))
		{
			delta_v /= 2.0f;
			if(irnd & 0x2) // Get random boolean from second bit of irnd
			{
				idx += shift;
				v += delta_v;
			}
			else
				v -= delta_v;
			shift <<= 1;
		}
		if(IsSplit_Theta(entry))
		{
			delta_theta /= 2.0f;
			if(irnd & 0x4) // Get random boolean from third bit of irnd
			{
				idx += shift;
				theta += delta_theta;
			}
			else
				theta -= delta_theta;
			shift <<= 1;
		}
		if(IsSplit_Phi(entry))
		{
			delta_phi /= 2.0f;
			if(irnd & 0x8) // Get random boolean from fourth bit of irnd
			{
				idx += shift;
				phi += delta_phi;
			}
			else
				phi -= delta_phi;
			//shift <<= 1; // Not needed, unless more dimensions are added
		}

		entry = &entries[idx];
	}
	delta_u *= 2.0f;
	delta_v *= 2.0f;
	delta_theta *= 2.0f;
	delta_phi *= 2.0f;

	if(pdelta_theta)
		*pdelta_theta = delta_theta;
	if(pdelta_phi)
		*pdelta_phi = delta_phi;
	if(psplitdepth)
		*psplitdepth = splitdepth;

	if(samplearea)
		*samplearea = GetAngularArea(theta, phi, delta_theta, delta_phi, is2d); // [theta, phi] must be bin-center!

	// >>> Randomize inside bin

	float rnd;

	// Offset u, v, theta and phi from bin center
	if(subentry)
	{
		*subentry = 0x0;

		if(random_entry_constraints.offset_u == RANDOM_OFFSET)
			rnd = GetRandomOffset(delta_u);
		else
			rnd = random_entry_constraints.offset_u * delta_u;
		u += rnd;
		if(rnd >= 0.0f)
			*subentry |= 0x1;

		if(random_entry_constraints.offset_v == RANDOM_OFFSET)
			rnd = GetRandomOffset(delta_v);
		else
			rnd = random_entry_constraints.offset_v * delta_v;
		v += rnd;
		if(rnd >= 0.0f)
			*subentry |= 0x2;

		if(random_entry_constraints.offset_theta == RANDOM_OFFSET)
			rnd = GetRandomOffset(delta_theta);
		else
			rnd = random_entry_constraints.offset_theta * delta_theta;
		theta += rnd;
		if(rnd >= 0.0f)
			*subentry |= 0x4;

		if(random_entry_constraints.offset_phi == RANDOM_OFFSET)
			rnd = GetRandomOffset(delta_phi);
		else
			rnd = random_entry_constraints.offset_phi * delta_phi;
		phi += rnd;
		if(rnd >= 0.0f)
			*subentry |= 0x8;
	}
	else
	{
		if(random_entry_constraints.offset_u == RANDOM_OFFSET)
			rnd = GetRandomOffset(delta_u);
		else
			rnd = random_entry_constraints.offset_u * delta_u;
		u += rnd;

		if(random_entry_constraints.offset_v == RANDOM_OFFSET)
			rnd = GetRandomOffset(delta_v);
		else
			rnd = random_entry_constraints.offset_v * delta_v;
		v += rnd;

		if(random_entry_constraints.offset_theta == RANDOM_OFFSET)
		rnd = GetRandomOffset(delta_theta);
		else
			rnd = random_entry_constraints.offset_theta * delta_theta;
		theta += rnd;

		if(random_entry_constraints.offset_phi == RANDOM_OFFSET)
			rnd = GetRandomOffset(delta_phi);
		else
			rnd = random_entry_constraints.offset_phi * delta_phi;
		phi += rnd;
	}

	*sfc = partition->sfc;
	return entry;
}
bool RaySpace::MultiResolutionLattice::AddScaled(Entry* entry, char subentry, int splitdepth, unsigned int splitdims, const Color* newlum, float factor)
{
	if(splitconditioncbk(&entry->luminance, (Color3*)newlum, entry->raycount, splitdepth))
	{
		// >>> Split entry

		size_t entryidx = entry - &entries[0];
		entry->raycount = 0;//1; // Set the raycount to one to let the newly split entry's luminance converge a bit before getting split again

		// Create split entry
		SplitEntry newsplitentry;
		newsplitentry.splitdims = splitdims;
		newsplitentry.subentryidx = entries.size();

		// Remove non-split dimensions
		int numdims = 4;
		if((newsplitentry.splitdims & SD_PHI) == 0) // If phi isn't split
		{
			subentry = ((subentry & 0xF0) >> 1) | (subentry & 0x7); // Remove 5th bit of subentry (abcdxfgh -> 0abcdfgh)
			--numdims;
		}
		if((newsplitentry.splitdims & SD_THETA) == 0) // If theta isn't split
		{
			subentry = ((subentry & 0xF8) >> 1) | (subentry & 0x3); // Remove 6th bit of subentry (abcdexgh -> 0abcdegh)
			--numdims;
		}
		if((newsplitentry.splitdims & SD_V) == 0) // If v isn't split
		{
			subentry = ((subentry & 0xFC) >> 1) | (subentry & 0x1); // Remove 7th bit of subentry (abcdefxh -> 0abcdefh)
			--numdims;
		}
		if((newsplitentry.splitdims & SD_U) == 0) // If u isn't split
		{
			subentry >>= 1; // Remove 8th bit of subentry (abcdefgx -> 0abcdefg)
			--numdims;
		}

		// Create 2^numdims new entries at the end of the lattice
		int numnewbins = 1 << numdims;
		entries.insert(entries.end(), 1 << numdims, entries[entryidx]);

		// Illuminate one of the new subentries (determined by subentry)
		entries[newsplitentry.subentryidx + subentry].AddScaled(newlum, factor);

		// Store split entry at entryidx
		entries[entryidx] = *(Entry*)&newsplitentry;

		return true;
	}
	else
	{
		// Illuminate entry
		entry->AddScaled(newlum, factor);
		return false;
	}
}

RaySpace::RaySpace(Scene* scene, Lattice* lattice, bool is2d) : scene(scene), lattice(lattice), is2d(is2d)
{
	if(is2d)
	{
		// Fix v and theta for 2D lattices
		lattice->random_entry_constraints.offset_v = 0.0f;
		lattice->random_entry_constraints.offset_theta = 0.0f;
	}
}

void RaySpace::AddSurface(Surface* sfc, size_t numsamples_u, size_t numsamples_v, size_t numsamples_theta, size_t numsamples_phi)
{
	lattice->AddSurface(sfc, numsamples_u, is2d ? 1 : numsamples_v, is2d ? 1 : numsamples_theta, numsamples_phi);
}
Result RaySpace::Allocate()
{
	return lattice->Allocate(scene);
}

void RaySpace::Entry::AddScaled(const Color* newlum, float factor)
{
	float den = 1.0f / (float)(raycount + 1);
	ClrScale(&luminance, &luminance, (float)raycount * den);
	ClrAddScaled(&luminance, &luminance, (Color3*)newlum, factor * den);
	raycount++;
}

void RaySpace::SampleLattice(Color* clr, RayCastParams& params, TextureSampler* sampler) const
{
	Lattice::ThetaPhiIterator* iter = lattice->GetThetaPhiIterator(params.hitsfc, &params.hituv);
	Vector3 indir, outdir, reflectdir, refractdir;
	bool indir_istransmitted, outdir_istransmitted;

	// Compute outdir (omega_o) from raydir
	Vec3Inverse(&outdir, &params.raydir);
	outdir_istransmitted = Vec3Dot(&outdir, &params.hitnml) <= 0.0f;

	while(lattice->IterateThetaPhi(iter))
	{
		// Compute indir (omega_i) from phi and theta
		DirFromLattice(&indir, iter->theta, iter->phi);
		Vec3Invert(&indir);
		indir_istransmitted = Vec3Dot(&indir, &params.hitnml) >= 0.0f;

		// Apply simple BRDF between indir (omega_i) and outdir (omega_o)

#ifdef AREA_FACTOR_ON_INSERT
		const float samplearea = 1.0f;
#else
		const float samplearea = GetAngularArea(iter->theta, iter->phi, iter->delta_theta, iter->delta_phi, is2d);
#endif
		Color3 finalclr(0.0f, 0.0f, 0.0f);

		// >>> Specular part

		if(!outdir_istransmitted && // Only illuminate rays travelling outside solid objects
		   !indir_istransmitted && // Specular part only applies to reflections from outside solid objects
		   params.hitsfc->materials[0]->absorbance && (params.hitsfc->materials[0]->specularclr.r || params.hitsfc->materials[0]->specularclr.g || params.hitsfc->materials[0]->specularclr.b))
		{
			// Specular component
			Vec3Reflect(&reflectdir, &params.hitnml, &indir);
			float specdot = (1.0f + params.hitsfc->materials[0]->specularpwr) * pow(saturate(Vec3Dot(&outdir, &reflectdir)), params.hitsfc->materials[0]->specularpwr);
#ifdef DOT_PRODUCT_ON_INSERT
			// Take dot product out of specular component
			const float dot = saturate(-Vec3Dot(&params.hitnml, &indir));
			if(dot)
				specdot /= dot;
#endif
			ClrAddScaled(&finalclr, &finalclr, &iter->entry->luminance, (Color3*)&params.hitsfc->materials[0]->specularclr, samplearea * specdot * params.hitsfc->materials[0]->absorbance);
		}

		// >>> Diffuse part

		if(params.hitsfc->materials[0]->diffuseclr.r || params.hitsfc->materials[0]->diffuseclr.g || params.hitsfc->materials[0]->diffuseclr.b)
		{
			// Diffuse component
#ifdef DOT_PRODUCT_ON_INSERT
			const float dot = 1.0f;
#else
			float dot = saturate(-Vec3Dot(&params.hitnml, &indir));
#endif

			// Diffuse reflected ray
			if(!outdir_istransmitted && // Only illuminate rays travelling outside solid objects
			   !indir_istransmitted && // Diffuse reflected part only applies to reflections from outside solid objects
			   params.hitsfc->materials[0]->absorbance)
				ClrAddScaled(&finalclr, &finalclr, &iter->entry->luminance, (Color3*)&params.hitsfc->materials[0]->diffuseclr, samplearea * dot * params.hitsfc->materials[0]->absorbance);

			// Diffuse transmitted ray
			if(indir_istransmitted != outdir_istransmitted && // Transmittance happens when a ray goes either from inside to outside, or from outside to inside
			   params.hitsfc->materials[0]->transmittance)
			{
				Vec3Refract(&refractdir, &params.hitnml, &params.raydir, outdir_istransmitted ?
							params.hitsfc->materials[0]->refraction / REFRACT_IDX_AIR : REFRACT_IDX_AIR / params.hitsfc->materials[0]->refraction);
				float refractdir_theta = 0.5f - asin(refractdir.y) / PI;
				float refractdir_phi = 0.5f - atan2(refractdir.x, -refractdir.z) / (2.0f * PI);
				if(refractdir_theta >= iter->theta - iter->delta_theta / 2.0f && refractdir_theta < iter->theta + iter->delta_theta / 2.0f &&
				   refractdir_phi >= iter->phi - iter->delta_phi / 2.0f && refractdir_phi < iter->phi + iter->delta_phi / 2.0f)
				{
					//float refractdot = saturate(-Vec3Dot(&refractdir, &indir));
//float refractdot = 1.0f;
float refractdot = saturate(-Vec3Dot(&refractdir, &params.hitnml));
//const float samplearea = 2.0f * PI;
					ClrAddScaled(&finalclr, &finalclr, &iter->entry->luminance, (Color3*)&params.hitsfc->materials[0]->diffuseclr, samplearea * refractdot * params.hitsfc->materials[0]->transmittance * 50.0f); //REMOVE FACTOR
				}
			}
		}

		clr->r += finalclr.r;
		clr->g += finalclr.g;
		clr->b += finalclr.b;
	}

	delete iter;

	Color surfaceclr;
	params.hitsfc->materials[0]->Sample(&surfaceclr, &params.hituv, sampler);

	const float scale = is2d ? (1.0f / (1.0f * PI) /* scale = 1 / area_of_hemicircle */ ) : (1.0f / (2.0f * PI) /* scale = 1 / area_of_hemisphere */ );
	clr->r *= surfaceclr.r * scale;
	clr->g *= surfaceclr.g * scale;
	clr->b *= surfaceclr.b * scale;

	clr->r += surfaceclr.r * params.hitsfc->materials[0]->emissiveclr.r;
	clr->g += surfaceclr.g * params.hitsfc->materials[0]->emissiveclr.g;
	clr->b += surfaceclr.b * params.hitsfc->materials[0]->emissiveclr.b;
}
void RaySpace::SampleLatticeMC(Color* clr, RayCastParams& params, TextureSampler* sampler) const
{
	Surface* sfc;
	float u, v, theta, phi, delta_theta, delta_phi;

	Lattice::RandomEntryConstraints oldrec = lattice->random_entry_constraints;
	lattice->random_entry_constraints.sfc = params.hitsfc;
	const Lattice::Partition& partition = lattice->latticepartitions[lattice->latticepartitionmap.find(params.hitsfc)->second];
	lattice->random_entry_constraints._u = (size_t)(params.hituv.x * (float)partition.numsamples_u);
	lattice->random_entry_constraints._v = (size_t)(params.hituv.y * (float)partition.numsamples_v);
	lattice->random_entry_constraints._u = min(lattice->random_entry_constraints._u, partition.numsamples_u - 1); // This is only neccessary when hituv.x can get 1.0
	lattice->random_entry_constraints._v = min(lattice->random_entry_constraints._v, partition.numsamples_v - 1); // This is only neccessary when hituv.y can get 1.0
	lattice->random_entry_constraints.offset_u = params.hituv.x * (float)partition.numsamples_u - (float)lattice->random_entry_constraints._u;
	lattice->random_entry_constraints.offset_v = params.hituv.y * (float)partition.numsamples_v - (float)lattice->random_entry_constraints._v;

	Vector3 indir, outdir, reflectdir, refractdir;
	bool indir_istransmitted, outdir_istransmitted;

	// Compute outdir (omega_o) from raydir
	Vec3Inverse(&outdir, &params.raydir);
	outdir_istransmitted = Vec3Dot(&outdir, &params.hitnml) <= 0.0f;

	float capturedsamplearea = 0.0f;

	for(int i = 0; i < (params.hitsfc->materials[0]->transmittance ? 128 : 64/*512 : 256*/); i++)
	{
		Entry* entry = lattice->GetRandomEntry(&sfc, u, v, theta, phi, &delta_theta, &delta_phi);

		// Compute indir (omega_i) from phi and theta
		DirFromLattice(&indir, theta, phi);
		Vec3Invert(&indir);
		indir_istransmitted = Vec3Dot(&indir, &params.hitnml) >= 0.0f;

		const float dot = saturate(-Vec3Dot(&params.hitnml, &indir));
		if(params.hitsfc->materials[0]->transmittance == 0.0f && indir_istransmitted != outdir_istransmitted)
			continue;

		// Apply simple BRDF between indir (omega_i) and outdir (omega_o)

#ifdef AREA_FACTOR_ON_INSERT
		const float samplearea = 1.0f;
#else
		float samplearea = GetAngularArea(theta, phi, delta_theta, delta_phi, is2d);
		//samplearea += params.hitsfc->GetSampleArea(u, v, 1.0f / delta_u, 1.0f / delta_v);
		capturedsamplearea += samplearea;
#endif
		Color3 finalclr(0.0f, 0.0f, 0.0f);

		// >>> Specular part

		if(!outdir_istransmitted && // Only illuminate rays travelling outside solid objects
		   !indir_istransmitted && // Specular part only applies to reflections from outside solid objects
		   params.hitsfc->materials[0]->absorbance && (params.hitsfc->materials[0]->specularclr.r || params.hitsfc->materials[0]->specularclr.g || params.hitsfc->materials[0]->specularclr.b))
		{
			// Specular component
			Vec3Reflect(&reflectdir, &params.hitnml, &indir);
			float specdot = pow(saturate(Vec3Dot(&outdir, &reflectdir)), params.hitsfc->materials[0]->specularpwr);
#ifdef DOT_PRODUCT_ON_INSERT
			// Take dot product out of specular component
			if(dot)
				specdot /= dot;
#endif
			ClrAddScaled(&finalclr, &finalclr, &entry->luminance, (Color3*)&params.hitsfc->materials[0]->specularclr, samplearea * specdot * params.hitsfc->materials[0]->absorbance);
		}

		// >>> Diffuse part

		if(params.hitsfc->materials[0]->diffuseclr.r || params.hitsfc->materials[0]->diffuseclr.g || params.hitsfc->materials[0]->diffuseclr.b)
		{
			// Diffuse component
#ifdef DOT_PRODUCT_ON_INSERT
			const float dot = 1.0f;
#else
			float dot = saturate(-Vec3Dot(&params.hitnml, &indir));
#endif

//float lum = MAX(MAX(iter->entry->luminance.r, iter->entry->luminance.g), iter->entry->luminance.b);
//ClrAddScaled(&finalclr, &finalclr, &Color3(1.0f, 1.0f, 1.0f), samplearea * lum); //DEBUG: Display white

			// Diffuse reflected ray
			if(!outdir_istransmitted && // Only illuminate rays travelling outside solid objects
			   !indir_istransmitted && // Diffuse reflected part only applies to reflections from outside solid objects
			   params.hitsfc->materials[0]->absorbance)
				ClrAddScaled(&finalclr, &finalclr, &entry->luminance, (Color3*)&params.hitsfc->materials[0]->diffuseclr, samplearea * dot * params.hitsfc->materials[0]->absorbance);
				//ClrAddScaled(&finalclr, &finalclr, &Color3(0.0f, 1.0f, 0.0f), samplearea * lum); //DEBUG: Display green

			// Diffuse transmitted ray
			if(indir_istransmitted != outdir_istransmitted && // Transmittance happens when a ray goes either from inside to outside, or from outside to inside
			   params.hitsfc->materials[0]->transmittance)
			{
				/*Vec3Refract(&refractdir, &params.hitnml, &indir, outdir_istransmitted ?
							params.hitsfc->materials[0]->refraction / REFRACT_IDX_AIR : REFRACT_IDX_AIR / params.hitsfc->materials[0]->refraction);
				float refractdot = saturate(-Vec3Dot(&outdir, &refractdir));
//float refractdot = 1.0f;
				ClrAddScaled(&finalclr, &finalclr, &iter->entry->luminance, samplearea * refractdot * dot * params.hitsfc->materials[0]->transmittance * 2.0f); //REMOVE FACTOR
				//ClrAddScaled(&finalclr, &finalclr, &Color3(0.0f, 0.0f, 1.0f), samplearea * lum); //DEBUG: Display blue*/

				Vec3Refract(&refractdir, &params.hitnml, &outdir, outdir_istransmitted ?
							params.hitsfc->materials[0]->refraction / REFRACT_IDX_AIR : REFRACT_IDX_AIR / params.hitsfc->materials[0]->refraction);
				float refractdir_theta = 0.5f - asin(refractdir.y) / PI;
				float refractdir_phi = 0.5f - atan2(refractdir.x, -refractdir.z) / (2.0f * PI);
				if(refractdir_theta >= theta - delta_theta / 2.0f && refractdir_theta < theta + delta_theta / 2.0f &&
				   refractdir_phi >= phi - delta_phi / 2.0f && refractdir_phi < phi + delta_phi / 2.0f)
				{
					//float refractdot = saturate(Vec3Dot(&indir, &refractdir));
float refractdot = 1.0f;
samplearea = 2.0f * PI;
					ClrAddScaled(&finalclr, &finalclr, &entry->luminance, samplearea * refractdot * params.hitsfc->materials[0]->transmittance * 2.0f); //REMOVE FACTOR
				}
			}
		}

		clr->r += finalclr.r;
		clr->g += finalclr.g;
		clr->b += finalclr.b;
	}

	Color surfaceclr;
	params.hitsfc->materials[0]->Sample(&surfaceclr, &params.hituv, sampler);
	if(capturedsamplearea)
	{
		const float scale = 1.0f / capturedsamplearea;
		clr->r *= surfaceclr.r * scale;
		clr->g *= surfaceclr.g * scale;
		clr->b *= surfaceclr.b * scale;
	}
	else
		ClrSet(clr, 0.0f, 0.0f, 0.0f);

	clr->r += surfaceclr.r * params.hitsfc->materials[0]->emissiveclr.r;
	clr->g += surfaceclr.g * params.hitsfc->materials[0]->emissiveclr.g;
	clr->b += surfaceclr.b * params.hitsfc->materials[0]->emissiveclr.b;

	lattice->random_entry_constraints = oldrec;
}

void RaySpace::CastRandomBwd(RayCastParams& params, TextureSampler* sampler)
{
Surface* raydest_sfc;
float u, v, theta, phi, delta_theta, delta_phi;
char subentry;
int splitdepth;
recast_random:
#ifdef AREA_FACTOR_ON_INSERT
float samplearea;
Entry* raydest_entry = lattice->GetRandomEntry(&raydest_sfc, u, v, theta, phi, &delta_theta, &delta_phi, &subentry, &splitdepth, &samplearea);
#else
const float samplearea = 1.0f;
Entry* raydest_entry = lattice->GetRandomEntry(&raydest_sfc, u, v, theta, phi, &delta_theta, &delta_phi, &subentry, &splitdepth);
#endif

	Vector3 raydest_sfcnml;
	if(!raydest_sfc->GetPositionAndNormal(&params.raypos, &raydest_sfcnml, u, v))
		goto recast_random; //return;

	DirFromLattice(&params.raydir, theta, phi);

	float dot = Vec3Dot(&params.raydir, &raydest_sfcnml);
bool foo = (dot <= 0.0f);
	if(dot <= 0.0f)
	{
		// Ray is comming from inside the surface

		if(raydest_sfc->materials[0]->transmittance == 0.0f)
			goto recast_random; // Ignore rays from inside the target object for non-transparent objects //EDIT: Flip ray instead of rejecting it (but this could violate random_entry_constraints)

		dot *= -1.0f; // Make dot product positive

		params.SetCullMode(Intersection::CM_FRONT);
	}
	else
		params.SetCullMode(Intersection::CM_BACK);
#ifdef DOT_PRODUCT_ON_INSERT
	dot = 1.0f;
#endif

	// >>> Cast ray backwards from lattice[idx] and add illumination sampled from ray cast destination to ray cast source

bool split; //DELETE
	const unsigned int splitdims = is2d ? (MultiResolutionLattice::SD_U | MultiResolutionLattice::SD_PHI) : (MultiResolutionLattice::SD_U | MultiResolutionLattice::SD_V | MultiResolutionLattice::SD_THETA | MultiResolutionLattice::SD_PHI);
	Color clr(0.0f, 0.0f, 0.0f);
	if(scene->CastRay(params, sampler))
	{
		// Sample illumination at ray cast destination (outgoing direction)
		SampleLattice(&clr, params, sampler);
	}
	else
	{
		//clr = Color(0.0f, 0.0f, 0.0f); // Zero illumination // EDIT: Should be bgclr
	}

	// Add illumination to ray cast source (incoming direction)
	split = lattice->AddScaled(raydest_entry, subentry, splitdepth, splitdims, &clr, dot * samplearea);


/*{ //DELETE
	Vector3 v0, v1;
	v0 = params.raypos; //DELETE
	Vec3Add(&v1, &params.raypos, &params.raydir); //DELETE
	scene->DebugDrawRay(&v0, &v1, &Color(0xFF00FF00)); //DELETE
} //DELETE

{ //DELETE
	Vector3 v0, v1;
	v0 = params.raypos; //DELETE
	Vec3Add(&v1, &params.raypos, &sfcnml); //DELETE
	scene->DebugDrawRay(&v0, &v1, &Color(0xFF00FFFF)); //DELETE
} //DELETE*/

/*if(split) //DELETE
{ //DELETE
	Vec3Scale(&params.hitpos, &params.raydir, MIN(params.hitdist, 1000.0f)); //DELETE
	Vec3Add(&params.hitpos, &params.raypos, &params.hitpos); //DELETE
	scene->DebugDrawRay(&params.raypos, &params.hitpos, &Color(0xFFFF0000)); //DELETE
	scene->DebugDrawRay(&params.raypos, &params.hitpos, &Color(split ? 0xFFFF0000 : 0xFF0000FF)); //DELETE
} //DELETE */
}

bool RaySpace::CastRandomFwd(RayCastParams& params, TextureSampler* sampler, BinDesc* destbin)
{
Surface* raysrc_sfc;
float u, v, theta, phi;
char subentry;
int splitdepth;
recast_random:
Entry* raysrc_entry = lattice->GetRandomEntry(&raysrc_sfc, u, v, theta, phi, NULL, NULL, &subentry, &splitdepth);

	Vector3 sfcnml;
	if(!raysrc_sfc->GetPositionAndNormal(&params.raypos, &sfcnml, u, v))
		return false;//goto recast_random;

	DirFromLattice(&params.raydir, theta, phi);

	float srcsfcdot = Vec3Dot(&params.raydir, &sfcnml);
	if(srcsfcdot <= 0.0f)
	{
		// Ray is going inside the surface

		if(raysrc_sfc->materials[0]->transmittance == 0.0f)
			return false;//goto recast_random; // Ignore rays going into the target object for non-transparent objects //EDIT: Flip ray instead of rejecting it (but this could violate random_entry_constraints)

		srcsfcdot *= -1.0f; // Make dot product positive

		params.SetCullMode(Intersection::CM_FRONT);
	}
	else
		params.SetCullMode(Intersection::CM_BACK);

	// >>> Cast ray to lattice[idx] and add illumination sampled from ray cast source to ray cast destination

	if(scene->CastRay(params, sampler))
	{
		// Add illumination to ray cast destination

		//  Invert [theta, phi] (slower method)
		Vec3Invert(&params.raydir);
		theta = 0.5f - asin(params.raydir.y) / PI;
		phi = 0.5f - atan2(params.raydir.x, -params.raydir.z) / (2.0f * PI);
		Vec3Invert(&params.raydir);

		char subentry;
		int splitdepth;
#ifdef AREA_FACTOR_ON_INSERT
		float samplearea;
		RaySpace::Entry* raydest_entry = lattice->GetEntry(params.hitsfc, params.hituv.x, params.hituv.y, theta, phi, NULL, NULL, &subentry, &splitdepth, &samplearea, destbin);
#else
		const float samplearea = 1.0f;
		RaySpace::Entry* raydest_entry = lattice->GetEntry(params.hitsfc, params.hituv.x, params.hituv.y, theta, phi, NULL, NULL, &subentry, &splitdepth, NULL, destbin);
#endif
#ifdef DOT_PRODUCT_ON_INSERT
		float dot = saturate(-Vec3Dot(&params.raydir, &params.hitnml));
#else
		const float dot = 1.0f;
#endif

		Color clr(0.0f, 0.0f, 0.0f);
		Vec2Set(&params.hituv, u, v);
		params.hitnml = sfcnml;
		Vec3Invert(&params.raydir);
		params.hitsfc = raysrc_sfc;
		// Sample illumination at ray cast source (outgoing direction)
		SampleLattice(&clr, params, sampler);

		const unsigned int splitdims = is2d ? (MultiResolutionLattice::SD_U | MultiResolutionLattice::SD_PHI) : (MultiResolutionLattice::SD_U | MultiResolutionLattice::SD_V | MultiResolutionLattice::SD_THETA | MultiResolutionLattice::SD_PHI);
		bool split = lattice->AddScaled(raydest_entry, subentry, splitdepth, splitdims, &clr, dot * samplearea);

/*{ //DELETE
	Vec3Scale(&params.hitpos, &params.raydir, MIN(params.hitdist, 1000.0f)); //DELETE
	Vec3Add(&params.hitpos, &params.raypos, &params.hitpos); //DELETE
	scene->DebugDrawRay(&params.raypos, &params.hitpos, &Color(split ? 0xFFFF0000 : 0xFF0000FF)); //DELETE
} //DELETE*/

		return true;
	}

	return false;
}

void RaySpace::DirFromLattice(Vector3* dir, float theta, float phi)
{
	dir->x = sin(phi * 2.0f * PI) * sin(theta * PI);
	dir->y = cos(-theta * PI);
	dir->z = cos(phi * 2.0f * PI) * sin(theta * PI);
}
float RaySpace::GetAngularArea(float theta, float phi, size_t numsamples_theta, size_t numsamples_phi, bool is2d) // [theta, phi] ... center of bin
{
	if(is2d)
	{
		assert(numsamples_theta == 1);

		return 2.0f * PI / (float)numsamples_phi;
	}
	else
	{
		// Subtract spherical cap with h=theta-0.5*delta_theta (lower bound of bin) from spherical cap with h=theta+0.5*delta_theta (upper bound of bin)
		auto h = [&](float alpha) {return 1.0f - cos(alpha * PI);};
		float h0 = h(theta - 0.5f / (float)numsamples_theta); // delta_theta = 1 / numsamples_theta
		float h1 = h(theta + 0.5f / (float)numsamples_theta); // delta_theta = 1 / numsamples_theta
		float A = 2.0f * PI * (h1 - h0);

		A /= (float)numsamples_phi; // Divide by number of sphere stacks

		return A;
	}
}
float RaySpace::GetAngularArea(float theta, float phi, float delta_theta, float delta_phi, bool is2d) // [theta, phi] ... center of bin
{
	if(is2d)
	{
		assert(delta_theta == 1.0f);

		return 2.0f * PI * delta_phi;
	}
	else
	{
		// Subtract spherical cap with h=theta (lower bound of bin) from spherical cap with h=theta+delta_theta (upper bound of bin)
		auto h = [&](float alpha) {return 1.0f - cos(alpha * PI);};
		float h0 = h(theta - 0.5f * delta_theta); // delta_theta = 1 / numsamples_theta
		float h1 = h(theta + 0.5f * delta_theta); // delta_theta = 1 / numsamples_theta
		float A = 2.0f * PI * (h1 - h0);

		A *= delta_phi; // Divide by number of sphere stacks

		return A;
	}
}
float RaySpace::GetBinCenter(float coordinate, float delta)
{
		// coordinate = floor(coordinate / delta) * delta + delta / 2
		coordinate /= delta;
		coordinate = (float)(int)coordinate;
		coordinate += 0.5f;
		coordinate *= delta;
		return coordinate;
}
void RaySpace::QuantifyRayDir(const Vector3* raydir, size_t* offset, size_t numsamples_theta, size_t numsamples_phi) //EDIT: Untested
{
	float theta = 0.5f - asin(raydir->y) / PI;
	float phi = 0.5f - atan2(raydir->x, -raydir->z) / (2.0f * PI);
	size_t _theta = (size_t)(theta * (float)numsamples_theta); _theta = min(_theta, numsamples_theta - 1);
	size_t _phi = (size_t)(phi * (float)numsamples_phi); _phi %= numsamples_phi;
	*offset = _phi * numsamples_theta + _theta;
}

//-----------------------------------------------------------------------------
// Name: SampleLattice()
// Desc: Shoot numsamples random rays through the ray space to approach a steady state solution
//-----------------------------------------------------------------------------
void RaySpace::SampleLattice(int numsamples, const RaySpaceSamplingOptions& options)
{
	RayCastParams params(Intersection::CM_BACK);
	params.scene = scene;
	params.raypos = scene->cam->pos;
	params.flags = RCP_GET_HIT_UV | RCP_GET_HIT_NML;

	TextureSampler* sampler;
	switch(options.texsampling)
	{
	case RayTracingOptions::TS_POINT: sampler = new PointTextureSampler(); break;
	case RayTracingOptions::TS_LINEAR: sampler = new LinearTextureSampler(); break;
	default:
		Result::PrintLogMessage(String("Unknown texture sampler id: ") << String((int)options.texsampling));
		sampler = new PointTextureSampler();
	}

	MultiResolutionLattice* multireslattice = dynamic_cast<MultiResolutionLattice*>(lattice);
	if(multireslattice)
		multireslattice->splitconditioncbk = options.splitconditioncbk;

	lattice->random_entry_constraints.Reset(is2d);

	if(options.bfstracing)
	{
		std::queue<BinDesc> queue;
		BinDesc bin;

		int totalnumrays = numsamples;
		int numdirectrays = 1000;

		while(totalnumrays)
		{
			while(totalnumrays && numdirectrays)
			{
				std::list<Surface*>::iterator iter;
				LIST_FOREACH(scene->surfaces, iter)
					if((*iter)->materials[0]->emissiveclr.r || (*iter)->materials[0]->emissiveclr.g || (*iter)->materials[0]->emissiveclr.b)
					{
						lattice->random_entry_constraints.sfc = *iter;

						// Log number of rays every 100000 rays
						if(!options.quietmodeenabled && ((numsamples - totalnumrays) / 100000) * 100000 == numsamples - totalnumrays)
						{
							cle->Print(numsamples - totalnumrays);
							cle->Print(" of ");
							cle->Print(numsamples);
							cle->PrintLine(" random rays casted");
						}

						if(CastRandomFwd(params, sampler, &bin))
							queue.push(bin);
						--totalnumrays;
						--numdirectrays;

						if(!options.keepalive)
							return;
					}
			}

			while(totalnumrays && queue.size())
			{
				//lattice->random_entry_constraints.SetBin(&queue.front());
lattice->random_entry_constraints._u = queue.front()._u;
lattice->random_entry_constraints._v = queue.front()._v;
lattice->random_entry_constraints.sfc = queue.front().sfc;
				queue.pop();
				if(CastRandomFwd(params, sampler, &bin) && (numsamples - totalnumrays) + (int)queue.size() < totalnumrays && rand() & 0x1) // 50% chance: follow the path
					queue.push(bin);
				--totalnumrays;

				// Log number of rays every 100000 rays
				if(!options.quietmodeenabled && ((numsamples - totalnumrays) / 100000) * 100000 == numsamples - totalnumrays)
				{
					cle->Print(numsamples - totalnumrays);
					cle->Print(" of ");
					cle->Print(numsamples);
					cle->PrintLine(" random rays casted");
				}

				if(!options.keepalive)
					return;
			}

			if(queue.size() == 0)
				numdirectrays = 1000;

			lattice->random_entry_constraints.Reset(is2d);
		}
	}
	else
	{
		int ray = 0;
		auto CastRays = [&](int numrays)
		{
			if(options.castbackwards)
				for(int i = 0; i < numrays; i++, ray++)
				{
					// Log number of rays every 100000 rays
					if(!options.quietmodeenabled && (i / 100000) * 100000 == i)
					{
						cle->Print(ray);
						cle->Print(" of ");
						cle->Print(numsamples);
						cle->PrintLine(" random rays casted");
					}

					CastRandomBwd(params, sampler);

					if(!options.keepalive)
						return;
				}
			else
				for(int i = 0; i < numrays; i++, ray++)
				{
					// Log number of rays every 100000 rays
					if(!options.quietmodeenabled && (i / 100000) * 100000 == i)
					{
						cle->Print(ray);
						cle->Print(" of ");
						cle->Print(numsamples);
						cle->PrintLine(" random rays casted");
					}

					CastRandomFwd(params, sampler);

					if(!options.keepalive)
						return;
				}
		};

		if(options.constraints & RaySpaceSamplingOptions::REC_EMMISSIVE_SFC)
		{
			int numrayspersurface = numsamples / scene->surfaces.size();

			std::list<Surface*>::iterator iter;
			LIST_FOREACH(scene->surfaces, iter)
			{
				lattice->random_entry_constraints.sfc = *iter;
				CastRays(numrayspersurface);
			}
		}
		else
			CastRays(numsamples);
	}

	delete sampler;
}

//-----------------------------------------------------------------------------
// Name: PostProcessImage()
// Desc: Transform rayspace from global coordinate system to surface-local coordinate system
//-----------------------------------------------------------------------------
void PostProcessImage(Image* img, Image* img2, const Scene* scene, int resolution_u, int resolution_phi)
{
	resolution_phi /= 4;

	DWORD* data = (DWORD*)img->data;
	DWORD* data2 = (DWORD*)img2->data;
	auto PostProcessUpFacingSurface = [&](int xmin, int xmax)
	{
		for(int x = xmin; x < xmax; x++)
			for(int y = 0; y < img2->height; y++)
				data2[y * img2->width + x] = data[(y + resolution_phi) * img->width + x];
	};
	auto PostProcessDownFacingSurface = [&](int xmin, int xmax)
	{
		for(int x = xmin; x < xmax; x++)
			for(int y = 0; y < img2->height / 2; y++)
			{
				data2[(img2->height / 2 + y) * img2->width + (xmax + xmin - x - 1)] = data[y * img->width + x];
				data2[(img2->height / 2 - y - 1) * img2->width + (xmax + xmin - x - 1)] = data[(img->height - y - 1) * img->width + x];
			}
	};
	auto PostProcessCircularSurface = [&](int xmin, int xmax)
	{
		for(int x = xmin; x < xmax; x++)
			for(int y = 0; y < img2->height; y++)
			{
				double offset = (double)(x - xmin) / (double)(xmax - xmin);
				offset += 0.5;
				offset *= img->height;
				DWORD c1 = data[((2 * y + (int)offset) % img->height) * img->width + x];
				DWORD c2 = data[((2 * y + 1 + (int)offset) % img->height) * img->width + x];
				DWORD c12 = (min((((c1 >>  0) & 0xFF) + ((c2 >>  0) & 0xFF) / 2), 255) <<  0) |
							(min((((c1 >>  8) & 0xFF) + ((c2 >>  8) & 0xFF) / 2), 255) <<  8) |
							(min((((c1 >> 16) & 0xFF) + ((c2 >> 16) & 0xFF) / 2), 255) << 16);
				data2[y * img2->width + x] = c12 | 0xFF000000;
			}
	};

	std::list<Surface*>::const_iterator iter;
	const Surface* sfc;
	const SphereSurface* spheresfc;
	const MeshSurface* meshsfc;
	int sfcidx;
	LIST_FOREACH_I(scene->surfaces, iter, sfcidx)
	{
		sfc = (Surface*)(*iter);
		spheresfc = dynamic_cast<const SphereSurface*>(sfc);
		meshsfc = dynamic_cast<const MeshSurface*>(sfc);
		if(spheresfc)
			PostProcessCircularSurface(sfcidx * resolution_u, (sfcidx + 1) * resolution_u);
		else if(meshsfc)
		{
			Vector3 sfcnml;
			if(meshsfc->GetNormal(&sfcnml, 0.5f, 0.5f))
			{
				if(sfcnml.z > 0.0f)
					PostProcessDownFacingSurface(sfcidx * resolution_u, (sfcidx + 1) * resolution_u);
				else
					PostProcessUpFacingSurface(sfcidx * resolution_u, (sfcidx + 1) * resolution_u);
			}
		}
		else
			LOG("Unable to determine image post processing for surface type other than mesh or sphere");
	}

	/*// ref:
	PostProcessDownFacingSurface(0, resolution_u);
	PostProcessUpFacingSurface(resolution_u, resolution_u + resolution_u / 2);
	PostProcessDownFacingSurface(resolution_u + resolution_u / 2, 2 * resolution_u);
	PostProcessUpFacingSurface(2 * resolution_u, 3 * resolution_u);

	// transparency2d:
	PostProcessDownFacingSurface(0, resolution_u);
	PostProcessUpFacingSurface(resolution_u, 2 * resolution_u);
	PostProcessCircularSurface(2 * resolution_u, 3 * resolution_u);*/
}

//-----------------------------------------------------------------------------
// Name: Draw2DRaySpace()
// Desc: Draw the ray space to an image of size [(resolution_u * num_surfaces) x resolution_phi] (throws an error in 3D mode)
//		 If splitimg != NULL, a second image will be drawn showing the number of splits for each bin
//-----------------------------------------------------------------------------
Result RaySpace::Draw2DRaySpace(int resolution_u, int resolution_phi, Image** pimg, Image** psplitimg) const
{
	if(!pimg)
		return ERR("Parameter img is NULL");
	*pimg = NULL;
	if(psplitimg)
		*psplitimg = NULL;
	if(!is2d)
		return ERR("Cannot draw 4D ray space to 2D image");

	Image* img = Image::New(resolution_u * scene->surfaces.size(), resolution_phi, 4);
	DWORD* data = (DWORD*)img->data;
	Image* splitimg;
	DWORD* splitdata;
	if(psplitimg)
	{
		splitimg = Image::New(resolution_u * scene->surfaces.size(), resolution_phi, 4);
		splitdata = (DWORD*)splitimg->data;
	}

	std::list<Surface*>::const_iterator iter;
	const Surface* sfc;
	int sfcidx;
	if(psplitimg)
	{
		LIST_FOREACH_I(scene->surfaces, iter, sfcidx)
		{
			sfc = (Surface*)(*iter);

			float delta_u = 1.0f / (float)resolution_u;
			float delta_phi = 1.0f / (float)resolution_phi;
			for(int phi = 0; phi < resolution_phi; phi++)
				for(int u = 0; u < resolution_u; u++)
				{
					int splitdepth;
					Color3 clr = lattice->GetEntry(sfc, (float)(u + 0.5f) * delta_u, 0.5f, 0.5f, (float)(phi + 0.5f) * delta_phi, NULL, NULL, NULL, &splitdepth)->luminance;
					data[phi * img->width + (u + resolution_u * sfcidx)] = clr.ToDWORD() | 0xFF000000;
					clr = Color3((float)splitdepth / 8.0f, 0.0f, 0.0f);
					splitdata[phi * img->width + (u + resolution_u * sfcidx)] = clr.ToDWORD() | 0xFF000000;
				}
		}
	}
	else
	{
		LIST_FOREACH_I(scene->surfaces, iter, sfcidx)
		{
			sfc = (Surface*)(*iter);

			float delta_u = 1.0f / (float)resolution_u;
			float delta_phi = 1.0f / (float)resolution_phi;
			for(int phi = 0; phi < resolution_phi; phi++)
				for(int u = 0; u < resolution_u; u++)
				{
					Color3 clr = lattice->GetEntry(sfc, (float)(u + 0.5f) * delta_u, 0.5f, 0.5f, (float)(phi + 0.5f) * delta_phi)->luminance;
					data[phi * img->width + (u + resolution_u * sfcidx)] = clr.ToDWORD() | 0xFF000000;
				}
		}
	}

	Image* img2 = Image::New(img->width, img->height / 2, 4);
	PostProcessImage(img, img2, scene, resolution_u, resolution_phi);
	delete img;
	*pimg = img2;

	if(psplitimg)
	{
		Image* splitimg2 = Image::New(splitimg->width, splitimg->height / 2, 4);
		PostProcessImage(splitimg, splitimg2, scene, resolution_u, resolution_phi);
		delete splitimg;
		*psplitimg = splitimg2;
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawIntegratedRaySpace()
// Desc: Draw the integrated ray space to an image of size [(resolution_u * num_surfaces) x resolution_phi] (throws an error in 3D mode)
//-----------------------------------------------------------------------------
Result RaySpace::DrawIntegratedRaySpace(int resolution_u, int resolution_phi, Image** pimg) const
{
	if(!pimg)
		return ERR("Parameter img is NULL");
	*pimg = NULL;

	if(!is2d)
		return ERR("Cannot draw 4D ray space to 2D image");

	RayCastParams params(Intersection::_CM_NONE);
	params.scene = scene;
	params.hituv.y = 0.5f;

	LinearTextureSampler tsampler;

	Image* img = Image::New(resolution_u * scene->surfaces.size(), resolution_phi, 4);
	DWORD* data = (DWORD*)img->data;

	float delta_u = 1.0f / (float)resolution_u;
	float delta_phi = 1.0f / (float)resolution_phi;

	std::list<Surface*>::const_iterator iter;
	int sfcidx;
	Color clr;
	LIST_FOREACH_I(scene->surfaces, iter, sfcidx)
	{
		params.hitsfc = (Surface*)(*iter);

		for(int u = 0; u < resolution_u; u++)
		{
			params.hituv.x = (float)(u + 0.5f) * delta_u;
			if(params.hitsfc->GetNormal(&params.hitnml, params.hituv.x, params.hituv.y))
			{
				for(int phi = 0; phi < resolution_phi; phi++)
				{
					float p = (float)(phi + 0.5f) * delta_phi;
					p += 0.5f;
					if(p >= 1.0f)
						p -= 1.0f;
					DirFromLattice(&params.raydir, 0.5f, p);

					SampleLattice(&clr, params, (TextureSampler*)&tsampler);
					data[phi * img->width + (u + resolution_u * sfcidx)] = clr.ToDWORD() | 0xFF000000;
				}
			}
		}
	}

	Image* img2 = Image::New(img->width, img->height / 2, 4);
	PostProcessImage(img, img2, scene, resolution_u, resolution_phi);
	//delete img;
	*pimg = img2;

	return R_OK;
}

/*//-----------------------------------------------------------------------------
// Name: DrawIntegratedRaySpace()
// Desc: Draw the integrated ray space to an image of size [(resolution_u * num_surfaces) x resolution_v] (resolution_v is 1 for 2D mode)
//-----------------------------------------------------------------------------
Result RaySpace::DrawIntegratedRaySpace(int resolution_u, int resolution_v, Image** pimg) const
{
	if(!pimg)
		return ERR("Parameter img is NULL");
	*pimg = NULL;

	if(!is2d)
		resolution_v = 1;

	RayCastParams params(Intersection::_CM_NONE);
	params.scene = scene;

	LinearTextureSampler tsampler;

	Image* img = Image::New(resolution_u * scene->surfaces.size(), resolution_v, 4);
	DWORD* data = (DWORD*)img->data;

	std::list<Surface*>::const_iterator iter;
	int sfcidx;
	Color clr;
	LIST_FOREACH_I(scene->surfaces, iter, sfcidx)
	{
		params.hitsfc = (Surface*)(*iter);

		float delta_u = 1.0f / (float)resolution_u;
		float delta_v = 1.0f / (float)resolution_v;
		for(int v = 0; v < resolution_v; v++)
		{
			params.hituv.y = (float)(v + 0.5f) * delta_v;
			for(int u = 0; u < resolution_u; u++)
			{
				params.hituv.x = (float)(u + 0.5f) * delta_u;
				if(params.hitsfc->GetNormal(&params.hitnml, params.hituv.x, params.hituv.y))
				{
					params.raydir = params.hitnml;
					Vec3Invert(&params.raydir);
					SampleLattice(&clr, params, (TextureSampler*)&tsampler);
					data[v * img->width + (u + resolution_u * sfcidx)] = clr.ToDWORD() | 0xFF000000;
				}
			}
		}
	}

	*pimg = img;
	return R_OK;
}*/

//-----------------------------------------------------------------------------
// Name: Compare()
// Desc: Return the average congruency (range = [0; 1]) between two lattices
//-----------------------------------------------------------------------------
double RaySpace::Compare(IRaySpace* _other) const
{
	RaySpace* other = (RaySpace*)_other;
	if(lattice->latticepartitions.size() != other->lattice->latticepartitions.size())
	{
		LOG("Cannot compare ray spaces based on different sets of surfaces");
		return 0.0;
	}

	double congruency = 0.0;
	int numsamples = 0;

	std::list<Surface*>::const_iterator iter1, iter2;
	for(std::list<Surface*>::const_iterator iter1 = scene->surfaces.begin(), iter2 = other->scene->surfaces.begin(); iter1 != scene->surfaces.end() || iter2 != other->scene->surfaces.end(); iter1++, iter2++)
	{
		const Surface *sfc1 = *iter1, *sfc2 = *iter2;
		const Lattice::Partition& partition1 = lattice->latticepartitions[lattice->latticepartitionmap.find(sfc1)->second];
		const Lattice::Partition& partition2 = other->lattice->latticepartitions[other->lattice->latticepartitionmap.find(sfc2)->second];

		int resolution_u = max(partition1.numsamples_u, partition2.numsamples_u); float delta_u = 1.0f / (float)resolution_u;
		int resolution_v = max(partition1.numsamples_v, partition2.numsamples_v); float delta_v = 1.0f / (float)resolution_v;
		int resolution_theta = max(partition1.numsamples_theta, partition2.numsamples_theta); float delta_theta = 1.0f / (float)resolution_theta;
		int resolution_phi = max(partition1.numsamples_phi, partition2.numsamples_phi); float delta_phi = 1.0f / (float)resolution_phi;

		for(int u = 0; u < resolution_u; u++)
			for(int v = 0; v < resolution_v; v++)
				for(int theta = 0; theta < resolution_theta; theta++)
					for(int phi = 0; phi < resolution_phi; phi++)
					{
						Color3 clr1 = lattice->GetEntry(sfc1, (float)(u + 0.5f) * delta_u, (float)(v + 0.5f) * delta_v, (float)(theta + 0.5f) * delta_theta, (float)(phi + 0.5f) * delta_phi)->luminance;
						Color3 clr2 = other->lattice->GetEntry(sfc2, (float)(u + 0.5f) * delta_u, (float)(v + 0.5f) * delta_v, (float)(theta + 0.5f) * delta_theta, (float)(phi + 0.5f) * delta_phi)->luminance;
						double delta = (abs((double)clr1.r - (double)clr2.r) + abs((double)clr1.g - (double)clr2.g) + abs((double)clr1.b - (double)clr2.b)) / 3.0;
						congruency += delta;
						++numsamples;
					}
	}

	return congruency / (double)numsamples;
}

//-----------------------------------------------------------------------------
// Name: Save()
// Desc: Save the lattice to file
//-----------------------------------------------------------------------------
Result RaySpace::Save(const FilePath& filename) const
{
	std::ofstream fs(filename, std::ios::binary);
	
	bool ismultires = (dynamic_cast<MultiResolutionLattice*>(lattice) != NULL);
	fs.write((char*)&ismultires, sizeof(bool));

	fs.write((char*)&is2d, sizeof(bool));

	size_t numlatticepartitions = lattice->latticepartitions.size();
	fs.write((char*)&numlatticepartitions, sizeof(size_t));
	std::vector<Lattice::Partition>::const_iterator iter;
	LIST_FOREACH(lattice->latticepartitions, iter)
	{
		fs.write((char*)&iter->numsamples_u, sizeof(size_t));
		fs.write((char*)&iter->numsamples_v, sizeof(size_t));
		fs.write((char*)&iter->numsamples_theta, sizeof(size_t));
		fs.write((char*)&iter->numsamples_phi, sizeof(size_t));
	}

	size_t numentries = lattice->entries.size();
	fs.write((char*)&numentries, sizeof(size_t));
	fs.write((char*)&lattice->entries[0], numentries * sizeof(Entry));

	fs.close();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: FromFile()
// Desc: Load the lattice from file
//-----------------------------------------------------------------------------
Result RaySpace::FromFile(std::ifstream& stream, Scene* scene, const std::list<Surface*> surfaces, RaySpace** rayspace)
{
	*rayspace = NULL;

	bool ismultires, is2d;
	stream.read((char*)&ismultires, sizeof(bool));
	stream.read((char*)&is2d, sizeof(bool));

	size_t numlatticepartitions;
	stream.read((char*)&numlatticepartitions, sizeof(size_t));
	if(surfaces.size() != numlatticepartitions)
		return ERR("Number of surfaces does not match number of serialized lattice partitions");

	RaySpace* newrayspace;
	CHKALLOC(newrayspace = new RaySpace(scene, ismultires ? (RaySpace::Lattice*)new RaySpace::MultiResolutionLattice(is2d) : (RaySpace::Lattice*)new RaySpace::SingleResolutionLattice(is2d), is2d));

	std::list<Surface*>::const_iterator iter;
	Surface* sfc;
	int i;
	size_t numsamples_u, numsamples_v, numsamples_theta, numsamples_phi;
	LIST_FOREACH_I(surfaces, iter, i)
	{
		sfc = (*iter);
		stream.read((char*)&numsamples_u, sizeof(size_t));
		stream.read((char*)&numsamples_v, sizeof(size_t));
		stream.read((char*)&numsamples_theta, sizeof(size_t));
		stream.read((char*)&numsamples_phi, sizeof(size_t));
		newrayspace->AddSurface(*iter, numsamples_u, numsamples_v, numsamples_theta, numsamples_phi);
	}

	size_t numentries;
	stream.read((char*)&numentries, sizeof(size_t));
	newrayspace->lattice->entries.resize(numentries);
	stream.read((char*)&newrayspace->lattice->entries[0], numentries * sizeof(Entry));

	*rayspace = newrayspace;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: GetLatticeSize()
// Desc: Return lattice size in bytes
//-----------------------------------------------------------------------------
size_t RaySpace::GetLatticeSize() const
{
	return lattice->entries.size();
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void RaySpace::Release()
{
	scene->rayspaces.remove(this);

	if(lattice)
		delete lattice;

	delete this;
}