#include "SQLite.h"

//-----------------------------------------------------------------------------
// Name: CreateSQLite()
// Desc: DLL API for creating an instance of SQLite
//-----------------------------------------------------------------------------
SQLITE_EXTERN_FUNC LPSQLITE __cdecl CreateSQLite()
{
	return CheckSQLiteSupport(NULL) ? new SQLite() : NULL;
}

//-----------------------------------------------------------------------------
// Name: CheckSQLiteSupport()
// Desc: DLL API for checking support for SQLite on the target system
//-----------------------------------------------------------------------------
SQLITE_EXTERN_FUNC bool __cdecl CheckSQLiteSupport(LPTSTR* missingdllname)
{
	if(!Engine::FindDLL("sqlite3.dll", missingdllname))
		return false;

   return true;
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void SQLite::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: Open()
// Desc: Create an instance of the Database class
//-----------------------------------------------------------------------------
Result SQLite::Open(const FilePath& filename, IDatabase** database)
{
	Result rlt;

	*database = NULL;

	Database* newdatabase;
	CHKALLOC(newdatabase = new Database(this));
	IFFAILED(newdatabase->Init(filename))
	{
		delete newdatabase;
		return rlt;
	}

	databases.push_back(newdatabase);
	*database = newdatabase;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void SQLite::Release()
{
	while(databases.size())
		databases.front()->Release();

	delete this;
}