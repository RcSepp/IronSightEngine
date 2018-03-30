#ifndef __ID3LIB_H
#define __ID3LIB_H

#include "ISID3Lib.h"

// ID3Lib Library
#ifdef _DEBUG
	#pragma comment(lib, "id3libd")
#else
	#pragma comment(lib, "id3lib")
#endif
// Delayed dll linkage
#pragma comment(lib, "DelayImp.lib")


//-----------------------------------------------------------------------------
// Name: ID3
// Desc: Representation of the ID3 metadata container inside a media file
//-----------------------------------------------------------------------------
class ID3Tag : public IID3Tag
{
private:
	ID3_Tag* id3;

public:

	ID3Tag()
	{
	};

	Result Load(FilePath& path);
	const Mp3_Headerinfo* ReadHeader() const
		{return id3->GetMp3HeaderInfo();}
	void GetNextFrame();
	void Release();
};

//-----------------------------------------------------------------------------
// Name: ID3Lib
// Desc: API to the ID3Lib library
//-----------------------------------------------------------------------------
class ID3Lib : public IID3Lib
{
private:
	std::list<ID3Tag*> tags;

public:

	ID3Lib()
	{
	};

	void Sync(GLOBALVARDEF_LIST);
	Result Init();
	Result LoadTagFromFile(FilePath& path, IID3Tag** tag);
	void Release();
};

#endif