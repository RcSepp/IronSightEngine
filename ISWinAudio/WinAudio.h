#ifndef __WINAUDIO_H
#define __WINAUDIO_H

#include "ISWinAudio.h"
#include "..\\global\\MMResult.h"
#include <endpointvolume.h>

#pragma comment (lib, "winmm.lib")


class OutDevice;
class InDevice;
class WinAudio;
DWORD __stdcall OutStreamingThread(LPVOID args);
DWORD __stdcall InStreamingThread(LPVOID args);

class Sound : public IWadSound
{
public:
	WAVEHDR* header;
	const OutDevice* device;

	Sound(const OutDevice* device, WAVEHDR* header, const AudioFormat* format) : IWadSound(header, format), device(device)
	{
		this->header = header;
	}
	Result Play();
};

class StreamedSound : public IWadStreamedSound
{
private:
	const OutDevice* device;
	const SOUNDDONECALLBACK ondonefunc;
	SOUNDFINISHEDCALLBACK onfinished;
	LPVOID ondonefuncuser, onfinisheduser;

	static DWORD __stdcall OutStreamingThread(LPVOID args);
	void ClearBuffers();

public:
	WAVEHDR* headers;
	HANDLE streamingevent;
	HANDLE streamingthread;
	CRITICAL_SECTION donefuncsection;
	enum StreamingState
		{SS_STOPPED, SS_PLAYING, SS_NODATA, SS_CLOSING} state;

	StreamedSound(const OutDevice* device, WAVEHDR* headers, DWORD numheaders, DWORD bufferlen,
				  const AudioFormat* format, const SOUNDDONECALLBACK ondonefunc, LPVOID user) :
		IWadStreamedSound(headers, numheaders, bufferlen, format), device(device), ondonefunc(ondonefunc), ondonefuncuser(user)
	{
		this->headers = headers;

		onfinished = NULL;
		state = SS_STOPPED;
		streamingthread = CreateThread(NULL, 0, OutStreamingThread, this, 0, NULL);
		streamingevent = CreateEvent(NULL, FALSE, FALSE, NULL);
		InitializeCriticalSection(&donefuncsection);
	}
		void RegisterFinishedCallback(SOUNDFINISHEDCALLBACK cbk, LPVOID user) {onfinished = cbk; onfinisheduser = user;}
	void Play();
	void Stop();
	void OnData();
};

class StreamedRecorder : public IWadStreamedRecorder
{
public:
	WAVEHDR* headers;
	const InDevice* device;
	const RECORDERDONECALLBACK ondonefunc;
	LPVOID ondonefuncuser;
	HANDLE streamingthread;
	HANDLE streamingevent;
	enum StreamingState
		{SS_STOPPED, SS_RECORDING, SS_CLOSING} state;

	StreamedRecorder(const InDevice* device, WAVEHDR* headers, DWORD numheaders, DWORD bufferlen,
					 const AudioFormat* format, const RECORDERDONECALLBACK ondonefunc, LPVOID user) :
		IWadStreamedRecorder(headers, numheaders, bufferlen, format), device(device), ondonefunc(ondonefunc), ondonefuncuser(user)
	{
		this->headers = headers;

		state = SS_STOPPED;
		streamingthread = CreateThread(NULL, 0, InStreamingThread, this, 0, NULL);
		streamingevent = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	void Record();
	void Stop();
};

//-----------------------------------------------------------------------------
// Name: OutDevice
// Desc: Wrapper class for a single waveform audio output device
//-----------------------------------------------------------------------------
class OutDevice : public IWadOutDevice
{
private:
	std::list<Sound*> sounds;
	std::list<StreamedSound*> streamedsounds;
	WAVEOUTCAPS caps;
	WinAudio* parent;

public:
	HWAVEOUT hdl;

	OutDevice(HWAVEOUT devicehandle, WinAudio* parent)
	{
		this->hdl = devicehandle;
		this->parent = parent;
		waveOutGetDevCaps((UINT_PTR)devicehandle, &caps, sizeof(WAVEOUTCAPS));
	}
	Result CreateSound(DWORD bufferlen, DWORD loops, IWadSound** snd);
	Result CreateSound(const FilePath& filename, DWORD loops, IWadSound** snd);
	Sound* CreateSoundFromFileWrapper(const FilePath& filename, DWORD loops)
	{
		Sound* snd;
		CreateSound(filename, loops, (LPWADSOUND*)&snd);
		return snd;
	}
	Result CreateStreamedSound(DWORD bufferlen, DWORD usedbufferlen, DWORD numbuffers,
							   const SOUNDDONECALLBACK ondonefunc, LPVOID user, IWadStreamedSound** snd);
	WORD GetVolume();
	void SetVolume(WORD val);
	WORD GetLVolume();
	void SetLVolume(WORD val);
	WORD GetRVolume();
	void SetRVolume(WORD val);
	void Pause();
	void Resume();
	void Release();
};

//-----------------------------------------------------------------------------
// Name: InDevice
// Desc: Wrapper class for a single waveform audio input device
//-----------------------------------------------------------------------------
class InDevice : public IWadInDevice
{
private:
	std::list<StreamedRecorder*> streamedrecorder;
	WAVEINCAPS caps;
	WinAudio* parent;

public:
	HWAVEIN hdl;

	InDevice(HWAVEIN devicehandle, WinAudio* parent)
	{
		this->hdl = devicehandle;
		this->parent = parent;
		waveInGetDevCaps((UINT_PTR)devicehandle, &caps, sizeof(WAVEOUTCAPS));
	}
	Result CreateStreamedRecorder(DWORD bufferlen, DWORD usedbufferlen, DWORD numbuffers,
								  const RECORDERDONECALLBACK ondonefunc, LPVOID user, IWadStreamedRecorder** rec);
	/*WORD GetVolume();
	void SetVolume(WORD val);
	WORD GetLVolume();
	void SetLVolume(WORD val);
	WORD GetRVolume();
	void SetRVolume(WORD val);
	void Pause();
	void Resume();*/
	void Release();
};

//-----------------------------------------------------------------------------
// Name: WinAudio
// Desc: API to waveform audio of windows multimedia library. Mainly used to open devices.
//-----------------------------------------------------------------------------
class WinAudio : public IWinAudio
{
private:
	HWAVEIN indev;
	String *outdevnames, *indevnames;
	UINT numindevs, numoutdevs;
	IAudioEndpointVolume* sysvolume;

	class AudioEndpointVolumeCallback : public IAudioEndpointVolumeCallback
	{
	private:
		ULONG refcounter;

	public:
		VOLUMELEVELCHANGEDCALLBACK cbk;
		LPVOID user;

		HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);

		HRESULT STDMETHODCALLTYPE QueryInterface(const IID &,void **);
		ULONG STDMETHODCALLTYPE AddRef();
		ULONG STDMETHODCALLTYPE Release();
	} sysvolumecbkclass;

public:
	std::list<OutDevice*> outdevs;
	std::list<InDevice*> indevs;

	WinAudio();
	void Sync(GLOBALVARDEF_LIST);
	Result Init();
	Result GetSupportedOutputFormats(UINT deviceid, AudioFormat** formats, int* numformats) const;
	Result QueryOutputFormat(UINT deviceid, const AudioFormat* format);
	Result OpenOutputDevice(UINT deviceid, const AudioFormat* format, IWadOutDevice** outdev);
	OutDevice* OpenOutputDeviceWrapper(UINT deviceid, const AudioFormat* format)
	{
		OutDevice* outdev;
		OpenOutputDevice(deviceid, format, (LPWADOUTDEVICE*)&outdev);
		return outdev;
	}
	Result QueryInputFormat(UINT deviceid, const AudioFormat* format);
	Result OpenInputDevice(UINT deviceid, const AudioFormat* format, IWadInDevice** indev);
	InDevice* OpenInputDeviceWrapper(UINT deviceid, const AudioFormat* format)
	{
		InDevice* indev;
		OpenInputDevice(deviceid, format, (LPWADINDEVICE*)&indev);
		return indev;
	}
	void SetVolumeChangedCallback(VOLUMELEVELCHANGEDCALLBACK cbk, LPVOID user);
	float GetVolume();
	void SetVolume(float val);
	float GetChannelVolume(UINT channel);
	void SetChannelVolume(UINT channel, float val);
	void Release();
};

void SyncWithPython();

#endif