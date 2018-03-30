#include "WinAudio.h"
#include "..\\global\\WavIO.h"

#include <mmdeviceapi.h>


#define	WAVEMAPPERNAME				"Microsoft Sound Mapper"
#define REMAPDEVICEID(deviceid)		deviceid == 0 ? WAVE_MAPPER : deviceid - 1


//-----------------------------------------------------------------------------
// Name: CreateWinAudio()
// Desc: DLL API for creating an instance of WinAudio
//-----------------------------------------------------------------------------
WINAUDIO_EXTERN_FUNC LPWINAUDIO __cdecl CreateWinAudio()
{
	return new WinAudio();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void WinAudio::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
	if(pse)
		SyncWithPython();
}

//-----------------------------------------------------------------------------
// Name: OutStreamingThread()
// Desc: Thread for continuously providing data to a streamed sound
//-----------------------------------------------------------------------------
DWORD __stdcall StreamedSound::OutStreamingThread(LPVOID args)
{
    StreamedSound* snd = (StreamedSound*)args;
	AudioDataPtr data;
	SoundDoneCallbackResult cbkrlt = SDCR_OK;

	// Initially clear buffers
	snd->ClearBuffers();

	while(snd->state != SS_CLOSING)
    {
        WaitForSingleObject(snd->streamingevent, INFINITE);

		if(snd->state == SS_PLAYING)
		{
			EnterCriticalSection(&snd->donefuncsection);
			for(DWORD i = 0; i < snd->numbuffers; i++)
				if(snd->headers[i].dwFlags & WHDR_DONE)
				{
					data.cdata = snd->headers[i].lpData;
					cbkrlt = snd->ondonefunc((IWadStreamedSound*)snd, data, snd->ondonefuncuser);
					if(cbkrlt != SDCR_OK)
					{
						snd->state = SS_STOPPED;
						break;
					}
					waveOutWrite(snd->device->hdl, snd->headers + i, sizeof(WAVEHDR));
				}
			LeaveCriticalSection(&snd->donefuncsection);
			if(cbkrlt == SDCR_FINISHED && snd->onfinished)
				snd->onfinished((IWadStreamedSound*)snd, snd->onfinisheduser);
			else if(cbkrlt == SDCR_NODATA)
				snd->state = SS_NODATA;
			cbkrlt = SDCR_OK;
		}
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Name: InStreamingThread()
// Desc: Thread for continuously providing data to a streamed sound
//-----------------------------------------------------------------------------
DWORD __stdcall InStreamingThread(LPVOID args)
{
    StreamedRecorder* rec = (StreamedRecorder*)args;
	AudioDataPtr data;

	while(rec->state != StreamedRecorder::SS_CLOSING)
    {
        WaitForSingleObject(rec->streamingevent, INFINITE);

		if(rec->state == StreamedRecorder::SS_RECORDING)
			for(DWORD i = 0; i < rec->numbuffers; i++)
				if(rec->headers[i].dwFlags & WHDR_DONE)
				{
					data.cdata = rec->headers[i].lpData;
					rec->ondonefunc((IWadStreamedRecorder*)rec, data, rec->ondonefuncuser);
					waveInAddBuffer(rec->device->hdl, &rec->headers[i], sizeof(WAVEHDR));
				}
    }

    return 0;
}

void CALLBACK WaveOutCallback(HWAVEOUT hwo, UINT msg, DWORD instance, DWORD wparam, DWORD lparam)
{
	StreamedSound* snd;

	switch(msg)
	{
	case MM_WOM_DONE:
		LPWAVEHDR wavhdr = (LPWAVEHDR)wparam;
		WadPlayable::SoundType type = ((WadPlayable*)wavhdr->dwUser)->type;
		if(type == WadPlayable::ST_STREAMEDSOUND)
		{
			snd = (StreamedSound*)wavhdr->dwUser;
			SetEvent(snd->streamingevent);
		}
	}
}
void CALLBACK WaveInCallback(HWAVEOUT hwo, UINT msg, DWORD instance, DWORD wparam, DWORD lparam)
{
	StreamedRecorder* rec;

	switch(msg)
	{
	case MM_WIM_DATA:
		rec = (StreamedRecorder*)((LPWAVEHDR)wparam)->dwUser;
		SetEvent(rec->streamingevent);
	}
}

//-----------------------------------------------------------------------------
// Name: WinAudio()
// Desc: Constructor
//-----------------------------------------------------------------------------
WinAudio::WinAudio()
{
	indev = NULL;
	outdevnames = indevnames = NULL;
	numindevs = numoutdevs = 0;
	sysvolume = NULL;
	sysvolumecbkclass.cbk = NULL;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Query audio devices
//-----------------------------------------------------------------------------
Result WinAudio::Init()
{
	// Query output devices
	WAVEOUTCAPS wocaps;

	numoutdevs = waveOutGetNumDevs();
	if(numoutdevs == 0)
		return ERR("No MME audio output devices found"); //EDIT: Log, don't fail
	numoutdevs++; // Add wave mapper device

	// Get output device names
	CHKALLOC(outdevnames = new String[numoutdevs]);
	outdevnames[0] = String(WAVEMAPPERNAME);
	for(UINT i = 0; i < numoutdevs; i++)
	{
		waveOutGetDevCaps(i, &wocaps, sizeof(wocaps));
		outdevnames[i] = String(wocaps.szPname);
	}

	// Query input devices
	WAVEINCAPS wicaps;

	numindevs = waveInGetNumDevs();
	if(numindevs == 0)
		return ERR("No MME audio input devices found"); //EDIT: Log, don't fail
	numindevs++; // Add wave mapper device

	// Get input device names
	CHKALLOC(indevnames = new String[numindevs]);
	indevnames[0] = String(WAVEMAPPERNAME);
	for(UINT i = 0; i < numindevs; i++)
	{
		waveInGetDevCaps(i, &wicaps, sizeof(wicaps));
		indevnames[i] = String(wocaps.szPname);
	}

	// Get system volume control
	IMMDeviceEnumerator* devenum;
	CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&devenum);

	IMMDevice* speakers;
	devenum->GetDefaultAudioEndpoint(eRender, eMultimedia, &speakers);
	devenum->Release();

	speakers->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&sysvolume);
	speakers->Release();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: GetSupportedOutputFormats()
// Desc: Get available sample rates, channels and bits per sample
//-----------------------------------------------------------------------------
Result WinAudio::GetSupportedOutputFormats(UINT deviceid, AudioFormat** formats, int* numformats) const
{
	if(deviceid >= numoutdevs)
		return ERR("Invalid device id");
	REMAPDEVICEID(deviceid); // Wave mapper acts as the first device

	WAVEOUTCAPS wocaps;
	waveOutGetDevCaps(deviceid, &wocaps, sizeof(wocaps));

	CHKALLOC(*formats = new AudioFormat[20]);
	*numformats = 0;

	if(wocaps.dwFormats & WAVE_FORMAT_1M08)
		(*formats)[(*numformats)++] = AudioFormat(11025, 1, 8);
	if(wocaps.dwFormats & WAVE_FORMAT_1S08)
		(*formats)[(*numformats)++] = AudioFormat(11025, 2, 8);
	if(wocaps.dwFormats & WAVE_FORMAT_1M16)
		(*formats)[(*numformats)++] = AudioFormat(11025, 1, 16);
	if(wocaps.dwFormats & WAVE_FORMAT_1S16)
		(*formats)[(*numformats)++] = AudioFormat(11025, 2, 16);
	if(wocaps.dwFormats & WAVE_FORMAT_2M08)
		(*formats)[(*numformats)++] = AudioFormat(22050, 1, 8);
	if(wocaps.dwFormats & WAVE_FORMAT_2S08)
		(*formats)[(*numformats)++] = AudioFormat(22050, 2, 8);
	if(wocaps.dwFormats & WAVE_FORMAT_2M16)
		(*formats)[(*numformats)++] = AudioFormat(22050, 1, 16);
	if(wocaps.dwFormats & WAVE_FORMAT_2S16)
		(*formats)[(*numformats)++] = AudioFormat(22050, 2, 16);
	if(wocaps.dwFormats & WAVE_FORMAT_44M08)
		(*formats)[(*numformats)++] = AudioFormat(44100, 1, 8);
	if(wocaps.dwFormats & WAVE_FORMAT_44S08)
		(*formats)[(*numformats)++] = AudioFormat(44100, 2, 8);
	if(wocaps.dwFormats & WAVE_FORMAT_44M16)
		(*formats)[(*numformats)++] = AudioFormat(44100, 1, 16);
	if(wocaps.dwFormats & WAVE_FORMAT_44S16)
		(*formats)[(*numformats)++] = AudioFormat(44100, 2, 16);
	if(wocaps.dwFormats & WAVE_FORMAT_48M08)
		(*formats)[(*numformats)++] = AudioFormat(48000, 1, 8);
	if(wocaps.dwFormats & WAVE_FORMAT_48S08)
		(*formats)[(*numformats)++] = AudioFormat(48000, 2, 8);
	if(wocaps.dwFormats & WAVE_FORMAT_48M16)
		(*formats)[(*numformats)++] = AudioFormat(48000, 1, 16);
	if(wocaps.dwFormats & WAVE_FORMAT_48S16)
		(*formats)[(*numformats)++] = AudioFormat(48000, 2, 16);
	if(wocaps.dwFormats & WAVE_FORMAT_96M08)
		(*formats)[(*numformats)++] = AudioFormat(96000, 1, 8);
	if(wocaps.dwFormats & WAVE_FORMAT_96S08)
		(*formats)[(*numformats)++] = AudioFormat(96000, 2, 8);
	if(wocaps.dwFormats & WAVE_FORMAT_96M16)
		(*formats)[(*numformats)++] = AudioFormat(96000, 1, 16);
	if(wocaps.dwFormats & WAVE_FORMAT_96S16)
		(*formats)[(*numformats)++] = AudioFormat(96000, 2, 16);

	CHKALLOC(*formats = (AudioFormat*)_realloc(*formats, 20 * sizeof(AudioFormat), *numformats * sizeof(AudioFormat)));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: QueryOutputFormat()
// Desc: Test output format without opening a device
//-----------------------------------------------------------------------------
Result WinAudio::QueryOutputFormat(UINT deviceid, const AudioFormat* format)
{
	REMAPDEVICEID(deviceid); // Wave mapper acts as the first device

	Result rlt;

	// Formulate wave format
	WAVEFORMATEXTENSIBLE wavefmt;
	format->GetWaveFormat(&wavefmt);

	CHKRESULT(waveOutOpen(NULL, deviceid, &wavefmt.Format, NULL, NULL, WAVE_FORMAT_QUERY));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: OpenOutputDevice()
// Desc: Open output device by id
//-----------------------------------------------------------------------------
Result WinAudio::OpenOutputDevice(UINT deviceid, const AudioFormat* format, IWadOutDevice** outdev)
{
	*outdev = NULL;

	if(deviceid >= numoutdevs)
		return ERR("Invalid device id");
	REMAPDEVICEID(deviceid); // Wave mapper acts as the first device

	Result rlt;

	// Formulate wave format
	WAVEFORMATEXTENSIBLE wavefmt;
	format->GetWaveFormat(&wavefmt);

	HWAVEOUT devicehandle;
	CHKRESULT(waveOutOpen(&devicehandle, deviceid, &wavefmt.Format, (DWORD_PTR)WaveOutCallback, NULL, CALLBACK_FUNCTION));

	OutDevice* newoutdev;
	CHKALLOC(newoutdev = new OutDevice(devicehandle, this));

	outdevs.push_back(newoutdev);
	*outdev = newoutdev;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: QueryInputFormat()
// Desc: Test input format without opening a device
//-----------------------------------------------------------------------------
Result WinAudio::QueryInputFormat(UINT deviceid, const AudioFormat* format)
{
	REMAPDEVICEID(deviceid); // Wave mapper acts as the first device

	Result rlt;

	// Formulate wave format
	WAVEFORMATEXTENSIBLE wavefmt;
	format->GetWaveFormat(&wavefmt);

	CHKRESULT(waveInOpen(NULL, deviceid, &wavefmt.Format, NULL, NULL, WAVE_FORMAT_QUERY));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: OpenInputDevice()
// Desc: Open input device by id
//-----------------------------------------------------------------------------
Result WinAudio::OpenInputDevice(UINT deviceid, const AudioFormat* format, IWadInDevice** indev)
{
	*indev = NULL;

	if(deviceid >= numindevs)
		return ERR("Invalid device id");
	REMAPDEVICEID(deviceid); // Wave mapper acts as the first device

	Result rlt;

	// Formulate wave format
	WAVEFORMATEXTENSIBLE wavefmt;
	format->GetWaveFormat(&wavefmt);

	HWAVEIN devicehandle;
	CHKRESULT(waveInOpen(&devicehandle, deviceid, &wavefmt.Format, (DWORD_PTR)WaveInCallback, NULL, CALLBACK_FUNCTION));

	InDevice* newindev;
	CHKALLOC(newindev = new InDevice(devicehandle, this));

	indevs.push_back(newindev);
	*indev = newindev;

	return R_OK;
}

void WinAudio::SetVolumeChangedCallback(VOLUMELEVELCHANGEDCALLBACK cbk, LPVOID user)
{
	if(sysvolume)
	{
		if(sysvolumecbkclass.cbk)
			sysvolume->UnregisterControlChangeNotify(&sysvolumecbkclass);

		sysvolume->RegisterControlChangeNotify(&sysvolumecbkclass);
		sysvolumecbkclass.cbk = cbk;
		sysvolumecbkclass.user = user;
	}
}

HRESULT STDMETHODCALLTYPE WinAudio::AudioEndpointVolumeCallback::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
{
	cbk(pNotify->fMasterVolume, user);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WinAudio::AudioEndpointVolumeCallback::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	if (!ppvObj)
		return E_INVALIDARG;
	*ppvObj = NULL;

	// Increment reference count and return the pointer
	*ppvObj = (LPVOID)this;
	AddRef();
	return NOERROR;
}
ULONG STDMETHODCALLTYPE WinAudio::AudioEndpointVolumeCallback::AddRef()
{
    InterlockedIncrement(&refcounter);
    return refcounter;
}
ULONG STDMETHODCALLTYPE WinAudio::AudioEndpointVolumeCallback::Release()
{
    // Decrement the object's internal counter.
    ULONG refcount = InterlockedDecrement(&refcounter);
    if(0 == refcounter)
        delete this;
    return refcount;
}

float WinAudio::GetVolume()
{
	float val;
	sysvolume->GetMasterVolumeLevelScalar(&val);
	return val;
}
void WinAudio::SetVolume(float val)
{
	sysvolume->SetMasterVolumeLevelScalar(val, NULL);
}
float WinAudio::GetChannelVolume(UINT channel)
{
	float val;
	sysvolume->GetChannelVolumeLevelScalar(channel, &val);
	return val;
}
void WinAudio::SetChannelVolume(UINT channel, float val)
{
	sysvolume->SetChannelVolumeLevelScalar(channel, val, NULL);
}

Result OutDevice::CreateSound(DWORD bufferlen, DWORD loops, IWadSound** snd)
{
	Result rlt;

	*snd = NULL;

	WAVEHDR* wavehdr;
	CHKALLOC(wavehdr = new WAVEHDR());
	CHKALLOC(wavehdr->lpData = new char[bufferlen]);
	wavehdr->dwBufferLength = bufferlen;
	wavehdr->dwFlags = NULL;
	CHKRESULT(waveOutPrepareHeader(hdl, wavehdr, sizeof(WAVEHDR)));

	if(loops)
	{
		wavehdr->dwFlags |= WHDR_BEGINLOOP | WHDR_ENDLOOP;
		wavehdr->dwLoops = loops;
	}

	Sound* newsnd;
	CHKALLOC(newsnd = new Sound(this, wavehdr, NULL)); // Edit: replace NULL with audio format of outdev
	wavehdr->dwUser = (DWORD_PTR)newsnd;

	sounds.push_back(newsnd);
	*snd = newsnd;

	return R_OK;
}

Result OutDevice::CreateSound(const FilePath& filename, DWORD loops, IWadSound** snd)
{
	Result rlt;

	*snd = NULL;

	WAVEFORMATEX fmt;
	DWORD numsamplesperchannel, wavdatasize;
	char* wavdata;
	CHKRESULT(LoadWAV(filename, &fmt, &numsamplesperchannel, &wavdata, &wavdatasize));

	WAVEHDR* wavehdr;
	CHKALLOC(wavehdr = new WAVEHDR());
	wavehdr->lpData = wavdata;
	wavehdr->dwBufferLength = wavdatasize;
	wavehdr->dwFlags = NULL;
	CHKRESULT(waveOutPrepareHeader(hdl, wavehdr, sizeof(WAVEHDR)));

	if(loops)
	{
		wavehdr->dwFlags |= WHDR_BEGINLOOP | WHDR_ENDLOOP;
		wavehdr->dwLoops = loops;
	}

	Sound* newsnd;
	CHKALLOC(newsnd = new Sound(this, wavehdr, NULL)); // Edit: replace NULL with audio format of outdev
	wavehdr->dwUser = (DWORD_PTR)newsnd;

	sounds.push_back(newsnd);
	*snd = newsnd;

	return R_OK;
}

Result OutDevice::CreateStreamedSound(DWORD bufferlen, DWORD usedbufferlen, DWORD numbuffers,
									  const SOUNDDONECALLBACK ondonefunc, LPVOID user, IWadStreamedSound** snd)
{
	Result rlt;

	if(!usedbufferlen)
		usedbufferlen = bufferlen;
	*snd = NULL;

	WAVEHDR* wavehdrs;
	CHKALLOC(wavehdrs = new WAVEHDR[numbuffers]);
	for(DWORD i = 0; i < numbuffers; i++)
	{
		CHKALLOC(wavehdrs[i].lpData = new char[bufferlen]);
		wavehdrs[i].dwBufferLength = usedbufferlen;
		wavehdrs[i].dwFlags = NULL;
		wavehdrs[i].dwUser = NULL;
		CHKRESULT(waveOutPrepareHeader(hdl, wavehdrs + i, sizeof(WAVEHDR)));
	}

	StreamedSound* newsnd;
	CHKALLOC(newsnd = new StreamedSound(this, wavehdrs, numbuffers, bufferlen, NULL, ondonefunc, user)); // Edit: replace NULL with audio format of outdev

	for(DWORD i = 0; i < numbuffers; i++)
		wavehdrs[i].dwUser = (DWORD_PTR)newsnd;

	streamedsounds.push_back(newsnd);
	*snd = newsnd;

//cle->Print((int)mixerGetNumDevs());
MMRESULT mmr;
UINT numdevs = mixerGetNumDevs();
/*MIXERCAPS caps;
for(int i = 0; i < numdevs; i++)
{
	mmr = mixerGetDevCaps(i, &caps, sizeof(MIXERCAPS));
	int abc = 0;
}*/

/*HMIXER hmx;
mmr = mixerOpen(&hmx, (UINT)hdl, NULL, NULL, MIXER_OBJECTF_HWAVEOUT);

MIXERLINE mixline;
mixline.cbStruct = sizeof(MIXERLINE);
mixline.dwDestination = 0;
mmr = mixerGetLineInfo((HMIXEROBJ)hmx, &mixline, MIXER_GETLINEINFOF_DESTINATION | MIXER_OBJECTF_HMIXER);

mmr = mixerClose(hmx);*/

	return R_OK;
}

Result InDevice::CreateStreamedRecorder(DWORD bufferlen, DWORD usedbufferlen, DWORD numbuffers,
										const RECORDERDONECALLBACK ondonefunc, LPVOID user, IWadStreamedRecorder** rec)
{
	Result rlt;

	if(!usedbufferlen)
		usedbufferlen = bufferlen;
	*rec = NULL;

	WAVEHDR* wavehdrs;
	CHKALLOC(wavehdrs = new WAVEHDR[numbuffers]);
	for(DWORD i = 0; i < numbuffers; i++)
	{
		CHKALLOC(wavehdrs[i].lpData = new char[bufferlen]);
		wavehdrs[i].dwBufferLength = usedbufferlen;
		wavehdrs[i].dwFlags = NULL;
		wavehdrs[i].dwUser = NULL;
		CHKRESULT(waveInPrepareHeader(hdl, wavehdrs + i, sizeof(WAVEHDR)));
	}

	StreamedRecorder* newrec;
	CHKALLOC(newrec = new StreamedRecorder(this, wavehdrs, numbuffers, bufferlen, NULL, ondonefunc, user)); // Edit: replace NULL with audio format of outdev

	for(DWORD i = 0; i < numbuffers; i++)
		wavehdrs[i].dwUser = (DWORD_PTR)newrec;

	streamedrecorder.push_back(newrec);
	*rec = newrec;

	return R_OK;
}

WORD OutDevice::GetVolume()
{
	DWORD vol;
	if(waveOutGetVolume(hdl, &vol) == MMSYSERR_NOERROR)
	{
		if(!(caps.dwSupport & WAVECAPS_LRVOLUME))
			return (WORD)((vol & 0xFFFF) + ((vol >> 16) & 0xFFFF) / 2); // Return average of left and right volume
		else
			return LOWORD(vol); // Return left volume
	}
	else
		return 0;
}
void OutDevice::SetVolume(WORD val)
{
	if(!(caps.dwSupport & WAVECAPS_VOLUME))
		return; // No volume control support

	waveOutSetVolume(hdl, (DWORD)val | ((DWORD)val << 16));
}
WORD OutDevice::GetLVolume()
{
	DWORD vol;
	if(waveOutGetVolume(hdl, &vol) == MMSYSERR_NOERROR)
		return LOWORD(vol); // Return left volume
	else
		return 0;
}
void OutDevice::SetLVolume(WORD val)
{
	if(!(caps.dwSupport & WAVECAPS_VOLUME))
		return; // No volume control support

	DWORD vol;
	if(waveOutGetVolume(hdl, &vol) == MMSYSERR_NOERROR)
	{
		vol = (DWORD)val | (vol & 0xFFFF0000); // Set left volume
		waveOutSetVolume(hdl, vol);
	}
}
WORD OutDevice::GetRVolume()
{
	DWORD vol;
	if(waveOutGetVolume(hdl, &vol) == MMSYSERR_NOERROR)
		return HIWORD(vol); // Return right volume
	else
		return 0;
}
void OutDevice::SetRVolume(WORD val)
{
	if(!(caps.dwSupport & WAVECAPS_VOLUME))
		return; // No volume control support
	if(!(caps.dwSupport & WAVECAPS_LRVOLUME))
		return SetVolume(val); // No left/right volume control support

	DWORD vol;
	if(waveOutGetVolume(hdl, &vol) == MMSYSERR_NOERROR)
	{
		vol = (vol & 0xFFFF) | ((DWORD)val << 16); // Set right volume
		waveOutSetVolume(hdl, vol);
	}
}

//-----------------------------------------------------------------------------
// Name: Pause()
// Desc: Pause all sounds of the output device
//-----------------------------------------------------------------------------
void OutDevice::Pause()
{
	waveOutPause(hdl);
}

//-----------------------------------------------------------------------------
// Name: Resume()
// Desc: Resume all paused sounds of the output device
//-----------------------------------------------------------------------------
void OutDevice::Resume()
{
	waveOutRestart(hdl);
}

Result Sound::Play()
{
	return Error(waveOutWrite(device->hdl, header, sizeof(WAVEHDR)));
}

void StreamedSound::ClearBuffers()
{
	for(DWORD i = 0; i < numbuffers; i++)
	{
		ZeroMemory(headers[i].lpData, headers[i].dwBufferLength);
		headers[i].dwFlags |= WHDR_DONE;
	}
}

void StreamedSound::Play()
{
	LeaveCriticalSection(&donefuncsection);
	state = SS_PLAYING;
	SetEvent(streamingevent);
}

void StreamedSound::Stop()
{
	EnterCriticalSection(&donefuncsection);
	state = SS_STOPPED;
	waveOutReset(device->hdl);
}

void StreamedSound::OnData()
{
	if(state == SS_NODATA)
	{
		state = SS_PLAYING;
		SetEvent(streamingevent);
	}
}

void StreamedRecorder::Record()
{
	state = SS_RECORDING;
	for(DWORD i = 0; i < numbuffers; i++)
		waveInAddBuffer(device->hdl, &headers[i], sizeof(WAVEHDR));
	//state = SS_RECORDING;
	//SetEvent(streamingevent);
	waveInStart(device->hdl);
}

void StreamedRecorder::Stop()
{
	state = SS_STOPPED;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void OutDevice::Release()
{
	parent->outdevs.remove(this);

	// End streaming
	for(std::list<StreamedSound*>::iterator iter = streamedsounds.begin(); iter != streamedsounds.end(); iter++)
	{
		(*iter)->state = StreamedSound::SS_CLOSING;
		SetEvent((*iter)->streamingevent);
		WaitForSingleObject((*iter)->streamingthread, INFINITE);
		CloseHandle((*iter)->streamingevent);
		CloseHandle((*iter)->streamingthread);
		DeleteCriticalSection(&(*iter)->donefuncsection);
	}

	waveOutReset(hdl);

	for(std::list<Sound*>::iterator iter = sounds.begin(); iter != sounds.end(); iter++)
	{
		waveOutUnprepareHeader(hdl, (*iter)->header, sizeof(WAVEHDR));
		REMOVE_ARRAY((*iter)->header->lpData);
		REMOVE((*iter)->header);
	}
	sounds.clear();

	for(std::list<StreamedSound*>::iterator iter = streamedsounds.begin(); iter != streamedsounds.end(); iter++)
	{
		for(DWORD i = 0; i < (*iter)->numbuffers; i++)
		{
			waveOutUnprepareHeader(hdl, (*iter)->headers + i, sizeof(WAVEHDR));
			REMOVE_ARRAY((*iter)->headers[i].lpData);
		}
		REMOVE_ARRAY((*iter)->headers);
	}
	streamedsounds.clear();

	waveOutClose(hdl);

	delete this;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void InDevice::Release()
{
	parent->indevs.remove(this);

	// End streaming
	for(std::list<StreamedRecorder*>::iterator iter = streamedrecorder.begin(); iter != streamedrecorder.end(); iter++)
	{
		(*iter)->state = StreamedRecorder::SS_CLOSING;
		SetEvent((*iter)->streamingevent);
		WaitForSingleObject((*iter)->streamingthread, INFINITE);
		CloseHandle((*iter)->streamingevent);
		CloseHandle((*iter)->streamingthread);
	}

	waveInReset(hdl);

	for(std::list<StreamedRecorder*>::iterator iter = streamedrecorder.begin(); iter != streamedrecorder.end(); iter++)
	{
		for(DWORD i = 0; i < (*iter)->numbuffers; i++)
		{
			waveInUnprepareHeader(hdl, (*iter)->headers + i, sizeof(WAVEHDR));
			REMOVE_ARRAY((*iter)->headers[i].lpData);
		}
		REMOVE_ARRAY((*iter)->headers);
	}
	streamedrecorder.clear();

	waveInClose(hdl);

	delete this;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void WinAudio::Release()
{
	if(sysvolume)
	{
		if(sysvolumecbkclass.cbk)
			sysvolume->UnregisterControlChangeNotify(&sysvolumecbkclass);
		sysvolume->Release();
	}

	while(outdevs.size())
		outdevs.front()->Release();
	//for(std::list<OutDevice*>::iterator iter = outdevs.begin(); iter != outdevs.end(); iter++)
	//	(*iter)->Release();
	//outdevs.clear();
	while(indevs.size())
		indevs.front()->Release();
	//for(std::list<InDevice*>::iterator iter = indevs.begin(); iter != indevs.end(); iter++)
	//	(*iter)->Release();
	//indevs.clear();

	delete this;
}