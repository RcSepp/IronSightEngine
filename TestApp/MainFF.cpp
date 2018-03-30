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
void __stdcall TimerElapsed(LPVOID user);
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

	CHKRESULT(ffm->LoadFile("L:\\Family Guy English\\Family Guy - Season 1\\101 - Death Has A Shadow.avi", &movie));
	//CHKRESULT(ffm->LoadFile("L:\\HD Movies\\X-Men Origins - First Class.ts", &movie));

	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	srand(t.LowPart);
LONGLONG videoframe = 10000;//4000000;
//videoframe = (rand() % 100) * 80;//(rand() % 100) * 80000;

	PixelFormat pixfmt = PIX_FMT_BGR24;//PIX_FMT_YUV420P;
	CHKRESULT(movie->PrepareVideoStreaming(0, &form1->ClientSize, pixfmt, SWS_AREA, &buffer)); //SWS_BICUBIC
	//CHKRESULT(movie->PrepareVideoStreaming(0, &Size<int>(640, 480), pixfmt, SWS_AREA, &buffer)); //SWS_BICUBIC
movie->SetVideoFrame(0, videoframe);

	//CHKRESULT(ffm->PrepareBlittingToWnd(fms->GetHwnd(0), &Rect<int>(0, 0, movie->GetVideoWidth(0), movie->GetVideoHeight(0))));
	CHKRESULT(ffm->PrepareBlittingToWnd(fms->GetHwnd(0), &Rect<int>(0, 0, form1->ClientWidth, form1->ClientHeight)));

//CHKRESULT(ffm->CreateNew("out.mp3", &movie));
//movie->CreateFrame(0,

	// Create timer
	Timer* tmr;
	eng->CreateTimer(TimerElapsed, 35, true, NULL, &tmr);

	return R_OK;
}

Result __stdcall PostInit()
{
	return R_OK;
}

void __stdcall TimerElapsed(LPVOID user)
{
	if(ffm && movie)
	{
		bool finishedstreaming;
		movie->StreamContent(eng->time.dt, &finishedstreaming);
		ffm->BlitToWnd(buffer);
		if(finishedstreaming)
			PostQuitMessage(0);
		//OutputDebugString(String((int)movie->GetVideoFrame(0)) << String('\n'));
	}
}

void UnloadFFmpeg()
{
	Result rlt;

	// Destroy movies
	if(movie)
	{
//		videoframe = movie->GetVideoFrame(0);
		movie->EndVideoStreaming(0);
		movie = NULL;
	}

	// Destroy FFmpeg
	if(ffm)
	{
		ffm->EndBlittingToWnd();
		RELEASE(ffm)
	}

#ifdef SND_DEVELOP
	// Destroy win-audio output device
	if(wad)
		RELEASE(wadoutdev);
#endif
}

void __stdcall End()
{
	UnloadFFmpeg();//RELEASE(ffm);
	RELEASE(cle);
	RELEASE(fms);
}