#ifndef __ISMMRESULT_H
#define __ISMMRESULT_H

#include <ISTypes.h>


Result Error(MMRESULT mmr);
Result Error(MMRESULT mmr, LPCTSTR file, LPCTSTR func, int line);
__forceinline Result CheckResult(MMRESULT mmr)
	{return Error(mmr);}

#endif