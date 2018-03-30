#ifdef ENGINE_SCRIPTABLE
#include "ISEngine.h"
#include "ISPythonScriptEngine.h"

//#define BOOST_PYTHON_STATIC_LIB
#include <boost\python.hpp>
using namespace boost::python;

template<class T> struct custom_string_to_python_str
{
	static PyObject* convert(T const& s)
	{
		return boost::python::incref(boost::python::object(s.ToTCharString()).ptr());
	}
};

template<class T> struct custom_string_from_python_str
{
	custom_string_from_python_str()
	{
		boost::python::converter::registry::push_back(&convertible, &construct, boost::python::type_id<T>());
	}

	static void* convertible(PyObject* obj_ptr)
	{
		if(!PyUnicode_Check(obj_ptr))
			return 0;
		return obj_ptr;
	}

	static void construct(
	PyObject* obj_ptr,
	boost::python::converter::rvalue_from_python_stage1_data* data)
	{
		const char* value = (const char*)PyUnicode_1BYTE_DATA(obj_ptr);
		if(value == 0)
			boost::python::throw_error_already_set();
		void* storage = ((boost::python::converter::rvalue_from_python_storage<T>*)data)->storage.bytes;
		new (storage) T(value);
		data->convertible = storage;
	}
};

HWND MakeHWND(int hwnd)
{
	return (HWND)hwnd;
}
void MessageBoxWrapper(String text, String title)
{
	MessageBox(NULL, text, title, 0);
}
Point<int> GetCursorPosWrapper()
{
	POINT cursorpos;
	GetCursorPos(&cursorpos);
	return Point<int>(cursorpos.x, cursorpos.y);
}
Point<int> ScreenToClientWrapper(HWND hwnd, Point<int> screenpos)
{
	POINT clientpos = {screenpos.x, screenpos.y};
	ScreenToClient(hwnd, &clientpos);
	return Point<int>(clientpos.x, clientpos.y);
}

BOOST_PYTHON_MODULE(Engine)
{
	// >>> Functions

	def("MakeHWND", MakeHWND, POLICY_MANAGED_BY_C);
	def("MessageBox", MessageBoxWrapper);
	def("GetCursorPos", GetCursorPosWrapper);
	def("ScreenToClient", ScreenToClientWrapper);

	// >>> Enums

	enum_<Orientation>("Orientation")
		.value("Horizontal", OT_HORIZONTAL)
		.value("Vertical", OT_VERTICAL)
	;
	enum_<DockType>("DockType")
		.value("TopLeft", DT_TOPLEFT)
		.value("TopCenter", DT_TOPCENTER)
		.value("TopRight", DT_TOPRIGHT)
		.value("MiddleLeft", DT_MIDDLELEFT)
		.value("MiddleCenter", DT_MIDDLECENTER)
		.value("MiddleRight", DT_MIDDLERIGHT)
		.value("BottomLeft", DT_BOTTOMLEFT)
		.value("BottomCenter", DT_BOTTOMCENTER)
		.value("BottomRight", DT_BOTTOMRIGHT)
	;

	// >>> Structs

    class_<HWND__, boost::noncopyable>("HWND")
		.def_readwrite("unused", &HWND__::unused)
	;

	class_<Result>("Result")
	;

	class_<PtrWrapper>("PtrWrapper", no_init)
    ;

	class_<Point<int>>("PointI")
		.def(init<int, int>())
		.def_readwrite("x", &Point<int>::x)
		.def_readwrite("y", &Point<int>::y)
	;
	class_<Point<UINT>>("PointUI")
		.def(init<UINT, UINT>())
		.def_readwrite("x", &Point<UINT>::x)
		.def_readwrite("y", &Point<UINT>::y)
	;
	class_<Point<float>>("PointF")
		.def(init<float, float>())
		.def_readwrite("x", &Point<float>::x)
		.def_readwrite("y", &Point<float>::y)
	;

	class_<Size<int>>("SizeI")
		.def(init<int, int>())
		.def_readwrite("width", &Size<int>::width)
		.def_readwrite("height", &Size<int>::height)
	;
	class_<Size<UINT>>("SizeUI")
		.def(init<UINT, UINT>())
		.def_readwrite("width", &Size<UINT>::width)
		.def_readwrite("height", &Size<UINT>::height)
	;
	class_<Size<float>>("SizeF")
		.def(init<float, float>())
		.def_readwrite("width", &Size<float>::width)
		.def_readwrite("height", &Size<float>::height)
	;

	class_<Rect<int>>("RectI")
		.def(init<int, int, int, int>())
		.def(init<Point<int>, Size<int>>())
		.def(init<int, int>())
		.add_property("x", &Rect<int>::GetX, &Rect<int>::SetX)
		.add_property("y", &Rect<int>::GetY, &Rect<int>::SetY)
		.add_property("width", &Rect<int>::GetWidth, &Rect<int>::SetWidth)
		.add_property("height", &Rect<int>::GetHeight, &Rect<int>::SetHeight)
	;
	class_<Rect<UINT>>("RectUI")
		.def(init<UINT, UINT, UINT, UINT>())
		.def(init<Point<UINT>, Size<UINT>>())
		.def(init<UINT, UINT>())
		.add_property("x", &Rect<UINT>::GetX, &Rect<UINT>::SetX)
		.add_property("y", &Rect<UINT>::GetY, &Rect<UINT>::SetY)
		.add_property("width", &Rect<UINT>::GetWidth, &Rect<UINT>::SetWidth)
		.add_property("height", &Rect<UINT>::GetHeight, &Rect<UINT>::SetHeight)
	;
	class_<Rect<float>>("RectF")
		.def(init<float, float, float, float>())
		.def(init<Point<float>, Size<float>>())
		.def(init<float, float>())
		.add_property("x", &Rect<float>::GetX, &Rect<float>::SetX)
		.add_property("y", &Rect<float>::GetY, &Rect<float>::SetY)
		.add_property("width", &Rect<float>::GetWidth, &Rect<float>::SetWidth)
		.add_property("height", &Rect<float>::GetHeight, &Rect<float>::SetHeight)
	;

	class_<MouseEventArgs>("MouseEventArgs", no_init)
		.def_readwrite("mousepos", &MouseEventArgs::mousepos)
		.def_readwrite("wheelpos", &MouseEventArgs::wheelpos)
		.add_property("ldown", &MouseEventArgs::IsLeftButtonDown)
		.add_property("mdown", &MouseEventArgs::IsMiddleButtonDown)
		.add_property("rdown", &MouseEventArgs::IsRightButtonDown)
		.add_property("x1down", &MouseEventArgs::IsX1ButtonDown)
		.add_property("x2down", &MouseEventArgs::IsX2ButtonDown)
		.add_property("ctrldown", &MouseEventArgs::IsCtrlKeyDown)
		.add_property("shiftdown", &MouseEventArgs::IsShiftKeyDown)
	;

	// >>> Classes

	/*void (Console::*Print)(const String&) = &Console::Print;
	void (Console::*PrintLine)(const String&) = &Console::PrintLine;
	void (Console::*PrintD3D)(const String&) = &Console::PrintD3D;
	class_<Console, boost::noncopyable>("Console")
		.def_readwrite("Print", &Print)
		.def_readwrite("PrintLine", &PrintLine)
		.def_readwrite("PrintD3D", &PrintD3D)
	;*/

	class_<Engine::Time, boost::noncopyable>("Time", no_init)
		.def_readwrite("t", &Engine::Time::t)
		.def_readwrite("dt", &Engine::Time::dt)
		.def("Configure", &Engine::Time::Configure)
		.def("Step", &Engine::Time::Step)
	;

	class_<Engine, boost::noncopyable>("Engine", no_init)
		.def_readwrite("time", &Engine::time)
		.def("Work", &Engine::Work)
	;

	boost::python::to_python_converter<String, custom_string_to_python_str<String>>();
	boost::python::to_python_converter<FilePath, custom_string_to_python_str<FilePath>>();
	custom_string_from_python_str<String>();
	custom_string_from_python_str<FilePath>();
}

void RegisterScriptableEngineClasses(const IPythonScriptEngine* pse)
{
	if(pse)
		pse->AddModule("Engine", &PyInit_Engine);
}
void SyncEngineWithPython()
{
	//AddToBuiltins("cle", &*cle);
	AddToBuiltins("eng", &*eng);
}
#endif