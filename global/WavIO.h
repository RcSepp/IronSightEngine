HRESULT LoadWAV(LPCSTR filename, WAVEFORMATEX *fmt, DWORD *numsamplesperchannel, char** wavdata, DWORD *wavdatasize);
HRESULT SaveWAV(LPCSTR filename, WAVEFORMATEX fmt, DWORD numsamplesperchannel, char* wavdata, DWORD wavdatasize);

bool ReadRIFF(std::ifstream &fs, WAVEFORMATEX &fmt, DWORD *numsamplesperchannel, DWORD *datasize);
bool WriteRIFF(std::ofstream &fs, WAVEFORMATEX fmt, DWORD numsamplesperchannel, DWORD datasize);