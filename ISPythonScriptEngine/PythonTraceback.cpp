#include <ISPythonScriptEngine.h>
#include "boost/python/detail/wrap_python.hpp"
#include <frameobject.h> // Needed for PyFrameObject used by Py_tracefunc


PythonTraceback::PythonTraceback(PythonTraceback& other) : traceback(other.traceback), isretreived(other.isretreived)
{
	Py_INCREF(traceback);
}
PythonTraceback::PythonTraceback(PyTracebackObject* traceback) : traceback(traceback), isretreived(false)
{
	Py_INCREF(traceback);
}

int PythonTraceback::GetLineNumber()
{
	if(!traceback)
		return -1;
	return traceback->tb_lineno;
}
String PythonTraceback::GetFilename()
{
	if(!traceback)
		return "";
	if(!isretreived)
	{
		PyFrame_FastToLocals(traceback->tb_frame);
		isretreived = true;
	}
	return String((char*)PyUnicode_1BYTE_DATA(traceback->tb_frame->f_code->co_filename));
}
PythonDictionary PythonTraceback::GetLocals()
{
	if(!traceback)
		return PythonDictionary();
	if(!isretreived)
	{
		PyFrame_FastToLocals(traceback->tb_frame);
		isretreived = true;
	}
	return PythonDictionary(traceback->tb_frame->f_locals);
}
PythonDictionary PythonTraceback::GetGlobals()
{
	if(!traceback)
		return PythonDictionary();
	if(!isretreived)
	{
		PyFrame_FastToLocals(traceback->tb_frame);
		isretreived = true;
	}
	return PythonDictionary(traceback->tb_frame->f_globals);
}
PythonDictionary PythonTraceback::GetBuiltins()
{
	if(!traceback)
		return PythonDictionary();
	if(!isretreived)
	{
		PyFrame_FastToLocals(traceback->tb_frame);
		isretreived = true;
	}
	return PythonDictionary(traceback->tb_frame->f_builtins);
}
bool PythonTraceback::HasNext()
{
	return traceback->tb_next != NULL;
}
PythonTraceback PythonTraceback::Next()
{
	if(!traceback)
		return PythonTraceback();
	if(traceback->tb_next)
		return PythonTraceback(traceback->tb_next);
	else
		return PythonTraceback();
}

PythonTraceback::~PythonTraceback()
{
	Py_DECREF(traceback);
}