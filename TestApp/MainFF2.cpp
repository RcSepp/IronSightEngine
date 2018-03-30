#include <ISEngine.h>
#include <ISForms.h>
#include <ISFFmpeg.h>

#include <vld.h>


#ifdef _DEBUG
	#pragma comment (lib, "ISEngine_d.lib")
	#pragma comment (lib, "ISFFmpeg_d.lib")
#else
	#pragma comment (lib, "ISEngine.lib")
	#pragma comment (lib, "ISFFmpeg.lib")
#endif


Result __stdcall Init();
Result __stdcall PostInit();
void __stdcall End();

HINSTANCE __hInst;
int __nCmdShow;


LPWINFORM form1 = NULL;
LPMEDIAFILE movie = NULL;
uint8_t* buffer;

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow)
{
	Result rlt;

__hInst = hInst;
__nCmdShow = nCmdShow;

	// Create Engine, Forms and Direct3D
	if(!(eng = new Engine(hInst)) || !(fms = CreateForms()))
		MessageBox(NULL, "Out of memory", "Iron Sight Engine", 0);

	// Register the engines window class
	IFFAILED(eng->RegisterWinClass(NULL, (HBRUSH)COLOR_WINDOW)) //(HBRUSH)COLOR_WINDOW
		MessageBox(NULL, rlt.GetLastResult(), "Iron Sight Engine", 0);

	CoInitialize(NULL);

	// Enter main loop
	IFFAILED(eng->Run(Init, PostInit, NULL, End))
	{
		OutputDebugString(rlt.GetLastResult());
		OutputDebugString("\n");
	}

	CoUninitialize();

	delete eng;

	return 1;
}

Result __stdcall Init()
{
	Result rlt;

	// Init Forms
	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create WinForm
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(800, 600);
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(0, 0);
//frmsettings.wndstate = WS_MAXIMIZED;
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form1));

	// Init FFmpeg
	LPTSTR missingdllname;
	if(CheckFFmpegSupport(&missingdllname) == false)
		return ERR(String("Library not found: ") << String(missingdllname));
	CHKALLOC(ffm = CreateFFmpeg()); ffm->Sync(GLOBALVAR_LIST);
	CHKRESULT(ffm->Init());

	LPMEDIAFILE movie;
	ffm->CreateNew("out.avi", &movie);

	return R_OK;
}

Result __stdcall PostInit()
{
	return R_OK;
}

void __stdcall End()
{
	RELEASE(ffm);
	RELEASE(cle);
	RELEASE(fms);
}