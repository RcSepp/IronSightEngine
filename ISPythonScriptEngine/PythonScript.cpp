#include "PythonScriptEngine.h"

std::map<FilePath, PyObject*> PythonScript::loadedmodules;

Result PythonScript::LoadPy(const FilePath& filename)
{
	// Close old file
	Close();

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

	std::map<FilePath, PyObject*>::const_iterator pair = loadedmodules.find(fname);
	if(pair == loadedmodules.end())
	{
/*// Copy all data from main's global namespace to builtins, so it can be used inside the module
object main_module((handle<>(borrowed(PyImport_AddModule("__main__")))));
object main_namespace = main_module.attr("__dict__");
object main_builtins = main_namespace["__builtins__"];

Py_ssize_t pos = 0;
PyObject *key, *value;
PyObject* pybuiltins = main_builtins.ptr();
PyObject* pyglobals = main_namespace.ptr();
while(PyDict_Next(pyglobals, &pos, &key, &value))
	if(PyUnicode_Check(key))
		PyModule_AddObject(pybuiltins, (const char*)PyUnicode_1BYTE_DATA(key), value);*/

		// Load module
		parent->BeginPythonCode();
		pymodule = PyImport_ImportModule(fname);
		parent->EndPythonCode();

		// Register loaded module
		loadedmodules[fname] = pymodule;
	}
	else
	{
		// Reload module
		parent->BeginPythonCode();
		pymodule = PyImport_ReloadModule(pymodule = pair->second);
		parent->EndPythonCode();
		//if(pymodule)
		//	Py_DECREF(pymodule);
	}

	// Print errors
	if(!pymodule)
	{
		PyErr_Print();
		return ERR(String("Error loading python script file: ") << filename);
	}

	// Save module name
	modulename = fname;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Close()
// Desc: Remove reference to pymodule
//-----------------------------------------------------------------------------
void PythonScript::Close()
{
	if(pymodule)
	{
		Py_DECREF(pymodule);
		pymodule = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void PythonScript::Release()
{
	parent->scripts.remove(this);

	Close();

	delete this;
}