#ifndef __DIRECTINPUT_H
#define __DIRECTINPUT_H

#include "ISDirectInput.h"
#include "..\\global\\HResult.h"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

namespace boost {namespace python {namespace api
{
	class object;
}}}


//-----------------------------------------------------------------------------
// Name: InputDevice
// Desc: Subdevice collection. Containing one actual device per window on foreground devices.
//		 Background devices contain only one actual device with a wnd of NULL.
//-----------------------------------------------------------------------------
typedef struct InputDevice
{
private:
	LPDIRECTINPUT8 lpdip;

	LPDIRECTINPUTDEVICE8* wnddevs;
	HWND* wnds;
	int numwnds, activewndidx;

	GUID guid;
	LPCDIDATAFORMAT datafmt;

public:
	bool isforeground, isexclusive;

	InputDevice();
	~InputDevice();

	Result Init(LPDIRECTINPUT8 lpdip, const GUID &guid, LPCDIDATAFORMAT datafmt, bool isforeground, bool isexclusive);
	Result ChangeCooperativeLevel(bool isforeground, bool isexclusive);
	void GetDeviceState(DWORD datasize, LPVOID data);
	void OnCreateForm(HWND wnd);
	void OnRemoveForm(HWND wnd);
	void OnGainFocus(HWND wnd);
	void OnLoseFocus(HWND wnd);
}* LPINPUTDEVICE;


//-----------------------------------------------------------------------------
// Name: DirectIpt
// Desc: Interface to keyboard, mouse and game controllers through DirectInput
//-----------------------------------------------------------------------------
class DirectIpt : public IDirectIpt
{
public:
	struct KeyDownDelegate
	{
		Key::Key key;
		bool isdown;
		virtual void RaiseCallback() = 0;
	};
	struct MouseKeyDelegate
	{
		int key;
		bool isdown;
		virtual void RaiseCallback() = 0;
	};

protected:
	LPDIRECTINPUT8 lpdip;
	LPINPUTDEVICE mousedevice, keyboarddevice, *ctrldevices;
	int numctrldevices;

	struct CKeyDownDelegate : public KeyDownDelegate
	{
		KEYDOWNCALLBACK cbk;
		LPVOID user;
		void RaiseCallback() {cbk(key, user);}
	};
	std::list<KeyDownDelegate*> lstdelegates;
	struct CMouseKeyDelegate : public MouseKeyDelegate
	{
		MOUSECALLBACK cbk;
		LPVOID user;
		void RaiseCallback() {cbk(key, user);}
	};
	std::list<MouseKeyDelegate*> lstmousedowndelegates;

	inline void ClearMouseState();
	inline void ClearKeyboardState();
	inline void ClearGameControllerState(DWORD index);

public:

	DirectIpt();
	void Sync(GLOBALVARDEF_LIST);
	Result Init();
	Result QueryGameControllers(std::vector<GameControllerDesc>& controllers, bool attachedonly, bool ffonly);
	Result EnableMouse(bool isforeground, bool isexclusive);
	Result EnableKeyboard(bool isforeground, bool isexclusive);
	Result EnableGameController(const GUID &guid, bool isforeground, bool isexclusive, int *index);
	void DisableMouse();
	void DisableKeyboard();
	void DisableGameController(int index);
	bool IsMouseEnabled() {return mousedevice != NULL;}
	bool IsKeyboardEnabled() {return keyboarddevice != NULL;}
	int GetNumGameControllers() {return numctrldevices;}
	Result ChangeMouseCooperativeLevel(bool isforeground, bool isexclusive);
	Result ChangeKeyboardCooperativeLevel(bool isforeground, bool isexclusive);
	Result ChangeGameControllerCooperativeLevel(bool isforeground, bool isexclusive);
	void GetMouseCooperativeLevel(bool& isforeground, bool& isexclusive);
	void GetKeyboardCooperativeLevel(bool& isforeground, bool& isexclusive);
	void GetGameControllerCooperativeLevel(bool& isforeground, bool& isexclusive);
	void OnCreateForm(HWND wnd);
	void OnRemoveForm(HWND wnd);
	void OnGainFocus(HWND wnd);
	void OnLoseFocus(HWND wnd);
	void Update();
	void AddKeyHandler(Key::Key key, KEYDOWNCALLBACK keydowncbk, LPVOID keydowncbkuser);
	void RemoveKeyHandler(Key::Key key);
	void AddMouseHandler(int key, MOUSECALLBACK keydowncbk, LPVOID keydowncbkuser);
	void RemoveMouseHandler(int key, MOUSECALLBACK cbk);
	void Release();
};

void SyncWithPython();

#endif