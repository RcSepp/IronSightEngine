#ifndef __ISHARUPDF_H
#define __ISHARUPDF_H

#include <ISEngine.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_HARUPDF
	#define HARUPDF_EXTERN_FUNC		__declspec(dllexport)
#else
	#define HARUPDF_EXTERN_FUNC		__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: IHaruPdf
// Desc: Interface to the HaruPdf class
//-----------------------------------------------------------------------------
typedef class IHaruPdf
{
public:

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Open(const FilePath& filename/*, IDatabase** database*/) = 0;

	virtual void Release() = 0;
}* LPHARUPDF;

extern "C" HARUPDF_EXTERN_FUNC LPHARUPDF __cdecl CreateHaruPdf();
extern "C" HARUPDF_EXTERN_FUNC bool __cdecl CheckHaruPdfSupport(LPTSTR* missingdllname);

#endif