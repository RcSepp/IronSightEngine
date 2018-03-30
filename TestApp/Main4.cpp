#include <ISEngine.h>
#include <ISMath.h>
#include <ISDirect3D.h>
#include <ISDirectInput.h>
#include <ISHavok.h>
#include <ISForms.h>
//#pragma comment (lib, "UxTheme.lib")
//#pragma comment (lib, "Comctl32.lib")
#include <ISPythonScriptEngine.h>

#include <vld.h>

#include <DxErr.h> //DELETE
#pragma comment (lib, "DxErr.lib") //DELETE
#pragma comment (lib, "d3d10.lib") //DELETE
#pragma comment (lib, "dxgi.lib") //DELETE
#ifdef _DEBUG //DELETE
	#pragma comment (lib, "d3dx10d.lib") //DELETE
#else //DELETE
	#pragma comment (lib, "d3dx10.lib") //DELETE
#endif //DELETE


#ifdef _DEBUG
	#pragma comment (lib, "ISEngine_d.lib")
	#pragma comment (lib, "ISMath_d.lib")
#else
	#pragma comment (lib, "ISEngine.lib")
	#pragma comment (lib, "ISMath.lib")
#endif


Result __stdcall Init();
Result __stdcall PostInit();
void __stdcall Cyclic();
void __stdcall End();

HINSTANCE __hInst;
int __nCmdShow;


LPWINFORM form1 = NULL;
/*LPOUTPUTWINDOW d3dwnd = NULL;
LPCAMERA cam;
LPTEXTURE tex, tex2;
LPSPRITECONTAINER sprite;
LPOBJECT objBox;*/

PyObject* pycyclicfunc = NULL;

void __stdcall OnSpace(int key, LPVOID user);

_CrtMemState s1;
void StartMemLog()
{
// Set start point for memory leak detection
_CrtMemCheckpoint(&s1);
}
void EndMemLog()
{
// Check memory leaks
_CrtMemState s2, s3;
_CrtMemCheckpoint(&s2);
if(_CrtMemDifference(&s3, &s1, &s2))
{
	_CrtMemDumpStatistics(&s3);
	_CrtDumpMemoryLeaks();
}
}

void __stdcall OnPythonException(const FilePath& scriptname, int linenumber, PythonDictionary& globals, PythonDictionary& locals, const String& extype, const String& exstring, LPVOID user)
{
	MessageBox(NULL, exstring, scriptname << String(" @ line ") << String(linenumber), 0);
}

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

	//OleInitialize(NULL); // WinForm::DragDrop requests OLE (calls CoInitialize() within), others just need COM

	// Enter main loop
	IFFAILED(eng->Run(Init, PostInit, Cyclic, End)) // Non-event-driven
	{
		OutputDebugString(rlt.GetLastResult());
		OutputDebugString("\n");
	}

	//OleUninitialize();

	delete eng;

	return 1;
}

Result __stdcall Init()
{
	Result rlt;

	// Init PythonScriptEngine
	CHKALLOC(pse = CreatePythonScriptEngine()); pse->Sync(GLOBALVAR_LIST);

	RegisterScriptableEngineClasses(pse);
	RegisterScriptableFormsClasses(pse);
	RegisterScriptableMathClasses(pse);
	RegisterScriptableDirect3DClasses(pse);
	RegisterScriptableDirectInputClasses(pse);
	RegisterScriptableHavokClasses(pse);
	//RegisterScriptableWinAudioClasses(pse);
	//RegisterScriptableXAudio2Classes(pse);

	// Init script engine with all scriptable classes registered up to this moment
	pse->Init();
	VLDMarkAllLeaksAsReported(); // Avoid printing leaks from boost python
	SyncEngineWithPython();
	pse->HookExceptions(OnPythonException, NULL);

	// Init Forms
	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

//FormSettings frmsettings;
//frmsettings.fbstyle = FBS_DEFAULT;
//frmsettings.clientsize = Size<int>(1024, 1024);
//frmsettings.caption = "Iron Sight Engine - Window 1";
//frmsettings.windowpos = Point<int>(0, 0);
//CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form1));
//LPBUTTON button = CreateButton();
//button->Text = "I'm a button";
//form1->AddControl(button);
//return R_OK;

	/*// Create WinForm
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(1024, 1024);
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(0, 0);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form1));

	CHKRESULT(pse->ExecuteIntoBuiltins("MAIN_WINDOW", String("Engine.MakeHWND(") << String((int)form1->GetHwnd()) << String(")")));*/

	// Init DirectInput
	CHKALLOC(dip = CreateDirectInput()); dip->Sync(GLOBALVAR_LIST);

	/*CHKRESULT(dip->Init());
	CHKRESULT(dip->EnableMouse(true, false));
	CHKRESULT(dip->EnableKeyboard(true, false));

	dip->DisableKeyboard();
	CHKRESULT(dip->EnableKeyboard(false, false));
	dip->AddKeyHandler(Key::Space, OnSpace, NULL);*/


	// Init Direct3D
	CHKALLOC(d3d = CreateDirect3D()); d3d->Sync(GLOBALVAR_LIST);
	/*D3dStartupSettings d3dsettings;
	d3dsettings.isfullscreen = false;
	CHKRESULT(d3d->Init(&d3dsettings));

	IOutputWindow::Settings wndsettings;
	wndsettings.backbuffersize = Size<UINT>(1920, 1200);
	wndsettings.wnd = form1->GetHwnd();
	wndsettings.enablemultisampling = true;
	wndsettings.screenidx = 0;
	CHKRESULT(d3d->CreateOutputWindow(&wndsettings, &d3dwnd));

	d3dwnd->EnableSprites("K:\\Resources\\Shader\\Sprite.fx");
	d3dwnd->MakeConsoleTarget(11, 11);
	d3dwnd->ShowFps();

	cle->PrintD3D(123);

CHKRESULT(d3d->CreateCamera(90.0f * PI / 180.0f, 0.001f, 200.0f, &cam));
cam->pos = Vector3(0.0f, 0.0f, -2.0f);
	cam->viewmode = ICamera::VM_FLY;
	d3dwnd->SetCamera(cam);

	CHKRESULT(d3dwnd->CreateTexture(FilePath("Pause.png"), ITexture::TU_SKIN_NOMIPMAP, true, &tex));
	CHKRESULT(d3dwnd->CreateTexture(FilePath("heightmap_64x64.png"), ITexture::TU_SKIN_NOMIPMAP, true, &tex2));
	CHKRESULT(d3dwnd->CreateSpriteContainer(16, &sprite));
	d3dwnd->RegisterForRendering(sprite, RT_FOREGROUND);


	LPSKYBOX skybox;
	CHKRESULT(d3dwnd->CreateSkyBox(&skybox));
	CHKRESULT(skybox->CreateLayer(FilePath("SkyBox\\North_Sky.bmp"), FilePath("SkyBox\\East_Sky.bmp"), FilePath("SkyBox\\South_Sky.bmp"),
								  FilePath("SkyBox\\West_Sky.bmp"), FilePath("SkyBox\\Top.bmp"), FilePath("SkyBox\\Bottom.bmp"), 0));
	CHKRESULT(skybox->CreateLayer(FilePath("SkyBox\\North_Scene.dds"), FilePath("SkyBox\\East_Scene.dds"), FilePath("SkyBox\\South_Scene.dds"),
								  FilePath("SkyBox\\West_Scene.dds"), FilePath(""), FilePath(""), 8));
	d3dwnd->RegisterForRendering(skybox, RT_BACKGROUND);
	d3dwnd->backcolorenabled = false; // Disable screen clearing


	LPHUD hud;
	CHKRESULT(d3dwnd->CreateHUD(&hud));
	hud->CreateElement(tex2, 100, 0, DT_TOPLEFT, 0xFFFFFFFF, Rect<int>(25, 25, 50, 50), 1);
	hud->CreateElement(tex2, 120, 0, DT_TOPLEFT, 0xFFFFFFFF, Rect<int>(25, 25, 50, 50), 0);
	d3dwnd->RegisterForRendering(hud, RT_FOREGROUND);*/

	// Init Havok
	CHKALLOC(hvk = CreateHavok()); hvk->Sync(GLOBALVAR_LIST);

	LPPYTHONSCRIPT script;
	//CHKRESULT(pse->LoadScript("sample3.py", &script));
	CHKRESULT(pse->LoadScript("K:\\VB\\ISGameDeveloper\\Debug\\TinyMiner\\Main.py", &script));
	int numfunctions;
	PyObject** scriptfunctions;
	String* scriptfuncnames;
	script->GetFunctions(&numfunctions, &scriptfunctions, &scriptfuncnames);
	for(int i = 0; i < numfunctions; i++)
		if(scriptfuncnames[i].Equals("Cyclic"))
			pycyclicfunc = scriptfunctions[i];

	return R_OK;
}

Result __stdcall PostInit()
{
	return R_OK;
}

void __stdcall Cyclic()
{
	if(pycyclicfunc)
		pse->Call(pycyclicfunc);
	else
	{
		// Update keyboard and mouse input
		if(dip)
			dip->Update();

		// Update physics
		if(hvk)
			hvk->Update();

		// Update graphics
		if(d3d)
			d3d->Render();
	}
}

void __stdcall OnSpace(int key, LPVOID user)
{
}

void __stdcall End()
{
	//RELEASE(dip);
	//RELEASE(d3d);
	RELEASE(cle);
	RELEASE(fms);
	RELEASE(pse);
}