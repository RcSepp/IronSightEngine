#define ISTYPES_NO_POINT
#include "Pbrt.h"

using namespace std;
#define PBRT_PROBES_NONE
//#define PBRT_HAS_OPENEXR
#include <core\renderer.h>
#include <renderers\metropolis.h>
#include <samplers\lowdiscrepancy.h>
/*#include <core\geometry.h>
#include <core\api.h>
#include <core\paramset.h>*/

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize PbrtRenderer
//-----------------------------------------------------------------------------
Result PbrtRenderer::Init()
{
	//renderOptions->MakeRenderer();
	//CreateLowDiscrepancySampler(
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void PbrtRenderer::Release()
{
	parent->renderers.remove(this);

	delete this;
}