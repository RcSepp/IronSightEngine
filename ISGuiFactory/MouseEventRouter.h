#ifndef __MOUSEEVENTROUTER_H
#define __MOUSEEVENTROUTER_H


class MouseEventRouter;


//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct MouseEventResult
{
	MOUSE_CALLBACK cbk;
	LPGUICONTROL ctrl;
	LPVOID user;
	MouseEventResult(MOUSE_CALLBACK cbk, LPGUICONTROL ctrl, LPVOID user) : cbk(cbk), ctrl(ctrl), user(user) {}
};


//-----------------------------------------------------------------------------
// Name: MouseEventRouter
// Desc: Routs mouse events to the next MouseEventReceiver
//-----------------------------------------------------------------------------
class MouseEventReceiver
{
public:
	bool oldmouseover, mousedowninsideregion;
	DockType dock, refdock;
	Rect<float> bounds;
	MouseEventRouter* router;

	MouseEventReceiver(MouseEventRouter* router) : router(router)
	{
		oldmouseover = mousedowninsideregion = false;
	}
	MouseEventReceiver() : router(NULL)
	{
		oldmouseover = mousedowninsideregion = false;
	}

	virtual void PointToClient(Point<float>& mousepos, Rect<float>* bounds = NULL);
	virtual bool TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds);
	virtual bool OnMouseDown(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt) = 0;
	virtual void OnMouseUp(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt) = 0;
	virtual void OnMouseMove(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt) = 0;
	virtual bool OnMouseDoubleClick(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt) = 0;
};

//-----------------------------------------------------------------------------
// Name: MouseEventRouter
// Desc: Routs mouse events to the next MouseEventReceiver
//-----------------------------------------------------------------------------
class MouseEventRouter : public MouseEventReceiver
{
public:
	std::list<MouseEventReceiver*> receivers;

	MouseEventRouter(MouseEventRouter* router) : MouseEventReceiver(router) {}
	MouseEventRouter() {}

	virtual void PointToClient(Point<float>& mousepos, Rect<float>* bounds = NULL);
	virtual bool OnMouseDown(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt);
	virtual void OnMouseUp(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt);
	virtual void OnMouseMove(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt);
	virtual bool OnMouseDoubleClick(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt);
};

#endif