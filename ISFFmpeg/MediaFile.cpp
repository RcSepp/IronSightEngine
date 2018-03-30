#include "FFmpeg.h"

#ifndef INT64_C
#define INT64_C(i)	((int64_t)(i))
#endif


struct VideoStreamingParams //EDIT: Not secure for multiple instances. Move to private section of the class
{
	bool isprepared;
	AVFrame *srcframe, *destframe;
	uint8_t *srcbuffer, *destbuffer;
	Size<int> srcsize, destsize;
	PixelFormat srcpixfmt, destpixfmt;
	SwsContext* convertctx;
	int64_t frameidx; // Current time [frames]

	VideoStreamingParams()
	{
		isprepared = false;
	}
}*vsparams = NULL;

struct AudioStreamingParams //EDIT: Not secure for multiple instances. Move to private section of the class
{
	/*bool isprepared;
	AVFrame *srcframe, *destframe;
	uint8_t *srcbuffer, *destbuffer;
	Size<int> srcsize, destsize;
	PixelFormat srcpixfmt, destpixfmt;
	SwsContext* convertctx;*/
	int64_t frameidx; // Current time [frames]

	AudioStreamingParams()
	{
		/*isprepared = false;*/
		frameidx = 0;
	}
}*asparams = NULL;

AVFormatContext* blablabla; //DELETE

uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;
double video_clock = 0.0;
int our_get_buffer(struct AVCodecContext* c, AVFrame* pic)
{
	int ret = avcodec_default_get_buffer(c, pic);
	uint64_t* pts = (uint64_t*)av_malloc(sizeof(uint64_t));
	*pts = global_video_pkt_pts;
	pic->opaque = pts;
	return ret;
}
void our_release_buffer(struct AVCodecContext* c, AVFrame* pic)
{
	if(pic)
		av_freep(&pic->opaque);
	avcodec_default_release_buffer(c, pic);
}

//-----------------------------------------------------------------------------
// Name: New()
// Desc: Create file and streams, write header and load codecs
//-----------------------------------------------------------------------------
//Result MediaFile::New(FilePath path, String* ext)
//{
//	// Close old file
//	Close();
//
//	this->path = path;
//
//	// Auto detect output format based on path
//	AVOutputFormat *fmt;
//	if(ext)
//		fmt = guess_format(NULL, (String('.') << *ext).ToCharString(), NULL);
//	else
//		fmt = guess_format(NULL, path, NULL);
//	if(!fmt)
//		return ERR("Could not deduce output format from file extension");
//
//	// Allocate output format context
//	CHKALLOC(formatctx = av_alloc_format_context());
//	formatctx->oformat = fmt;
//
//	// Add audio and video streams using default format codecs and initialize codecs
//	/*if(fmt->video_codec == CODEC_ID_NONE)
//		video_st = NULL;
//	else
//		video_st = add_video_stream(formatctx, fmt->video_codec);
//	if(fmt->audio_codec == CODEC_ID_NONE)
//		audio_st = NULL;
//	else
//		audio_st = add_audio_stream(formatctx, fmt->audio_codec);*/
//numaudiostreams = 1;
//CHKALLOC(audiostreams = (Stream**)new LPVOID[numaudiostreams]);
//CHKALLOC(audiostreams[0] = new Stream());
//
//AVStream *st = av_new_stream(formatctx, 1);
//if(!st)
//	return ERR("Error allocating audio stream");
//
//audiostreams[0]->absolutestreamidx = 0;
//audiostreams[0]->stream = st;
//audiostreams[0]->codecctx = st->codec;
//audiostreams[0]->codecctx->codec_id = fmt->audio_codec;
//audiostreams[0]->codecctx->codec_type = AVMEDIA_TYPE_AUDIO;
//audiostreams[0]->enabled = true;
//
//// Set sample parameters
//audiostreams[0]->codecctx->bit_rate = 64000;
//audiostreams[0]->codecctx->sample_rate = 44100;
//audiostreams[0]->codecctx->channels = 2;
//
//	// Set output parameters (essential, even if no parameters are present)
//	if(av_set_parameters(formatctx, NULL) < 0)
//		return ERR("Invalid output format parameters");
//
//	/*// Open audio and video codecs and allocate necessary encode buffers
//	if(video_st)
//		open_video(formatctx, video_st);
//	if(audio_st)
//		open_audio(formatctx, audio_st);*/
//
//	// Open output file
//	if(url_fopen(&formatctx->pb, path, URL_WRONLY) < 0)
//		return ERR(String("Error opening file ") << path << String(" for writing"));
//
//	// Write stream header, if any
//	av_write_header(formatctx);
//
//	type = FT_OUTPUT;
//
//blablabla = formatctx;
//	return R_OK;
//}
Result MediaFile::New(FilePath path, String* ext)
{
	// Close old file
	Close();

	this->path = path;

	// Auto detect output format based on path
	AVOutputFormat *fmt;
//EDIT
/*	if(ext)
		fmt = guess_format(NULL, (String('.') << *ext).ToCharString(), NULL);
	else
		fmt = guess_format(NULL, path, NULL);*/
	if(!fmt)
		return ERR("Could not deduce output format from file extension");

	// Allocate output format context
	CHKALLOC(formatctx = avformat_alloc_context());
	formatctx->oformat = fmt;

	// Add audio and video streams using default format codecs and initialize codecs
	if(fmt->audio_codec == CODEC_ID_NONE)
	{
		numaudiostreams = 0;
		audiostreams = NULL;
	}
	else
	{
		numaudiostreams = 1;
		CHKALLOC(audiostreams = (Stream**)new LPVOID[numaudiostreams]);
		CHKALLOC(audiostreams[0] = new Stream());

		AVStream *st = av_new_stream(formatctx, 1);
		if(!st)
			return ERR("Error allocating audio stream");

		audiostreams[0]->absolutestreamidx = 0;
		audiostreams[0]->stream = st;
		audiostreams[0]->codecctx = st->codec;
		audiostreams[0]->codecctx->codec_id = fmt->audio_codec;
		audiostreams[0]->codecctx->codec_type = AVMEDIA_TYPE_AUDIO;
		audiostreams[0]->enabled = true;

		// Set sample parameters
		audiostreams[0]->codecctx->bit_rate = 64000;
		audiostreams[0]->codecctx->sample_rate = 44100;
		audiostreams[0]->codecctx->channels = 2;
	}

	if(fmt->video_codec == CODEC_ID_NONE)
	{
		numvideostreams = 0;
		videostreams = NULL;
	}
	else
	{
		numvideostreams = 1;
		CHKALLOC(videostreams = (Stream**)new LPVOID[numvideostreams]);
		CHKALLOC(videostreams[0] = new Stream());

		AVStream *st = av_new_stream(formatctx, 1);
		if(!st)
			return ERR("Error allocating audio stream");

		videostreams[0]->absolutestreamidx = fmt->audio_codec == CODEC_ID_NONE ? 0 : 1;
		videostreams[0]->stream = st;
		videostreams[0]->codecctx = st->codec;
		videostreams[0]->codecctx->codec_id = fmt->video_codec;
		videostreams[0]->codecctx->codec_type = AVMEDIA_TYPE_VIDEO;
		videostreams[0]->enabled = true;

		/*// Set sample parameters
		videostreams[0]->codecctx->bit_rate = 64000;
		videostreams[0]->codecctx->sample_rate = 44100;
		videostreams[0]->codecctx->channels = 2;*/
	}
	/*if(fmt->video_codec == CODEC_ID_NONE)
		video_st = NULL;
	else
		video_st = add_video_stream(formatctx, fmt->video_codec);
	if(fmt->audio_codec == CODEC_ID_NONE)
		audio_st = NULL;
	else
		audio_st = add_audio_stream(formatctx, fmt->audio_codec);*/

	// Set output parameters (essential, even if no parameters are present)
//EDIT
/*if(av_set_parameters(formatctx, NULL) < 0)
	return ERR("Invalid output format parameters");*/

	/*// Open audio and video codecs and allocate necessary encode buffers
	if(video_st)
		open_video(formatctx, video_st);
	if(audio_st)
		open_audio(formatctx, audio_st);*/

	// Open output file
//EDIT
/*if(url_fopen(&formatctx->pb, path, URL_WRONLY) < 0)
	return ERR(String("Error opening file ") << path << String(" for writing"));*/

	// Write stream header, if any
//EDIT
//av_write_header(formatctx);

	type = FT_OUTPUT;

blablabla = formatctx;
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Load()
// Desc: Load file, extract streams and load codecs
//-----------------------------------------------------------------------------
Result MediaFile::Load(FilePath path)
{
	// Close old file
	Close();

	this->path = path;

	// Open file
	if(avformat_open_input(&formatctx, path, NULL, NULL))
		return ERR(String("Error opening file") << path);

	// Retrieve stream information
	if(av_find_stream_info(formatctx) < 0)
		ERR("Stream information not found");

	// Get streams and load decoders
	Stream *newstream, ***streams;
	UINT *numstreams;
	CHKALLOC(streamidxmap = new UINT[formatctx->nb_streams]);
	for(UINT i = 0; i < formatctx->nb_streams; i++)
	{
		switch(formatctx->streams[i]->codec->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			streams = &videostreams;
			numstreams = &numvideostreams;
			break;

		case AVMEDIA_TYPE_AUDIO:
			streams = &audiostreams;
			numstreams = &numaudiostreams;
			break;

		default:
			streams = NULL;
		}
		if(!streams)
		{
			streamidxmap[i] = -1;
			continue;
		}

		CHKALLOC(newstream = new Stream());
		newstream->absolutestreamidx = i;

		// Disable all but the first stream per default
		newstream->enabled = !*numstreams;

		// Get stream
		newstream->stream = formatctx->streams[i];

		// Get codec context
		newstream->codecctx = formatctx->streams[i]->codec;
newstream->codecctx->get_buffer = our_get_buffer;
newstream->codecctx->release_buffer = our_release_buffer;

		// Find codec
		newstream->encoder = NULL;
		newstream->decoder = avcodec_find_decoder(newstream->codecctx->codec_id);

		// Report missing codec
		if(!newstream->decoder)
		{
			char buf[256];
			avcodec_string(buf, 256, newstream->codecctx, 0);
			return ERR(String("Codec not found for ") << String(buf)); //EDIT: Just report, don't fail
		}

		// Allow truncated bitstreams
		if(newstream->decoder->capabilities & CODEC_CAP_TRUNCATED)
			newstream->codecctx->flags |= CODEC_FLAG_TRUNCATED;

		// Open codec
		if(avcodec_open2(newstream->codecctx, newstream->decoder, NULL) < 0)
			return ERR("Error opening codec");

		CHKALLOC(*streams = (Stream**)_realloc(*streams, *numstreams * sizeof(Stream*), (*numstreams + 1) * sizeof(Stream*)));
		(*streams)[*numstreams] = newstream;
		streamidxmap[i] = (*numstreams)++;
	}

	// Initialize streaming parameters
	asparams = new AudioStreamingParams[numaudiostreams];
	vsparams = new VideoStreamingParams[numvideostreams];

	type = FT_INPUT;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Close()
// Desc: Close file
//-----------------------------------------------------------------------------
void MediaFile::Close()
{
	if(type == FT_NONE)
		return;

	UINT i;

	// Close video codecs and remove streams
	for(i = 0; i < numvideostreams; i++)
	{
		avcodec_close(videostreams[i]->codecctx);
		REMOVE(videostreams[i]);
	}
	REMOVE_ARRAY(videostreams)
	numvideostreams = 0;

	// Close audio codecs and remove streams
	for(i = 0; i < numaudiostreams; i++)
	{
		avcodec_close(audiostreams[i]->codecctx);
		REMOVE(audiostreams[i]);
	}
	REMOVE_ARRAY(audiostreams)
	numaudiostreams = 0;

	// Remove stream index map
	REMOVE_ARRAY(streamidxmap)

	// Remove streaming parameters
	for(i = 0; i < numvideostreams; i++)
		EndVideoStreaming(i);
	REMOVE_ARRAY(vsparams)

	// Close file and streams
	if(formatctx)
	{
		if(type == FT_INPUT)
			// Close input file (ffmpeg streams are removed internally)
			av_close_input_file(formatctx);
		else
		{
			// Write trailer, if any
			av_write_trailer(formatctx);

			// Remove ffmpeg streams
			for(i = 0; i < formatctx->nb_streams; i++)
				av_freep(&formatctx->streams[i]);

			// Close output file
//EDIT //url_fclose(formatctx->pb);
		}

		// Remove format context
//		av_free(formatctx); //EDIT: Properly close formatctx
		formatctx = NULL;
	}

	type = FT_NONE;
}

//-----------------------------------------------------------------------------
// Name: GetNumberOfStreams()
// Desc: Return number of streams of the given stream type
//-----------------------------------------------------------------------------
UINT MediaFile::GetNumberOfStreams(AVMediaType streamtype)
{
	switch(streamtype)
	{
	case AVMEDIA_TYPE_AUDIO: return numaudiostreams;
	case AVMEDIA_TYPE_VIDEO: return numvideostreams;
	default: return 0;
	}
}

//-----------------------------------------------------------------------------
// Name: Enable/DisableStream()
// Desc: Set enabled variable inside stream class. Disabled streams will be skipped inside ReadRawPacket()
//-----------------------------------------------------------------------------
void MediaFile::EnableStream(UINT streamindex, AVMediaType streamtype)
{
	switch(streamtype)
	{
	case AVMEDIA_TYPE_AUDIO: if(streamindex < numaudiostreams) audiostreams[streamindex]->enabled = true;
	case AVMEDIA_TYPE_VIDEO: if(streamindex < numvideostreams) videostreams[streamindex]->enabled = true;
	}
}
void MediaFile::DisableStream(UINT streamindex, AVMediaType streamtype)
{
	switch(streamtype)
	{
	case AVMEDIA_TYPE_AUDIO: if(streamindex < numaudiostreams) audiostreams[streamindex]->enabled = false;
	case AVMEDIA_TYPE_VIDEO: if(streamindex < numvideostreams) videostreams[streamindex]->enabled = false;
	}
}

//-----------------------------------------------------------------------------
// Name: GetAudioFormat()
// Desc: Get audio format of the given audio stream
//-----------------------------------------------------------------------------
AudioFormat* MediaFile::GetAudioFormat(UINT streamindex)
{
	if(streamindex >= numaudiostreams)
		return NULL;

	WORD bits;
	GUID subformat;
	switch(audiostreams[streamindex]->codecctx->sample_fmt)
	{
	case AV_SAMPLE_FMT_U8: bits = 8; subformat = KSDATAFORMAT_SUBTYPE_PCM; break;
	case AV_SAMPLE_FMT_S16: bits = 16; subformat = KSDATAFORMAT_SUBTYPE_PCM; break;
	case AV_SAMPLE_FMT_S32: bits = 32; subformat = KSDATAFORMAT_SUBTYPE_PCM; break;
	case AV_SAMPLE_FMT_FLT: bits = 32; subformat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT; break;
	case AV_SAMPLE_FMT_DBL: bits = 64; subformat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT; break;
	default: return NULL;
	}
	return new AudioFormat(audiostreams[streamindex]->codecctx->sample_rate, bits, audiostreams[streamindex]->codecctx->channels, subformat);
}

#pragma region Video Streaming
Result AllocPicture(int width, int height, PixelFormat pixfmt, uint8_t **buffer, AVFrame** picture)
{
	int bufferlength;

	CHKALLOC(*picture = avcodec_alloc_frame());

	bufferlength = avpicture_get_size(pixfmt, width, height);
	*buffer = (uint8_t*)av_malloc(bufferlength);
	if(!*buffer)
	{
		AVREMOVE(picture)
		return ERR("Out of memory");
	}

	avpicture_fill((AVPicture*)*picture, *buffer, pixfmt, width, height);

	return R_OK;
}

Result MediaFile::CreateFrame(UINT streamindex, uint8_t **buffer, AVFrame** frame) const
{
	AVCodecContext* codecctx = videostreams[streamindex]->codecctx;
	return ffm->CreateFrame(codecctx->width, codecctx->height, codecctx->pix_fmt, buffer, frame);
}

Result MediaFile::CreateRescaler(UINT streamindex, int destwidth, int destheight, PixelFormat destpixfmt, int flags, SwsContext** context) const
{
	AVCodecContext* codecctx = videostreams[streamindex]->codecctx;
	return ffm->CreateRescaler(codecctx->width, codecctx->height, codecctx->pix_fmt, destwidth, destheight, destpixfmt, flags, context);
}

//-----------------------------------------------------------------------------
// Name: PrepareVideoStreaming()
// Desc: Allocate memory for video streaming and setup video transformations
//-----------------------------------------------------------------------------
Result MediaFile::PrepareVideoStreaming(UINT streamidx, const Size<int>* destsize, PixelFormat destpixfmt,
										int swscontextflags, uint8_t** framedata)
{
	*framedata = NULL;

	if(streamidx >= numvideostreams)
		return ERR("Invalid stream index");
	if(videostreams[streamidx]->decoder == NULL)
		return ERR("Trying to decode stream with no codec loaded");

	AVCodecContext* codecctx = videostreams[streamidx]->codecctx;
	VideoStreamingParams* params = &vsparams[streamidx];
	Result rlt;

	// Set video sizes
	params->srcsize.width = codecctx->width;
	params->srcsize.height = codecctx->height;
	if(destsize)
		params->destsize = Size<int>(*destsize);
	else
		params->destsize = params->srcsize;

	// Set pixel formats
	params->srcpixfmt = codecctx->pix_fmt;
	if(destpixfmt)
		params->destpixfmt = destpixfmt;
	else
		params->destpixfmt = params->srcpixfmt;

	if(params->srcsize != params->destsize || params->srcpixfmt != params->destpixfmt)
	{
		// Allocate memory
		CHKRESULT(AllocPicture(params->srcsize.width, params->srcsize.height, params->srcpixfmt, &params->srcbuffer, &params->srcframe));
		CHKRESULT(AllocPicture(params->destsize.width, params->destsize.height, params->destpixfmt, &params->destbuffer, &params->destframe));
		*framedata = params->destbuffer;

		// Prepare video transformations
		params->convertctx = sws_getContext(params->srcsize.width, params->srcsize.height, params->srcpixfmt,
											params->destsize.width, params->destsize.height, params->destpixfmt, swscontextflags, NULL, NULL, NULL);
	}
	else
	{
		// Allocate memory
		CHKRESULT(AllocPicture(params->srcsize.width, params->srcsize.height, params->srcpixfmt, &params->srcbuffer, &params->srcframe));
		*framedata = params->srcbuffer;

		// Video transformations, destination frame and destination buffer aren't needed
		params->destbuffer = NULL;
		params->destframe = NULL;
		params->convertctx = NULL;
	}

	params->isprepared = true;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: StreamContent()
// Desc: Decode audio and video streams and return the new frame
//-----------------------------------------------------------------------------
Result MediaFile::StreamContent(float dt, bool *finished)
{
	if(type != FT_INPUT)
		return ERR("Media file is not an input file");

	//VideoStreamingParams* videoparams;
	int readresult, framefinished;
	static AVPacket packet;
	//UINT streamidx;

	*finished = false;

//int16_t *samples;
//int frame_size_ptr;
//int result;
//if(blabla)
//	samples = new int16_t[(1024*128)];


static VideoStreamingParams* videoparams;
static UINT streamidx;
static int bytesdecoded, bytesremaining = 0;
static byte* rawdata;
static bool firstframe = true;
if(firstframe)
{
	firstframe = false;
	packet.data = NULL;
}

/*while(1)
{
	while(bytesremaining > 0)
	{
bytesdecoded = avcodec_decode_video2(videostreams[streamidx]->codecctx, videoparams->srcframe, &framefinished, &packet);
bytesremaining = 0;
//bytesdecoded = avcodec_decode_video2(videostreams[streamidx]->codecctx, videoparams->srcframe, &framefinished, rawdata, bytesremaining); //EDIT: Function is deprecated
		if(bytesdecoded < 0)
			return ERR(String(bytesdecoded));

		bytesremaining -= bytesdecoded;
		rawdata += bytesdecoded;

		if(framefinished)
		{
			// Apply transformation
			if(videoparams->convertctx)
				sws_scale(videoparams->convertctx, videoparams->srcframe->data, videoparams->srcframe->linesize, 0,
						  videostreams[streamidx]->codecctx->height, videoparams->destframe->data, videoparams->destframe->linesize);

			return S_OK;
		}
	}

	do
	{
		if(packet.data != NULL)
			av_free_packet(&packet);

		if((readresult = av_read_frame(formatctx, &packet)) < 0)
			goto loop_exit;
	} while(formatctx->streams[packet.stream_index]->codec->codec_type != AVMEDIA_TYPE_VIDEO);

	streamidx = streamidxmap[packet.stream_index]; // Get video stream index from absolute stream index
	videoparams = &vsparams[streamidx];

	bytesremaining = packet.size;
	rawdata = packet.data;
}
loop_exit:*/

while(1)
{
	do
	{
		if(packet.data != NULL)
			av_free_packet(&packet);

		if((readresult = av_read_frame(formatctx, &packet)) < 0)
			goto loop_exit;
	} while(formatctx->streams[packet.stream_index]->codec->codec_type != AVMEDIA_TYPE_VIDEO);

	streamidx = streamidxmap[packet.stream_index]; // Get video stream index from absolute stream index
	videoparams = &vsparams[streamidx];

	bytesdecoded = avcodec_decode_video2(videostreams[streamidx]->codecctx, videoparams->srcframe, &framefinished, &packet);
	if(bytesdecoded < 0)
		return ERR(String(bytesdecoded));

	if(packet.data)
	{
		packet.size -= bytesdecoded;
		packet.data += bytesdecoded;
	}

	if(framefinished)
	{
		// Apply transformation
		if(videoparams->convertctx)
			sws_scale(videoparams->convertctx, videoparams->srcframe->data, videoparams->srcframe->linesize, 0,
						videostreams[streamidx]->codecctx->height, videoparams->destframe->data, videoparams->destframe->linesize);

		return S_OK;
	}

	streamidx = streamidxmap[packet.stream_index]; // Get video stream index from absolute stream index
	videoparams = &vsparams[streamidx];
}
loop_exit:




	/*while((readresult = av_read_frame(formatctx, &packet)) >= 0)
	{
		switch(formatctx->streams[packet.stream_index]->codec->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			if(vsparams[streamidxmap[packet.stream_index]].isprepared)
			{
				streamidx = streamidxmap[packet.stream_index]; // Get video stream index from absolute stream index
				videoparams = &vsparams[streamidx];

bytesremaining = packet.size;
rawdata = packet.data;

				// Store frame index
				videoparams->frameidx = packet.pts;

				// Decode video frame
while(bytesremaining > 0)
{
				bytesdecoded = avcodec_decode_video(videostreams[streamidx]->codecctx, videoparams->srcframe, &framefinished, rawdata, bytesremaining); //EDIT: Function is deprecated
if(bytesdecoded < 0)
	return ERR(String(bytesdecoded));
bytesremaining -= bytesdecoded;
rawdata += bytesdecoded;

				if(framefinished)
				{
					av_free_packet(&packet);

					// Apply transformation
					if(videoparams->convertctx)
						sws_scale(videoparams->convertctx, videoparams->srcframe->data, videoparams->srcframe->linesize, 0,
								  videostreams[streamidx]->codecctx->height, videoparams->destframe->data, videoparams->destframe->linesize);

					return R_OK; //EDIT: Wait for framefinished of other streams
				}
}
av_free_packet(&packet);

				//if(framefinished) //EDIT: Wait for framefinished of other streams
				//	return R_OK;
			}
			break;

		case AVMEDIA_TYPE_AUDIO:
if(blabla)
{
frame_size_ptr = (1024*128) * sizeof(int16_t);
streamidx = 0;

//for(int i = 4; i < packet.size; i++)
//	packet.data[i] = rand() % 255;
			result = avcodec_decode_audio3(audiostreams[streamidx]->codecctx, samples, &frame_size_ptr, &packet);
			//result = avcodec_decode_audio2(audiostreams[streamidx]->codecctx, samples, &frame_size_ptr, packet.data, packet.size);
av_free_packet(&packet);
if(result < 0)

{
	delete[] samples;
	return ERR(String(result));
}

if(frame_size_ptr)
{
*bla = samples;
*blabla = frame_size_ptr;
	//delete[] samples;
	return R_OK;
}
}
else
	av_free_packet(&packet);

			break;

		default:
			av_free_packet(&packet);
		}
	}*/

	if(readresult != -32) // -32 ... Return code for finished streaming
		return ERR("Error reading frame");

	//if(params->repeat) //EDIT: Set repeat globally
	//{
	//	av_seek_frame(formatctx, videostreams[streamidx]->absolutestreamidx, 0, AVSEEK_FLAG_FRAME);
	//	av_free_packet(&packet); // EDIT: not jet working (also freeing and recreating this non-pointer value seems wrong)
	//	av_init_packet(&packet); // EDIT: not jet working (also freeing and recreating this non-pointer value seems wrong)
	//	packet.data = (uint8_t*)"FLUSH"; // EDIT: not jet working (also freeing and recreating this non-pointer value seems wrong)
	//}
	//else
		*finished = true;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: StreamAudioDirect()
// Desc: Decode audio stream from a file with only one stream and a constant bitrate
//-----------------------------------------------------------------------------
Result MediaFile::StreamAudioDirect(bool *finished, int16_t* audiodata, int* audiodatalen)
{
	if(type != FT_INPUT)
		return ERR("Media file is not an input file");

	int readresult;
	static AVPacket packet;
	int result;

	*finished = false;

//audiodata = new int16_t[(0x1200 * 0x28)];
//*audiodatalen += (0x1200 * 0x28) * sizeof(int16_t);
	while((readresult = av_read_frame(formatctx, &packet)) >= 0)
	{
		switch(formatctx->streams[packet.stream_index]->codec->codec_type)
		{
		case AVMEDIA_TYPE_AUDIO:
//for(int i = 4; i < packet.size; i++)
//	packet.data[i] = rand() % 255;

			result = avcodec_decode_audio3(audiostreams[0]->codecctx, audiodata, audiodatalen, &packet);

//for(int i = 4; i < packet.size; i++)
//	packet.data[i] = rand() % 255;
//result = av_write_frame(blablabla, &packet);

			av_free_packet(&packet);

			if(result < 0)
				return ERR(String(result));
			if(audiodatalen)
				return R_OK;
			else
				return ERR("");

		case AVMEDIA_TYPE_VIDEO:
			av_free_packet(&packet);
			return ERR(String("Decoding video streams not supported by ") << String(__FUNCTION__));

		default:
			av_free_packet(&packet);
		}
	}

	if(readresult != -32) // -32 ... Return code for finished streaming
		return ERR("Error reading frame");

	*finished = true;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: ReadPacket()
// Desc: Read the next raw data packet from file
//-----------------------------------------------------------------------------
Result MediaFile::ReadRawPacket(AVPacket* packet, AVMediaType* codectype, bool *finished)
{
	if(type != FT_INPUT)
		return ERR("Media file is not an input file");

	WaitForSingleObject(readmutex, INFINITE);

	int result;
	UINT streamidx;
	bool enabled;
	do
	{
		result = av_read_frame(formatctx, packet);

		streamidx = streamidxmap[packet->stream_index]; // Get media type specific stream index from absolute stream index

		switch(formatctx->streams[packet->stream_index]->codec->codec_type)
		{
			case AVMEDIA_TYPE_AUDIO: enabled = audiostreams[streamidx]->enabled; break;
			case AVMEDIA_TYPE_VIDEO: enabled = videostreams[streamidx]->enabled; break;
			default: enabled = false; // Disable streams of unknown codec type
		}
	}
	while(result >= 0 && !enabled);

	*finished = false;

	/*// Update time
	asparams[streamidx].frameidx = packet->pts;

	// Throw timer events
	static int oldsecond = 0; // EDIT: There may be a better way

	int currentsecond;
	if(packet->pts == AV_NOPTS_VALUE)
		currentsecond = packet->pos / (audiostreams[streamidx]->codecctx->sample_rate * audiostreams[streamidx]->codecctx->channels); // EDIT: This is just plain wrong!!!
	else
		currentsecond = (int)(av_q2d(formatctx->streams[packet->stream_index]->time_base) * (double)packet->pts);
	if(currentsecond != oldsecond)
	{
		oldsecond = currentsecond;
		if(parent->clockcbk)
			parent->clockcbk(this, currentsecond, parent->clockcbkuser);
	}*/

	ReleaseMutex(readmutex);

	if(result >= 0)
	{
		if(codectype)
			*codectype = formatctx->streams[packet->stream_index]->codec->codec_type;
	}
	else if(result == -32 || result == -5) // EDIT: -5 is actually an IO error
		*finished = true;
	else
		return ERR(String("Error reading packet: Error code ") << String(result));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: GetPacketMediaType()
// Desc: Return media type of packet content
//-----------------------------------------------------------------------------
AVMediaType MediaFile::GetPacketMediaType(const AVPacket* packet) const
{
	return formatctx->streams[packet->stream_index]->codec->codec_type;
}

//-----------------------------------------------------------------------------
// Name: DecodeAudioPacket()
// Desc: Decode audio packet
// Parameter audiodata: IN: length of audiodata buffer, OUT: length of data decoded into audiodata (if any)
//-----------------------------------------------------------------------------
Result MediaFile::DecodeAudioPacket(AVPacket* packet, int16_t* audiodata, int* audiodatalen, double* time)
{
	if(type != FT_INPUT)
		return ERR("Media file is not an input file");
	if(formatctx->streams[packet->stream_index]->codec->codec_type != AVMEDIA_TYPE_AUDIO)
		return ERR("Packet content is not of type audio");

	int len = avcodec_decode_audio3(audiostreams[streamidxmap[packet->stream_index]]->codecctx, audiodata, audiodatalen, packet);

	if(len < 0)
		return ERR(String("Error decoding audio packet: Error code ") << String(len));

	packet->size -= len;
	packet->data += len;

	// Update time
	UINT streamidx = streamidxmap[packet->stream_index]; // Get media type specific stream index from absolute stream index
	asparams[streamidx].frameidx = packet->pts;

	// Throw timer events
	static int oldsecond = 0; // EDIT: There may be a better way

	if(packet->pts == AV_NOPTS_VALUE)
		*time = (double)packet->pos / (double)(audiostreams[streamidx]->codecctx->sample_rate * audiostreams[streamidx]->codecctx->channels * 2); // EDIT: This is just plain wrong!!!
	else
		*time = av_q2d(formatctx->streams[packet->stream_index]->time_base) * (double)packet->pts;
	int currentsecond = (int)*time;
	if(currentsecond != oldsecond)
	{
		oldsecond = currentsecond;
		if(parent->clockcbk)
			parent->clockcbk(this, currentsecond, parent->clockcbkuser);
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: DecodeVideoPacket()
// Desc: Decode video packet
//-----------------------------------------------------------------------------
Result MediaFile::DecodeVideoPacket(AVPacket* packet, AVFrame* frame, bool* framedone, double* time)
{
	if(type != FT_INPUT)
		return ERR("Media file is not an input file");
	if(formatctx->streams[packet->stream_index]->codec->codec_type != AVMEDIA_TYPE_VIDEO)
		return ERR("Packet content is not of type audio");

	int len, done;
	do
	{
		len = avcodec_decode_video2(videostreams[streamidxmap[packet->stream_index]]->codecctx, frame, &done, packet);

		if(len < 0)
			return ERR(String("Error decoding video packet: Error code ") << String(len));

		packet->size -= len;
		packet->data += len;
	}
	while(packet->size > 0 && !done);

	*time = av_q2d(formatctx->streams[packet->stream_index]->time_base) * (double)packet->pts;

	*framedone = (done != 0);

	if(done)
	{
		if(packet->dts == AV_NOPTS_VALUE && frame->opaque && *(uint64_t*)frame->opaque != AV_NOPTS_VALUE)
			*time = (double)*(uint64_t*)frame->opaque * av_q2d(formatctx->streams[packet->stream_index]->time_base);
		else if(packet->dts != AV_NOPTS_VALUE)
			*time = (double)packet->dts * av_q2d(formatctx->streams[packet->stream_index]->time_base);
		else
			*time = 0.0;

if(*time)
	video_clock = *time;
else
	*time = video_clock;

double frame_delay = av_q2d(formatctx->streams[packet->stream_index]->time_base);
frame_delay += frame->repeat_pict * (frame_delay * 0.5);
video_clock += frame_delay;
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: RescaleVideoFrame()
// Desc: Rescale video frame
//-----------------------------------------------------------------------------
void MediaFile::RescaleVideoFrame(SwsContext* convertctx, const AVPacket* packet, AVFrame* srcframe, AVFrame* destframe)
{
	sws_scale(convertctx, srcframe->data, srcframe->linesize, 0,
			  videostreams[streamidxmap[packet->stream_index]]->codecctx->height, destframe->data, destframe->linesize);
}

//-----------------------------------------------------------------------------
// Name: WritePacket()
// Desc: Write a raw data packet to file
//-----------------------------------------------------------------------------
Result MediaFile::WriteRawPacket(AVPacket* packet)
{
	if(type != FT_OUTPUT)
		return ERR("Media file is not an output file");

	int result = av_write_frame(blablabla, packet);
	if(result < 0)
		return ERR(String("Error writing packet: Error code ") << String(result));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: EndVideoStreaming()
// Desc: Free memory
//-----------------------------------------------------------------------------
void MediaFile::EndVideoStreaming(UINT streamidx)
{
	if(streamidx >= numvideostreams || !vsparams[streamidx].isprepared)
		return;

	VideoStreamingParams* params = &vsparams[streamidx];

	// Remove buffers
	AVREMOVE(params->srcbuffer)
	AVREMOVE(params->destbuffer)

	// Remove frames
	AVREMOVE(params->srcframe)
	AVREMOVE(params->destframe)

	// Remove video transformations
	if(params->convertctx)
	{
		sws_freeContext(params->convertctx);
		params->convertctx = NULL;
	}

	params->isprepared = false;
}
#pragma endregion

//-----------------------------------------------------------------------------
// Name: GetVideoWidth()
// Desc: Get video frame width
//-----------------------------------------------------------------------------
int MediaFile::GetVideoWidth(UINT streamindex)
{
	return streamindex < numvideostreams ? videostreams[streamindex]->codecctx->width : 0;
}

//-----------------------------------------------------------------------------
// Name: GetVideoHeight()
// Desc: Get video frame height
//-----------------------------------------------------------------------------
int MediaFile::GetVideoHeight(UINT streamindex)
{
	return streamindex < numvideostreams ? videostreams[streamindex]->codecctx->height : 0;
}

//-----------------------------------------------------------------------------
// Name: Get[Audio/Video]Duration()
// Desc: Get audio/video duration in seconds
//-----------------------------------------------------------------------------
double MediaFile::GetAudioDuration(UINT streamindex)
{
	return streamindex < numaudiostreams ? (double)audiostreams[streamindex]->stream->duration * av_q2d(audiostreams[streamindex]->stream->time_base) : 0.0; //&& asparams[streamindex].isprepared
}

//-----------------------------------------------------------------------------
// Name: Get[Audio/Video][Frame/Time]()
// Desc: Get audio/video frame-index/time from the given stream
//-----------------------------------------------------------------------------
int64_t MediaFile::GetAudioFrame(UINT streamindex)
{
	return streamindex < numaudiostreams ? asparams[streamindex].frameidx : 0; //&& asparams[streamindex].isprepared
}
double MediaFile::GetAudioTime(UINT streamindex)
{
	return streamindex < numaudiostreams ? (double)asparams[streamindex].frameidx * av_q2d(audiostreams[streamindex]->stream->time_base) : 0.0; //&& asparams[streamindex].isprepared
}
int64_t MediaFile::GetVideoFrame(UINT streamindex)
{
	return streamindex < numvideostreams && vsparams[streamindex].isprepared ? vsparams[streamindex].frameidx : 0;
}

//-----------------------------------------------------------------------------
// Name: Set[Audio/Video][Frame/Time]()
// Desc: Seek to the given audio/video frame-index/time
//-----------------------------------------------------------------------------
void MediaFile::SetAudioFrame(UINT streamindex, int64_t frame)
{
	if(streamindex >= numaudiostreams) //|| !vsparams[streamindex].isprepared //EDIT
		return;

	WaitForSingleObject(readmutex, INFINITE);
	int flags = frame < asparams[streamindex].frameidx ? AVSEEK_FLAG_BACKWARD : NULL;
	asparams[streamindex].frameidx = frame;
	av_seek_frame(formatctx, audiostreams[streamindex]->absolutestreamidx, frame, flags);
	ReleaseMutex(readmutex);
}
void MediaFile::SetAudioTime(UINT streamindex, double time)
{
	if(streamindex >= numaudiostreams) //|| !vsparams[streamindex].isprepared //EDIT
		return;

	WaitForSingleObject(readmutex, INFINITE);
	int64_t newframe = (int64_t)(time / av_q2d(audiostreams[streamindex]->stream->time_base));
	int flags = newframe < asparams[streamindex].frameidx ? AVSEEK_FLAG_BACKWARD : NULL;
	asparams[streamindex].frameidx = newframe;
	av_seek_frame(formatctx, audiostreams[streamindex]->absolutestreamidx, newframe, flags);
	avcodec_flush_buffers(audiostreams[streamindex]->codecctx);
	ReleaseMutex(readmutex);
}
void MediaFile::SetVideoFrame(UINT streamindex, int64_t frame)
{
	if(streamindex >= numvideostreams || !vsparams[streamindex].isprepared)
		return;

	WaitForSingleObject(readmutex, INFINITE);
	vsparams[streamindex].frameidx = frame;
	av_seek_frame(formatctx, videostreams[streamindex]->absolutestreamidx, frame, AVSEEK_FLAG_FRAME);
	avcodec_flush_buffers(videostreams[streamindex]->codecctx);
	ReleaseMutex(readmutex);
}

//-----------------------------------------------------------------------------
// Name: Scroll[Audio/Video][Frame/Time]()
// Desc: Seek forward or backward, given an offset in seconds or frames
//-----------------------------------------------------------------------------
void MediaFile::ScrollAudioFrame(UINT streamindex, int64_t frames)
{
	if(streamindex >= numaudiostreams) //|| !vsparams[streamindex].isprepared //EDIT
		return;

	WaitForSingleObject(readmutex, INFINITE);
	int flags = frames < 0 ? AVSEEK_FLAG_BACKWARD : NULL;
	asparams[streamindex].frameidx += frames;
	av_seek_frame(formatctx, audiostreams[streamindex]->absolutestreamidx, asparams[streamindex].frameidx, flags);
	avcodec_flush_buffers(audiostreams[streamindex]->codecctx);
	ReleaseMutex(readmutex);
}
void MediaFile::ScrollAudioTime(UINT streamindex, double dt)
{
	if(streamindex >= numaudiostreams) //|| !vsparams[streamindex].isprepared //EDIT
		return;

	WaitForSingleObject(readmutex, INFINITE);
	int64_t frames = (int64_t)(dt / av_q2d(audiostreams[streamindex]->stream->time_base));
	int flags = frames < 0 ? AVSEEK_FLAG_BACKWARD : NULL;
	asparams[streamindex].frameidx += frames;
	av_seek_frame(formatctx, audiostreams[streamindex]->absolutestreamidx, asparams[streamindex].frameidx, flags);
	avcodec_flush_buffers(audiostreams[streamindex]->codecctx);
	ReleaseMutex(readmutex);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void MediaFile::Release()
{
	Close();
	CloseHandle(readmutex);

	delete this;
}







