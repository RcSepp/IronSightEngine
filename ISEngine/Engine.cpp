#include "ISEngine.h"
#ifdef _WIN32
#include "ISForms.h"
#include "ISGuiFactory.h"
#endif

std::vector<CUSTOMWNDPROC> Engine::customwndprocs;
std::vector<LPVOID> Engine::customwndprocsuser;

//-----------------------------------------------------------------------------
// Name: Engine()
// Desc: Constructor
//-----------------------------------------------------------------------------
#ifdef _WIN32
Engine::Engine(HINSTANCE instance) : cyclicfunc(NULL), _cyclicfunc(NULL)
{
	this->instance = instance;
	cle = new Console(); // Create console here, since this is usually the first function to ever be called in the application
}
#else
Engine::Engine() : cyclicfunc(NULL), _cyclicfunc(NULL)
{
	cle = new Console(); // Create console here, since this is usually the first function to ever be called in the application
}
#endif

//-----------------------------------------------------------------------------
// Name: FindDLL()
// Desc: Searches for the specified DLL in the same directories LoadLibrary() would and returns true if it exists
//-----------------------------------------------------------------------------
bool Engine::FindDLL(const String& dllname, LPTSTR* missingdllname)
{
#ifdef _WIN32
	if(SearchPath(NULL, dllname.ToCharString(), NULL, 0, NULL, NULL))
	{
		if(missingdllname)
			*missingdllname = NULL;
		return true;
	}
	else
	{
		if(missingdllname)
			*missingdllname = dllname.CopyChars();
		return false;
	}
#else
	//EDIT: Adapt for other platforms
	if(missingdllname)
		*missingdllname = NULL;
	return true;
#endif
}

//-----------------------------------------------------------------------------
// Name: GetConsolePtr()
// Desc: Sync the given pointer with this module's cle
//-----------------------------------------------------------------------------
void Engine::GetConsolePtr(SyncedPtr<Console>& execle)
{
	execle = cle;
}

#ifdef _WIN32
#pragma region Win
//-----------------------------------------------------------------------------
// Name: EngWndProc()
// Desc: The engine's message handler
//-----------------------------------------------------------------------------
LRESULT CALLBACK Engine::EngWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	DWORD i;

	switch(msg)
	{
	//case WM_CREATE:
		//if(fms)
		//	fms->OnCreate();
		//break;

	case WM_GETMINMAXINFO:
		{
			MINMAXINFO* minmaxinfo = (MINMAXINFO*)lparam;
			minmaxinfo->ptMaxSize.y = minmaxinfo->ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN) + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);
		}
		break;

	case WM_CLOSE:
		if(fms)
		{
			/*LRESULT rlt = fms->OnFormClosing(wnd);
			if(rlt != -1)
				return rlt;*/
			fms->OnFormClosing(wnd);
		}
	    break;

	case WM_TIMER:
		{
			Timer* tmr = Timer::FromId(wparam);
			tmr->cbk(tmr->user);
			if(!tmr->autoreset)
				eng->RemoveTimer(tmr);
		}
	    break;

	case WM_SIZE:
		if(gui)
			gui->OnResize(wnd, Size<UINT>(LOWORD(lparam), HIWORD(lparam)));
		if(fms)
		{
			WindowState wndstate;
			if(wparam & SIZE_MINIMIZED)
				wndstate = WS_MINIMIZED;
			else if(wparam & SIZE_MAXIMIZED)
				wndstate = WS_MAXIMIZED;
			else
				wndstate = WS_NORMAL;
			if(fms->OnFormResize(wnd, Size<UINT>(LOWORD(lparam), HIWORD(lparam)), wndstate))
				return TRUE;
		}
		break;

	case WM_SIZING:
		if(fms)
		{
			LPRECT rect = (LPRECT)lparam;
			Rect<int> windowbounds(rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top);
			fms->OnFormSizing(wnd, &windowbounds, (BYTE)wparam);
			rect->left = windowbounds.left();
			rect->right = windowbounds.right();
			rect->top = windowbounds.top();
			rect->bottom = windowbounds.bottom();
			return TRUE;
		}
		break;

	case WM_MOVING:
		if(fms)
		{
			LPRECT rect = (LPRECT)lparam;
			Rect<int> windowbounds(rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top);
			fms->OnFormMoving(wnd, &windowbounds);
			rect->left = windowbounds.left();
			rect->right = windowbounds.right();
			rect->top = windowbounds.top();
			rect->bottom = windowbounds.bottom();
			return TRUE;
		}
		break;

	case WM_ENTERSIZEMOVE:
		if(fms)
			fms->OnFormEnterSizeMove(wnd);
		break;

	case WM_EXITSIZEMOVE:
		if(fms)
			fms->OnFormExitSizeMove(wnd);
		break;

	case WM_HSCROLL:
		if(gui)
		{
			if(LOWORD(wparam) == SB_THUMBTRACK)
			{
				SCROLLINFO scrifo;
				scrifo.cbSize = sizeof(SCROLLINFO);
				scrifo.fMask = SIF_POS;
				scrifo.nPos = HIWORD(wparam);
				SetScrollInfo(wnd, SB_HORZ, &scrifo, TRUE);
				gui->OnHScrolling(wnd, (short)HIWORD(wparam));
				return 0;
			}
			else if(LOWORD(wparam) == SB_THUMBPOSITION)
			{
				gui->OnHScrolled(wnd, (short)HIWORD(wparam));
				return 0;
			}
		}
		break;

	case WM_VSCROLL:
		if(gui)
		{
			if(LOWORD(wparam) == SB_THUMBTRACK)
			{
				SCROLLINFO scrifo;
				scrifo.cbSize = sizeof(SCROLLINFO);
				scrifo.fMask = SIF_POS;
				scrifo.nPos = HIWORD(wparam);
				SetScrollInfo(wnd, SB_VERT, &scrifo, TRUE);
				gui->OnVScrolling(wnd, (short)HIWORD(wparam));
				return 0;
			}
			else if(LOWORD(wparam) == SB_THUMBPOSITION)
			{
				gui->OnVScrolled(wnd, (short)HIWORD(wparam));
				return 0;
			}
		}
		break;

	case WM_PAINT:
	case WM_DISPLAYCHANGE:
		if(fms)
			fms->OnPaint(wnd);
		if(gui)
		{
			PAINTSTRUCT ps;
			BeginPaint(wnd, &ps);
			bool painthandled = false;
			gui->OnPaint(wnd, ps.hdc, ps.rcPaint, painthandled);
			EndPaint(wnd, &ps);
			if(painthandled)
				return 0; // EDIT: Needed for immediate repainting if InvalidateRect gets called inside OnPaint()
		}
		break;

	case WM_SETFOCUS:
		if(fms)
			fms->CallOnGainFocus(wnd);
		break;

	case WM_KILLFOCUS:
		if(fms)
			fms->CallOnLoseFocus(wnd);
		break;

	case WM_KEYDOWN:
		if(fms)
			fms->CallOnKeyDown(wparam);
		break;

	case WM_KEYUP:
		if(fms)
			fms->CallOnKeyUp(wparam);
		break;

	case WM_LBUTTONDOWN: case WM_MBUTTONDOWN: case WM_RBUTTONDOWN:
		SetCapture(wnd);
		if(fms)
			fms->OnMouseDown(wnd, MouseEventArgs(msg, wparam, lparam));
		if(gui)
			gui->OnMouseDown(wnd, MouseEventArgs(msg, wparam, lparam));
		break;

	case WM_LBUTTONUP: case WM_MBUTTONUP: case WM_RBUTTONUP:
		if((wparam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)) == 0)
			ReleaseCapture();
		if(fms)
			fms->OnMouseUp(wnd, MouseEventArgs(msg, wparam, lparam));
		if(gui)
			gui->OnMouseUp(wnd, MouseEventArgs(msg, wparam, lparam));
		break;

	case WM_LBUTTONDBLCLK: case WM_MBUTTONDBLCLK: case WM_RBUTTONDBLCLK:
		if(fms)
			fms->OnMouseDoubleClick(wnd, MouseEventArgs(msg, wparam, lparam));
		if(gui)
			gui->OnMouseDoubleClick(wnd, MouseEventArgs(msg, wparam, lparam));
		break;

	case WM_MOUSEWHEEL:
		if(fms)
			fms->OnMouseWheel(wnd, MouseEventArgs(msg, wparam, lparam));
		break;

	case WM_MOUSEMOVE:
		{
			if(fms)
				fms->OnMouseMove(wnd, MouseEventArgs(msg, wparam, lparam));
			if(gui)
				gui->OnMouseMove(wnd, MouseEventArgs(msg, wparam, lparam));
			break;
		}

	case WM_MOUSELEAVE:
		{
			if(fms)
				fms->OnMouseLeave(wnd, MouseEventArgs(msg, wparam, lparam));
			break;
		}

	case WM_COMMAND:
		if(fms)
		{
			if(lparam == 0)
			{
				if(HIWORD(wparam) == 0)
					fms->OnMenuMessage(wnd, LOWORD(wparam));
			}
			else
				fms->OnButtonMessage(wnd, (HWND)lparam, HIWORD(wparam));
		}

	case WM_TRAYICONMSG:
		if(fms)
			fms->OnTrayIconMessage(wnd, lparam);
		break;

	case WM_NOTIFY:
		if(fms)
			fms->OnNotifyMessage(wnd, (NMHDR*)lparam);
		break;

	case WM_CONTEXTMENU:
		if(fms)
			fms->OnFormContextMenuMessage(wnd);
		break;
	}

    // Call custom window procedures, exit if one of them returns false
	for(i = 0; i < customwndprocs.size(); i++)
		if(!customwndprocs[i](wnd, msg, wparam, lparam, customwndprocsuser[i]))
			return 1;

	return 0x7FFFFFFF;
}
LRESULT CALLBACK Engine::EngNonChildWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = EngWndProc(wnd, msg, wparam, lparam);
	//return result == 0x7FFFFFFF ? DefWindowProc(wnd, msg, wparam, lparam) : result;

	if(result != 0x7FFFFFFF)
		return result;
	for(int i = 0; i < fms->GetNumForms(); i++)
		if(fms->GetHwnd(i) == wnd)
		{
			if(fms->GetMDIClientHwnd(i))
				return DefFrameProc(wnd, fms->GetMDIClientHwnd(i), msg, wparam, lparam);
			else
				return DefWindowProc(wnd, msg, wparam, lparam);
		}
	return DefWindowProc(wnd, msg, wparam, lparam);
}
LRESULT CALLBACK Engine::EngChildWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = EngWndProc(wnd, msg, wparam, lparam);
	return result == 0x7FFFFFFF ? DefMDIChildProc(wnd, msg, wparam, lparam) : result;
}

//-----------------------------------------------------------------------------
// Name: RegisterWinClass()
// Desc: Registered window class
//-----------------------------------------------------------------------------
Result Engine::RegisterWinClass(const String* classname, HBRUSH bgbrush, bool ismdichildclass, HICON icon, HICON icon_small)
{
	WNDCLASSEX winClass;

	bool usedefaultclassname = (classname == NULL);
	if(usedefaultclassname)
		classname = new String(DEFAULT_WINDOWCLASS);

	// Remove old WinForms and unregister old window class
	std::list<String>::iterator iter;
	LIST_FOREACH(wndclasses, iter)
		if(*iter == *classname)
			UnregisterWinClass(classname);

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.lpszClassName = classname->ToCharString();
	winClass.lpfnWndProc = ismdichildclass ? EngChildWndProc : EngNonChildWndProc;
	winClass.style = bgbrush ? CS_VREDRAW | CS_HREDRAW : NULL;
	winClass.hInstance = instance;
	winClass.hIcon = icon;
	winClass.hIconSm = icon_small;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = bgbrush;
	winClass.lpszMenuName = NULL;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;

	if(!RegisterClassEx(&winClass))
		return ERR("Error registering window class");
	wndclasses.push_back(*classname);

	if(usedefaultclassname)
		delete classname;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: RegisterCustomWndProc()
// Desc: Register a custom window procedure to be called after the engines window procedure
//-----------------------------------------------------------------------------
Result Engine::RegisterCustomWndProc(CUSTOMWNDPROC customwndproc, LPVOID user)
{
	customwndprocs.push_back(customwndproc);
	customwndprocsuser.push_back(user);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: UnregisterCustomWndProc()
// Desc: Unregister the given window procedure
//-----------------------------------------------------------------------------
Result Engine::UnregisterCustomWndProc(CUSTOMWNDPROC customwndproc)
{
	DWORD i;

	// Find index of customwndproc
	for(i = 0; i < customwndprocs.size(); i++)
		if(customwndprocs[i] == customwndproc)
			break;
	if(i == customwndprocs.size())
		return R_OK;

	// Shift procs after customwndproc to the front
	for(i++; i < customwndprocs.size(); i++)
	{
		customwndprocs[i - 1] = customwndprocs[i];
		customwndprocsuser[i - 1] = customwndprocsuser[i];
	}

	customwndprocs.pop_back();
	customwndprocsuser.pop_back();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: UnregisterWinClass()
// Desc: Remove WinForms and unregister window class
//-----------------------------------------------------------------------------
void Engine::UnregisterWinClass(const String* classname)
{
	bool usedefaultclassname = (classname == NULL);
	const String* cname = usedefaultclassname ? new String(DEFAULT_WINDOWCLASS) : classname;

	if(fms)
	{
		TCHAR formclassname[64];
		for(int i = 0; i < fms->GetNumForms(); i++)
			if(GetClassName(fms->GetHwnd(i), formclassname, 64) && *cname == String(formclassname) == 0)
				fms->CloseForm(i);
	}

	UnregisterClass(cname->ToCharString(), instance);
	wndclasses.remove(*cname);

	if(usedefaultclassname)
		delete cname;
}

//-----------------------------------------------------------------------------
// Name: IsWinClassRegistered()
// Desc: Returns true if the given window class has already been registered
//-----------------------------------------------------------------------------
bool Engine::IsWinClassRegistered(const String* classname)
{
	bool result = false;

	bool usedefaultclassname = (classname == NULL);
	if(usedefaultclassname)
		classname = new String(DEFAULT_WINDOWCLASS);

	std::list<String>::iterator iter;
	LIST_FOREACH(wndclasses, iter)
		if(*iter == *classname)
		{
			result = true;
			break;
		}

	if(usedefaultclassname)
		delete classname;

	return result;
}
#pragma endregion
#endif

//-----------------------------------------------------------------------------
// Name: Run()
// Desc: Enter the main application loop
//-----------------------------------------------------------------------------
Result Engine::Run(INITFUNC initfunc, INITFUNC postinitfunc, CYCLICFUNC cyclicfunc, QUITFUNC quitfunc)
{
	static bool enginerunning = false;
	Result rlt;

	if(enginerunning)
		return ERR("Engine already running");

	enginerunning = true;

	if(initfunc)
		IFFAILED(initfunc())
		{
			quitfunc();
			enginerunning = false;
			return rlt;
		}

#ifdef _WIN32
	if(fms)
		fms->ShowAll();
#endif

	if(postinitfunc)
		IFFAILED(postinitfunc())
		{
			quitfunc();
			enginerunning = false;
			return rlt;
		}

	// Set cyclicfunc after running initfunc and postinitfunc so that cyclicfunc isn't called if Wait() or Work() are called inside an initialization function
	this->cyclicfunc = this->_cyclicfunc ? this->_cyclicfunc : cyclicfunc;

	while(1)
	{
		if(this->cyclicfunc)
		{
			if(!Work())
				break;
		}
		else
		{
			if(!Wait())
				break;
		}
	}

	quitfunc();
	enginerunning = false;
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Work()
// Desc: Check event queue (non blocking). Return false to initiate application termination
//-----------------------------------------------------------------------------
bool Engine::Work()
{
	time.Step();

#ifdef _WIN32
	static MSG uMsg = {0};
	if(PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE))
	//if(GetMessage(&uMsg, NULL, 0, 0))
    {
        TranslateMessage(&uMsg);
        DispatchMessage(&uMsg);
    }

	if(uMsg.message == WM_QUIT)
	{
		PostQuitMessage(0); // Repost quit message to finish all pending waits
		return false;
	}
	else
	{
		if(this->cyclicfunc)
			this->cyclicfunc();
		return true;
	}
#else
	/*EDIT: Something like in Wait(), but using XCheckWindowEvent() instead of XNextEvent()*/
	return true;
#endif
}

//-----------------------------------------------------------------------------
// Name: Wait()
// Desc: Wait for events (blocking). Return false to initiate application termination
//-----------------------------------------------------------------------------
bool Engine::Wait()
{
	time.Step();

#ifdef _WIN32
	static MSG uMsg = {0};
	//if(PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE))
	if(GetMessage(&uMsg, NULL, 0, 0))
		if(!fms || !fms->GetMDIClientHwnd(0) || !TranslateMDISysAccel(fms->GetMDIClientHwnd(0), &uMsg))
		{
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		}

	if(uMsg.message == WM_QUIT)
	{
		PostQuitMessage(0); // Repost quit message to finish all pending waits
		return false;
	}
	else
		return true;
#else
	/*EDIT: Something like:
	static XEvent event;
	while(XPending(display) > 0){
		XNextEvent(display, &event);
	}*/
	return true;
#endif
}

//-----------------------------------------------------------------------------
// Name: WakeUp()
// Desc: Interrupt a wait, previously entered by Engine::Wait()
//-----------------------------------------------------------------------------
void Engine::WakeUp()
{
#ifdef _WIN32
	PostMessage(0, 0, 0, 0);
#else
	//EDIT
#endif
}

//-----------------------------------------------------------------------------
// Name: Timer
// Desc: Creates a new custom timer, that raises periodic events in the given interval [ms]
//-----------------------------------------------------------------------------
Result Engine::CreateTimer(TIMERELAPSED_CALLBACK timercbk, UINT interval, bool autoreset, LPVOID user, Timer** tmr)
{
#ifdef _WIN32
	Result rlt;

	*tmr = NULL;

	Timer* newtmr;
	CHKALLOC(newtmr = new Timer(timercbk, interval, autoreset, user));
	if(!SetTimer(fms->GetHwnd(0), newtmr->id(), interval, NULL))
	{
		TCHAR err[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, err, 256, NULL);
		return ERR(String("Error calling SetTimer(): ") << String(err));
	}
	timers.push_back(newtmr);
	*tmr = newtmr;

	return R_OK;
#else
	return ERR("Not implemented");
#endif
}

Result Engine::ResetTimer(Timer* tmr, UINT newinterval)
{
#ifdef _WIN32
	Result rlt;

	if(!SetTimer(fms->GetHwnd(0), tmr->id(), tmr->interval = newinterval, NULL))
	{
		TCHAR err[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, err, 256, NULL);
		return ERR(String("Error calling SetTimer(): ") << String(err));
	}

	return R_OK;
#else
	return ERR("Not implemented");
#endif
}

void Engine::RemoveTimer(Timer* tmr)
{
#ifdef _WIN32
	if(!fms)
		return;
	HWND wnd = fms->GetHwnd(0);
	if(!wnd)
		return;

	KillTimer(wnd, tmr->id());

	timers.remove(tmr);
	delete tmr;
#endif
}

//-----------------------------------------------------------------------------
// Name: ~Engine()
// Desc: Destructor
//-----------------------------------------------------------------------------
Engine::~Engine()
{
	for(std::list<Timer*>::iterator iter = timers.begin(); iter != timers.end(); iter++)
		delete *iter;
	timers.clear();

#ifdef _WIN32
	while(wndclasses.size())
		UnregisterWinClass(&wndclasses.front());

	customwndprocs.clear();
	customwndprocsuser.clear();
#endif
}
