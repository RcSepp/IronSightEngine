#include "..\\global\\HKResult.h"

Result Error(hkResult hkr)
{
	return hkr == HK_FAILURE ? Result("Havok Engine: An undetermined error occured") : R_OK;
}
Result Error(hkResult hkr, LPCTSTR file, LPCTSTR func, int line)
{
	return hkr == HK_FAILURE ? Result("Havok Engine: An undetermined error occured", file, func, line) : R_OK;
}