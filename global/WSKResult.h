#ifndef __ISWSKRESULT_H
#define __ISWSKRESULT_H

#include <ISTypes.h>


Result Error(int wskr);
Result Error(int wskr, LPCTSTR file, LPCTSTR func, int line);
__forceinline Result CheckResult(int wskr)
	{return Error(wskr);}

#endif