#include "Forms.h"


//-----------------------------------------------------------------------------
// Name: Menu()
// Desc: Constructor
//-----------------------------------------------------------------------------
Menu::Menu(WinForm* parent, Type menutype) : handle(NULL), parent(parent), menutype(menutype), itemaddedcbk(NULL)
{
	hwndparent = parent->GetHwnd();
}

//-----------------------------------------------------------------------------
// Name: Show()
// Desc: Display the menu at the given position
//-----------------------------------------------------------------------------
void Menu::Show(int x, int y, UINT allignment)
{
	if(menutype != MT_CONTEXTMENU && menutype != MT_DROPDOWNMENU)
	{
		LOG("Only context menus and drop down menus can be shown at a specific location");
		return;
	}

	// Remove old menu
	if(handle)
	{
		DestroyMenu(handle);
		handle = NULL;
	}

	// Create menu
	handle = CreatePopupMenu();
	if(!handle)
		return;

	// Insert items
	std::list<MenuItem*>::iterator iter;
	LIST_FOREACH(menuitems, iter)
		(*iter)->Insert(handle);

	// Open menu
	SetForegroundWindow(hwndparent);
	TrackPopupMenu(handle, allignment, x, y, 0, hwndparent, NULL);
	DestroyMenu(handle);
	handle = NULL;

	// Notify items that the menu is closing
	LIST_FOREACH(menuitems, iter)
		(*iter)->OnMenuClosed();
}

//-----------------------------------------------------------------------------
// Name: Show()
// Desc: Display the menu
//-----------------------------------------------------------------------------
void Menu::Show()
{
	switch(menutype)
	{
	case MT_CONTEXTMENU: case MT_DROPDOWNMENU:
		{
			POINT pt;
			GetCursorPos(&pt);
			Show((int)pt.x, (int)pt.y, TPM_BOTTOMALIGN);
		}
		break;

	case MT_TITLEMENU:
		{
			// Remove old menu
			if(handle)
			{
				DestroyMenu(handle);
				handle = NULL;
			}

			// Create menu
			handle = CreateMenu();
			if(!handle)
				return;

			// Insert items
			std::list<MenuItem*>::iterator iter;
			LIST_FOREACH(menuitems, iter)
				(*iter)->Insert(handle);

			// Show menu
			SetMenu(hwndparent, handle);
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// Name: AddMenuItem()
// Desc: Adds an item to the menu
//-----------------------------------------------------------------------------
Result Menu::AddMenuItem(const String& text, IMenuItem::Type itemtype, IMenuItem* parentitem, const MENUITEMCLICK_CALLBACK clickcbk, LPVOID clickcbkuser, IMenuItem** menuitem)
{
	if(menuitem)
		*menuitem = NULL;
	Result rlt;

	MenuItem* newmenuitem;
	CHKALLOC(newmenuitem = new MenuItem(this, (MenuItem*)parentitem, itemtype));

	if(!text.IsEmpty())
		newmenuitem->SetText(text);
	newmenuitem->clickcbk = clickcbk;
	newmenuitem->clickcbkuser = clickcbkuser;

	menuitems.push_back(newmenuitem);
	if(menuitem)
		*menuitem = newmenuitem;

	// Invoke callback
	if(itemaddedcbk)
		itemaddedcbk(this, newmenuitem, itemaddedcbkuser);

	return R_OK;
}
Result Menu::AddMenuItem(IMenuItem* menuitem, IMenuItem* parentitem, const String& text, const MENUITEMCLICK_CALLBACK clickcbk, LPVOID clickcbkuser)
{
	Result rlt;

	MenuItem* newmenuitem = (MenuItem*)menuitem;
	CHKRESULT(newmenuitem->Adopt(this, (MenuItem*)parentitem));

	if(!text.IsEmpty())
		newmenuitem->SetText(text);
	if(clickcbk)
		newmenuitem->clickcbk = clickcbk;
	if(clickcbkuser)
		newmenuitem->clickcbkuser = clickcbkuser;

	menuitems.push_back(newmenuitem);

	// Invoke callback
	if(itemaddedcbk)
		itemaddedcbk(this, newmenuitem, itemaddedcbkuser);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CallOnMenuItemClicked()
// Desc: Fire click event for the menu item with the specified id
//-----------------------------------------------------------------------------
void Menu::CallOnMenuItemClicked(WORD itemid)
{
	std::list<MenuItem*>::iterator iter;
	LIST_FOREACH(menuitems, iter)
		if((*iter)->id == itemid && (*iter)->clickcbk)
		{
			(*iter)->clickcbk(*iter, (*iter)->clickcbkuser);
			break;
		}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Menu::Release()
{
	parent->menus.remove(this);

	if(handle)
	{
		DestroyMenu(handle);
		handle = NULL;
	}

	while(menuitems.size())
		menuitems.front()->Release();

	delete this;
}


// >>> MenuItem


//-----------------------------------------------------------------------------
// Name: MenuItem()
// Desc: Constructor
//-----------------------------------------------------------------------------
MenuItem::MenuItem(Menu* parent, MenuItem* parentitem, IMenuItem::Type type) : parent(parent), type(type), parentitem(parentitem), hmenu(NULL)
{
	if(parent)
		id = parent->menuitems.size() + (parent->parent->menus.size() << 8);

	ZeroMemory(&info, sizeof(MENUITEMINFO));
	info.cbSize = sizeof(MENUITEMINFO);
	info.wID = id;
	info.fMask = MIIM_ID;

	switch(type)
	{
	case MI_SEPARATOR:
		info.fType = MFT_SEPARATOR;
		info.fMask |= MIIM_TYPE;
		break;
	case MI_DROPDOWN_STATIC:
		info.hSubMenu = CreatePopupMenu();
		info.fMask |= MIIM_SUBMENU;
		break;
	}
}

//-----------------------------------------------------------------------------
// Name: Adopt()
// Desc: Sets parent, parentitem and id for menuitems that aren't created with a specific parent menu
//-----------------------------------------------------------------------------
Result MenuItem::Adopt(Menu* parent, MenuItem* parentitem)
{
	if(this->parent)
		return ERR("This menuitem already has a parent menu");
	this->parent = parent;
	this->parentitem = parentitem;
	this->id = parent->menuitems.size() + (parent->parent->menus.size() << 8);

	return R_OK;
}

String MenuItem::GetText()
{
	return info.dwTypeData ? String((LPCSTR)info.dwTypeData) : String();
}
void MenuItem::SetText(const String& val)
{
	if(type == MI_SEPARATOR)
		return;

	info.dwTypeData = new char[info.cch = val.length() + 1];
	strcpy_s(info.dwTypeData, info.cch, val.ToCharString());

	info.fMask |= MIIM_STRING;
	info.fType |= MFT_STRING;

	if(hmenu)
	{
		MENUITEMINFO tmpinfo(info);
		tmpinfo.fMask = MIIM_STRING;
		tmpinfo.fType = MFT_STRING;
		SetMenuItemInfo(hmenu, id, TRUE, &info);
	}
}

//-----------------------------------------------------------------------------
// Name: Insert()
// Desc: Create menu item using InsertMenu()
//-----------------------------------------------------------------------------
void MenuItem::Insert(HMENU hmenu)
{
	if(parentitem)
		hmenu = parentitem->info.hSubMenu;

	if(type == MI_DROPDOWN)
	{
		info.hSubMenu = CreatePopupMenu();
		info.fMask |= MIIM_SUBMENU;
	}

	InsertMenuItem(hmenu, id, TRUE, &info);

	this->hmenu = hmenu;
}

//-----------------------------------------------------------------------------
// Name: OnMenuClosed()
// Desc: Should be called when closing the underlying menu to indicate that future editing of item properties doesn't need to apply changes via SetMenuItemInfo()
//-----------------------------------------------------------------------------
void MenuItem::OnMenuClosed()
{
	hmenu = NULL;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void MenuItem::Release()
{
	if(parent)
		parent->menuitems.remove(this);

	if(type == MI_DROPDOWN_STATIC && info.hSubMenu && !hmenu)
	{
		// Destroy abandoned submenu
		DestroyMenu(info.hSubMenu);
		info.hSubMenu = NULL;
	}

	delete this;
}


// >>> IHasContextMenu


//-----------------------------------------------------------------------------
// Name: CreateContextMenu()
// Desc: Create a context menu that appears when the user right-clicks the icon
//-----------------------------------------------------------------------------
Result IHasContextMenu::CreateContextMenu(IMenu** ctxmenu)
{
	*ctxmenu = NULL;
	Result rlt;

	// Remove old context menu
	RELEASE(this->ctxmenu);

	Menu* newctxmenu;
	CHKALLOC(newctxmenu = new Menu(parentform, Menu::MT_CONTEXTMENU));

	parentform->menus.push_back(newctxmenu);
	*ctxmenu = this->ctxmenu = newctxmenu;

	return R_OK;
}