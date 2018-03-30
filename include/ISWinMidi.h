#ifndef __ISWINMIDI_H
#define __ISWINMIDI_H

#include <ISEngine.h>

//#include <mmsystem.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_WINMIDI
	#define WINMIDI_EXTERN_FUNC	__declspec(dllexport)
#else
	#define WINMIDI_EXTERN_FUNC	__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// UNIONS
//-----------------------------------------------------------------------------
union MIDIDataPtr
{
	char* cdata;
	short* sdata;
};


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum SoundDoneCallbackResult
	{SDCR_OK, SDCR_FINISHED, SDCR_NODATA};
enum MidiDeviceTechnology
{
	MDT_UNDEFINED = 0,
	MDT_HARDWARE_PORT = 1,
	MDT_SYNTHESIZER = 2,
	MDT_SQUAREWAVE_SYNTHESIZER = 3,
	MDT_FM_SYNTHESIZER = 4,
	MDT_MAPPER = 5,
	MDT_WAVETABLE_SYNTHESIZER = 6,
	MDT_SOFTWARE_SYNTHESIZER = 7
};


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
typedef void (__stdcall* NOTEDOWNCALLBACK)(byte note, byte velo);


//-----------------------------------------------------------------------------
// Name: IWadOutDevice
// Desc: Interface to the OutDevice class
//-----------------------------------------------------------------------------
typedef class IMidiOutDevice
{
public:

	virtual void SendNoteOn(byte channel, byte note, byte velo) = 0;
	virtual void SendNoteOff(byte channel, byte note, byte velo) = 0;
	virtual WORD GetVolume() = 0;
	virtual void SetVolume(WORD val) = 0;
	__declspec(property(get=GetVolume, put=SetVolume)) WORD Volume;
	virtual WORD GetLVolume() = 0;
	virtual void SetLVolume(WORD val) = 0;
	__declspec(property(get=GetLVolume, put=SetLVolume)) WORD LVolume;
	virtual WORD GetRVolume() = 0;
	virtual void SetRVolume(WORD val) = 0;
	__declspec(property(get=GetRVolume, put=SetRVolume)) WORD RVolume;
	virtual void Release() = 0;
}* LPMIDIOUTDEVICE;

//-----------------------------------------------------------------------------
// Name: IWadInDevice
// Desc: Interface to the InDevice class
//-----------------------------------------------------------------------------
typedef class IMidiInDevice
{
public:

	virtual void RegisterNoteDownCallback(NOTEDOWNCALLBACK cbk, LPVOID user) = 0;
	/*virtual WORD GetVolume() = 0;
	virtual void SetVolume(WORD val) = 0;
	__declspec(property(get=GetVolume, put=SetVolume)) WORD Volume;
	virtual WORD GetLVolume() = 0;
	virtual void SetLVolume(WORD val) = 0;
	__declspec(property(get=GetLVolume, put=SetLVolume)) WORD LVolume;
	virtual WORD GetRVolume() = 0;
	virtual void SetRVolume(WORD val) = 0;
	__declspec(property(get=GetRVolume, put=SetRVolume)) WORD RVolume;*/
	virtual void Release() = 0;
}* LPMIDIINDEVICE;

//-----------------------------------------------------------------------------
// Name: IWinMidi
// Desc: Interface to the WinMidi class
//-----------------------------------------------------------------------------
typedef class IWinMidi
{
public:

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init() = 0;
	virtual Result GetOutputDeviceTechnology(UINT deviceid, MidiDeviceTechnology* mdt) const = 0;
	virtual Result GetInputProductName(UINT deviceid, char (&name)[32]) const = 0;
	virtual Result OpenOutputDevice(UINT deviceid, IMidiOutDevice** outdev) = 0;
	virtual Result OpenInputDevice(UINT deviceid, IMidiInDevice** indev) = 0;
	virtual void Release() = 0;
}* LPWINMIDI;

extern "C" WINMIDI_EXTERN_FUNC LPWINMIDI __cdecl CreateWinMidi();

#endif