#ifndef __INTERACTIVECONTROLS_H
#define __INTERACTIVECONTROLS_H

class GuiControl;

class InteractiveControl : public IManagedObject
{
public:
	virtual void Release() = 0;
};

//-----------------------------------------------------------------------------
// Name: SliderControl
// Desc: A control collection with predefined slider behavior
//-----------------------------------------------------------------------------
class SliderControl : public InteractiveControl, public ISliderControl
{
private:
	VALUECHANGED_CALLBACK vchangecbk, scrollcbk;
	LPVOID vchangecbkuser, scrollcbkuser;
	GuiControl *bgctrl, *sliderctrl;
	Orientation orientation;
	bool enablesnapback;
	float startpos, endpos, pos;
	float minimum, maximum, value;
	bool ldowninsideregion; // Used inside OnSliderMove
	float dragstartpos; // Used inside OnSliderMove and OnSliderUp

	static void __stdcall OnSliderDown(IGuiControl* sender, const MouseEventArgs& args, LPVOID user);
	static void __stdcall OnSliderUp(IGuiControl* sender, const MouseEventArgs& args, LPVOID user);
	static void __stdcall OnSliderMove(IGuiControl* sender, const MouseEventArgs& args, LPVOID user);
	void SetPos(float newpos);

public:

	SliderControl(GuiControl* bgctrl, GuiControl* sliderctrl, Orientation orientation, float startpos, float endpos)
	{
		this->bgctrl = bgctrl;
		this->sliderctrl = sliderctrl;
		this->orientation = orientation;
		this->startpos = startpos;
		this->endpos = endpos;
		vchangecbk = scrollcbk = NULL;
		enablesnapback = true;
		ldowninsideregion = false;
		minimum = 0.0f;
		maximum = 100.0f;

		SetPos(pos = startpos);
	};

	bool GetEnableSnapBack() {return enablesnapback;}
	void SetEnableSnapBack(bool val) {enablesnapback = val;};
	float GetStartpos() {return startpos;}
	void SetStartpos(float val) {startpos = val; SetPos(pos);};
	float GetEndpos() {return endpos;}
	void SetEndpos(float val) {endpos = val; SetPos(pos);};
	float GetMinimum() {return minimum;}
	void SetMinimum(float val) {minimum = val;};
	float GetMaximum() {return maximum;}
	void SetMaximum(float val) {maximum = val;};

	Result Init();
	void SetValue(float newvalue);
	void SetValueChangedCallback(const VALUECHANGED_CALLBACK vchangecbk, LPVOID user) {this->vchangecbk = vchangecbk; vchangecbkuser = user;}
	void SetScrollCallback(const VALUECHANGED_CALLBACK scrollcbk, LPVOID user) {this->scrollcbk = scrollcbk; scrollcbkuser = user;}
	void Release();
};

#endif