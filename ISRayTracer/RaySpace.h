#ifndef __RAYSPACE_H
#define __RAYSPACE_H

#include "ISRaySpace.h"
#include <boost\random\mersenne_twister.hpp>
#include <boost\random\uniform_int_distribution.hpp>

class Scene;
class Surface;
class TextureSampler;
struct RayCastParams;

static const size_t RANDOM_INDEX = (size_t)-1;
static const Surface* RANDOM_SURFACE = NULL;
static const float RANDOM_OFFSET = 1e20f;

//-----------------------------------------------------------------------------
// Name: RaySpace
// Desc: A 4D lattice covering two dimensions of incoming angles and two dimensions of surface area for every surface in a scene
//-----------------------------------------------------------------------------
class RaySpace : public ISRayTracer::IRaySpace
{
public:
	struct Entry
	{
		Color3 luminance;
		int raycount;
		void AddScaled(const Color* newlum, float factor);
	};
	struct BinDesc
	{
		size_t _u, _v, _theta, _phi;
		const Surface* sfc;
	};
	class Lattice
	{
	public:
		struct Partition
		{
			Surface* sfc;
			size_t start;
			size_t numsamples_u, numsamples_v;
			size_t numsamples_theta, numsamples_phi;
			size_t stride; // stride = numsamples_theta * numsamples_phi * sfc_numsamples_u
			size_t length; // length = numsamples_theta * numsamples_phi * sfc_numsamples_u * sfc_numsamples_v
		};
		std::vector<Partition> latticepartitions;
		std::map<const Surface*, size_t> latticepartitionmap;
		std::vector<Entry> entries;
		const bool is2d;

	protected:
		boost::random::mt19937 rand_generator;
		boost::random::uniform_int_distribution<> rand_distribution;
		size_t numentries; // Only valid during initialization (until Allocate() is called)

	public:
		struct ThetaPhiIterator
		{
int numsplits;
			Entry* entry;
			float theta, phi, delta_theta, delta_phi;
#ifdef NEED_PHYSICAL_BIN_CENTER_AND_SIZE_INSIDE_THETA_PHI_ITERATOR
			float u, v, delta_u, delta_v;
#endif
			virtual ~ThetaPhiIterator() {}
		};

		struct RandomEntryConstraints : BinDesc
		{
			//size_t _u, _v, _theta, _phi; // Index [0, numsamples_XYZ]. Set to RANDOM_INDEX to derestrict constraint
			//const Surface* sfc; // Set to RANDOM_SURFACE to derestrict constraint
			float offset_u, offset_v, offset_theta, offset_phi; // Bin center offset [-0.5f, 0.5f]. Set to RANDOM_OFFSET to derestrict constraint
			RandomEntryConstraints(bool is2d) {Reset(is2d);}
			void SetBin(const BinDesc* bin)
			{
				memcpy(this, bin, sizeof(BinDesc));
			}
			void Reset(bool is2d)
			{
				_u = _v = _theta = _phi = RANDOM_INDEX;
				sfc = RANDOM_SURFACE;
				offset_u = offset_phi = RANDOM_OFFSET;
				offset_v = offset_theta = is2d ? 0.0f : RANDOM_OFFSET;
			}
		} random_entry_constraints;

		// Initialization
		Lattice(bool is2d) : numentries(0), is2d(is2d), random_entry_constraints(is2d) {}
		virtual void AddSurface(Surface* sfc, size_t numsamples_u, size_t numsamples_v, size_t numsamples_theta, size_t numsamples_phi);
		virtual Result Allocate(const Scene* scene);

		virtual Entry* GetEntry(const Surface* sfc, float u, float v, float theta, float phi, float* delta_theta = NULL, float* delta_phi = NULL, char* subentry = NULL, int* splitdepth = NULL, float* samplearea = NULL, BinDesc* bin = NULL) = 0;
		virtual Entry* GetRandomEntry(Surface** sfc, float& u, float& v, float& theta, float& phi, float* delta_theta = NULL, float* delta_phi = NULL, char* subentry = NULL, int* splitdepth = NULL, float* samplearea = NULL) = 0;
		virtual ThetaPhiIterator* GetThetaPhiIterator(const Surface* sfc, const Vector2* uv) = 0;
		virtual bool IterateThetaPhi(ThetaPhiIterator* iter) = 0;

		virtual bool AddScaled(Entry* entry, char subentry, int splitdepth, unsigned int splitdims, const Color* newlum, float factor) = 0; // Return value: true ... Entry has been split
	};
	class SingleResolutionLattice : public Lattice
	{
	public://private: //EDIT: Made public for SceneComposer

	public:
		struct ThetaPhiIterator : Lattice::ThetaPhiIterator
		{
			const Partition& partition;
			size_t lattice_uv, _theta, _phi;
			ThetaPhiIterator(const Partition& partition, size_t lattice_uv) : partition(partition), lattice_uv(lattice_uv), _theta((size_t)-1), _phi(0) {}
		};

		// Initialization
		SingleResolutionLattice(bool is2d) : Lattice(is2d) {}
		
		Entry* GetEntry(const Surface* sfc, float u, float v, float theta, float phi, float* delta_theta, float* delta_phi, char* subentry, int* splitdepth, float* samplearea, BinDesc* bin);
		Entry* GetRandomEntry(Surface** sfc, float& u, float& v, float& theta, float& phi, float* delta_theta, float* delta_phi, char* subentry, int* splitdepth, float* samplearea);
		Lattice::ThetaPhiIterator* GetThetaPhiIterator(const Surface* sfc, const Vector2* uv);
		bool IterateThetaPhi(Lattice::ThetaPhiIterator* iter);

		bool AddScaled(Entry* entry, char subentry, int splitdepth, unsigned int splitdims, const Color* newlum, float factor) {entry->AddScaled(newlum, factor); return false;}
	};
	class MultiResolutionLattice : public Lattice
	{
	public:
		struct ThetaPhiIterator : Lattice::ThetaPhiIterator
		{
			const Partition& partition;
			const size_t lattice_uv;
			size_t _theta, _phi;
			
			const float _ufract, _vfract; // Position inside the bin

			struct AlternativePath : Lattice::ThetaPhiIterator
			{
				AlternativePath(const ThetaPhiIterator* iter)
				{
					//memcpy(this, iter, sizeof(Lattice::ThetaPhiIterator));
					this->entry = iter->entry;
					this->theta = iter->theta;
					this->phi = iter->phi;
					this->delta_theta = iter->delta_theta;
					this->delta_phi = iter->delta_phi;
				}
				void Apply(ThetaPhiIterator* iter) const
				{
					//memcpy(iter, this, sizeof(Lattice::ThetaPhiIterator));
					iter->entry = this->entry;
					iter->theta = this->theta;
					iter->phi = this->phi;
					iter->delta_theta = this->delta_theta;
					iter->delta_phi = this->delta_phi;
				}
				virtual ~AlternativePath() {}
			};
			std::stack<AlternativePath> alternativepaths;

			ThetaPhiIterator(float _ufract, float _vfract, const Partition& partition, size_t lattice_uv) : _ufract(_ufract), _vfract(_vfract), partition(partition), lattice_uv(lattice_uv), _theta((size_t)-1), _phi(0) {}
			virtual ~ThetaPhiIterator() {}
		};
		enum SplitDimension
		{
			SD_U = 0x1,
			SD_V = 0x2,
			SD_THETA = 0x4,
			SD_PHI = 0x8,
			FORCE_INT32 = 0xFFFFFFFF // Make sure this enum takes 32bit on every compiler
		};

	private:
		struct SplitEntry
		{
			size_t subentryidx;
			int unused;
			unsigned int splitdims;
			int raycount;
			SplitEntry() : raycount(-1) {}
		};
		static bool IsSplitEntry(const Entry* entry) {return entry->raycount < 0;}
		static bool IsSplit_U(const Entry* entry) {return (((const SplitEntry*)entry)->splitdims & SD_U) != 0;}
		static bool IsSplit_V(const Entry* entry) {return (((const SplitEntry*)entry)->splitdims & SD_V) != 0;}
		static bool IsSplit_Theta(const Entry* entry) {return (((const SplitEntry*)entry)->splitdims & SD_THETA) != 0;}
		static bool IsSplit_Phi(const Entry* entry) {return (((const SplitEntry*)entry)->splitdims & SD_PHI) != 0;}
		void RecursiveSplitPathExplorer(RaySpace::MultiResolutionLattice::ThetaPhiIterator& _iter, int idx, float _ufract, float _vfract);

	public:
		SPLITCONDITION_CALLBACK splitconditioncbk;

		// Initialization
		MultiResolutionLattice(bool is2d) : Lattice(is2d) {}
		
		Entry* GetEntry(const Surface* sfc, float u, float v, float theta, float phi, float* delta_theta, float* delta_phi, char* subentry, int* splitdepth, float* samplearea, BinDesc* bin);
		Entry* GetRandomEntry(Surface** sfc, float& u, float& v, float& theta, float& phi, float* delta_theta, float* delta_phi, char* subentry, int* splitdepth, float* samplearea);
		Lattice::ThetaPhiIterator* GetThetaPhiIterator(const Surface* sfc, const Vector2* uv);
		bool IterateThetaPhi(Lattice::ThetaPhiIterator* iter);

		bool AddScaled(Entry* entry, char subentry, int splitdepth, unsigned int splitdims, const Color* newlum, float factor);
	};

private:
	Scene* scene;
	Lattice* lattice;
	const bool is2d;

public:

	RaySpace(Scene* scene, Lattice* lattice, bool is2d);

	// Initialization
	void AddSurface(Surface* sfc, size_t numsamples_u, size_t numsamples_v, size_t numsamples_theta, size_t numsamples_phi);
	Result Allocate();

	// Helper functions
	static void DirFromLattice(Vector3* dir, float theta, float phi);
	static float GetAngularArea(float theta, float phi, size_t numsamples_theta, size_t numsamples_phi, bool is2d);
	static float GetAngularArea(float theta, float phi, float delta_theta, float delta_phi, bool is2d);
	static float GetBinCenter(float coordinate, float delta);
	static void QuantifyRayDir(const Vector3* raydir, size_t* offset, size_t numsamples_theta, size_t numsamples_phi);

	// Ray casting
	void FullCast_FromPartition(RayCastParams& params, TextureSampler* sampler, const Surface* sfc);
	void SampleLattice(Color* clr, RayCastParams& params, TextureSampler* sampler) const;
	void SampleLatticeMC(Color* clr, RayCastParams& params, TextureSampler* sampler) const;
	bool CastRandomFwd(RayCastParams& params, TextureSampler* sampler, BinDesc* destbin = NULL);
	void CastRandomBwd(RayCastParams& params, TextureSampler* sampler);

	void SampleLattice(int numsamples, const RaySpaceSamplingOptions& options);
	Result Draw2DRaySpace(int resolution_u, int resolution_phi, Image** img, Image** splitimg) const;
	Result DrawIntegratedRaySpace(int resolution_u, int resolution_v, Image** img) const;
	double Compare(IRaySpace* other) const;
	size_t GetLatticeSize() const;
	Result Save(const FilePath& filename) const;
	static Result FromFile(std::ifstream& stream, Scene* scene, const std::list<Surface*> surfaces, RaySpace** rayspace);
	void Release();
};

#endif