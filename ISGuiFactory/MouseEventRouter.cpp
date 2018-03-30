#include "GuiFactory.h"

bool MouseEventRouter::OnMouseDown(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt)
{
	for(std::list<MouseEventReceiver*>::const_reverse_iterator iter = receivers.rbegin(); iter != receivers.rend(); iter++)
	{
		Point<float> transformedpos(mousepos);
		Rect<float> transformedbounds(bounds);
		bool mouseover = (*iter)->TransformMousePos(transformedpos, transformedbounds);
		if(mouseover)
		{
			(*iter)->mousedowninsideregion = true;
			if((*iter)->OnMouseDown(args, transformedpos, transformedbounds, rlt))
				return true;
		}
	}
	return false;
}
void MouseEventRouter::OnMouseUp(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt)
{
	for(std::list<MouseEventReceiver*>::const_reverse_iterator iter = receivers.rbegin(); iter != receivers.rend(); iter++)
	{
		Point<float> transformedpos(mousepos);
		Rect<float> transformedbounds(bounds);
		bool mouseover = (*iter)->TransformMousePos(transformedpos, transformedbounds);
		if(mouseover)
			(*iter)->OnMouseUp(args, transformedpos, transformedbounds, rlt);
		else if((*iter)->mousedowninsideregion && !args.IsLeftButtonDown())
		{
			(*iter)->OnMouseUp(args, transformedpos, transformedbounds, rlt);
			(*iter)->mousedowninsideregion = false;
		}
	}
}
void MouseEventRouter::OnMouseMove(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt)
{
	for(std::list<MouseEventReceiver*>::const_reverse_iterator iter = receivers.rbegin(); iter != receivers.rend(); iter++)
	{
		Point<float> transformedpos(mousepos);
		Rect<float> transformedbounds(bounds);
		bool mouseover = (*iter)->TransformMousePos(transformedpos, transformedbounds);
		if(mouseover)
			(*iter)->OnMouseMove(args, transformedpos, transformedbounds, rlt);
		else
			if((*iter)->oldmouseover || (*iter)->mousedowninsideregion)
				(*iter)->OnMouseMove(args, transformedpos, transformedbounds, rlt);
		(*iter)->oldmouseover = mouseover;
	}
}
bool MouseEventRouter::OnMouseDoubleClick(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt)
{
	for(std::list<MouseEventReceiver*>::const_reverse_iterator iter = receivers.rbegin(); iter != receivers.rend(); iter++)
	{
		Point<float> transformedpos(mousepos);
		Rect<float> transformedbounds(bounds);
		bool mouseover = (*iter)->TransformMousePos(transformedpos, transformedbounds);
		if(mouseover &&(*iter)->OnMouseDoubleClick(args, transformedpos, transformedbounds, rlt))
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Name: TransformMousePos()
// Desc: Transforms bounds into next layer and returns whether mousepos is still inside. (Default implementation (overrideable))
//-----------------------------------------------------------------------------
bool MouseEventReceiver::TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds)
{
	Rect<float> finalbounds;
	GuiFactory::TransformBounds(dock, refdock, bounds, this->bounds, &bounds);
	return mousepos.x >= bounds.left() && mousepos.x <= bounds.right() && mousepos.y >= bounds.top() && mousepos.y <= bounds.bottom();
}

//-----------------------------------------------------------------------------
// Name: TransformMousePos()
// Desc: Transforms mousepos into previous layer. (Default implementation (overrideable))
//-----------------------------------------------------------------------------
void MouseEventRouter::PointToClient(Point<float>& mousepos, Rect<float>* bounds)
{
	bool lastlayer = !bounds;
	if(lastlayer)
		bounds = new Rect<float>();

	if(router)
	{
		router->PointToClient(mousepos, bounds);
		GuiFactory::TransformBounds(dock, refdock, *bounds, this->bounds, bounds);
	}
	else
	{
		bounds->x = this->bounds.x;
		bounds->y = this->bounds.y;
		bounds->width = this->bounds.width;
		bounds->height = this->bounds.height;
	}

	if(lastlayer)
	{
		//mousepos.x = (mousepos.x - bounds->x) * this->bounds.width / bounds->width + this->bounds.x; // Mathematically correct formula
		//mousepos.y = (mousepos.y - bounds->y) * this->bounds.height / bounds->height + this->bounds.y; // Mathematically correct formula
		mousepos.x = mousepos.x - bounds->x + this->bounds.x; // Size ratio can be neglected, because docking does no resizing
		mousepos.y = mousepos.y - bounds->y + this->bounds.y; // Size ratio can be neglected, because docking does no resizing
		switch(refdock & 0xF)
		{
		case 0x1:
			mousepos.x -= this->bounds.width / 2.0f;
			break;
		case 0x2:
			mousepos.x -= this->bounds.width;
		}
		switch((refdock >> 4) & 0xF)
		{
		case 0x1:
			mousepos.y -= this->bounds.height / 2.0f;
			break;
		case 0x2:
			mousepos.y -= this->bounds.height;
		}
		delete bounds;
	}
}
void MouseEventReceiver::PointToClient(Point<float>& mousepos, Rect<float>* bounds)
{
	Rect<float>* transformedbounds = new Rect<float>(this->bounds);
	router->PointToClient(mousepos, transformedbounds);
	delete transformedbounds;
}