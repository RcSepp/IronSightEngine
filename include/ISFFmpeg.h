#ifndef __ISFFMPEG_H
#define __ISFFMPEG_H

#include <ISEngine.h>

extern "C"
{
	#include <libavcodec\\avcodec.h>
	#include <libavformat\\avformat.h>
	#include <libswscale\\swscale.h>
	#include <libavutil\\mem.h>
}


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_FFMPEG
	#define FFMPEG_EXTERN_FUNC		__declspec(dllexport)
#else
	#define FFMPEG_EXTERN_FUNC		__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
class IMediaFile;
typedef void (__stdcall* CLOCKTICK_CALLBACK)(IMediaFile* sender, int seconds, LPVOID user);


//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
#ifndef AUDIO_FORMAT_DEFINED
#define AUDIO_FORMAT_DEFINED
/*struct AudioFormat
{
	DWORD samplerate;
	WORD channels, bits;

	AudioFormat() {}
	AudioFormat(DWORD samplerate, WORD channels, WORD bits)
	{
		this->samplerate = samplerate;
		this->channels = channels;
		this->bits = bits;
	}
	WAVEFORMATEX* GetWaveFormat(WAVEFORMATEX* result) const
	{
		result->cbSize = 0;
		result->wFormatTag = WAVE_FORMAT_PCM;
		result->nChannels = channels;
		result->nSamplesPerSec = samplerate;
		result->wBitsPerSample = bits;
		result->nBlockAlign = result->nChannels * result->wBitsPerSample / 8;
		result->nAvgBytesPerSec = result->nSamplesPerSec * result->nBlockAlign;

		return result;
	}
};*/
struct AudioFormat
{
	DWORD samplerate, channelmask;
	WORD channels, bits;
	GUID subformat;

	AudioFormat() {}
	AudioFormat(DWORD samplerate, WORD bits, WORD channels, GUID subformat = KSDATAFORMAT_SUBTYPE_PCM)
	{
		this->samplerate = samplerate;
		this->bits = bits;
		this->channelmask = SPEAKER_ALL;
		this->channels = channels;
		this->subformat = subformat;
	}
	static AudioFormat* FromChannelMask(DWORD samplerate, WORD bits, DWORD channelmask, GUID subformat = KSDATAFORMAT_SUBTYPE_PCM)
	{
		AudioFormat* fmt = new AudioFormat();
		if(!fmt)
			return NULL;

		fmt->samplerate = samplerate;
		fmt->bits = bits;
		fmt->channelmask = channelmask;
		fmt->subformat = subformat;

		// Count speakers
		fmt->channels = 0;
		for(DWORD i = 0; i <= 30; i++)
			fmt->channels += (channelmask >> i) & 1;

		// Default to stereo
		if(fmt->channels == 0)
			fmt->channels = 2;

		return fmt;
	}
	WAVEFORMATEXTENSIBLE* GetWaveFormat(WAVEFORMATEXTENSIBLE* result) const
	{
		result->Format.cbSize = 22;
		result->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		result->Format.nChannels = channels;
		result->Format.nSamplesPerSec = samplerate;
		result->Format.wBitsPerSample = (bits % 8 == 0) ? bits : ((bits / 8 + 1) * 8);
		result->Format.nBlockAlign = result->Format.nChannels * result->Format.wBitsPerSample / 8;
		result->Format.nAvgBytesPerSec = result->Format.nSamplesPerSec * result->Format.nBlockAlign;

		result->Samples.wValidBitsPerSample = bits;
		result->dwChannelMask = channelmask;
		result->SubFormat = subformat;

		return result;
	}
	WAVEFORMATEX* GetWaveFormat(WAVEFORMATEX* result) const
	{
		result->cbSize = 0;
		result->wFormatTag = WAVE_FORMAT_PCM; // Formats other than WAVE_FORMAT_PCM are not supported in WAVEFORMATEX
		result->nChannels = channels;
		result->nSamplesPerSec = samplerate;
		result->wBitsPerSample = (bits % 8 == 0) ? bits : ((bits / 8 + 1) * 8);
		result->nBlockAlign = result->nChannels * result->wBitsPerSample / 8;
		result->nAvgBytesPerSec = result->nSamplesPerSec * result->nBlockAlign;

		return result;
	}
	bool operator==(const AudioFormat& other) const
	{
		return (this->samplerate == other.samplerate && this->channelmask == other.channelmask && this->bits == other.bits && this->subformat == other.subformat);
	}
	bool operator!=(const AudioFormat& other) const
	{
		return (this->samplerate != other.samplerate || this->channelmask != other.channelmask || this->bits != other.bits || this->subformat != other.subformat);
	}
};
#endif


//-----------------------------------------------------------------------------
// Name: IMediaFile
// Desc: Interface to the MediaFile class
//-----------------------------------------------------------------------------
typedef class IMediaFile
{
public:
	virtual UINT GetNumberOfStreams(AVMediaType streamtype) = 0;
	virtual void EnableStream(UINT streamindex, AVMediaType streamtype) = 0;
	virtual void DisableStream(UINT streamindex, AVMediaType streamtype) = 0;
	virtual AudioFormat* GetAudioFormat(UINT streamindex) = 0;
	virtual int GetVideoWidth(UINT streamindex) = 0;
	virtual int GetVideoHeight(UINT streamindex) = 0;
	virtual double GetAudioDuration(UINT streamindex) = 0;
	virtual int64_t GetAudioFrame(UINT streamindex) = 0;
	virtual double GetAudioTime(UINT streamindex) = 0;
	virtual int64_t GetVideoFrame(UINT streamindex) = 0;
	virtual void SetAudioFrame(UINT streamindex, int64_t frame) = 0;
	virtual void SetAudioTime(UINT streamindex, double time) = 0;
	virtual void SetVideoFrame(UINT streamindex, int64_t frame) = 0;
	virtual void ScrollAudioFrame(UINT streamindex, int64_t frames) = 0;
	virtual void ScrollAudioTime(UINT streamindex, double dt) = 0;

	virtual Result CreateFrame(UINT streamindex, uint8_t **buffer, AVFrame** frame) const = 0;
	virtual Result CreateRescaler(UINT streamindex, int destwidth, int destheight, PixelFormat destpixfmt, int flags, SwsContext** context) const = 0;
	virtual Result PrepareVideoStreaming(UINT streamidx, const Size<int>* destsize, PixelFormat destpixfmt,
										 int swscontextflags, uint8_t** framedata) = 0;
	virtual Result StreamContent(float dt, bool *finished) = 0;
	virtual Result StreamAudioDirect(bool *finished, int16_t* audiodata, int* audiodatalen) = 0;
	virtual Result ReadRawPacket(AVPacket* packet, AVMediaType* codectype, bool *finished) = 0;
	virtual AVMediaType GetPacketMediaType(const AVPacket* packet) const = 0;
	virtual Result DecodeAudioPacket(AVPacket* packet, int16_t* audiodata, int* audiodatalen, double* time) = 0;
	virtual Result DecodeVideoPacket(AVPacket* packet, AVFrame* frame, bool* framedone, double* time) = 0;
	virtual void RescaleVideoFrame(SwsContext* convertctx, const AVPacket* packet, AVFrame* srcframe, AVFrame* destframe) = 0;
	virtual Result WriteRawPacket(AVPacket* packet) = 0;
	virtual void EndVideoStreaming(UINT streamidx) = 0;
	virtual void Release() = 0;
}* LPMEDIAFILE;

//-----------------------------------------------------------------------------
// Name: IFFmpeg
// Desc: Interface to the FFmpeg class
//-----------------------------------------------------------------------------
typedef class IFFmpeg
{
public:

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init() = 0;

	virtual void SetMouseDownCallback(const CLOCKTICK_CALLBACK clockcbk, LPVOID user) = 0;

	virtual Result CreateNew(FilePath path, IMediaFile** file, String* ext = NULL) = 0;
	virtual Result LoadFile(FilePath path, IMediaFile** file) = 0;
	virtual void CloseFile(IMediaFile* file) = 0;

	virtual Result PrepareBlittingToWnd(HWND wndtarget, const Rect<int>* destrect) = 0;
	virtual void BlitToWnd(uint8_t* framedata) = 0;
	virtual void EndBlittingToWnd() = 0;

	virtual Result CreateFrame(int width, int height, PixelFormat pixfmt, uint8_t **buffer, AVFrame** frame) const = 0;
	virtual Result CreateRescaler(int srcwidth, int srcheight, PixelFormat srcpixfmt, int destwidth,
								  int destheight, PixelFormat destpixfmt, int flags, SwsContext** context) const = 0;
	virtual void FreePacket(AVPacket* packet) const = 0;

	virtual void Release() = 0;
}* LPFFMPEG;

extern "C" FFMPEG_EXTERN_FUNC LPFFMPEG __cdecl CreateFFmpeg();
extern "C" FFMPEG_EXTERN_FUNC bool __cdecl CheckFFmpegSupport(LPTSTR* missingdllname);

#endif