#ifndef __XFILE_H
#define __XFILE_H


#pragma comment (lib, "Rpcrt4.lib")

// XOF header constants
#define XOFFILE_FORMAT_MAGIC			((long)'x' + ((long)'o' << 8) + ((long)'f' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_VERSION			((long)'0' + ((long)'3' << 8) + ((long)'0' << 16) + ((long)'3' << 24))
#define XOFFILE_FORMAT_BINARY			((long)'b' + ((long)'i' << 8) + ((long)'n' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_TEXT				((long)'t' + ((long)'x' << 8) + ((long)'t' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_COMPRESSED		((long)'c' + ((long)'m' << 8) + ((long)'p' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_FLOAT_BITS_32	((long)'0' + ((long)'0' << 8) + ((long)'3' << 16) + ((long)'2' << 24))
#define XOFFILE_FORMAT_FLOAT_BITS_64	((long)'0' + ((long)'0' << 8) + ((long)'6' << 16) + ((long)'4' << 24))

// XOF type constants
#define D3DXF_FILEFORMAT_BINARY			0x0
#define D3DXF_FILEFORMAT_TEXT			0x1
#define D3DXF_FILEFORMAT_COMPRESSED		0x2

// XOF record-bearing tokens
#define TOKEN_NAME						0x1
#define TOKEN_STRING					0x2
#define TOKEN_INTEGER					0x3
#define TOKEN_GUID						0x5
#define TOKEN_INTEGER_LIST				0x6
#define TOKEN_FLOAT_LIST				0x7
// XOF stand-alone tokens
#define TOKEN_OBRACE					0xA
#define TOKEN_CBRACE					0xB
#define TOKEN_OPAREN					0xC
#define TOKEN_CPAREN					0xD
#define TOKEN_OBRACKET					0xE
#define TOKEN_CBRACKET					0xF
#define TOKEN_OANGLE					0x10
#define TOKEN_CANGLE					0x11
#define TOKEN_DOT						0x12
#define TOKEN_COMMA						0x13
#define TOKEN_SEMICOLON					0x14
#define TOKEN_TEMPLATE					0x1F
#define TOKEN_WORD						0x28
#define TOKEN_DWORD						0x29
#define TOKEN_FLOAT						0x2A
#define TOKEN_DOUBLE					0x2B
#define TOKEN_CHAR						0x2C
#define TOKEN_UCHAR						0x2D
#define TOKEN_SWORD						0x2E
#define TOKEN_SDWORD					0x2F
#define TOKEN_VOID						0x30
#define TOKEN_LPSTR						0x31
#define TOKEN_UNICODE					0x32
#define TOKEN_CSTRING					0x33
#define TOKEN_ARRAY						0x34
// Invalid tokens (not part of XOF standard)
#define TOKEN_PLAINTEXT					0x100
#define TOKEN_PLAININTEGER				0x101
#define TOKEN_PLAINDECIMAL				0x102
#define TOKEN_PLAINUUID					0x103

typedef UINT INDEX_TYPE;

struct MeshData
{
	struct Vertex
	{
		Vector3 pos, nml;
		Vector2 texcoord;
		UINT blendidx;
	} *vertices;
	INDEX_TYPE *indices;
	UINT numvertices, numfaces;
	UINT numblendparts;

	MeshData() : vertices(NULL), indices(NULL) {}

	~MeshData()
	{
		REMOVE_ARRAY(vertices);
		REMOVE_ARRAY(indices);
	}
};

struct XMaterial
{
	float faceColor[4];
	float power;
	float specularColor[3];
	float emissiveColor[3];
	FilePath textureFilename;
};



Result LoadMeshFromX(const FilePath& filename, MeshData *mdata, IObject::AttributeRange** attrs, XMaterial** mats, UINT* nummats);

#endif