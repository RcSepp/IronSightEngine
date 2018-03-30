#ifndef __FORMS_H
#define __FORMS_H

#include "ISForms.h"
#include "..\\global\\HResult.h"
#include <Uxtheme.h>
#include <Vssym32.h>
#include <Shobjidl.h> // For file dialogs

#pragma comment (lib, "UxTheme.lib")
#pragma comment (lib, "Comctl32.lib")

namespace boost {namespace python {namespace api
{
	class object;
}}}
struct PyCallbackArgs;


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
typedef void (__stdcall* NOTIFY_CALLBACK)(UINT notifycode, LPVOID user);


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#define BASECONTROL_ADAPTER_FUNCTIONS	__forceinline bool GetVisible() {return basecontrol.visible;} \
										__forceinline void SetVisible(bool val) {basecontrol.SetVisible(val);} \
										__forceinline void SuspendLayout() {basecontrol.suspendlayout = true;} \
										__forceinline void ResumeLayout() {basecontrol.suspendlayout = false;} \
										__forceinline virtual HWND GetHwnd() {return basecontrol.handle;} \
										__forceinline bool GetEnabled() {return basecontrol.enabled;} \
										__forceinline void SetEnabled(bool val) {basecontrol.SetEnabled(val);} \
										__forceinline String GetText() const {return basecontrol.text;} \
										__forceinline void SetText(const String& val) {basecontrol.SetText(val);} \
										__forceinline Rect<int> GetBounds() {return basecontrol.bounds;} \
										__forceinline void SetBounds(const Rect<int> &val) {basecontrol.SetBounds(val);} \
										__forceinline Point<int> GetLocation() {return basecontrol.bounds.location;} \
										__forceinline void SetLocation(const Point<int> &val) {basecontrol.SetBounds(Rect<int>(val, basecontrol.bounds.size));} \
										__forceinline ::Size<int> GetSize() {return basecontrol.bounds.size;} \
										__forceinline void SetSize(const ::Size<int> &val) {basecontrol.SetBounds(Rect<int>(basecontrol.bounds.location, val));} \
										__forceinline int GetTop() {return basecontrol.bounds.y;} \
										__forceinline void SetTop(int val) {basecontrol.SetBounds(Rect<int>(basecontrol.bounds.x, val, basecontrol.bounds.width, basecontrol.bounds.height));} \
										__forceinline int GetLeft() {return basecontrol.bounds.x;} \
										__forceinline void SetLeft(int val) {basecontrol.SetBounds(Rect<int>(val, basecontrol.bounds.y, basecontrol.bounds.width, basecontrol.bounds.height));} \
										__forceinline int GetWidth() {return basecontrol.bounds.width;} \
										__forceinline void SetWidth(int val) {basecontrol.SetBounds(Rect<int>(basecontrol.bounds.x, basecontrol.bounds.y, val, basecontrol.bounds.height));} \
										__forceinline int GetHeight() {return basecontrol.bounds.height;} \
										__forceinline void SetHeight(int val) {basecontrol.SetBounds(Rect<int>(basecontrol.bounds.x, basecontrol.bounds.y, basecontrol.bounds.width, val));} \
										__forceinline void SetMouseDownCallback(const CONTROLMOUSEEVENT_CALLBACK cbk, LPVOID user) {basecontrol.mousedowncbk = cbk; basecontrol.mousedowncbkuser = user;} \
										__forceinline void SetMouseDownCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user); \
										__forceinline void SetMouseUpCallback(const CONTROLMOUSEEVENT_CALLBACK cbk, LPVOID user) {basecontrol.mouseupcbk = cbk; basecontrol.mouseupcbkuser = user;} \
										__forceinline void SetMouseUpCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user); \
										__forceinline void SetMouseMoveCallback(const CONTROLMOUSEEVENT_CALLBACK cbk, LPVOID user) {basecontrol.mousemovecbk = cbk; basecontrol.mousemovecbkuser = user;} \
										__forceinline void SetMouseMoveCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user); \
										__forceinline void SetMouseDoubleClickCallback(const CONTROLMOUSEEVENT_CALLBACK cbk, LPVOID user) {basecontrol.mousedclickcbk = cbk; basecontrol.mousedclickcbkuser = user;} \
										__forceinline void SetMouseDoubleClickCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user); \
										__forceinline void SetMouseWheelCallback(const CONTROLMOUSEEVENT_CALLBACK cbk, LPVOID user) {basecontrol.mousewheelcbk = cbk; basecontrol.mousewheelcbkuser = user;} \
										__forceinline void SetMouseWheelCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user); \
										__forceinline void SetMouseClickCallback(MOUSECLICK_CALLBACK cbk, LPVOID user) {basecontrol.mclickcbk = cbk; basecontrol.mclickcbkuser = user;} \
										__forceinline void SetMouseClickCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user); \
										__forceinline void SetKeyDownCallback(KEYEVENT_CALLBACK cbk, LPVOID user) {basecontrol.keydowncbk = cbk; basecontrol.keydowncbkuser = user;} \
										__forceinline void SetKeyDownCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user); \
										__forceinline void SetKeyUpCallback(KEYEVENT_CALLBACK cbk, LPVOID user) {basecontrol.keyupcbk = cbk; basecontrol.keyupcbkuser = user;} \
										__forceinline void SetKeyUpCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user); \
										__forceinline Result CreateContextMenu(IMenu** ctxmenu) {return basecontrol.CreateContextMenu(ctxmenu);} \
										__forceinline IMenu* GetContextMenu() {return basecontrol.GetContextMenu();} \
										__forceinline void ShowContextMenu() {basecontrol.ShowContextMenu();} \
										LPVOID _getbase() {return &basecontrol;} \
										void Release() {delete this;}


class WinForm;
class Control;

//-----------------------------------------------------------------------------
// Name: IContainer
// Desc: Interface to a class that can hold controls
//-----------------------------------------------------------------------------
class IContainer
{
public:
	std::list<Control*> controls;

	virtual HWND GetHwnd() = 0;
	virtual WinForm* GetParentForm() = 0;
	Result AddControl(IControl* ctrl);
	Result RemoveControl(IControl* ctrl);
	Result RemoveAllControls();
};

//-----------------------------------------------------------------------------
// Name: MenuItem
// Desc: Representing an item in the menu (see Menu class)
//-----------------------------------------------------------------------------
class Menu;
class MenuItem : public IMenuItem
{
public:
	enum Type
		{MIT_TEXT, MIT_DROPDOWN};
private:
	Menu* parent;
	MenuItem* parentitem;
	IMenuItem::Type type;
	MENUITEMINFO info;
	HMENU hmenu;

public:
	UINT id;
	MENUITEMCLICK_CALLBACK clickcbk;
	LPVOID clickcbkuser;

	MenuItem(Menu* parent, MenuItem* parentitem, IMenuItem::Type type);
	Result Adopt(Menu* parent, MenuItem* parentitem);
	void Insert(HMENU hmenu);
	void OnMenuClosed();
	String GetText();
	void SetText(const String& val);
	HMENU GetSubMenuHandle() {return info.hSubMenu;}
	void SetClickCallback(const MENUITEMCLICK_CALLBACK clickcbk, LPVOID user) {this->clickcbk = clickcbk; clickcbkuser = user;}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Menu
// Desc: Representing a menu bar, drop-down menu, submenu, shortcut menu or context menu
//-----------------------------------------------------------------------------
class Menu : public IMenu
{
public:
	enum Type
		{MT_CONTEXTMENU, MT_DROPDOWNMENU, MT_TITLEMENU};
private:
	HMENU handle;
	HWND hwndparent;
	Type menutype;
	MENUITEMADDED_CALLBACK itemaddedcbk;
	LPVOID itemaddedcbkuser;

public:
	std::list<MenuItem*> menuitems;
	WinForm* parent;

	Menu(WinForm* parent, Type menutype);
	void Show(int x, int y, UINT allignment);
	void Show();
	Result AddMenuItem(const String& text, IMenuItem::Type itemtype, IMenuItem* parentitem, const MENUITEMCLICK_CALLBACK clickcbk, LPVOID clickcbkuser, IMenuItem** menuitem);
	Result AddMenuItem(IMenuItem* menuitem, IMenuItem* parentitem, const String& text, const MENUITEMCLICK_CALLBACK clickcbk, LPVOID clickcbkuser);
	void SetMenuItemAddedCallback(MENUITEMADDED_CALLBACK cbk, LPVOID user) {itemaddedcbk = cbk; itemaddedcbkuser = user;}
	void CallOnMenuItemClicked(WORD itemid);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: IHasContextMenu
// Desc: Interface to a class capable of holding a context menu
//-----------------------------------------------------------------------------
class IHasContextMenu
{
private:
	Menu* ctxmenu;

public:
	WinForm* parentform;

	IHasContextMenu() : ctxmenu(NULL), parentform(NULL) {}
	Result CreateContextMenu(IMenu** ctxmenu);
	IMenu* GetContextMenu() {return ctxmenu;}
	void ShowContextMenu() {if(ctxmenu) ctxmenu->Show();}
	bool ContextMenuUsed() {return ctxmenu != NULL;}
};

//-----------------------------------------------------------------------------
// Name: Control
// Desc: Representing a non-top-level window
//-----------------------------------------------------------------------------
class Control : public IHasContextMenu
{
private:
	WNDPROC superclasswndproc;

	static LONG ControlWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void RemovePyCallbacks();

public:
	LPCONTROL ifc;
	struct ControlType
	{
		LPCTSTR wndbaseclass;
		LPCTSTR wndsuperclass;
		DWORD ctrlstyle, ctrlexstyle;
		LPCWSTR vsclass;
		int vspart;
	} type;
	HWND handle;
	HTHEME theme;
	Menu* ctxmenu;
	String text;
	HFONT font;
	Rect<int> bounds;
	bool visible, suspendlayout, enabled;
	CheckedState checkedstate;
	NOTIFY_CALLBACK notifycbk;
	TRACKMOUSEEVENT mousetracker;
	CONTROLMOUSEEVENT_CALLBACK mousedowncbk, mouseupcbk, mousemovecbk, mousedclickcbk, mousewheelcbk;
	MOUSECLICK_CALLBACK mclickcbk;
	KEYEVENT_CALLBACK keydowncbk, keyupcbk;
	LPVOID mousedowncbkuser, mouseupcbkuser, mousemovecbkuser, mousedclickcbkuser, mousewheelcbkuser;
	LPVOID notifycbkuser, mclickcbkuser, keydowncbkuser, keyupcbkuser;
	PyCallbackArgs *pymousedowncbk, *pymouseupcbk, *pymousemovecbk, *pymousedclickcbk, *pymousewheelcbk;
	PyCallbackArgs *pymclickcbk, *pykeydowncbk, *pykeyupcbk;

	Control(LPCTSTR wndbaseclass, LPCTSTR wndsuperclass, DWORD ctrlstyle, LPCWSTR vsclass, int vspart, WNDPROC& baseclasswndproc, WNDPROC superclasswndproc, DWORD ctrlexstyle = 0);
	~Control();

	static bool CreateSuperClass(LPCTSTR wndbaseclassname, LPCTSTR wndsuperclassname, WNDPROC& baseclasswndproc, WNDPROC superclasswndproc);

	Result OnAddControl(IContainer* parent, IControl* ifc);
	Result OnRemoveControl();
	void OnNotifyMessage(UINT notifycode);
	void Paint(int state);
	void UpdateVisualStyles();
	void SetVisible(bool val);
	void SetEnabled(bool val);
	void SetText(const String& val);
	void SetBounds(const Rect<int> &val);

	void SetChecked(bool val);
	void SetCheckedState(CheckedState val);

	void RaiseMouseDownCbk(const MouseEventArgs& args);
	void RaiseMouseUpCbk(const MouseEventArgs& args);
	void RaiseMouseMoveCbk(const MouseEventArgs& args);
	void RaiseMouseDClickCbk(const MouseEventArgs& args);
	void RaiseMouseWheelCbk(const MouseEventArgs& args);
	void RaiseMouseClickCbk();
	void RaiseKeyDownCbk(WORD vkey);
	void RaiseKeyUpCbk(WORD vkey);
};


//-----------------------------------------------------------------------------
// Name: Button
// Desc: Representing a push button
//-----------------------------------------------------------------------------
class Button : public IButton
{
private:
	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	Control basecontrol;

	Button(LPCTSTR wndbaseclass, LPCTSTR wndsuperclass, DWORD ctrlstyle, LPCWSTR vsclass, int vspart) :
		basecontrol(wndbaseclass, wndsuperclass, ctrlstyle, vsclass, vspart, BaseClassWndProc, (WNDPROC)SuperClassWndProc)
	{
	}
	~Button()
	{
	}

	BASECONTROL_ADAPTER_FUNCTIONS
};

//-----------------------------------------------------------------------------
// Name: CheckBox
// Desc: Representing a check box button
//-----------------------------------------------------------------------------
class CheckBox : public ICheckBox
{
private:
	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	Control basecontrol;

	CheckBox(LPCTSTR wndbaseclass, LPCTSTR wndsuperclass, DWORD ctrlstyle, LPCWSTR vsclass, int vspart) :
		basecontrol(wndbaseclass, wndsuperclass, ctrlstyle, vsclass, vspart, BaseClassWndProc, (WNDPROC)SuperClassWndProc)
	{
	}
	~CheckBox()
	{
	}

	BASECONTROL_ADAPTER_FUNCTIONS

	__forceinline bool GetChecked() {return (basecontrol.checkedstate != 0);}
	__forceinline void SetChecked(bool val) {basecontrol.SetCheckedState((CheckedState)val);}
	__forceinline CheckedState GetCheckedState() {return basecontrol.checkedstate;}
	__forceinline void SetCheckedState(CheckedState val) {basecontrol.SetCheckedState(val);}
};

//-----------------------------------------------------------------------------
// Name: RadioButton
// Desc: Representing a radio button
//-----------------------------------------------------------------------------
class RadioButton : public IRadioButton
{
private:
	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	Control basecontrol;

	RadioButton(LPCTSTR wndbaseclass, LPCTSTR wndsuperclass, DWORD ctrlstyle, LPCWSTR vsclass, int vspart) :
		basecontrol(wndbaseclass, wndsuperclass, ctrlstyle, vsclass, vspart, BaseClassWndProc, (WNDPROC)SuperClassWndProc)
	{
	}
	~RadioButton()
	{
	}

	BASECONTROL_ADAPTER_FUNCTIONS

	__forceinline bool GetChecked() {return (basecontrol.checkedstate != 0);}
	__forceinline void SetChecked(bool val) {basecontrol.SetCheckedState((CheckedState)val);}
};

//-----------------------------------------------------------------------------
// Name: GroupBox
// Desc: Representing a group box
//-----------------------------------------------------------------------------
class GroupBox : public IGroupBox
{
private:
	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	Control basecontrol;

	GroupBox(LPCTSTR wndbaseclass, LPCTSTR wndsuperclass, DWORD ctrlstyle, LPCWSTR vsclass, int vspart) :
		basecontrol(wndbaseclass, wndsuperclass, ctrlstyle, vsclass, vspart, BaseClassWndProc, (WNDPROC)SuperClassWndProc)
	{
	}
	~GroupBox()
	{
	}

	BASECONTROL_ADAPTER_FUNCTIONS
};

//-----------------------------------------------------------------------------
// Name: Label
// Desc: Representing a label
//-----------------------------------------------------------------------------
class Label : public ILabel
{
private:
	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	Control basecontrol;

	Label(LPCTSTR wndbaseclass, LPCTSTR wndsuperclass, DWORD ctrlstyle, LPCWSTR vsclass, int vspart) :
		basecontrol(wndbaseclass, wndsuperclass, ctrlstyle, vsclass, vspart, BaseClassWndProc, (WNDPROC)SuperClassWndProc)
	{
	}
	~Label()
	{
	}

	BASECONTROL_ADAPTER_FUNCTIONS
};

//-----------------------------------------------------------------------------
// Name: TextBox
// Desc: Representing a text box
//-----------------------------------------------------------------------------
class TextBox : public ITextBox
{
private:
	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	Control basecontrol;

	TextBox(LPCTSTR wndbaseclass, LPCTSTR wndsuperclass, DWORD ctrlstyle, LPCWSTR vsclass, int vspart) :
		basecontrol(wndbaseclass, wndsuperclass, ctrlstyle, vsclass, vspart, BaseClassWndProc, (WNDPROC)SuperClassWndProc)
	{
	}
	~TextBox()
	{
	}

	BASECONTROL_ADAPTER_FUNCTIONS

	void AppendText(const String& text);
};

//-----------------------------------------------------------------------------
// Name: RichEdit
// Desc: Representing a text box with rich text formatting options
//-----------------------------------------------------------------------------
class RichEdit : public IRichEdit
{
private:
	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	Control basecontrol;

	RichEdit(LPCTSTR wndbaseclass, LPCTSTR wndsuperclass, DWORD ctrlstyle, LPCWSTR vsclass, int vspart) :
		basecontrol(wndbaseclass, wndsuperclass, ctrlstyle, vsclass, vspart, BaseClassWndProc, (WNDPROC)SuperClassWndProc)
	{
	}
	~RichEdit()
	{
	}

	BASECONTROL_ADAPTER_FUNCTIONS

	void SetFormat(COLORREF fgcolor, COLORREF bgcolor, const String& font);
	void InsertText(const String& text);
};

//-----------------------------------------------------------------------------
// Name: NumericUpDown
// Desc: Representing a text box with up and down arrows
//-----------------------------------------------------------------------------
class NumericUpDown : public INumericUpDown
{
private:
	short value, minvalue, maxvalue;
	WinForm* parentform;
	_VALUECHANGED_CALLBACK vccbk;
	LPVOID vccbkuser;

	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	HWND handle;
	Control basecontrol;

	NumericUpDown(WinForm* parentform);
	~NumericUpDown();

	BASECONTROL_ADAPTER_FUNCTIONS

	short SetValue(short v);
	short GetValue();
	short SetMinValue(short v);
	short GetMinValue() {return minvalue;}
	short SetMaxValue(short v);
	short GetMaxValue() {return maxvalue;}

	void OnNotifyMessage(LPNMUPDOWN nmupdown);
	void SetValueChangedCallback(const _VALUECHANGED_CALLBACK cbk, LPVOID user) {vccbk = cbk; vccbkuser = user;}
};

//-----------------------------------------------------------------------------
// Name: Listview
// Desc: Representing a list view
//-----------------------------------------------------------------------------
class Listview : public IListview
{
private:
	bool groupsenabled;

	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	Control basecontrol;

	Listview();
	~Listview();

	BASECONTROL_ADAPTER_FUNCTIONS

	bool SetGroupsEnabled(bool v);
	bool GetGroupsEnabled() {return groupsenabled;}

	void InsertGroup(const String& caption, int groupindex);
	void InsertColumn(const String& caption, int width, int columnindex);
	void InsertItem(const String& text, char textdelimiter, int groupindex, int rowindex);
	void DeleteItem(int rowindex);
	void DeleteAllItems();
	/*void SetView();*/
};

//-----------------------------------------------------------------------------
// Name: TreeviewItem
// Desc: Representing an item in the treeview
//-----------------------------------------------------------------------------
class Treeview;
class TreeviewItem : public ITreeviewItem
{
private:
	Treeview* parent;
	HTREEITEM handle;
	String text;

public:
	TreeviewItem(Treeview* parent) : parent(parent) {}
	Result Init(const String& text, const TreeviewItem* parentitem);
	/*const String& GetText() {return text;}
	void SetText(const String& val);*/
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Treeview
// Desc: Representing a treeview control
//-----------------------------------------------------------------------------
class Treeview : public ITreeview
{
private:
	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	Control basecontrol;
	std::list<TreeviewItem*> items;

	Treeview();
	~Treeview();

	BASECONTROL_ADAPTER_FUNCTIONS

	Result AddItem(const String& text, const ITreeviewItem* parentitem, ITreeviewItem** item);
};

//-----------------------------------------------------------------------------
// Name: TrayIcon
// Desc: Representing a notification icon in the taskbar
//-----------------------------------------------------------------------------
class TrayIcon : public ITrayIcon, public IHasContextMenu
{
private:
	UINT id;

public:

	TrayIcon();
	Result Init(const FilePath& filename, WinForm* parent);
	Result SetToolTipText(const String& text);
	Result SetNotifycationBaloon(const String& text, const String& title, const FilePath& iconfilename, IconType icontype);
	Result CreateContextMenu(IMenu** ctxmenu) {return IHasContextMenu::CreateContextMenu(ctxmenu);}
	LPMENU GetContextMenu() {return IHasContextMenu::GetContextMenu();}
	void ShowContextMenu() {IHasContextMenu::ShowContextMenu();}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: ToolbarItem
// Desc: Representing an item in the toolbar
//-----------------------------------------------------------------------------
class Toolbar;
class ToolbarItem : public IToolbarItem
{
private:
	Toolbar* parent;
	const Type type;
	const int id;
	String text;
	Menu* dropdownmenu;

public:
	TOOLBARITEMCLICK_CALLBACK clickcbk;
	LPVOID clickcbkuser;

	ToolbarItem(Toolbar* parent, Type type, int id) : parent(parent), type(type), id(id), dropdownmenu(NULL) {}
	Result Init(const String& text, int imageidx);
	const String& GetText() {return text;}
	void SetText(const String& val);
	INT_PTR GetImage();
	Result SetImage(INT_PTR imageresid, DWORD imageclrkey);
	void SetWidth(WORD newwidth);
	bool GetCheckedState();
	void SetCheckedState(bool val);
	bool GetEnabledState();
	void SetEnabledState(bool val);
	void SetClickCallback(const TOOLBARITEMCLICK_CALLBACK clickcbk, LPVOID user) {this->clickcbk = clickcbk; clickcbkuser = user;}
	Result CreateDropDownMenu(IMenu** dropdownmenu);
	IMenu* GetDropDownMenu() {return dropdownmenu;}
	void ShowDropDownMenu();
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Toolbar
// Desc: Representing a toolbar menu inside a form window
//-----------------------------------------------------------------------------
class Toolbar : public IToolbar, public IHasContextMenu
{
private:
	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	std::list<ToolbarItem*> items;
	struct ImageInfo
	{
		INT_PTR resid;
		HBITMAP hbmp;
		ImageInfo() {}
		ImageInfo(INT_PTR resid, HBITMAP hbmp) : resid(resid), hbmp(hbmp) {}
	};
	std::map<int, ImageInfo> imagelist;
	HWND handle;

	Toolbar();
	Result Init(bool useliststyle, WinForm* parent);
	Result AddButton(const String& text, IToolbarItem::Type btntype, const TOOLBARITEMCLICK_CALLBACK clickcbk, LPVOID clickcbkuser, IToolbarItem** item);
	Result AddButton(const String& text, INT_PTR imageresid, DWORD imageclrkey, IToolbarItem::Type btntype, const TOOLBARITEMCLICK_CALLBACK clickcbk, LPVOID clickcbkuser, IToolbarItem** item);
	Size<int> GetSize();
	void OnNotifyMessage(const NMTOOLBAR* nmtoolbar);
	Result CreateContextMenu(IMenu** ctxmenu) {return IHasContextMenu::CreateContextMenu(ctxmenu);}
	LPMENU GetContextMenu() {return IHasContextMenu::GetContextMenu();}
	void ShowContextMenu() {IHasContextMenu::ShowContextMenu();}
	Result LoadImage(INT_PTR imageresid, DWORD imageclrkey, int& imageidx, int& imgwidth);
	Result ReplaceImage(INT_PTR imageresid, DWORD imageclrkey, int& imageidx, int& imgwidth);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: TabControl
// Desc: Representing an area divided into multiple selectable tabs
//-----------------------------------------------------------------------------
class TabControl : public ITabControl, IContainer
{
private:
	std::vector<LPVOID> tags;
	TABCHANGED_CALLBACK tabchangedcbk;
	LPVOID tabchangedcbkuser;

	static WNDPROC BaseClassWndProc;
	static LONG SuperClassWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static void __stdcall OnNotifyMessage(UINT notifycode, LPVOID user);

public:
	Control basecontrol;

	TabControl();
	~TabControl();

	BASECONTROL_ADAPTER_FUNCTIONS

	Result AddControl(IControl* ctrl) {return IContainer::AddControl(ctrl);}
	Result RemoveControl(IControl* ctrl) {return IContainer::RemoveControl(ctrl);}
	Result RemoveAllControls() {return IContainer::RemoveAllControls();}
	WinForm* GetParentForm() {return basecontrol.parentform;}
	void InsertTab(const String& caption, int tabindex, LPVOID tag);
	Rect<>* GetClientBounds(Rect<>* bounds);
	int GetCurrentTabIndex();
	LPVOID GetCurrentTabTag();
	void ChangeTab(int tabindex);
	void ChangeTab(LPVOID tabtag);
	void SetTabChangedCallback(const TABCHANGED_CALLBACK cbk, LPVOID user) {tabchangedcbk = cbk; tabchangedcbkuser = user;}
};

//-----------------------------------------------------------------------------
// Name: Tooltip
// Desc: Representing a virtual control that pops up and displays notifications when hovering over registered windows
//-----------------------------------------------------------------------------
class Tooltip : public ITooltip
{
private:
	HWND parentwnd;
	TOOLTIPSHOW_CALLBACK showcbk;
	TOOLTIPHIDE_CALLBACK hidecbk;
	LPVOID showcbkuser, hidecbkuser;

public:
	HWND handle;

	Tooltip(HWND parentwnd);

	void OnNotifyMessage(UINT notifycode, const NMHDR* nmhdr);
	HWND GetHwnd() {return handle;}
	void Popup();
	void SetTooltip(HWND wnd, const String& text);
	void SetTooltip(HWND wnd, GETTOOLTIPTEXT_CALLBACK cbk);
	void SetTooltipShowCallback(TOOLTIPSHOW_CALLBACK cbk, LPVOID user) {showcbk = cbk; showcbkuser = user;}
	void SetTooltipHideCallback(TOOLTIPHIDE_CALLBACK cbk, LPVOID user) {hidecbk = cbk; hidecbkuser = user;}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: WinForm
// Desc: Representing a single top-level window
//-----------------------------------------------------------------------------
class WinForm : public IWinForm, public IContainer, IDropTarget, IHasContextMenu
{
private:
	struct MouseTracker
	{
		TRACKMOUSEEVENT tracker;
		bool enabled;

		MouseTracker() : enabled(false)
		{
			tracker.cbSize = sizeof(TRACKMOUSEEVENT);
			tracker.dwFlags = TME_LEAVE;
			tracker.hwndTrack = NULL;
		}
		void Init(HWND parentwnd) {tracker.hwndTrack = parentwnd;}
	} tracker;
	DRAGDROP_CALLBACK dragentercbk, dragovercbk, dropcbk;
	DRAGLEAVE_CALLBACK dragleavecbk;
	LPVOID dragentercbkuser, dragovercbkuser, dropcbkuser, dragleavecbkuser;
	bool dragdropenabled;
	IDataObject* dragenterobj; // Needed for DragOver()
	DWORD olddrageffect;
	bool visible;
	Menu* titlemenu;
	String classname;
	Rect<int> entersizemovebounds;
	Tooltip* tooltip;

	// IDropTarget interfaces:
	HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* dataobj, DWORD keystate, POINTL mousepos, DWORD* effect);
	HRESULT STDMETHODCALLTYPE DragOver(DWORD keystate, POINTL mousepos, DWORD* effect);
	HRESULT STDMETHODCALLTYPE DragLeave();
	HRESULT STDMETHODCALLTYPE Drop(IDataObject* dataobj, DWORD keystate, POINTL mousepos, DWORD* effect);

	// >>> Simple IUnknown implementation
	DWORD iunknownrefctr;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
		{if(ppvObject) return S_OK; else return E_POINTER;}
	ULONG STDMETHODCALLTYPE AddRef()
		{return ++iunknownrefctr;}
	ULONG STDMETHODCALLTYPE Release()
		{long temp = --iunknownrefctr; if(!temp) delete this; return temp;}
	// <<< Simple IUnknown implementation

	void GetBoundsInternal(Rect<int>& bounds) const;
	void GetClientBoundsInternal(Rect<int>& bounds) const;
	void SetBoundsInternal(const Rect<int>& bounds);
	void SetClientBoundsInternal(const Rect<int>& bounds);
	void GetNormalBoundsInternal(Rect<int>& bounds) const;
	void GetNormalClientBoundsInternal(Rect<int>& bounds) const;

	void RaiseMouseDownCbk(const MouseEventArgs& args);
	void RaiseMouseUpCbk(const MouseEventArgs& args);
	void RaiseMouseMoveCbk(const MouseEventArgs& args);
	void RaiseMouseDClickCbk(const MouseEventArgs& args);
	void RaiseMouseWheelCbk(const MouseEventArgs& args);
	void RaisePaintCbk();
	void RemovePyCallbacks();

public:
	WinForm* parent;
	MOUSEEVENT_CALLBACK mousedowncbk, mouseupcbk, mousemovecbk, mousedclickcbk, mousewheelcbk;
	PAINT_CALLBACK paintcbk;
	FORMCLOSING_CALLBACK fclosingcbk;
	FORMCLOSED_CALLBACK fclosedcbk;
	FORMRESIZE_CALLBACK fresizecbk;
	FORMSIZECHANGED_CALLBACK fsizechangedcbk;
	FORMMOVING_CALLBACK fmovingcbk;
	FORMMOVED_CALLBACK fmovedcbk;
	LPVOID mousedowncbkuser, mouseupcbkuser, mousemovecbkuser, mousedclickcbkuser, mousewheelcbkuser, paintcbkuser;
	LPVOID fclosingcbkuser, fclosedcbkuser, fresizecbkuser, fsizechangedcbkuser, fmovingcbkuser, fmovedcbkuser;
	PyCallbackArgs *pymousedowncbk, *pymouseupcbk, *pymousemovecbk, *pymousedclickcbk, *pymousewheelcbk, *pypaintcbk, *pydragentercbk, *pydragovercbk,
				   *pydragleavecbk, *pydropcbk, *pyfresizecbk, *pyfsizechangedcbk, *pyfmovingcbk, *pyfmovedcbk, *pyfclosingcbk, *pyfclosedcbk;
	HWND handle, mdiclienthandle;
	HINSTANCE instance;
	String caption;
	std::list<TrayIcon*> trayicons;
	std::list<Toolbar*> toolbars;
	std::list<NumericUpDown*> nuds;
	std::list<Menu*> menus;
	std::list<WinForm*> childforms;
	WindowState oldwndstate;

	WinForm();
	~WinForm();

	int GetToolbarAndMenuHeight() const;
	int GetCaptionHeight() const;
	WinForm* GetParentForm() {return this;}

	Result Init(FormSettings *settings, const String* classname);
	Result Init(HWND handle);
	Result AddControl(IControl* ctrl) {return IContainer::AddControl(ctrl);}
	Result RemoveControl(IControl* ctrl) {return IContainer::RemoveControl(ctrl);}
	Result RemoveAllControls() {return IContainer::RemoveAllControls();}
	Result CreateNumericUpDown(INumericUpDown** nud);
	Result CreateTrayIcon(const FilePath& iconfilename, ITrayIcon** trayicon);
	Result CreateToolbar(bool useliststyle, IToolbar** toolbar);
	void Show();
	void Hide();
	bool IsVisible() {return visible;}
	void BringToFront();
	__forceinline HWND GetHwnd() {return handle;}
	__forceinline Point<int> GetLocation() {Rect<int> bounds; GetBoundsInternal(bounds); return bounds.location;}
	__forceinline void SetLocation(Point<int> &val) {Rect<int> bounds; GetBoundsInternal(bounds); SetBoundsInternal(Rect<int>(val, bounds.size));}
	__forceinline ::Size<int> GetClientSize() {Rect<int> bounds; GetClientBoundsInternal(bounds); return bounds.size;}
	__forceinline void SetClientSize(::Size<int> &val) {Rect<int> bounds; GetClientBoundsInternal(bounds); SetClientBoundsInternal(Rect<int>(bounds.location, val));}
	__forceinline int GetTop() {Rect<int> bounds; GetBoundsInternal(bounds); return bounds.y;}
	__forceinline void SetTop(int val) {Rect<int> bounds; GetBoundsInternal(bounds); SetBoundsInternal(Rect<int>(bounds.x, val, bounds.width, bounds.height));}
	__forceinline int GetLeft() {Rect<int> bounds; GetBoundsInternal(bounds); return bounds.x;}
	__forceinline void SetLeft(int val) {Rect<int> bounds; GetBoundsInternal(bounds); SetBoundsInternal(Rect<int>(val, bounds.y, bounds.width, bounds.height));}
	__forceinline int GetClientWidth() {Rect<int> bounds; GetClientBoundsInternal(bounds); return bounds.width;}
	__forceinline void SetClientWidth(int val) {Rect<int> bounds; GetClientBoundsInternal(bounds); SetClientBoundsInternal(Rect<int>(bounds.x, bounds.y, val, bounds.height));}
	__forceinline int GetClientHeight() {Rect<int> bounds; GetClientBoundsInternal(bounds); return bounds.height;}
	__forceinline void SetClientHeight(int val) {Rect<int> bounds; GetClientBoundsInternal(bounds); SetClientBoundsInternal(Rect<int>(bounds.x, bounds.y, bounds.width, val));}
	__forceinline Rect<int> GetBounds() {Rect<int> bounds; GetBoundsInternal(bounds); return bounds;}
	__forceinline void SetBounds(const Rect<int>& val) {SetBoundsInternal(val);}
	__forceinline Rect<int> GetClientBounds() {Rect<int> bounds; GetClientBoundsInternal(bounds); return bounds;}
	__forceinline void SetClientBounds(const Rect<int>& val) {SetClientBoundsInternal(val);}
	__forceinline WindowState GetWindowState();
	__forceinline WindowState SetWindowState(WindowState val);
	__forceinline Point<int> GetNormalLocation() {Rect<int> bounds; GetNormalBoundsInternal(bounds); return bounds.location;}
	__forceinline ::Size<int> GetNormalClientSize() {Rect<int> bounds; GetNormalClientBoundsInternal(bounds); return bounds.size;}
	__forceinline int GetNormalTop() {Rect<int> bounds; GetNormalBoundsInternal(bounds); return bounds.y;}
	__forceinline int GetNormalLeft() {Rect<int> bounds; GetNormalBoundsInternal(bounds); return bounds.x;}
	__forceinline int GetNormalClientWidth() {Rect<int> bounds; GetNormalClientBoundsInternal(bounds); return bounds.width;}
	__forceinline int GetNormalClientHeight() {Rect<int> bounds; GetNormalClientBoundsInternal(bounds); return bounds.height;}
	__forceinline FormBorderStyle GetFormBorderStyle();
	__forceinline FormBorderStyle SetFormBorderStyle(FormBorderStyle val);
	__forceinline bool SetMaximizeBox(bool hasmaximizebox);
	__forceinline bool GetMaximizeBox();
	__forceinline bool SetMinimizeBox(bool hasminimizebo);
	__forceinline bool GetMinimizeBox();
	__forceinline const String& GetClassName() const {return classname;}
	Result EnableDragDrop();
	Result DisableDragDrop();
	Result CreateTitleMenu(IMenu** titlemenu);
	LPTOOLTIP GetTooltip();
	void SetMouseDownCallback(const MOUSEEVENT_CALLBACK cbk, LPVOID user) {mousedowncbk = cbk; mousedowncbkuser = user;}
	void SetMouseDownCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetMouseUpCallback(const MOUSEEVENT_CALLBACK cbk, LPVOID user) {mouseupcbk = cbk; mouseupcbkuser = user;}
	void SetMouseUpCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetMouseMoveCallback(const MOUSEEVENT_CALLBACK cbk, LPVOID user) {mousemovecbk = cbk; mousemovecbkuser = user;}
	void SetMouseMoveCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetMouseDoubleClickCallback(const MOUSEEVENT_CALLBACK cbk, LPVOID user) {mousedclickcbk = cbk; mousedclickcbkuser = user;}
	void SetMouseDoubleClickCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetMouseWheelCallback(const MOUSEEVENT_CALLBACK cbk, LPVOID user) {mousewheelcbk = cbk; mousewheelcbkuser = user;}
	void SetMouseWheelCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetPaintCallback(const PAINT_CALLBACK cbk, LPVOID user) {paintcbk = cbk; paintcbkuser = user;}
	void SetPaintCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetDragEnterCallback(const DRAGDROP_CALLBACK dragentercbk, LPVOID user) {this->dragentercbk = dragentercbk; dragentercbkuser = user;}
	void SetDragEnterCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetDragOverCallback(const DRAGDROP_CALLBACK dragovercbk, LPVOID user) {this->dragovercbk = dragovercbk; dragovercbkuser = user;}
	void SetDragOverCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetDragLeaveCallback(const DRAGLEAVE_CALLBACK dragleavecbk, LPVOID user) {this->dragleavecbk = dragleavecbk; dragleavecbkuser = user;}
	void SetDragLeaveCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetDropCallback(const DRAGDROP_CALLBACK dropcbk, LPVOID user) {this->dropcbk = dropcbk; dropcbkuser = user;}
	void SetDropCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetFormResizeCallback(const FORMRESIZE_CALLBACK fresizecbk, LPVOID user) {this->fresizecbk = fresizecbk; fresizecbkuser = user;}
	void SetFormResizeCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetFormSizeChangedCallback(const FORMSIZECHANGED_CALLBACK fsizechangedcbk, LPVOID user) {this->fsizechangedcbk = fsizechangedcbk; fsizechangedcbkuser = user;}
	void SetFormSizeChangedCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetFormMovingCallback(const FORMMOVING_CALLBACK fmovingcbk, LPVOID user) {this->fmovingcbk = fmovingcbk; fmovingcbkuser = user;}
	void SetFormMovingCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetFormMovedCallback(const FORMMOVED_CALLBACK fmovedcbk, LPVOID user) {this->fmovedcbk = fmovedcbk; fmovedcbkuser = user;}
	void SetFormMovedCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetFormClosingCallback(const FORMCLOSING_CALLBACK fclosingcbk, LPVOID user) {this->fclosingcbk = fclosingcbk; fclosingcbkuser = user;}
	void SetFormClosingCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void SetFormClosedCallback(const FORMCLOSED_CALLBACK fclosedcbk, LPVOID user) {this->fclosedcbk = fclosedcbk; fclosedcbkuser = user;}
	void SetFormClosedCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user);
	void OnMouseDown(const MouseEventArgs& args);
	void OnMouseUp(const MouseEventArgs& args);
	void OnMouseMove(const MouseEventArgs& args);
	void OnMouseDoubleClick(const MouseEventArgs& args);
	void OnMouseWheel(const MouseEventArgs& args);
	void OnMouseLeave(const MouseEventArgs& args);
	void OnPaint();
	void OnButtonMessage(HWND handle, WORD notifycode);
	void OnMenuMessage(WORD menuid);
	void OnTrayIconMessage(LPARAM lparam);
	void OnNotifyMessage(const NMHDR* nmhdr);
	void OnFormSizing(Rect<int>* bounds, byte sizeedge);
	void OnFormMoving(Rect<int>* bounds);
	void OnFormEnterSizeMove();
	void OnFormExitSizeMove();
	void OnContextMenuMessage();
	Result CreateContextMenu(IMenu** ctxmenu) {return IHasContextMenu::CreateContextMenu(ctxmenu);}
	LPMENU GetContextMenu() {return IHasContextMenu::GetContextMenu();}
	void ShowContextMenu() {IHasContextMenu::ShowContextMenu();}
	void Close(bool* cancel);
	bool CloseWrapper()
	{
		bool cancel;
		Close(&cancel);
		return !cancel;
	}
};

struct IFileDialogEvents;
class OpenFileDialog : public IOpenFileDialog
{
private:
	IFileDialog* dialog;
	IFileDialogEvents* dialogevents;
	DWORD cookie;

public:

	OpenFileDialog() : dialog(NULL), dialogevents(NULL), cookie(0xFFFFFFFF) {files = NULL; numfiles = 0;}
	Result Init();
	Result SetFileTypes(std::vector<FileType>& filetypes, UINT defaultindex) {return SetFileTypes(filetypes.size() ? &filetypes[0] : NULL, filetypes.size(), defaultindex);}
	Result SetFileTypes(FileType* filetypes, UINT numfiletypes, UINT defaultindex);
	Result SetDefaultExtension(String extension);
	Result Show(HWND parent);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Forms
// Desc: Main Forms subapplication class.
//-----------------------------------------------------------------------------
class Forms : public IForms
{
private:
	std::vector<WinForm*> forms;
	std::list<IFormDependend*> frmdeplist;
	std::list<IKeyboardMonitoring*> keymonlist;

	void CallOnCreateForm(HWND wnd);
	void CallOnRemoveForm(HWND wnd);

public:
	HINSTANCE instance;
	bool visualstylesenabled;

	__forceinline HINSTANCE GetInstance() {return instance;}
	__forceinline LPWINFORM GetWinForm(size_t index)
		{return index < forms.size() ? forms[index] : NULL;}
	__forceinline HWND GetHwnd(size_t index)
		{return index < forms.size() ? forms[index]->handle : NULL;}
	__forceinline HWND GetMDIClientHwnd(size_t index)
		{return index < forms.size() ? forms[index]->mdiclienthandle : NULL;}
	__forceinline int GetNumForms()
		{return forms.size();}
	__forceinline bool VisualStylesEnabled()
		{return visualstylesenabled;}

	Forms();

	void Sync(GLOBALVARDEF_LIST);
	Result Init(HINSTANCE instance);
	Result CreateForm(FormSettings *settings, const String* classname, IWinForm** winform = NULL);
	WinForm* CreateFormWrapper(FormSettings *settings, const String* classname)
	{
		WinForm* winform;
		CreateForm(settings, classname, (LPWINFORM*)&winform);
		return winform;
	}
	WinForm* CreateFormWrapper2(FormSettings *settings)
		{return CreateFormWrapper(settings, NULL);}
	Result CreateForm(HWND handle, IWinForm** winform);
	WinForm* CreateFormFromHwndWrapper(HWND handle)
	{
		WinForm* winform;
		CreateForm(handle, (LPWINFORM*)&winform);
		return winform;
	}
	void CloseForm(size_t index, bool* cancel);
	void CloseFormByHwnd(HWND wnd, bool* cancel);
	void ShowAll();
	void CloseAll(bool* cancel);
	bool CloseAllWrapper()
	{
		bool cancel;
		CloseAll(&cancel);
		return !cancel;
	}
	void EnableVisualStates();
	void DisableVisualStates();
	void CallOnGainFocus(HWND wnd);
	void CallOnLoseFocus(HWND wnd);
	void CallOnKeyDown(DWORD vkey);
	void CallOnKeyUp(DWORD vkey);
	void OnPaint(HWND wnd);
	void OnMouseDown(HWND wnd, const MouseEventArgs& args);
	void OnMouseUp(HWND wnd, const MouseEventArgs& args);
	void OnMouseMove(HWND wnd, const MouseEventArgs& args);
	void OnMouseDoubleClick(HWND wnd, const MouseEventArgs& args);
	void OnMouseWheel(HWND wnd, const MouseEventArgs& args);
	void OnMouseLeave(HWND wnd, const MouseEventArgs& args);
	LRESULT OnFormClosing(HWND wnd);
	bool OnFormResize(HWND wnd, Size<UINT> size, WindowState wndstate);
	void OnFormSizing(HWND wnd, Rect<int>* bounds, byte sizeedge);
	void OnFormMoving(HWND wnd, Rect<int>* bounds);
	void OnFormEnterSizeMove(HWND wnd);
	void OnFormExitSizeMove(HWND wnd);
	void OnButtonMessage(HWND wnd, HWND handle, WORD notifycode);
	void OnMenuMessage(HWND wnd, WORD menuid);
	void OnTrayIconMessage(HWND wnd, LPARAM lparam);
	void OnNotifyMessage(HWND wnd, const NMHDR* nmhdr);
	void OnFormContextMenuMessage(HWND wnd);
	void RegisterForFormDependence(IFormDependend* cls);
	void DeregisterFromFormDependence(IFormDependend* cls);
	void RegisterForKeyboardMonitoring(IKeyboardMonitoring* cls);
	void DeregisterFromKeyboardMonitoring(IKeyboardMonitoring* cls);
	void Release();
};

void SyncWithPython();

#endif