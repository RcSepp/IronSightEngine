#include "..\\global\\WSKResult.h"

Result Error(int wskr)
{
	if(wskr == ERROR_SUCCESS)
		return R_OK;

	if(wskr == SOCKET_ERROR)
		wskr = WSAGetLastError();

	TCHAR err[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, wskr, 0, err, 256, NULL);
	return Result(String("WinSock: ") << String(err));
}
Result Error(int wskr, LPCTSTR file, LPCTSTR func, int line)
{
	if(wskr == ERROR_SUCCESS)
		return R_OK;

	if(wskr == SOCKET_ERROR)
		wskr = WSAGetLastError();

	TCHAR err[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, wskr, 0, err, 256, NULL);
	return Result(String("WinSock: ") << String(err), file, func, line);
}