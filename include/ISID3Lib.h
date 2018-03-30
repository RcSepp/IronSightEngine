#ifndef __ISID3LIB_H
#define __ISID3LIB_H

#include <ISEngine.h>
// ID3Lib headers
#include "id3\\tag.h"


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_ID3LIB
	#define ID3LIB_EXTERN_FUNC		__declspec(dllexport)
#else
	#define ID3LIB_EXTERN_FUNC		__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// Name: IID3Tag
// Desc: Interface to the ID3Tag class
//-----------------------------------------------------------------------------
typedef class IID3Tag
{
public:

	virtual Result Load(FilePath& path) = 0;
	virtual const Mp3_Headerinfo* ReadHeader() const = 0;
	virtual void Release() = 0;
}* LPID3TAG;

//-----------------------------------------------------------------------------
// Name: IID3Lib
// Desc: Interface to the ID3Lib class
//-----------------------------------------------------------------------------
typedef class IID3Lib
{
public:

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init() = 0;
	virtual Result LoadTagFromFile(FilePath& path, IID3Tag** tag) = 0;
	virtual void Release() = 0;
}* LPID3LIB;

extern "C" ID3LIB_EXTERN_FUNC LPID3LIB __cdecl CreateID3Lib();
extern "C" ID3LIB_EXTERN_FUNC bool __cdecl CheckID3LibSupport(LPTSTR* missingdllname);

#endif