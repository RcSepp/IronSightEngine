#include "PythonScriptEngine.h"


//-----------------------------------------------------------------------------
// Name: CreatePythonScriptEngine()
// Desc: DLL API for creating an instance of PythonScriptEngine
//-----------------------------------------------------------------------------
PYTHONSCRIPTENGINE_EXTERN_FUNC LPPYTHONSCRIPTENGINE __cdecl CreatePythonScriptEngine()
{
	return CheckPythonSupport(NULL) ? new PythonScriptEngine() : NULL;
}

//-----------------------------------------------------------------------------
// Name: CheckPythonScriptEngineSupport()
// Desc: DLL API for checking support for PythonScriptEngine on the target system
//-----------------------------------------------------------------------------
PYTHONSCRIPTENGINE_EXTERN_FUNC bool __cdecl CheckPythonSupport(LPTSTR* missingdllname)
{
	if(!Engine::FindDLL("python33.dll", missingdllname))
		return false;

   return true;
}

PyMODINIT_FUNC PyInit_outputredirect(void)
{
	return PyModule_Create(&OutputRedirectModule);
}

//-----------------------------------------------------------------------------
// Name: AddModule()
// Desc: Register module to be added and initialized during Init()
//-----------------------------------------------------------------------------
void PythonScriptEngine::AddModule(const String& name, INIT_PYMODULE_CALLBACK initfunc) const
{
	if(isinitialized)
		LOG("Warning: PythonScriptEngine::AddModule() called after PythonScriptEngine::Init(). Module ignored");
	else
		registeredmodules.push_back(RegisteredModule(name, initfunc));
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize python
//-----------------------------------------------------------------------------
void PythonScriptEngine::Init()
{
	// Append output redirection module to standard modules
	PyImport_AppendInittab("__outputredirect__", &PyInit_outputredirect);

	// Append registered modules
	std::list<RegisteredModule>::iterator iter;
	LIST_FOREACH(registeredmodules, iter)
		PyImport_AppendInittab(iter->name, iter->initfunc);

	Py_Initialize();
	isinitialized = true;

	// Get main module and namespace
	main_module = object((handle<>( borrowed( PyImport_AddModule( "__main__" )))));
	main_namespace = main_module.attr("__dict__");

	// Load registered modules
	LIST_FOREACH(registeredmodules, iter)
		PyDict_SetItemString(main_namespace.ptr(), iter->name, PyImport_ImportModule(iter->name));

	// Invoke python code to enable output redirection
	const char* redirectoutputcode =
"import sys\n\
import __outputredirect__\n\
class ErrorRedirectClass:\n\
	def write(self, txt):\n\
		__outputredirect__.OnOutput(txt)\n\
	def flush(self):\n\
		pass\n\
redirectclass = ErrorRedirectClass()\n\
sys.stdout = redirectclass\n\
sys.stderr = redirectclass\n\
sys.excepthook = __outputredirect__.ExceptHook\n";
	PyRun_SimpleString(redirectoutputcode);
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void PythonScriptEngine::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: Execute()
// Desc: Send raw commands to python interpreter
//-----------------------------------------------------------------------------
Result PythonScriptEngine::Execute(const String& code)
{
	BeginPythonCode();
	try
		{PyRun_SimpleString(code);}
		//{handle<> result((PyRun_String(code, Py_file_input, main_namespace.ptr(), main_namespace.ptr())));}
	catch(boost::python::error_already_set const &)
	{
		EndPythonCode();
		return ERR(String("Error in Python: ") << PyExceptionToString<0>() << '\n');
	}
	EndPythonCode();
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: ExecuteIntoBuiltins()
// Desc: Send raw commands to python interpreter and store result inside globally accessible builtin memory
//-----------------------------------------------------------------------------
Result PythonScriptEngine::ExecuteIntoBuiltins(const String& name, const String& code)
{
	BeginPythonCode();
	try
	{
		handle<> result((PyRun_String(code, Py_eval_input, main_namespace.ptr(), main_namespace.ptr())));
		main_namespace["__builtins__"].attr(name) = result;
	}
	catch(boost::python::error_already_set const &)
	{
		EndPythonCode();
		return ERR(String("Error in Python: ") << PyExceptionToString<0>() << '\n');
	}
	EndPythonCode();
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: AddToBuiltins()
// Desc: Store python object inside globally accessible builtin memory
//-----------------------------------------------------------------------------
Result PythonScriptEngine::AddToBuiltins(const String& name, PyObject* pyobject)
{
	try
		{main_namespace["__builtins__"].attr(name) = handle<>(pyobject);}
	catch(boost::python::error_already_set const &)
		{return ERR(String("Error in Python: ") << PyExceptionToString<0>() << '\n');}
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: LoadScript()
// Desc: Load python module from file
//-----------------------------------------------------------------------------
Result PythonScriptEngine::LoadScript(const FilePath& filename, LPPYTHONSCRIPT* script)
{
	Result rlt;

	*script = NULL;

	PythonScript* newscript;
	CHKALLOC(newscript = new PythonScript(this));
	CHKRESULT(newscript->LoadPy(filename));

	scripts.push_back(newscript);
	*script = newscript;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: GetModulFunctions()
// Desc: Get names and or python objects to every function of a given module
//-----------------------------------------------------------------------------
Result PythonScriptEngine::GetModuleFunctions(PyObject* pymodule, int* numfuncs, PyObject*** pyfuncs, String** funcnames) const
{
	PyObject *key, *value;
	Py_ssize_t pos = 0;
	Result rlt;

	// Querry module dictionary
	PyObject* pydict = PyModule_GetDict(pymodule);
	if(!pydict)
		return ERR("Error getting module dictionary");

	// Get number of functions
	*numfuncs = 0;
	while(PyDict_Next(pydict, &pos, &key, &value))
	{
		if(!PyFunction_Check(value))
			continue;
		(*numfuncs)++;
	}
	if(*numfuncs == 0)
		return R_OK;

	// Allocate memory
	if(pyfuncs)
		*pyfuncs = (PyObject**) new LPVOID[*numfuncs];
	if(funcnames)
		*funcnames = new String[*numfuncs];

	// Get functions and names
	int i = 0;
	pos = 0;
	while(PyDict_Next(pydict, &pos, &key, &value))
	{
		if(!PyFunction_Check(value))
			continue;
		if(pyfuncs)
		{
			(*pyfuncs)[i] = value;
			Py_IncRef(value);
		}
		if(funcnames)
			(*funcnames)[i] = (LPSTR)PyUnicode_1BYTE_DATA(key);
		i++;
	}

	return R_OK;
}

void SetModulCapsule(PyObject* pymodule, const char* capsulename, LPVOID capsule)
{
	if(capsule)
	{
		PyObject* pycapsule = PyCapsule_New(capsule, NULL, NULL);
		PyObject_SetAttrString(pymodule, capsulename, pycapsule);
	}
	else
	{
		Py_INCREF(Py_None);
		PyObject_SetAttrString(pymodule, capsulename, Py_None);
	}
}
bool SetModulCapsule(const char* modulename, const char* capsulename, LPVOID capsule)
{
	PyObject* pymodule = PyImport_ImportModule(modulename);
	if(!pymodule)
		return false;

	SetModulCapsule(pymodule, capsulename, capsule);
	Py_DecRef(pymodule);
	return true;
}
void RemoveModulCapsule(PyObject* pymodule, const char* capsulename)
{
	PyObject* pycapsule = PyObject_GetAttrString(pymodule, capsulename);
	if(!pycapsule)
		return;
	Py_DECREF(pycapsule); // Remove reference attained from PyObject_GetAttrString
	Py_DECREF(pycapsule); // Remove reference to trigger object removal
}
void RemoveModulCapsule(const char* modulename, const char* capsulename)
{
	PyObject* pymodule = PyImport_ImportModule(modulename);
	if(!pymodule)
		return;
	RemoveModulCapsule(pymodule, capsulename);
	Py_DecRef(pymodule);
}
void PythonScriptEngine::HookExceptions(EXCEPTIONHOOK_CALLBACK cbk, LPVOID user)
{
	SetModulCapsule("__outputredirect__", "cbk", cbk);
	SetModulCapsule("__outputredirect__", "user", user);
}
void PythonScriptEngine::UnhookExceptions()
{
	RemoveModulCapsule("__outputredirect__", "cbk");
	RemoveModulCapsule("__outputredirect__", "user");
}

//-----------------------------------------------------------------------------
// Name: Throw()
// Desc: Throw a python error resembling rlt
//-----------------------------------------------------------------------------
void PythonScriptEngine::Throw(const Result& rlt) const
{
	if(isexecuting)
	{
		PyErr_SetString(PyExc_Exception, rlt.GetLastResult());
		throw_error_already_set();
	}
}
void PythonScriptEngine::Throw(const Result* rlt) const
{
	if(isexecuting)
	{
		PyErr_SetString(PyExc_Exception, rlt->GetLastResult());
		throw_error_already_set();
	}
}

//-----------------------------------------------------------------------------
// Name: Call()
// Desc: Call a callable python object
//-----------------------------------------------------------------------------
PyObject* PythonScriptEngine::Call(PyObject* pyfunc, const char* format, ...) const //EDIT: Return Result
{
	if(PyCallable_Check(pyfunc))
	{
		va_list valist;
		va_start(valist, format);
		PyObject* pyargs = Py_VaBuildValue(format, valist);
		va_end(valist);
		if(!pyargs)
			return NULL;

		BeginPythonCode();
		PyObject* result = PyObject_CallObject(pyfunc, pyargs == Py_None ? NULL : pyargs);
		EndPythonCode();
		Py_DECREF(pyargs);
		if(!result)
			return NULL;

		if(result == Py_None)
		{
			Py_DECREF(result);
			return NULL;
		}
		else
			return result;
	}

	return NULL;
}
PyObject* PythonScriptEngine::Call(PyObject* pyfunc) const //EDIT: Return Result
{
	if(PyCallable_Check(pyfunc))
	{
		BeginPythonCode();
		PyObject* result = PyObject_CallObject(pyfunc, NULL);
		EndPythonCode();
		if(!result)
			return NULL;

		if(result == Py_None)
		{
			Py_DECREF(result);
			return NULL;
		}
		else
			return result;
	}

	return NULL;
}

void PythonScriptEngine::EndPythonCode() const
{
	isexecuting = false;
	if(PyErr_Occurred())
	{
		PyErr_Print();
		PyErr_Clear();
	}
}

#pragma region Invoke
Result VaListToPyTuple(va_list& valist, const char* typestr, PyObject** pytupple)
{
	Result rlt;

	// Get number of arguments
	int numargs = 0, bracketdepth = 0;
	const char* c = typestr;
	while(*c)
	{
		if(bracketdepth)
		{
			if(*c == '(')
				bracketdepth++;
			else if(*c == ')')
				bracketdepth--;
			// Inside bracket area (ignore type chars)
		}
		else
		{
			if(*c == '(')
				bracketdepth++;
			else if(*c == ')' || *c == '|')
				break;
			numargs++; // Outside bracket area (use type chars)
		}
		c++;
	}
	if(bracketdepth)
		return ERR("Syntax error in type string. Missing ')'");

	// Create argument tuple
	*pytupple = PyTuple_New(numargs);
	if(!*pytupple)
		return ERR("Failed to create tuple");

	// Parse arguments
	for(int i = 0; i < numargs; i++)
	{
		PyObject* pyvalue;
		switch(*typestr++)
		{
		case 'O':
			pyvalue = va_arg(valist, PyObject*);
			Py_INCREF(pyvalue);
			break;
		case 'i': case 'l':
			pyvalue = PyLong_FromLong(va_arg(valist, long));
			break;
		case 's':
			pyvalue = PyUnicode_FromString(va_arg(valist, const char*));
			break;
		case 'f': case 'd': // (Floats are always widened to doubles by the variable argument list)
			pyvalue = PyFloat_FromDouble(va_arg(valist, double));
			break;
		case '(':
			IFFAILED(VaListToPyTuple(valist, typestr, &pyvalue))
			{
				Py_DECREF(*pytupple); *pytupple = NULL;
				return rlt;
			}
			typestr = strchr(typestr, ')') + 1;
			break;
		default:
			Py_DECREF(*pytupple); *pytupple = NULL;
			return ERR(String("Invalid type identifier: ") << String(typestr[i]));
		}
		if(!pyvalue)
		{
			Py_DECREF(*pytupple); *pytupple = NULL;
			return ERR("Value parsing failed");
		}
		PyTuple_SetItem(*pytupple, i, pyvalue);
	}

	return R_OK;
}

Result PyObjectToVaList(PyObject* pyvalue, const char* typestr, va_list& valist)
{
	// Get number of arguments
	int numargs = 0, bracketdepth = 0;
	const char* c = typestr;
	while(*c)
	{
		if(bracketdepth)
		{
			if(*c == '(')
				bracketdepth++;
			else if(*c == ')')
				bracketdepth--;
			// Inside bracket area (ignore type chars)
		}
		else
		{
			if(*c == '(')
				bracketdepth++;
			else if(*c == ')' || *c == '|')
				break;
			numargs++; // Outside bracket area (use type chars)
		}
		c++;
	}
	if(bracketdepth)
		return ERR("Syntax error in type string. Missing ')'");

	// Prepare argument conversion
	PyObject* pysubvalue;
	if(numargs > 1)
	{
		if(!PyTuple_Check(pyvalue))
			return ERR("Invalid type identifier: Value is not of type tuple");
	}
	else
		pysubvalue = pyvalue;

	// Convert arguments
	for(int i = 0; i < numargs; i++)
	{
		if(numargs > 1)
			pysubvalue = PyTuple_GetItem(pyvalue, i);

		switch(typestr[i])
		{
		case 'i': case 'l':
			if(!PyLong_Check(pysubvalue))
			{
				if(numargs > 1)
					Py_DECREF(pyvalue);
				return ERR("Invalid type identifier: Value is not of type long");
			}
			*va_arg(valist, long*) = PyLong_AsLong(pysubvalue);
			break;
		case 's':
			if(!PyUnicode_Check(pysubvalue))
			{
				if(numargs > 1)
					Py_DECREF(pyvalue);
				return ERR("Invalid type identifier: Value is not of type string");
			}
			*va_arg(valist, unsigned char**) = PyUnicode_1BYTE_DATA(pysubvalue);
			break;
		case 'f':
			if(!PyFloat_Check(pysubvalue))
			{
				if(numargs > 1)
					Py_DECREF(pyvalue);
				return ERR("Invalid type identifier: Value is not of type float");
			}
			*va_arg(valist, float*) = (float)PyFloat_AsDouble(pysubvalue);
			if(PyErr_Occurred())
			{
				if(numargs > 1)
					Py_DECREF(pyvalue);
				return ERR("Value parsing failed");
			}
			break;
		case 'd':
			if(!PyFloat_Check(pysubvalue))
			{
				if(numargs > 1)
					Py_DECREF(pyvalue);
				return ERR("Invalid type identifier: Value is not of type float");
			}
			*va_arg(valist, double*) = PyFloat_AsDouble(pysubvalue);
			if(PyErr_Occurred())
			{
				if(numargs > 1)
					Py_DECREF(pyvalue);
				return ERR("Value parsing failed");
			}
			break;
		case 'O':
			*va_arg(valist, PyObject**) = pysubvalue;
			Py_INCREF(pysubvalue);
			break;
		default:
			if(numargs > 1)
				Py_DECREF(pyvalue);
			return ERR(String("Invalid type identifier: ") << String(typestr[i]));
		}
	}

	return R_OK;
}

/*Result PythonScriptEngine::Invoke(const String& funcname, const char* argtypes, ...) const
{
	va_list valist;
	va_start(valist, argtypes);
	Result rlt = InvokeVaList(funcname, argtypes, valist);
	va_end(valist);
	return rlt;
}*/

Result PythonScriptEngine::Invoke(PyObject* pyfunc, const char* argtypes, ...) const
{
	va_list valist;
	va_start(valist, argtypes);
	Result rlt = InvokeVaList(pyfunc, argtypes, valist);
	va_end(valist);
	return rlt;
}

Result PythonScriptEngine::InvokeVaList(PyObject* pyfunc, const char* argtypes, va_list& valist) const
{
	Result rlt;

	// Verify if pyfunc is callable
	if(!PyCallable_Check(pyfunc))
		return ERR(String("Error calling python function: The given python object is not a callable function"));

	// Parse arguments
	PyObject* pyargs;
	if(*argtypes == '\0')
		pyargs = NULL;
	else
		CHKRESULT(VaListToPyTuple(valist, argtypes, &pyargs));

	// Call function
	PyObject* pyresult = PyObject_CallObject(pyfunc, pyargs);
	Py_XDECREF(pyargs);
	if(!pyresult)
	{
		PyErr_Print();
		return ERR("Error calling python function: An error occured inside the function. See output window for a stack trace");
	}

	// Get result type
	const char* resulttypes = strchr(argtypes, '|');

	// Parse return value
	rlt = resulttypes ? PyObjectToVaList(pyresult, resulttypes + 1, valist) : R_OK;
	Py_DECREF(pyresult);
	return rlt;
}

/*Result PythonScriptEngine::InvokeVaList(const String& funcname, const char* argtypes, va_list& valist) const
{
	Result rlt;

	// Query function attribute
	PyObject* pyfunc = PyObject_GetAttrString(pymodule, funcname);
	if(!pyfunc)
		return ERR(String("Error querying python script function: ") << funcname);

	// Invoke pyfunc
	rlt = InvokeVaList(pyfunc, argtypes, valist);
	Py_DECREF(pyfunc);
	return rlt;
}*/
#pragma endregion

/*PythonDictionary* PythonScriptEngine::GetGlobals() const
{
	return new PythonDictionary(((object)main_namespace["__globals__"]).ptr());
}
PythonDictionary* PythonScriptEngine::GetLocals() const
{
	return new PythonDictionary(((object)main_namespace["__locals__"]).ptr());
}*/
PythonDictionary* PythonScriptEngine::GetBuiltins() const
{
	return new PythonDictionary(((object)main_namespace["__builtins__"].attr("__dict__")).ptr());
}

String* IPythonScriptEngine::PyObjectToString(PyObject* pyobject, String* result)
{
	PyObject* pystring = PyObject_Str(pyobject);
	if(!pystring)
	{
		result->Clear();
		return result;
	}
	const char* chars = (char*)PyUnicode_1BYTE_DATA(pystring);
	if(!chars)
	{
		Py_DecRef(pystring);
		result->Clear();
		return result;
	}
	*result = String(chars);
	Py_DecRef(pystring);
	return result;
}

std::shared_ptr<void> PythonScriptEngine::GetHandle(PyObject* pyobject) const
{
	Py_INCREF(pyobject);
	return std::shared_ptr<void>(pyobject, [](LPVOID pyobject){Py_XDECREF((PyObject*)pyobject);});
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void PythonScriptEngine::Release()
{
	while(scripts.size())
		scripts.front()->Release();

	Py_Finalize();

	delete this;
}