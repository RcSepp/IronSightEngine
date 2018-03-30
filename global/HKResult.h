#ifndef __ISHKRESULT_H
#define __ISHKRESULT_H

#include <ISTypes.h>
#include <Common/Base/Types/hkBaseTypes.h>

#pragma comment (lib, "DxErr.lib")

Result Error(hkResult hkr);
Result Error(hkResult hkr, LPCTSTR file, LPCTSTR func, int line);
__forceinline Result CheckResult(hkResult hkr)
	{return Error(hkr);}

#endif