#include "Forms.h"


HINSTANCE fms_instance;

//-----------------------------------------------------------------------------
// Name: CreateForms()
// Desc: DLL API for creating an instance of Forms
//-----------------------------------------------------------------------------
__declspec(dllexport) LPFORMS __cdecl CreateForms()
{
	return new Forms();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void Forms::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
	if(pse)
		SyncWithPython();
}

//-----------------------------------------------------------------------------
// Name: Forms()
// Desc: Constructor
//-----------------------------------------------------------------------------
Forms::Forms()
{
	visualstylesenabled = false;

	// Create default font
	HDC hdc = GetDC(NULL);
	defaultfont = CreateFont(-MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
							 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
							 DEFAULT_PITCH | FF_DONTCARE, "Microsoft Sans Serif");
	ReleaseDC(NULL, hdc);
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Make local copies of the window properties given
//-----------------------------------------------------------------------------
Result Forms::Init(HINSTANCE instance)
{
	this->instance = fms_instance = instance;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateForm()
// Desc: Create a new instance of WinForm
//-----------------------------------------------------------------------------
Result Forms::CreateForm(FormSettings *settings, const String* classname, IWinForm** winform)
{
	Result rlt;

	WinForm* newform;
	CHKALLOC(newform = new WinForm());
	IFFAILED(newform->Init(settings, classname))
	{
		REMOVE(newform);
		return rlt;
	}

	forms.push_back(newform);
	if(winform)
		*winform = newform;

	CallOnCreateForm(newform->handle);

	return R_OK;
}
Result Forms::CreateForm(HWND handle, IWinForm** winform)
{
	Result rlt;

	WinForm* newform;
	CHKALLOC(newform = new WinForm());
	IFFAILED(newform->Init(handle))
	{
		REMOVE(newform);
		return rlt;
	}

	forms.push_back(newform);
	if(winform)
		*winform = newform;

	CallOnCreateForm(newform->handle);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CloseForm()
// Desc: Remove WinForm at the given index
//-----------------------------------------------------------------------------
void Forms::CloseForm(size_t index, bool* cancel)
{
	if(cancel)
		*cancel = false;

	if(index >= forms.size())
		return;

	if(cancel)
		*cancel = (SendMessage(forms[index]->handle, WM_CLOSE, 0, 0) == 1);
	else
		SendMessage(forms[index]->handle, WM_CLOSE, 0, 0);
}
void Forms::CloseFormByHwnd(HWND wnd, bool* cancel)
{
	if(cancel)
		*cancel = (SendMessage(wnd, WM_CLOSE, 0, 0) == 1);
	else
		SendMessage(wnd, WM_CLOSE, 0, 0);
}

//-----------------------------------------------------------------------------
// Name: ShowAll()
// Desc: Show all WinForms
//-----------------------------------------------------------------------------
void Forms::ShowAll()
{
	for(size_t i = 0; i < forms.size(); i++)
		forms[i]->Show();
}

//-----------------------------------------------------------------------------
// Name: CloseAll()
// Desc: Remove all WinForms
//-----------------------------------------------------------------------------
void Forms::CloseAll(bool* cancel)
{
	// Close forms in reverse order, so that parent forms get closed after their children
	size_t i = forms.size();
	if(cancel)
	{
		*cancel = false;
		while(i--)
		{
			bool c;
			forms[i]->Close(&c);
			if(c)
				*cancel = true;
		}
	}
	else
		while(i--)
			forms[i]->Close(NULL);
	forms.clear();
}

//-----------------------------------------------------------------------------
// Name: XXXVisualStates()
// Desc: Enable/Disable visual styles for every control in every WinForm
//-----------------------------------------------------------------------------
void Forms::EnableVisualStates()
{
	visualstylesenabled = true;

	std::list<Control*>::iterator itr;
	for(size_t i = 0; i < forms.size(); i++)
		LIST_FOREACH(forms[i]->controls, itr)
			(*itr)->UpdateVisualStyles();
}
void Forms::DisableVisualStates()
{
	visualstylesenabled = false;

	std::list<Control*>::iterator itr;
	for(size_t i = 0; i < forms.size(); i++)
		LIST_FOREACH(forms[i]->controls, itr)
			(*itr)->UpdateVisualStyles();
}

//-----------------------------------------------------------------------------
// Name: RegisterForXXX()
// Desc: Register cls to be called on corresponding events
//-----------------------------------------------------------------------------
void Forms::RegisterForFormDependence(IFormDependend* cls)
{
	frmdeplist.push_back(cls);
}
void Forms::RegisterForKeyboardMonitoring(IKeyboardMonitoring* cls)
{
	keymonlist.push_back(cls);
}

//-----------------------------------------------------------------------------
// Name: DeregisterFromXXX()
// Desc: Deregister cls from being called on corresponding events
//-----------------------------------------------------------------------------
void Forms::DeregisterFromFormDependence(IFormDependend* cls)
{
	frmdeplist.remove(cls);
}
void Forms::DeregisterFromKeyboardMonitoring(IKeyboardMonitoring* cls)
{
	keymonlist.remove(cls);
}

//-----------------------------------------------------------------------------
// Name: CallOnXXX()
// Desc: Fire events
//-----------------------------------------------------------------------------
void Forms::CallOnCreateForm(HWND wnd)
{
	std::list<IFormDependend*>::const_iterator frmdep_itr;
	LIST_FOREACH(frmdeplist, frmdep_itr)
		(*frmdep_itr)->OnCreateForm(wnd);
}
void Forms::CallOnRemoveForm(HWND wnd)
{
	std::list<IFormDependend*>::const_iterator frmdep_itr;
	LIST_FOREACH(frmdeplist, frmdep_itr)
		(*frmdep_itr)->OnRemoveForm(wnd);
}
void Forms::CallOnKeyDown(DWORD vkey)
{
	std::list<IKeyboardMonitoring*>::const_iterator keymon_itr;
	LIST_FOREACH(keymonlist, keymon_itr)
		(*keymon_itr)->OnKeyDown(vkey);
}
void Forms::CallOnKeyUp(DWORD vkey)
{
	std::list<IKeyboardMonitoring*>::const_iterator keymon_itr;
	LIST_FOREACH(keymonlist, keymon_itr)
		(*keymon_itr)->OnKeyUp(vkey);
}
void Forms::OnMouseDown(HWND wnd, const MouseEventArgs& args)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnMouseDown(args);
			return;
		}
}
void Forms::OnMouseUp(HWND wnd, const MouseEventArgs& args)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnMouseUp(args);
			return;
		}
}
void Forms::OnMouseMove(HWND wnd, const MouseEventArgs& args)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnMouseMove(args);
			return;
		}
}
void Forms::OnMouseDoubleClick(HWND wnd, const MouseEventArgs& args)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnMouseDoubleClick(args);
			return;
		}
}
void Forms::OnMouseWheel(HWND wnd, const MouseEventArgs& args)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			if(forms[i]->mdiclienthandle)
			{
				// Call OnMouseWheel() of top MDI child window
				wnd = GetTopWindow(forms[i]->mdiclienthandle);
				for(i = 0; i < forms.size(); i++)
					if(forms[i]->GetHwnd() == wnd)
					{
						forms[i]->OnMouseWheel(args);
						return;
					}
			}
			else
				// Call OnMouseWheel() of this window
				forms[i]->OnMouseWheel(args);
			return;
		}
}
void Forms::OnMouseLeave(HWND wnd, const MouseEventArgs& args)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnMouseLeave(args);
			return;
		}
}
void Forms::OnPaint(HWND wnd)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnPaint();
			return;
		}
}
void Forms::CallOnGainFocus(HWND wnd)
{
	std::list<IFormDependend*>::const_iterator frmdep_itr;
	LIST_FOREACH(frmdeplist, frmdep_itr)
		(*frmdep_itr)->OnGainFocus(wnd);
}
void Forms::CallOnLoseFocus(HWND wnd)
{
	std::list<IFormDependend*>::const_iterator frmdep_itr;
	LIST_FOREACH(frmdeplist, frmdep_itr)
		(*frmdep_itr)->OnLoseFocus(wnd);
}

//-----------------------------------------------------------------------------
// Name: OnFormClosing()
// Desc: Handle form closing. Should only be called on WM_CLOSE
//-----------------------------------------------------------------------------
LRESULT Forms::OnFormClosing(HWND wnd)
{
	// Get form from wnd
	WinForm* form = NULL;
	for(size_t i = forms.size() - 1; i >= 0; i--)
		if(forms[i]->GetHwnd() == wnd)
		{
			form = forms[i];
			break;
		}
	HWND parentwnd = form->parent ? form->parent->handle : NULL;//GetWindow(wnd, GW_OWNER);

	if(form)
	{
		// Fire user event
		if(form->fclosingcbk)
		{
			bool cancel = false;
			form->fclosingcbk(form, form->fclosingcbkuser, cancel);
			if(cancel) // If a user callback returned a cancel request
				return 1; // Prohibit application shutdown and report cancel
		}

		// Close and remove child forms before closing parents
		while(!form->childforms.empty())
		{
			bool cancel;
			CloseFormByHwnd(form->childforms.front()->handle, &cancel);
			if(cancel) // If a user callback returned a cancel request
				return 1; // Prohibit application shutdown and report cancel
		}

		// Inform form dependent classes
		CallOnRemoveForm(form->handle);

		/*// Close dialog window
		DestroyWindow(form->GetHwnd());*/ // Closing an MDI child over the MDI close button does not remove MDI buttons if the window is destroyed manually!

		// Fire user event
		if(form->fclosedcbk)
			form->fclosedcbk(form, form->fclosedcbkuser);

		// Remove form
		std::vector<WinForm*>::iterator iter;
		LIST_FOREACH(forms, iter)
			if(*iter == form)
			{
				forms.erase(iter);
				break;
			}
		delete form;
	}

	if(parentwnd) // If window is a dialog window
	{
		EnableWindow(parentwnd, true); // Reenable parent window
		SetActiveWindow(parentwnd); // Activate parent window
	}
			
	if(!forms.empty()) // If another form is still open
		return 0; // Prohibit application shutdown

	PostQuitMessage(0);

	return -1;
}
void Forms::OnMenuMessage(HWND wnd, WORD menuid)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnMenuMessage(menuid);
			return;
		}
}
void Forms::OnButtonMessage(HWND wnd, HWND handle, WORD notifycode)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnButtonMessage(handle, notifycode);
			return;
		}
}
void Forms::OnTrayIconMessage(HWND wnd, LPARAM lparam)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnTrayIconMessage(lparam);
			return;
		}
}
void Forms::OnNotifyMessage(HWND wnd, const NMHDR* nmhdr)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnNotifyMessage(nmhdr);
			return;
		}
}
bool Forms::OnFormResize(HWND wnd, Size<UINT> size, WindowState wndstate)
{
	bool skipdefaultwndproc = false;
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			// Fit mdi area to client area
			if(forms[i]->mdiclienthandle)
			{
				if(forms[i]->toolbars.empty())
					SetWindowPos(forms[i]->mdiclienthandle, NULL, 0, 0, size.width, size.height, SWP_NOMOVE | SWP_NOZORDER);
				else
				{
					int toolbarheight = 0;
					std::list<Toolbar*>::iterator iter;
					LIST_FOREACH(forms[i]->toolbars, iter)
						toolbarheight += (*iter)->GetSize().height;
					SetWindowPos(forms[i]->mdiclienthandle, NULL, 0, toolbarheight, size.width, size.height - toolbarheight, SWP_NOZORDER);
				}
			}

			// Call user callbacks
			if(forms[i]->fsizechangedcbk && forms[i]->oldwndstate != wndstate && wndstate != WS_MINIMIZED)
			{
				Size<int> nonclientsize(size.width, size.height);
				nonclientsize.width += 2 * GetSystemMetrics(SM_CXSIZEFRAME);
				nonclientsize.height += 2 * GetSystemMetrics(SM_CYSIZEFRAME) + forms[i]->GetCaptionHeight() + forms[i]->GetToolbarAndMenuHeight();
				forms[i]->fsizechangedcbk(forms[i], nonclientsize, forms[i]->fsizechangedcbkuser);
			}

			skipdefaultwndproc = true;
			forms[i]->oldwndstate = wndstate;
		}
	return skipdefaultwndproc;
}
void Forms::OnFormSizing(HWND wnd, Rect<int>* bounds, byte sizeedge)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnFormSizing(bounds, sizeedge);

			// If forms[i] is an mdi client, call OnFormSizing() also for maximized child windows
			if(forms[i]->mdiclienthandle)
				for(size_t j = 0; j < forms.size(); j++)
					if(forms[j]->parent && forms[j]->parent == forms[i] && forms[j]->GetWindowState() == WS_MAXIMIZED)
					{
						Rect<int> childbounds(*bounds);
						//EDIT: childbounds.location should also be adjusted
						childbounds.height += forms[j]->GetCaptionHeight() + forms[j]->GetToolbarAndMenuHeight()
											- forms[i]->GetCaptionHeight() - forms[i]->GetToolbarAndMenuHeight();
						forms[j]->OnFormSizing(&childbounds, sizeedge);
					}
			return;
		}
}
void Forms::OnFormMoving(HWND wnd, Rect<int>* bounds)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnFormMoving(bounds);
			return;
		}
}
void Forms::OnFormEnterSizeMove(HWND wnd)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnFormEnterSizeMove();
			return;
		}
}
void Forms::OnFormExitSizeMove(HWND wnd)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnFormExitSizeMove();
			return;
		}
}

void Forms::OnFormContextMenuMessage(HWND wnd)
{
	for(size_t i = 0; i < forms.size(); i++)
		if(forms[i]->GetHwnd() == wnd)
		{
			forms[i]->OnContextMenuMessage();
			return;
		}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Forms::Release()
{
	CloseAll(NULL);
	if(defaultfont)
	{
		DeleteObject(defaultfont);
		defaultfont = NULL;
	}

	delete this;
}