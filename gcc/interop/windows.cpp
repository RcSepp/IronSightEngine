#include "windows.h"

BOOL SetRect(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom)
{
	lprc->left = xLeft;
	lprc->top = yTop;
	lprc->right = xRight;
	lprc->bottom = yBottom;
}

void OutputDebugStringA(LPCSTR str)
{
	printf(str);
}
