#include "Forms.h"


WNDPROC TabControl::BaseClassWndProc = NULL;
#define NUD_SUPERCLASSNAME "TabControl"

//-----------------------------------------------------------------------------
// Name: CreateTabControl()
// Desc: DLL API for creating an instance of a tabcontrol control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPTABCONTROL __cdecl CreateTabControl()
{
	return new TabControl();
}

//-----------------------------------------------------------------------------
// Name: TabControl()
// Desc: Constructor
//-----------------------------------------------------------------------------
TabControl::TabControl() : basecontrol(WC_TABCONTROL, NUD_SUPERCLASSNAME, TCS_FIXEDWIDTH, NULL, NULL, BaseClassWndProc, (WNDPROC)SuperClassWndProc, WS_EX_CLIENTEDGE)
{
	basecontrol.notifycbk = OnNotifyMessage;
	basecontrol.notifycbkuser = this;
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG TabControl::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	/*Control* ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
	if(ctrl)
		switch(msg)
		{
		case WM_LBUTTONDOWN:
			ctrl->RaiseMouseClickCbk();
			break;
		}*/
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}

void TabControl::InsertTab(const String& caption, int tabindex, LPVOID tag)
{
	TCITEM ti = {0};
	ti.mask = TCIF_TEXT;
	ti.pszText = caption;
	ti.cchTextMax = caption.length();
	tabindex = SendMessage(basecontrol.handle, TCM_INSERTITEM, tabindex, (LPARAM)&ti);

	if(tabindex == tags.size())
		tags.push_back(tag);
	else
	{
		std::vector<LPVOID>::iterator iter = tags.begin() + tabindex;
		/*for(int i = 0; i < tabindex; i++)
			iter++;*/
		tags.insert(iter, tag);
	}
}

Rect<>* TabControl::GetClientBounds(Rect<>* bounds)
{
	RECT rect = {0};
	rect.right = basecontrol.bounds.width;
	rect.bottom = basecontrol.bounds.height;
	TabCtrl_AdjustRect(basecontrol.handle, FALSE, &rect);

	*bounds = rect;
	return bounds;
}

int TabControl::GetCurrentTabIndex()
{
	return TabCtrl_GetCurSel(basecontrol.handle);
}
LPVOID TabControl::GetCurrentTabTag()
{
	return tags[TabCtrl_GetCurSel(basecontrol.handle)];
}
void TabControl::ChangeTab(int tabindex)
{
	TabCtrl_SetCurSel(basecontrol.handle, tabindex);
	if(tabchangedcbk)
		tabchangedcbk(this, tabindex, tabindex >= 0 ? tags[tabindex] : NULL, tabchangedcbkuser);
}
void TabControl::ChangeTab(LPVOID tabtag)
{
	for(size_t i = 0; i < tags.size(); i++)
		if(tabtag == tags[i])
		{
			ChangeTab(i);
			break;
		}
}

//-----------------------------------------------------------------------------
// Name: OnNotifyMessage()
// Desc: Gets called by the underlying control whenever a notication code is received through WM_COMMAND or WM_NOTIFY
//-----------------------------------------------------------------------------
void __stdcall TabControl::OnNotifyMessage(UINT notifycode, LPVOID user)
{
	TabControl* tabctrl = (TabControl*)user;
	switch(notifycode)
	{
	case TCN_SELCHANGE:
		if(tabctrl->tabchangedcbk)
		{
			int index = TabCtrl_GetCurSel(tabctrl->basecontrol.handle);
			tabctrl->tabchangedcbk(tabctrl, index, index >= 0 ? tabctrl->tags[index] : NULL, tabctrl->tabchangedcbkuser);
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// Name: ~TabControl()
// Desc: Destructor
//-----------------------------------------------------------------------------
TabControl::~TabControl()
{
}