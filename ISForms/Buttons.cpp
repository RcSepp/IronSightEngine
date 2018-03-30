#include "Forms.h"

#pragma region Visual styles drawing
#define GROUPBOXTITLE_XPOS			9
#define GROUPBOXTITLE_LEFTEXTEND	1
#define GROUPBOXTITLE_RIGHTEXTEND	3

int GetFinalState(int basestate, CheckedState checked, bool enabled)
{
	return (enabled ? basestate : PBS_DISABLED) + (int)checked * PBS_DISABLED;
}

void DrawVisualStylesButton(Control* ctrl, int state)
{
	// Begin paint
	LPPAINTSTRUCT paintstruct = new PAINTSTRUCT();
	RECT boundingrect, contentrect;
	HDC hdc = BeginPaint(ctrl->handle, paintstruct);
	SetRect(&boundingrect, 0, 0, ctrl->bounds.width, ctrl->bounds.height);

	// Draw background
	if(IsThemeBackgroundPartiallyTransparent(ctrl->theme, ctrl->type.vspart, state))
		DrawThemeParentBackground(ctrl->handle, hdc, &paintstruct->rcPaint);
	DrawThemeBackground(ctrl->theme, hdc, ctrl->type.vspart, state, &boundingrect, &paintstruct->rcPaint);

	// Draw foreground
	SelectObject(hdc, ctrl->font);
	GetThemeBackgroundContentRect(ctrl->theme, hdc, ctrl->type.vspart, state, &boundingrect, &contentrect);
	DrawThemeText(ctrl->theme, hdc, ctrl->type.vspart, state, ctrl->text.ToWCharString(), ctrl->text.length(),
				  DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &contentrect);

	// End paint
	EndPaint(ctrl->handle, paintstruct);
}

void DrawVisualStylesGroupBox(Control* ctrl, int state)
{
	// Begin paint
	LPPAINTSTRUCT paintstruct = new PAINTSTRUCT();
	RECT boundingrect, contentrect;
	HDC hdc = BeginPaint(ctrl->handle, paintstruct);
	SetRect(&boundingrect, 0, 0, ctrl->bounds.width, ctrl->bounds.height);

	// Draw background
	if(IsThemeBackgroundPartiallyTransparent(ctrl->theme, ctrl->type.vspart, state))
		DrawThemeParentBackground(ctrl->handle, hdc, &paintstruct->rcPaint);
	DrawThemeBackground(ctrl->theme, hdc, ctrl->type.vspart, state, &boundingrect, &paintstruct->rcPaint);

	// Draw foreground
	SelectObject(hdc, ctrl->font);
	DTTOPTS opts;
	ZeroMemory(&opts, sizeof(DTTOPTS));
	opts.dwSize = sizeof(DTTOPTS);
	opts.dwFlags = DTT_CALCRECT;
	SetRectEmpty(&boundingrect);
	DrawThemeTextEx(ctrl->theme, hdc, ctrl->type.vspart, state, ctrl->text.ToWCharString(), ctrl->text.length(), DT_CALCRECT, &boundingrect, &opts);
	SetRect(&contentrect, GROUPBOXTITLE_XPOS - GROUPBOXTITLE_LEFTEXTEND, -boundingrect.bottom / 2,
			boundingrect.right + GROUPBOXTITLE_XPOS + GROUPBOXTITLE_RIGHTEXTEND, boundingrect.bottom);
	DrawThemeParentBackground(ctrl->handle, hdc, &contentrect);
	contentrect.left += GROUPBOXTITLE_LEFTEXTEND; contentrect.right -= GROUPBOXTITLE_RIGHTEXTEND;
	DrawThemeTextEx(ctrl->theme, hdc, ctrl->type.vspart, state, ctrl->text.ToWCharString(), ctrl->text.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, &contentrect, &opts);

	// End paint
	EndPaint(ctrl->handle, paintstruct);
}

void DrawVisualStylesTextBox(Control* ctrl, int state)
{
	// Begin paint
	LPPAINTSTRUCT paintstruct = new PAINTSTRUCT();
	RECT boundingrect, contentrect;
	HDC hdc = BeginPaint(ctrl->handle, paintstruct);
	SetRect(&boundingrect, 0, 0, ctrl->bounds.width, ctrl->bounds.height);

	// Draw background
	if(IsThemeBackgroundPartiallyTransparent(ctrl->theme, EP_BACKGROUNDWITHBORDER, state))
		DrawThemeParentBackground(ctrl->handle, hdc, &paintstruct->rcPaint);
	DrawThemeBackground(ctrl->theme, hdc, EP_BACKGROUNDWITHBORDER, state, &boundingrect, &paintstruct->rcPaint);

	// Draw text
	SelectObject(hdc, ctrl->font);
	GetThemeBackgroundContentRect(ctrl->theme, hdc, EP_BACKGROUNDWITHBORDER, state, &boundingrect, &contentrect);
	DrawThemeText(ctrl->theme, hdc, EP_EDITTEXT, state, ctrl->text.ToWCharString(), ctrl->text.length(),
				  DT_LEFT | DT_TOP | DT_SINGLELINE, 0, &contentrect);

	if(state == ETS_FOCUSED)
	{
		LineTo(hdc, 100, 100);
		//DrawThemeBackground(ctrl->theme, hdc, EP_CARET, state, &boundingrect, &paintstruct->rcPaint);
	}

	// End paint
	EndPaint(ctrl->handle, paintstruct);
}
#pragma endregion

#pragma region PushButton
WNDPROC Button::BaseClassWndProc = NULL;

//-----------------------------------------------------------------------------
// Name: CreateButton()
// Desc: DLL API for creating an instance of a push button control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPBUTTON __cdecl CreateButton()
{
	return new Button(WC_BUTTON, "PushButton", BS_PUSHBUTTON, VSCLASS_BUTTON, BP_PUSHBUTTON);
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG Button::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static bool mousetrackingenabled = false; //EDIT: DOESN'T WORK THAT WAY (See Engine.cpp::WndProc for how it's done properly)
	static int currentstate = PBS_NORMAL;
	static bool mousedownwasonctrl = false;

	Control* ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
	if(ctrl)
	{
		switch(msg)
		{
		case WM_KEYDOWN:
			ctrl->RaiseKeyDownCbk(wparam);
			break;
		case WM_KEYUP:
			ctrl->RaiseKeyUpCbk(wparam);
			break;

		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
			mousedownwasonctrl = true; //EDIT: DOESN'T WORK THAT WAY (See Engine.cpp::WndProc for how it's done properly)
			break;

		case WM_LBUTTONUP:
			mousedownwasonctrl = false; //EDIT: DOESN'T WORK THAT WAY (See Engine.cpp::WndProc for how it's done properly)
			break;
		}
		if(ctrl->theme)
			switch(msg)
			{
			case WM_PAINT:
				DrawVisualStylesButton(ctrl, GetFinalState(currentstate, ctrl->checkedstate, ctrl->enabled));
				return 0; // Event handled
			case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
				currentstate = PBS_PRESSED;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_LBUTTONUP:
				currentstate = PBS_HOT;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_MOUSEMOVE:
				if(!mousetrackingenabled)
					if(TrackMouseEvent(&ctrl->mousetracker))
					{
						mousetrackingenabled = true;
						// Mouse enter
						currentstate = PBS_HOT;
						RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
					}
				return 0; // Event handled
			case WM_MOUSELEAVE:
				mousetrackingenabled = false;
				currentstate = PBS_NORMAL;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_ENABLE:
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			}
	}
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}
#pragma endregion

#pragma region CheckBox
WNDPROC CheckBox::BaseClassWndProc = NULL;

//-----------------------------------------------------------------------------
// Name: CreateCheckBox()
// Desc: DLL API for creating an instance of a check box button control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPCHECKBOX __cdecl CreateCheckBox()
{
	return new CheckBox(WC_BUTTON, "CheckBox", BS_CHECKBOX, VSCLASS_BUTTON, BP_CHECKBOX);
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG CheckBox::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static bool mousetrackingenabled = false;
	static int currentstate = CBS_UNCHECKEDNORMAL;

	Control* ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
	if(ctrl)
	{
		switch(msg)
		{
		case WM_KEYDOWN:
			ctrl->RaiseKeyDownCbk(wparam);
			break;
		case WM_KEYUP:
			ctrl->RaiseKeyUpCbk(wparam);
			break;

		case WM_LBUTTONUP:
			ctrl->checkedstate = (CheckedState)!((BOOL)ctrl->checkedstate);
			Button_SetCheck(ctrl->handle, ctrl->checkedstate);
		}
		if(ctrl->theme)
			switch(msg)
			{
			case WM_PAINT:
				DrawVisualStylesButton(ctrl, GetFinalState(currentstate, ctrl->checkedstate, ctrl->enabled));
				return 0; // Event handled
			case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
				currentstate = PBS_PRESSED;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_LBUTTONUP:
				currentstate = PBS_HOT;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_MOUSEMOVE:
				if(!mousetrackingenabled)
					if(TrackMouseEvent(&ctrl->mousetracker))
					{
						mousetrackingenabled = true;
						// Mouse enter
						currentstate = PBS_HOT;
						RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
					}
				return 0; // Event handled
			case WM_MOUSELEAVE:
				mousetrackingenabled = false;
				currentstate = PBS_NORMAL;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_ENABLE:
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			}
	}
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}
#pragma endregion

#pragma region RadioButton
WNDPROC RadioButton::BaseClassWndProc = NULL;

//-----------------------------------------------------------------------------
// Name: CreateRadioButton()
// Desc: DLL API for creating an instance of a radio button control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPRADIOBUTTON __cdecl CreateRadioButton()
{
	return new RadioButton(WC_BUTTON, "RadioButton", BS_RADIOBUTTON, VSCLASS_BUTTON, BP_RADIOBUTTON);
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG RadioButton::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static bool mousetrackingenabled = false;
	static int currentstate = RBS_UNCHECKEDNORMAL;

	Control* ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
	if(ctrl)
	{
		switch(msg)
		{
		case WM_KEYDOWN:
			ctrl->RaiseKeyDownCbk(wparam);
			break;
		case WM_KEYUP:
			ctrl->RaiseKeyUpCbk(wparam);
			break;

		case WM_LBUTTONUP:
			ctrl->checkedstate = (CheckedState)!((BOOL)ctrl->checkedstate);
			Button_SetCheck(ctrl->handle, ctrl->checkedstate);
		}
		if(ctrl->theme)
			switch(msg)
			{
			case WM_PAINT:
				DrawVisualStylesButton(ctrl, GetFinalState(currentstate, ctrl->checkedstate, ctrl->enabled));
				return 0; // Event handled
			case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
				currentstate = PBS_PRESSED;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_LBUTTONUP:
				currentstate = PBS_HOT;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_MOUSEMOVE:
				if(!mousetrackingenabled)
					if(TrackMouseEvent(&ctrl->mousetracker))
					{
						mousetrackingenabled = true;
						// Mouse enter
						currentstate = PBS_HOT;
						RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
					}
				return 0; // Event handled
			case WM_MOUSELEAVE:
				mousetrackingenabled = false;
				currentstate = PBS_NORMAL;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_ENABLE:
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			}
	}
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}
#pragma endregion

#pragma region GroupBox
WNDPROC GroupBox::BaseClassWndProc = NULL;

//-----------------------------------------------------------------------------
// Name: CreateGroupBox()
// Desc: DLL API for creating an instance of a group box button control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPGROUPBOX __cdecl CreateGroupBox()
{
	return new GroupBox(WC_BUTTON, "GroupButton", BS_GROUPBOX, VSCLASS_BUTTON, BP_GROUPBOX);
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG GroupBox::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static bool mousetrackingenabled = false;
	static int currentstate = GBS_NORMAL;

	Control* ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
	if(ctrl)
	{
		switch(msg)
		{
		case WM_KEYDOWN:
			ctrl->RaiseKeyDownCbk(wparam);
			break;
		case WM_KEYUP:
			ctrl->RaiseKeyUpCbk(wparam);
		}
		if(ctrl->theme)
			switch(msg)
			{
			case WM_PAINT:
				ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
				DrawVisualStylesGroupBox(ctrl, GetFinalState(currentstate, ctrl->checkedstate, ctrl->enabled));
				return 0; // Event handled
			case WM_ENABLE:
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			}
	}
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}
#pragma endregion

#pragma region Label
WNDPROC Label::BaseClassWndProc = NULL;

//-----------------------------------------------------------------------------
// Name: CreateLabel()
// Desc: DLL API for creating an instance of a group box button control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPLABEL __cdecl CreateLabel()
{
	return new Label(WC_STATIC, "Label", SS_ENDELLIPSIS, NULL, 0);
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG Label::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static bool mousetrackingenabled = false;
	static int currentstate = GBS_NORMAL;

	Control* ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
	if(ctrl)
	{
		switch(msg)
		{
		case WM_KEYDOWN:
			ctrl->RaiseKeyDownCbk(wparam);
			break;
		case WM_KEYUP:
			ctrl->RaiseKeyUpCbk(wparam);
		}
	}
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}
#pragma endregion

#pragma region TextBox
WNDPROC TextBox::BaseClassWndProc = NULL;

//-----------------------------------------------------------------------------
// Name: CreateTextBox()
// Desc: DLL API for creating an instance of a group box button control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPTEXTBOX __cdecl CreateTextBox(bool hasvscroll)
{
	return new TextBox(WC_EDIT, "TextBox", WS_BORDER | ES_MULTILINE | (hasvscroll * WS_VSCROLL), VSCLASS_EDIT, EP_BACKGROUNDWITHBORDER);
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG TextBox::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static bool mousetrackingenabled = false;
	static int currentstate = ETS_NORMAL;

	Control* ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
	if(ctrl)
	{
		switch(msg)
		{
		case WM_KEYDOWN:
			ctrl->RaiseKeyDownCbk(wparam);
			break;
		case WM_KEYUP:
			ctrl->RaiseKeyUpCbk(wparam);
			break;
case WM_CHAR: //EDIT: This was added to avoid a system beep when the return key is pressed (as noticed in VMPU -> ScriptConstantsWindow.cpp when pressing return to close the edit box)
	if(wparam == VK_RETURN)
		return 0;
	break;

		case WM_CONTEXTMENU:
			if(ctrl->ctxmenu)
			{
				ctrl->ShowContextMenu(); // Show custom contextmenu
				return 0; // Supress standard contextmenu
			}
		}
		if(ctrl->theme)
			switch(msg)
			{
			case WM_PAINT:
				ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
				DrawVisualStylesTextBox(ctrl, GetFinalState(currentstate, ctrl->checkedstate, ctrl->enabled));
				return 0; // Event handled
			case WM_COMMAND:
				ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
				DrawVisualStylesTextBox(ctrl, GetFinalState(currentstate, ctrl->checkedstate, ctrl->enabled));
				return 0; // Event handled
			case WM_SETFOCUS:
				currentstate = ETS_FOCUSED;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_KILLFOCUS:
				currentstate = ETS_NORMAL;
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			case WM_ENABLE:
				RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);
				return 0; // Event handled
			//default:
				//cle->PrintLine(String((int)msg));
			}
	}
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}

void TextBox::AppendText(const String& text)
{
	basecontrol.text <<= text;

	if(basecontrol.handle)
		SetWindowText(basecontrol.handle, basecontrol.text);
}
#pragma endregion