#include "WinInput.h"
#include "ISForms.h"


//-----------------------------------------------------------------------------
// Name: CreateWinInput()
// Desc: DLL API for creating an instance of WinInput
//-----------------------------------------------------------------------------
__declspec(dllexport) LPWINIPT __cdecl CreateWinInput()
{
	return new WinIpt();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void WinIpt::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: WinIpt()
// Desc: Constructor
//-----------------------------------------------------------------------------
WinIpt::WinIpt()
{
	forms = NULL;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize WinIpt
//-----------------------------------------------------------------------------
Result WinIpt::Init(IForms* forms)
{
	Result rlt;

	if(!forms)
		return ERR("Parameter is NULL: Forms");

	this->forms = forms;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: EnableKeyboard()
// Desc: Enable keyboard monitoring
//-----------------------------------------------------------------------------
void WinIpt::EnableKeyboard()
{
	forms->RegisterForKeyboardMonitoring(this);
	keyboardenabled = true;
}

//-----------------------------------------------------------------------------
// Name: DisableKeyboard()
// Desc: Disable keyboard monitoring
//-----------------------------------------------------------------------------
void WinIpt::DisableKeyboard()
{
	forms->DeregisterFromKeyboardMonitoring(this);
	keyboardenabled = false;
}

//-----------------------------------------------------------------------------
// Name: OnKeyDown()
// Desc: Event handler for key down
//-----------------------------------------------------------------------------
void WinIpt::OnKeyDown(DWORD vkey)
{
	if(vkey > 0xFF)
		return; //EDIT: log error

	keys[vkey] = 0x80;
if(vkey == VK_ESCAPE)
	PostQuitMessage(0);
}

//-----------------------------------------------------------------------------
// Name: OnKeyUp()
// Desc: Event handler for key up
//-----------------------------------------------------------------------------
void WinIpt::OnKeyUp(DWORD vkey)
{
	if(vkey > 0xFF)
		return; //EDIT: log error

	keys[vkey] = 0x80;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Raise events
//-----------------------------------------------------------------------------
void WinIpt::Update()
{
/*// Raise keyboard events
std::list<KeyDownDelegate>::iterator i;
for (i = lstdelegates.begin(); i != lstdelegates.end(); i++)
{
	if(!(*i).isdown && keys[(*i).key] & 0x80)
	{
		(*i).isdown = true;
		(*i).func(app);
	}
	if((*i).isdown && !(keys[(*i).key] & 0x80))
		(*i).isdown = false;
}*/
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void WinIpt::Release()
{
	if(forms && keyboardenabled)
		forms->DeregisterFromKeyboardMonitoring(this);

	delete this;
}