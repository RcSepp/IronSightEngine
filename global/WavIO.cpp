#include <fstream>
#include <windows.h>

//-----------------------------------------------------------------------------
// Name: ReadRIFF()
// Desc: Verify wav file and read header
//-----------------------------------------------------------------------------
bool ReadRIFF(std::ifstream &fs, WAVEFORMATEX &fmt, DWORD *numsamplesperchannel, DWORD *datasize)
{
	std::streamoff datastart, dataend;
	char cbuf[4];
	int ibuf;
	short sbuf;

	fs.read((char*) &cbuf, sizeof(cbuf));
	if(memcmp(cbuf, "RIFF", 4) != 0)
		return false;

	fs.read((char*) &ibuf, sizeof(ibuf));
	dataend = ibuf;

	fs.read((char*) &cbuf, sizeof(cbuf));
	if(memcmp(cbuf, "WAVE", 4) != 0)
		return false;
	fs.read((char*) &cbuf, sizeof(cbuf));
	if(memcmp(cbuf, "fmt ", 4) != 0)
	{
		if(memcmp(cbuf, "LIST", 4) != 0)
			return false;

		fs.read((char*) &ibuf, sizeof(ibuf)); // Format chunk length
		fs.seekg(ibuf, std::ios::cur);
		fs.read((char*) &cbuf, sizeof(cbuf));
		if(memcmp(cbuf, "fmt ", 4) != 0)
			return false;
	}

	fs.read((char*) &ibuf, sizeof(ibuf)); // Format chunk length = 16
	fs.read((char*) &sbuf, sizeof(sbuf)); // File padding = 1

	fs.read((char*) &sbuf, sizeof(sbuf));
	fmt.nChannels = sbuf;
	fs.read((char*) &ibuf, sizeof(ibuf));
	fmt.nSamplesPerSec = ibuf;
	fs.read((char*) &ibuf, sizeof(ibuf));
	fmt.nAvgBytesPerSec = ibuf;

	fs.read((char*) &sbuf, sizeof(sbuf)); // Bytes per sample

	fs.read((char*) &sbuf, sizeof(sbuf));
	fmt.wBitsPerSample = sbuf;

	/*fs.read((char*) &cbuf, sizeof(cbuf));
	if(strcmp(cbuf,"data") != 0)
		return false;*/
	/*fs.read((char*) &c, 1);
	while(fs.tellg() < dataend)
	{
		if(c == 'd')
		{
			fs.read((char*) &c, 1);
			if(c == 'a')
			{
				fs.read((char*) &c, 1);
				if(c == 't')
				{
					fs.read((char*) &c, 1);
					if(c == 'a')
						break;
				}
			}
		}
		else
			fs.read((char*) &c, 1);
	}
	if(fs.tellg() >= dataend)
		return false;*/
//fs.read((char*) &cbuf, 2); // BUGFIX
	fs.read((char*) &cbuf, sizeof(cbuf));
	if(memcmp(cbuf, "fact", 4) == 0)
	{
		fs.read((char*) &ibuf, sizeof(ibuf)); // Format chunk length = 4
		if(ibuf != 4)
			return false;
		fs.read((char*) numsamplesperchannel, sizeof(ibuf));
		fs.read((char*) &cbuf, sizeof(cbuf));
	}
	else
		*numsamplesperchannel = 0;
	if(memcmp(cbuf, "data", 4) != 0)
		return false;
	fs.read((char*) &ibuf, sizeof(ibuf));
	*datasize = ibuf;

	fmt.wFormatTag = WAVE_FORMAT_PCM;
	fmt.nBlockAlign = fmt.nChannels * fmt.wBitsPerSample / 8;
	fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;
	fmt.cbSize = 0;

	datastart = fs.tellg();
	/*fs.seekg(0, std::ios::end);
	dataend = fs.tellg();
	fs.seekg(0, std::ios::beg);*/
	//*datasize = (DWORD)(dataend - datastart);

	return true;
}

//-----------------------------------------------------------------------------
// Name: WriteRIFF()
// Desc: Write wav file header
//-----------------------------------------------------------------------------
bool WriteRIFF(std::ofstream &fs, WAVEFORMATEX fmt, DWORD numsamplesperchannel, DWORD datasize)
{
	std::streamoff datastart, dataend;
	int ibuf;
	short sbuf;

	fs.write("RIFF", 4);

	ibuf = 0; fs.write((char*)&ibuf, sizeof(ibuf));
	dataend = ibuf;

	fs.write("WAVEfmt ", 8);

	ibuf = 16; fs.write((char*)&ibuf, sizeof(ibuf)); // Format chunk length = 16
	sbuf = 1; fs.write((char*)&sbuf, sizeof(sbuf)); // File padding = 1

	sbuf = fmt.nChannels; fs.write((char*)&sbuf, sizeof(sbuf));
	ibuf = fmt.nSamplesPerSec; fs.write((char*)&ibuf, sizeof(ibuf));
	ibuf = fmt.nAvgBytesPerSec; fs.write((char*)&ibuf, sizeof(ibuf));

	sbuf = fmt.nBlockAlign; fs.write((char*)&sbuf, sizeof(sbuf));
	sbuf = fmt.wBitsPerSample; fs.write((char*)&sbuf, sizeof(sbuf));

	if(numsamplesperchannel)
	{
		fs.write("fact", 4);
		ibuf = 4; fs.write((char*)&ibuf, sizeof(ibuf)); // Format chunk length = 4
		fs.write((char*)&numsamplesperchannel, sizeof(ibuf));
	}
	fs.write("data", 4);
	ibuf = datasize; fs.write((char*)&ibuf, sizeof(ibuf));

	datastart = fs.tellp();
	dataend = datastart + datasize - 8;
	fs.seekp(4, std::ios::beg);
	ibuf = (int)dataend; fs.write((char*)&ibuf, sizeof(ibuf));
	fs.seekp(datastart, std::ios::beg);

	return true;
}

//-----------------------------------------------------------------------------
// Name: LoadWAV()
// Desc: Load wav music stream from file
//-----------------------------------------------------------------------------
HRESULT LoadWAV(LPCSTR filename, WAVEFORMATEX *fmt, DWORD *numsamplesperchannel, char** wavdata, DWORD *wavdatasize)
{
	std::ifstream fs(filename, std::ios::binary);

	if(fs.peek() == -1)
		return ERROR_FILE_NOT_FOUND;

	if(!ReadRIFF(fs, *fmt, numsamplesperchannel, wavdatasize))
		return 0x8004022f; // INVALID_FILE_FORMAT

	// Read data from file
	*wavdata = (char*)malloc(*wavdatasize);
	fs.read(*wavdata, *wavdatasize);

	fs.close();

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SaveWAV()
// Desc: Save wav music stream
//-----------------------------------------------------------------------------
HRESULT SaveWAV(LPCSTR filename, WAVEFORMATEX fmt, DWORD numsamplesperchannel, char* wavdata, DWORD wavdatasize)
{
	std::ofstream fs(filename, std::ios::binary);

	WriteRIFF(fs, fmt, numsamplesperchannel, wavdatasize);

	// Write data to file
	fs.write(wavdata, wavdatasize);

	int ibuf = 0;
	fs.write((char*)&ibuf, sizeof(ibuf));
	fs.write((char*)&ibuf, sizeof(ibuf));

	fs.close();

	return S_OK;
}