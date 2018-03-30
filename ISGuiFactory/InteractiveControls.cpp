#include "GuiFactory.h"

#define SLIDER_LONG_DISTANCE_SNAPBACK		145
#define SLIDER_SHORT_DISTANCE_SNAPBACK		68


static bool DoesHorizontalSliderSnapBack(const Point<float>& pos, float xstart, float xend, float y)
{
	if(pos.x < xstart)
		return xstart - pos.x > SLIDER_SHORT_DISTANCE_SNAPBACK;
	else if(pos.x > xend)
		return pos.x - xend > SLIDER_SHORT_DISTANCE_SNAPBACK;
	else
		return fabs(pos.y - y) > SLIDER_LONG_DISTANCE_SNAPBACK;
}
static bool DoesVerticalSliderSnapBack(const Point<float>& pos, float x, float ystart, float yend)
{
	if(pos.y < ystart)
		return ystart - pos.y > SLIDER_SHORT_DISTANCE_SNAPBACK;
	else if(pos.y > yend)
		return pos.y - yend > SLIDER_SHORT_DISTANCE_SNAPBACK;
	else
		return fabs(pos.x - x) > SLIDER_LONG_DISTANCE_SNAPBACK;
}


void __stdcall SliderControl::OnSliderDown(IGuiControl* sender, const MouseEventArgs& args, LPVOID user)
{
	SliderControl* slider = (SliderControl*)user;
	slider->ldowninsideregion = true;

	// Transform mousepos into bgctrl space
	Point<float> mousepos((float)args.mousepos.x, (float)args.mousepos.y);
	slider->bgctrl->PointToClient(mousepos);

	if(slider->orientation == OT_HORIZONTAL)
	{
		slider->SetPos(mousepos.x);
		slider->dragstartpos = sender->Left;
	}
	else // slider->orientation == OT_VERTICAL
	{
		slider->SetPos(mousepos.y);
		slider->dragstartpos = sender->Top;
	}
}

void __stdcall SliderControl::OnSliderUp(IGuiControl* sender, const MouseEventArgs& args, LPVOID user)
{
	SliderControl* slider = (SliderControl*)user;
	slider->ldowninsideregion = false;

	// Throw event
	if(slider->vchangecbk && slider->pos != slider->dragstartpos)
		slider->vchangecbk(slider, slider->value, slider->vchangecbkuser);
}

void __stdcall SliderControl::OnSliderMove(IGuiControl* sender, const MouseEventArgs& args, LPVOID user)
{
	SliderControl* slider = (SliderControl*)user;
	if(slider->ldowninsideregion)
	{
		// Transform mousepos into bgctrl space
		Point<float> mousepos((float)args.mousepos.x, (float)args.mousepos.y);
		slider->bgctrl->PointToClient(mousepos);

		if(slider->orientation == OT_HORIZONTAL)
		{
			if(slider->enablesnapback && DoesHorizontalSliderSnapBack(mousepos, slider->startpos,  slider->endpos,
																	  sender->GetTop() + sender->GetHeight() / 2.0f))
				slider->SetPos(slider->dragstartpos);
			else
				slider->SetPos(mousepos.x);
		}
		else // slider->orientation == OT_VERTICAL
		{
			if(slider->enablesnapback && DoesVerticalSliderSnapBack(mousepos, sender->GetLeft() + sender->GetWidth() / 2.0f,
																	slider->startpos, slider->endpos))
				slider->SetPos(slider->dragstartpos);
			else
				slider->SetPos(mousepos.y);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize SliderControl
//-----------------------------------------------------------------------------
Result SliderControl::Init()
{
	sliderctrl->SetMouseDownCallback(&OnSliderDown, this);
	sliderctrl->SetMouseUpCallback(&OnSliderUp, this);
	sliderctrl->SetMouseMoveCallback(&OnSliderMove, this);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: UpdateValue()
// Desc: Update value variable and throw event if it has changed
//-----------------------------------------------------------------------------
void SliderControl::SetPos(float newpos)
{
	// Clamp value to borders
	pos = min(endpos, max(startpos, newpos));

	// Update slider position visually
	if(orientation == OT_HORIZONTAL)
		switch(sliderctrl->refdock & 0xF)
		{
		case 0x0:
			sliderctrl->Left = pos - sliderctrl->size.GetWidth() / 2.0f; // Compensate refpos (always snap cursor to center)
			break;
		case 0x1:
			sliderctrl->Left = pos;
			break;
		case 0x2:
			sliderctrl->Left = pos + sliderctrl->size.GetWidth() / 2.0f; // Compensate refpos (always snap cursor to center)
		}
	else // orientation == OT_VERTICAL
		switch((sliderctrl->refdock >> 4) & 0xF)
		{
		case 0x0:
			sliderctrl->Top = pos - sliderctrl->size.GetHeight() / 2.0f; // Compensate refpos (always snap cursor to center)
			break;
		case 0x1:
			sliderctrl->Top = pos;
			break;
		case 0x2:
			sliderctrl->Top = pos + sliderctrl->size.GetHeight() / 2.0f; // Compensate refpos (always snap cursor to center)
		}

	// Compute new value
	float oldvalue = value;
	value = minimum + (pos - startpos) / (endpos - startpos) * (maximum - minimum);

	// Throw event
	if(scrollcbk && value != oldvalue)
		scrollcbk(this, value, scrollcbkuser);
}

void SliderControl::SetValue(float newvalue)
{
	// Ignore value chages while the slider is held by mouse
	if(ldowninsideregion)
	{
		// If the position gets programatically changed during a drag, then update drag start position
		float tmpvalue = min(maximum, max(minimum, newvalue));
		float tmppos = startpos + (tmpvalue - minimum) / (maximum - minimum) * (endpos - startpos);
		dragstartpos = tmppos;

		return;
	}

	// Clamp value
	value = min(maximum, max(minimum, newvalue));

	// Compute pos
	pos = startpos + (value - minimum) / (maximum - minimum) * (endpos - startpos);

	// Update slider position visually
	if(orientation == OT_HORIZONTAL)
		switch(sliderctrl->refdock & 0xF)
		{
		case 0x0:
			sliderctrl->Left = pos - sliderctrl->size.GetWidth() / 2.0f; // Compensate refpos (always snap cursor to center)
			break;
		case 0x1:
			sliderctrl->Left = pos;
			break;
		case 0x2:
			sliderctrl->Left = pos + sliderctrl->size.GetWidth() / 2.0f; // Compensate refpos (always snap cursor to center)
		}
	else // orientation == OT_VERTICAL
		switch((sliderctrl->refdock >> 4) & 0xF)
		{
		case 0x0:
			sliderctrl->Top = pos - sliderctrl->size.GetHeight() / 2.0f; // Compensate refpos (always snap cursor to center)
			break;
		case 0x1:
			sliderctrl->Top = pos;
			break;
		case 0x2:
			sliderctrl->Top = pos + sliderctrl->size.GetHeight() / 2.0f; // Compensate refpos (always snap cursor to center)
		}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void SliderControl::Release()
{
	Unregister();

	delete this;
}