#include "Forms.h"


//-----------------------------------------------------------------------------
// Name: TrayIcon()
// Desc: Constructor
//-----------------------------------------------------------------------------
TrayIcon::TrayIcon()
{
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize the notification icon
//-----------------------------------------------------------------------------
Result TrayIcon::Init(const FilePath& iconfilename, WinForm* parent)
{
	Result rlt = R_OK;

	this->parentform = parent;

	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(nid);
	nid.uID = this->id = parent->trayicons.size();
	nid.uFlags = NIF_ICON | NIF_MESSAGE;
	nid.hWnd = parent->GetHwnd();

	nid.hIcon = iconfilename.IsEmpty() ? NULL : (HICON)LoadImage(NULL, iconfilename, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE);
	nid.uCallbackMessage = WM_TRAYICONMSG;

	if(!Shell_NotifyIcon(NIM_ADD, &nid))
		rlt = ERR("Error creating notification icon\nFunction: Shell_NotifyIcon()");

	if(nid.hIcon)
		DestroyIcon(nid.hIcon);

	return rlt;
}

//-----------------------------------------------------------------------------
// Name: SetToolTipText()
// Desc: Set the text, that will be displayed when the user hovers the mouse over the icon
//-----------------------------------------------------------------------------
Result TrayIcon::SetToolTipText(const String& text)
{
	if(text.length() > 127)
		return ERR("Tool tip text may not be longer than 127 characters");

	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(nid);
	nid.uID = this->id;
	nid.uFlags = NIF_TIP;
	nid.hWnd = parentform->GetHwnd();

	strcpy_s(nid.szTip, text);

	if(!Shell_NotifyIcon(NIM_MODIFY, &nid))
		return ERR("Error setting notify icon tool tip text icon\nFunction: Shell_NotifyIcon()");

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetNotifycationBaloon()
// Desc: Set the notification (a special type of balloon tooltip control)
//-----------------------------------------------------------------------------
Result TrayIcon::SetNotifycationBaloon(const String& text, const String& title, const FilePath& iconfilename, IconType icontype)
{
	if(text.length() > 255)
		return ERR("Tool notify text may not be longer than 255 characters");
	if(title.length() > 63)
		return ERR("Tool notify title may not be longer than 63 characters");

	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(nid);
	nid.uID = this->id;
	nid.uFlags = NIF_INFO;
	nid.hWnd = parentform->GetHwnd();

	strcpy_s(nid.szInfo, text);
	strcpy_s(nid.szInfoTitle, title);

	if(icontype == IT_USER && iconfilename.IsEmpty())
		icontype = IT_NONE;
	nid.dwInfoFlags = NIIF_LARGE_ICON | (DWORD)icontype;
	nid.hIcon = icontype != IT_USER ? NULL : (HICON)LoadImage(NULL, iconfilename, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE);

	if(!Shell_NotifyIcon(NIM_MODIFY, &nid))
		return ERR("Error setting notify icon tool tip text icon\nFunction: Shell_NotifyIcon()");

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void TrayIcon::Release()
{
	parentform->trayicons.remove(this);

	// Remove icon
	NOTIFYICONDATA nid = {};
	nid.uID = this->id;
	nid.hWnd = parentform->GetHwnd();
	Shell_NotifyIcon(NIM_DELETE, &nid);

//	// Remove context menu //EDIT: When ctxmenu gets released by its parentform (WinForm) first, than this pointer points to invalid memory!
//	RELEASE(ctxmenu);

	delete this;
}