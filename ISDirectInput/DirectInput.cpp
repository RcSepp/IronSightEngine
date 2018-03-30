#include "DirectInput.h"
#include "ISForms.h"


//-----------------------------------------------------------------------------
// Name: CreateDirectInput()
// Desc: DLL API for creating an instance of DirectInput
//-----------------------------------------------------------------------------
__declspec(dllexport) LPDIRECTIPT __cdecl CreateDirectInput()
{
	return new DirectIpt();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void DirectIpt::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
	if(pse)
		SyncWithPython();
}

#pragma region InputDevice

//-----------------------------------------------------------------------------
// Name: InputDevice()
// Desc: Constructor
//-----------------------------------------------------------------------------
InputDevice::InputDevice()
{
	wnddevs = NULL;
	wnds = NULL;
	numwnds = 0;
	activewndidx = -1;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize InputDevice
//-----------------------------------------------------------------------------
Result InputDevice::Init(LPDIRECTINPUT8 lpdip, const GUID &guid, LPCDIDATAFORMAT datafmt, bool isforeground, bool isexclusive)
{
	Result rlt;

	this->lpdip = lpdip;
	this->guid = guid;
	this->datafmt = datafmt;
	this->isforeground = isforeground;
	this->isexclusive = isexclusive;

	if(!isforeground)
	{
		CHKALLOC(wnddevs = new LPDIRECTINPUTDEVICE8[1]);
		CHKALLOC(wnds = new HWND[1]);
		wnds[0] = NULL;
		numwnds = 1;

		DWORD foregroundflag = isforeground ? DISCL_FOREGROUND : DISCL_BACKGROUND;
		DWORD exclusiveflag = isexclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE;

		CHKRESULT(lpdip->CreateDevice(guid, wnddevs, NULL));
		CHKRESULT(wnddevs[0]->SetDataFormat(datafmt));
		CHKRESULT(wnddevs[0]->SetCooperativeLevel(NULL, foregroundflag | exclusiveflag)); // | DISCL_NOWINKEY
		CHKRESULT(wnddevs[0]->Acquire());
	}
	/*else
		OnCreateForm(GetForegroundWindow());*/

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: ChangeCooperativeLevel()
// Desc: Change the device's cooperative level and reaquire the device
//-----------------------------------------------------------------------------
Result InputDevice::ChangeCooperativeLevel(bool isforeground, bool isexclusive)
{
// <<< EDIT >>>
	Result rlt;

	this->isforeground = isforeground;
	this->isexclusive = isexclusive;

	for(int i = 0; i < numwnds; i++)
		CHKRESULT(wnddevs[i]->Unacquire());

	DWORD foregroundflag = isforeground ? DISCL_FOREGROUND : DISCL_BACKGROUND;
	DWORD exclusiveflag = isexclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE;

	if(isforeground)
	{
		for(int i = 0; i < numwnds; i++)
			{CHKRESULT(wnddevs[i]->SetCooperativeLevel(GetForegroundWindow(), foregroundflag | exclusiveflag | DISCL_NOWINKEY));}
	}
	else
	{
		delete[] wnddevs; CHKALLOC(wnddevs = new LPDIRECTINPUTDEVICE8[1]);
		delete[] wnds; CHKALLOC(wnds = new HWND[1]);
		wnds[0] = NULL;
		numwnds = 1;
		//CHKRESULT(wnddevs[0]->SetCooperativeLevel(NULL, foregroundflag | exclusiveflag)); // | DISCL_NOWINKEY //EDIT: Doesn't work (why?)
		{
			CHKRESULT(lpdip->CreateDevice(guid, wnddevs, NULL));
			CHKRESULT(wnddevs[0]->SetDataFormat(datafmt));
			CHKRESULT(wnddevs[0]->SetCooperativeLevel(NULL, foregroundflag | exclusiveflag)); // | DISCL_NOWINKEY
		}
		CHKRESULT(wnddevs[0]->Acquire());
	}
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: GetDeviceState()
// Desc: Query the device state frome the currently active subdevice
//-----------------------------------------------------------------------------
void InputDevice::GetDeviceState(DWORD datasize, LPVOID data)
{
	if(activewndidx < 0 || activewndidx >= numwnds)
		return;

	if(FAILED(wnddevs[activewndidx]->GetDeviceState(datasize, data)))
		wnddevs[activewndidx]->Acquire();
}

//-----------------------------------------------------------------------------
// Name: OnCreateForm()
// Desc: Create a new subdevice for the new form
//-----------------------------------------------------------------------------
void InputDevice::OnCreateForm(HWND wnd)
{
	if(!isforeground)
		return;

	// Break if the window already exists in wnds
	for(int i = 0; i < numwnds; i++)
		if(wnds[i] == wnd)
			return;

	wnddevs = (LPDIRECTINPUTDEVICE8*)_realloc(wnddevs, numwnds * sizeof(LPDIRECTINPUTDEVICE8), (numwnds + 1) * sizeof(LPDIRECTINPUTDEVICE8)); //EDIT: Log for errors
	wnds = (HWND*)_realloc(wnds, numwnds * sizeof(HWND), (numwnds + 1) * sizeof(HWND)); //EDIT: Log for errors

	DWORD foregroundflag = isforeground ? DISCL_FOREGROUND : DISCL_BACKGROUND;
	DWORD exclusiveflag = isexclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE;

	lpdip->CreateDevice(guid, &wnddevs[numwnds], NULL); //EDIT: Log for errors
	wnddevs[numwnds]->SetDataFormat(datafmt); //EDIT: Log for errors
	wnddevs[numwnds]->SetCooperativeLevel(wnds[numwnds] = wnd, foregroundflag | exclusiveflag | DISCL_NOWINKEY); //EDIT: Log for errors
	wnddevs[numwnds++]->Acquire();
}

//-----------------------------------------------------------------------------
// Name: OnRemoveForm()
// Desc: Remove form's corresponding subdevice
//-----------------------------------------------------------------------------
void InputDevice::OnRemoveForm(HWND wnd)
{
	if(!isforeground)
		return;

	// Break if the window doesn't exist in wnds
	int idx;
	for(idx = 0; idx < numwnds; idx++)
		if(wnds[idx] == wnd)
			break;
	if(idx == numwnds)
		return;

	wnddevs[idx]->Unacquire(); //EDIT: Log for errors
	RELEASE(wnddevs[idx])

	for(int i = idx + 1; i < numwnds; i++)
	{
		wnddevs[i - 1] = wnddevs[i];
		wnds[i - 1] = wnds[i];
	}

	wnddevs = (LPDIRECTINPUTDEVICE8*)_realloc(wnddevs, numwnds * sizeof(LPDIRECTINPUTDEVICE8), (numwnds - 1) * sizeof(LPDIRECTINPUTDEVICE8)); //EDIT: Log for errors
	wnds = (HWND*)_realloc(wnds, numwnds * sizeof(HWND), (numwnds - 1) * sizeof(HWND)); //EDIT: Log for errors
	numwnds--;
}

//-----------------------------------------------------------------------------
// Name: OnGainFocus()
// Desc: Activate the form's corresponding subdevice
//-----------------------------------------------------------------------------
void InputDevice::OnGainFocus(HWND wnd)
{
	if(!isforeground)
		return;

	int i;
	for(i = 0; i < numwnds; i++)
		if(wnds[i] == wnd)
		{
			activewndidx = i;
			wnddevs[i]->Acquire();
			break;
		}
	if(i == numwnds)
	{
		OnCreateForm(wnd);
		activewndidx = i;
	}
}

//-----------------------------------------------------------------------------
// Name: OnLoseFocus()
// Desc: Deactivate the form's corresponding subdevice
//-----------------------------------------------------------------------------
void InputDevice::OnLoseFocus(HWND wnd)
{
	if(!isforeground)
		return;

	int i;
	for(i = 0; i < numwnds; i++)
		if(wnds[i] == wnd)
		{
			activewndidx = -1;
			wnddevs[i]->Unacquire();
			break;
		}
}

//-----------------------------------------------------------------------------
// Name: ~InputDevice()
// Desc: Destructor
//-----------------------------------------------------------------------------
InputDevice::~InputDevice()
{
	for(int i = 0; i < numwnds; i++)
	{
		wnddevs[i]->Unacquire();
		RELEASE(wnddevs[i])
	}
	REMOVE_ARRAY(wnddevs);
	REMOVE_ARRAY(wnds);
	numwnds = 0;
}
#pragma endregion

//-----------------------------------------------------------------------------
// Name: ClearXXXState()
// Desc: Reset mouse/keyboard/game controller specific device state
//-----------------------------------------------------------------------------
inline void DirectIpt::ClearMouseState()
{
	ZeroMemory(&mouse, sizeof(DIMOUSESTATE2));
}
inline void DirectIpt::ClearKeyboardState()
{
	ZeroMemory(&keys, sizeof(BYTE) * 256);
}
inline void DirectIpt::ClearGameControllerState(DWORD index)
{
	ZeroMemory(&ctrls[index], sizeof(DIJOYSTATE2));
}

//-----------------------------------------------------------------------------
// Name: DirectIpt()
// Desc: Constructor
//-----------------------------------------------------------------------------
DirectIpt::DirectIpt()
{
	lpdip = NULL;
	mousedevice = keyboarddevice = NULL;
	ctrldevices = NULL;
	ctrls = NULL;
	numctrldevices = 0;

	ClearMouseState();
	ClearKeyboardState();
}

/*//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
//       joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext)
{
	HWND hDlg = (HWND)pContext;

	static int nSliderCount = 0; // Number of returned slider controls
	static int nPOVCount = 0; // Number of returned POV controls

	// For axes that are returned, set the DIPROP_RANGE property for the
	// enumerated axis in order to scale min/max values.
	if(pdidoi->dwType & DIDFT_AXIS)
	{
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprg.diph.dwHow = DIPH_BYID;
		diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
		diprg.lMin = -1000;
		diprg.lMax = 1000;

		// Set the range for the axis
		if(FAILED(_ctrldevice->SetProperty(DIPROP_RANGE, &diprg.diph)))
			return DIENUM_STOP;

		if(pdidoi->guidType == GUID_XAxis) cle->Print("Game controler: X Axis found");
		else if(pdidoi->guidType == GUID_YAxis) cle->Print("Game controler: Y Axis found");
		else if(pdidoi->guidType == GUID_ZAxis) cle->Print("Game controler: Z Axis found");
		else if(pdidoi->guidType == GUID_RxAxis) cle->Print("Game controler: Rx Axis found");
		else if(pdidoi->guidType == GUID_RyAxis) cle->Print("Game controler: Ry Axis found");
		else if(pdidoi->guidType == GUID_RzAxis) cle->Print("Game controler: Rz Axis found");
	}

	return DIENUM_CONTINUE;
}*/

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize DirectIpt
//-----------------------------------------------------------------------------
Result DirectIpt::Init()
{
	Result rlt;

	CHKRESULT(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpdip, NULL));

	if(fms)
		fms->RegisterForFormDependence(this);
	else
		LOG("Forms not initialized. ISDirectInput depends on Forms");

/*_lpdip = lpdip;
CHKRESULT(lpdip->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumFFDevicesCallback, NULL, DIEDFL_ATTACHEDONLY));
if(!_ctrldevice)
	return ERR("No attached game controller found");
CHKRESULT(_ctrldevice->SetDataFormat(&c_dfDIJoystick2));
CHKRESULT(_ctrldevice->SetCooperativeLevel(fms->GetHwnd(0), DISCL_EXCLUSIVE | DISCL_FOREGROUND));
CHKRESULT(_ctrldevice->EnumObjects(EnumObjectsCallback, NULL, DIDFT_ALL));
_ctrldevice->Acquire();*/

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: EnumDevicesCallback()
// Desc: Called once for each enumerated device
//-----------------------------------------------------------------------------
/*LPDIRECTINPUT8 _lpdip;
LPDIRECTINPUTDEVICE8 _ctrldevice;*/
BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* instance, VOID* context)
{
	std::vector<GameControllerDesc>* controllerlist = (std::vector<GameControllerDesc>*)context;

	GameControllerDesc& gcd = GameControllerDesc();
	gcd.productname = String(instance->tszProductName);
	gcd.guid = GUID(instance->guidInstance);
	controllerlist->push_back(gcd);

	return DIENUM_CONTINUE;
}

//-----------------------------------------------------------------------------
// Name: QueryGameControllers()
// Desc: Query an array of all registered game controllers
//-----------------------------------------------------------------------------
Result DirectIpt::QueryGameControllers(std::vector<GameControllerDesc>& controllers, bool attachedonly, bool ffonly)
{
	Result rlt;

	if(!lpdip)
		return ERR("ISDirectInput not initialized");

	DWORD flags = DIEDFL_ALLDEVICES;
	if(attachedonly) flags |= DIEDFL_ATTACHEDONLY;
	if(ffonly) flags |= DIEDFL_FORCEFEEDBACK;

	CHKRESULT(lpdip->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDevicesCallback, &controllers, flags));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: EnableXXX()
// Desc: Create and enable the mouse/keyboard/game controller
//-----------------------------------------------------------------------------
Result DirectIpt::EnableMouse(bool isforeground, bool isexclusive)
{
	Result rlt;

	CHKALLOC(mousedevice = new InputDevice());
	CHKRESULT(mousedevice->Init(lpdip, GUID_SysMouse, &c_dfDIMouse2, isforeground, isexclusive));
	ClearMouseState();

	return R_OK;
}
Result DirectIpt::EnableKeyboard(bool isforeground, bool isexclusive)
{
	Result rlt;

	CHKALLOC(keyboarddevice = new InputDevice());
	CHKRESULT(keyboarddevice->Init(lpdip, GUID_SysKeyboard, &c_dfDIKeyboard, isforeground, isexclusive));
	ClearKeyboardState();

	return R_OK;
}
Result DirectIpt::EnableGameController(const GUID &guid, bool isforeground, bool isexclusive, int *index)
{
	Result rlt;

	LPINPUTDEVICE newinputdevice;
	CHKALLOC(newinputdevice = new InputDevice());

	CHKRESULT(newinputdevice->Init(lpdip, guid, &c_dfDIJoystick2, isforeground, isexclusive));

	CHKALLOC(ctrldevices = (LPINPUTDEVICE*)_realloc(ctrldevices, numctrldevices * sizeof(LPINPUTDEVICE), (numctrldevices + 1) * sizeof(LPINPUTDEVICE)));
	CHKALLOC(ctrls = (DIJOYSTATE2*)_realloc(ctrls, numctrldevices * sizeof(DIJOYSTATE2), (numctrldevices + 1) * sizeof(DIJOYSTATE2)));
	ctrldevices[numctrldevices] = newinputdevice;
	ClearGameControllerState(numctrldevices);

	if(index)
		*index = numctrldevices;
	numctrldevices++;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: DisableXXX()
// Desc: Remove the mouse/keyboard/game controller
//-----------------------------------------------------------------------------
void DirectIpt::DisableMouse()
{
	REMOVE(mousedevice)
	ClearMouseState();
}
void DirectIpt::DisableKeyboard()
{
	REMOVE(keyboarddevice)
	ClearKeyboardState();
}
void DirectIpt::DisableGameController(int index)
{
	if(index >= numctrldevices)
		return;

	REMOVE(ctrldevices[index])
	ClearGameControllerState(index);

	for(int i = index + 1; i < numctrldevices; i++)
	{
		ctrldevices[i - 1] = ctrldevices[i];
		ctrls[i - 1] = ctrls[i];
	}

	ctrldevices = (LPINPUTDEVICE*)_realloc(ctrldevices, numctrldevices * sizeof(LPINPUTDEVICE), (numctrldevices - 1) * sizeof(LPINPUTDEVICE)); //EDIT: Log for errors
	ctrls = (DIJOYSTATE2*)_realloc(ctrls, numctrldevices * sizeof(DIJOYSTATE2), (numctrldevices - 1) * sizeof(DIJOYSTATE2)); //EDIT: Log for errors
	numctrldevices--;
}

//-----------------------------------------------------------------------------
// Name: ChangeXXXCooperativeLevel()
// Desc: Change cooperative level of the mouse/keyboard/game controller device
//-----------------------------------------------------------------------------
Result DirectIpt::ChangeMouseCooperativeLevel(bool isforeground, bool isexclusive)
{
	return mousedevice ? mousedevice->ChangeCooperativeLevel(isforeground, isexclusive) : R_OK;
}
Result DirectIpt::ChangeKeyboardCooperativeLevel(bool isforeground, bool isexclusive)
{
	return keyboarddevice ? keyboarddevice->ChangeCooperativeLevel(isforeground, isexclusive) : R_OK;
}
Result DirectIpt::ChangeGameControllerCooperativeLevel(bool isforeground, bool isexclusive)
{
	Result rlt;
	for(int i = 0; i < numctrldevices; i++)
		{CHKRESULT(ctrldevices[i]->ChangeCooperativeLevel(isforeground, isexclusive));}
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: GetXXXCooperativeLevel()
// Desc: Get cooperative level of the mouse/keyboard/game controller device
//-----------------------------------------------------------------------------
void DirectIpt::GetMouseCooperativeLevel(bool& isforeground, bool& isexclusive)
{
	if(mousedevice)
	{
		isforeground = mousedevice->isforeground;
		isexclusive = mousedevice->isexclusive;
	}
}
void DirectIpt::GetKeyboardCooperativeLevel(bool& isforeground, bool& isexclusive)
{
	if(keyboarddevice)
	{
		isforeground = keyboarddevice->isforeground;
		isexclusive = keyboarddevice->isexclusive;
	}
}
void DirectIpt::GetGameControllerCooperativeLevel(bool& isforeground, bool& isexclusive)
{
	if(numctrldevices)
	{
		isforeground = ctrldevices[0]->isforeground;
		isexclusive = ctrldevices[0]->isexclusive;
	}
}

//-----------------------------------------------------------------------------
// Name: OnCreateForm()
// Desc: Event handler for form creation
//-----------------------------------------------------------------------------
void DirectIpt::OnCreateForm(HWND wnd)
{
	if(mousedevice)
		mousedevice->OnCreateForm(wnd);
	if(keyboarddevice)
		keyboarddevice->OnCreateForm(wnd);
	for(int i = 0; i < numctrldevices; i++)
		ctrldevices[i]->OnCreateForm(wnd);
}

//-----------------------------------------------------------------------------
// Name: OnRemoveForm()
// Desc: Event handler for form destruction
//-----------------------------------------------------------------------------
void DirectIpt::OnRemoveForm(HWND wnd)
{
	if(mousedevice)
		mousedevice->OnRemoveForm(wnd);
	if(keyboarddevice)
		keyboarddevice->OnRemoveForm(wnd);
	for(int i = 0; i < numctrldevices; i++)
		ctrldevices[i]->OnRemoveForm(wnd);
}

//-----------------------------------------------------------------------------
// Name: OnGainFocus()
// Desc: Event handler for form focus gaining
//-----------------------------------------------------------------------------
void DirectIpt::OnGainFocus(HWND wnd)
{
	if(mousedevice)
		mousedevice->OnGainFocus(wnd);
	if(keyboarddevice)
		keyboarddevice->OnGainFocus(wnd);
	for(int i = 0; i < numctrldevices; i++)
		ctrldevices[i]->OnGainFocus(wnd);
}

//-----------------------------------------------------------------------------
// Name: OnLoseFocus()
// Desc: Event handler for form focus losing
//-----------------------------------------------------------------------------
void DirectIpt::OnLoseFocus(HWND wnd)
{
	if(mousedevice)
		mousedevice->OnLoseFocus(wnd);
	if(keyboarddevice)
		keyboarddevice->OnLoseFocus(wnd);
	for(int i = 0; i < numctrldevices; i++)
		ctrldevices[i]->OnLoseFocus(wnd);
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Query data for mouse, keyboard and game controlers. Raise events
//-----------------------------------------------------------------------------
void DirectIpt::Update()
{
	// Get mouse state
	if(mousedevice)
	{
		ClearMouseState();
		mousedevice->GetDeviceState(sizeof(mouse), &mouse);
	}

	// Get keyboard state
	if(keyboarddevice)
	{
		ClearKeyboardState();
		keyboarddevice->GetDeviceState(sizeof(keys), &keys);
	}

	// Raise keyboard events
	std::list<KeyDownDelegate*>::iterator kiter;
	LIST_FOREACH(lstdelegates, kiter)
	{
		if(!(*kiter)->isdown && keys[(*kiter)->key] & 0x80)
		{
			(*kiter)->isdown = true;
			(*kiter)->RaiseCallback();
		}
		if((*kiter)->isdown && !(keys[(*kiter)->key] & 0x80))
			(*kiter)->isdown = false;
	}

	// Raise mouse events
	std::list<MouseKeyDelegate*>::iterator miter;
	LIST_FOREACH(lstmousedowndelegates, miter)
	{
		if(!(*miter)->isdown && mouse.rgbButtons[(*miter)->key])
		{
			(*miter)->isdown = true;
			(*miter)->RaiseCallback();
		}
		if((*miter)->isdown && !(mouse.rgbButtons[(*miter)->key]))
			(*miter)->isdown = false;
	}

	// Get game controller state
	for(int i = 0; i < numctrldevices; i++)
	{
		ClearGameControllerState(i);
		ctrldevices[i]->GetDeviceState(sizeof(DIJOYSTATE2), &ctrls[i]);
	}
}

//-----------------------------------------------------------------------------
// Name: AddKeyHandler()
// Desc: Register key down event handler
//-----------------------------------------------------------------------------
void DirectIpt::AddKeyHandler(Key::Key key, KEYDOWNCALLBACK keydowncbk, LPVOID keydowncbkuser)
{
	// Skip already existing delegates
	std::list<KeyDownDelegate*>::iterator iter;
	LIST_FOREACH(lstdelegates, iter)
		if((*iter)->key == key)
			return;

	CKeyDownDelegate* newkdd = new CKeyDownDelegate();
	newkdd->key = key;
	newkdd->cbk = keydowncbk;
	newkdd->user = keydowncbkuser;

	lstdelegates.push_back(newkdd);
}

//-----------------------------------------------------------------------------
// Name: RemoveKeyHandler()
// Desc: Deregister key down event handler
//-----------------------------------------------------------------------------
void DirectIpt::RemoveKeyHandler(Key::Key key)
{
	std::list<KeyDownDelegate*>::iterator iter;
	LIST_FOREACH(lstdelegates, iter)
		if((*iter)->key == key)
		{
			lstdelegates.erase(iter);
			break;
		}
}

//-----------------------------------------------------------------------------
// Name: AddMouseHandler()
// Desc: Register mouse key down event handler
//-----------------------------------------------------------------------------
void DirectIpt::AddMouseHandler(int key, MOUSECALLBACK keydowncbk, LPVOID keydowncbkuser)
{
	/*// Skip already existing delegates
	std::list<MouseKeyDelegate*>::iterator iter;
	LIST_FOREACH(lstmousedowndelegates, iter)
		if((*iter)->key == key)
			return;*/

	CMouseKeyDelegate* newkdd = new CMouseKeyDelegate();
	newkdd->key = key;
	newkdd->cbk = keydowncbk;
	newkdd->user = keydowncbkuser;

	lstmousedowndelegates.push_back(newkdd);
}

//-----------------------------------------------------------------------------
// Name: RemoveMouseHandler()
// Desc: Deregister mouse key down event handler
//-----------------------------------------------------------------------------
void DirectIpt::RemoveMouseHandler(int key, MOUSECALLBACK cbk)
{
	std::list<MouseKeyDelegate*>::iterator iter;
	LIST_FOREACH(lstmousedowndelegates, iter)
		if((*iter)->key == key && ((CMouseKeyDelegate*)*iter)->cbk == cbk)
		{
			lstmousedowndelegates.erase(iter);
			break;
		}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void DirectIpt::Release()
{
	if(fms)
		fms->DeregisterFromFormDependence(this);

	std::list<KeyDownDelegate*>::iterator iter;
	LIST_FOREACH(lstdelegates, iter)
		delete *iter;

	RELEASE(lpdip)

	REMOVE(mousedevice)
	REMOVE(keyboarddevice)
	REMOVE_ARRAY(ctrldevices)
	numctrldevices = 0;

	delete this;
}