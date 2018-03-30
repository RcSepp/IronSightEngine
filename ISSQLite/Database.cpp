#include "SQLite.h"

#ifdef UNICODE
#define _sqlite3_prepare sqlite3_prepare16_v2
#define _sqlite3_open(filename, db) sqlite3_open16(filename, db)
#else
#define _sqlite3_prepare sqlite3_prepare_v2
#define _sqlite3_open(filename, db) sqlite3_open_v2(filename, db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)
#endif

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Create an instance of the Database class
//-----------------------------------------------------------------------------
Result Database::Init(const FilePath& filename)
{
	Result rlt;

	if(_sqlite3_open(filename, &db) != SQLITE_OK)
	{
		sqlite3_close(db);
		return ERR(String("Can't open database \"") << filename << String("\": ") << String(sqlite3_errmsg(db)));
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Execute()
// Desc: Execute a SQL statement on the given database without parsing results
//-----------------------------------------------------------------------------
/*Result Database::Execute(const String& statement)
{
	sqlite3_stmt* stmt;
	Result rlt;

	// Compile SQL statement string into SQLite prepared statement
	if(_sqlite3_prepare(db, statement.ToTCharString(), (statement.length() + 1) * sizeof(TCHAR), &stmt, NULL) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return ERR(String("Error executing SQL statement: ") << statement);
	}

	switch(sqlite3_step(stmt))
	{
	case SQLITE_BUSY:
		rlt = ERR("Error executing SQL statement: The database engine is BUSY");
		break;

	case SQLITE_ROW:
		LOG("Execute called on a SQL statement with results");
	case SQLITE_DONE:
		rlt = R_OK;
		break;

	default:
		rlt = ERR(String("Error executing SQL statement: ") << String(sqlite3_errmsg(db)));
		break;
	}

	// Release prepared statement
	sqlite3_finalize(stmt);

	return rlt;
}*/

//-----------------------------------------------------------------------------
// Name: Execute()
// Desc: Execute a SQL statement on the given database without parsing results
//		 Use parameters like with printf()
//		 Parameters are:
//		 ?b ... void* blob, size_t blobsize
//		 ?f ... float* num
//		 ?d ... double* num
//		 ?i ... int* num
//		 ?I ... int64* num
//		 ?n ... NULL (no parameters)
//		 ?s ... char* string
//		 ?u ... WCHAR* unicode_string
//		 ?z ... size_t blobsize (blob of zeros)
//-----------------------------------------------------------------------------
Result Database::Execute(const String statement, ...)
{
	sqlite3_stmt* stmt;
	Result rlt;

	// Extract type metadata of parameters to bind
	TCHAR* statementstr = statement.CopyChars();
	bool insinglequotes = false, indoublequotes = false;
	std::vector<TCHAR> paramtypes;
	for(TCHAR *c = statementstr; *c != '\0'; c++)
	{
		switch(*c)
		{
		case '\'': insinglequotes = !insinglequotes; break;
		case '\"': indoublequotes = !indoublequotes; break;
		case '?':
			if(insinglequotes || indoublequotes)
				break;
			c++;
			switch(*c)
			{
			case 'b': case 'f': case 'd': case 'i': case 'I': case 'n': case 's': case 'u': case 'z':
				paramtypes.push_back(*c);
				for(TCHAR *cc = c; *cc != '\0'; cc++)
					cc[0] = cc[1];
				break;
			case '\0':
				delete[] statementstr;
				return ERR("Missing type specifier after '?'");
			default:
				delete[] statementstr;
				return ERR(String("Unknown type specifier: ") << *c);
			}
		}
	}

	// Compile SQL statement string into SQLite prepared statement
	if(_sqlite3_prepare(db, statementstr, (_tcslen(statementstr) + 1) * sizeof(TCHAR), &stmt, NULL) != SQLITE_OK)
	{
		delete[] statementstr;
		sqlite3_finalize(stmt);
		return ERR(String("Error executing SQL statement: ") << statement);
	}
	delete[] statementstr;

	// Bind values to prepared statement
	va_list valist;
	va_start(valist, statement);
	for(size_t i = 0; i < paramtypes.size(); i++)
	{
		BYTE* blob = new BYTE[5];
		switch(paramtypes[i])
		{
		case 'b':
			{
				LPVOID blob = va_arg(valist, LPVOID);
				size_t size = va_arg(valist, size_t);
				sqlite3_bind_blob(stmt, i + 1, blob, size, SQLITE_STATIC);
				break;
			}
		case 'f': sqlite3_bind_double(stmt, i + 1, va_arg(valist, float)); break;
		case 'd': sqlite3_bind_double(stmt, i + 1, va_arg(valist, double)); break;
		case 'i': sqlite3_bind_int(stmt, i + 1, va_arg(valist, int)); break;
		case 'I': sqlite3_bind_int64(stmt, i + 1, va_arg(valist, sqlite3_int64)); break;
		case 'n': sqlite3_bind_null(stmt, i + 1); break;
		case 's': sqlite3_bind_text(stmt, i + 1, va_arg(valist, char*), -1, SQLITE_STATIC); break;
		case 'u': sqlite3_bind_text16(stmt, i + 1, va_arg(valist, WCHAR*), -1, SQLITE_STATIC); break;
		case 'z': sqlite3_bind_zeroblob(stmt, i + 1, va_arg(valist, size_t)); break;
		}
	}
	va_end(valist);

	switch(sqlite3_step(stmt))
	{
	case SQLITE_BUSY:
		rlt = ERR("Error executing SQL statement: The database engine is BUSY");
		break;

	case SQLITE_ROW:
		LOG("Execute called on a SQL statement with results");
	case SQLITE_DONE:
		rlt = R_OK;
		break;

	default:
		rlt = ERR(String("Error executing SQL statement: ") << String(sqlite3_errmsg(db)));
		break;
	}

	// Release prepared statement
	sqlite3_finalize(stmt);

	return rlt;
}

//-----------------------------------------------------------------------------
// Name: Query()
// Desc: Prepare a SQL statement for later result traversal
//-----------------------------------------------------------------------------
Result Database::Query(const String& statement, IResultSet** rset)
{
	sqlite3_stmt* stmt;
	Result rlt;

	*rset = NULL;

	// Compile SQL statement string into SQLite prepared statement
	if(_sqlite3_prepare(db, statement.ToTCharString(), (statement.length() + 1) * sizeof(TCHAR), &stmt, NULL) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return ERR(String("Error executing SQL statement: ") << statement);
	}

	ResultSet* newresultset;
	CHKALLOC(newresultset = new ResultSet(this, stmt));

	resultsets.push_back(newresultset);
	*rset = newresultset;

	return rlt;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Database::Release()
{
	parent->databases.remove(this);

	while(resultsets.size())
		resultsets.front()->Release();

	// Close database
	sqlite3_close(db);

	delete this;
}


// >>> ResultSet


//-----------------------------------------------------------------------------
// Name: Step()
// Desc: Get the next row of results. Return false if there is none
//-----------------------------------------------------------------------------
bool ResultSet::Step()
{
	switch(sqlite3_step(stmt))
	{
	case SQLITE_BUSY:
		LOG("Error executing SQL statement: The database engine is BUSY");
		return false;

	case SQLITE_ROW:
		return true;
	case SQLITE_DONE:
		return false;

	default:
		LOG(String("Error executing SQL statement: ") << String(sqlite3_errmsg(db)));
		return false;
	}
}

//-----------------------------------------------------------------------------
// Name: GetXXX()
// Desc: Get the value of the given column
//-----------------------------------------------------------------------------
int ResultSet::GetInt(int column)
{
	return sqlite3_column_int(stmt, column);
}
__int64 ResultSet::GetInt64(int column)
{
	return sqlite3_column_int64(stmt, column);
}
String ResultSet::GetText(int column)
{
	return String((LPSTR)sqlite3_column_text(stmt, column));
}
String ResultSet::GetUnicode(int column)
{
	return String((LPWSTR)sqlite3_column_text16(stmt, column));
}
double ResultSet::GetDouble(int column)
{
	return sqlite3_column_double(stmt, column);
}
float ResultSet::GetFloat(int column)
{
	return (float)sqlite3_column_double(stmt, column);
}
const void* ResultSet::GetBlob(int column)
{
	return sqlite3_column_blob(stmt, column);
}
bool ResultSet::GetBool(int column)
{
	return (sqlite3_column_int(stmt, column) != 0);
}
int ResultSet::GetSize(int column)
{
	return sqlite3_column_bytes(stmt, column);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void ResultSet::Release()
{
	parent->resultsets.remove(this);

	// Release prepared statement
	sqlite3_finalize(stmt);

	delete this;
}