#ifndef __ISSQLITE_H
#define __ISSQLITE_H

#include <ISEngine.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_SQLITE
	#define SQLITE_EXTERN_FUNC		__declspec(dllexport)
#else
	#define SQLITE_EXTERN_FUNC		__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: IResultSet
// Desc: Interface to the ResultSet class
//-----------------------------------------------------------------------------
typedef class IResultSet
{
public:
	virtual bool Step() = 0;
	virtual int GetInt(int column) = 0;
	virtual __int64 GetInt64(int column) = 0;
	virtual String GetText(int column) = 0;
	virtual String GetUnicode(int column) = 0;
	virtual double GetDouble(int column) = 0;
	virtual float GetFloat(int column) = 0;
	virtual const void* GetBlob(int column) = 0;
	virtual bool GetBool(int column) = 0;
	virtual int GetSize(int column) = 0;
	virtual void Release() = 0;
}* LPRESULTSET;

//-----------------------------------------------------------------------------
// Name: IDatabase
// Desc: Interface to the Database class
//-----------------------------------------------------------------------------
typedef class IDatabase
{
public:
	virtual Result Execute(const String statement, ...) = 0;
	virtual Result Query(const String& statement, IResultSet** rset) = 0;
	virtual void Release() = 0;
}* LPDATABASE;

//-----------------------------------------------------------------------------
// Name: ISQLite
// Desc: Interface to the SQLite class
//-----------------------------------------------------------------------------
typedef class ISQLite
{
public:

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Open(const FilePath& filename, IDatabase** database) = 0;

	virtual void Release() = 0;
}* LPSQLITE;

extern "C" SQLITE_EXTERN_FUNC LPSQLITE __cdecl CreateSQLite();
extern "C" SQLITE_EXTERN_FUNC bool __cdecl CheckSQLiteSupport(LPTSTR* missingdllname);

#endif