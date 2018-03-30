#ifndef __ISWINAUDIO_H
#define __ISWINAUDIO_H

#include <ISEngine.h>
#include <ISAudioFormat.h>

//#include <mmsystem.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_WINAUDIO
	#define WINAUDIO_EXTERN_FUNC	__declspec(dllexport)
#else
	#define WINAUDIO_EXTERN_FUNC	__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// UNIONS
//-----------------------------------------------------------------------------
union AudioDataPtr
{
	char* cdata;
	short* sdata;
};


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum SoundDoneCallbackResult
	{SDCR_OK, SDCR_FINISHED, SDCR_NODATA};


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
class IWadStreamedSound;
class IWadStreamedRecorder;
typedef SoundDoneCallbackResult (__stdcall* SOUNDDONECALLBACK)(IWadStreamedSound* snd, AudioDataPtr data, LPVOID user);
typedef void (__stdcall* SOUNDFINISHEDCALLBACK)(IWadStreamedSound* snd, LPVOID user);
typedef void (__stdcall* RECORDERDONECALLBACK)(IWadStreamedRecorder* rec, AudioDataPtr data, LPVOID user);

typedef void (__stdcall* VOLUMELEVELCHANGEDCALLBACK)(float volume, LPVOID user);

class WadPlayable
{
public:
	const enum SoundType
		{ST_SOUND, ST_STREAMEDSOUND} type;
	WadPlayable(SoundType type) : type(type) {}
};
class WadRecordable
{
public:
	const enum RecorderType
		{ST_STREAMEDRECORDER} type;
	WadRecordable(RecorderType type) : type(type) {}
};


//-----------------------------------------------------------------------------
// Name: IWadSound
// Desc: Interface to the Sound class
//-----------------------------------------------------------------------------
typedef class IWadSound : public WadPlayable
{
public:
	AudioDataPtr data;
	const DWORD datalen;
	const AudioFormat* format;

	IWadSound(WAVEHDR* header, const AudioFormat* format) : WadPlayable(ST_SOUND), datalen(header->dwBufferLength)
	{
		this->data.cdata = header->lpData;
		ZeroMemory(this->data.cdata, datalen);
		this->format = format;
	}
	virtual Result Play() = 0;
}* LPWADSOUND;

//-----------------------------------------------------------------------------
// Name: IWadStreamedSound
// Desc: Interface to a streamed sound
//-----------------------------------------------------------------------------
typedef class IWadStreamedSound : public WadPlayable
{
public:
	const DWORD bufferlen, usedbufferlen, numbuffers;
	const AudioFormat* format;

	IWadStreamedSound(WAVEHDR* headers, DWORD numheaders, DWORD bufferlen, const AudioFormat* format) : WadPlayable(ST_STREAMEDSOUND), usedbufferlen(headers[0].dwBufferLength),
																										numbuffers(numheaders), bufferlen(bufferlen)
	{
		this->format = format;
	}
	virtual void RegisterFinishedCallback(SOUNDFINISHEDCALLBACK cbk, LPVOID user) = 0;
	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual void OnData() = 0;
}* LPWADSTREAMEDSOUND;

//-----------------------------------------------------------------------------
// Name: IWadStreamedRecorder
// Desc: Interface to a streamed recorder
//-----------------------------------------------------------------------------
typedef class IWadStreamedRecorder : public WadRecordable
{
public:
	const DWORD bufferlen, usedbufferlen, numbuffers;
	const AudioFormat* format;

	IWadStreamedRecorder(WAVEHDR* headers, DWORD numheaders, DWORD bufferlen, const AudioFormat* format) : WadRecordable(ST_STREAMEDRECORDER), usedbufferlen(headers[0].dwBufferLength),
																										   numbuffers(numheaders), bufferlen(bufferlen)
	{
		this->format = format;
	}
	virtual void Record() = 0;
	virtual void Stop() = 0;
}* LPWADSTREAMEDRECORDER;

//-----------------------------------------------------------------------------
// Name: IWadOutDevice
// Desc: Interface to the OutDevice class
//-----------------------------------------------------------------------------
typedef class IWadOutDevice
{
public:

	virtual Result CreateSound(DWORD bufferlen, DWORD loops, IWadSound** snd) = 0;
	virtual Result CreateSound(const FilePath& filename, DWORD loops, IWadSound** snd) = 0;
	virtual Result CreateStreamedSound(DWORD bufferlen, DWORD usedbufferlen, DWORD numbuffers,
									   const SOUNDDONECALLBACK ondonefunc, LPVOID user, IWadStreamedSound** snd) = 0;
	virtual WORD GetVolume() = 0;
	virtual void SetVolume(WORD val) = 0;
	__declspec(property(get=GetVolume, put=SetVolume)) WORD Volume;
	virtual WORD GetLVolume() = 0;
	virtual void SetLVolume(WORD val) = 0;
	__declspec(property(get=GetLVolume, put=SetLVolume)) WORD LVolume;
	virtual WORD GetRVolume() = 0;
	virtual void SetRVolume(WORD val) = 0;
	__declspec(property(get=GetRVolume, put=SetRVolume)) WORD RVolume;
	virtual void Pause() = 0;
	virtual void Resume() = 0;
	virtual void Release() = 0;
}* LPWADOUTDEVICE;

//-----------------------------------------------------------------------------
// Name: IWadInDevice
// Desc: Interface to the InDevice class
//-----------------------------------------------------------------------------
typedef class IWadInDevice
{
public:

	virtual Result CreateStreamedRecorder(DWORD bufferlen, DWORD usedbufferlen, DWORD numbuffers,
										  const RECORDERDONECALLBACK ondonefunc, LPVOID user, IWadStreamedRecorder** rec) = 0;
	/*virtual WORD GetVolume() = 0;
	virtual void SetVolume(WORD val) = 0;
	__declspec(property(get=GetVolume, put=SetVolume)) WORD Volume;
	virtual WORD GetLVolume() = 0;
	virtual void SetLVolume(WORD val) = 0;
	__declspec(property(get=GetLVolume, put=SetLVolume)) WORD LVolume;
	virtual WORD GetRVolume() = 0;
	virtual void SetRVolume(WORD val) = 0;
	__declspec(property(get=GetRVolume, put=SetRVolume)) WORD RVolume;
	virtual void Pause() = 0;
	virtual void Resume() = 0;*/
	virtual void Release() = 0;
}* LPWADINDEVICE;

//-----------------------------------------------------------------------------
// Name: IWinAudio
// Desc: Interface to the WinAudio class
//-----------------------------------------------------------------------------
typedef class IWinAudio
{
public:

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init() = 0;
	virtual Result GetSupportedOutputFormats(UINT deviceid, AudioFormat** formats, int* numformats) const = 0;
	virtual Result QueryOutputFormat(UINT deviceid, const AudioFormat* format) = 0;
	virtual Result OpenOutputDevice(UINT deviceid, const AudioFormat* format, IWadOutDevice** outdev) = 0;
	virtual Result QueryInputFormat(UINT deviceid, const AudioFormat* format) = 0;
	virtual Result OpenInputDevice(UINT deviceid, const AudioFormat* format, IWadInDevice** indev) = 0;
	virtual void SetVolumeChangedCallback(VOLUMELEVELCHANGEDCALLBACK cbk, LPVOID user) = 0;
	virtual float GetVolume() = 0;
	virtual void SetVolume(float val) = 0;
	__declspec(property(get=GetVolume, put=SetVolume)) float Volume;
	virtual float GetChannelVolume(UINT channel) = 0;
	virtual void SetChannelVolume(UINT channel, float val) = 0;
	virtual void Release() = 0;
}* LPWINAUDIO;

extern "C" WINAUDIO_EXTERN_FUNC LPWINAUDIO __cdecl CreateWinAudio();
extern "C" WINAUDIO_EXTERN_FUNC void __cdecl RegisterScriptableWinAudioClasses(const IPythonScriptEngine* pse);

#endif