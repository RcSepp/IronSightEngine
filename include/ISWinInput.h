#ifndef __ISWININPUT_H
#define __ISWININPUT_H

#include <ISEngine.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_WININPUT
	#define WININPUT_EXTERN_FUNC	__declspec(dllexport)
#else
	#define WININPUT_EXTERN_FUNC	__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// Name: IWinIpt
// Desc: Interface to the WinInput class
//-----------------------------------------------------------------------------
typedef class IWinIpt
{
public:

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init(IForms* forms) = 0;
	virtual void EnableKeyboard() = 0;
	virtual void DisableKeyboard() = 0;
	virtual void Update() = 0;
	virtual void Release() = 0;
}* LPWINIPT;

WININPUT_EXTERN_FUNC LPWINIPT __cdecl CreateWinInput();

#endif