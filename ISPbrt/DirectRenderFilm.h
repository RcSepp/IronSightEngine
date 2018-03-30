#define ISTYPES_NO_POINT
#include "Pbrt.h"

namespace D3D
{
	#undef ISTYPES_NO_POINT
	#undef __ISTYPES_H
	#define PARENT_NAMESPACE D3D
	class IOutputWindow;
	#include <ISDirect3D.h>
}
class DRFilm;

//-----------------------------------------------------------------------------
// Name: DirectRenderFilm
// Desc: A render target for the direct-render film
//-----------------------------------------------------------------------------
class DirectRenderFilm : public D3D::IUpdateable
{
private:
	D3D::IOutputWindow* d3dwnd;
	D3D::IHUD* d3dhud;
	D3D::ITexture* d3dtex;

	struct Options
	{
		float invgamma;
	} options;

public:
	DRFilm* film;
	bool unhookfilm;
	DirectRenderFilm(D3D::IOutputWindow* d3dwnd) : d3dwnd(d3dwnd), d3dtex(NULL), d3dhud(NULL), film(NULL), unhookfilm(false) {}

	static void Options_OutputSizeChanged(int width, int height, LPVOID user);

	Result Init();
	void Update();
	void Render(D3D::RenderType rendertype);
	void Release();
};