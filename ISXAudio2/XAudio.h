#ifndef __XAUDIO2_H
#define __XAUDIO2_H

#include "ISXAudio2.h"
#include "..\\global\\HResult.h"

//#define WIN_8

#ifdef WIN_8
namespace DirectX
{
	#include <XAudio2.h>
};
#pragma comment(lib,"XAudio2.lib")
#else
namespace DirectX
{
	#include "D:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\XAudio2.h"
};
#endif
#pragma comment(lib,"ole32.lib")
//#include <X3DAudio.h>


class XAudio;
class Sound : public IXA2Sound
{
private:
	DirectX::IXAudio2SourceVoice* voice;
	DirectX::XAUDIO2_BUFFER* buffer;

public:
	XAudio* parent;

	Sound(XAudio* parent, const AudioFormat& format) : IXA2Sound(format), voice(NULL), buffer(NULL), parent(parent) {}
	Result Init(const BYTE* wavdata, UINT wavdatasize);
	Result Play();
	Result Pause();
	Result Stop();
	float GetVolume() {float val; voice->GetVolume(&val); return val;}
	void SetVolume(float val) {voice->SetVolume(min(abs(val), XAUDIO2_MAX_VOLUME_LEVEL), XAUDIO2_COMMIT_NOW);}
	float GetPitch() {float val; voice->GetFrequencyRatio(&val); return val;}
	void SetPitch(float val) {voice->SetFrequencyRatio(val, XAUDIO2_COMMIT_NOW);}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: XAudio
// Desc: API to the DirectX XAudio2 library
//-----------------------------------------------------------------------------
class XAudio : public IXAudio2
{
private:
	DirectX::IXAudio2MasteringVoice* vcemaster;
	DirectX::IXAudio2SourceVoice* vcestream;
	DirectX::XAUDIO2_BUFFER* xaudiobuffers;
	short* bufstream;
	bool streamingaudio;

public:
	DirectX::IXAudio2* lpxad;
	DWORD numchannels, samplesperbuffer;
	std::list<Sound*> sounds;

	XAudio();
	void Sync(GLOBALVARDEF_LIST);
	Result Init();
	Result CreateSound(const FilePath& filename, LPXA2SOUND* snd);
	Sound* CreateSoundFromFileWrapper(const FilePath& filename)
	{
		Sound* snd;
		CreateSound(filename, (LPXA2SOUND*)&snd);
		return snd;
	}
	void Update();
	/*Result GetSupportedOutputFormats(UINT deviceid, AudioFormat** formats) const;
	Result QueryOutputFormat(UINT deviceid, const AudioFormat* format);
	Result OpenOutputDevice(UINT deviceid, const AudioFormat* format, IWadOutDevice** outdev);
	Result QueryInputFormat(UINT deviceid, const AudioFormat* format);
	Result OpenInputDevice(UINT deviceid, const AudioFormat* format, IWadInDevice** indev);*/
	float GetVolume() {float val; vcemaster->GetVolume(&val); return val;}
	void SetVolume(float val) {vcemaster->SetVolume(min(abs(val), XAUDIO2_MAX_VOLUME_LEVEL), XAUDIO2_COMMIT_NOW);}
	void Release();
};

void SyncWithPython();

#endif