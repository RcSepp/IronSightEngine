#ifndef __ISXAUDIO2_H
#define __ISXAUDIO2_H

#include <ISEngine.h>
#include <ISAudioFormat.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_XAUDIO2
	#define XAUDIO2_EXTERN_FUNC	__declspec(dllexport)
#else
	#define XAUDIO2_EXTERN_FUNC	__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// UNIONS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
/*typedef SoundDoneCallbackResult (__stdcall* SOUNDDONECALLBACK)(IWadStreamedSound* snd, AudioDataPtr data, LPVOID user);
typedef void (__stdcall* SOUNDFINISHEDCALLBACK)(IWadStreamedSound* snd, LPVOID user);
typedef void (__stdcall* RECORDERDONECALLBACK)(IWadStreamedRecorder* rec, AudioDataPtr data, LPVOID user);*/


//-----------------------------------------------------------------------------
// Name: IXa2Sound
// Desc: Interface to the Sound class
//-----------------------------------------------------------------------------
typedef class IXA2Sound
{
public:
	const AudioFormat format;

	IXA2Sound(const AudioFormat& format) : format(format) {}
	virtual Result Play() = 0;
	virtual Result Pause() = 0;
	virtual Result Stop() = 0;
	virtual void Release() = 0;
}* LPXA2SOUND;


//-----------------------------------------------------------------------------
// Name: IXAudio2
// Desc: Interface to the XAudio class
//-----------------------------------------------------------------------------
typedef class IXAudio2
{
public:
	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init() = 0;
	virtual Result CreateSound(const FilePath& filename, LPXA2SOUND* snd) = 0;
	virtual void Update() = 0;
	/*virtual Result GetSupportedOutputFormats(UINT deviceid, AudioFormat** formats) const = 0;
	virtual Result QueryOutputFormat(UINT deviceid, const AudioFormat* format) = 0;
	virtual Result OpenOutputDevice(UINT deviceid, const AudioFormat* format, IWadOutDevice** outdev) = 0;
	virtual Result QueryInputFormat(UINT deviceid, const AudioFormat* format) = 0;
	virtual Result OpenInputDevice(UINT deviceid, const AudioFormat* format, IWadInDevice** indev) = 0;*/
	virtual float GetVolume() = 0;
	virtual void SetVolume(float val) = 0;
	__declspec(property(get=GetVolume, put=SetVolume)) float Volume;
	virtual void Release() = 0;
}* LPXAUDIO2;

extern "C" XAUDIO2_EXTERN_FUNC LPXAUDIO2 __cdecl CreateXAudio2();
extern "C" XAUDIO2_EXTERN_FUNC void __cdecl RegisterScriptableXAudio2Classes(const IPythonScriptEngine* pse);

#endif