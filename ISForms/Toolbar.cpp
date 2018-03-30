#include "Forms.h"


WNDPROC Toolbar::BaseClassWndProc = NULL;
#define TOOLBAR_SUPERCLASSNAME "Toolbar"

//-----------------------------------------------------------------------------
// Name: Toolbar()
// Desc: Constructor
//-----------------------------------------------------------------------------
Toolbar::Toolbar()
{
	if(!BaseClassWndProc)
		Control::CreateSuperClass(TOOLBARCLASSNAME, TOOLBAR_SUPERCLASSNAME, BaseClassWndProc, (WNDPROC)SuperClassWndProc);
}

//-----------------------------------------------------------------------------
// Name: SuperClassWndProc()
// Desc: A custom message handler for the control window
//-----------------------------------------------------------------------------
LONG Toolbar::SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	/*switch(msg)
	{
	
	}*/
	return CallWindowProc(BaseClassWndProc, wnd, msg, wparam, lparam);
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize the toolbar
//-----------------------------------------------------------------------------
Result Toolbar::Init(bool useliststyle, WinForm* parent)
{
	//Result rlt;

	this->parentform = parent;

	// Create toolbar window
	handle = CreateWindowEx(0, TOOLBAR_SUPERCLASSNAME, 0, TBSTYLE_FLAT | useliststyle * TBSTYLE_LIST | CCS_ADJUSTABLE | CCS_NODIVIDER | WS_CHILD | WS_VISIBLE, //EDIT: Enable CCS_ADJUSTABLE support
							0, 0, 0, 0, parentform->handle, NULL, fms->GetInstance(), 0);
	if(!handle)
		return ERR("Error creating toolbar");
	SendMessage(handle, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: AddButton()
// Desc: Adds a button to the tool bar
//-----------------------------------------------------------------------------
Result Toolbar::AddButton(const String& text, IToolbarItem::Type btntype, const TOOLBARITEMCLICK_CALLBACK clickcbk, LPVOID clickcbkuser, IToolbarItem** item)
{
	if(item)
		*item = NULL;
	Result rlt;

	ToolbarItem* newitem;
	CHKALLOC(newitem = new ToolbarItem(this, btntype, items.size()));
	CHKRESULT(newitem->Init(text, I_IMAGENONE));

	newitem->clickcbk = clickcbk;
	newitem->clickcbkuser = clickcbkuser;

	// Resize toolbar
	SendMessage(handle, TB_AUTOSIZE, 0, 0);

	items.push_back(newitem);
	if(item)
		*item = newitem;

	return R_OK;
}
Result Toolbar::AddButton(const String& text, INT_PTR imageresid, DWORD imageclrkey, IToolbarItem::Type btntype,
						  const TOOLBARITEMCLICK_CALLBACK clickcbk, LPVOID clickcbkuser, IToolbarItem** item)
{
	if(item)
		*item = NULL;
	Result rlt;

	ToolbarItem* newitem;
	CHKALLOC(newitem = new ToolbarItem(this, btntype, items.size()));
	int imageidx, imgwidth;
	CHKRESULT(LoadImage(imageresid, imageclrkey, imageidx, imgwidth));
	CHKRESULT(newitem->Init(text, imageidx));
	newitem->SetWidth(imgwidth + 6);

	newitem->clickcbk = clickcbk;
	newitem->clickcbkuser = clickcbkuser;

	// Resize toolbar
	SendMessage(handle, TB_AUTOSIZE, 0, 0);

	items.push_back(newitem);
	if(item)
		*item = newitem;

	return R_OK;
}

Size<int> Toolbar::GetSize()
{
	RECT rect;
	GetWindowRect(handle, &rect);
	return Size<int>(rect.right - rect.left, rect.bottom - rect.top);
}

void Toolbar::OnNotifyMessage(const NMTOOLBAR* nmtoolbar)
{
	// Get toolbar item with index nmtoolbar->iItem
	std::list<ToolbarItem*>::iterator iter;
	UINT i;
	LIST_FOREACH_I(items, iter, i)
		if(i == nmtoolbar->iItem)
			break;
	if(i == items.size())
		return;
	ToolbarItem* item = *iter;

	// Handle notification
	switch(nmtoolbar->hdr.code)
	{
	case TBN_DROPDOWN:
			item->ShowDropDownMenu();
		break;
	case NM_CLICK:
		if(item->clickcbk)
			item->clickcbk(*iter, item->clickcbkuser);
		break;
	}
}

Result Toolbar::LoadImage(INT_PTR imageresid, DWORD imageclrkey, int& imageidx, int& imgwidth)
{
	imageidx = I_IMAGENONE;

	DWORD bgcolor = GetSysColor(COLOR_BTNFACE);

	// Load image
	HBITMAP hbm = LoadBitmap(fms->GetInstance(), MAKEINTRESOURCE(imageresid));
	//HBITMAP hbm = (HBITMAP)LoadImageA(NULL, "K:\\MPU\\VMPU (C++)\\VMPU\\Play.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(!hbm)
		return ERR("Error loading toolbar image from resource");

	// Load bitmap info
	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	HDC hdc = CreateCompatibleDC(NULL);
	SelectObject(hdc, hbm);
	GetDIBits(hdc, hbm, 0, 0, NULL, &bmi, DIB_RGB_COLORS);
	DeleteDC(hdc);

	imgwidth = bmi.bmiHeader.biWidth;

	// Replace imageclrkey color with bgcolor
	int numpixels = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight;
	DWORD* bmpdata = new DWORD[numpixels];
	GetBitmapBits(hbm, numpixels * sizeof(DWORD), bmpdata);
	for(int i = 0; i < numpixels; i++)
		if(bmpdata[i] == imageclrkey)
			bmpdata[i] = bgcolor;
	SetBitmapBits(hbm, numpixels * sizeof(DWORD), bmpdata);

	// Add image to this toolbar's image list
	TBADDBITMAP tb;
	tb.hInst = NULL;
	tb.nID = (UINT_PTR)hbm;
	imageidx = SendMessage(handle, TB_ADDBITMAP, 1, (LPARAM)&tb);
	if(imageidx == -1)
		return ERR("Error adding toolbar image");
	imagelist[imageidx] = ImageInfo(imageresid, hbm);

	return R_OK;
}

Result Toolbar::ReplaceImage(INT_PTR imageresid, DWORD imageclrkey, int& imageidx, int& imgwidth)
{
	DWORD bgcolor = GetSysColor(COLOR_BTNFACE);

	// Load image
	HBITMAP hbm = LoadBitmap(fms->GetInstance(), MAKEINTRESOURCE(imageresid));
	//HBITMAP hbm = (HBITMAP)LoadImageA(NULL, "K:\\MPU\\VMPU (C++)\\VMPU\\Play.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(!hbm)
		return ERR("Error loading toolbar image from resource");

	// Load bitmap info
	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	HDC hdc = CreateCompatibleDC(NULL);
	SelectObject(hdc, hbm);
	GetDIBits(hdc, hbm, 0, 0, NULL, &bmi, DIB_RGB_COLORS);
	DeleteDC(hdc);

	imgwidth = bmi.bmiHeader.biWidth;

	// Replace imageclrkey color with bgcolor
	int numpixels = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight;
	DWORD* bmpdata = new DWORD[numpixels];
	GetBitmapBits(hbm, numpixels * sizeof(DWORD), bmpdata);
	for(int i = 0; i < numpixels; i++)
		if(bmpdata[i] == imageclrkey)
			bmpdata[i] = bgcolor;
	SetBitmapBits(hbm, numpixels * sizeof(DWORD), bmpdata);

	ImageInfo& oldimg = imagelist.find(imageidx)->second;

	// Replace image in this toolbar's image list
	TBREPLACEBITMAP tb;
	tb.hInstOld = NULL;
	tb.nIDOld = (UINT_PTR)oldimg.hbmp;
	tb.hInstNew = NULL;
	tb.nIDNew = (UINT_PTR)hbm;
	tb.nButtons = 1;
	if(!SendMessage(handle, TB_REPLACEBITMAP, 0, (LPARAM)&tb))
		return ERR("Error adding toolbar image");
	DeleteObject(oldimg.hbmp);
	imagelist[imageidx] = ImageInfo(imageresid, hbm);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Toolbar::Release()
{
	parentform->toolbars.remove(this);

	while(items.size())
		items.front()->Release();

	delete this;
}


// >>> ToolbarItem


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize item and add it to the toolbar
//-----------------------------------------------------------------------------
Result ToolbarItem::Init(const String& text, int imageidx)
{
	TBBUTTON tbb = {0};
	tbb.iBitmap = imageidx;
	tbb.idCommand = id;
	tbb.iString = (INT_PTR)text.ToTCharString();
	tbb.fsState = TBSTATE_ENABLED;
	switch(type)
	{
	case TBI_BUTTON: tbb.fsStyle = BTNS_BUTTON; break;
	case TBI_CHECK: tbb.fsStyle = BTNS_CHECK; break;
	case TBI_RADIO: tbb.fsStyle = BTNS_CHECKGROUP; break;
	case TBI_DROPDOWN: tbb.fsStyle = BTNS_DROPDOWN; break;
	case TBI_WHOLEDROPDOWN: tbb.fsStyle = BTNS_WHOLEDROPDOWN; break;
	case TBI_SEPARATOR: tbb.fsStyle = BTNS_SEP;
	}
	if(SendMessage(parent->handle, TB_ADDBUTTONS, 1, (LPARAM)&tbb) == FALSE)
		return ERR("Error adding toolbar button");

	return R_OK;
}

void ToolbarItem::SetText(const String& val)
{
	this->text = val;

	TBBUTTONINFO ifo;
	ifo.cbSize = sizeof(TBBUTTONINFO);
	ifo.dwMask = TBIF_TEXT;
	ifo.pszText = val;
	SendMessage(parent->handle, TB_SETBUTTONINFO, id, (LPARAM)&ifo);
}

INT_PTR ToolbarItem::GetImage()
{
	int imgidx = SendMessage(parent->handle, TB_GETBITMAP, id, 0);
	return parent->imagelist.find(imgidx)->second.resid;
}

Result ToolbarItem::SetImage(INT_PTR imageresid, DWORD imageclrkey)
{
	Result rlt;

	// Get old image index
	int oldimgidx = SendMessage(parent->handle, TB_GETBITMAP, id, 0);

	// Check whether old image == new image
	if(imageresid == parent->imagelist.find(oldimgidx)->second.resid)
		return R_OK;

	// Load new image
	int imageidx = oldimgidx, imgwidth;
	CHKRESULT(parent->ReplaceImage(imageresid, imageclrkey, imageidx, imgwidth));
	SetWidth(imgwidth + 6);

	return R_OK;
}

void ToolbarItem::SetWidth(WORD newwidth)
{
	TBBUTTONINFO ifo;
	ifo.cbSize = sizeof(TBBUTTONINFO);
	ifo.dwMask = TBIF_SIZE;
	ifo.cx = newwidth;
	SendMessage(parent->handle, TB_SETBUTTONINFO, id, (LPARAM)&ifo);
}

bool ToolbarItem::GetCheckedState()
{
	TBBUTTONINFO ifo;
	ifo.cbSize = sizeof(TBBUTTONINFO);
	ifo.dwMask = TBIF_STATE;
	SendMessage(parent->handle, TB_GETBUTTONINFO, id, (LPARAM)&ifo);
	return (ifo.fsState & TBSTATE_CHECKED) != 0;
}
void ToolbarItem::SetCheckedState(bool val)
{
	TBBUTTONINFO ifo;
	ifo.cbSize = sizeof(TBBUTTONINFO);
	ifo.dwMask = TBIF_STATE;
	SendMessage(parent->handle, TB_GETBUTTONINFO, id, (LPARAM)&ifo);
	ifo.fsState = (ifo.fsState & ~TBSTATE_CHECKED) | (TBSTATE_CHECKED * val);
	SendMessage(parent->handle, TB_SETBUTTONINFO, id, (LPARAM)&ifo);
}
bool ToolbarItem::GetEnabledState()
{
	TBBUTTONINFO ifo;
	ifo.cbSize = sizeof(TBBUTTONINFO);
	ifo.dwMask = TBIF_STATE;
	SendMessage(parent->handle, TB_GETBUTTONINFO, id, (LPARAM)&ifo);
	return (ifo.fsState & TBSTATE_ENABLED) != 0;
}
void ToolbarItem::SetEnabledState(bool val)
{
	TBBUTTONINFO ifo;
	ifo.cbSize = sizeof(TBBUTTONINFO);
	ifo.dwMask = TBIF_STATE;
	SendMessage(parent->handle, TB_GETBUTTONINFO, id, (LPARAM)&ifo);
	ifo.fsState = (ifo.fsState & ~TBSTATE_ENABLED) | (TBSTATE_ENABLED * val);
	SendMessage(parent->handle, TB_SETBUTTONINFO, id, (LPARAM)&ifo);
}

//-----------------------------------------------------------------------------
// Name: CreateDropDownMenu()
// Desc: Create a drop-down menu that appears when the TBN_DROPDOWN message is received
//-----------------------------------------------------------------------------
Result ToolbarItem::CreateDropDownMenu(IMenu** dropdownmenu)
{
	*dropdownmenu = NULL;
	Result rlt;

	// Remove old context menu
	RELEASE(this->dropdownmenu);

	Menu* newdropdownmenu;
	CHKALLOC(newdropdownmenu = new Menu(parent->parentform, Menu::MT_DROPDOWNMENU));

	parent->parentform->menus.push_back(newdropdownmenu);
	*dropdownmenu = this->dropdownmenu = newdropdownmenu;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: ShowDropDownMenu()
// Desc: Show the drop-down menu below the item
//-----------------------------------------------------------------------------
void ToolbarItem::ShowDropDownMenu()
{
	if(!dropdownmenu)
		return;

	// Get button coordinates
	RECT rect;
	SendMessage(parent->handle, TB_GETRECT, (WPARAM)id, (LPARAM)&rect);

	// Convert to screen coordinates.
	MapWindowPoints(parent->handle, HWND_DESKTOP, (LPPOINT)&rect, 2);

	// Show drop down menu at the bottom left corner of the button
	dropdownmenu->Show(rect.left, rect.bottom, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void ToolbarItem::Release()
{
	parent->items.remove(this);

//	// Remove drop-down menu //EDIT: When dropdownmenu gets released by its parentform (WinForm) first, than this pointer points to invalid memory!
//	RELEASE(dropdownmenu);

	delete this;
}