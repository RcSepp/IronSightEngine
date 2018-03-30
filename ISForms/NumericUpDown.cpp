#include "Forms.h"


WNDPROC NumericUpDown::BaseClassWndProc = NULL;
#define NUD_SUPERCLASSNAME "NumericUpDown"

//-----------------------------------------------------------------------------
// Name: NumericUpDown()
// Desc: Constructor
//-----------------------------------------------------------------------------
NumericUpDown::NumericUpDown(WinForm* parentform) : handle(NULL), value(0), minvalue(0), maxvalue(100), parentform(parentform),
													basecontrol("EDIT", NUD_SUPERCLASSNAME, NULL, NULL, NULL, BaseClassWndProc, (WNDPROC)SuperClassWndProc, WS_EX_CLIENTEDGE)
{
	// Load and register Updown control class
    INITCOMMONCONTROLSEX iccx;
    iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccx.dwICC = ICC_UPDOWN_CLASS;
    InitCommonControlsEx(&iccx);
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG NumericUpDown::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_CREATE:
		{
			LPCREATESTRUCT createstruct = (LPCREATESTRUCT)lparam;
			Control* ctrl = (Control*)createstruct->lpCreateParams;
			NumericUpDown* nud = (NumericUpDown*)ctrl->ifc;

			RECT rect;
			GetWindowRect(wnd, &rect);
			ScreenToClient(wnd, (LPPOINT)&rect.left);
			ScreenToClient(wnd, (LPPOINT)&rect.right);

			// Create up-down control
			nud->handle = CreateWindowEx(0, UPDOWN_CLASS, 0, UDS_ALIGNRIGHT | UDS_SETBUDDYINT | UDS_ARROWKEYS | WS_CHILD | WS_VISIBLE, rect.right - 40, rect.top,
											40, rect.bottom - rect.top, GetParent(wnd), NULL, fms->GetInstance(), 0);

			// Explicitly attach the up-down control to its 'buddy' edit control
			SendMessage(nud->handle, UDM_SETBUDDY, (WPARAM)wnd, 0);

			// Set value range
			SendMessage(nud->handle, UDM_SETRANGE, 0, MAKELONG(nud->maxvalue, nud->minvalue));

			// Set current value based on current text of edit control
			short value = ctrl->text.IsEmpty() ? 0 : ctrl->text.ToInt();
			value = max(nud->minvalue, min(nud->maxvalue, value));
			SendMessage(nud->handle, UDM_SETPOS, 0, value);
		}
		break;
	}
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}

void NumericUpDown::OnNotifyMessage(LPNMUPDOWN nmupdown)
{
	if(vccbk)
	{
		int newvalue = nmupdown->iPos + nmupdown->iDelta;
		newvalue = max((int)minvalue, newvalue);
		newvalue = min((int)maxvalue, newvalue);
		vccbk(this, (short)newvalue, vccbkuser);
	}
}

short NumericUpDown::SetValue(short v)
{
	if(value == v)
		return value;
	value = v;
	if(handle)
		SendMessage(handle, UDM_SETPOS, 0, v);
	else
		basecontrol.SetText(v);
	return value;
}
short NumericUpDown::GetValue()
{
	short value = basecontrol.text.ToInt();
	return max(minvalue, min(maxvalue, value));
}
short NumericUpDown::SetMinValue(short v)
{
	if(minvalue == v)
		return minvalue;
	minvalue = v;
	SendMessage(handle, UDM_SETRANGE, 0, MAKELONG(maxvalue, minvalue));
	return minvalue;
}
short NumericUpDown::SetMaxValue(short v)
{
	if(maxvalue == v)
		return maxvalue;
	maxvalue = v;
	SendMessage(handle, UDM_SETRANGE, 0, MAKELONG(maxvalue, minvalue));
	return maxvalue;
}

//-----------------------------------------------------------------------------
// Name: ~NumericUpDown()
// Desc: Destructor
//-----------------------------------------------------------------------------
NumericUpDown::~NumericUpDown()
{
	parentform->nuds.remove(this);
}