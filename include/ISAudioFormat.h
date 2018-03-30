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
	AudioFormat(WAVEFORMATEX& wavfmt)
	{
		this->samplerate = wavfmt.nSamplesPerSec;
		this->bits = (wavfmt.wBitsPerSample % 8 == 0) ? wavfmt.wBitsPerSample : ((wavfmt.wBitsPerSample / 8 + 1) * 8);
		this->channelmask = SPEAKER_ALL;
		this->channels = wavfmt.nChannels;
		this->subformat = KSDATAFORMAT_SUBTYPE_PCM;
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
	String ToString() const
	{
		return String("AudioFormat(") << String((int)bits) << String(" bit, ") << String((int)samplerate) << String(" Hz)");
	}
};
#endif