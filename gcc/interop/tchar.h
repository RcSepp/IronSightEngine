#ifndef __TCHAR_H
#define __TCHAR_H

#include "windows.h"

typedef CHAR TCHAR;
typedef LPSTR LPTSTR;
typedef LPCSTR LPCTSTR;

#define _tcsrchr strrchr
#define _tcscpy_s strcpy_s
#define _tcsncpy_s strncpy_s
#define _tcscat_s strcat_s
#define _tcsclen strlen
#define _tcsncmp strncmp
#define _itot_s _itoa_s
#define _tstoi atoi
#define _tstof atof

#define MultiByteToWideChar(a, b, c, d, e, f)
#define WideCharToMultiByte(a, b, c, d, e, f, g, h)
#define CP_ACP

#endif
