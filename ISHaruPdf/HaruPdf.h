#ifndef __HARUPDF_H
#define __HARUPDF_H

#include "ISHaruPdf.h"

#pragma comment(lib, "libhpdf.lib")
//#pragma comment(lib, "libpng.lib")
//#pragma comment(lib, "zlib.lib")

/*//-----------------------------------------------------------------------------
// Name: Database
// Desc: Representation of an sql database
//-----------------------------------------------------------------------------
class Database : public IDatabase
{
private:
	HaruPdf* parent;
	sqlite3* db;

public:
	std::list<ResultSet*> resultsets;

	Database(HaruPdf* parent) : parent(parent), db(NULL) {}

	Result Init(const FilePath& filename);
	Result Execute(const String& statement);
	Result Query(const String& statement, IResultSet** rset);
	void Release();
};*/

//-----------------------------------------------------------------------------
// Name: HaruPdf
// Desc: API to the HaruPdf3 library
//-----------------------------------------------------------------------------
class HaruPdf : public IHaruPdf
{
public:
	/*std::list<Database*> databases;*/

	HaruPdf()
	{
	};

	void Sync(GLOBALVARDEF_LIST);
	Result Open(const FilePath& filename/*, IDatabase** database*/);

	void Release();
};

#endif