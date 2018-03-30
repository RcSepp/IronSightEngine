#ifndef __ISPYTHONSCRIPTENGINE_H
#define __ISPYTHONSCRIPTENGINE_H

#include <ISEngine.h>

struct _object;
#ifndef PyObject
typedef _object PyObject;
#endif

struct _traceback;
#ifndef PyTracebackObject
typedef _traceback PyTracebackObject;
#endif



//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_PYTHONSCRIPTENGINE
	#define PYTHONSCRIPTENGINE_EXTERN_FUNC		__declspec(dllexport)
#else
	#define PYTHONSCRIPTENGINE_EXTERN_FUNC		__declspec(dllimport)
#endif

#define POLICY_MANAGED_BY_PYTHON	return_value_policy<manage_new_object>()
#define POLICY_MANAGED_BY_C			return_value_policy<reference_existing_object>()


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
class PythonTraceback;
typedef PyObject* (*INIT_PYMODULE_CALLBACK)(void);
typedef void (__stdcall* EXCEPTIONHOOK_CALLBACK)(const String& extype, const String& exstring, PythonTraceback& extraceback, LPVOID user);


//-----------------------------------------------------------------------------
// CLASSES
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: PythonDictionary
// Desc: Contains a single Python dictionary object
//-----------------------------------------------------------------------------
class PYTHONSCRIPTENGINE_EXTERN_FUNC PythonDictionary
{
private:
	PyObject* dict;
	bool ownref;

public:
	typedef _W64 int iterator;
	static iterator begin() {return 0;}

	PythonDictionary();
	PythonDictionary(PythonDictionary& other);
	PythonDictionary(PyObject* dict);
	~PythonDictionary();
	void Insert(PyObject* pyindex, PyObject* pyvalue);
	PyObject* Get(PyObject* pyindex);
	bool Next(iterator* i, PyObject** key = NULL, PyObject** value = NULL);
	String ToString();
};

//-----------------------------------------------------------------------------
// Name: PythonTraceback
// Desc: Contains a single Python traceback object
//-----------------------------------------------------------------------------
class PYTHONSCRIPTENGINE_EXTERN_FUNC PythonTraceback
{
private:
	PyTracebackObject* traceback;
	bool isretreived;

public:

	PythonTraceback() : traceback(NULL), isretreived(false) {}
	PythonTraceback(PythonTraceback& other);
	PythonTraceback(PyTracebackObject* dict);
	~PythonTraceback();

	bool IsValid() {return traceback != NULL;}
	int GetLineNumber();
	String GetFilename();
	PythonDictionary GetLocals();
	PythonDictionary GetGlobals();
	PythonDictionary GetBuiltins();
	bool HasNext();
	PythonTraceback Next();
};

//-----------------------------------------------------------------------------
// Name: IPythonScript
// Desc: Interface to the PythonScript class
//-----------------------------------------------------------------------------
typedef class IPythonScript
{
public:
	virtual const String& GetModuleName() const = 0;
	virtual PyObject* GetModule() const = 0;
	/*virtual void SetCapsule(const char* capsulename, LPVOID capsule) = 0;*/
	virtual Result GetFunctions(int* numfuncs, PyObject*** pyfuncs = NULL, String** funcnames = NULL) const = 0;
	virtual void Close() = 0;
	virtual void Release() = 0;
}* LPPYTHONSCRIPT;

//-----------------------------------------------------------------------------
// Name: IPythonScriptEngine
// Desc: Interface to the PythonScriptEngine class
//-----------------------------------------------------------------------------
typedef class IPythonScriptEngine
{
public:
	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual void AddModule(const String& name, INIT_PYMODULE_CALLBACK initfunc) const = 0;
	virtual void Init() = 0;
	virtual Result Execute(const String& code) = 0;
	virtual Result ExecuteIntoBuiltins(const String& name, const String& code) = 0;
	virtual Result AddToBuiltins(const String& name, PyObject* pyobject) = 0;
	virtual Result LoadScript(const FilePath& filename, LPPYTHONSCRIPT* script) = 0;
	virtual Result GetModuleFunctions(PyObject* pymodule, int* numfuncs, PyObject*** pyfuncs = NULL, String** funcnames = NULL) const = 0;
	virtual void HookExceptions(EXCEPTIONHOOK_CALLBACK cbk, LPVOID user) = 0;
	virtual void UnhookExceptions() = 0;
	virtual void Throw(const Result& rlt) const = 0;
	virtual void Throw(const Result* rlt) const = 0;
	virtual PyObject* Call(PyObject* pyfunc, const char* format, ...) const = 0;
	virtual PyObject* Call(PyObject* pyfunc) const = 0;
	virtual Result Invoke(PyObject* pyfunc, const char* argtypes, ...) const = 0; // Deprecated: Use Call() instead
	/*virtual Result Invoke(const String& funcname, const char* argtypes, ...) const = 0; // Deprecated: Use Call() instead*/
	virtual Result InvokeVaList(PyObject* pyfunc, const char* argtypes, va_list& valist) const = 0; // Deprecated: Use Call() instead
	/*virtual Result InvokeVaList(const String& funcname, const char* argtypes, va_list& valist) const = 0; // Deprecated: Use Call() instead*/
	/*virtual PythonDictionary* GetGlobals() const = 0;
	virtual PythonDictionary* GetLocals() const = 0;*/
	virtual PythonDictionary* GetBuiltins() const = 0;
	virtual void BeginPythonCode() const = 0;
	virtual void EndPythonCode() const = 0;
	static String* PyObjectToString(PyObject* pyobject, String* result);
	virtual std::shared_ptr<void> GetHandle(PyObject* pyobject) const = 0;

	virtual void Release() = 0;
}* LPPYTHONSCRIPTENGINE;

extern "C" PYTHONSCRIPTENGINE_EXTERN_FUNC LPPYTHONSCRIPTENGINE __cdecl CreatePythonScriptEngine();
extern "C" PYTHONSCRIPTENGINE_EXTERN_FUNC bool __cdecl CheckPythonSupport(LPTSTR* missingdllname);


#ifdef _WIN32 // gcc compiles this, even though it's not instantiated!
//-----------------------------------------------------------------------------
// Name: PyExceptionToString()
// Desc: MFetch Python exception and format as string
//-----------------------------------------------------------------------------
template<int I> String PyExceptionToString()
{
	String errstr("Unknown Python error");
	PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
	PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);

	if(type_ptr != NULL){
		handle<> h_type(type_ptr);
		str type_pstr(h_type);
		extract<const char*> e_type_pstr(type_pstr);
		if(e_type_pstr.check())
			errstr = e_type_pstr();
		else
			errstr = "Unknown exception type";
	}
	if(value_ptr != NULL){
		handle<> h_val(value_ptr);
		str a(h_val);
		extract<const char*> returned(a);
		if(returned.check())
			errstr <<=  String(": ") << returned();
		else
			errstr <<= ": Unparseable Python error: ";
	}
	if(traceback_ptr != NULL){
		handle<> h_tb(traceback_ptr);
		object tb(import("traceback"));
		object fmt_tb(tb.attr("format_tb"));
		object tb_list(fmt_tb(h_tb));
		object tb_str(str("\n").join(tb_list));
		extract<const char*> returned(tb_str);
		if(returned.check())
			errstr <<= String(": ") << returned();
		else
			errstr <<= ": Unparseable Python traceback";
	}
	return errstr;
}

//-----------------------------------------------------------------------------
// Name: AddToBuiltins()
// Desc: Make object accessible from Python's main namespace
//-----------------------------------------------------------------------------
template<class T> void AddToBuiltins(const String& objectname, T object)
{
	try
	{
		boost::python::object main_module((handle<>(borrowed(PyImport_AddModule("__main__")))));
		boost::python::object main_namespace = main_module.attr("__dict__");
		main_namespace["__builtins__"].attr(objectname) = ptr(object);
	}
	catch(boost::python::error_already_set const &)
		{OutputDebugString(String("Error in Python: ") << PyExceptionToString<0>() << '\n');}
}
#endif

#endif