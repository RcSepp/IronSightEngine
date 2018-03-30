#include "Main.h"

HINSTANCE __hInst;
int __nCmdShow;

//#define MOVIE_DEVELOP

//-----------------------------------------------------------------------------
// Name: EngWndProc()
// Desc: A custom message handler for the engine's top-level windows
//-----------------------------------------------------------------------------
bool __stdcall EngWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam, LPVOID user)
{
	Result rlt;

#ifdef MOVIE_DEVELOP
	switch(msg)
	{
	case WM_PAINT:
	case WM_DISPLAYCHANGE:
	case WM_TIMER:
	//case WM_KEYDOWN:
		OnPaint();
		InvalidateRect(fms->GetHwnd(0), NULL, FALSE);
		return false;
	}
#endif

	return true;
}

/*int AllocHook( int allocType, void *userData, size_t size, int 
blockType, long requestNumber, const unsigned char *filename, int 
lineNumber)
{
	return TRUE;
}*/

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow)
{
	Result rlt;

__hInst = hInst;
__nCmdShow = nCmdShow;

/*String foo("foo2qwe1abc12bla");
String newstr;
foo.Replace("2", "12", &newstr);
int abc = 0;*/

	/*// String & FilePath debugging
	FilePath path = "SomeFile.ext";
	FilePath path2;
	String str;
	path.AddSuffix(String("_1"), &path2);
	//path.Substring(4, 4, &str);
	//OutputDebugString(str << '\n');
	//path.Substring(4, 8, &str);
	OutputDebugString(str << '\n');
	OutputDebugString(path << '\n');
	OutputDebugString(path2 << '\n');*/

	/*// List debugging
	List<int> Pts;
	int* lpint = new int(2);
	Pts.Add(lpint);
	Pts.Add(new int(3));
	Pts.AddUpFront(new int(1));

	for(Pts.First(); Pts.Current(); Pts.Next())
		OutputDebugString(String(*Pts.Current()));
	OutputDebugString("\n");

	Pts.Destroy(lpint);

	for(Pts.Last(); Pts.Current(); Pts.Prev())
		OutputDebugString(String(*Pts.Current()));
	OutputDebugString("\n");

	Pts.DestroyAll();*/


	/*// Set start point for memory leak detection
	_CrtMemState s1, s2, s3;
	_CrtMemCheckpoint(&s1);

	// Hook into C run-time debug memory allocation process
	_CrtSetAllocHook(AllocHook);*/

	// Create Engine, Forms and Direct3D
	if(!(eng = new Engine(hInst)) || !(fms = CreateForms()))
		MessageBox(NULL, "Out of memory", "Iron Sight Engine", 0);

	// RegisterCustomWndProc
	IFFAILED(eng->RegisterCustomWndProc(EngWndProc, NULL))
		MessageBox(NULL, rlt.GetLastResult(), "Iron Sight Engine", 0);

	// Register the engines window class
	IFFAILED(eng->RegisterWinClass(NULL, (HBRUSH)COLOR_WINDOW)) //(HBRUSH)COLOR_WINDOW
		MessageBox(NULL, rlt.GetLastResult(), "Iron Sight Engine", 0);

	OleInitialize(NULL); // WinForm::DragDrop requests OLE (calls CoInitialize() within), others just need COM

	// Enter main loop
	IFFAILED(eng->Run(Init, PostInit, cyclic, End))
	{
		OutputDebugString(rlt.GetLastResult());
		OutputDebugString("\n");
	}
		//MessageBox(NULL, rlt.GetLastResult(), "Iron Sight Engine", 0);

	OleUninitialize();

	delete eng;

	/*// Unhook C run-time debug memory allocation process
	_CrtSetAllocHook(NULL);

	// Check memory leaks
	_CrtMemCheckpoint(&s2);
	if(_CrtMemDifference(&s3, &s1, &s2))
	{
		_CrtMemDumpStatistics(&s3);
		_CrtDumpMemoryLeaks();
	}*/

	return 1;
}