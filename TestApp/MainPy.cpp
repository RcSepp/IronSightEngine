#include <ISEngine.h>
#include <ISForms.h>

#ifdef _DEBUG
	#undef _DEBUG
	#include <Python.h>
	#define _DEBUG
#else
	#include <Python.h>
#endif
#include <structmember.h>


#include <vld.h>


#ifdef _DEBUG
	#pragma comment (lib, "ISEngine_d.lib")
#else
	#pragma comment (lib, "ISEngine.lib")
#endif


Result __stdcall Init();
Result __stdcall PostInit();
void __stdcall TimerElapsed(LPVOID user);
void __stdcall End();

HINSTANCE __hInst;
int __nCmdShow;


LPWINFORM form1 = NULL;



class SomeClass
{
public:
	void SomeMethod()
	{
		cle->PrintLine("SomeMethod()");
	}
};

PyObject* MpuGraphics_DrawLine(PyObject *self, PyObject *args);
PyObject* MpuGraphics_DrawRect(PyObject *self, PyObject *args);
PyObject* MpuGraphics_FillRect(PyObject *self, PyObject *args);
PyObject* MpuGraphics_DrawText(PyObject *self, PyObject *args);

struct PyHeadStruct
{
	PyObject_HEAD
};
typedef struct SomeClassWrapper : public PyHeadStruct, public SomeClass
{
	template<void (SomeClass::*F)()> static PyObject* SomeMethodWrapper(PyObject *self, PyObject *args)
	{
		((SomeClass*)self)->*F();
		Py_RETURN_NONE;
	}
} MpuGraphics_Object;
/*PyObject* SomeMethodWrapper(PyObject *self, PyObject *args)
{
	((MpuGraphics_Object*)self)->cls->SomeMethod();
	Py_RETURN_NONE;
}*/

/*template<class C, void (C::*F)()> PyObject* SomeMethodWrapper(PyObject *self, PyObject *args)
{
	((C*)self)->**F();
	//((MpuGraphics_Object*)self)->cls->SomeMethod();
	Py_RETURN_NONE;
}*/

/*template<void (SomeClass::*F)()> static PyObject* SomeClass::Wrapper(PyObject *self, PyObject *args)
{
	Py_RETURN_NONE;
}*/

/*template <class F>
self& def(char const* name, F f)
{
    this->def_impl(
        detail::unwrap_wrapper((W*)0)
        , name, f, detail::def_helper<char const*>(0), &f);
    return *this;
}*/

PyObject* MpuGraphics_Constructor(PyTypeObject *type, PyObject *args, PyObject *kwds);
int MpuGraphics_Init(MpuGraphics_Object *self, PyObject *args, PyObject *kwds);
void MpuGraphics_Destructor(MpuGraphics_Object* self);
static PyMemberDef MpuGraphics_Members[] = {
	{NULL}
};
static PyMethodDef MpuGraphics_Methods[] = {
	//{"SomeMethod", SomeMethodWrapper<SomeClass, &SomeClass::SomeMethod>, METH_VARARGS, "SomeMethod description"},
	{"SomeMethod", SomeClassWrapper::SomeMethodWrapper<&SomeClass::SomeMethod>, METH_VARARGS, "SomeMethod description"},
	{NULL}
};
static PyTypeObject MpuGraphics_Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"mpu.SomeClass", // tp_name
	sizeof(MpuGraphics_Object), // tp_basicsize
	0, // tp_itemsize
	0,//(destructor)MpuGraphics_Destructor, // tp_dealloc
	0, // tp_print
	0, // tp_getattr
	0, // tp_setattr
	0, // tp_reserved
	0, // tp_repr
	0, // tp_as_number
	0, // tp_as_sequence
	0, // tp_as_mapping
	0, // tp_hash 
	0, // tp_call
	0, // tp_str
	0, // tp_getattro
	0, // tp_setattro
	0, // tp_as_buffer
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // tp_flags
	"SomeClass doc", // tp_doc
	0, // tp_traverse
	0, // tp_clear
	0, // tp_richcompare
	0, // tp_weaklistoffset
	0, // tp_iter
	0, // tp_iternext
	MpuGraphics_Methods, // tp_methods
	MpuGraphics_Members, // tp_members
	0, // tp_getset
	0, // tp_base
	0, // tp_dict
	0, // tp_descr_get
	0, // tp_descr_set
	0, // tp_dictoffset
	0,//(initproc)MpuGraphics_Init, // tp_init
	0, // tp_alloc
	0,//(newfunc)MpuGraphics_Constructor, // tp_new
};


PyObject* PythonMPU_DefineConstant(PyObject *self, PyObject *args);
PyObject* PythonMPU_LoadChannels(PyObject *self, PyObject *args);
PyObject* PythonMPU_LoadConstantMemory(PyObject *self, PyObject *args);
PyObject* PythonMPU_Unload(PyObject *self, PyObject *args);
PyObject* PythonMPU_MemAccess(PyObject *self, PyObject *args);
PyObject* PythonMPU_Priority(PyObject *self, PyObject *args);
static PyMethodDef MpuBasicCommandModule_Methods[] = {{NULL, NULL, 0, NULL}};
static PyModuleDef MpuBasicCommandModule = {PyModuleDef_HEAD_INIT, "mpu", "Contains all basic commands for interacting with the MPU", -1, MpuBasicCommandModule_Methods, NULL, NULL, NULL, NULL};
PyMODINIT_FUNC PyInit_Mpu(void)
{
	// Create graphics class
	MpuGraphics_Type.tp_new = PyType_GenericNew;
	if(PyType_Ready(&MpuGraphics_Type) < 0)
		return NULL;

	// Create mpu module
	PyObject* pymodule = PyModule_Create(&MpuBasicCommandModule);
	if(!pymodule)
		return NULL;

	// Add graphics class to mpu module
	Py_INCREF(&MpuGraphics_Type);
	PyModule_AddObject(pymodule, "SomeClass", (PyObject *)&MpuGraphics_Type);

	return pymodule;
}



#pragma region Module OutputRedirectModule
//-----------------------------------------------------------------------------
// Python module: OutputRedirectModule
// Functions: {Python_OnOutput}
//-----------------------------------------------------------------------------
static PyObject* Python_OnOutput(PyObject *self, PyObject *args)
{
	char* str;
	if(!PyArg_ParseTuple(args, "s", &str))
		return NULL;
	cle->Print(str);
	Py_RETURN_NONE;
}
static PyMethodDef OutputRedirectModule_Methods[] = {{"OnOutput", Python_OnOutput, METH_VARARGS, "Callback function for output redirection"},
													 {NULL, NULL, 0, NULL}};
static PyModuleDef OutputRedirectModule = {PyModuleDef_HEAD_INIT, "__outputredirect__", NULL, -1, OutputRedirectModule_Methods, NULL, NULL, NULL, NULL};
PyMODINIT_FUNC PyInit_OutputRedirectModule(void) {return PyModule_Create(&OutputRedirectModule);}
#pragma endregion



void InitPython()
{
	// Load output redirection module
	PyImport_AppendInittab("outputredirect", &PyInit_OutputRedirectModule);
	PyImport_AppendInittab("mpu", &PyInit_Mpu);

	// Initialize python
/*#ifdef _WIN32
Py_SetPythonHome("D:\\Programs\\Python-2.7.6");
#endif*/
	Py_Initialize(); // Initialize python core

	// Invoke python code to enable output redirection
	const char* redirectoutputcode =
"import sys\n\
import outputredirect\n\
class ErrorRedirectClass:\n\
	def write(self, txt):\n\
		outputredirect.OnOutput(txt)\n\
	def flush(self):\n\
		pass\n\
redirectclass = ErrorRedirectClass()\n\
sys.stdout = redirectclass\n\
sys.stderr = redirectclass";
	PyRun_SimpleString(redirectoutputcode);

	/*// Load RayTracer module
	PyRun_SimpleString("from RayTracer import *");
	PyObject *pymodulname = PyString_FromString("RayTracer");
	pyraytracer = PyImport_Import(pymodulname);
	Py_DECREF(pymodulname);*/
}
void FinalizePython()
{
	// Finalize python
	Py_Finalize();
}
Result LoadPy(const FilePath& filename)
{
	// Split filename into path, extensionless name and extension
	FilePath fpath, fname;
	String fext;
	if(filename.GetParentDir(&fpath)) // Get path
		// Path is absolute
		filename.Substring(fpath.length(), &fname); // Trim path
	else
		// Path is relative
		filename.Copy(&fname);

	fname.GetExtension(&fext); // Get extension
	fname.TrimExtension(); // Trim extension

	// Validate extension
	if(fext != String("py"))
		return ERR(String("File is not a python file of the form *.py: ") << filename);

	// Add fpath to pythons module search paths
	if(fpath)
	{
		LPWSTR path = Py_GetPath();
		rsize_t newpathlen = wcslen(path) + fpath.length() + 2; // 2... ; + \n
		LPWSTR newpath = new WCHAR[newpathlen], fpathw = fpath.ToWCharString();
		wcscpy_s(newpath, newpathlen, path);
		wcscat_s(newpath, newpathlen, L";");
		wcscat_s(newpath, newpathlen, fpathw);
		PySys_SetPath(newpath);
		delete newpath;
		delete fpathw;
	}

	// Load module
	PyObject* pymodule = PyImport_ImportModule(fname);

	// Print errors
	if(!pymodule)
	{
		PyErr_Print();
		return ERR(String("Error loading python script file: ") << filename);
	}

	return R_OK;
}






int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow)
{
	Result rlt;

__hInst = hInst;
__nCmdShow = nCmdShow;

	// Create Engine, Forms and Direct3D
	if(!(eng = new Engine(hInst)) || !(fms = CreateForms()))
		MessageBox(NULL, "Out of memory", "Iron Sight Engine", 0);

	// Register the engines window class
	IFFAILED(eng->RegisterWinClass(NULL, (HBRUSH)COLOR_WINDOW)) //(HBRUSH)COLOR_WINDOW
		MessageBox(NULL, rlt.GetLastResult(), "Iron Sight Engine", 0);

	CoInitialize(NULL);

	// Enter main loop
	IFFAILED(eng->Run(Init, PostInit, NULL, End))
	{
		OutputDebugString(rlt.GetLastResult());
		OutputDebugString("\n");
	}

	CoUninitialize();

	delete eng;

	return 1;
}

Result __stdcall Init()
{
	Result rlt;

	// Init Forms
	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create WinForm
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(800, 600);
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(0, 0);
//frmsettings.wndstate = WS_MAXIMIZED;
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form1));

	InitPython();

	return R_OK;
}

Result __stdcall PostInit()
{
	Result rlt;

	CHKRESULT(LoadPy("MainPy.py"));

	return R_OK;
}

void __stdcall End()
{
	FinalizePython();

	RELEASE(cle);
	RELEASE(fms);
}