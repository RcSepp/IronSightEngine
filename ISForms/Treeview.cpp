#include "Forms.h"


WNDPROC Treeview::BaseClassWndProc = NULL;
#define TREEVIEW_SUPERCLASSNAME "Treeview"

//-----------------------------------------------------------------------------
// Name: CreateTreeview()
// Desc: DLL API for creating an instance of a treeview control
//-----------------------------------------------------------------------------
__declspec(dllexport) LPTREEVIEW __cdecl CreateTreeview()
{
	return new Treeview();
}

//-----------------------------------------------------------------------------
// Name: Treeview()
// Desc: Constructor
//-----------------------------------------------------------------------------
Treeview::Treeview() : basecontrol(WC_TREEVIEW, TREEVIEW_SUPERCLASSNAME, TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, NULL, NULL, BaseClassWndProc, (WNDPROC)SuperClassWndProc)
{
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG Treeview::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	/*switch(msg)
	{
	
	}*/
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}

//-----------------------------------------------------------------------------
// Name: AddItem()
// Desc: Add a node to the treeview's tree
//-----------------------------------------------------------------------------
Result Treeview::AddItem(const String& text, const ITreeviewItem* parentitem, ITreeviewItem** item)
{
	if(item)
		*item = NULL;
	Result rlt;

	if(!basecontrol.handle)
		return ERR("Treeview items can't be added, before the treeview has been added to a form");

	TreeviewItem* newitem;
	CHKALLOC(newitem = new TreeviewItem(this));
	CHKRESULT(newitem->Init(text, (TreeviewItem*)parentitem));

	items.push_back(newitem);
	if(item)
		*item = newitem;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: ~Treeview()
// Desc: Destructor
//-----------------------------------------------------------------------------
Treeview::~Treeview()
{
	while(items.size())
		items.front()->Release();
}


// >>> ToolbarItem


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize item and add it to the treeview
//-----------------------------------------------------------------------------
Result TreeviewItem::Init(const String& text, const TreeviewItem* parentitem)
{
    TVITEM tvi = {0};
    tvi.mask = TVIF_TEXT; //| TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvi.pszText = text;
    tvi.cchTextMax = text.length();
    tvi.iImage = 0;

    TVINSERTSTRUCT tvis = {0};
    tvi.iSelectedImage = 1;
    tvis.item = tvi; 
    tvis.hInsertAfter = 0;
    tvis.hParent = parentitem ? parentitem->handle : TVI_ROOT;

	handle = (HTREEITEM)SendMessage(parent->basecontrol.handle, TVM_INSERTITEM, 0, (LPARAM)&tvis);
    if(!handle)
		return ERR("Error adding treeview item");

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void TreeviewItem::Release()
{
	parent->items.remove(this);

	delete this;
}