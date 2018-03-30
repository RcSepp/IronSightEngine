#ifndef __GUIMOUSEREGION_H
#define __GUIMOUSEREGION_H

class GuiControl;

struct SubRegion
{
	Rect<float> bounds;
	DockType dock, refdock;

	SubRegion(DockType dock, DockType refdock) : dock(dock), refdock(refdock) {}
	virtual bool CheckMousePos(const Point<float>& mousepos) = 0;
};

struct RectangularSubRegion : public SubRegion
{
	RectangularSubRegion(const Rect<float>& bounds, DockType dock, DockType refdock) : SubRegion(dock, refdock)
	{
		this->bounds = Rect<float>(bounds);
	}

	bool CheckMousePos(const Point<float>& mousepos)
	{
		return mousepos.x >= bounds.left() && mousepos.x <= bounds.right() &&
			   mousepos.y >= bounds.top() && mousepos.y <= bounds.bottom();
	}
};

struct EllipticalSubRegion : public SubRegion
{
	EllipticalSubRegion(const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock) : SubRegion(dock, refdock)
	{
		bounds = Rect<float>(center.x, center.y, 2.0f * radius.x, 2.0f * radius.y);
	}

	bool CheckMousePos(const Point<float>& mousepos)
	{
		const Point<float> r(bounds.width / 2.0f, bounds.height / 2.0f);
		Point<float> pos = mousepos - r;
		pos.x /= r.x;
		pos.y /= r.y;
		return pos.x*pos.x + pos.y*pos.y <= 1.0f;
	}
};

//-----------------------------------------------------------------------------
// Name: GuiMouseRegion
// Desc: A collection of primitive subregions that trigger mouse events
//-----------------------------------------------------------------------------
class GuiMouseRegion : public IGuiMouseRegion, public MouseEventReceiver
{
private:

public:

	GuiMouseRegion(GuiControl* parentctrl)
	{
		this->parentctrl = parentctrl;
		downcbk = upcbk = movecbk = entercbk = leavecbk = NULL;
		oldmouseover = mousedowninsideregion = false;
	};

	Result Init();

	void AddRectangle(const Rect<float>& bounds, DockType dock, DockType refdock);
	void AddEllipse(const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock);

	void Release();
};

#endif