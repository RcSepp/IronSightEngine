#ifndef __SQLITE_H
#define __SQLITE_H

#include "ISPythonScriptEngine.h"

namespace boost {namespace python {namespace api
{
	class object;
}}}

//#define BOOST_PYTHON_STATIC_LIB
#define BOOST_LIB_DIAGNOSTIC
#include <boost\python.hpp>
using namespace boost::python;

/*#ifdef _DEBUG
	#undef _DEBUG
	#include <Python.h>
	#define _DEBUG
#else
	#include <Python.h>
#endif*/
//#include "boost/python/detail/wrap_python.hpp" // <- Use instead of #include <Python.h> when also using boost in other files


//-----------------------------------------------------------------------------
// PYTHON MODULES
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Python module: OutputRedirectModule
//-----------------------------------------------------------------------------
PyObject* OutputRedirectModule_OnOutput(PyObject *self, PyObject *args);
PyObject* OutputRedirectModule_ExceptHook(PyObject *self, PyObject *args);
static PyMethodDef OutputRedirectModule_Methods[] = {{"OnOutput", OutputRedirectModule_OnOutput, METH_VARARGS, "Callback function for output redirection"},
													 {"ExceptHook", OutputRedirectModule_ExceptHook, METH_VARARGS, "Callback function for exception hooking"},
													 {NULL, NULL, 0, NULL}};
static PyModuleDef OutputRedirectModule = {PyModuleDef_HEAD_INIT, "__outputredirect__", NULL, -1, OutputRedirectModule_Methods, NULL, NULL, NULL, NULL};


//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct RegisteredModule
{
	String name;
	INIT_PYMODULE_CALLBACK initfunc;
	RegisteredModule(const String& name, INIT_PYMODULE_CALLBACK initfunc) : name(name), initfunc(initfunc) {}
};


//-----------------------------------------------------------------------------
// CLASSES
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: PythonScript
// Desc: Contains a single python module loaded from file
//-----------------------------------------------------------------------------
class PythonScriptEngine;
class PythonScript : public IPythonScript
{
private:
	PythonScriptEngine* parent;
	PyObject* pymodule;
	String modulename;
	static std::map<FilePath, PyObject*> loadedmodules;

public:
	PythonScript(PythonScriptEngine* parent) : parent(parent), pymodule(NULL) {}
	Result LoadPy(const FilePath& filename);
	const String& GetModuleName() const {return modulename;}
	PyObject* GetModule() const {return pymodule;}
	/*void SetCapsule(const char* capsulename, LPVOID capsule) {return PythonAPI::SetModulCapsule(pymodule, capsulename, capsule);}*/
	Result GetFunctions(int* numfuncs, PyObject*** pyfuncs, String** funcnames = NULL) const {return pse->GetModuleFunctions(pymodule, numfuncs, pyfuncs, funcnames);}
	void Close();
	void Release();
};

//-----------------------------------------------------------------------------
// Name: PythonScriptEngine
// Desc: A manager class for python objects
//-----------------------------------------------------------------------------
class PythonScriptEngine : public IPythonScriptEngine
{
private:
	bool isinitialized;
	mutable std::list<RegisteredModule> registeredmodules;
	mutable bool isexecuting;

public:
	std::list<PythonScript*> scripts;
	boost::python::object main_module, main_namespace;

	PythonScriptEngine() : isinitialized(false), isexecuting(false) {}

	void Sync(GLOBALVARDEF_LIST);
	void AddModule(const String& name, INIT_PYMODULE_CALLBACK initfunc) const;
	void Init();
	Result Execute(const String& code);
	Result ExecuteIntoBuiltins(const String& name, const String& code);
	Result AddToBuiltins(const String& name, PyObject* pyobject);
	Result LoadScript(const FilePath& filename, LPPYTHONSCRIPT* script);
	Result GetModuleFunctions(PyObject* pymodule, int* numfuncs, PyObject*** pyfuncs, String** funcnames) const;
	void HookExceptions(EXCEPTIONHOOK_CALLBACK cbk, LPVOID user);
	void UnhookExceptions();
	void Throw(const Result& rlt) const;
	void Throw(const Result* rlt) const;
	PyObject* Call(PyObject* pyfunc, const char* format, ...) const;
	PyObject* Call(PyObject* pyfunc) const;
	Result Invoke(PyObject* pyfunc, const char* argtypes, ...) const; // Deprecated: Use Call() instead
	/*Result Invoke(const String& funcname, const char* argtypes, ...) const; // Deprecated: Use Call() instead*/
	Result InvokeVaList(PyObject* pyfunc, const char* argtypes, va_list& valist) const; // Deprecated: Use Call() instead
	/*Result InvokeVaList(const String& funcname, const char* argtypes, va_list& valist) const; // Deprecated: Use Call() instead*/
	/*PythonDictionary* GetGlobals() const;
	PythonDictionary* GetLocals() const;*/
	PythonDictionary* GetBuiltins() const;

	void BeginPythonCode() const {isexecuting = true;}
	void EndPythonCode() const;

	std::shared_ptr<void> GetHandle(PyObject* pyobject) const;

	void Release();
};

#endif