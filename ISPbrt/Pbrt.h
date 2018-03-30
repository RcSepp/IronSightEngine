#ifndef __PBRT_H
#define __PBRT_H

#include "ISPbrt.h"

class Pbrt;
class Filter; // Filter class from PBRT
class Film; // Film class from PBRT
class Camera; // Camera class from PBRT
class Sampler; // Sampler class from PBRT
class SurfaceIntegrator; // SurfaceIntegrator class from PBRT
class VolumeIntegrator; // VolumeIntegrator class from PBRT
class Renderer; // Renderer class from PBRT

class DirectRenderFilm;

Filter* MakeFilter(const FilterDesc* desc);
Film* MakeFilm(const FilmDesc* desc, Filter* filter);
Camera* MakeCamera(const CameraDesc* desc, const Transform* tstart, const Transform* tend, Film *film);
Sampler* MakeSampler(const SamplerDesc* desc, const Camera* camera);
SurfaceIntegrator* MakeSurfaceIntegrator(const SurfaceIntegratorDesc* desc);
VolumeIntegrator* MakeVolumeIntegrator(const VolumeIntegratorDesc* desc);
Renderer* MakeRenderer(const RendererDesc* desc, Camera* camera);

//-----------------------------------------------------------------------------
// Name: PbrtRenderer
// Desc: Wrapper class for an instance of a PBRT renderer
//-----------------------------------------------------------------------------
class PbrtRenderer : public IPbrtRenderer
{
private:
	Pbrt* parent;
	Renderer* renderer;

public:
	PbrtRenderer(Pbrt* parent) : parent(parent), renderer(NULL) {}
	Result Init();
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Pbrt
// Desc: ?
//-----------------------------------------------------------------------------
class Pbrt : public IPbrt
{
private:
	Film* MakeFilm(const ImageFilmDesc* desc, Filter* filter);

public:
	std::list<PbrtRenderer*> renderers;
	DirectRenderFilm* drfilm;

	Pbrt();

	void UnhookFilm();

	void Sync(GLOBALVARDEF_LIST);
	Result CreateRenderer(LPPBRTRENDERER* renderer);
	Result CreateDirectRenderView(IOutputWindow* wnd);
	void RemoveDirectRenderView();
	void Render();
	Result Render(IOutputWindow* wnd, const FilterDesc* filter, const RendererDesc* renderer);
	void Release();
};

#endif