#include "Forms.h"


//-----------------------------------------------------------------------------
// Name: WinForm()
// Desc: Constructor
//-----------------------------------------------------------------------------
WinForm::WinForm()
{
	handle = mdiclienthandle = NULL;
	parent = NULL;
	dragentercbk = dragovercbk = dropcbk = NULL;
	dragleavecbk = NULL;
	mousedowncbk = mouseupcbk = mousemovecbk = mousedclickcbk = mousewheelcbk = NULL;
	paintcbk = NULL;
	fclosingcbk = NULL;
	fclosedcbk = NULL;
	fresizecbk = NULL;
	fsizechangedcbk = NULL;
	fmovingcbk = NULL;
	fmovedcbk = NULL;
	pymousedowncbk = pymouseupcbk = pymousemovecbk = pymousedclickcbk = pymousewheelcbk = pypaintcbk = NULL;
	pyfclosingcbk = pyfclosedcbk = pyfresizecbk = pyfsizechangedcbk = pyfmovingcbk = pyfmovedcbk = NULL;
	dragdropenabled = false;
	iunknownrefctr = 0;
	visible = false;
	parentform = this;
	titlemenu = NULL;
	oldwndstate = WS_NORMAL;
	tooltip = NULL;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize form
//-----------------------------------------------------------------------------
Result WinForm::Init(FormSettings *settings, const String* classname)
{
	this->instance = fms->GetInstance();
	this->caption = settings->caption;
	this->visible = true;
	this->parent = (WinForm*)settings->parent;
	this->oldwndstate = settings->wndstate;

	int captionheight = settings->fbstyle & WS_THICKFRAME && !(settings->options & FS_MDICHILD) ? GetSystemMetrics(SM_CYCAPTION) : GetSystemMetrics(SM_CYSMCAPTION);
	int totalwidth = settings->clientsize.width + 2 * GetSystemMetrics(SM_CXSIZEFRAME);
	int totalheight = settings->clientsize.height + 2 * GetSystemMetrics(SM_CYSIZEFRAME) + captionheight;

	if(settings->options & FS_CENTERTOSCREEN)
	{
		settings->windowpos.x = (GetSystemMetrics(SM_CXSCREEN) - totalwidth) / 2;
		settings->windowpos.y = (GetSystemMetrics(SM_CYSCREEN) - totalheight) / 2;
	}
	else if(settings->fbstyle == FBS_NOBORDER)
	{
		// Adjust for clipped border
		settings->windowpos.x -= GetSystemMetrics(SM_CXFIXEDFRAME);
		settings->windowpos.y -= GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION);
	}

	bool usedefaultclassname = (classname == NULL || classname->IsEmpty());
	if(usedefaultclassname)
	{
		this->classname = String(DEFAULT_WINDOWCLASS);
		classname = new String(this->classname);
	}
	else
		this->classname = String(*classname);

	if(!eng->IsWinClassRegistered(classname))
		eng->RegisterWinClass(classname);

	DWORD styles = settings->fbstyle | WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
				   (settings->wndstate == WS_MINIMIZED) * WS_MINIMIZE | (settings->wndstate == WS_MAXIMIZED) * WS_MAXIMIZE |
				   (settings->hasvscroll == true) * WS_VSCROLL | (settings->hashscroll == true) * WS_HSCROLL;
	if(settings->options & FS_MDICHILD)
	{
		if(!settings->parent)
			return ERR("Parameter parent not specified");

		if(settings->windowpos.x == -1) settings->windowpos.x = CW_USEDEFAULT;
		if(settings->windowpos.y == -1) settings->windowpos.y = CW_USEDEFAULT;
		if(settings->clientsize.width == -1) totalwidth = CW_USEDEFAULT;
		if(settings->clientsize.height == -1) totalheight = CW_USEDEFAULT;

		handle = CreateWindowEx(WS_EX_MDICHILD | (WS_EX_TOOLWINDOW * settings->istoolwindow), *classname, settings->caption, WS_VISIBLE | WS_CHILD | styles, settings->windowpos.x,
								 settings->windowpos.y, totalwidth, totalheight, ((WinForm*)settings->parent)->mdiclienthandle, NULL, instance, NULL);
	}
	else
		handle = CreateWindowEx(WS_EX_TOOLWINDOW * settings->istoolwindow, *classname, settings->caption, WS_CLIPCHILDREN | styles, settings->windowpos.x,
								settings->windowpos.y, totalwidth, totalheight, settings->parent ? ((WinForm*)settings->parent)->handle : NULL, NULL, instance, NULL);

	if(usedefaultclassname)
		delete classname;

	if(!handle)
		return ERR(String("Error creating form: error code = ") << String((int)GetLastError()));

	tracker.Init(handle);

	if(settings->fbstyle == FBS_NOBORDER)
	{
		// Clip window frame
		HRGN hrgn = CreateRectRgn(GetSystemMetrics(SM_CXFIXEDFRAME), GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION),
								  GetSystemMetrics(SM_CXFIXEDFRAME) + settings->clientsize.width,
								  GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION) + settings->clientsize.height);
		SetWindowRgn(handle, hrgn, TRUE);
	}

	// Create MDI client area
	if(settings->options & FS_MDICLIENT)
	{
		CLIENTCREATESTRUCT clientcreatestruct;
		clientcreatestruct.idFirstChild = 100;
		if(settings->mdiwindowmenu)
		{
			MenuItem* newmenuitem;
			CHKALLOC(newmenuitem = new MenuItem(NULL, NULL, IMenuItem::MI_DROPDOWN_STATIC));
			newmenuitem->clickcbk = NULL;
			newmenuitem->clickcbkuser = NULL;
			*settings->mdiwindowmenu = newmenuitem;
			clientcreatestruct.hWindowMenu = newmenuitem->GetSubMenuHandle();
		}
		else
			clientcreatestruct.hWindowMenu = NULL;
		mdiclienthandle = CreateWindowEx(0, "MDICLIENT", NULL, WS_CHILD | WS_CLIPCHILDREN, 0, 0, 0, 0, handle, NULL, instance, &clientcreatestruct);

		if(!mdiclienthandle)
			return ERR("Error creating mdi client area");

		ShowWindow(mdiclienthandle, SW_SHOW);

		SetWindowPos(mdiclienthandle, NULL, 0, 0, settings->clientsize.width, settings->clientsize.height, SWP_NOMOVE | SWP_NOZORDER);
	}

	if(settings->wndstate == WS_MINIMIZED || settings->wndstate == WS_MAXIMIZED)
	{
		WINDOWPLACEMENT wp = {0};
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(handle, &wp);
		wp.showCmd = SW_HIDE;
		wp.rcNormalPosition.left = settings->windowpos.x;
		wp.rcNormalPosition.top = settings->windowpos.y;
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + totalwidth;
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + totalheight;
		SetWindowPlacement(handle, &wp);
	}

	if(parent)
	{
		// Report this window as child of settings->parent
		parent->childforms.push_back(this);

		if(!(settings->options & FS_MDICHILD))
		{
			// Directly display dialog windows, disabling the parent window beneath
			Show();
			EnableWindow(parent->handle, false);
		}
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize form from existing window
//-----------------------------------------------------------------------------
Result WinForm::Init(HWND handle)
{
	this->handle = handle;

	this->instance = fms->GetInstance();
	this->visible = true;
	this->parent = NULL;
	this->oldwndstate = GetWindowState();

	int textlen = GetWindowTextLength(handle);
	char* tmp = new char[textlen + 1];
	GetWindowText(handle, tmp, textlen + 1);
	this->caption = String::StealReference(tmp);

	this->classname = "";

	//EDIT: Hook dll that created handle using hook functions as in http://stackoverflow.com/questions/1091557/wndproc-hook-does-not-receive-wm-command-from-menus

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateNumericUpDown()
// Desc: Create and register a numeric-up-down control
//-----------------------------------------------------------------------------
Result WinForm::CreateNumericUpDown(INumericUpDown** nud)
{
	Result rlt;

	NumericUpDown* newnud;
	CHKALLOC(newnud = new NumericUpDown(this));

	nuds.push_back(newnud);
	*nud = newnud;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateTrayIcon()
// Desc: Create and register a notification icon in the taskbar
//-----------------------------------------------------------------------------
Result WinForm::CreateTrayIcon(const FilePath& iconfilename, ITrayIcon** trayicon)
{
	*trayicon = NULL;
	Result rlt;

	TrayIcon* newtrayicon;
	CHKALLOC(newtrayicon = new TrayIcon());
	CHKRESULT(newtrayicon->Init(iconfilename, this));

	trayicons.push_back(newtrayicon);
	*trayicon = newtrayicon;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateToolbar()
// Desc: Create a toolbar inside this form
//-----------------------------------------------------------------------------
Result WinForm::CreateToolbar(bool useliststyle, IToolbar** toolbar)
{
	*toolbar = NULL;
	Result rlt;

	Toolbar* newtoolbar;
	CHKALLOC(newtoolbar = new Toolbar());
	CHKRESULT(newtoolbar->Init(useliststyle, this));

	toolbars.push_back(newtoolbar);
	*toolbar = newtoolbar;

	// Fit mdi area to client area
	if(mdiclienthandle)
	{
		int toolbarheight = GetToolbarAndMenuHeight();

		Rect<int> bounds;
		GetClientBoundsInternal(bounds);

		SetWindowPos(mdiclienthandle, NULL, 0, toolbarheight, bounds.width, bounds.height - toolbarheight, SWP_NOZORDER);
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Show()
// Desc: Show the form
//-----------------------------------------------------------------------------
void WinForm::Show()
{
	visible = true;

	if(!handle)
		return;

	WINDOWPLACEMENT wp;
	GetWindowPlacement(handle, &wp);

	int cmdshow;
	if(wp.showCmd == SW_SHOWMAXIMIZED)
		cmdshow = SW_MAXIMIZE;
	else if(wp.showCmd == SW_SHOWMINIMIZED)
		cmdshow = SW_MINIMIZE;
	else
		cmdshow = SW_RESTORE;
	ShowWindow(handle, cmdshow);
}

//-----------------------------------------------------------------------------
// Name: Hide()
// Desc: Hide the form
//-----------------------------------------------------------------------------
void WinForm::Hide()
{
	visible = false;

	if(handle)
		ShowWindow(handle, SW_HIDE);
}

//-----------------------------------------------------------------------------
// Name: BringToFront()
// Desc: Move form to the front of the z order
//-----------------------------------------------------------------------------
void WinForm::BringToFront()
{
	SetForegroundWindow(handle);
}

//-----------------------------------------------------------------------------
// Name: GetBoundsInternal()
// Desc: Get bounds using GetClientRect()
//-----------------------------------------------------------------------------
void WinForm::GetBoundsInternal(Rect<int>& bounds) const
{
	if(!handle)
		return;

	RECT rect;
	WINDOWPLACEMENT wp;
	GetWindowPlacement(handle, &wp);
	if(wp.showCmd == SW_SHOWMINIMIZED)
	{
		rect = wp.rcNormalPosition;

		bounds.x = rect.left;
		bounds.y = rect.top;
		bounds.width = rect.right - rect.left;
		bounds.height = rect.bottom - rect.top;
	}
	else
	{
		GetWindowRect(handle, &rect);

		bounds.x = rect.left;
		bounds.y = rect.top;
		bounds.width = rect.right - rect.left;
		bounds.height = rect.bottom - rect.top;

		// Make relative to parent position for child windows
		if(parent)
		{
			GetWindowRect(parent->handle, &rect);
			int toolbarheight = parent->GetToolbarAndMenuHeight();
			int captionheight = parent->GetCaptionHeight();
			bounds.x -= rect.left + GetSystemMetrics(SM_CXSIZEFRAME);
			bounds.y -= rect.top + toolbarheight + GetSystemMetrics(SM_CYSIZEFRAME) + captionheight;
		}
	}
}
void WinForm::GetClientBoundsInternal(Rect<int>& bounds) const
{
	if(!handle)
		return;

	int toolbarheight = GetToolbarAndMenuHeight();
	int captionheight = GetCaptionHeight();

	RECT rect;
	WINDOWPLACEMENT wp;
	GetWindowPlacement(handle, &wp);
	if(wp.showCmd == SW_SHOWMINIMIZED)
	{
		rect = wp.rcNormalPosition;

		bounds.x = rect.left + GetSystemMetrics(SM_CXSIZEFRAME);
		bounds.y = rect.top + toolbarheight + GetSystemMetrics(SM_CYSIZEFRAME) + captionheight;
		bounds.width = rect.right - rect.left - 2 * GetSystemMetrics(SM_CXSIZEFRAME);
		bounds.height = rect.bottom - rect.top - toolbarheight - 2 * GetSystemMetrics(SM_CYSIZEFRAME) - captionheight;
	}
	else
	{
		GetWindowRect(handle, &rect);

		bounds.x = rect.left + GetSystemMetrics(SM_CXSIZEFRAME);
		bounds.y = rect.top + toolbarheight + GetSystemMetrics(SM_CYSIZEFRAME) + captionheight;
		bounds.width = rect.right - rect.left - 2 * GetSystemMetrics(SM_CXSIZEFRAME);
		bounds.height = rect.bottom - rect.top - toolbarheight - 2 * GetSystemMetrics(SM_CYSIZEFRAME) - captionheight;

		if(parent)
		{
			// Make relative to parent position for child windows
			GetWindowRect(parent->handle, &rect);
			toolbarheight = parent->GetToolbarAndMenuHeight();
			captionheight = parent->GetCaptionHeight();
			bounds.x -= rect.left + GetSystemMetrics(SM_CXSIZEFRAME);
			bounds.y -= rect.top + toolbarheight + GetSystemMetrics(SM_CYSIZEFRAME) + captionheight;

			// Adjust size returned by GetWindowRect() for maximized child windows
			if(wp.showCmd == SW_SHOWMAXIMIZED)
				bounds.height += toolbarheight + GetToolbarAndMenuHeight();
		}
	}
}

//-----------------------------------------------------------------------------
// Name: SetBoundsInternal()
// Desc: Set bounds using MoveWindow()
//-----------------------------------------------------------------------------
void WinForm::SetBoundsInternal(const Rect<int>& bounds)
{
	if(!handle)
		return;

	MoveWindow(handle, bounds.x, bounds.y, bounds.width, bounds.height, visible);
}
void WinForm::SetClientBoundsInternal(const Rect<int>& bounds)
{
	if(!handle)
		return;

	int toolbarheight = GetToolbarAndMenuHeight();
	int captionheight = GetCaptionHeight();
	int nonclientleft = bounds.x - GetSystemMetrics(SM_CXSIZEFRAME);
	int nonclienttop = bounds.y - toolbarheight - GetSystemMetrics(SM_CYSIZEFRAME) - captionheight;
	int totalwidth = bounds.width + 2 * GetSystemMetrics(SM_CXSIZEFRAME);
	int totalheight = bounds.height + toolbarheight + 2 * GetSystemMetrics(SM_CYSIZEFRAME) + captionheight;
	MoveWindow(handle, nonclientleft, nonclienttop, totalwidth, totalheight, visible);
}

//-----------------------------------------------------------------------------
// Name: GetWindowState()
// Desc: Get window state using GetWindowPlacement()
//-----------------------------------------------------------------------------
WindowState WinForm::GetWindowState()
{
	WINDOWPLACEMENT wp;
	GetWindowPlacement(handle, &wp);

	WindowState wndstate;
	if(wp.showCmd == SW_SHOWMINIMIZED)
		wndstate = WS_MINIMIZED;
	else if(wp.showCmd == SW_SHOWMAXIMIZED)
		wndstate = WS_MAXIMIZED;
	else
		wndstate = WS_NORMAL;
	return wndstate;
}

//-----------------------------------------------------------------------------
// Name: GetNormalBounds()
// Desc: Get bounds of the unmaximized client area using GetWindowPlacement()
//-----------------------------------------------------------------------------
void WinForm::GetNormalBoundsInternal(Rect<int>& bounds) const
{
	if(!handle)
		return;

	WINDOWPLACEMENT wp;
	GetWindowPlacement(handle, &wp);
	bounds.x = wp.rcNormalPosition.left;
	bounds.y = wp.rcNormalPosition.top;
	bounds.width = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	bounds.height = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
}
void WinForm::GetNormalClientBoundsInternal(Rect<int>& bounds) const
{
	if(!handle)
		return;

	int captionheight = GetCaptionHeight();

	WINDOWPLACEMENT wp;
	GetWindowPlacement(handle, &wp);
	bounds.x = wp.rcNormalPosition.left + GetSystemMetrics(SM_CXSIZEFRAME);
	bounds.y = wp.rcNormalPosition.top + GetSystemMetrics(SM_CYSIZEFRAME) + captionheight;
	bounds.width = wp.rcNormalPosition.right - wp.rcNormalPosition.left - 2 * GetSystemMetrics(SM_CXSIZEFRAME);
	bounds.height = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top - 2 * GetSystemMetrics(SM_CYSIZEFRAME) - captionheight;
}

//-----------------------------------------------------------------------------
// Name: GetToolbarAndMenuHeight()
// Desc: Return the total height of all toolbars and the title menu
//-----------------------------------------------------------------------------
int WinForm::GetToolbarAndMenuHeight() const
{
	int toolbarheight = 0;
	std::list<Toolbar*>::const_iterator iter;
	LIST_FOREACH(toolbars, iter)
		toolbarheight += (*iter)->GetSize().height;
	if(titlemenu)
		toolbarheight += GetSystemMetrics(SM_CYMENU);
	return toolbarheight;
}

//-----------------------------------------------------------------------------
// Name: GetCaptionHeight()
// Desc: Return the height of this windows's caption
//-----------------------------------------------------------------------------
int WinForm::GetCaptionHeight() const
{
	return GetWindowLongPtr(handle, GWL_STYLE) & WS_THICKFRAME && !(GetWindowLongPtr(handle, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) ?
		GetSystemMetrics(SM_CYCAPTION) : GetSystemMetrics(SM_CYSMCAPTION);
}

//-----------------------------------------------------------------------------
// Name: SetWindowState()
// Desc: Set window state using ShowWindow()
//-----------------------------------------------------------------------------
WindowState WinForm::SetWindowState(WindowState val)
{
	int cmdshow;
	if(val == WS_MINIMIZED)
		cmdshow = SW_MINIMIZE;
	else if(val == WS_MAXIMIZED)
		cmdshow = SW_MAXIMIZE;
	else // val == WS_NORMAL
		cmdshow = SW_RESTORE;
	ShowWindow(handle, cmdshow);
	return val;
}

//-----------------------------------------------------------------------------
// Name: GetFormBorderStyle()
// Desc: Get form border style using GetWindowLongPtr()
//-----------------------------------------------------------------------------
FormBorderStyle WinForm::GetFormBorderStyle()
{
	LONG style = GetWindowLongPtr(handle, GWL_STYLE);
	style &= FBS_DEFAULT; // Strip all styles that aren't related to the frame
	return (FormBorderStyle)style;
}

//-----------------------------------------------------------------------------
// Name: SetFormBorderStyle()
// Desc: Set window state using SetWindowLongPtr()
//-----------------------------------------------------------------------------
FormBorderStyle WinForm::SetFormBorderStyle(FormBorderStyle val)
{
	LONG style = GetWindowLongPtr(handle, GWL_STYLE);
	style &= ~FBS_DEFAULT; // Keep all styles unchanged except the ones related to the frame
	style |= (LONG)val; // Add new styles
	SetWindowLongPtr(handle, GWL_STYLE, style);
	return val;
}

//-----------------------------------------------------------------------------
// Name: GetMaximizeBox()
// Desc: Return whether the maximize box is displayed
//-----------------------------------------------------------------------------
bool WinForm::GetMaximizeBox()
{
	LONG style = GetWindowLongPtr(handle, GWL_STYLE);
	return (style & WS_MAXIMIZEBOX) != 0;
}

//-----------------------------------------------------------------------------
// Name: SetMaximizeBox()
// Desc: Set whether the maximize box is displayed
//-----------------------------------------------------------------------------
bool WinForm::SetMaximizeBox(bool hasmaximizebox)
{
	LONG style = GetWindowLongPtr(handle, GWL_STYLE);
	style &= ~WS_MAXIMIZEBOX; // Keep all styles unchanged except WS_MAXIMIZEBOX
	if(hasmaximizebox)
		style |= WS_MAXIMIZEBOX; // Add WS_MAXIMIZEBOX style
	SetWindowLongPtr(handle, GWL_STYLE, style);
	return hasmaximizebox;
}

//-----------------------------------------------------------------------------
// Name: GetMinimizeBox()
// Desc: Return whether the maximize box is displayed
//-----------------------------------------------------------------------------
bool WinForm::GetMinimizeBox()
{
	LONG style = GetWindowLongPtr(handle, GWL_STYLE);
	return (style & WS_MINIMIZEBOX) != 0;
}

//-----------------------------------------------------------------------------
// Name: SetMinimizeBox()
// Desc: Set whether the maximize box is displayed
//-----------------------------------------------------------------------------
bool WinForm::SetMinimizeBox(bool hasmaximizebox)
{
	LONG style = GetWindowLongPtr(handle, GWL_STYLE);
	style &= ~WS_MINIMIZEBOX; // Keep all styles unchanged except WS_MINIMIZEBOX
	if(hasmaximizebox)
		style |= WS_MINIMIZEBOX; // Add WS_MINIMIZEBOX style
	SetWindowLongPtr(handle, GWL_STYLE, style);
	return hasmaximizebox;
}

#pragma region DragDrop
//-----------------------------------------------------------------------------
// Name: EnableDragDrop()
// Desc: Enable catching of dragdrop events
//-----------------------------------------------------------------------------
Result WinForm::EnableDragDrop()
{
	if(dragdropenabled)
		return R_OK;

	Result rlt;

	CHKRESULT(RegisterDragDrop(handle, this));

	dragdropenabled = true;
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: DisableDragDrop()
// Desc: Enable catching of dragdrop events
//-----------------------------------------------------------------------------
Result WinForm::DisableDragDrop()
{
	if(!dragdropenabled)
		return R_OK;

	Result rlt;

	if(IsWindowVisible(handle))
		CHKRESULT(RevokeDragDrop(handle));

	dragdropenabled = false;
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: DragEnter()
// Desc: Callback, called on mouse enter with dragdrop data pending
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE WinForm::DragEnter(IDataObject* dataobj, DWORD keystate, POINTL mousepos, DWORD* effect)
{
	dragenterobj = dataobj;
	if(!effect)
		return E_INVALIDARG;

	if(dragentercbk)
		dragentercbk(dataobj, keystate, Point<int>(mousepos.x, mousepos.y), effect, dragentercbkuser);
	olddrageffect = *effect; // Save drag effect
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DragOver()
// Desc: Callback, called on mouse move and mouse hover with dragdrop data pending
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE WinForm::DragOver(DWORD keystate, POINTL mousepos, DWORD* effect)
{
	if(!effect)
		return E_INVALIDARG;

	*effect = olddrageffect; // Retrieve past drag effect
	if(dragovercbk)
		dragovercbk(dragenterobj, keystate, Point<int>(mousepos.x, mousepos.y), effect, dragovercbkuser);
	olddrageffect = *effect; // Save drag effect
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DragLeave()
// Desc: Callback, called on mouse leave or mouse up with DROPEFFECT_NONE and dragdrop data pending
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE WinForm::DragLeave()
{
	if(dragleavecbk)
		dragleavecbk(dragleavecbkuser);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Drop()
// Desc: Callback, called on mouse up without DROPEFFECT_NONE and dragdrop data pending
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE WinForm::Drop(IDataObject* dataobj, DWORD keystate, POINTL mousepos, DWORD* effect)
{
	if(!effect)
		return E_INVALIDARG;

	*effect = olddrageffect; // Retrieve past drag effect
	if(dropcbk)
		dropcbk(dataobj, keystate, Point<int>(mousepos.x, mousepos.y), effect, dropcbkuser);
	return S_OK;
}
#pragma endregion

//-----------------------------------------------------------------------------
// Name: CreateTitleMenu()
// Desc: Create a title menu and attach it to the form
//-----------------------------------------------------------------------------
Result WinForm::CreateTitleMenu(IMenu** titlemenu)
{
	*titlemenu = NULL;
	Result rlt;

	// Remove old context menu
	RELEASE(this->titlemenu);

	Menu* newtitlemenu;
	CHKALLOC(newtitlemenu = new Menu(parentform, Menu::MT_TITLEMENU));

	parentform->menus.push_back(newtitlemenu);
	*titlemenu = this->titlemenu = newtitlemenu;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: GetTooltip()
// Desc: Create a tooltip if nonexistant and return its pointer
//-----------------------------------------------------------------------------
LPTOOLTIP WinForm::GetTooltip()
{
	return tooltip ? tooltip: tooltip = new Tooltip(handle);
}

void WinForm::OnMouseDown(const MouseEventArgs& args)
{
	RaiseMouseDownCbk(args);
}
void WinForm::OnMouseUp(const MouseEventArgs& args)
{
	RaiseMouseUpCbk(args);
}
void WinForm::OnMouseMove(const MouseEventArgs& args)
{
	if(!tracker.enabled)
		if(TrackMouseEvent(&tracker.tracker))
		{
			tracker.enabled = true;
			// Mouse enter
		}

	RaiseMouseMoveCbk(args);
}
void WinForm::OnMouseDoubleClick(const MouseEventArgs& args)
{
	RaiseMouseDClickCbk(args);
}
void WinForm::OnMouseWheel(const MouseEventArgs& args)
{
	RaiseMouseWheelCbk(args);
}
void WinForm::OnMouseLeave(const MouseEventArgs& args)
{
	tracker.enabled = false;
}
void WinForm::OnPaint()
{
	RaisePaintCbk();
}
void WinForm::OnButtonMessage(HWND handle, WORD notifycode)
{
	std::list<Control*>::iterator iter;
	LIST_FOREACH(controls, iter)
		if((*iter)->handle == handle)
		{
			(*iter)->OnNotifyMessage(notifycode);
			return;
		}
}
void WinForm::OnMenuMessage(WORD menuid) // (This gets also called when a toolbar item is clicked!)
{
	std::list<Menu*>::iterator iter;
	UINT i;
	LIST_FOREACH_I(menus, iter, i)
		if(i + 1 == menuid >> 8)
		{
			(*iter)->CallOnMenuItemClicked(menuid);
			break;
		}
}
void WinForm::OnTrayIconMessage(LPARAM lparam)
{
	switch(LOWORD(lparam))
	{
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		{
			std::list<TrayIcon*>::iterator iter;
			UINT i;
			LIST_FOREACH_I(trayicons, iter, i)
				if(i == HIWORD(lparam))
				{
					(*iter)->ShowContextMenu();
					break;
				}
		}
	}
}
void WinForm::OnNotifyMessage(const NMHDR* nmhdr)
{
	std::list<Toolbar*>::iterator titer;
	LIST_FOREACH(toolbars, titer)
		if((*titer)->handle == nmhdr->hwndFrom)
		{
			(*titer)->OnNotifyMessage((NMTOOLBAR*)nmhdr);
			return;
		}

	std::list<NumericUpDown*>::iterator niter;
	LIST_FOREACH(nuds, niter)
		if((*niter)->handle == nmhdr->hwndFrom)
		{
			(*niter)->OnNotifyMessage((LPNMUPDOWN)nmhdr);
			return;
		}

	std::list<Control*>::iterator citer;
	LIST_FOREACH(controls, citer)
		if((*citer)->handle == nmhdr->hwndFrom)
		{
			(*citer)->OnNotifyMessage(nmhdr->code);
			return;
		}

	if(tooltip && tooltip->handle == nmhdr->hwndFrom)
		tooltip->OnNotifyMessage(nmhdr->code, nmhdr);
}
void WinForm::OnFormSizing(Rect<int>* bounds, byte sizeedge)
{
	// Call user callbacks
	if(fresizecbk)
		fresizecbk(this, bounds, sizeedge, fresizecbkuser);
}
void WinForm::OnFormMoving(Rect<int>* bounds)
{
	// Call user callbacks
	if(fmovingcbk)
		fmovingcbk(this, bounds, fmovingcbkuser);
}
void WinForm::OnFormEnterSizeMove()
{
	GetBoundsInternal(entersizemovebounds);
}
void WinForm::OnFormExitSizeMove()
{
	Rect<int> bounds;
	GetBoundsInternal(bounds);

	// Call user callbacks
	if(bounds.size != entersizemovebounds.size)
	{
		if(fsizechangedcbk)
			fsizechangedcbk(this, bounds.size, fsizechangedcbkuser);
	}
	else
	{
		if(fmovedcbk)
			fmovedcbk(this, bounds.location, fmovedcbkuser);
	}
}
void WinForm::OnContextMenuMessage()
{
	// Get mouse position
	POINT pt;
	GetCursorPos(&pt);

	// Get window at position pt
	std::list<Control*>::iterator iter;
	LIST_FOREACH(controls, iter)
	{
		if((*iter)->bounds.Contains(pt.x, pt.y))
		{
			// Show the context menu for this control
			if((*iter)->ContextMenuUsed())
				(*iter)->ShowContextMenu();
			return;
		}
	}

	// If no window is at pt, show the context menu for the form
	if(ContextMenuUsed())
		ShowContextMenu();
}

//-----------------------------------------------------------------------------
// Name: Close()
// Desc: Send message to remove the form
//-----------------------------------------------------------------------------
void WinForm::Close(bool* cancel)
{
	if(cancel)
		*cancel = (SendMessage(handle, WM_CLOSE, 0, 0) == 1);
	else
		SendMessage(handle, WM_CLOSE, 0, 0);
}

//-----------------------------------------------------------------------------
// Name: ~WinForm()
// Desc: Destructor
//-----------------------------------------------------------------------------
WinForm::~WinForm()
{
	if(parent)
		parent->childforms.remove(this);

	std::list<Control*>::iterator iter;
	LIST_FOREACH(controls, iter)
		(*iter)->parentform = NULL;

	while(menus.size())
		menus.front()->Release();
	while(trayicons.size())
		trayicons.front()->Release();
	while(toolbars.size())
		toolbars.front()->Release();
	while(nuds.size())
		nuds.front()->Release();

	RemovePyCallbacks();

	//RemoveAllControls();
	DisableDragDrop();
	//DestroyWindow(handle); // Closing an MDI child over the MDI close button does not remove MDI buttons if the window is destroyed manually!

	if(tooltip)
		tooltip->Release();
}