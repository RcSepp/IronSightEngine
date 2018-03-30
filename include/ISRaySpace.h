#ifndef __ISRAYSPACE_H
#define	__ISRAYSPACE_H

#include <ISEngine.h>
#include <ISMath.h>

struct Image;

//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
typedef bool (*SPLITCONDITION_CALLBACK)(const Color3* oldlum, const Color3* newlum, int oldnumrays, int splitdepth);
template<int THRESHOLD_DENOM, int MINRAYCOUNT, int MAXDEPTH> bool DefaultSplitCondition(const Color3* oldlum, const Color3* newlum, int oldnumrays, int splitdepth)
{
	if(splitdepth >= MAXDEPTH || oldnumrays < MINRAYCOUNT)
		return false;

	auto Luma = [](const Color3* clr) -> float {return 0.3f * clr->r + 0.59f * clr->g + 0.11f * clr->b;};
	float oldluma = Luma(oldlum);
	float newluma = Luma(newlum);

	return abs(oldluma - newluma) >= 1.0f / (float)THRESHOLD_DENOM;
}

struct RaySpaceSamplingOptions
{
	bool castbackwards, bfstracing;
	bool quietmodeenabled;
	enum TextureSampling
	{
		TS_POINT, TS_LINEAR
	} texsampling;
	enum RandomEntryConstraints
	{
		REC_NONE = 0x0,
		REC_EMMISSIVE_SFC = 0x1
	} constraints;
	SPLITCONDITION_CALLBACK splitconditioncbk;

	//EDIT: Add randomness-restrictions here
	volatile bool keepalive; // Boolean value that can be switched to false from another thread to request algorithm termination

	RaySpaceSamplingOptions() : quietmodeenabled(false), bfstracing(false), castbackwards(false), texsampling(TS_LINEAR), constraints(REC_NONE), splitconditioncbk(DefaultSplitCondition<100, 10, 20>), keepalive(true) {}
};

namespace ISRayTracer
{
	//-----------------------------------------------------------------------------
	// Name: IRaySpace
	// Desc: Interface to the RaySpace class
	//-----------------------------------------------------------------------------
	typedef class IRaySpace
	{
	public:
		virtual void SampleLattice(int numsamples, const RaySpaceSamplingOptions& options) = 0;
		virtual Result Draw2DRaySpace(int resolution_u, int resolution_phi, Image** img, Image** splitimg = NULL) const = 0;
		virtual Result DrawIntegratedRaySpace(int resolution_u, int resolution_v, Image** img) const = 0;
		virtual double Compare(IRaySpace* other) const = 0;
		virtual size_t GetLatticeSize() const = 0;
		virtual Result Save(const FilePath& filename) const = 0;
		virtual void Release() = 0;
	}* LPRAYSPACE;
}

#endif