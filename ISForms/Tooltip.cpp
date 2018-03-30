#include "Forms.h"


//-----------------------------------------------------------------------------
// Name: Tooltip()
// Desc: Constructor
//-----------------------------------------------------------------------------
Tooltip::Tooltip(HWND parentwnd) : parentwnd(parentwnd), showcbk(NULL), hidecbk(NULL)
{
//INITCOMMONCONTROLSEX icc;
//icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
//icc.dwICC = ICC_WIN95_CLASSES;
//InitCommonControlsEx(&icc);
	handle = CreateWindowEx(0, TOOLTIPS_CLASS, "", TTS_ALWAYSTIP, 0, 0, 0, 0, parentwnd, 0, fms->GetInstance(), 0);
}

//-----------------------------------------------------------------------------
// Name: OnNotifyMessage()
// Desc: Gets called by the parent form whenever a notication code is received through WM_COMMAND or WM_NOTIFY
//-----------------------------------------------------------------------------
void Tooltip::OnNotifyMessage(UINT notifycode, const NMHDR* nmhdr)
{
	switch(notifycode)
	{
	case TTN_SHOW:
		if(showcbk)
			showcbk(this, (HWND)nmhdr->idFrom, showcbkuser);
		break;

	case TTN_POP:
		if(hidecbk)
			hidecbk(this, (HWND)nmhdr->idFrom, hidecbkuser);
		break;

	case TTN_GETDISPINFO:
		{
			NMTTDISPINFO* dispinfo = (NMTTDISPINFO*)nmhdr;
			String tooltiptext = ((GETTOOLTIPTEXT_CALLBACK)dispinfo->lParam)(this, (HWND)dispinfo->hdr.idFrom);
			if(!tooltiptext.IsEmpty())
				dispinfo->lpszText = tooltiptext.CopyChars();
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// Name: Popup()
// Desc: Show tool tip at position of last mouse event
//-----------------------------------------------------------------------------
void Tooltip::Popup()
{
	SendMessage(handle, TTM_POPUP, NULL, NULL);
}

void Tooltip::SetTooltip(HWND wnd, const String& text)
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.uId = (UINT_PTR)wnd;
	ti.lpszText = text;
	ti.hwnd = parentwnd;
	SendMessage(handle, TTM_ADDTOOL, 0, (LPARAM)&ti);
}
void Tooltip::SetTooltip(HWND wnd, GETTOOLTIPTEXT_CALLBACK cbk)
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.uId = (UINT_PTR)wnd;
	ti.lpszText = LPSTR_TEXTCALLBACK;
	ti.hwnd = parentwnd;
	ti.lParam = (LPARAM)cbk;
	SendMessage(handle, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Tooltip::Release()
{
	delete this;
}