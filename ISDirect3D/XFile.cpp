#include "Direct3D.h"
#include "Mesh.h"
#include "XFile.h"
#include <fstream>

#define ReadVar(pvar, len)			{fs->read((char*)pvar, len); if(fs->eof()) return ERR("Unexpected end of file reached");}

struct Template;
struct PlainData
{
	union PlainDataPtr
		{String* s; UINT* i; float* f; double* d; UUID* u;} ptr;
	enum PlainDataType
		{S, I, F, D, U} type;
	UINT len;

	PlainData() {ptr.i = NULL;}
	~PlainData() {FreeData();}
	String* SetData(String* s, UINT len) {FreeData(); type = S; this->len = len; return ptr.s = s;}
	UINT* SetData(UINT* i, UINT len) {FreeData(); type = I; this->len = len; return ptr.i = i;}
	float* SetData(float* f, UINT len) {FreeData(); type = F; this->len = len; return ptr.f = f;}
	double* SetData(double* d, UINT len) {FreeData(); type = D; this->len = len; return ptr.d = d;}
	UUID* SetData(UUID* u, UINT len) {FreeData(); type = U; this->len = len; return ptr.u = u;}
	void FreeData()
	{
		if(ptr.i)
		{
			if(len == 1)
				switch(type)
				{
				case S: delete ptr.s; break;
				case I: delete ptr.i; break;
				case F: delete ptr.f; break;
				case D: delete ptr.d; break;
				case U: delete ptr.u;
				}
			else
				switch(type)
				{
				case S: delete[] ptr.s; break;
				case I: delete[] ptr.i; break;
				case F: delete[] ptr.f; break;
				case D: delete[] ptr.d; break;
				case U: delete[] ptr.u;
				}
		}
		ptr.i = NULL;
	}
};

/*enum ArraySizeUsage
	{ASU_CONSTANT, ASU_INDEX};
struct ArrayDimension
{
	ArraySizeUsage arraysize_usage;
	UINT arraysize;

	ArrayDimension(ArraySizeUsage arraysize_usage, UINT arraysize)
	{
		this->arraysize_usage = arraysize_usage;
		this->arraysize = arraysize;
	}
};


enum TokenUsage
	{TU_IGNORE, TU_ARRAYSIZE, TU_DATA};

struct TokenAndUsage
{
	WORD token;
	TokenUsage usage;

	TokenAndUsage(WORD token, TokenUsage usage) : token(token), usage(usage) {}
};

struct XFileTemplateClass
{
};

const LPSTR XFILEVECTORNAME = "Vector";
static TokenAndUsage VECTOR_TOKENLIST[] = {TokenAndUsage(TOKEN_FLOAT, TU_DATA)};
struct XFileVector : public XFileTemplateClass
{
	TokenAndUsage* tokenlist;
	float x, y, z;

	XFileVector() : tokenlist(VECTOR_TOKENLIST) {}
};

const LPSTR XFILEMESHFACENAME = "MeshFace";
struct XFileMeshFace : public XFileTemplateClass
{
	UINT nFaceVertexIndices;
	UINT* faceVertexIndices;
};

const LPSTR XFILEMESHNAME = "Mesh";
struct XFileMesh : public XFileTemplateClass
{
	UINT nVertices;
	XFileVector* vertices;
	UINT nFaces;
	XFileMeshFace* faces;
};*/

Result ParseToken(std::ifstream* fs, WORD* token, PlainData& tokendata);
Result LoadMesh(std::ifstream* fs, MeshData *mdata, IObject::AttributeRange** attrs, XMaterial** mats, UINT* nummats);
Result SkipSection(std::ifstream* fs)
{
	PlainData tokendata;
	WORD token;
	int bracedepth = 0;
	Result rlt;

	do
	{
		CHKRESULT(ParseToken(fs, &token, tokendata));
		switch(token)
		{
		case TOKEN_OBRACE: bracedepth++; break;
		case TOKEN_CBRACE: bracedepth--;
		}
	} while(bracedepth);

	return R_OK;
}

byte fileformat;
byte floatbits;

#include "XFileSections.h"


Result LoadMeshFromX(const FilePath& filename, MeshData *mdata, IObject::AttributeRange** attrs, XMaterial** mats, UINT* nummats)
{
	PlainData tokendata;
	WORD token;
	char* buf;
	Result rlt;

	if(filename.IsEmpty())
		return ERR("Parameter filename is empty");

	CHKALLOC(buf = new char[8]);

	std::ifstream fs(filename, std::ios::in | std::ios::binary);
	if(!fs.good())
	{
		fs.close();
		return ERR(String("File not found or inaccessible: ") << String(filename));
	}

	fs.read(buf, sizeof(long));
	if(*(long*)buf != XOFFILE_FORMAT_MAGIC)
		return ERR(String("Invalid file: ") << String(filename));

	fs.read(buf, sizeof(long));
	if(*(long*)buf != XOFFILE_FORMAT_VERSION)
		return ERR(String("Unsupported file version: ") << String(filename));

	fs.read(buf, sizeof(long));
	switch(*(long*)buf)
	{
	case XOFFILE_FORMAT_BINARY:
		fileformat = D3DXF_FILEFORMAT_BINARY;
		break;

	case XOFFILE_FORMAT_TEXT:
		fileformat = D3DXF_FILEFORMAT_TEXT;
		break;

	case XOFFILE_FORMAT_COMPRESSED:
		fileformat = D3DXF_FILEFORMAT_COMPRESSED;
		return ERR(String("Compressed file not supported: ") << String(filename));

	default:
		return ERR(String("Invalid file: ") << String(filename));
	}

	fs.read(buf, sizeof(long));
	switch(*(long*)buf)
	{
	case XOFFILE_FORMAT_FLOAT_BITS_32:
		floatbits = 32;
		break;

	case XOFFILE_FORMAT_FLOAT_BITS_64:
		floatbits = 64;
		break;

	default:
		return ERR(String("Unsupported float size: ") << String(filename));
	}
	delete[] buf;

	while(fs.peek() != -1)
	{
		CHKRESULT(ParseToken(&fs, &token, tokendata));
		switch(token)
		{
		case TOKEN_TEMPLATE:
			// Template definition

			// Template name
			CHKRESULT(ParseToken(&fs, &token, tokendata));
			if(token != TOKEN_PLAINTEXT && token != TOKEN_NAME)
				return ERR("");

			// Skip template
			SkipSection(&fs);
			break;

		case TOKEN_NAME: case TOKEN_PLAINTEXT:
			// Template name as type in text files (ASCII string) or binary files (TOKEN_NAME record)
			if(*tokendata.ptr.s == String("Mesh"))
				{CHKRESULT(LoadMesh(&fs, mdata, attrs, mats, nummats));}
			else
			{
				LOG(String("Section of unknown template '") << *tokendata.ptr.s << String("' skipped"));
				SkipSection(&fs);
			}
			break;

		case TOKEN_WORD: case TOKEN_DWORD: case TOKEN_FLOAT: case TOKEN_DOUBLE: case TOKEN_CHAR:
		case TOKEN_UCHAR: case TOKEN_SWORD: case TOKEN_SDWORD: case TOKEN_LPSTR:
			// Valid primitive type
			return ERR("Top level primitive types not supported");

		case TOKEN_CSTRING:
			// Unsupported type
			return ERR("Unsupported type: CString");
		case TOKEN_UNICODE:
			// Unsupported type
			return ERR("Unsupported type: Unicode");

case TOKEN_OBRACE:
break; // Skip token

		default:
			return ERR(String("Unexpected token: ") << String(token));
		}
	}

	fs.close();

	return R_OK;
}

Result ParseTokenString(std::ifstream* fs, byte floatbits, WORD* token, PlainData& tokendata)
{
	bool iscomment = false;
	String strtoken;
	std::streamoff oanglepos;
	char chr;

	strtoken = String();
	*token = NULL;

	// Find first character of token string
	while(1)
	{
		ReadVar(&chr, 1);

		if(iscomment)
		{
			if(chr == '\n')
				iscomment = false;

			continue;
		}

		switch(chr)
		{
		case '{':
			*token = TOKEN_OBRACE;
			return R_OK;

		case '}':
			*token = TOKEN_CBRACE;
			return R_OK;

		case '(':
			*token = TOKEN_OPAREN;
			return R_OK;

		case ')':
			*token = TOKEN_CPAREN;
			return R_OK;

		case '[':
			*token = TOKEN_OBRACKET;
			return R_OK;

		case ']':
			*token = TOKEN_CBRACKET;
			return R_OK;

		case '<':
			{
				// This brace might open a UUID
				char nextchr = fs->peek();
				if((nextchr >= '0' && nextchr <= '9') || (nextchr >= 'a' && nextchr <= 'f') || (nextchr >= 'A' && nextchr <= 'F'))
				{
					ReadVar(&chr, 1); // Skip '<'
					oanglepos = fs->tellg(); // Save file position after next char to '<'
					*token = TOKEN_PLAINUUID;
					break;
				}
			}
			*token = TOKEN_OANGLE;
			return R_OK;

		case '>':
			*token = TOKEN_CANGLE;
			return R_OK;

		case '.':
			*token = TOKEN_DOT;
			return R_OK;

		case ',':
			*token = TOKEN_COMMA;
			return R_OK;

		case ';':
			*token = TOKEN_SEMICOLON;
			return R_OK;

		case '/':
			if(fs->peek() != '/')
				break;
		case '#':
			iscomment = true;
		}

		if((chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z') || (chr >= '0' && chr <= '9') || chr == '_' || chr == '-')
			break;
	}

	if(*token != TOKEN_PLAINUUID)
		*token = (chr >= '0' && chr <= '9') || chr == '-' ? TOKEN_PLAININTEGER : TOKEN_PLAINTEXT;

	// Read string
	strtoken = String();
	while(1)
	{
		strtoken <<= (TCHAR)chr;

		ReadVar(&chr, 1);

		if(*token == TOKEN_PLAININTEGER && chr == '.')
		{
			char nextchr = fs->peek();
			if(nextchr == '.') // '123..' -> error
				return ERR(String("Syntax error parsing number: \"") << strtoken << String("..\""));

			// This variable is no longer a whole number
			*token = TOKEN_PLAINDECIMAL;
			continue;
		}

		// 'a' - 'f', 'A' - 'F' are invalid for numeric contend
		if((chr >= 'a' && chr <= 'f') || (chr >= 'A' && chr <= 'F'))
		{
			if(*token == TOKEN_PLAININTEGER || *token == TOKEN_PLAINDECIMAL)
				return ERR(String("Syntax error parsing number: \"") << strtoken << String((TCHAR)chr) << String('\"'));
		}
		// 'g' - 'z', 'G' - 'Z' and '_' are invalid for numeric contend and UUIDs
		else if((chr >= 'g' && chr <= 'z') || (chr >= 'G' && chr <= 'Z') || chr == '_')
		{
			if(*token == TOKEN_PLAININTEGER || *token == TOKEN_PLAINDECIMAL)
				return ERR(String("Syntax error parsing number: \"") << strtoken << String((TCHAR)chr) << String('\"'));
			else if(*token == TOKEN_PLAINUUID)
				return ERR(String("Syntax error parsing UUID: \"") << strtoken << String((TCHAR)chr) << String('\"'));
		}
		// '-' is invalid for numeric contend and plain text
		else if(chr == '-')
		{
			if(*token == TOKEN_PLAININTEGER || *token == TOKEN_PLAINDECIMAL)
				return ERR(String("Syntax error parsing number: \"") << strtoken << String((TCHAR)chr) << String('\"'));
			else if(*token == TOKEN_PLAINTEXT)
				return ERR(String("Syntax error parsing string: \"") << strtoken << String((TCHAR)chr) << String('\"'));
		}
		// '0' - '9' is valid for all plain data
		else if(chr < '0' || chr > '9')
			break;
	}

	switch(*token)
	{
	case TOKEN_PLAINTEXT:
		// Parse string
		if(strtoken == String("template"))
			*token = TOKEN_TEMPLATE;
		else if(strtoken == String("WORD"))
			*token = TOKEN_WORD;
		else if(strtoken == String("UINT"))
			*token = TOKEN_DWORD;
		else if(strtoken == String("FLOAT"))
			*token = TOKEN_FLOAT;
		else if(strtoken == String("DOUBLE"))
			*token = TOKEN_DOUBLE;
		else if(strtoken == String("CHAR"))
			*token = TOKEN_CHAR;
		else if(strtoken == String("FLOAT"))
			*token = TOKEN_FLOAT;
		else if(strtoken == String("UCHAR"))
			*token = TOKEN_UCHAR;
		else if(strtoken == String("SWORD"))
			*token = TOKEN_SWORD;
		else if(strtoken == String("VOID"))
			*token = TOKEN_VOID;
		else if(strtoken == String("STRING"))
			*token = TOKEN_LPSTR;
		//else if(strtoken == String("UNICODE")) // Unsupported
		//	*token = TOKEN_UNICODE; // Unsupported
		//else if(strtoken == String("CSTRING")) // Unsupported
		//	*token = TOKEN_CSTRING; // Unsupported
		else if(strtoken == String("array"))
			*token = TOKEN_ARRAY;

		CHKALLOC(tokendata.SetData(new String(strtoken), 1));
		break;

	case TOKEN_PLAININTEGER:
		// Parse integer
		CHKALLOC(tokendata.SetData(new UINT(strtoken.ToInt()), 1));
		break;

	case TOKEN_PLAINDECIMAL:
		// Parse decimal
		if(floatbits == 32)
		{
			CHKALLOC(tokendata.SetData(new float(strtoken.ToFloat()), 1));
		}
		else // floatbits == 64
		{
			CHKALLOC(tokendata.SetData(new double(strtoken.ToDouble()), 1));
		}
		break;

	case TOKEN_PLAINUUID:
		// Parse UUID
		UUID uuidtoken;
#ifdef UNICODE
typedef RPC_WSTR RPC_STR;
#else
typedef RPC_CSTR RPC_STR;
#endif
		if(!UuidFromString((RPC_STR)(LPTSTR)strtoken, &uuidtoken) == RPC_S_OK)
		{
			// Invalid UUID -> return '<' token
			fs->seekg(oanglepos); // Restore file position after next char to '<'
			*token = TOKEN_OANGLE;
		}
		else
			CHKALLOC(tokendata.SetData(new UUID(uuidtoken), 1));
		break;
	}

	// Shift readpos to end of token
	if(*token != TOKEN_PLAINUUID) // Skip '>' after UUID
		fs->seekg((int)fs->tellg() - 1);

	return R_OK;
}

#pragma region ParseToken
Result ParseToken(std::ifstream* fs, WORD* token, PlainData& tokendata)
{
	UINT arraysize;
	Result rlt;

	switch(fileformat)
	{
	case D3DXF_FILEFORMAT_BINARY:
		ReadVar(token, sizeof(WORD));
		switch(*token)
		{
		case TOKEN_NAME: case TOKEN_STRING:
			// Read TOKEN_NAME record
			{
				char* namechars;
				UINT strlen;

				ReadVar(&strlen, sizeof(UINT));
				CHKALLOC(namechars = new char[strlen + 1]);
				ReadVar(namechars, strlen);
				namechars[strlen] = NULL;
				CHKALLOC(tokendata.SetData(new String(namechars), 1)); //EDIT: Will cause an error when build for WCHAR systems
				delete[] namechars;

				if(*token == TOKEN_STRING) // A string record ends with a terminator token
				{
					WORD terminator;
					ReadVar(&terminator, sizeof(WORD));
					if(terminator != TOKEN_SEMICOLON && terminator != TOKEN_COMMA)
						return ERR("");
				}
			}
			break;

		case TOKEN_INTEGER:
			// Read TOKEN_INTEGER record
			CHKALLOC(tokendata.SetData(new UINT(), 1));
			ReadVar(tokendata.ptr.i, sizeof(UINT))
			break;

		case TOKEN_GUID:
			// Read TOKEN_GUID record
			CHKALLOC(tokendata.SetData(new GUID(), 1));
			ReadVar(tokendata.ptr.u, sizeof(UUID))
			break;

		case TOKEN_INTEGER_LIST:
			// Read TOKEN_INTEGER_LIST record
			ReadVar(&arraysize, sizeof(UINT));
			CHKALLOC(tokendata.SetData(new UINT[arraysize], arraysize));
			ReadVar(tokendata.ptr.i, arraysize * sizeof(UINT));
			break;

		case TOKEN_FLOAT_LIST:
			// Read TOKEN_INTEGER_LIST record
			ReadVar(&arraysize, sizeof(UINT));
			if(floatbits == 32)
			{
				CHKALLOC(tokendata.SetData(new float[arraysize], arraysize));
				ReadVar(tokendata.ptr.f, arraysize * sizeof(float));
			}
			else // floatbits == 64
			{
				CHKALLOC(tokendata.SetData(new double[arraysize], arraysize));
				ReadVar(tokendata.ptr.d, arraysize * sizeof(double));
			}
			break;

		default:
			tokendata.SetData((UINT*)NULL, 0);
		}
		return R_OK;

	case D3DXF_FILEFORMAT_TEXT:
		CHKRESULT(ParseTokenString(fs, floatbits, token, tokendata));
		return R_OK;

	default:
		return ERR("Invalid file format");
	}
}
#pragma endregion

Result LoadMesh(std::ifstream* fs, MeshData *mdata, IObject::AttributeRange** attrs, XMaterial** mats, UINT* nummats)
{
	std::list<Template*>::const_iterator template_itr;
	PlainData plaindata;
	WORD token;
	UINT i;
	Result rlt;

	CHKRESULT(ReadMeshSection(fs, mdata));
	CHKRESULT(ParseToken(fs, &token, plaindata));

	// "MeshNormals"
	if(token != TOKEN_PLAINTEXT && token != TOKEN_NAME)
		return ERR("");
	if(*plaindata.ptr.s == String("MeshNormals"))
	{
		CHKRESULT(ReadMeshNormalsSection(fs, mdata));
		CHKRESULT(ParseToken(fs, &token, plaindata));
	}
	else
	{
		for(i = 0; i < mdata->numvertices; i++)
			mdata->vertices[i].nml = Vector3(0.0f, 0.0f, 0.0f);
	}

	// "MeshTextureCoords"
	if(token != TOKEN_PLAINTEXT && token != TOKEN_NAME)
		return ERR("");
	if(*plaindata.ptr.s == String("MeshTextureCoords"))
	{
		CHKRESULT(ReadMeshTextureCoordsSection(fs, mdata));
		CHKRESULT(ParseToken(fs, &token, plaindata));
	}
	else
	{
		for(i = 0; i < mdata->numvertices; i++)
			mdata->vertices[i].texcoord = Vector2(0.0f, 0.0f);
	}

	// "MeshMaterialList"
	if(token != TOKEN_PLAINTEXT && token != TOKEN_NAME)
		return ERR("");
	if(*plaindata.ptr.s != String("MeshMaterialList"))
		return ERR("");

	CHKRESULT(ReadMeshMaterialListSection(fs, mdata, attrs, mats, nummats));
	CHKRESULT(ParseToken(fs, &token, plaindata));

mdata->numblendparts = 0;

	// "FVFData" or "VertexDuplicationIndices"
	if(token == TOKEN_PLAINTEXT || token == TOKEN_NAME)
	{
		if(*plaindata.ptr.s == String("FVFData"))
			{CHKRESULT(ReadMeshFVFDataSection(fs, mdata));}
		else
			SkipSection(fs);
		CHKRESULT(ParseToken(fs, &token, plaindata));
	}

	if(!mdata->numblendparts)
	{
		mdata->numblendparts = 1;
		for(i = 0; i < mdata->numvertices; i++)
			mdata->vertices[i].blendidx = 0;
	}

	// '}' (end of Mesh)
	if(token != TOKEN_CBRACE)
		return ERR("");

	return R_OK;
}