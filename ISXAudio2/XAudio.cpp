#include "XAudio.h"
#include <ISWinAudio.h> // For AudioFormat struct
#include "..\\global\\WavIO.h"


#define STARTUPDELAY					32 // [frames]
#define NUMSTREAMBUFFERS				64 // Has to be <= XAUDIO2_MAX_QUEUED_BUFFERS
#define STREAMINGBUFFERSIZE				0x200


/*//--------------------------------------------------------------------------------------
// Callback structure
//--------------------------------------------------------------------------------------
struct StreamingVoiceContext : public DirectX::IXAudio2VoiceCallback
{
    STDMETHOD_( void, OnVoiceProcessingPassStart )( UINT32 )
    {
    }
    STDMETHOD_( void, OnVoiceProcessingPassEnd )()
    {
    }
    STDMETHOD_( void, OnStreamEnd )()
    {
    }
    STDMETHOD_( void, OnBufferStart )( void* )
    {
    }
    STDMETHOD_( void, OnBufferEnd )( void* )
    {
        SetEvent( hBufferEndEvent );
    }
    STDMETHOD_( void, OnLoopEnd )( void* )
    {
    }
    STDMETHOD_( void, OnVoiceError )( void*, HRESULT )
    {
    }

    HANDLE hBufferEndEvent;

    StreamingVoiceContext() : hBufferEndEvent( CreateEvent( NULL, FALSE, FALSE, NULL ) )
    {
    }
    virtual ~StreamingVoiceContext()
    {
        CloseHandle( hBufferEndEvent );
    }
} voiceContext;*/


//-----------------------------------------------------------------------------
// Name: CreateXAudio()
// Desc: DLL API for creating an instance of XAudio
//-----------------------------------------------------------------------------
XAUDIO2_EXTERN_FUNC LPXAUDIO2 __cdecl CreateXAudio2()
{
	return new XAudio();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void XAudio::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
	if(pse)
		SyncWithPython();
}

//-----------------------------------------------------------------------------
// Name: Constructor()
// Desc: ?
//-----------------------------------------------------------------------------
XAudio::XAudio()
{
	lpxad = NULL;
	vcemaster = NULL;
	vcestream = NULL;
	xaudiobuffers = NULL;
	bufstream = NULL;
	streamingaudio = false;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: ?
//-----------------------------------------------------------------------------
Result XAudio::Init()
{
	Result rlt;

	// Create XAudio2
	UINT32 flags = 0;
/*#ifndef WIN_8
#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
#endif*/
	CHKRESULT(XAudio2Create(&lpxad, flags));

	// Create the mastering voice
	//CHKRESULT(lpxad->CreateMasteringVoice(&vcemaster)); //hr = pXAudio2->CreateMasteringVoice( &pMasteringVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, deviceID, NULL, AudioCategory_GameEffects );

#if WIN_8
CHKRESULT(lpxad->CreateMasteringVoice(&vcemaster, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, deviceID, NULL, AudioCategory_GameEffects));
#else
CHKRESULT(lpxad->CreateMasteringVoice(&vcemaster));
//UINT32 deviceIndex = 0;
//CHKRESULT(lpxad->CreateMasteringVoice(&vcemaster, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, deviceIndex, NULL));
#endif

	// Get device details/
	DirectX::XAUDIO2_VOICE_DETAILS voicedetails;
	vcemaster->GetVoiceDetails(&voicedetails);
	/*DWORD channelmask;
	vcemaster->GetChannelMask(&channelmask);

DirectX::X3DAUDIO_HANDLE x3DInstance;
DirectX::X3DAudioInitialize(channelmask, X3DAUDIO_SPEED_OF_SOUND, x3DInstance);*/

	// Store the number of channels
	numchannels = voicedetails.InputChannels;//devdetails.OutputFormat.Format.nChannels;
	samplesperbuffer = STREAMINGBUFFERSIZE / numchannels;

	/*// Create streaming voice
	AudioFormat fmt(voicedetails.InputSampleRate, 16, numchannels);
	WAVEFORMATEX wavefmt;
	fmt.GetWaveFormat(&wavefmt);
	CHKRESULT(lpxad->CreateSourceVoice(&vcestream, &wavefmt, NULL, 1.0f, &voiceContext));

	// Create streaming buffers
	xaudiobuffers = new DirectX::XAUDIO2_BUFFER[NUMSTREAMBUFFERS];
	bufstream = new short[NUMSTREAMBUFFERS * STREAMINGBUFFERSIZE];
	CHKALLOC(xaudiobuffers && bufstream);
	ZeroMemory(xaudiobuffers, NUMSTREAMBUFFERS * sizeof(DirectX::XAUDIO2_BUFFER));
	ZeroMemory(bufstream, NUMSTREAMBUFFERS * STREAMINGBUFFERSIZE * sizeof(short));
	for(DWORD i = 0; i < NUMSTREAMBUFFERS; i++)
	{
		xaudiobuffers[i].AudioBytes = STREAMINGBUFFERSIZE * sizeof(short);
		xaudiobuffers[i].pAudioData = (BYTE*)&bufstream[i * STREAMINGBUFFERSIZE];
		CHKRESULT(vcestream->SubmitSourceBuffer(&xaudiobuffers[i]));
	}*/

	return S_OK;
}

Result XAudio::CreateSound(const FilePath& filename, LPXA2SOUND* snd)
{
	Result rlt;

	*snd = NULL;

	WAVEFORMATEX fmt;
	DWORD numsamplesperchannel, wavdatasize;
	char* wavdata;
	CHKRESULT(LoadWAV(filename, &fmt, &numsamplesperchannel, &wavdata, &wavdatasize));

	Sound* newsnd;
	CHKALLOC(newsnd = new Sound(this, AudioFormat(fmt))); // Edit: replace NULL with audio format of outdev
	CHKRESULT(newsnd->Init((BYTE*)wavdata, wavdatasize));

	sounds.push_back(newsnd);
	*snd = newsnd;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Stream to the main voice
//-----------------------------------------------------------------------------
void XAudio::Update()
{
	//// Delay the starting of audio streaming by STARTUPDELAY frames, because the first few frames can take too long
	//static DWORD startupdelay = 1;
	//if(startupdelay <= STARTUPDELAY)
	//{
	//	if(startupdelay == STARTUPDELAY)
	//	{
	//		// Start streaming
	//		Result rlt;
	//		IFFAILED(vcestream->Start(NULL, NULL))
	//			LOG(rlt.GetLastResult());
	//		streamingaudio = true;
	//	}

	//	startupdelay++;
	//	return;
	//}

	//// Stream audio
	//static DWORD currentbuffer = 0;
	//DWORD firedevent = WaitForSingleObject(voiceContext.hBufferEndEvent, 0);
	//if(firedevent == WAIT_OBJECT_0)
	//{
	//	DirectX::XAUDIO2_VOICE_STATE state;
	//	vcestream->GetState(&state);

	//	if(state.BuffersQueued == 0)
	//	{
	//		LOG("Error: The audio buffers ran out of data. Maybe the frame rate is too low.");
	//		PostQuitMessage(0);
	//		return;
	//	}

	//	for(UINT32 i = state.BuffersQueued; i < NUMSTREAMBUFFERS; i++)
	//	{
	//		/*std::list<IAudio*>::iterator ia;
	//		for(ia = app->audiostreaminglist.begin(); ia != app->audiostreaminglist.end(); ia++)
	//			(*ia)->SubmitAudioData(bufstream + currentbuffer * STREAMINGBUFFERSIZE, STREAMINGBUFFERSIZE / numchannels);*/

	//		vcestream->SubmitSourceBuffer(&xaudiobuffers[currentbuffer]);
	//		currentbuffer++;
	//		currentbuffer %= NUMSTREAMBUFFERS;
	//	}
	//}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void XAudio::Release()
{
	while(sounds.size())
		sounds.front()->Release();

	if(vcestream)
	{
		vcestream->Stop(NULL);
		vcestream->DestroyVoice();
	}

	RELEASE(lpxad);
	REMOVE_ARRAY(bufstream);

	delete this;
}

Result Sound::Init(const BYTE* wavdata, UINT wavdatasize)
{
	Result rlt;

	// Create voice
	WAVEFORMATEX fmt;
	format.GetWaveFormat(&fmt);
	CHKRESULT(parent->lpxad->CreateSourceVoice(&voice, &fmt, NULL, 1.0f));

	// Submit audio data
	buffer = new DirectX::XAUDIO2_BUFFER();
	buffer->AudioBytes = wavdatasize;
	buffer->pAudioData = wavdata;
buffer->LoopCount = XAUDIO2_LOOP_INFINITE;
	CHKRESULT(voice->SubmitSourceBuffer(buffer));
//voice->SetFrequencyRatio(0.6f, XAUDIO2_COMMIT_NOW);

	return R_OK;
}

Result Sound::Play()
{
	return Error(voice->Start(0, XAUDIO2_COMMIT_NOW));
}
Result Sound::Pause()
{
	return Error(voice->Stop(0));
}
Result Sound::Stop()
{
	Result rlt;
	CHKRESULT(voice->Stop(0));
	CHKRESULT(voice->FlushSourceBuffers());
	CHKRESULT(voice->SubmitSourceBuffer(buffer));
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void Sound::Release()
{
	parent->sounds.remove(this);

	if(voice)
	{
		voice->Stop(NULL);
		voice->DestroyVoice();
	}
	if(buffer)
	{
		delete[] buffer->pAudioData;
		delete buffer;
	}

	delete this;
}