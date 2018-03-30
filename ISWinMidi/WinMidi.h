#ifndef __WinMidi_H
#define __WinMidi_H

#include "ISWinMidi.h"
#include "..\\global\\MMResult.h"

#pragma comment (lib, "winmm.lib")


class OutDevice;
class InDevice;
class WinMidi;
DWORD __stdcall OutStreamingThread(LPVOID args);
DWORD __stdcall InStreamingThread(LPVOID args);

//-----------------------------------------------------------------------------
// Name: OutDevice
// Desc: Wrapper class for a single midi output device
//-----------------------------------------------------------------------------
class OutDevice : public IMidiOutDevice
{
private:
	MIDIOUTCAPS caps;
	WinMidi* parent;

public:
	HMIDIOUT hdl;

	OutDevice(HMIDIOUT devicehandle, WinMidi* parent)
	{
		this->hdl = devicehandle;
		this->parent = parent;
		midiOutGetDevCaps((UINT_PTR)devicehandle, &caps, sizeof(MIDIOUTCAPS));
	}
	void SendNoteOn(byte channel, byte note, byte velo);
	void SendNoteOff(byte channel, byte note, byte velo);
	WORD GetVolume();
	void SetVolume(WORD val);
	WORD GetLVolume();
	void SetLVolume(WORD val);
	WORD GetRVolume();
	void SetRVolume(WORD val);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: InDevice
// Desc: Wrapper class for a single midi input device
//-----------------------------------------------------------------------------
class InDevice : public IMidiInDevice
{
private:
	MIDIINCAPS caps;
	WinMidi* parent;

	static void CALLBACK MidiInCallback(HMIDIIN hmi, UINT msg, DWORD_PTR instance, DWORD_PTR wparam, DWORD_PTR lparam);

public:
	HMIDIIN hdl;
	NOTEDOWNCALLBACK notedowncbk;
	LPVOID notedowncbkuser;

	InDevice(WinMidi* parent)
		: notedowncbk(NULL), notedowncbkuser(NULL)
	{
		this->parent = parent;
	}
	Result Init(UINT deviceid);
	void RegisterNoteDownCallback(NOTEDOWNCALLBACK cbk, LPVOID user) {notedowncbk = cbk; notedowncbkuser = user;}
	/*WORD GetVolume();
	void SetVolume(WORD val);
	WORD GetLVolume();
	void SetLVolume(WORD val);
	WORD GetRVolume();
	void SetRVolume(WORD val);*/
	void Release();
};

//-----------------------------------------------------------------------------
// Name: WinMidi
// Desc: API to the midi functionality of windows multimedia library. Mainly used to open devices.
//-----------------------------------------------------------------------------
class WinMidi : public IWinMidi
{
private:
	String *outdevnames, *indevnames;
	UINT numindevs, numoutdevs;

public:
	std::list<OutDevice*> outdevs;
	std::list<InDevice*> indevs;

	WinMidi();
	void Sync(GLOBALVARDEF_LIST);
	Result Init();
	Result GetOutputDeviceTechnology(UINT deviceid, MidiDeviceTechnology* mdt) const;
	Result GetInputProductName(UINT deviceid, char (&name)[32]) const;
	Result OpenOutputDevice(UINT deviceid, IMidiOutDevice** outdev);
	Result OpenInputDevice(UINT deviceid, IMidiInDevice** indev);
	void Release();
};

#endif