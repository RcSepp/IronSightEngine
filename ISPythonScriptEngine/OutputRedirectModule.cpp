#include "PythonScriptEngine.h"
#include <frameobject.h> // Needed for PyFrameObject used by Py_tracefunc

PyObject* OutputRedirectModule_OnOutput(PyObject *self, PyObject *args)
{
	const char* str;
	if(!PyArg_ParseTuple(args, "s", &str))
		return NULL;
	cle->Print(str);
	Py_RETURN_NONE;
}

PyObject* OutputRedirectModule_ExceptHook(PyObject *self, PyObject *args)
{
	PyObject* pycbkcapsule = PyObject_GetAttrString(self, "cbk");
	PyObject* pyusercapsule = PyObject_GetAttrString(self, "user");
	if(pycbkcapsule && pyusercapsule)
	{
		EXCEPTIONHOOK_CALLBACK cbk = (EXCEPTIONHOOK_CALLBACK)PyCapsule_GetPointer(pycbkcapsule, NULL);
		LPVOID user = PyCapsule_GetPointer(pyusercapsule, NULL);

		PyObject *pytype, *pyvalue;
		PyTracebackObject* pytraceback;
		if(!PyArg_UnpackTuple(args, "ref", 3, 3, &pytype, &pyvalue, &pytraceback))
			return NULL;
		PyObject* pyvaluestr = PyObject_Str(pyvalue);
		if(PyTraceBack_Check(pytraceback))
			cbk(pyvalue->ob_type->tp_name, (char*)PyUnicode_1BYTE_DATA(pyvaluestr), PythonTraceback(pytraceback), user);
		else
cbk(pyvalue->ob_type->tp_name, (char*)PyUnicode_1BYTE_DATA(pyvaluestr), PythonTraceback(), user); //EDIT
		Py_DECREF(pyvaluestr);
	}
	Py_XDECREF(pycbkcapsule);
	Py_XDECREF(pyusercapsule);

	Py_RETURN_NONE;
}