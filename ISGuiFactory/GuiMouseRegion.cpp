#include "GuiFactory.h"


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize GuiMouseRegion
//-----------------------------------------------------------------------------
Result GuiMouseRegion::Init()
{
	//Result rlt;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: AddXXX()
// Desc: Add primitive subregion
//-----------------------------------------------------------------------------
void GuiMouseRegion::AddRectangle(const Rect<float>& bounds, DockType dock, DockType refdock)
{
	subregions.push_front(new RectangularSubRegion(bounds, dock, refdock));
}
void GuiMouseRegion::AddEllipse(const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock)
{
	subregions.push_front(new EllipticalSubRegion(center, radius, dock, refdock));
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void GuiMouseRegion::Release()
{

	delete this;
}