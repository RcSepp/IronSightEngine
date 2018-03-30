#include "Forms.h"
#include <richedit.h>


WNDPROC RichEdit::BaseClassWndProc = NULL;

//-----------------------------------------------------------------------------
// Name: CreateRichEdit()
// Desc: DLL API for creating an instance of a group box button control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPRICHEDIT __cdecl CreateRichEdit(bool hashscroll, bool hasvscroll)
{
	static bool dll_loaded = false;
	if(!dll_loaded)
	{
		dll_loaded = true;
		LoadLibrary("Msftedit.dll");
	}

	return new RichEdit("RICHEDIT50W", "RichEdit", WS_BORDER | ES_MULTILINE | WS_TABSTOP | (hashscroll * WS_HSCROLL) | (hasvscroll * WS_VSCROLL), VSCLASS_EDIT, EP_BACKGROUNDWITHBORDER);
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG RichEdit::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Control* ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
	if(ctrl)
	{
		switch(msg)
		{
		case WM_KEYDOWN:
			if(ctrl->keydowncbk)
				ctrl->keydowncbk(ctrl->ifc, wparam, ctrl->keydowncbkuser);
			break;
		case WM_KEYUP:
			ctrl->RaiseKeyUpCbk(wparam);
			break;

		case WM_CONTEXTMENU:
			if(ctrl->ctxmenu)
			{
				ctrl->ShowContextMenu(); // Show custom contextmenu
				return 0; // Supress standard contextmenu
			}
		}
	}
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}

void RichEdit::SetFormat(COLORREF fgcolor, COLORREF bgcolor, const String& font)
{
	if(!basecontrol.handle)
		return;

	CHARFORMAT2 cf;
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = (CFM_COLOR * (fgcolor != (COLORREF)-1)) | (CFM_BACKCOLOR * (bgcolor != (COLORREF)-1)) | (CFM_FACE * !font.IsEmpty());
	cf.crTextColor = fgcolor;
	cf.crBackColor = bgcolor;
	if(!font.IsEmpty())
		strcpy_s(cf.szFaceName, font);
	SendMessage(basecontrol.handle, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
}

void RichEdit::InsertText(const String& text)
{
	if(!basecontrol.handle)
		return;

	SendMessage(basecontrol.handle, EM_REPLACESEL, FALSE, (LPARAM)text.ToTCharString());
}