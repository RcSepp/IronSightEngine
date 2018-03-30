#ifndef __ISHRESULT_H
#define __ISHRESULT_H

#include <ISTypes.h>

/* // Old version
#include <DxErr.h>
#pragma comment (lib, "DxErr.lib")*/

// New version
#include <winerror.h>

Result Error(HRESULT hr);
Result Error(HRESULT hr, LPCTSTR file, LPCTSTR func, int line);
__forceinline Result CheckResult(HRESULT hr)
	{return Error(hr);}

#endif