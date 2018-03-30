#ifndef __WININPUT_H
#define __WININPUT_H

#include "ISWinInput.h"


//-----------------------------------------------------------------------------
// Name: WinIpt
// Desc: Interface to keyboard, mouse and game controllers through WinInput
//-----------------------------------------------------------------------------
class WinIpt : public IWinIpt, public IKeyboardMonitoring
{
private:
	bool keyboardenabled;
	IForms* forms;

public:
	BYTE keys[256];

	WinIpt();
	void Sync(GLOBALVARDEF_LIST);
	Result Init(IForms* forms);
	void EnableKeyboard();
	void DisableKeyboard();
	void OnKeyDown(DWORD vkey);
	void OnKeyUp(DWORD vkey);
	void Update();
	void Release();
};

#endif