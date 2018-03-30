#include "XAudio.h"
#include "ISPythonScriptEngine.h"

//#define BOOST_PYTHON_STATIC_LIB
#include <boost\python.hpp>
#include <boost\python\suite\indexing\vector_indexing_suite.hpp>
using namespace boost::python;

AudioFormat* FromChannelMaskWrapper(int samplerate, int bits, int channelmask)
{
	return AudioFormat::FromChannelMask((DWORD)samplerate, (WORD)bits, (DWORD)channelmask);
}

BOOST_PYTHON_MODULE(XAudio2)
{
	// >>> Enums

	/*enum_<Speaker>("Speaker")
		.value("FrontLeft", SPEAKER_FRONT_LEFT)
		.value("FrontRight", SPEAKER_FRONT_RIGHT)
		.value("FrontCenter", SPEAKER_FRONT_CENTER)
		.value("Subwoofer", SPEAKER_LOW_FREQUENCY)
		.value("RearLeft", SPEAKER_BACK_LEFT)
		.value("RearRight", SPEAKER_BACK_RIGHT)
		.value("FrontLeftCenter", SPEAKER_FRONT_LEFT_OF_CENTER)
		.value("FrontRightCenter", SPEAKER_FRONT_RIGHT_OF_CENTER)
		.value("RearCenter", SPEAKER_BACK_CENTER)
		.value("SideLeft", SPEAKER_SIDE_LEFT)
		.value("SideRight", SPEAKER_SIDE_RIGHT)
		.value("TopCenter", SPEAKER_TOP_CENTER)
		.value("TopFrontLeft", SPEAKER_TOP_FRONT_LEFT)
		.value("TopFrontCenter", SPEAKER_TOP_FRONT_CENTER)
		.value("TopFrontRight", SPEAKER_TOP_FRONT_RIGHT)
		.value("TopRearLeft", SPEAKER_TOP_BACK_LEFT)
		.value("TopRearCenter", SPEAKER_TOP_BACK_CENTER)
		.value("TopRearRight", SPEAKER_TOP_BACK_RIGHT)
		.value("All", SPEAKER_ALL)
	;*/

	// >>> Structs

	WAVEFORMATEXTENSIBLE* (AudioFormat::*GetWaveFormatExtensible)(WAVEFORMATEXTENSIBLE* result) const = &AudioFormat::GetWaveFormat;
	WAVEFORMATEX* (AudioFormat::*GetWaveFormatEx)(WAVEFORMATEX* result) const = &AudioFormat::GetWaveFormat;
	class_<AudioFormat>("AudioFormat")
		.def(init<DWORD, WORD, DWORD, GUID>())
		.def(init<DWORD, WORD, DWORD>())
		.def(init<>())
		.def_readonly("samplerate", &AudioFormat::samplerate)
		.def_readonly("channelmask", &AudioFormat::channelmask)
		.def_readonly("channels", &AudioFormat::channels)
		.def_readonly("bits", &AudioFormat::bits)
		.def_readonly("subformat", &AudioFormat::subformat)
		.def("FromChannelMask", &FromChannelMaskWrapper, POLICY_MANAGED_BY_PYTHON)
		.def("GetWaveFormatExtensible", GetWaveFormatExtensible, POLICY_MANAGED_BY_C)
		.def("GetWaveFormatEx", GetWaveFormatEx, POLICY_MANAGED_BY_C)
		.def(self == AudioFormat())
		.def(self != AudioFormat())
		.def("__str__", &AudioFormat::ToString) //EDIT: Not working: boost converts string to char* using operator and tries to forward the string as char
	;
	/*class_<std::vector<AudioFormat>>("AudioFormatArray")
        .def(vector_indexing_suite<std::vector<AudioFormat>>())
    ;*/

	// >>> Classes

	class_<Sound, boost::noncopyable>("Sound", no_init)
		.add_property("format", make_getter(&Sound::format, POLICY_MANAGED_BY_C))
		.def("Play", &Sound::Play)
		.def("Pause", &Sound::Pause)
		.def("Stop", &Sound::Stop)
		.add_property("volume", &Sound::GetVolume, &Sound::SetVolume)
		.add_property("pitch", &Sound::GetPitch, &Sound::SetPitch)
	;
	objects::register_conversion<IXA2Sound, Sound>();
	objects::register_conversion<Sound, IXA2Sound>();

	class_<XAudio, boost::noncopyable>("XAudio2")
		.def("Init", &XAudio::Init)
		.def("CreateSound", &XAudio::CreateSoundFromFileWrapper, POLICY_MANAGED_BY_C)
		.add_property("volume", &XAudio::GetVolume, &XAudio::SetVolume)
		.def("Release", &XAudio::Release)
	;
}

XAUDIO2_EXTERN_FUNC void __cdecl RegisterScriptableXAudio2Classes(const IPythonScriptEngine* pse)
{
	if(pse)
		pse->AddModule("XAudio2", &PyInit_XAudio2);
}
void SyncWithPython()
{
	AddToBuiltins("xa2", (XAudio*)&*xa2);
}