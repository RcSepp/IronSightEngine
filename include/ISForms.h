#ifndef __ISFORMS_H
#define __ISFORMS_H

#include <ISEngine.h>


//-----------------------------------------------------------------------------
// CONSTANTS
//-----------------------------------------------------------------------------
#define WM_TRAYICONMSG				WM_APP + 0x100


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_FORMS
	#define FORMS_EXTERN_FUNC		__declspec(dllexport)
#else
	#define FORMS_EXTERN_FUNC		__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum CheckedState
	{CS_UNCHECKED, CS_CHECKED, CS_INDETERMINATE};
enum WindowState
	{WS_NORMAL, WS_MINIMIZED, WS_MAXIMIZED};
enum FormBorderStyle
{
	FBS_DEFAULT = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
	FBS_THINBORDER = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
	FBS_NOBORDER = WS_OVERLAPPED,
	FBS_ONLYBORDER = WS_THICKFRAME
};


class IControl;
class IWinForm;
class IMenu;
class IMenuItem;
class IToolbarItem;
class ITabControl;
class ITooltip;
class INumericUpDown;

//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
typedef void (__stdcall* MOUSECLICK_CALLBACK)(IControl* sender, LPVOID user);
typedef void (__stdcall* MOUSEEVENT_CALLBACK)(IWinForm* sender, const MouseEventArgs& args, LPVOID user);
typedef void (__stdcall* CONTROLMOUSEEVENT_CALLBACK)(IControl* sender, const MouseEventArgs& args, LPVOID user);
typedef void (__stdcall* KEYEVENT_CALLBACK)(IControl* sender, WORD vkey, LPVOID user);
typedef void (__stdcall* PAINT_CALLBACK)(IWinForm* sender, LPVOID user);
typedef void (__stdcall* DRAGDROP_CALLBACK)(IDataObject* dataobj, DWORD keystate, Point<int>& mousepos, DWORD* dropeffect, LPVOID user);
typedef void (__stdcall* DRAGLEAVE_CALLBACK)(LPVOID user);
typedef void (__stdcall* FORMRESIZE_CALLBACK)(IWinForm* sender, Rect<int>* newsize, byte sizeedge, LPVOID user);
typedef void (__stdcall* FORMSIZECHANGED_CALLBACK)(IWinForm* sender, Size<int> newsize, LPVOID user);
typedef void (__stdcall* FORMMOVING_CALLBACK)(IWinForm* sender, Rect<int>* bounds, LPVOID user);
typedef void (__stdcall* FORMMOVED_CALLBACK)(IWinForm* sender, Point<int> newpos, LPVOID user);
typedef void (__stdcall* FORMCLOSING_CALLBACK)(IWinForm* sender, LPVOID user, bool& cancel);
typedef void (__stdcall* FORMCLOSED_CALLBACK)(IWinForm* sender, LPVOID user);
typedef void (__stdcall* MENUITEMCLICK_CALLBACK)(IMenuItem* sender, LPVOID user);
typedef void (__stdcall* MENUITEMADDED_CALLBACK)(IMenu* sender, IMenuItem* item, LPVOID user);
typedef void (__stdcall* TOOLBARITEMCLICK_CALLBACK)(IToolbarItem* sender, LPVOID user);
typedef void (__stdcall* TABCHANGED_CALLBACK)(ITabControl* sender, int tabindex, LPVOID tag, LPVOID user);
typedef String (__stdcall* GETTOOLTIPTEXT_CALLBACK)(ITooltip* sender, HWND controlhandle);
typedef void (__stdcall* TOOLTIPSHOW_CALLBACK)(ITooltip* sender, HWND controlhandle, LPVOID user);
typedef void (__stdcall* TOOLTIPHIDE_CALLBACK)(ITooltip* sender, HWND controlhandle, LPVOID user);
typedef void (__stdcall* _VALUECHANGED_CALLBACK)(INumericUpDown* sender, short value, LPVOID user);


//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct FormSettings
{
	String caption;
	Point<int> windowpos;
	Size<int> clientsize;
	WindowState wndstate;
	FormBorderStyle fbstyle;
	bool istoolwindow, hasvscroll, hashscroll;
	IWinForm* parent;
	IMenuItem** mdiwindowmenu;
	DWORD options;

	FormSettings() : caption(""), windowpos(0, 0), clientsize(800, 600), wndstate(WS_NORMAL), fbstyle(FBS_DEFAULT),
					 istoolwindow(false), hasvscroll(false), hashscroll(false), parent(NULL), mdiwindowmenu(NULL), options(0) {}
};
#define FS_CENTERTOSCREEN		0x1
#define FS_MDICLIENT			0x2
#define FS_MDICHILD				0x4


//-----------------------------------------------------------------------------
// Name: IMenuItem
// Desc: Interface to the MenuItem class of ISForms
//-----------------------------------------------------------------------------
typedef class IMenuItem
{
public:
	enum Type
		{MI_BUTTON, MI_DROPDOWN, MI_DROPDOWN_STATIC, MI_SEPARATOR};
	virtual String GetText() = 0;
	virtual void SetText(const String& val) = 0;
	__declspec(property(get=GetText, put=SetText)) String Text;
	virtual void SetClickCallback(const MENUITEMCLICK_CALLBACK clickcbk, LPVOID user) = 0;
	virtual void Release() = 0;
}* LPMENUITEM;

//-----------------------------------------------------------------------------
// Name: IMenu
// Desc: Interface to the Menu class of ISForms
//-----------------------------------------------------------------------------
typedef class IMenu
{
public:
	virtual void Show(int x, int y, UINT allignment = TPM_BOTTOMALIGN) = 0;
	virtual void Show() = 0;
	virtual Result AddMenuItem(const String& text = "", IMenuItem::Type itemtype = IMenuItem::MI_BUTTON, IMenuItem* parentitem = NULL,
							   const MENUITEMCLICK_CALLBACK clickcbk = NULL, LPVOID clickcbkuser = NULL, IMenuItem** menuitem = NULL) = 0;
	virtual Result AddMenuItem(IMenuItem* menuitem, IMenuItem* parentitem = NULL, const String& text = "", const MENUITEMCLICK_CALLBACK clickcbk = NULL, LPVOID clickcbkuser = NULL) = 0;
	virtual void SetMenuItemAddedCallback(MENUITEMADDED_CALLBACK cbk, LPVOID user) = 0;
	virtual void Release() = 0;
}* LPMENU;

//-----------------------------------------------------------------------------
// Name: IIHasContextMenu
// Desc: Interface to the IHasContextMenu interface of ISForms
//-----------------------------------------------------------------------------
class IIHasContextMenu
{
public:
	virtual Result CreateContextMenu(IMenu** ctxmenu) = 0;
	virtual LPMENU GetContextMenu() = 0;
	virtual void ShowContextMenu() = 0;
};

//-----------------------------------------------------------------------------
// Name: IControl
// Desc: Interface to the Control class of ISForms
//-----------------------------------------------------------------------------
typedef class IControl : public IIHasContextMenu
{
public:
	virtual bool GetVisible() = 0;
	virtual void SetVisible(bool val) = 0;
	virtual void SuspendLayout() = 0;
	virtual void ResumeLayout() = 0;
	virtual HWND GetHwnd() = 0;
	__declspec(property(get=GetVisible, put=SetVisible)) bool Visible;
	virtual bool GetEnabled() = 0;
	virtual void SetEnabled(bool val) = 0;
	__declspec(property(get=GetEnabled, put=SetEnabled)) bool Enabled;
	virtual String GetText() const = 0;
	virtual void SetText(const String& val) = 0;
	__declspec(property(get=GetText, put=SetText)) String Text;
	virtual Rect<int> GetBounds() = 0;
	virtual void SetBounds(const Rect<int> &val) = 0;
	__declspec(property(get=GetBounds, put=SetBounds)) Rect<int> Bounds;
	virtual Point<int> GetLocation() = 0;
	virtual void SetLocation(const Point<int> &val) = 0;
	__declspec(property(get=GetLocation, put=SetLocation)) Point<int> Location;
	virtual ::Size<int> GetSize() = 0;
	virtual void SetSize(const ::Size<int> &val) = 0;
	__declspec(property(get=GetSize, put=SetSize)) Size<int> Size;
	virtual int GetTop() = 0;
	virtual void SetTop(int val) = 0;
	__declspec(property(get=GetTop, put=SetTop)) int Top;
	virtual int GetLeft() = 0;
	virtual void SetLeft(int val) = 0;
	__declspec(property(get=GetLeft, put=SetLeft)) int Left;
	virtual int GetWidth() = 0;
	virtual void SetWidth(int val) = 0;
	__declspec(property(get=GetWidth, put=SetWidth)) int Width;
	virtual int GetHeight() = 0;
	virtual void SetHeight(int val) = 0;
	__declspec(property(get=GetHeight, put=SetHeight)) int Height;
	virtual void SetMouseDownCallback(const CONTROLMOUSEEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetMouseUpCallback(const CONTROLMOUSEEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetMouseMoveCallback(const CONTROLMOUSEEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetMouseDoubleClickCallback(const CONTROLMOUSEEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetMouseWheelCallback(const CONTROLMOUSEEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetMouseClickCallback(MOUSECLICK_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetKeyDownCallback(KEYEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetKeyUpCallback(KEYEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual LPVOID _getbase() = 0;
	virtual void Release() = 0;
}* LPCONTROL;

//-----------------------------------------------------------------------------
// Name: IButton
// Desc: Interface to the Button class of ISForms
//-----------------------------------------------------------------------------
typedef class IButton : public IControl
{
public:
}* LPBUTTON;

//-----------------------------------------------------------------------------
// Name: ICheckBox
// Desc: Interface to the CheckBox class of ISForms
//-----------------------------------------------------------------------------
typedef class ICheckBox : public IControl
{
public:
	virtual bool GetChecked() = 0;
	virtual void SetChecked(bool val) = 0;
	__declspec(property(get=GetChecked, put=SetChecked)) bool Checked;
	virtual CheckedState GetCheckedState() = 0;
	virtual void SetCheckedState(CheckedState val) = 0;
	__declspec(property(get=GetCheckedState, put=SetCheckedState)) CheckedState CState;
}* LPCHECKBOX;

//-----------------------------------------------------------------------------
// Name: IRadioButton
// Desc: Interface to the RadioButton class of ISForms
//-----------------------------------------------------------------------------
typedef class IRadioButton : public IControl
{
public:
	virtual bool GetChecked() = 0;
	virtual void SetChecked(bool val) = 0;
	__declspec(property(get=GetChecked, put=SetChecked)) bool Checked;
}* LPRADIOBUTTON;

//-----------------------------------------------------------------------------
// Name: IGroupBox
// Desc: Interface to the GroupBox class of ISForms
//-----------------------------------------------------------------------------
typedef class IGroupBox : public IControl
{
public:
}* LPGROUPBOX;

//-----------------------------------------------------------------------------
// Name: ILabel
// Desc: Interface to the Label class of ISForms
//-----------------------------------------------------------------------------
typedef class ILabel : public IControl
{
public:
}* LPLABEL;

//-----------------------------------------------------------------------------
// Name: ITextBox
// Desc: Interface to the TextBox class of ISForms
//-----------------------------------------------------------------------------
typedef class ITextBox : public IControl
{
public:
	virtual void AppendText(const String& text) = 0;
}* LPTEXTBOX;

//-----------------------------------------------------------------------------
// Name: IRichEdit
// Desc: Interface to the RichEdit class of ISForms
//-----------------------------------------------------------------------------
typedef class IRichEdit : public IControl
{
public:
	virtual void SetFormat(COLORREF fgcolor = (COLORREF)-1, COLORREF bgcolor = (COLORREF)-1, const String& font = "") = 0;
	virtual void InsertText(const String& text) = 0;
}* LPRICHEDIT;

//-----------------------------------------------------------------------------
// Name: INumericUpDown
// Desc: Interface to the NumericUpDown class of ISForms
//-----------------------------------------------------------------------------
typedef class INumericUpDown : public IControl
{
public:
	virtual short SetValue(short v) = 0;
	virtual short GetValue() = 0;
	__declspec(property(get=GetValue, put=SetValue)) short Value;
	virtual short SetMinValue(short v) = 0;
	virtual short GetMinValue() = 0;
	__declspec(property(get=GetMinValue, put=SetMinValue)) short MinValue;
	virtual short SetMaxValue(short v) = 0;
	virtual short GetMaxValue() = 0;
	__declspec(property(get=GetMaxValue, put=SetMaxValue)) short MaxValue;
	virtual void SetValueChangedCallback(const _VALUECHANGED_CALLBACK cbk, LPVOID user) = 0;
}* LPNUMERICUPDOWN;

//-----------------------------------------------------------------------------
// Name: IListview
// Desc: Interface to the Listview class of ISForms
//-----------------------------------------------------------------------------
typedef class IListview : public IControl
{
public:
	virtual bool SetGroupsEnabled(bool v) = 0;
	virtual bool GetGroupsEnabled() = 0;
	__declspec(property(get=GetValue, put=SetValue)) bool GroupsEnabled;

	virtual void InsertGroup(const String& caption, int groupindex = -1) = 0;
	virtual void InsertColumn(const String& caption, int width, int columnindex = 0x7FFFFFFF) = 0;
	virtual void InsertItem(const String& text, char textdelimiter, int groupindex = -1, int rowindex = 0x7FFFFFFF) = 0;
	virtual void DeleteItem(int rowindex) = 0;
	virtual void DeleteAllItems() = 0;
}* LPLISTVIEW;

//-----------------------------------------------------------------------------
// Name: ITreeviewItem
// Desc: Interface to the ToolbarItem class of ISForms
//-----------------------------------------------------------------------------
typedef class ITreeviewItem
{
public:
	/*virtual const String& GetText() = 0;
	virtual void SetText(const String& val) = 0;
	__declspec(property(get=GetText, put=SetText)) const String& Text;*/
	/*virtual void SetClickCallback(const MENUITEMCLICK_CALLBACK clickcbk, LPVOID user) = 0;*/
	virtual void Release() = 0;
}* LPTREEVIEWITEM;

//-----------------------------------------------------------------------------
// Name: ITreeview
// Desc: Interface to the Treeview class of ISForms
//-----------------------------------------------------------------------------
typedef class ITreeview : public IControl
{
public:
	virtual Result AddItem(const String& text, const ITreeviewItem* parentitem = NULL, ITreeviewItem** item = NULL) = 0;
}* LPTREEVIEW;

//-----------------------------------------------------------------------------
// Name: ITrayIcon
// Desc: Interface to the TrayIcon class of ISForms
//-----------------------------------------------------------------------------
typedef class ITrayIcon : public IIHasContextMenu
{
public:
	enum IconType
		{IT_NONE = 0, IT_INFO = 1, IT_WARNING = 2, IT_ERROR = 3, IT_USER = 4};

	virtual Result SetToolTipText(const String& text) = 0;
	virtual Result SetNotifycationBaloon(const String& text, const String& title, const FilePath& iconfilename, IconType icontype) = 0;
	virtual void Release() = 0;
}* LPTRAYICON;

//-----------------------------------------------------------------------------
// Name: IToolbarItem
// Desc: Interface to the ToolbarItem class of ISForms
//-----------------------------------------------------------------------------
typedef class IToolbarItem
{
public:
	enum Type
		{TBI_BUTTON, TBI_CHECK, TBI_RADIO, TBI_DROPDOWN, TBI_WHOLEDROPDOWN, TBI_SEPARATOR};
	virtual const String& GetText() = 0;
	virtual void SetText(const String& val) = 0;
	__declspec(property(get=GetText, put=SetText)) const String& Text;
	virtual INT_PTR GetImage() = 0;
	virtual Result SetImage(INT_PTR imageresid, DWORD imageclrkey) = 0;
	virtual void SetWidth(WORD newwidth) = 0;
	virtual bool GetCheckedState() = 0;
	virtual void SetCheckedState(bool val) = 0;
	__declspec(property(get=GetCheckedState, put=SetCheckedState)) bool Checked;
	virtual bool GetEnabledState() = 0;
	virtual void SetEnabledState(bool val) = 0;
	__declspec(property(get=GetEnabledState, put=SetEnabledState)) bool Enabled;
	/*virtual void SetClickCallback(const MENUITEMCLICK_CALLBACK clickcbk, LPVOID user) = 0;*/
	virtual Result CreateDropDownMenu(IMenu** ctxmenu) = 0;
	virtual IMenu* GetDropDownMenu() = 0;
	virtual void ShowDropDownMenu() = 0;
	virtual void Release() = 0;
}* LPTOOLBARITEM;

//-----------------------------------------------------------------------------
// Name: IToolbar
// Desc: Interface to the Toolbar class of ISForms
//-----------------------------------------------------------------------------
typedef class IToolbar : public IIHasContextMenu
{
public:
	virtual Result AddButton(const String& text, IToolbarItem::Type btntype = IToolbarItem::TBI_BUTTON,
							 const TOOLBARITEMCLICK_CALLBACK clickcbk = NULL, LPVOID clickcbkuser = NULL, IToolbarItem** item = NULL) = 0;
	virtual Result AddButton(const String& text, INT_PTR imageresid, DWORD imageclrkey, IToolbarItem::Type btntype = IToolbarItem::TBI_BUTTON,
							 const TOOLBARITEMCLICK_CALLBACK clickcbk = NULL, LPVOID clickcbkuser = NULL, IToolbarItem** item = NULL) = 0;
	virtual Size<int> GetSize() = 0;
}* LPTOOLBAR;

//-----------------------------------------------------------------------------
// Name: ITabControl
// Desc: Interface to the TabControl class of ISForms
//-----------------------------------------------------------------------------
typedef class ITabControl : public IControl
{
public:
	virtual Result AddControl(IControl* ctrl) = 0;
	virtual Result RemoveControl(IControl* ctrl) = 0;
	virtual Result RemoveAllControls() = 0;
	virtual void InsertTab(const String& caption, int tabindex, LPVOID tag = NULL) = 0;
	virtual int GetCurrentTabIndex() = 0;
	virtual LPVOID GetCurrentTabTag() = 0;
	virtual Rect<>* GetClientBounds(Rect<>* bounds) = 0;
	virtual void ChangeTab(int tabindex) = 0;
	virtual void ChangeTab(LPVOID tabtag) = 0;
	virtual void SetTabChangedCallback(const TABCHANGED_CALLBACK cbk, LPVOID user) = 0;
}* LPTABCONTROL;

//-----------------------------------------------------------------------------
// Name: ITooltip
// Desc: Interface to the Tooltip class of ISForms
//-----------------------------------------------------------------------------
typedef class ITooltip
{
public:
	virtual HWND GetHwnd() = 0;
	virtual void Popup() = 0;
	virtual void SetTooltip(HWND wnd, const String& text) = 0;
	void SetTooltip(LPCONTROL ctrl, const String& text) {SetTooltip(ctrl->GetHwnd(), text);}
	virtual void SetTooltip(HWND wnd, GETTOOLTIPTEXT_CALLBACK cbk) = 0;
	void SetTooltip(LPCONTROL ctrl, GETTOOLTIPTEXT_CALLBACK cbk) {SetTooltip(ctrl->GetHwnd(), cbk);}
	virtual void SetTooltipShowCallback(TOOLTIPSHOW_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetTooltipHideCallback(TOOLTIPHIDE_CALLBACK cbk, LPVOID user) = 0;
}* LPTOOLTIP;

//-----------------------------------------------------------------------------
// Name: IWinForm
// Desc: Interface to the WinForm class of ISForms
//-----------------------------------------------------------------------------
typedef class IWinForm : public IIHasContextMenu
{
public:

	virtual Result AddControl(IControl* ctrl) = 0;
	virtual Result RemoveControl(IControl* ctrl) = 0;
	virtual Result RemoveAllControls() = 0;
	virtual Result CreateNumericUpDown(INumericUpDown** nud) = 0;
	virtual Result CreateTrayIcon(const FilePath& iconfilename, ITrayIcon** trayicon) = 0;
	virtual Result CreateToolbar(bool useliststyle, IToolbar** toolbar) = 0;
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual bool IsVisible() = 0;
	virtual void BringToFront() = 0;
	virtual HWND GetHwnd() = 0;
	virtual Point<int> GetLocation() = 0;
	virtual void SetLocation(Point<int> &val) = 0;
	__declspec(property(get=GetLocation, put=SetLocation)) Point<int> Location;
	virtual Size<int> GetClientSize() = 0;
	virtual void SetClientSize(Size<int> &val) = 0;
	__declspec(property(get=GetClientSize, put=SetClientSize)) Size<int> ClientSize;
	virtual int GetTop() = 0;
	virtual void SetTop(int val) = 0;
	__declspec(property(get=GetTop, put=SetTop)) int Top;
	virtual int GetLeft() = 0;
	virtual void SetLeft(int val) = 0;
	__declspec(property(get=GetLeft, put=SetLeft)) int Left;
	virtual int GetClientWidth() = 0;
	virtual void SetClientWidth(int val) = 0;
	__declspec(property(get=GetClientWidth, put=SetClientWidth)) int ClientWidth;
	virtual int GetClientHeight() = 0;
	virtual void SetClientHeight(int val) = 0;
	__declspec(property(get=GetClientHeight, put=SetClientHeight)) int ClientHeight;
	virtual Rect<int> GetBounds() = 0;
	virtual void SetBounds(const Rect<int>& val) = 0;
	__declspec(property(get=GetBounds, put=SetBounds)) Rect<int> Bounds;
	virtual Rect<int> GetClientBounds() = 0;
	virtual void SetClientBounds(const Rect<int>& val) = 0;
	__declspec(property(get=GetClientBounds, put=SetClientBounds)) Rect<int> ClientBounds;
	virtual WindowState GetWindowState() = 0;
	virtual WindowState SetWindowState(WindowState val) = 0;
	__declspec(property(get=GetWindowState, put=SetWindowState)) WindowState WndState;
	virtual Point<int> GetNormalLocation() = 0;
	virtual ::Size<int> GetNormalClientSize() = 0;
	virtual int GetNormalTop() = 0;
	virtual int GetNormalLeft() = 0;
	virtual int GetNormalClientWidth() = 0;
	virtual int GetNormalClientHeight() = 0;
	virtual FormBorderStyle GetFormBorderStyle() = 0;
	virtual FormBorderStyle SetFormBorderStyle(FormBorderStyle val) = 0;
	__declspec(property(get=GetFormBorderStyle, put=SetFormBorderStyle)) FormBorderStyle FBStyle;
	virtual bool SetMaximizeBox(bool hasmaximizebox) = 0;
	virtual bool GetMaximizeBox() = 0;
	__declspec(property(get=GetMaximizeBox, put=SetMaximizeBox)) bool HasMaximizeBox;
	virtual bool SetMinimizeBox(bool hasminimizebox) = 0;
	virtual bool GetMinimizeBox() = 0;
	__declspec(property(get=GetMinimizeBox, put=SetMinimizeBox)) bool HasMinimizeBox;
	virtual const String& GetClassName() const = 0;
	virtual Result EnableDragDrop() = 0;
	virtual Result DisableDragDrop() = 0;
	virtual Result CreateTitleMenu(IMenu** titlemenu) = 0;
	virtual LPTOOLTIP GetTooltip() = 0;
	virtual void SetMouseDownCallback(const MOUSEEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetMouseUpCallback(const MOUSEEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetMouseMoveCallback(const MOUSEEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetMouseDoubleClickCallback(const MOUSEEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetMouseWheelCallback(const MOUSEEVENT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetPaintCallback(const PAINT_CALLBACK cbk, LPVOID user) = 0;
	virtual void SetDragEnterCallback(const DRAGDROP_CALLBACK dragentercbk, LPVOID user) = 0;
	virtual void SetDragOverCallback(const DRAGDROP_CALLBACK dragovercbk, LPVOID user) = 0;
	virtual void SetDragLeaveCallback(const DRAGLEAVE_CALLBACK dragleavecbk, LPVOID user) = 0;
	virtual void SetDropCallback(const DRAGDROP_CALLBACK dropcbk, LPVOID user) = 0;
	virtual void SetFormResizeCallback(const FORMRESIZE_CALLBACK fresizecbk, LPVOID user) = 0;
	virtual void SetFormSizeChangedCallback(const FORMSIZECHANGED_CALLBACK fsizechangedcbk, LPVOID user) = 0;
	virtual void SetFormMovingCallback(const FORMMOVING_CALLBACK fmovingcbk, LPVOID user) = 0;
	virtual void SetFormMovedCallback(const FORMMOVED_CALLBACK fmovedcbk, LPVOID user) = 0;
	virtual void SetFormClosingCallback(const FORMCLOSING_CALLBACK fclosingcbk, LPVOID user) = 0;
	virtual void SetFormClosedCallback(const FORMCLOSED_CALLBACK fclosedcbk, LPVOID user) = 0;
	virtual void Close(bool* cancel = NULL) = 0;
}* LPWINFORM;

//-----------------------------------------------------------------------------
// Name: IOpenFileDialog
// Desc: Interface to the OpenFileDialog class of ISForms
//-----------------------------------------------------------------------------
typedef class IOpenFileDialog
{
public:
	struct FileType
	{
		String description, extension;
		bool operator==(const FileType& other) const
			{return this->description == other.description && this->extension == other.extension;}
	};
	FilePath* files;
	UINT numfiles;

	virtual Result Init() = 0;
	virtual Result SetFileTypes(FileType* filetypes, UINT numfiletypes, UINT defaultindex = 0) = 0;
	virtual Result SetDefaultExtension(String extension) = 0;
	virtual Result Show(HWND parent = NULL) = 0;
	virtual void Release() = 0;
}* LPOPENFILEDIALOG;

//-----------------------------------------------------------------------------
// Name: IForms
// Desc: Interface to the Forms class of ISForms
//-----------------------------------------------------------------------------
typedef class IForms
{
public:
	HFONT defaultfont;

	virtual HINSTANCE GetInstance() = 0;
	virtual LPWINFORM GetWinForm(size_t index) = 0;
	virtual HWND GetHwnd(size_t index) = 0;
	virtual HWND GetMDIClientHwnd(size_t index) = 0;
	virtual int GetNumForms() = 0;
	virtual bool VisualStylesEnabled() = 0;

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init(HINSTANCE instance) = 0;
	virtual Result CreateForm(FormSettings *settings, const String* classname = NULL, IWinForm** winform = NULL) = 0;
	virtual Result CreateForm(HWND handle, IWinForm** winform = NULL) = 0;
	virtual void CloseForm(size_t index, bool* cancel = NULL) = 0;
	virtual void CloseFormByHwnd(HWND wnd, bool* cancel = NULL) = 0;
	virtual void ShowAll() = 0;
	virtual void CloseAll(bool* cancel = NULL) = 0;
	virtual void EnableVisualStates() = 0;
	virtual void DisableVisualStates() = 0;
	virtual void CallOnGainFocus(HWND wnd) = 0;
	virtual void CallOnLoseFocus(HWND wnd) = 0;
	virtual void CallOnKeyDown(DWORD vkey) = 0;
	virtual void CallOnKeyUp(DWORD vkey) = 0;
	virtual void OnPaint(HWND wnd) = 0;
	virtual void OnMouseDown(HWND wnd, const MouseEventArgs& args) = 0;
	virtual void OnMouseUp(HWND wnd, const MouseEventArgs& args) = 0;
	virtual void OnMouseMove(HWND wnd, const MouseEventArgs& args) = 0;
	virtual void OnMouseDoubleClick(HWND wnd, const MouseEventArgs& args) = 0;
	virtual void OnMouseWheel(HWND wnd, const MouseEventArgs& args) = 0;
	virtual void OnMouseLeave(HWND wnd, const MouseEventArgs& args) = 0;
	virtual LRESULT OnFormClosing(HWND wnd) = 0;
	virtual bool OnFormResize(HWND wnd, Size<UINT> size, WindowState wndstate) = 0;
	virtual void OnFormSizing(HWND wnd, Rect<int>* bounds, byte sizeedge) = 0;
	virtual void OnFormMoving(HWND wnd, Rect<int>* bounds) = 0;
	virtual void OnFormEnterSizeMove(HWND wnd) = 0;
	virtual void OnFormExitSizeMove(HWND wnd) = 0;
	virtual void OnButtonMessage(HWND wnd, HWND handle, WORD notifycode) = 0;
	virtual void OnMenuMessage(HWND wnd, WORD menuid) = 0;
	virtual void OnTrayIconMessage(HWND wnd, LPARAM lparam) = 0;
	virtual void OnNotifyMessage(HWND wnd, const NMHDR* nmhdr) = 0;
	virtual void OnFormContextMenuMessage(HWND wnd) = 0;
	virtual void RegisterForFormDependence(IFormDependend* cls) = 0;
	virtual void DeregisterFromFormDependence(IFormDependend* cls) = 0;
	virtual void RegisterForKeyboardMonitoring(IKeyboardMonitoring* cls) = 0;
	virtual void DeregisterFromKeyboardMonitoring(IKeyboardMonitoring* cls) = 0;
	virtual void Release() = 0;
}* LPFORMS;

extern "C" FORMS_EXTERN_FUNC LPFORMS __cdecl CreateForms();
extern "C" FORMS_EXTERN_FUNC LPBUTTON __cdecl CreateButton();
extern "C" FORMS_EXTERN_FUNC LPCHECKBOX __cdecl CreateCheckBox();
extern "C" FORMS_EXTERN_FUNC LPRADIOBUTTON __cdecl CreateRadioButton();
extern "C" FORMS_EXTERN_FUNC LPGROUPBOX __cdecl CreateGroupBox();
extern "C" FORMS_EXTERN_FUNC LPLABEL __cdecl CreateLabel();
extern "C" FORMS_EXTERN_FUNC LPTEXTBOX __cdecl CreateTextBox(bool hasvscroll = false);
extern "C" FORMS_EXTERN_FUNC LPRICHEDIT __cdecl CreateRichEdit(bool hashscroll = false, bool hasvscroll = false);
extern "C" FORMS_EXTERN_FUNC LPLISTVIEW __cdecl CreateListview();
extern "C" FORMS_EXTERN_FUNC LPTREEVIEW __cdecl CreateTreeview();
extern "C" FORMS_EXTERN_FUNC LPTABCONTROL __cdecl CreateTabControl();
extern "C" FORMS_EXTERN_FUNC LPOPENFILEDIALOG __cdecl CreateOpenFileDialog();
extern "C" FORMS_EXTERN_FUNC void __cdecl RegisterScriptableFormsClasses(const IPythonScriptEngine* pse);

#endif