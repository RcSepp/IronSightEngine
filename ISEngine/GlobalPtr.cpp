#include "ISEngine.h"

SyncedPtr<Engine> eng = NULL;
SyncedPtr<IDirect3D> d3d = NULL;
SyncedPtr<IOpenGL> ogl = NULL;
SyncedPtr<IHavok> hvk = NULL;
SyncedPtr<IGuiFactory> gui = NULL;
SyncedPtr<IForms> fms = NULL;
SyncedPtr<IDirectIpt> dip = NULL;
SyncedPtr<IWinIpt> wip = NULL;
SyncedPtr<IFFmpeg> ffm = NULL;
SyncedPtr<ICryptoPP> cpp = NULL;
SyncedPtr<IID3Lib> id3 = NULL;
SyncedPtr<IWinAudio> wad = NULL;
SyncedPtr<IXAudio2> xa2 = NULL;
SyncedPtr<IWinMidi> mid = NULL;
SyncedPtr<IWinSock> wsk = NULL;
SyncedPtr<IAsyncWorkers> aws = NULL;
SyncedPtr<ISQLite> sql = NULL;
SyncedPtr<IHaruPdf> pdf = NULL;
SyncedPtr<IRayTracer> rtr = NULL;
SyncedPtr<IPbrt> pbrt = NULL;
SyncedPtr<IPythonScriptEngine> pse = NULL;
SyncedPtr<Console> cle = NULL;