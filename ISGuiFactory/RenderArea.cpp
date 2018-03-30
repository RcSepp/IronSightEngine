#include "GuiFactory.h"

/*bool RenderArea::TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds)
{
	RECT rect;
	if(!GetClientRect(gui->GetTargetWnd(), &rect))
		return false;

	bounds.x = 0.0f;
	bounds.y = 0.0f;
	bounds.width = (float)(rect.right - rect.left);
	bounds.height = (float)(rect.bottom - rect.top);
	return true;
}

bool ClippedRenderArea::TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds)
{
	//mousepos.x -= bounds.x;
	//mousepos.y -= bounds.y;
	return mousepos.x >= 0.0f && mousepos.x <= bounds.width && mousepos.y >= 0.0f && mousepos.y <= bounds.height;
}*/

//-----------------------------------------------------------------------------
// Name: UpdateContentSpan()
// Desc: Update total span of content based on the bounds of a single control or primitive. (This should be called on add or expansion of a control or primitive)
//-----------------------------------------------------------------------------
void RenderArea::UpdateContentSpan(const Rect<float>& bounds)
{
	contentspan.width = max(contentspan.width, bounds.right());
	contentspan.height = max(contentspan.height, bounds.bottom());

	parent->OnContentSpanIncrease(contentspan);
}

//-----------------------------------------------------------------------------
// Name: UpdateContentSpan()
// Desc: Update total span of content based on the bounds of all currently present controls and primitives. (This should be called on removal or contraction of a control or primitive)
//-----------------------------------------------------------------------------
void RenderArea::UpdateContentSpan()
{
	::Size<float> oldspan = contentspan;
	contentspan = ::Size<float>(0.0f, 0.0f);
	std::list<GuiControl*>::const_iterator citer;
	LIST_FOREACH(ctrls, citer)
	{
		contentspan.width = max(contentspan.width, (*citer)->bounds.right());
		contentspan.height = max(contentspan.height, (*citer)->bounds.bottom());
	}
	std::list<Shape*>::const_iterator piter;
	LIST_FOREACH(primitives, piter)
	{
		contentspan.width = max(contentspan.width, (*piter)->bounds.right());
		contentspan.height = max(contentspan.height, (*piter)->bounds.bottom());
	}

	if(oldspan.width != contentspan.width || oldspan.height != contentspan.height) parent->OnContentSpanDecrease(oldspan);
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Clip render target and draw controls
//-----------------------------------------------------------------------------
void RenderArea::Render(RenderTarget *rendertarget)
{
	invalidated = false;

	D2D1_SIZE_U rendertargetsize = rendertarget->context->GetPixelSize();
	std::list<GuiControl*>::iterator iter;
	LIST_FOREACH(ctrls, iter)
		(*iter)->Paint(rendertarget, Rect<float>(0.0f, 0.0f, (float)rendertargetsize.width, (float)rendertargetsize.height));

	std::list<Shape*>::const_iterator piter;
	LIST_FOREACH(primitives, piter)
		(*piter)->Paint(rendertarget, bounds, 1.0f);
}
void ClippedRenderArea::Render(RenderTarget *rendertarget)
{
	invalidated = false;

	rendertarget->context->SetTransform(D2D1::Matrix3x2F::Identity());
	if(bounds)
		rendertarget->context->PushAxisAlignedClip(D2D1::RectF(bounds.left(), bounds.top(), bounds.right(), bounds.bottom()), aliasmode);

	std::list<GuiControl*>::iterator iter;
	LIST_FOREACH(ctrls, iter)
		(*iter)->Paint(rendertarget, bounds);

	std::list<Shape*>::const_iterator piter;
	LIST_FOREACH(primitives, piter)
		(*piter)->Paint(rendertarget, bounds, 1.0f);

	if(bounds)
		rendertarget->context->PopAxisAlignedClip();
}

void RenderArea::Invalidate()
{
	if(!invalidated)
	{
		// EDIT: Invalidate only render area size rect
		InvalidateRect(parent->targetwnd, NULL, FALSE);
		invalidated = true;
	}
}

Result RenderArea::CreateControl(DockType dock, DockType refdock, IGuiControl** ctrl)
{
	Result rlt;

	*ctrl = NULL;

	GuiControl* newctrl;
	CHKALLOC(newctrl = new GuiControl(this, dock, refdock));

	UpdateContentSpan(newctrl->bounds);
	ctrls.push_back(newctrl); receivers.push_front(newctrl);
	*ctrl = newctrl;

	return R_OK;
}

void RenderArea::RemoveControl(IGuiControl* ctrl)
{
	GuiControl* basectrl = reinterpret_cast<GuiControl*>(ctrl);
	if(basectrl)
	{
		ctrls.remove(basectrl); receivers.remove(basectrl);
		basectrl->Release();
	}
	UpdateContentSpan();
}

void RenderArea::RemoveAllControls()
{
	while(ctrls.size())
		ctrls.front()->Release();
	UpdateContentSpan();
}

//-----------------------------------------------------------------------------
// Name: DrawXXX()
// Desc: Add XXX primitive to be drawn on repaint
//-----------------------------------------------------------------------------
void RenderArea::FillRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius, DockType dock, DockType refdockDraw)
{
	FilledRectangle* newshape;
	newshape = new FilledRectangle(NULL, bounds, radius, brush, dock, refdock);

	UpdateContentSpan(newshape->bounds);
	primitives.push_back(newshape);
	Invalidate();
}
void RenderArea::DrawRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius, DockType dock, DockType refdockDraw)
{
	DrawnRectangle* newshape;
	newshape = new DrawnRectangle(NULL, bounds, radius, brush, dock, refdock);

	UpdateContentSpan(newshape->bounds);
	primitives.push_back(newshape);
	Invalidate();
}
void RenderArea::FillEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdockDraw)
{
	FilledEllipse* newshape;
	newshape = new FilledEllipse(NULL, center, radius, brush, dock, refdock);

	UpdateContentSpan(newshape->bounds);
	primitives.push_back(newshape);
	Invalidate();
}
void RenderArea::DrawEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdockDraw)
{
	DrawnEllipse* newshape;
	newshape = new DrawnEllipse(NULL, center, radius, brush, dock, refdock);

	UpdateContentSpan(newshape->bounds);
	primitives.push_back(newshape);
	Invalidate();
}
void RenderArea::DrawPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed, float strokewidth, DockType dock, DockType refdockDraw)
{
	DrawnPolygon* newshape;
	newshape = new DrawnPolygon(NULL, vertices, numvertices, isclosed, strokewidth, brush, dock, refdock);

	UpdateContentSpan(newshape->bounds);
	primitives.push_back(newshape);
	Invalidate();
}
void RenderArea::FillPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed, DockType dock, DockType refdockDraw)
{
	FilledPolygon* newshape;
	newshape = new FilledPolygon(NULL, vertices, numvertices, isclosed, brush, dock, refdock);

	UpdateContentSpan(newshape->bounds);
	primitives.push_back(newshape);
	Invalidate();
}
void RenderArea::DrawLine(IGuiBrush* brush, const Point<float>& p0, const Point<float>& p1, float strokewidth, DockType dock, DockType refdockDraw)
{
	Line* newshape;
	newshape = new Line(NULL, p0, p1, strokewidth, brush, dock, refdock);

	UpdateContentSpan(newshape->bounds);
	primitives.push_back(newshape);
	Invalidate();
}
void RenderArea::DrawBitmap(IGuiBitmap* bmp, const Rect<float>& bounds, DockType dock, DockType refdockDraw)
{
	Bitmap* newshape;
	newshape = new Bitmap(NULL, bmp, bounds, dock, refdock);

	UpdateContentSpan(newshape->bounds);
	primitives.push_back(newshape);
	Invalidate();
}
void RenderArea::DrawTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Rect<float>& bounds, DockType dock, DockType refdockDraw, IGuiShape** primitive)
{
	TextField* newshape; Result rlt;
	newshape = new TextField(format, bounds, brush, dock, refdock);
	IFFAILED(newshape->SetText(text))
	{
		LOG(rlt.GetLastResult());
		return;
	}
	newshape->SetTextAllignment(GTA_LEADING);

	UpdateContentSpan(newshape->bounds);
	primitives.push_back(newshape);
	Invalidate();
	if(primitive)
		*primitive = newshape;
}

void RenderArea::ClearPrimitives()
{
	std::list<Shape*>::iterator iter;
	LIST_FOREACH(primitives, iter)
		(*iter)->Release();
	primitives.clear();
	UpdateContentSpan();
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void RenderArea::Release()
{
	parent->renderareas.remove(this);

	RemoveAllControls();
	ClearPrimitives();

	delete this;
}
void ClippedRenderArea::Release()
{
	parent->renderareas.remove(this);

	Unregister();
	RemoveAllControls();
	ClearPrimitives();

	delete this;
}