#include <ISPythonScriptEngine.h>
#include "boost/python/detail/wrap_python.hpp"


PythonDictionary::PythonDictionary() : ownref(true)
{
	dict = PyDict_New();
if(!dict || !dict->ob_refcnt)
	int abc = 0;
}
PythonDictionary::PythonDictionary(PythonDictionary& other) : dict(other.dict), ownref(false)
{
	Py_INCREF(dict);
if(!dict || !dict->ob_refcnt)
	int abc = 0;
}
PythonDictionary::PythonDictionary(PyObject* dict) : dict(dict), ownref(false)
{
	Py_INCREF(dict);
if(!dict || !dict->ob_refcnt)
	int abc = 0;
}

void PythonDictionary::Insert(PyObject* pyindex, PyObject* pyvalue)
{
	PyDict_SetItem(dict, pyindex, pyvalue);
if(!dict || !dict->ob_refcnt)
	int abc = 0;
}

PyObject* PythonDictionary::Get(PyObject* pyindex)
{
if(!dict || !dict->ob_refcnt)
	int abc = 0;
	PyObject* pyvalue = PyDict_GetItem(dict, pyindex);
	if(pyvalue)
		Py_INCREF(pyvalue);
	return pyvalue;
}

bool PythonDictionary::Next(iterator* i, PyObject** key, PyObject** value)
{
if(!dict || !dict->ob_refcnt)
	int abc = 0;
	return PyDict_Next(dict, i, key, value) != 0;
}

String PythonDictionary::ToString()
{
	String result("{"), keystr, valstr;
	PyObject *key, *val;
	iterator iter = begin();
	if(Next(&iter, &key, &val))
	{
		IPythonScriptEngine::PyObjectToString(key, &keystr);
		IPythonScriptEngine::PyObjectToString(val, &valstr);
		result <<= keystr << String(": ") << valstr;
	}
	while(Next(&iter, &key, &val))
	{
		IPythonScriptEngine::PyObjectToString(key, &keystr);
		IPythonScriptEngine::PyObjectToString(val, &valstr);
		result <<= String(", ") << keystr << String(": ") << valstr;
	}
	result <<= "}";

	return result;
}

PythonDictionary::~PythonDictionary()
{
	if(ownref)
		PyDict_Clear(dict);
	Py_DECREF(dict);
}