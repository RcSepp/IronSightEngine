#include "Forms.h"


static Control* currentcreatewindowctrl; //EDIT: Not thread safe

//-----------------------------------------------------------------------------
// Name: Control()
// Desc: Constructor; Set type and default properties
//-----------------------------------------------------------------------------
Control::Control(LPCTSTR wndbaseclass, LPCTSTR wndsuperclass, DWORD ctrlstyle, LPCWSTR vsclass, int vspart, WNDPROC& baseclasswndproc, WNDPROC superclasswndproc, DWORD ctrlexstyle) :
	suspendlayout(false), superclasswndproc(superclasswndproc)
{
	type.wndbaseclass = wndbaseclass;
	type.wndsuperclass = wndsuperclass;
	type.ctrlstyle = ctrlstyle;
	type.ctrlexstyle = ctrlexstyle;
	type.vsclass = vsclass;
	type.vspart = vspart;

	if(!baseclasswndproc)
		CreateSuperClass(type.wndbaseclass, type.wndsuperclass, baseclasswndproc, (WNDPROC)ControlWndProc);

	handle = NULL;
	theme = NULL;
	text = String("");
	font = fms->defaultfont;
	bounds = Rect<int>(75, 23);
	visible = enabled = true;
	checkedstate = CS_UNCHECKED;
	mousedowncbk = mouseupcbk = mousemovecbk = mousedclickcbk = mousewheelcbk = NULL;
	notifycbk = NULL;
	mclickcbk = NULL;
	keydowncbk = keyupcbk = NULL;
	pymousedowncbk = pymouseupcbk = pymousemovecbk = pymousedclickcbk = pymousewheelcbk = pymclickcbk = pykeydowncbk = pykeyupcbk = NULL;
}

//-----------------------------------------------------------------------------
// Name: CreateSuperClass()
// Desc: Create a super class based on a base class
//-----------------------------------------------------------------------------
bool Control::CreateSuperClass(LPCTSTR wndbaseclassname, LPCTSTR wndsuperclassname, WNDPROC& baseclasswndproc, WNDPROC superclasswndproc)
{
	WNDCLASS wndsuperclass;
	if(!GetClassInfo(eng->GetHInstance(), wndbaseclassname, &wndsuperclass))
		return false;
	wndsuperclass.hInstance = eng->GetHInstance();
	wndsuperclass.lpszClassName = wndsuperclassname;
	baseclasswndproc = wndsuperclass.lpfnWndProc;
	wndsuperclass.lpfnWndProc = (WNDPROC)*superclasswndproc;
	return (RegisterClass(&wndsuperclass) != 0);
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG Control::ControlWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Control* ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
	if(!ctrl)
		ctrl = currentcreatewindowctrl;

	switch(msg)
	{
	case WM_LBUTTONDOWN: case WM_MBUTTONDOWN: case WM_RBUTTONDOWN:
		SetCapture(wnd);
		ctrl->RaiseMouseDownCbk(MouseEventArgs(msg, wparam, lparam));
		break;

	case WM_LBUTTONUP: case WM_MBUTTONUP: case WM_RBUTTONUP:
		ctrl->RaiseMouseUpCbk(MouseEventArgs(msg, wparam, lparam));
		break;

	case WM_LBUTTONDBLCLK: case WM_MBUTTONDBLCLK: case WM_RBUTTONDBLCLK:
		ctrl->RaiseMouseDClickCbk(MouseEventArgs(msg, wparam, lparam));
		break;

	case WM_MOUSEWHEEL:
		ctrl->RaiseMouseWheelCbk(MouseEventArgs(msg, wparam, lparam));
		break;

	case WM_MOUSEMOVE:
		ctrl->RaiseMouseMoveCbk(MouseEventArgs(msg, wparam, lparam));
		break;

	case WM_PAINT:
		if(ctrl->suspendlayout)
			return 0;
	break;
	}

	return CallWindowProc(ctrl->superclasswndproc, wnd, msg, wparam, lparam);
}

//-----------------------------------------------------------------------------
// Name: OnAddControl()
// Desc: Initialize control
//-----------------------------------------------------------------------------
Result Control::OnAddControl(IContainer* parent, IControl* ifc)
{
	this->parentform = parent->GetParentForm();
	this->ifc = ifc;

	currentcreatewindowctrl = this;
	handle = CreateWindowEx(type.ctrlexstyle, (LPCTSTR)type.wndsuperclass, text, type.ctrlstyle | visible * WS_VISIBLE | !enabled * WS_DISABLED | WS_CHILD, bounds.x, bounds.y,
						  bounds.width, bounds.height, parent->GetHwnd(), NULL, parentform->instance, this); // EDIT: Use ES_MULTILINE only on textbox control and only if enabled
	if(checkedstate != CS_UNCHECKED)
		Button_SetCheck(handle, checkedstate);
	SetProp(handle, "PROP_CTRL", this); // EDIT: Use ATOM for "PROP_CTRL"

	// Set default font
	SendMessage(handle, WM_SETFONT, (WPARAM)((Forms*)&*fms)->defaultfont, TRUE);

	// Prepare mouse tracker
	mousetracker.cbSize = sizeof(TRACKMOUSEEVENT);
	mousetracker.dwFlags = TME_LEAVE;
	mousetracker.hwndTrack = handle;

	UpdateVisualStyles();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Paint()
// Desc: Redraw control (should be called on WM_PAINT) *deprecated
//-----------------------------------------------------------------------------
void Control::Paint(int state)
{
	if(theme)
	{
		// Begin paint
		LPPAINTSTRUCT paintstruct = new PAINTSTRUCT();
		RECT boundingrect, contentrect;
		HDC hdc = BeginPaint(handle, paintstruct);
		SetRect(&boundingrect, 0, 0, bounds.width, bounds.height);

		// Draw background
		if(IsThemeBackgroundPartiallyTransparent(theme, type.vspart, state))
			DrawThemeParentBackground(handle, hdc, &paintstruct->rcPaint);
		DrawThemeBackground(theme, hdc, type.vspart, state, &boundingrect, &paintstruct->rcPaint);

		// Draw foreground
		SelectObject(hdc, font);
		//GetThemeBackgroundContentRect(theme, hdc, type.vspart, state, &boundingrect, &contentrect);
		//DrawThemeParentBackground(handle, hdc, Rect<>(6,-6,100,20));
		//DrawThemeText(theme, hdc, type.vspart, state, text.ToWCharString(), text.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, 0, Rect<>(6,-6,bounds.width,bounds.height)); // | DT_CENTER | DT_VCENTER		&contentrect

		DTTOPTS opts;
		ZeroMemory(&opts, sizeof(DTTOPTS));
		opts.dwSize = sizeof(DTTOPTS);
		opts.dwFlags = DTT_CALCRECT; // | DTT_BORDERCOLOR | DTT_GLOWSIZE | DTT_SHADOWTYPE;
		opts.crBorder = 0xFFFF0000;
		opts.iBorderSize = 10;
		opts.iGlowSize = 3;
		opts.iTextShadowType = TST_CONTINUOUS;
		contentrect = RECT(boundingrect);
		DrawThemeTextEx(theme, hdc, type.vspart, state, text.ToWCharString(), text.length(), DT_CALCRECT, &contentrect, &opts);
		contentrect.left += 8; contentrect.right += 12;
		contentrect.top -= 6; contentrect.bottom -= 6;
		DrawThemeParentBackground(handle, hdc, &contentrect);
		contentrect.left += 1; contentrect.right -= 3;
		DrawThemeTextEx(theme, hdc, type.vspart, state, text.ToWCharString(), text.length(), DT_LEFT | DT_TOP | DT_SINGLELINE, &contentrect, &opts);

		// End paint
		EndPaint(handle, paintstruct);
	}
}

//-----------------------------------------------------------------------------
// Name: UpdateVisualStyles()
// Desc: Open or close visual styles depending on fms->VisualStylesEnabled()
//-----------------------------------------------------------------------------
void Control::UpdateVisualStyles()
{
	if(fms->VisualStylesEnabled())
	{
		if(!theme && handle)
			theme = OpenThemeData(handle, type.vsclass);
	}
	else if(theme)
	{
		CloseThemeData(theme);
		theme = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name: SetVisible()
// Desc: Set visibility state and issue a visibility change to the window
//-----------------------------------------------------------------------------
void Control::SetVisible(bool val)
{
	if(visible == val)
		return;
	visible = val;

	if(handle)
	{
		if(val)
			ShowWindow(handle, SW_SHOW);
		else
			ShowWindow(handle, SW_HIDE);
	}
}

//-----------------------------------------------------------------------------
// Name: SetEnable()
// Desc: Set enabled state and issue enabling/disabling to the window
//-----------------------------------------------------------------------------
void Control::SetEnabled(bool val)
{
	if(enabled == val)
		return;
	enabled = val;

	if(handle)
		EnableWindow(handle, val);
}

//-----------------------------------------------------------------------------
// Name: SetText()
// Desc: Set text and issue a text change to the window
//-----------------------------------------------------------------------------
void Control::SetText(const String& val)
{
	if(text == val)
		return;
	text = val;

	if(handle)
		SetWindowText(handle, text);
}

//-----------------------------------------------------------------------------
// Name: SetBounds()
// Desc: Set bounds and issue a bounds change to the window
//-----------------------------------------------------------------------------
void Control::SetBounds(const Rect<int> &val)
{
	if(bounds == val)
		return;
	bounds = val;

	if(handle)
		MoveWindow(handle, bounds.x, bounds.y, bounds.width, bounds.height, visible);
}

//-----------------------------------------------------------------------------
// Name: SetCheckedState()
// Desc: Set checked state and issue a checked state change to the window
//-----------------------------------------------------------------------------
void Control::SetCheckedState(CheckedState val)
{
	if(checkedstate == val)
		return;
	checkedstate = val;

	if(handle)
		Button_SetCheck(handle, checkedstate);
}

//-----------------------------------------------------------------------------
// Name: OnRemoveControl()
// Desc: A release function with result
//-----------------------------------------------------------------------------
Result Control::OnRemoveControl()
{
	if(!parentform)
		return R_OK;
	Result rlt;

	parentform = NULL;

	if(handle && !DestroyWindow(handle))
		return ERR(String("Error destroying control: Error code ") << String((int)GetLastError()));
	if(theme)
	{
		CHKRESULT(CloseThemeData(theme));
		theme = NULL;
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: OnNotifyMessage()
// Desc: Gets called by the parent form whenever a notication code is received through WM_COMMAND or WM_NOTIFY
//-----------------------------------------------------------------------------
void Control::OnNotifyMessage(UINT notifycode)
{
	switch(notifycode)
	{
	case BN_CLICKED:
		RaiseMouseClickCbk();
		break;

	case EN_CHANGE:
		{
			int textlen = GetWindowTextLength(handle);
			char* tmp = new char[textlen + 1];
			GetWindowText(handle, tmp, textlen + 1);
			text = String::StealReference(tmp);
		}
		break;
	}

	if(notifycbk)
		notifycbk(notifycode, notifycbkuser);
}

//-----------------------------------------------------------------------------
// Name: ~Control()
// Desc: Destructor
//-----------------------------------------------------------------------------
Control::~Control()
{
	RemovePyCallbacks();

	OnRemoveControl();
}
/*//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Control::Release()
{
	if(parentform)
		parentform->controls.remove(this);

	OnRemoveControl();

	delete this;
}*/