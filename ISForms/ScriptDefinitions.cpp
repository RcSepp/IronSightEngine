#ifdef FORMS_SCRIPTABLE
#include "Forms.h"
#include "ISPythonScriptEngine.h"

//#define BOOST_PYTHON_STATIC_LIB
#include <boost\python.hpp>
#include <boost\python\suite\indexing\vector_indexing_suite.hpp>
using namespace boost::python;

struct PyCallbackArgs
{
	object cbk, user;
	PyCallbackArgs(object& cbk, object& user) : cbk(cbk), user(user) {}
};

#define BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(cls) \
	void cls::SetMouseDownCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {basecontrol.pymousedowncbk = new PyCallbackArgs(cbk, user);} \
	void cls::SetMouseUpCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {basecontrol.pymouseupcbk = new PyCallbackArgs(cbk, user);} \
	void cls::SetMouseMoveCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {basecontrol.pymousemovecbk = new PyCallbackArgs(cbk, user);} \
	void cls::SetMouseDoubleClickCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {basecontrol.pymousedclickcbk = new PyCallbackArgs(cbk, user);} \
	void cls::SetMouseWheelCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {basecontrol.pymousewheelcbk = new PyCallbackArgs(cbk, user);} \
	void cls::SetMouseClickCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {basecontrol.pymclickcbk = new PyCallbackArgs(cbk, user);} \
	void cls::SetKeyDownCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {basecontrol.pykeydowncbk = new PyCallbackArgs(cbk, user);} \
	void cls::SetKeyUpCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {basecontrol.pykeyupcbk = new PyCallbackArgs(cbk, user);}
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(Button)
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(CheckBox)
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(RadioButton)
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(GroupBox)
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(Label)
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(TextBox)
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(RichEdit)
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(NumericUpDown)
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(Listview)
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(Treeview)
BASECONTROL_ADAPTER_FUNCTION_WRAPPERS(TabControl)

void WinForm::SetMouseDownCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pymousedowncbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetMouseUpCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pymouseupcbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetMouseMoveCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pymousemovecbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetMouseDoubleClickCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pymousedclickcbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetMouseWheelCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pymousewheelcbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetPaintCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pypaintcbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetDragEnterCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pydragentercbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetDragOverCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pydragovercbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetDragLeaveCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pydragleavecbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetDropCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pydropcbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetFormResizeCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pyfresizecbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetFormSizeChangedCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pyfsizechangedcbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetFormMovingCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pyfmovingcbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetFormMovedCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pyfmovedcbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetFormClosingCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pyfclosingcbk = new PyCallbackArgs(cbk, user);}
void WinForm::SetFormClosedCallbackWrapper(boost::python::api::object cbk, boost::python::api::object user) {pyfclosedcbk = new PyCallbackArgs(cbk, user);}

void Control::RaiseMouseDownCbk(const MouseEventArgs& args)
{
	if(mousedowncbk)
		mousedowncbk(this->ifc, args, mousedowncbkuser);
	if(pymousedowncbk)
	{
		pse->BeginPythonCode();
		try
			{pymousedowncbk->cbk(boost::ref(this->ifc), args, pymousedowncbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void Control::RaiseMouseUpCbk(const MouseEventArgs& args)
{
	if(mouseupcbk)
		mouseupcbk(this->ifc, args, mouseupcbkuser);
	if(pymouseupcbk)
	{
		pse->BeginPythonCode();
		try
			{pymouseupcbk->cbk(boost::ref(this->ifc), args, pymouseupcbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void Control::RaiseMouseMoveCbk(const MouseEventArgs& args)
{
	if(mousemovecbk)
		mousemovecbk(this->ifc, args, mousemovecbkuser);
	if(pymousemovecbk)
	{
		pse->BeginPythonCode();
		try
			{pymousemovecbk->cbk(boost::ref(this->ifc), args, pymousemovecbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void Control::RaiseMouseDClickCbk(const MouseEventArgs& args)
{
	if(mousedclickcbk)
		mousedclickcbk(this->ifc, args, mousedclickcbkuser);
	if(pymousedclickcbk)
	{
		pse->BeginPythonCode();
		try
			{pymousedclickcbk->cbk(boost::ref(this->ifc), args, pymousedclickcbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void Control::RaiseMouseWheelCbk(const MouseEventArgs& args)
{
	if(mousewheelcbk)
		mousewheelcbk(this->ifc, args, mousewheelcbkuser);
	if(pymousewheelcbk)
	{
		pse->BeginPythonCode();
		try
			{pymousewheelcbk->cbk(boost::ref(this->ifc), args, pymousewheelcbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void Control::RaiseMouseClickCbk()
{
	if(mclickcbk)
		mclickcbk(this->ifc, mclickcbkuser);
	if(pymclickcbk)
	{
		pse->BeginPythonCode();
		try
			{pymclickcbk->cbk(boost::ref(this->ifc), pymclickcbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void Control::RaiseKeyDownCbk(WORD vkey)
{
	if(keydowncbk)
		keydowncbk(this->ifc, vkey, keydowncbkuser);
	if(pykeydowncbk)
	{
		pse->BeginPythonCode();
		try
			{pykeydowncbk->cbk(boost::ref(this->ifc), vkey, pykeydowncbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void Control::RaiseKeyUpCbk(WORD vkey)
{
	if(keyupcbk)
		keyupcbk(this->ifc, vkey, keyupcbkuser);
	if(pykeyupcbk)
	{
		pse->BeginPythonCode();
		try
			{pykeyupcbk->cbk(boost::ref(this->ifc), vkey, pykeyupcbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}

void Control::RemovePyCallbacks()
{
	REMOVE(pymousedowncbk);
	REMOVE(pymouseupcbk);
	REMOVE(pymousemovecbk);
	REMOVE(pymousedclickcbk);
	REMOVE(pymousewheelcbk);
	REMOVE(pymclickcbk);
	REMOVE(pykeydowncbk);
	REMOVE(pykeyupcbk);
}

void WinForm::RaiseMouseDownCbk(const MouseEventArgs& args)
{
	if(mousedowncbk)
		mousedowncbk(this, args, mousedowncbkuser);
	if(pymousedowncbk)
	{
		pse->BeginPythonCode();
		try
			{pymousedowncbk->cbk(boost::ref(this), args, pymousedowncbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void WinForm::RaiseMouseUpCbk(const MouseEventArgs& args)
{
	if(mouseupcbk)
		mouseupcbk(this, args, mouseupcbkuser);
	if(pymouseupcbk)
	{
		pse->BeginPythonCode();
		try
			{pymouseupcbk->cbk(boost::ref(this), args, pymouseupcbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void WinForm::RaiseMouseMoveCbk(const MouseEventArgs& args)
{
	if(mousemovecbk)
		mousemovecbk(this, args, mousemovecbkuser);
	if(pymousemovecbk)
	{
		pse->BeginPythonCode();
		try
			{pymousemovecbk->cbk(boost::ref(this), args, pymousemovecbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void WinForm::RaiseMouseDClickCbk(const MouseEventArgs& args)
{
	if(mousedclickcbk)
		mousedclickcbk(this, args, mousedclickcbkuser);
	if(pymousedclickcbk)
	{
		pse->BeginPythonCode();
		try
			{pymousedclickcbk->cbk(boost::ref(this), args, pymousedclickcbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void WinForm::RaiseMouseWheelCbk(const MouseEventArgs& args)
{
	if(mousewheelcbk)
		mousewheelcbk(this, args, mousewheelcbkuser);
	if(pymousewheelcbk)
	{
		pse->BeginPythonCode();
		try
			{pymousewheelcbk->cbk(boost::ref(this), args, pymousewheelcbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}
void WinForm::RaisePaintCbk()
{
	if(paintcbk)
		paintcbk(this, paintcbkuser);
	if(pypaintcbk)
	{
		pse->BeginPythonCode();
		try
			{pypaintcbk->cbk(boost::ref(this), pypaintcbk->user);}
		catch(boost::python::error_already_set const &) {}
		pse->EndPythonCode();
	}
}

void WinForm::RemovePyCallbacks()
{
	REMOVE(pymousedowncbk);
	REMOVE(pymouseupcbk);
	REMOVE(pymousemovecbk);
	REMOVE(pymousedclickcbk);
	REMOVE(pymousewheelcbk);
	REMOVE(pypaintcbk);
	REMOVE(pyfclosingcbk);
	REMOVE(pyfclosedcbk);
	REMOVE(pyfresizecbk);
	REMOVE(pyfsizechangedcbk);
	REMOVE(pyfmovingcbk);
	REMOVE(pyfmovedcbk);
}

Button* CreateButtonWrapper() {return (Button*)CreateButton();}
/*CheckBox* CreateCheckBoxWrapper() {return (CheckBox*)CreateCheckBox();}
RadioButton* CreateRadioButtonWrapper() {return (RadioButton*)CreateRadioButton();}
GroupBox* CreateGroupBoxWrapper() {return (GroupBox*)CreateGroupBox();}
Label* CreateLabelWrapper() {return (Label*)CreateLabel();}
TextBox* CreateTextBoxWrapper(bool hasvscroll = false) {return (TextBox*)CreateTextBox(hasvscroll);}
RichEdit* CreateRichEditWrapper(bool hashscroll = false, bool hasvscroll = false) {return (RichEdit*)CreateRichEdit(hashscroll, hasvscroll);}
NumericUpDown* CreateNumericUpDownWrapper() {return (NumericUpDown*)CreateNumericUpDown();}
Listview* CreateListviewWrapper() {return (Listview*)CreateListview();}
Treeview* CreateTreeviewWrapper() {return (Treeview*)CreateTreeview();}
TabControl* CreateTabControlWrapper() {return (TabControl*)CreateTabControl();}*/
OpenFileDialog* CreateOpenFileDialogWrapper() {return (OpenFileDialog*)CreateOpenFileDialog();}

BOOST_PYTHON_MODULE(Forms)
{
	// >>> Functions

	def("CreateButton", CreateButtonWrapper, POLICY_MANAGED_BY_C);
	def("CreateOpenFileDialog", CreateOpenFileDialogWrapper, POLICY_MANAGED_BY_C);

	// >>> Enums

	enum_<WindowState>("WindowState")
		.value("Normal", WS_NORMAL)
		.value("Minimized", WS_MINIMIZED)
		.value("Maximized", WS_MAXIMIZED)
	;

	enum_<FormBorderStyle>("FormBorderStyle")
		.value("Default", FBS_DEFAULT)
		.value("ThinBorder", FBS_THINBORDER)
		.value("NoBorder", FBS_NOBORDER)
		.value("OnlyBorder", FBS_ONLYBORDER)
	;

	// >>> Structs

	class_<FormSettings>("FormSettings")
		.def_readwrite("caption", &FormSettings::caption)
		.def_readwrite("windowpos", &FormSettings::windowpos)
		.def_readwrite("clientsize", &FormSettings::clientsize)
		.def_readwrite("wndstate", &FormSettings::wndstate)
		.def_readwrite("fbstyle", &FormSettings::fbstyle)
		.def_readwrite("istoolwindow", &FormSettings::istoolwindow)
		.def_readwrite("hasvscroll", &FormSettings::hasvscroll)
		.def_readwrite("hashscroll", &FormSettings::hashscroll)
		.def_readwrite("parent", &FormSettings::parent)
		.def_readwrite("mdiwindowmenu", &FormSettings::mdiwindowmenu)
		.def_readwrite("options", &FormSettings::options)
	;

	class_<IOpenFileDialog::FileType>("FileType")
		.def_readwrite("description", &IOpenFileDialog::FileType::description)
		.def_readwrite("extension", &IOpenFileDialog::FileType::extension)
	;
	class_<std::vector<IOpenFileDialog::FileType>>("FileTypeList")
        .def(vector_indexing_suite<std::vector<IOpenFileDialog::FileType>>())
    ;

	/*WAVEFORMATEXTENSIBLE* (AudioFormat::*GetWaveFormatExtensible)(WAVEFORMATEXTENSIBLE* result) const = &AudioFormat::GetWaveFormat;
	WAVEFORMATEX* (AudioFormat::*GetWaveFormatEx)(WAVEFORMATEX* result) const = &AudioFormat::GetWaveFormat;
	class_<AudioFormat>("AudioFormat")
		.def(init<DWORD, WORD, DWORD, GUID>())
		.def(init<DWORD, WORD, DWORD>())
		.def(init<>())
		.def_readonly("samplerate", &AudioFormat::samplerate)
		.def_readonly("channelmask", &AudioFormat::channelmask)
		.def_readonly("channels", &AudioFormat::channels)
		.def_readonly("bits", &AudioFormat::bits)
		.def_readonly("subformat", &AudioFormat::subformat)
		.def("FromChannelMask", &FromChannelMaskWrapper, POLICY_MANAGED_BY_PYTHON)
		.def("GetWaveFormatExtensible", GetWaveFormatExtensible, POLICY_MANAGED_BY_C)
		.def("GetWaveFormatEx", GetWaveFormatEx, POLICY_MANAGED_BY_C)
		.def(self == AudioFormat())
		.def(self != AudioFormat())
		.def("__str__", &AudioFormat::ToString) //EDIT: Not working: boost converts string to char* using operator and tries to forward the string as char
	;*/

	// >>> Classes

	class_<Button, boost::noncopyable>("Button", no_init)
		.add_property("Visible", &Button::GetVisible, &Button::SetVisible)
		.add_property("Enabled", &Button::GetEnabled, &Button::SetEnabled)
		.add_property("Text", &Button::GetText, &Button::SetText)
		.add_property("Bounds", &Button::GetBounds, &Button::SetBounds)
		.add_property("Location", &Button::GetLocation, &Button::SetLocation)
		.add_property("Size", &Button::GetSize, &Button::SetSize)
		.add_property("Top", &Button::GetTop, &Button::SetTop)
		.add_property("Left", &Button::GetLeft, &Button::SetLeft)
		.add_property("Width", &Button::GetWidth, &Button::SetWidth)
		.add_property("Height", &Button::GetHeight, &Button::SetHeight)
		.def("SetMouseDownCallback", &Button::SetMouseDownCallbackWrapper)
		.def("SetMouseUpCallback", &Button::SetMouseUpCallbackWrapper)
		.def("SetMouseMoveCallback", &Button::SetMouseMoveCallbackWrapper)
		.def("SetMouseDoubleClickCallback", &Button::SetMouseDoubleClickCallbackWrapper)
		.def("SetMouseWheelCallback", &Button::SetMouseWheelCallbackWrapper)
		.def("SetMouseClickCallback", &Button::SetMouseClickCallbackWrapper)
		.def("SetKeyDownCallback", &Button::SetKeyDownCallbackWrapper)
		.def("SetKeyUpCallback", &Button::SetKeyUpCallbackWrapper)
	;
	objects::register_conversion<Button, IControl>();

	Result (OpenFileDialog::*OpenFileDialog_SetFileTypes)(std::vector<IOpenFileDialog::FileType>& filetypes, UINT defaultindex) = &OpenFileDialog::SetFileTypes;
	class_<OpenFileDialog, boost::noncopyable>("OpenFileDialog", no_init)
		.def("Init", &OpenFileDialog::Init)
		.def("SetFileTypes", OpenFileDialog_SetFileTypes)
		.def("SetDefaultExtension", &OpenFileDialog::SetDefaultExtension)
		.def("Show", &OpenFileDialog::Show)
	;

	class_<Forms, boost::noncopyable>("Forms")
		//.def("Init", &Forms::Init)
		.def("CreateForm", &Forms::CreateFormWrapper, POLICY_MANAGED_BY_C)
		.def("CreateForm", &Forms::CreateFormWrapper2, POLICY_MANAGED_BY_C)
		.def("CreateForm", &Forms::CreateFormFromHwndWrapper, POLICY_MANAGED_BY_C)
		.def("ShowAll", &Forms::ShowAll)
		.def("CloseAll", &Forms::CloseAll)
	;

	class_<WinForm, boost::noncopyable>("WinForm", no_init)
		.def("AddControl", &WinForm::AddControl)
		.def("RemoveControl", &WinForm::RemoveControl)
		.def("Show", &WinForm::Show)
		.def("Hide", &WinForm::Hide)
		.def("BringToFront", &WinForm::BringToFront)
		.def("GetHwnd", &WinForm::GetHwnd, POLICY_MANAGED_BY_C)
		.add_property("Location", &WinForm::GetLocation, &WinForm::SetLocation)
		.add_property("ClientSize", &WinForm::GetClientSize, &WinForm::SetClientSize)
		.add_property("Top", &WinForm::GetTop, &WinForm::SetTop)
		.add_property("Left", &WinForm::GetLeft, &WinForm::SetLeft)
		.add_property("ClientWidth", &WinForm::GetClientWidth, &WinForm::SetClientWidth)
		.add_property("ClientHeight", &WinForm::GetClientHeight, &WinForm::SetClientHeight)
		.add_property("Bounds", &WinForm::GetBounds, &WinForm::SetBounds)
		.add_property("ClientBounds", &WinForm::GetClientBounds, &WinForm::SetClientBounds)
		.add_property("WndState", &WinForm::GetWindowState, &WinForm::SetWindowState)
		.add_property("FBStyle", &WinForm::GetFormBorderStyle, &WinForm::SetFormBorderStyle)
		.def("SetMouseDownCallback", &WinForm::SetMouseDownCallbackWrapper)
		.def("SetMouseUpCallback", &WinForm::SetMouseUpCallbackWrapper)
		.def("SetMouseMoveCallback", &WinForm::SetMouseMoveCallbackWrapper)
		.def("SetMouseDoubleClickCallback", &WinForm::SetMouseDoubleClickCallbackWrapper)
		.def("SetMouseWheelCallback", &WinForm::SetMouseWheelCallbackWrapper)
		.def("SetPaintCallback", &WinForm::SetPaintCallbackWrapper)
		.def("SetDragEnterCallback", &WinForm::SetDragEnterCallbackWrapper)
		.def("SetDragOverCallback", &WinForm::SetDragOverCallbackWrapper)
		.def("SetDragLeaveCallback", &WinForm::SetDragLeaveCallbackWrapper)
		.def("SetDropCallback", &WinForm::SetDropCallbackWrapper)
		.def("SetFormResizeCallback", &WinForm::SetFormResizeCallbackWrapper)
		.def("SetFormSizeChangedCallback", &WinForm::SetFormSizeChangedCallbackWrapper)
		.def("SetFormMovingCallback", &WinForm::SetFormMovingCallbackWrapper)
		.def("SetFormMovedCallback", &WinForm::SetFormMovedCallbackWrapper)
		.def("SetFormClosingCallback", &WinForm::SetFormClosingCallbackWrapper)
		.def("SetFormClosedCallback", &WinForm::SetFormClosedCallbackWrapper)
		.def("Close", &WinForm::CloseWrapper)
	;
}

FORMS_EXTERN_FUNC void __cdecl RegisterScriptableFormsClasses(const IPythonScriptEngine* pse)
{
	if(pse)
		pse->AddModule("Forms", &PyInit_Forms);
}
void SyncWithPython()
{
	AddToBuiltins("fms", (Forms*)&*fms);
}
#else
#include "Forms.h"
void Control::RaiseMouseDownCbk(const MouseEventArgs& args)
{
	if(mousedowncbk)
		mousedowncbk(this->ifc, args, mousedowncbkuser);
}
void Control::RaiseMouseUpCbk(const MouseEventArgs& args)
{
	if(mouseupcbk)
		mouseupcbk(this->ifc, args, mouseupcbkuser);
}
void Control::RaiseMouseMoveCbk(const MouseEventArgs& args)
{
	if(mousemovecbk)
		mousemovecbk(this->ifc, args, mousemovecbkuser);
}
void Control::RaiseMouseDClickCbk(const MouseEventArgs& args)
{
	if(mousedclickcbk)
		mousedclickcbk(this->ifc, args, mousedclickcbkuser);
}
void Control::RaiseMouseWheelCbk(const MouseEventArgs& args)
{
	if(mousewheelcbk)
		mousewheelcbk(this->ifc, args, mousewheelcbkuser);
}
void Control::RaiseMouseClickCbk()
{
	if(mclickcbk)
		mclickcbk(this->ifc, mclickcbkuser);
}
void Control::RaiseKeyDownCbk(WORD vkey)
{
	if(keydowncbk)
		keydowncbk(this->ifc, vkey, keydowncbkuser);
}
void Control::RaiseKeyUpCbk(WORD vkey)
{
	if(keyupcbk)
		keyupcbk(this->ifc, vkey, keyupcbkuser);
}
void Control::RemovePyCallbacks() {}

void WinForm::RaiseMouseDownCbk(const MouseEventArgs& args)
{
	if(mousedowncbk)
		mousedowncbk(this, args, mousedowncbkuser);
}
void WinForm::RaiseMouseUpCbk(const MouseEventArgs& args)
{
	if(mouseupcbk)
		mouseupcbk(this, args, mouseupcbkuser);
}
void WinForm::RaiseMouseMoveCbk(const MouseEventArgs& args)
{
	if(mousemovecbk)
		mousemovecbk(this, args, mousemovecbkuser);
}
void WinForm::RaiseMouseDClickCbk(const MouseEventArgs& args)
{
	if(mousedclickcbk)
		mousedclickcbk(this, args, mousedclickcbkuser);
}
void WinForm::RaiseMouseWheelCbk(const MouseEventArgs& args)
{
	if(mousewheelcbk)
		mousewheelcbk(this, args, mousewheelcbkuser);
}
void WinForm::RaisePaintCbk()
{
	if(paintcbk)
		paintcbk(this, paintcbkuser);
}
void WinForm::RemovePyCallbacks() {}

void SyncWithPython() {}
#endif