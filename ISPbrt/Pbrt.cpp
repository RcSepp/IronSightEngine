#define ISTYPES_NO_POINT
#include "Pbrt.h"

using namespace std;
#define PBRT_PROBES_NONE
//#define PBRT_HAS_OPENEXR
#include <core\geometry.h>
#include <core\api.h>
#include <core\paramset.h>


//-----------------------------------------------------------------------------
// Name: CreatePbrt()
// Desc: DLL API for creating an instance of Pbrt
//-----------------------------------------------------------------------------
PBRT_EXTERN_FUNC LPPBRT __cdecl CreatePbrt()
{
	return new Pbrt();
}

PBRT_EXTERN_FUNC void RedirectStdOut(const String& pipename)
{
	freopen(String("\\\\.\\pipe\\") << pipename, "w", stdout);
	setvbuf(stdout, NULL, _IOLBF, 1024);
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void Pbrt::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: Pbrt()
// Desc: Start PBRT
//-----------------------------------------------------------------------------
Pbrt::Pbrt() : drfilm(NULL)
{
	Options options;
	pbrtInit(options);
}

//-----------------------------------------------------------------------------
// Name: CreateRenderer()
// Desc: Create a PBRT renderer
//-----------------------------------------------------------------------------
Result Pbrt::CreateRenderer(LPPBRTRENDERER* renderer)
{
	Result rlt;

	*renderer = NULL;

	PbrtRenderer* newrenderer;
	CHKALLOC(newrenderer = new PbrtRenderer(this));
	CHKRESULT(newrenderer->Init());

	renderers.push_back(newrenderer);
	*renderer = newrenderer;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: ...
//-----------------------------------------------------------------------------
void Pbrt::Render()
{
	ParamSet params;
	float f[3];
	int i[1];

	pbrtFilm("image", params); params.Clear();
	pbrtLookAt(0.0f, 0.2f, 0.2f, -0.02f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f);
	f[0] = 60.0f; params.AddFloat("fov", f, 1);
	pbrtCamera("perspective", params); params.Clear();

	i[0] = 1; params.AddInt("pixelsamples", i, 1);
	pbrtSampler("lowdiscrepancy", params); params.Clear();
	
	pbrtWorldBegin();

	pbrtAttributeBegin();
	pbrtCoordSysTransform("camera");
	f[0] = 0.7f; f[1] = 0.7f; f[2] = 0.7f; params.AddRGBSpectrum("I", f, 3);
	pbrtLightSource("point", params); params.Clear();
	pbrtAttributeEnd();

	pbrtAttributeBegin();
	f[0] = 15.0f; f[1] = 15.0f; f[2] = 15.0f; params.AddRGBSpectrum("L", f, 3);
	i[0] = 4; params.AddInt("nsamples", i, 1);
	pbrtAreaLightSource("area", params); params.Clear();
	pbrtTranslate(0.0f, 2.0f, 0.0f);
	pbrtRotate(90.0f, 1.0f, 0.0f, 0.0f);
	f[0] = 0.25f; params.AddFloat("radius", f, 1);
	pbrtShape("disk", params); params.Clear();
	pbrtAttributeEnd();

	f[0] = 0.4f; f[1] = 0.42f; f[2] = 0.4f; params.AddRGBSpectrum("Kd", f, 3);
	pbrtMaterial("matte", params); params.Clear();

	Point p[] = {Point(-1.0f, 0.0f, -1.0f), Point(1.0f, 0.0f, -1.0f), Point(1.0f, 0.0f, 1.0f), Point(-1.0f, 0.0f, 1.0f)}; params.AddPoint("P", p, ARRAYSIZE(p));
	int indices[] = {0, 1, 2, 2, 3, 0}; params.AddInt("indices", indices, ARRAYSIZE(indices));
	pbrtShape("trianglemesh", params); params.Clear();

	params.AddString("filename", &std::string("K:\\Third Party Libraries\\pbrt-v2-master\\scenes\\brdfs\\mystique.brdf"), 1);
	pbrtMaterial("measured", params); params.Clear();

	/*pbrtAttributeBegin();
	pbrtTranslate(0.0f, -.033f, 0.0f);
	Point p2[] = {

	};
	params.AddPoint("P", p2, ARRAYSIZE(p2));
	pbrtShape("trianglemesh", params); params.Clear();
	pbrtAttributeEnd();*/

	pbrtWorldEnd();
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Pbrt::Release()
{
	while(renderers.size())
		renderers.front()->Release();

	RemoveDirectRenderView();

	pbrtCleanup();

	delete this;
}