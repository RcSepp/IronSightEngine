#ifndef __SQLITE_H
#define __SQLITE_H

#include "ISSQLite.h"
#include "sqlite3.h"

#pragma comment(lib, "sqlite3.lib")

#pragma comment(lib, "DelayImp.lib")


class SQLite;
class Database;

//-----------------------------------------------------------------------------
// Name: ResultSet
// Desc: Representation of a prepared sql statement for result traversal
//-----------------------------------------------------------------------------
class ResultSet : public IResultSet
{
private:
	Database* parent;
	sqlite3* db;
	sqlite3_stmt* stmt;

public:
	ResultSet(Database* parent, sqlite3_stmt* stmt) : parent(parent), db(NULL), stmt(stmt) {}

	bool Step();
	int GetInt(int column);
	__int64 GetInt64(int column);
	String GetText(int column);
	String GetUnicode(int column);
	double GetDouble(int column);
	float GetFloat(int column);
	const void* GetBlob(int column);
	bool GetBool(int column);
	int GetSize(int column);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: Database
// Desc: Representation of an sql database
//-----------------------------------------------------------------------------
class Database : public IDatabase
{
private:
	SQLite* parent;
	sqlite3* db;

public:
	std::list<ResultSet*> resultsets;

	Database(SQLite* parent) : parent(parent), db(NULL) {}

	Result Init(const FilePath& filename);
	Result Execute(const String statement, ...);
	Result Query(const String& statement, IResultSet** rset);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: SQLite
// Desc: API to the SQLite3 library
//-----------------------------------------------------------------------------
class SQLite : public ISQLite
{
public:
	std::list<Database*> databases;

	SQLite()
	{
	};

	void Sync(GLOBALVARDEF_LIST);
	Result Open(const FilePath& filename, IDatabase** database);

	void Release();
};

#endif