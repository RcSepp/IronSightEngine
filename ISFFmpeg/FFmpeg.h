#ifndef __FFMPEG_H
#define __FFMPEG_H

#include "ISFFmpeg.h"

#pragma comment (lib, "avcodec.lib")
#pragma comment (lib, "avformat.lib")
#pragma comment (lib, "avutil.lib")
#pragma comment (lib, "swscale.lib")

#pragma comment(lib, "DelayImp.lib")


class FFmpeg;


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#define AVREMOVE(ptr)			{if(ptr) {av_free(ptr); (ptr) = NULL;}}


//-----------------------------------------------------------------------------
// Name: MediaFile
// Desc: Representation of an audio and/or video file in memory
//-----------------------------------------------------------------------------
class MediaFile : public IMediaFile
{
private:
	FFmpeg* parent;
	FilePath path;
	enum FileType
		{FT_NONE, FT_INPUT, FT_OUTPUT} type;
	bool ischanged;

	AVFormatContext* formatctx;
	struct Stream
	{
		int absolutestreamidx;
		AVStream* stream;
		AVCodecContext* codecctx;
		AVCodec *encoder, *decoder;
		bool enabled;
	} **videostreams, **audiostreams;
	UINT numvideostreams, numaudiostreams;
	UINT* streamidxmap;
	HANDLE readmutex;

public:
	Result New(FilePath path, String* ext = NULL);
	Result Load(FilePath path);
	Result SaveAs(FilePath path);
	Result Save();
	void Close();

	UINT GetNumberOfStreams(AVMediaType streamtype);
	void EnableStream(UINT streamindex, AVMediaType streamtype);
	void DisableStream(UINT streamindex, AVMediaType streamtype);
	AudioFormat* GetAudioFormat(UINT streamindex);
	int GetVideoWidth(UINT streamindex);
	int GetVideoHeight(UINT streamindex);
	double GetAudioDuration(UINT streamindex);
	int64_t GetAudioFrame(UINT streamindex);
	double GetAudioTime(UINT streamindex);
	int64_t GetVideoFrame(UINT streamindex);
	void SetAudioFrame(UINT streamindex, int64_t frame);
	void SetAudioTime(UINT streamindex, double time);
	void SetVideoFrame(UINT streamindex, int64_t frame);
	void ScrollAudioFrame(UINT streamindex, int64_t frames);
	void ScrollAudioTime(UINT streamindex, double dt);

	MediaFile(FFmpeg* parent) : parent(parent)
	{
		type = FT_NONE;
		formatctx = NULL;
		videostreams = audiostreams = NULL;
		numvideostreams = numaudiostreams = 0;
		streamidxmap = NULL;
		readmutex = CreateMutex(NULL, FALSE, NULL);
	}

	Result CreateFrame(UINT streamindex, uint8_t **buffer, AVFrame** frame) const;
	Result CreateRescaler(UINT streamindex, int destwidth, int destheight, PixelFormat destpixfmt, int flags, SwsContext** context) const;
	Result PrepareVideoStreaming(UINT streamidx, const Size<int>* destsize, PixelFormat destpixfmt,
								 int swscontextflags, uint8_t** framedata);
	Result StreamContent(float dt, bool *finished);
	Result StreamAudioDirect(bool *finished, int16_t* audiodata, int* audiodatalen);
	Result ReadRawPacket(AVPacket* packet, AVMediaType* codectype, bool *finished);
	AVMediaType GetPacketMediaType(const AVPacket* packet) const;
	Result DecodeAudioPacket(AVPacket* packet, int16_t* audiodata, int* audiodatalen, double* time);
	Result DecodeVideoPacket(AVPacket* packet, AVFrame* frame, bool* framedone, double* time);
	void RescaleVideoFrame(SwsContext* convertctx, const AVPacket* packet, AVFrame* srcframe, AVFrame* destframe);
	Result WriteRawPacket(AVPacket* packet);
	void EndVideoStreaming(UINT streamidx);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: FFmpeg
// Desc: API to the FFmpeg library
//-----------------------------------------------------------------------------
class FFmpeg : public IFFmpeg
{
private:
	std::list<MediaFile*> files;

public:
	CLOCKTICK_CALLBACK clockcbk;
	LPVOID clockcbkuser;

	FFmpeg()
	{
		clockcbk = NULL;
	};

	void Sync(GLOBALVARDEF_LIST);
	Result Init();

	void SetMouseDownCallback(const CLOCKTICK_CALLBACK clockcbk, LPVOID user) {this->clockcbk = clockcbk; clockcbkuser = user;}

	Result CreateNew(FilePath path, IMediaFile** file, String* ext);
	Result LoadFile(FilePath path, IMediaFile** file);
	void CloseFile(IMediaFile* file);

	Result PrepareBlittingToWnd(HWND wndtarget, const Rect<int>* destrect);
	void BlitToWnd(uint8_t* framedata);
	void EndBlittingToWnd();

	Result CreateFrame(int width, int height, PixelFormat pixfmt, uint8_t **buffer, AVFrame** frame) const;
	Result CreateRescaler(int srcwidth, int srcheight, PixelFormat srcpixfmt, int destwidth,
						  int destheight, PixelFormat destpixfmt, int flags, SwsContext** context) const;
	void FreePacket(AVPacket* packet) const
		{av_free_packet(packet);}

	void Release();
};

#endif