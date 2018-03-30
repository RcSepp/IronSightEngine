#ifndef PYHANDLE_DEFINED
#define PYHANDLE_DEFINED

struct _object;
#ifndef PyObject
typedef _object PyObject;
#endif

//-----------------------------------------------------------------------------
// Name: PyHandle
// Desc: Interface to the PyHandle_ class
//-----------------------------------------------------------------------------
class PyHandle
{
public:
	//virtual PyHandle& operator=(PyHandle const& other) = 0;
	virtual operator PyObject*() const {return NULL;}
};

#endif PYHANDLE_DEFINED