#ifndef __ISMAIN_H
#define __ISMAIN_H

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <ISEngine.h>
#include <ISMath.h>

//#define DIRECT3D
#ifdef DIRECT3D
	#include <ISDirect3D.h>
#endif

//#define HAVOK
#ifdef HAVOK
	#include <ISHavok.h>
#endif

//#define GUIFACTORY
#ifdef GUIFACTORY
	#include <ISGuiFactory.h>
#endif

#define FORMS
#ifdef FORMS
	#include <ISForms.h>
	#pragma comment (lib, "UxTheme.lib")
	#pragma comment (lib, "Comctl32.lib")
#endif

//#define FFMPEG
#ifdef FFMPEG
	#include <ISFFmpeg.h>
#endif

//#define CRYPTOPP
#ifdef CRYPTOPP
	#include <ISCryptoPP.h>
#endif

//#define ID3LIB
#ifdef ID3LIB
	#include <ISID3Lib.h>
#endif

//#define WINAUDIO
#ifdef WINAUDIO
	#include <ISWinAudio.h>
#endif

//#define XAUDIO2
#ifdef XAUDIO2
	#include <ISXAudio2.h>
#endif

//#define WINMIDI
#ifdef WINMIDI
	#include <ISWinMidi.h>
#endif

//#define WINSOCK
#ifdef WINSOCK
	#include <ISWinSock.h>
#endif

//#define DIRECTINPUT
#ifdef DIRECTINPUT
	#include <ISDirectInput.h>
#endif

//#define WININPUT
#ifdef WININPUT
	#include <ISWinInput.h>
#endif

//#define ASYNCWORKERS
#ifdef ASYNCWORKERS
	#include <ISAsyncWorkers.h>
#endif

//#define SQLITE
#ifdef SQLITE
	#include <ISSQLite.h>
#endif

//#define HARUPDF
#ifdef HARUPDF
	#include <ISHaruPdf.h>
#endif

//#define PYTHON
#ifdef PYTHON
	#include <ISPythonScriptEngine.h>
#endif

#ifdef _DEBUG
	#pragma comment (lib, "ISEngine_d.lib")
	#pragma comment (lib, "ISMath_d.lib")
#else
	#pragma comment (lib, "ISEngine.lib")
	#pragma comment (lib, "ISMath.lib")
#endif


Result __stdcall Init();
Result __stdcall PostInit();

Result LoadFFmpeg();
void UnloadFFmpeg();
void OnPaint();

extern CYCLICFUNC cyclic;

void __stdcall End();

extern HINSTANCE __hInst;
extern int __nCmdShow;

#endif