#ifdef DIRECTINPUT_SCRIPTABLE
#include "DirectInput.h"
#include "ISPythonScriptEngine.h"

//#define BOOST_PYTHON_STATIC_LIB
#include <boost\python.hpp>
#include <boost\python\suite\indexing\vector_indexing_suite.hpp>
using namespace boost::python;

struct PyDIJOYSTATE2 : public DIJOYSTATE2
{
	bool IsButtonPressed(int idx) {return rgbButtons[idx] != 0;}
};

struct PyMouseState : public _DIMOUSESTATE2
{
	bool IsMouseButtonPressed(int idx) {return idx < ARRAYSIZE(DIMOUSESTATE2::rgbButtons) ? rgbButtons[idx] != 0 : false;}
};

class PyDirectIpt : public DirectIpt
{
public:
	PyMouseState* GetMouseState() {return (PyMouseState*)&mouse;}
	void AddKeyHandlerWrapper(Key::Key key, boost::python::api::object keydowncbk, boost::python::api::object keydowncbkuser);
	std::vector<GameControllerDesc>* QueryGameControllersWrapper(bool attachedonly, bool ffonly)
	{
		Result rlt;
		std::vector<GameControllerDesc>* controllers = new std::vector<GameControllerDesc>();
		IFFAILED(QueryGameControllers(*controllers, attachedonly, ffonly))
			throw rlt.GetLastResult();
		return controllers;
	}
	int EnableGameControllerWrapper(const GUID &guid, bool isforeground, bool isexclusive)
	{
		Result rlt;
		int index;
		IFFAILED(EnableGameController(guid, isforeground, isexclusive, &index))
			throw rlt.GetLastResult();
		return index;
	}
	PyDIJOYSTATE2* CtrlsWrapper(int idx)
	{
		return idx < GetNumGameControllers() ? (PyDIJOYSTATE2*)&ctrls[idx] : NULL;
	}
};

struct PyKeyDownDelegate : public PyDirectIpt::KeyDownDelegate
{
	object cbk;
	object user;
	void RaiseCallback() {cbk(key, user);}
};

void PyDirectIpt::AddKeyHandlerWrapper(Key::Key key, object keydowncbk, object keydowncbkuser)
{
	// Skip already existing delegates
	std::list<KeyDownDelegate*>::iterator iter;
	LIST_FOREACH(lstdelegates, iter)
		if((*iter)->key == key)
			return;

	PyKeyDownDelegate* newkdd = new PyKeyDownDelegate();
	newkdd->key = key;
	newkdd->cbk = keydowncbk;
	newkdd->user = keydowncbkuser;

	lstdelegates.push_back(newkdd);
}

BOOST_PYTHON_MODULE(DirectInput)
{
	// >>> Enums

#pragma region Keys
	enum_<Key::Key>("Key")
		.value("Escape", Key::Escape)
		.value("One", Key::One)
		.value("Two", Key::Two)
		.value("Three", Key::Three)
		.value("Four", Key::Four)
		.value("Five", Key::Five)
		.value("Six", Key::Six)
		.value("Seven", Key::Seven)
		.value("Eight", Key::Eight)
		.value("Nine", Key::Nine)
		.value("Zero", Key::Zero)
		.value("Minus", Key::Minus) /* - on main keyboard */
		.value("Equals", Key::Equals)
		.value("Back", Key::Back) /* backspace */
		.value("Tab", Key::Tab)
		.value("Q", Key::Q)
		.value("W", Key::W)
		.value("E", Key::E)
		.value("R", Key::R)
		.value("T", Key::T)
		.value("Y", Key::Y)
		.value("U", Key::U)
		.value("I", Key::I)
		.value("O", Key::O)
		.value("P", Key::P)
		.value("LBracket", Key::LBracket)
		.value("RBracket", Key::RBracket)
		.value("Return", Key::Return) /* Enter on main keyboard */
		.value("LControl", Key::LControl)
		.value("A", Key::A)
		.value("S", Key::S)
		.value("D", Key::D)
		.value("F", Key::F)
		.value("G", Key::G)
		.value("H", Key::H)
		.value("J", Key::J)
		.value("K", Key::K)
		.value("L", Key::L)
		.value("Semicolon", Key::Semicolon)
		.value("Apostrophe", Key::Apostrophe)
		.value("Grave", Key::Grave) /* accent grave */
		.value("LShift", Key::LShift)
		.value("Backslash", Key::Backslash)
		.value("Z", Key::Z)
		.value("X", Key::X)
		.value("C", Key::C)
		.value("V", Key::V)
		.value("B", Key::B)
		.value("N", Key::N)
		.value("M", Key::M)
		.value("Comma", Key::Comma)
		.value("Period", Key::Period) /* . on main keyboard */
		.value("Slash", Key::Slash) /* / on main keyboard */
		.value("RShift", Key::RShift)
		.value("Multiply", Key::Multiply) /* * on numeric keypad */
		.value("LMenu", Key::LMenu) /* left Alt */
		.value("Space", Key::Space)
		.value("Capital", Key::Capital)
		.value("F1", Key::F1)
		.value("F2", Key::F2)
		.value("F3", Key::F3)
		.value("F4", Key::F4)
		.value("F5", Key::F5)
		.value("F6", Key::F6)
		.value("F7", Key::F7)
		.value("F8", Key::F8)
		.value("F9", Key::F9)
		.value("F10", Key::F10)
		.value("Numlock", Key::Numlock)
		.value("Scroll", Key::Scroll) /* Scroll Lock */
		.value("Numpad7", Key::Numpad7)
		.value("Numpad8", Key::Numpad8)
		.value("Numpad9", Key::Numpad9)
		.value("Subtract", Key::Subtract) /* - on numeric keypad */
		.value("Numpad4", Key::Numpad4)
		.value("Numpad5", Key::Numpad5)
		.value("Numpad6", Key::Numpad6)
		.value("Add", Key::Add) /* + on numeric keypad */
		.value("Numpad1", Key::Numpad1)
		.value("Numpad2", Key::Numpad2)
		.value("Numpad3", Key::Numpad3)
		.value("Numpad0", Key::Numpad0)
		.value("Decimal", Key::Decimal) /* . on numeric keypad */
		.value("OEM_102", Key::OEM_102) /* <> or \| on RT 102-key keyboard (Non-U.S.) */
		.value("F11", Key::F11)
		.value("F12", Key::F12)
		.value("F13", Key::F13) /* (NEC PC98) */
		.value("F14", Key::F14) /* (NEC PC98) */
		.value("F15", Key::F15) /* (NEC PC98) */
		.value("Kana", Key::Kana) /* (Japanese keyboard) */
		.value("ABNT_C1", Key::ABNT_C1) /* /? on Brazilian keyboard */
		.value("Convert", Key::Convert) /* (Japanese keyboard) */
		.value("NoConvert", Key::NoConvert) /* (Japanese keyboard) */
		.value("Yen", Key::Yen) /* (Japanese keyboard) */
		.value("ABNT_C2", Key::ABNT_C2) /* Numpad . on Brazilian keyboard */
		.value("NumpadEquals", Key::NumpadEquals) /* = on numeric keypad (NEC PC98) */
		.value("PrevTrack", Key::PrevTrack) /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
		.value("At", Key::At) /* (NEC PC98) */
		.value("Colon", Key::Colon) /* (NEC PC98) */
		.value("Underline", Key::Underline) /* (NEC PC98) */
		.value("Kanji", Key::Kanji) /* (Japanese keyboard) */
		.value("Stop", Key::Stop) /* (NEC PC98) */
		.value("AX", Key::AX) /* (Japan AX) */
		.value("Unlabeled", Key::Unlabeled) /* (J3100) */
		.value("NextTrack", Key::NextTrack) /* Next Track */
		.value("NumpadEnter", Key::NumpadEnter) /* Enter on numeric keypad */
		.value("RControl", Key::RControl)
		.value("Mute", Key::Mute) /* Mute */
		.value("Calculator", Key::Calculator) /* Calculator */
		.value("PlayPause", Key::PlayPause) /* Play / Pause */
		.value("MediaStop", Key::MediaStop) /* Media Stop */
		.value("VolumeDown", Key::VolumeDown) /* Volume - */
		.value("VolumeUp", Key::VolumeUp) /* Volume + */
		.value("WebHome", Key::WebHome) /* Web home */
		.value("NumpadComma", Key::NumpadComma) /* , on numeric keypad (NEC PC98) */
		.value("Divide", Key::Divide) /* / on numeric keypad */
		.value("SysRq", Key::SysRq)
		.value("RMenu", Key::RMenu) /* right Alt */
		.value("Pause", Key::Pause) /* Pause */
		.value("Home", Key::Home) /* Home on arrow keypad */
		.value("Up", Key::Up) /* UpArrow on arrow keypad */
		.value("Prior", Key::Prior) /* PgUp on arrow keypad */
		.value("Left", Key::Left) /* LeftArrow on arrow keypad */
		.value("Right", Key::Right) /* RightArrow on arrow keypad */
		.value("End", Key::End) /* End on arrow keypad */
		.value("Down", Key::Down) /* DownArrow on arrow keypad */
		.value("Next", Key::Next) /* PgDn on arrow keypad */
		.value("Insert", Key::Insert) /* Insert on arrow keypad */
		.value("Delete", Key::Delete) /* Delete on arrow keypad */
		.value("LWin", Key::LWin) /* Left Windows key */
		.value("RWin", Key::RWin) /* Right Windows key */
		.value("Apps", Key::Apps) /* AppMenu key */
		.value("Power", Key::Power) /* System Power */
		.value("Sleep", Key::Sleep) /* System Sleep */
		.value("Wake", Key::Wake) /* System Wake */
		.value("WebSearch", Key::WebSearch) /* Web Search */
		.value("WebFAvorites", Key::WebFAvorites) /* Web Favorites */
		.value("WebRefresh", Key::WebRefresh) /* Web Refresh */
		.value("WebStop", Key::WebStop) /* Web Stop */
		.value("WebForward", Key::WebForward) /* Web Forward */
		.value("WebBack", Key::WebBack) /* Web Back */
		.value("MyComputer", Key::MyComputer) /* My Computer */
		.value("Mail", Key::Mail) /* Mail */
		.value("MediaSelect", Key::MediaSelect) /* Media Select */

		//  Alternate names for keys, to facilitate transition from DOS.
		.value("Backspace", Key::Backspace) /* backspace */
		.value("NumpadStar", Key::NumpadStar) /* * on numeric keypad */
		.value("LAlt", Key::LAlt) /* left Alt */
		.value("Capslock", Key::Capslock) /* CapsLock */
		.value("NumpadMinus", Key::NumpadMinus) /* - on numeric keypad */
		.value("NumpadPlus", Key::NumpadPlus) /* + on numeric keypad */
		.value("NumpadPeriod", Key::NumpadPeriod) /* . on numeric keypad */
		.value("NumpadSlash", Key::NumpadSlash) /* / on numeric keypad */
		.value("RAlt", Key::RAlt) /* right Alt */
		.value("UpArrow", Key::UpArrow) /* UpArrow on arrow keypad */
		.value("PgUp", Key::PgUp) /* PgUp on arrow keypad */
		.value("LeftArrow", Key::LeftArrow) /* LeftArrow on arrow keypad */
		.value("RightArrow", Key::RightArrow) /* RightArrow on arrow keypad */
		.value("DownArrow", Key::DownArrow) /* DownArrow on arrow keypad */
		.value("PgDn", Key::PgDn) /* PgDn on arrow keypad */

		//  Alternate names for keys originally not used on US keyboards.
		.value("Circumflex", Key::Circumflex) /* Japanese keyboard */
	;
#pragma endregion

	// >>> Structs

	class_<_GUID>("GUID")
		.def_readwrite("Data1", &_GUID::Data1)
		.def_readwrite("Data2", &_GUID::Data2)
		.def_readwrite("Data3", &_GUID::Data3)
		
	;

	class_<GameControllerDesc>("GameControllerDesc")
		.def_readwrite("productname", &GameControllerDesc::productname)
		.def_readwrite("guid", &GameControllerDesc::guid)
	;
	class_<std::vector<GameControllerDesc>>("GameControllerDescArray")
        .def(vector_indexing_suite<std::vector<GameControllerDesc>>())
    ;

	class_<PyMouseState, boost::noncopyable>("MouseState")
		.def_readonly("x", &PyMouseState::lX)
		.def_readonly("y", &PyMouseState::lY)
		.def_readonly("z", &PyMouseState::lZ)
		.def("button", &PyMouseState::IsMouseButtonPressed)
	;

	class_<PyDIJOYSTATE2, boost::noncopyable>("JoystickState")
		.def_readonly("x", &PyDIJOYSTATE2::lX)
		.def_readonly("y", &PyDIJOYSTATE2::lY)
		.def_readonly("z", &PyDIJOYSTATE2::lZ)
		.def_readonly("rx", &PyDIJOYSTATE2::lRx)
		.def_readonly("ry", &PyDIJOYSTATE2::lRy)
		.def_readonly("rz", &PyDIJOYSTATE2::lRz)
		.def_readonly("rglSlider", &PyDIJOYSTATE2::rglSlider)
		.def_readonly("rgdwPOV", &PyDIJOYSTATE2::rgdwPOV)
		.def("button", &PyDIJOYSTATE2::IsButtonPressed)
		.def_readonly("vx", &PyDIJOYSTATE2::lVX)
		.def_readonly("vy", &PyDIJOYSTATE2::lVY)
		.def_readonly("vz", &PyDIJOYSTATE2::lVZ)
	;

	// >>> Classes

	class_<PyDirectIpt, boost::noncopyable>("DirectInput")
		.add_property("mouse", make_function(&PyDirectIpt::GetMouseState, POLICY_MANAGED_BY_C))
		.def("key", &PyDirectIpt::IsKeyPressed)
		.def("ctrls", &PyDirectIpt::CtrlsWrapper, POLICY_MANAGED_BY_C)
		.def("Init", &PyDirectIpt::Init)
		.def("QueryGameControllers", &PyDirectIpt::QueryGameControllersWrapper, POLICY_MANAGED_BY_PYTHON)
		.def("EnableMouse", &PyDirectIpt::EnableMouse)
		.def("EnableKeyboard", &PyDirectIpt::EnableKeyboard)
		.def("EnableGameController", &PyDirectIpt::EnableGameControllerWrapper)
		.def("DisableMouse", &PyDirectIpt::DisableMouse)
		.def("DisableKeyboard", &PyDirectIpt::DisableKeyboard)
		.def("IsMouseEnabled", &PyDirectIpt::DisableMouse)
		.def("IsKeyboardEnabled", &PyDirectIpt::DisableKeyboard)
		.def("Update", &PyDirectIpt::Update)
		.def("AddKeyHandler", &PyDirectIpt::AddKeyHandlerWrapper)
		.def("RemoveKeyHandler", &PyDirectIpt::RemoveKeyHandler)
	;
}

DIRECTINPUT_EXTERN_FUNC void __cdecl RegisterScriptableDirectInputClasses(const IPythonScriptEngine* pse)
{
	if(pse)
		pse->AddModule("DirectInput", &PyInit_DirectInput);
}
void SyncWithPython()
{
	AddToBuiltins("dip", (PyDirectIpt*)&*dip);
}
#else
void SyncWithPython() {}
#endif