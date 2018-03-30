#include "ID3Lib.h"

//-----------------------------------------------------------------------------
// Name: CreateID3Lib()
// Desc: DLL API for creating an instance of ID3Lib
//-----------------------------------------------------------------------------
__declspec(dllexport) LPID3LIB __cdecl CreateID3Lib()
{
	return CheckID3LibSupport(NULL) ? new ID3Lib() : NULL;
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void ID3Lib::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: CheckID3LibSupport()
// Desc: DLL API for checking support for ID3Lib on the target system
//-----------------------------------------------------------------------------
#ifdef EXPORT_ID3LIB
	__declspec(dllexport) bool __cdecl CheckID3LibSupport(LPTSTR* missingdllname)
#else
	__declspec(dllimport) bool __cdecl CheckID3LibSupport(LPTSTR* missingdllname)
#endif
{
	if(!Engine::FindDLL("id3lib.dll", missingdllname))
		return false;

   return true;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize ID3Lib
//-----------------------------------------------------------------------------
Result ID3Lib::Init()
{
	//EDIT

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: LoadTag()
// Desc: Load an ID3 tag from its containing media file
//-----------------------------------------------------------------------------
Result ID3Lib::LoadTagFromFile(FilePath& path, IID3Tag** tag)
{
	Result rlt;

	ID3Tag* newtag;
	CHKALLOC(newtag = new ID3Tag());
	CHKRESULT(newtag->Load(path));

	tags.push_back(newtag);
	*tag = newtag;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void ID3Lib::Release()
{
	for(std::list<ID3Tag*>::iterator iter = tags.begin(); iter != tags.end(); iter++)
		(*iter)->Release();
	tags.clear();

	delete this;
}