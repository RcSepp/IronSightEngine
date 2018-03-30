#ifndef __ISDIRECTINPUT_H
#define __ISDIRECTINPUT_H

#define DIRECTINPUT_VERSION 0x0800

#include <ISEngine.h>

#include <dinput.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_DIRECTINPUT
	#define DIRECTINPUT_EXTERN_FUNC		__declspec(dllexport)
#else
	#define DIRECTINPUT_EXTERN_FUNC		__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
typedef void (__stdcall* KEYDOWNCALLBACK)(int key, LPVOID user);
typedef void (__stdcall* KEYUPCALLBACK)(int key, LPVOID user);
typedef void (__stdcall* MOUSECALLBACK)(int key, LPVOID user);


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
#pragma region Keys
namespace Key
{
	enum Key
	{
		Escape = DIK_ESCAPE,
		One = DIK_1,
		Two = DIK_2,
		Three = DIK_3,
		Four = DIK_4,
		Five = DIK_5,
		Six = DIK_6,
		Seven = DIK_7,
		Eight = DIK_8,
		Nine = DIK_9,
		Zero = DIK_0,
		Minus = DIK_MINUS, /* - on main keyboard */
		Equals = DIK_EQUALS,
		Back = DIK_BACK, /* backspace */
		Tab = DIK_TAB,
		Q = DIK_Q,
		W = DIK_W,
		E = DIK_E,
		R = DIK_R,
		T = DIK_T,
		Y = DIK_Y,
		U = DIK_U,
		I = DIK_I,
		O = DIK_O,
		P = DIK_P,
		LBracket = DIK_LBRACKET,
		RBracket = DIK_RBRACKET,
		Return = DIK_RETURN, /* Enter on main keyboard */
		LControl = DIK_LCONTROL,
		A = DIK_A,
		S = DIK_S,
		D = DIK_D,
		F = DIK_F,
		G = DIK_G,
		H = DIK_H,
		J = DIK_J,
		K = DIK_K,
		L = DIK_L,
		Semicolon = DIK_SEMICOLON,
		Apostrophe = DIK_APOSTROPHE,
		Grave = DIK_GRAVE, /* accent grave */
		LShift = DIK_LSHIFT,
		Backslash = DIK_BACKSLASH,
		Z = DIK_Z,
		X = DIK_X,
		C = DIK_C,
		V = DIK_V,
		B = DIK_B,
		N = DIK_N,
		M = DIK_M,
		Comma = DIK_COMMA,
		Period = DIK_PERIOD, /* . on main keyboard */
		Slash = DIK_SLASH, /* / on main keyboard */
		RShift = DIK_RSHIFT,
		Multiply = DIK_MULTIPLY, /* * on numeric keypad */
		LMenu = DIK_LMENU, /* left Alt */
		Space = DIK_SPACE,
		Capital = DIK_CAPITAL,
		F1 = DIK_F1,
		F2 = DIK_F2,
		F3 = DIK_F3,
		F4 = DIK_F4,
		F5 = DIK_F5,
		F6 = DIK_F6,
		F7 = DIK_F7,
		F8 = DIK_F8,
		F9 = DIK_F9,
		F10 = DIK_F10,
		Numlock = DIK_NUMLOCK,
		Scroll = DIK_SCROLL, /* Scroll Lock */
		Numpad7 = DIK_NUMPAD7,
		Numpad8 = DIK_NUMPAD8,
		Numpad9 = DIK_NUMPAD9,
		Subtract = DIK_SUBTRACT, /* - on numeric keypad */
		Numpad4 = DIK_NUMPAD4,
		Numpad5 = DIK_NUMPAD5,
		Numpad6 = DIK_NUMPAD6,
		Add = DIK_ADD, /* + on numeric keypad */
		Numpad1 = DIK_NUMPAD1,
		Numpad2 = DIK_NUMPAD2,
		Numpad3 = DIK_NUMPAD3,
		Numpad0 = DIK_NUMPAD0,
		Decimal = DIK_DECIMAL, /* . on numeric keypad */
		OEM_102 = DIK_OEM_102, /* <> or \| on RT 102-key keyboard (Non-U.S.) */
		F11 = DIK_F11,
		F12 = DIK_F12,
		F13 = DIK_F13, /* (NEC PC98) */
		F14 = DIK_F14, /* (NEC PC98) */
		F15 = DIK_F15, /* (NEC PC98) */
		Kana = DIK_KANA, /* (Japanese keyboard) */
		ABNT_C1 = DIK_ABNT_C1, /* /? on Brazilian keyboard */
		Convert = DIK_CONVERT, /* (Japanese keyboard) */
		NoConvert = DIK_NOCONVERT, /* (Japanese keyboard) */
		Yen = DIK_YEN, /* (Japanese keyboard) */
		ABNT_C2 = DIK_ABNT_C2, /* Numpad . on Brazilian keyboard */
		NumpadEquals = DIK_NUMPADEQUALS, /* = on numeric keypad (NEC PC98) */
		PrevTrack = DIK_PREVTRACK, /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
		At = DIK_AT, /* (NEC PC98) */
		Colon = DIK_COLON, /* (NEC PC98) */
		Underline = DIK_UNDERLINE, /* (NEC PC98) */
		Kanji = DIK_KANJI, /* (Japanese keyboard) */
		Stop = DIK_STOP, /* (NEC PC98) */
		AX = DIK_AX, /* (Japan AX) */
		Unlabeled = DIK_UNLABELED, /* (J3100) */
		NextTrack = DIK_NEXTTRACK, /* Next Track */
		NumpadEnter = DIK_NUMPADENTER, /* Enter on numeric keypad */
		RControl = DIK_RCONTROL,
		Mute = DIK_MUTE, /* Mute */
		Calculator = DIK_CALCULATOR, /* Calculator */
		PlayPause = DIK_PLAYPAUSE, /* Play / Pause */
		MediaStop = DIK_MEDIASTOP, /* Media Stop */
		VolumeDown = DIK_VOLUMEDOWN, /* Volume - */
		VolumeUp = DIK_VOLUMEUP, /* Volume + */
		WebHome = DIK_WEBHOME, /* Web home */
		NumpadComma = DIK_NUMPADCOMMA, /* , on numeric keypad (NEC PC98) */
		Divide = DIK_DIVIDE, /* / on numeric keypad */
		SysRq = DIK_SYSRQ,
		RMenu = DIK_RMENU, /* right Alt */
		Pause = DIK_PAUSE, /* Pause */
		Home = DIK_HOME, /* Home on arrow keypad */
		Up = DIK_UP, /* UpArrow on arrow keypad */
		Prior = DIK_PRIOR, /* PgUp on arrow keypad */
		Left = DIK_LEFT, /* LeftArrow on arrow keypad */
		Right = DIK_RIGHT, /* RightArrow on arrow keypad */
		End = DIK_END, /* End on arrow keypad */
		Down = DIK_DOWN, /* DownArrow on arrow keypad */
		Next = DIK_NEXT, /* PgDn on arrow keypad */
		Insert = DIK_INSERT, /* Insert on arrow keypad */
		Delete = DIK_DELETE, /* Delete on arrow keypad */
		LWin = DIK_LWIN, /* Left Windows key */
		RWin = DIK_RWIN, /* Right Windows key */
		Apps = DIK_APPS, /* AppMenu key */
		Power = DIK_POWER, /* System Power */
		Sleep = DIK_SLEEP, /* System Sleep */
		Wake = DIK_WAKE, /* System Wake */
		WebSearch = DIK_WEBSEARCH, /* Web Search */
		WebFAvorites = DIK_WEBFAVORITES, /* Web Favorites */
		WebRefresh = DIK_WEBREFRESH, /* Web Refresh */
		WebStop = DIK_WEBSTOP, /* Web Stop */
		WebForward = DIK_WEBFORWARD, /* Web Forward */
		WebBack = DIK_WEBBACK, /* Web Back */
		MyComputer = DIK_MYCOMPUTER, /* My Computer */
		Mail = DIK_MAIL, /* Mail */
		MediaSelect = DIK_MEDIASELECT, /* Media Select */

		//  Alternate names for keys, to facilitate transition from DOS.
		Backspace = DIK_BACKSPACE, /* backspace */
		NumpadStar = DIK_NUMPADSTAR, /* * on numeric keypad */
		LAlt = DIK_LALT, /* left Alt */
		Capslock = DIK_CAPSLOCK, /* CapsLock */
		NumpadMinus = DIK_NUMPADMINUS, /* - on numeric keypad */
		NumpadPlus = DIK_NUMPADPLUS, /* + on numeric keypad */
		NumpadPeriod = DIK_NUMPADPERIOD, /* . on numeric keypad */
		NumpadSlash = DIK_NUMPADSLASH, /* / on numeric keypad */
		RAlt = DIK_RALT, /* right Alt */
		UpArrow = DIK_UPARROW, /* UpArrow on arrow keypad */
		PgUp = DIK_PGUP, /* PgUp on arrow keypad */
		LeftArrow = DIK_LEFTARROW, /* LeftArrow on arrow keypad */
		RightArrow = DIK_RIGHTARROW, /* RightArrow on arrow keypad */
		DownArrow = DIK_DOWNARROW, /* DownArrow on arrow keypad */
		PgDn = DIK_PGDN, /* PgDn on arrow keypad */

		//  Alternate names for keys originally not used on US keyboards.
		Circumflex = DIK_CIRCUMFLEX /* Japanese keyboard */
	};
}
#pragma endregion


//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct GameControllerDesc
{
	String productname;
	GUID guid;
	bool operator==(const GameControllerDesc& other) const
		{return IsEqualGUID(this->guid, other.guid) == TRUE;}
};


//-----------------------------------------------------------------------------
// Name: IDirectIpt
// Desc: Interface to the DirectInput class
//-----------------------------------------------------------------------------
typedef class IDirectIpt : public IFormDependend
{
public:
	DIMOUSESTATE2 mouse;
	BYTE keys[256];
	DIJOYSTATE2* ctrls;

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init() = 0;
	virtual Result QueryGameControllers(std::vector<GameControllerDesc>& controllers, bool attachedonly = true, bool ffonly = false) = 0;
	virtual Result EnableMouse(bool isforeground, bool isexclusive) = 0;
	virtual Result EnableKeyboard(bool isforeground, bool isexclusive) = 0;
	virtual Result EnableGameController(const GUID &guid, bool isforeground, bool isexclusive, int *index = NULL) = 0;
	virtual void DisableMouse() = 0;
	virtual void DisableKeyboard() = 0;
	virtual void DisableGameController(int index) = 0;
	virtual bool IsMouseEnabled() = 0;
	virtual bool IsKeyboardEnabled() = 0;
	virtual int GetNumGameControllers() = 0;
	virtual Result ChangeMouseCooperativeLevel(bool isforeground, bool isexclusive) = 0;
	virtual Result ChangeKeyboardCooperativeLevel(bool isforeground, bool isexclusive) = 0;
	virtual Result ChangeGameControllerCooperativeLevel(bool isforeground, bool isexclusive) = 0;
	virtual void GetMouseCooperativeLevel(bool& isforeground, bool& isexclusive) = 0;
	virtual void GetKeyboardCooperativeLevel(bool& isforeground, bool& isexclusive) = 0;
	virtual void GetGameControllerCooperativeLevel(bool& isforeground, bool& isexclusive) = 0;
	virtual void Update() = 0;
	virtual void AddKeyHandler(Key::Key key, KEYDOWNCALLBACK keydowncbk, LPVOID keydowncbkuser) = 0;
	virtual void RemoveKeyHandler(Key::Key key) = 0;
	virtual void AddMouseHandler(int key, MOUSECALLBACK keydowncbk, LPVOID keydowncbkuser) = 0;
	virtual void RemoveMouseHandler(int key, MOUSECALLBACK cbk) = 0;
	virtual void Release() = 0;

	bool IsKeyPressed(Key::Key key) {return keys[key] != 0;}
}* LPDIRECTIPT;

extern "C" DIRECTINPUT_EXTERN_FUNC LPDIRECTIPT __cdecl CreateDirectInput();
extern "C" DIRECTINPUT_EXTERN_FUNC void __cdecl RegisterScriptableDirectInputClasses(const IPythonScriptEngine* pse);

#endif