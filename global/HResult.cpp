#include "..\\global\\HResult.h"

/* // Old version (requires DxErr.lib)

Result Error(HRESULT hr)
{
	return FAILED(hr) ? Result((LPCTSTR)DXGetErrorDescription(hr)) : R_OK;
}
Result Error(HRESULT hr, LPCTSTR file, LPCTSTR func, int line)
{
	return FAILED(hr) ? Result((LPCTSTR)DXGetErrorDescription(hr), file, func, line) : R_OK;
}*/

Result Error(HRESULT hr)
{
	if(FAILED(hr))
	{
		LPTSTR buffer;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)), (LPTSTR)&buffer, 0, NULL);
		Result rlt(buffer);
		//delete[] buffer;
		return rlt;
	}
	else
		return R_OK;
}
Result Error(HRESULT hr, LPCTSTR file, LPCTSTR func, int line)
{
	if(FAILED(hr))
	{
		LPTSTR buffer;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)), (LPTSTR)&buffer, 0, NULL);
		Result rlt(buffer, file, func, line);
		//delete[] buffer;
		return rlt;
	}
	else
		return R_OK;
}