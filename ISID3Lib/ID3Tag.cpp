#include "ID3Lib.h"

#include "ISCryptoPP.h"

struct AudioEncryptionFrame
{
	LPCSTR ownerid;
	WORD previewstart, previewlength;
	BYTE encdata[16]; // Array length isn't fixed by the standard
};

//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load an ID3 tag from its containing media file
//-----------------------------------------------------------------------------
Result ID3Tag::Load(FilePath& path)
{
	CHKALLOC(id3 = new ID3_Tag(path.ToCharString()));
	LPCSTR ownerid = "RcSepp";
	WORD prevstart = 0;
	WORD previewlength = 0;
	BYTE encdata[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
					  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

byte *zlencdata;
int zlencdatalen;
cpp->ToZerolessKey(encdata, sizeof(encdata), &zlencdata, &zlencdatalen);

	ID3_Frame* aencframe = new ID3_Frame(ID3FID_AUDIOCRYPTO);
	aencframe->Field(ID3FN_DATA).Set(zlencdata, zlencdatalen);
	id3->AddFrame(aencframe);

	ID3_Frame* encrframe = new ID3_Frame(ID3FID_CRYPTOREG);
	encrframe->Field(ID3FN_OWNER).Set(ownerid);
	encrframe->Field(ID3FN_ID).Set(0x80);
	encrframe->Field(ID3FN_DATA).Set(zlencdata, zlencdatalen);
	id3->AddFrame(encrframe);

	//id3->Update();




//EDIT: ToZerolessKey() and FromZerolessKey() done;
//TODO:
//	1) Split ID3Tag::Load() into multiple read- and write functions (call ToZerolessKey() and FromZerolessKey())
//	2) Finish implementing encryption into ID3TagLib



ID3_Frame* titleframe = id3->Find(ID3FID_TITLE);
if(!titleframe)
{
	titleframe = new ID3_Frame(ID3FID_TITLE);
	titleframe->SetEncryptionID(0x80);
	titleframe->Field(ID3FN_TEXTENC).Set(ID3TE_ISO8859_1);
	titleframe->Field(ID3FN_TEXT).Set("Encrypted Title");
	id3->AddFrame(titleframe);
}
else
{
	titleframe->SetEncryptionID(0x80);
	titleframe->Field(ID3FN_TEXTENC).Set(ID3TE_ISO8859_1);
	titleframe->Field(ID3FN_TEXT).Set("Encrypted Title");
}

id3->Update();
id3->Clear();
id3->Link(path.ToCharString());

ID3_Frame* tf = id3->Find(ID3FID_TITLE);
char buffer[1024];
tf->Field(ID3FN_TEXT).Get(buffer, sizeof(buffer));

	/*ID3_Frame* aencframe = id3->Find(ID3FID_AUDIOCRYPTO);
	BYTE buffer[1024];
	aencframe->Field(ID3FN_DATA).Get(buffer, sizeof(buffer));*/

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void ID3Tag::Release()
{
	if(id3)
	{
		// In rare cases (e.g. if the media file is corrupt) deleting id3 can fail
		try
			{delete id3;}
		catch(...) {}
		id3 = NULL;
	}

	delete this;
}