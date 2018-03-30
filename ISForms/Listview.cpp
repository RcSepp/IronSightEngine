#include "Forms.h"


WNDPROC Listview::BaseClassWndProc = NULL;
#define NUD_SUPERCLASSNAME "Listview"

//-----------------------------------------------------------------------------
// Name: CreateListview()
// Desc: DLL API for creating an instance of a listview control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPLISTVIEW __cdecl CreateListview()
{
	return new Listview();
}

//-----------------------------------------------------------------------------
// Name: Listview()
// Desc: Constructor
//-----------------------------------------------------------------------------
Listview::Listview() : groupsenabled(false), basecontrol(WC_LISTVIEW, NUD_SUPERCLASSNAME, WS_HSCROLL | LVS_REPORT | LVS_SINGLESEL,
														 NULL, NULL, BaseClassWndProc, (WNDPROC)SuperClassWndProc, WS_EX_CLIENTEDGE)
{
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG Listview::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Control* ctrl = (Control*)GetProp(wnd, "PROP_CTRL");
	if(ctrl)
		switch(msg)
		{
		case WM_LBUTTONDOWN:
			ctrl->RaiseMouseClickCbk();
			break;
		}
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}

bool Listview::SetGroupsEnabled(bool v)
{
	if(v == groupsenabled)
		return groupsenabled;
	ListView_EnableGroupView(basecontrol.handle, groupsenabled = v);
	return groupsenabled;
}

void Listview::InsertGroup(const String& caption, int groupindex)
{
	if(groupindex < 0)
		groupindex = ListView_GetGroupCount(basecontrol.handle);

	LVGROUP lvg;
	lvg.cbSize = sizeof(LVGROUP);
	lvg.mask = LVGF_HEADER | LVGF_GROUPID;
	lvg.pszHeader = caption.ToWCharString();
	lvg.iGroupId = groupindex;

	ListView_InsertGroup(basecontrol.handle, groupindex, &lvg);
	delete[] lvg.pszHeader;

	SetGroupsEnabled(true);
}

void Listview::InsertColumn(const String& caption, int width, int columnindex)
{
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;
	lvc.cx = width;
	lvc.pszText = caption;
	lvc.cchTextMax = (int)caption.length() + 1;
	lvc.iSubItem = 0;

	ListView_InsertColumn(basecontrol.handle, columnindex, &lvc);
}

void Split(char* str, char spltchr, std::vector<char*>& words)
{
	char *wordstart = str, *c = str;
	words.clear();

	while(*c != '\0')
	{
		if(*c == spltchr)
		{
			*c = '\0';
			words.push_back(wordstart);
			wordstart = ++c;
		}
		else
			++c;
	}

	words.push_back(wordstart);
}

void Listview::InsertItem(const String& text, char textdelimiter, int groupindex, int rowindex = 0x7FFFFFFF)
{
	LPSTR textchars = text.CopyChars();
	std::vector<char*> splittedtext;
	Split(textchars, textdelimiter, splittedtext);

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_GROUPID;
	lvi.iItem = rowindex;
	lvi.iSubItem = 0;
	lvi.iGroupId = groupindex >= 0 ? groupindex : I_GROUPIDNONE;
	lvi.pszText = splittedtext[0];
	rowindex = ListView_InsertItem(basecontrol.handle, &lvi);

	for(size_t i = 1; i < splittedtext.size(); i++)
		ListView_SetItemText(basecontrol.handle, rowindex, i, splittedtext[i]);

	delete[] textchars;
}
void Listview::DeleteItem(int rowindex)
{
	ListView_DeleteItem(basecontrol.handle, rowindex);
}
void Listview::DeleteAllItems()
{
	ListView_DeleteAllItems(basecontrol.handle);
}

/*void Listview::SetView()
{
DWORD newstyles = WS_HSCROLL | LVS_REPORT | LVS_SINGLESEL; // EDIT
	DWORD oldstyles = GetWindowLong(basecontrol.handle, GWL_STYLE);
	SetWindowLong(basecontrol.handle, GWL_STYLE, (oldstyles & ~LVS_TYPEMASK) | newstyles);
}*/

//-----------------------------------------------------------------------------
// Name: ~Listview()
// Desc: Destructor
//-----------------------------------------------------------------------------
Listview::~Listview()
{
}