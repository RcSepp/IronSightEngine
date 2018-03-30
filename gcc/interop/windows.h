#ifndef __WINDOWS_H
#define __WINDOWS_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>

#define __forceinline inline
#define __cdecl
#define __stdcall
#define CALLBACK __stdcall
#define CONST const

#define __declspec(x) // __declspec(dllexport) isn't needed on Unix

typedef unsigned char byte, BYTE, BOOLEAN;

typedef char CHAR, CCHAR;
typedef char* LPSTR;
typedef const char* LPCSTR;

typedef wchar_t WCHAR;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;

typedef unsigned int UINT;

typedef int BOOL;

typedef unsigned long ULONG, DWORD;

typedef long LONG;

typedef unsigned short WORD;

#define _X64
#ifdef _X64
typedef long long LONG_PTR;
typedef unsigned long long ULONG_PTR;
typedef long long LONGLONG; 
#else
typedef long LONG_PTR;
typedef unsigned long ULONG_PTR;
typedef double LONGLONG;
#endif
#define _W64
typedef LONG_PTR LRESULT;
typedef void *LPVOID, *HWND, *HINSTANCE;
typedef UINT* UINT_PTR;
typedef ULONG_PTR DWORD_PTR;


typedef union _LARGE_INTEGER
{
	struct
	{
		DWORD LowPart;
		LONG  HighPart;
	};
	struct
	{
		DWORD LowPart;
		LONG  HighPart;
	} u;
	LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef struct _RECT
{
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT, *PRECT, *LPRECT;
BOOL SetRect(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom);

void OutputDebugStringA(LPCSTR str);
#define OutputDebugString OutputDebugStringA

inline void strcpy_s(char* dest, size_t size, const char* src) {strcpy(dest, src);}
inline void strncpy_s(char* dest, size_t size, const char* src, size_t maxcount) {strncpy(dest, src, maxcount);}
inline void strcat_s(char* dest, size_t size, const char* src) {strcat(dest, src);}
inline void itoa(int value, char* dest, int radix) {sprintf(dest, "%d", value);/*EDIT: Different bases not supported!*/}
inline void _itoa_s(int value, char* dest, int radix) {sprintf(dest, "%d", value);/*EDIT: Different bases not supported!*/}
inline void _itoa_s(int value, char* dest, size_t size, int radix) {snprintf(dest, size, "%d", value);/*EDIT: Different bases not supported!*/}
inline void _gcvt_s(char* dest, double value, int numdigits) {gcvt(value, numdigits, dest);}
inline void _gcvt_s(char* dest, size_t size, double value, int numdigits) {gcvt(value, numdigits, dest);}

#ifndef max
//#define max(a, b) (((a) > (b)) ? (a) : (b))
template<class T> T max(T a, T b) {return a > b ? a : b;}
#endif
#ifndef min
//#define min(a, b) (((a) < (b)) ? (a) : (b))
template<class T> T min(T a, T b) {return a < b ? a : b;}
#endif


#define MK_CONTROL	0x0008
#define MK_LBUTTON	0x0001
#define MK_MBUTTON	0x0010
#define MK_RBUTTON	0x0002
#define MK_SHIFT	0x0004
#define MK_XBUTTON1	0x0020
#define MK_XBUTTON2	0x0040
typedef unsigned int WPARAM, LPARAM;
#define LOWORD(a)						((WORD)(a))
#define HIWORD(a)						((WORD)(((DWORD)(a) >> 16) & 0xFFFF))
#define GET_X_LPARAM(lp)				((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)				((int)(short)HIWORD(lp))
#define GET_WHEEL_DELTA_WPARAM(wParam)	((short)HIWORD(wParam)) 

#endif
