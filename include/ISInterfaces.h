#ifndef __ISINTERFACES_H
#define __ISINTERFACES_H

//-----------------------------------------------------------------------------
// EVENT INHERITANCE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: IFormDependend
// Desc: Interface to a class depending on WinForm creation/destruction and focus gaining/loosing
//-----------------------------------------------------------------------------
class IFormDependend
{
public:
	virtual void OnCreateForm(HWND wnd) = 0;
	virtual void OnRemoveForm(HWND wnd) = 0;
	virtual void OnGainFocus(HWND wnd) = 0;
	virtual void OnLoseFocus(HWND wnd) = 0;
};

//-----------------------------------------------------------------------------
// Name: IKeyboardMonitoring
// Desc: Interface to a class monitoring WM_KEYDOWN/WM_KEYUP
//-----------------------------------------------------------------------------
class IKeyboardMonitoring
{
public:
	virtual void OnKeyDown(DWORD vkey) = 0;
	virtual void OnKeyUp(DWORD vkey) = 0;
};

class IManagedObject;
//-----------------------------------------------------------------------------
// Name: IObjectManager
// Desc: Interface to class that holds a list of managed objects that will be destroyed when this class is released
//-----------------------------------------------------------------------------
class IObjectManager
{
private:
	std::list<IManagedObject*> objectregister;

protected:
	void RegisterObject(IManagedObject* obj);
	void ReleaseRegisteredObjects(); // Should be called in the destructor of the implementing class

public:
	void UnregisterObject(IManagedObject* obj);
	virtual void Release() = 0;
};

//-----------------------------------------------------------------------------
// Name: IManagedObject
// Desc: Interface to class that will be destroyed when it's parent class is released
//-----------------------------------------------------------------------------
class IManagedObject
{
private:
	IObjectManager* manager;

protected:
	void Unregister(); // Should be called in the destructor of the implementing class

public:
	virtual void Release() = 0;
};


//-----------------------------------------------------------------------------
// GLOBAL ACCESS POINTERS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: eng
// Desc: Engine of ISEngine (declared in ISEngine.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class Engine> eng;

//-----------------------------------------------------------------------------
// Name: d3d
// Desc: Direct3D of ISDirect3D (declared in ISDirect3D.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IDirect3D> d3d;

//-----------------------------------------------------------------------------
// Name: ogl
// Desc: OpenGL of ISOpenGL (declared in ISOpenGL.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IOpenGL> ogl;

//-----------------------------------------------------------------------------
// Name: hvk
// Desc: Direct3D of ISDirect3D (declared in ISDirect3D.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IHavok> hvk;

//-----------------------------------------------------------------------------
// Name: gui
// Desc: Interface to the GuiFactory class of ISGuiFactory (declared in ISGuiFactory.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IGuiFactory> gui;

//-----------------------------------------------------------------------------
// Name: fms
// Desc: Interface to the Forms class of ISForms (declared in ISForms.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IForms> fms;

//-----------------------------------------------------------------------------
// Name: dip
// Desc: Interface to the DirectIpt class of ISDirectInput (declared in ISDirectInput.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IDirectIpt> dip;

//-----------------------------------------------------------------------------
// Name: wip
// Desc: Interface to the WinInput class of ISWinInput (declared in ISWinInput.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IWinIpt> wip;

//-----------------------------------------------------------------------------
// Name: ffm
// Desc: Interface to the FFmpeg class of ISFFmpeg (declared in ISFFmpeg.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IFFmpeg> ffm;

//-----------------------------------------------------------------------------
// Name: cpp
// Desc: Interface to the CryptPP class of ISCryptoPP (declared in ISCryptoPP.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class ICryptoPP> cpp;

//-----------------------------------------------------------------------------
// Name: id3
// Desc: Interface to the ID3Lib class of ISID3Lib (declared in ISID3Lib.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IID3Lib> id3;

//-----------------------------------------------------------------------------
// Name: wad
// Desc: Interface to the WinAudio class of ISWinAudio (declared in ISWinAudio.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IWinAudio> wad;

//-----------------------------------------------------------------------------
// Name: xa2
// Desc: Interface to the XAudio2 class of ISXAudio2 (declared in ISXAudio2.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IXAudio2> xa2;

//-----------------------------------------------------------------------------
// Name: mid
// Desc: Interface to the WinMidi class of ISWinMidi (declared in ISWinMidi.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IWinMidi> mid;

//-----------------------------------------------------------------------------
// Name: wsk
// Desc: Interface to the WinSock class of ISWinSock (declared in ISWinSock.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IWinSock> wsk;

//-----------------------------------------------------------------------------
// Name: aws
// Desc: Interface to the AsyncWorkers class of ISAsyncWorkers (declared in ISAsyncWorkers.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IAsyncWorkers> aws;

//-----------------------------------------------------------------------------
// Name: sql
// Desc: Interface to the SQLite class of ISSQLite (declared in ISSQLite.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class ISQLite> sql;

//-----------------------------------------------------------------------------
// Name: pdf
// Desc: Interface to the HaruPdf class of ISHaruPdf (declared in ISHaruPdf.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IHaruPdf> pdf;

//-----------------------------------------------------------------------------
// Name: rtr
// Desc: Interface to the RayTracer class of ISRayTracer (declared in ISRayTracer.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IRayTracer> rtr;

//-----------------------------------------------------------------------------
// Name: pbrt
// Desc: Interface to the Pbrt class of ISPbrt (declared in ISPbrt.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IPbrt> pbrt;

//-----------------------------------------------------------------------------
// Name: pse
// Desc: Interface to the PythonScriptEngine class of ISPythonScriptEngine (declared in ISPythonScriptEngine.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class IPythonScriptEngine> pse;

//-----------------------------------------------------------------------------
// Name: cls
// Desc: Console for text-IO of ISEngine (declared in ISEngine.h)
//-----------------------------------------------------------------------------
extern SyncedPtr<class Console> cle;

#define GLOBALVAR_LIST		eng, d3d, ogl, hvk, gui, fms, dip, wip, ffm, cpp, id3, wad, xa2, mid, wsk, aws, sql, pdf, rtr, pbrt, pse, cle
#define GLOBALVARDEF_LIST	SyncedPtr<Engine>& exeeng, SyncedPtr<IDirect3D>& exed3d, SyncedPtr<IOpenGL>& exeogl, SyncedPtr<IHavok>& exehvk, SyncedPtr<IGuiFactory>& exegui, \
							SyncedPtr<IForms>& exefms, SyncedPtr<IDirectIpt>& exedip, SyncedPtr<IWinIpt>& exewip, SyncedPtr<IFFmpeg>& exeffm, SyncedPtr<ICryptoPP>& execpp, \
							SyncedPtr<IID3Lib>& exeid3, SyncedPtr<IWinAudio>& exewad, SyncedPtr<IXAudio2>& exexa2, SyncedPtr<IWinMidi>& exemid, SyncedPtr<IWinSock>& exewsk, \
							SyncedPtr<IAsyncWorkers>& exeaws, SyncedPtr<ISQLite>& exesql, SyncedPtr<IHaruPdf>& exepdf, SyncedPtr<IRayTracer>& exertr, SyncedPtr<IPbrt>& exepbrt, \
							SyncedPtr<IPythonScriptEngine>& exepse, SyncedPtr<Console>& execle
#define SYNC_GLOBALVARS		{eng = exeeng; d3d = exed3d; ogl = exeogl; hvk = exehvk; gui = exegui; \
							 fms = exefms; dip = exedip; wip = exewip; ffm = exeffm; cpp = execpp; \
							 id3 = exeid3;  wad = exewad; xa2 = exexa2; mid = exemid; wsk = exewsk; \
							 aws = exeaws;  sql = exesql; pdf = exepdf; rtr = exertr; pbrt = exepbrt; \
							 pse = exepse; cle = execle;}

#endif