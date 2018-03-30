#include "WinMidi.h"


#define	MIDIMAPPERNAME				"Microsoft MIDI Mapper"
#define REMAPDEVICEID(deviceid)		deviceid == 0 ? MIDI_MAPPER : deviceid - 1


//-----------------------------------------------------------------------------
// Name: CreateWinMidi()
// Desc: DLL API for creating an instance of WinMidi
//-----------------------------------------------------------------------------
WINMIDI_EXTERN_FUNC LPWINMIDI __cdecl CreateWinMidi()
{
	return new WinMidi();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void WinMidi::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

//-----------------------------------------------------------------------------
// Name: MidiInCallback()
// Desc: Catches events from opened midi input devices
//-----------------------------------------------------------------------------
void CALLBACK InDevice::MidiInCallback(HMIDIIN hmi, UINT msg, DWORD_PTR instance, DWORD_PTR wparam, DWORD_PTR lparam)
{
	InDevice* dev = (InDevice*)instance;
	if(msg == MM_WIM_DATA || msg == MM_MIM_DATA)
	{
		const bool isdown = (wparam & 0xF0) == 0x90;
		const byte note = (wparam >> 8) & 0xFF;
		const byte velo = ((wparam >> 16) & 0xFF) * 2;

		if(dev->notedowncbk && isdown)
			dev->notedowncbk(note, velo);
	}
}

//-----------------------------------------------------------------------------
// Name: WinMidi()
// Desc: Constructor
//-----------------------------------------------------------------------------
WinMidi::WinMidi()
{
	outdevnames = indevnames = NULL;
	numindevs = numoutdevs = 0;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Query MIDI devices
//-----------------------------------------------------------------------------
Result WinMidi::Init()
{
	// Query output devices
	MIDIOUTCAPS mocaps;

	numoutdevs = midiOutGetNumDevs();
	if(numoutdevs == 0)
		LOG("No MIDI output devices found");
	numoutdevs++; // Add midi mapper device

	// Get output device names
	CHKALLOC(outdevnames = new String[numoutdevs]);
	outdevnames[0] = String(MIDIMAPPERNAME);
	for(UINT i = 0; i < numoutdevs; i++)
	{
		midiOutGetDevCaps(i, &mocaps, sizeof(mocaps));
		outdevnames[i] = String(mocaps.szPname);
	}

	// Query input devices
	MIDIINCAPS micaps;

	numindevs = midiInGetNumDevs();
	if(numindevs == 0)
		LOG("No MIDI input devices found");
	numindevs++; // Add midi mapper device

	// Get input device names
	CHKALLOC(indevnames = new String[numindevs]);
	indevnames[0] = String(MIDIMAPPERNAME);
	for(UINT i = 0; i < numindevs; i++)
	{
		midiInGetDevCaps(i, &micaps, sizeof(micaps));
		indevnames[i] = String(micaps.szPname);
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: GetOutputDeviceTechnology()
// Desc: Get the technology the given device is based on
//-----------------------------------------------------------------------------
Result WinMidi::GetOutputDeviceTechnology(UINT deviceid, MidiDeviceTechnology* mdt) const
{
	*mdt = MDT_UNDEFINED;

	if(deviceid >= numoutdevs)
		return ERR("Invalid device id");
	REMAPDEVICEID(deviceid); // Midi mapper acts as the first device

	MIDIOUTCAPS mocaps;
	midiOutGetDevCaps(deviceid, &mocaps, sizeof(mocaps));

	*mdt = (MidiDeviceTechnology)mocaps.wTechnology;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: GetInputProductName()
// Desc: Get the product name of the given input device
//-----------------------------------------------------------------------------
Result WinMidi::GetInputProductName(UINT deviceid, char (&name)[32]) const
{
	name[0] = '\0';

	if(deviceid >= numindevs)
		return ERR("Invalid device id");
	REMAPDEVICEID(deviceid); // Midi mapper acts as the first device

	MIDIINCAPS micaps;
	midiInGetDevCaps(deviceid, &micaps, sizeof(micaps));

	memcpy(name, micaps.szPname, ARRAYSIZE(name));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: OpenOutputDevice()
// Desc: Open output device by id
//-----------------------------------------------------------------------------
Result WinMidi::OpenOutputDevice(UINT deviceid, IMidiOutDevice** outdev)
{
	*outdev = NULL;

	if(deviceid >= numoutdevs)
		return ERR("Invalid device id");
	REMAPDEVICEID(deviceid); // Midi mapper acts as the first device

	Result rlt;

	HMIDIOUT devicehandle;
	CHKRESULT(midiOutOpen(&devicehandle, deviceid, NULL, NULL, CALLBACK_NULL));

	OutDevice* newoutdev;
	CHKALLOC(newoutdev = new OutDevice(devicehandle, this));

	outdevs.push_back(newoutdev);
	*outdev = newoutdev;

	/*byte data[] = {
		0xF0, //SysEx
		0x7E, //Non-Realtime
		0x7F, //The SysEx channel. Could be from 0x00 to 0x7F. Here we set it to "disregard channel".
		0x09, //Sub-ID -- GM System Enable/Disable
		0x01, //Sub-ID2 -- NN=00 for disable, NN=01 for enable
		0xF7  //End of SysEx
	};
	MIDIHDR msg = {0};
	msg.lpData = (LPSTR)data;
	msg.dwBufferLength = msg.dwBytesRecorded = sizeof(data);
	CHKRESULT(midiOutPrepareHeader(devicehandle, &msg, sizeof(MIDIHDR)));
	CHKRESULT(midiOutLongMsg(devicehandle, &msg, sizeof(MIDIHDR)));
	Sleep(1000);
	CHKRESULT(midiOutUnprepareHeader(devicehandle, &msg, sizeof(MIDIHDR)));*/

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: OpenInputDevice()
// Desc: Open input device by id
//-----------------------------------------------------------------------------
Result WinMidi::OpenInputDevice(UINT deviceid, IMidiInDevice** indev)
{
	*indev = NULL;

	if(deviceid >= numindevs)
		return ERR("Invalid device id");
	REMAPDEVICEID(deviceid); // Midi mapper acts as the first device

	Result rlt;

	InDevice* newindev;
	CHKALLOC(newindev = new InDevice(this));
	IFFAILED(newindev->Init(deviceid))
	{
		delete newindev;
		return rlt;
	}

	indevs.push_back(newindev);
	*indev = newindev;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize InDevice
//-----------------------------------------------------------------------------
Result InDevice::Init(UINT deviceid)
{
	Result rlt;

	CHKRESULT(midiInOpen(&hdl, deviceid, (DWORD_PTR)MidiInCallback, (DWORD_PTR)this, CALLBACK_FUNCTION));

	midiInGetDevCaps((UINT_PTR)hdl, &caps, sizeof(MIDIINCAPS));

	CHKRESULT(midiInStart(hdl));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SendNoteOn()
// Desc: Send note on through midi out
//-----------------------------------------------------------------------------
void OutDevice::SendNoteOn(byte channel, byte note, byte velo)
{
	midiOutShortMsg(hdl, 0x9 | (DWORD)channel << 4 | (DWORD)note << 8 | (DWORD)(velo >> 1) << 16);
}

//-----------------------------------------------------------------------------
// Name: SendNoteOff()
// Desc: Send note off through midi out
//-----------------------------------------------------------------------------
void OutDevice::SendNoteOff(byte channel, byte note, byte velo)
{
	midiOutShortMsg(hdl, 0x8 | (DWORD)channel << 4 | (DWORD)note << 8 | (DWORD)(velo >> 1) << 16);
}

WORD OutDevice::GetVolume()
{
	DWORD vol;
	if(midiOutGetVolume(hdl, &vol) == MMSYSERR_NOERROR)
	{
		if(!(caps.dwSupport & MIDICAPS_LRVOLUME))
			return (WORD)((vol & 0xFFFF) + ((vol >> 16) & 0xFFFF) / 2); // Return average of left and right volume
		else
			return LOWORD(vol); // Return left volume
	}
	else
		return 0;
}
void OutDevice::SetVolume(WORD val)
{
	if(!(caps.dwSupport & MIDICAPS_VOLUME))
		return; // No volume control support

	midiOutSetVolume(hdl, (DWORD)val | ((DWORD)val << 16));
}
WORD OutDevice::GetLVolume()
{
	DWORD vol;
	if(midiOutGetVolume(hdl, &vol) == MMSYSERR_NOERROR)
		return LOWORD(vol); // Return left volume
	else
		return 0;
}
void OutDevice::SetLVolume(WORD val)
{
	if(!(caps.dwSupport & MIDICAPS_VOLUME))
		return; // No volume control support

	DWORD vol;
	if(midiOutGetVolume(hdl, &vol) == MMSYSERR_NOERROR)
	{
		vol = (DWORD)val | (vol & 0xFFFF0000); // Set left volume
		midiOutSetVolume(hdl, vol);
	}
}
WORD OutDevice::GetRVolume()
{
	DWORD vol;
	if(midiOutGetVolume(hdl, &vol) == MMSYSERR_NOERROR)
		return HIWORD(vol); // Return right volume
	else
		return 0;
}
void OutDevice::SetRVolume(WORD val)
{
	if(!(caps.dwSupport & MIDICAPS_VOLUME))
		return; // No volume control support
	if(!(caps.dwSupport & MIDICAPS_LRVOLUME))
		return SetVolume(val); // No left/right volume control support

	DWORD vol;
	if(midiOutGetVolume(hdl, &vol) == MMSYSERR_NOERROR)
	{
		vol = (vol & 0xFFFF) | ((DWORD)val << 16); // Set right volume
		midiOutSetVolume(hdl, vol);
	}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void OutDevice::Release()
{
	parent->outdevs.remove(this);

	midiOutReset(hdl);
	midiOutClose(hdl);

	delete this;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void InDevice::Release()
{
	parent->indevs.remove(this);

	midiInReset(hdl);
	midiInClose(hdl);

	delete this;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void WinMidi::Release()
{
	while(outdevs.size())
		outdevs.front()->Release();

	while(indevs.size())
		indevs.front()->Release();

	delete this;
}