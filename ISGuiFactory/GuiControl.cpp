#include "GuiFactory.h"

void __stdcall Invalidate(LPVOID renderarea)
{
	((RenderArea*)renderarea)->Invalidate();
}

GuiControl::GuiControl(RenderArea* parent, DockType dock, DockType refdock) : parent(parent), MouseEventReceiver(parent), textfield(NULL),
																			  pos(AnimatedPoint<float>(0.0f, 0.0f, Invalidate, parent)), size(AnimatedSize<float>(100.0f, 100.0f, Invalidate, parent)),
																			  overfade(AnimatedValue<float>(0.0f, Invalidate, parent)), downfade(AnimatedValue<float>(0.0f, Invalidate, parent)),
																			  alpha(1.0f, Invalidate, parent), overfadedesc(NULL), downfadedesc(NULL)
{
	this->dock = dock;
	this->refdock = refdock;

	pos.SetVariableChangeHandler(this);
	size.SetVariableChangeHandler(this);
	alpha.SetVariableChangeHandler(this);

	downcbk = upcbk = movecbk = entercbk = leavecbk = NULL;
	oldmouseover = mousedowninsideregion = false;
	img = overimg = downimg = NULL;
	overfade.SetVariableChangeHandler(this);
	downfade.SetVariableChangeHandler(this);
}

bool GuiControl::TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds)
{
	Rect<float> localbounds(this->pos.GetX(), this->pos.GetY(), this->size.GetWidth(), this->size.GetHeight());
	GuiFactory::TransformBounds(dock, refdock, bounds, localbounds, &bounds);

	/*mousepos.x -= this->pos.GetX();
	mousepos.y -= this->pos.GetY();
	return mousepos.x >= 0.0f && mousepos.x <= this->size.GetWidth() && mousepos.y >= 0.0f && mousepos.y <= this->size.GetHeight();*/
	return mousepos.x >= bounds.left() && mousepos.x <= bounds.right() && mousepos.y >= bounds.top() && mousepos.y <= bounds.bottom();
}
void GuiControl::PointToClient(Point<float>& mousepos, Rect<float>* bounds)
{
	Rect<float> localbounds(this->pos.GetX(), this->pos.GetY(), this->size.GetWidth(), this->size.GetHeight());

	bool lastlayer = !bounds;
	if(lastlayer)
		bounds = new Rect<float>();

	if(router)
	{
		router->PointToClient(mousepos, bounds);
		GuiFactory::TransformBounds(dock, refdock, *bounds, localbounds, bounds);
	}
	else
	{
		bounds->x = localbounds.x;
		bounds->y = localbounds.y;
		bounds->width = localbounds.width;
		bounds->height = localbounds.height;
	}

	if(lastlayer)
	{
		//mousepos.x = (mousepos.x - bounds->x) * localbounds.width / bounds->width + localbounds.x; // Mathematically correct formula
		//mousepos.y = (mousepos.y - bounds->y) * localbounds.height / bounds->height + localbounds.y; // Mathematically correct formula
		mousepos.x = mousepos.x - bounds->x + localbounds.x; // Size ratio can be neglected, because docking does no resizing
		mousepos.y = mousepos.y - bounds->y + localbounds.y; // Size ratio can be neglected, because docking does no resizing
		switch(refdock & 0xF)
		{
		case 0x1:
			mousepos.x -= localbounds.width / 2.0f;
			break;
		case 0x2:
			mousepos.x -= localbounds.width;
		}
		switch((refdock >> 4) & 0xF)
		{
		case 0x1:
			mousepos.y -= localbounds.height / 2.0f;
			break;
		case 0x2:
			mousepos.y -= localbounds.height;
		}
		delete bounds;
	}
}

//-----------------------------------------------------------------------------
// Name: Paint()
// Desc: Call Paint() for all contained shapes
//-----------------------------------------------------------------------------
void GuiControl::Paint(RenderTarget *rendertarget, const Rect<float>& areabounds) const
{
	Rect<float> localbounds(this->pos.GetX(), this->pos.GetY(), this->size.GetWidth(), this->size.GetHeight());

	/*D2D1_POINT_2F transformedpoint = rendertarget->transform.TransformPoint(D2D1::Point2F(localbounds.x, localbounds.y));
	localbounds.x = transformedpoint.x;
	localbounds.y = transformedpoint.y;
	transformedpoint = rendertarget->transform.TransformPoint(D2D1::Point2F(localbounds.right(), localbounds.bottom()));
	localbounds.width = transformedpoint.x - localbounds.x;
	localbounds.height = transformedpoint.y - localbounds.y;

	transformedpoint = rendertarget->transform.TransformPoint(D2D1::Point2F(areabounds.x, areabounds.y));
	Rect<float> newareabounds;
	newareabounds.x = transformedpoint.x;
	newareabounds.y = transformedpoint.y;
	transformedpoint = rendertarget->transform.TransformPoint(D2D1::Point2F(areabounds.right(), areabounds.bottom()));
	newareabounds.width = transformedpoint.x - newareabounds.x;
	newareabounds.height = transformedpoint.y - newareabounds.y;*/

	Rect<float> finalbounds;
	GuiFactory::TransformBounds(dock, refdock, areabounds, localbounds, &finalbounds);

	D2D1::Matrix3x2F matTrans = D2D1::Matrix3x2F::Translation(finalbounds.x, finalbounds.y);
	rendertarget->context->SetTransform(matTrans * rendertarget->transform);

	// Apply alpha fading
	float effectfade = downfade.GetVal();
	GuiImage* effectimg;
	GuiBrush* effectbrush;
	GuiImage* _img = img;
	GuiBrush* _brush = brush;
	if(!effectfade)
	{
		effectimg = overimg;
		effectbrush = overbrush;
		effectfade = overfade.GetVal();
	}
	else
	{
		effectimg = downimg;
		effectbrush = downbrush;
		_img = overimg;
		_brush = overbrush;
	}

	if(effectimg)
		rendertarget->context->DrawBitmap(effectimg->Paint(rendertarget), D2D1::RectF(0.0f, 0.0f, finalbounds.width, finalbounds.height), effectfade * alpha.GetVal());
	if(_img)
		rendertarget->context->DrawBitmap(_img->Paint(rendertarget), D2D1::RectF(0.0f, 0.0f, finalbounds.width, finalbounds.height), (1.0f - effectfade) * alpha.GetVal());

	// textfield->Paint() alters rendertarget transform internally
	if(textfield && effectbrush)
		textfield->Paint(rendertarget, effectbrush, effectfade, finalbounds);
	if(textfield && _brush)
		textfield->Paint(rendertarget, _brush, (1.0f - effectfade), finalbounds);
}

void GuiControl::CreateTextField(IGuiTextFormat* format, IGuiBrush* brush, const Rect<float>& bounds, DockType dock, DockType refdock)
{
	textfield = new TextField(format, bounds, dock, refdock);
	this->brush = (GuiBrush*)brush;
}

void GuiControl::BringToFront()
{
	// Let this control be drawn last
	parent->ctrls.remove(this);
	parent->ctrls.push_back(this);

	// Let this control be checked for mouse events first
	parent->receivers.remove(this);
	parent->receivers.push_front(this);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void GuiControl::Release()
{
	parent->ctrls.remove(this);
	parent->receivers.remove(this);

	pos.CancelPendingAnimations();
	size.CancelPendingAnimations();
//shapes.clear();

	if(textfield)
		delete textfield;

	if(overfadedesc)
		delete overfadedesc;
	if(downfadedesc)
		delete downfadedesc;

	delete this;
}


/*bool Shape::TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds)
{
	Rect<float> finalbounds;
	GuiFactory::TransformBounds(dock, refdock, bounds, this->bounds, &finalbounds);

	//mousepos.x -= bounds.x;
	//mousepos.y -= bounds.y;
	return true;
}*/

//-----------------------------------------------------------------------------
// Name: OnMouseXXX()
// Desc: Call appropriate events
//-----------------------------------------------------------------------------
bool GuiControl::OnMouseDown(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt)
{
	bool mouseover = (img && img->CheckMousePos(mousepos, bounds)) || (textfield && textfield->CheckMousePos(mousepos, bounds));
	if(mouseover)
	{
		mousedowninsideregion = true;
		if(downfadedesc && downfadedesc->type != TransitionScript::T_Unset)
		{
			GuiAnimation* ani;
			gui->CreateAnimation((IGuiAnimation**)&ani);
			downfadedesc->dest = 1.0;
			ani->AddFloatChange(downfade, *downfadedesc);
			ani->Schedule();
			parent->Invalidate();
		}
		if(downcbk)
		{
			*rlt = new MouseEventResult(downcbk, this, downcbkuser);
			return true;
		}
	}

	return false;
}
void GuiControl::OnMouseUp(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt)
{
	bool mouseover = (img && img->CheckMousePos(mousepos, bounds)) || (textfield && textfield->CheckMousePos(mousepos, bounds));
	if(mouseover || (mousedowninsideregion && !args.IsLeftButtonDown()))
	{
		if(downfadedesc && downfadedesc->type != TransitionScript::T_Unset)
		{
			GuiAnimation* ani;
			gui->CreateAnimation((IGuiAnimation**)&ani);
			downfadedesc->dest = 0.0;
			ani->AddFloatChange(downfade, *downfadedesc);
			ani->Schedule();
			parent->Invalidate();
		}
		if(upcbk)
			*rlt = new MouseEventResult(upcbk, this, upcbkuser);
	}
	if(!args.IsLeftButtonDown())
		mousedowninsideregion = false;

	//return mouseover;
}
void GuiControl::OnMouseMove(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt)
{
	bool mouseover = (img && img->CheckMousePos(mousepos, bounds)) || (textfield && textfield->CheckMousePos(mousepos, bounds));
	if(mouseover)
	{
		if(!oldmouseover)
		{
			if(overfadedesc && overfadedesc->type != TransitionScript::T_Unset)
			{
				GuiAnimation* ani;
				gui->CreateAnimation((IGuiAnimation**)&ani);
				overfadedesc->dest = 1.0;
				ani->AddFloatChange(overfade, *overfadedesc);
				ani->Schedule();
				parent->Invalidate();
			}
			if(entercbk)
				*rlt = new MouseEventResult(entercbk, this, entercbkuser);
		}
		if(movecbk)
			*rlt = new MouseEventResult(movecbk, this, movecbkuser);
	}
	else
	{
		if(oldmouseover)
		{
			if(overfadedesc && overfadedesc->type != TransitionScript::T_Unset)
			{
				GuiAnimation* ani;
				gui->CreateAnimation((IGuiAnimation**)&ani);
				overfadedesc->dest = 0.0;
				ani->AddFloatChange(overfade, *overfadedesc);
				ani->Schedule();
				parent->Invalidate();
			}
			if(leavecbk)
				*rlt = new MouseEventResult(leavecbk, this, leavecbkuser);
		}
		if(movecbk && mousedowninsideregion)
			*rlt = new MouseEventResult(movecbk, this, movecbkuser);
	}
	oldmouseover = mouseover;

	//return mouseover;
}
bool GuiControl::OnMouseDoubleClick(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt)
{
	bool mouseover = (img && img->CheckMousePos(mousepos, bounds)) || (textfield && textfield->CheckMousePos(mousepos, bounds));
//EDIT

	return mouseover;
}


GuiControl::TextField::TextField(IGuiTextFormat* format, const Rect<float>& bounds, DockType dock, DockType refdock)
{
	this->dock = dock;
	this->refdock = refdock;
	this->bounds = Rect<float>(bounds);
	this->format = (GuiTextFormat*)format;
	layout = NULL;
	/*cachedtextbounds = NULL;*/
}

void GuiControl::TextField::Paint(RenderTarget *rendertarget, GuiBrush* brush, float opacity, const Rect<float>& areabounds) const
{
	if(!layout || opacity == 0.0f)
		return;

	Rect<float> finalbounds;
	GuiFactory::TransformBounds(dock, refdock, areabounds, bounds, &finalbounds);

	D2D1::Matrix3x2F matTrans = D2D1::Matrix3x2F::Translation(finalbounds.x, finalbounds.y);
	rendertarget->context->SetTransform(matTrans * rendertarget->transform);

	rendertarget->context->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), layout, brush->GetD2DBrush(opacity, rendertarget));
}

Result GuiControl::TextField::SetText(const String& text)
{
	/*if(cachedtextbounds)
		delete cachedtextbounds; // Invalidate old text bounds*/
	if(this->text == text)
		return R_OK; // layout unchanged
	this->text = text;
	RELEASE(layout);
	if(text.IsEmpty())
		return R_OK; // layout == NULL
	return Error(dwtfactory->CreateTextLayout(this->text.ToWCharString(), this->text.length(), *format, bounds.width, bounds.height, &layout)); // new layout
}
Result GuiControl::TextField::MeasureText(Rect<float>* bounds)
{
	/*if(cachedtextbounds)
	{
		bounds->x = cachedtextbounds->x;
		bounds->y = cachedtextbounds->y;
		bounds->width = cachedtextbounds->width;
		bounds->height = cachedtextbounds->height;
		return R_OK;
	}*/

	if(!layout)
		return ERR("MeasureText() called on a textfield without text.");

	DWRITE_TEXT_METRICS metrics;
	Result rlt;

	CHKRESULT(layout->GetMetrics(&metrics));
	bounds->x = metrics.left + this->bounds.x;
	bounds->y = metrics.top + this->bounds.y;
	bounds->width = metrics.width;
	bounds->height = metrics.height;
	/*cachedtextbounds = new Rect<float>(*bounds);*/

	return R_OK;
}
Result GuiControl::FitTextSizeToText()
{
	if(!textfield)
		return ERR("MeasureText() called on a control without any text fields.");

	Rect<float> bounds;
	Result rlt;
	CHKRESULT(textfield->MeasureText(&bounds));
	textfield->SetSize(bounds.size);
	return R_OK;
}
const ::Size<float>& GuiControl::TextField::SetSize(const ::Size<float>& val)
{
	if(layout)
	{
		layout->SetMaxWidth(val.width);
		layout->SetMaxHeight(val.height);
	}
	return bounds.size = val;
}
float GuiControl::TextField::SetWidth(float val)
{
	if(layout)
		layout->SetMaxWidth(val);
	return bounds.width = val;
}
float GuiControl::TextField::SetHeight(float val)
{
	if(layout)
		layout->SetMaxHeight(val);
	return bounds.height = val;
}

/*void TextField::Release()
{
	if(cachedtextbounds)
		delete cachedtextbounds;
}*/

TransitionScript::IScript* TransitionScript::IScript::Copy(const IScript& other)
{
	switch(other.type)
	{
	case T_Instantaneous: return new Instantaneous(other.dest);
	case T_Linear: return new Linear(other.dest, ((Linear*)&other)->duration);
	case T_AccelerateDecelerate: return new AccelerateDecelerate(other.dest, ((AccelerateDecelerate*)&other)->duration, ((AccelerateDecelerate*)&other)->accratio, ((AccelerateDecelerate*)&other)->decratio);
	}
}