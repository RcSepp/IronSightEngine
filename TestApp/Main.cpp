#include "Main.h"
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



#define D3D_DEVELOP
#define HVK_DEVELOP
//#define FMS_DEVELOP
//#define SND_DEVELOP
//#define WAD_DEVELOP
//#define XA2_DEVELOP
//#define MOVIE_DEVELOP
//#define GUI_DEVELOP
//#define MIDI_DEVELOP
//#define WSK_DEVELOP
//#define SQL_DEVELOP
//#define PDF_DEVELOP
//#define PSE_DEVELOP

#ifdef D3D_DEVELOP
	#include <ISDirect3D.h>
	#include <ISDirectInput.h>

	void __stdcall Render(IOutputWindow* wnd);
	void __stdcall Render2(IOutputWindow* wnd);
#endif

#ifdef HVK_DEVELOP
	#include <ISHavok.h>
#endif

LPWINFORM form1 = NULL;


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

#ifdef GUI_DEVELOP
#include <Uxtheme.h>
#include <Vssym32.h>
void DrawMyControl(HDC hDC, HWND hwndButton, HTHEME hTheme, int iState)
{
	RECT rcContent;
	RECT rc = {12, 532, 12+150, 532+52};
	TCHAR szButtonText[255];
	String ButtonText;
	HRESULT hr;

static int bla = 0;
rc.top += bla;
rc.bottom += bla;
bla += 52;

	//GetWindowRect(hwndButton, &rc);
	GetWindowText(hwndButton, szButtonText, (sizeof(szButtonText)/sizeof(szButtonText[0])+1));
	ButtonText = String(szButtonText);
	if(hTheme)
	{
		hr = DrawThemeBackground(hTheme, hDC, BP_RADIOBUTTON, iState, &rc, 0);
		// Always check your result codes.

		hr = GetThemeBackgroundContentRect(hTheme, hDC, BP_RADIOBUTTON, iState, &rc, &rcContent);
		hr = DrawThemeText(hTheme, hDC, BP_RADIOBUTTON, iState, ButtonText.ToWCharString(), ButtonText.length(), DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rcContent);
	}
	else
	{
		// Draw the control without using visual styles.
	}
}
#endif

#ifdef SND_DEVELOP
LPWADOUTDEVICE wadoutdev;
LPWADINDEVICE wadindev;
#endif

#ifdef MIDI_DEVELOP
LPMIDIINDEVICE midindev;
LPMIDIOUTDEVICE midoutdev;
#endif

#ifdef D3D_DEVELOP
LPOBJECT obj = NULL, obj2 = NULL;
LPOUTPUTWINDOW d3dwnd = NULL;
LPOBJECT sphere = NULL;
LPWEAPON weapon = NULL;
LPBOXEDLEVEL blevel = NULL;
void __stdcall OnSpace(int key, LPVOID user);
void __stdcall OnEnter(int key, LPVOID user);
void __stdcall OnPlus(int key, LPVOID user);
void __stdcall OnMinus(int key, LPVOID user);
#endif

#ifdef HVK_DEVELOP
LPRAGDOLLENTITY entity2_ragdoll = NULL;
LPPOSE entity2_pose = NULL;
LPVEHICLE vehicle = NULL;
LPANIMATION ani = NULL, ani2 = NULL;
#endif

#if defined(MOVIE_DEVELOP) || defined(SND_DEVELOP)
HMODULE isffmpeg_lib = NULL;
FARPROC isffmpeg_procs[2];
#define CreateFFmpeg			((LPFFMPEG (__cdecl*)())isffmpeg_procs[0])
#define CheckFFmpegSupport		((bool (__cdecl*)(LPTSTR* missingdllname))isffmpeg_procs[1])

LPMEDIAFILE movie = NULL, movie2 = NULL;
LONGLONG videoframe = 4000000;//4150000;
uint8_t* buffer;
#endif

#ifdef WSK_DEVELOP
LPWSUNIFIED wsu;
#endif

//#ifdef MOVIE_DEVELOP
//HMODULE isffmpeg_lib = NULL;
//FARPROC isffmpeg_procs[2];
//#define CreateFFmpeg			((IFFmpeg* (WINAPI*)(GLOBALVARDEF_LIST))isffmpeg_procs[0])
//#define CheckFFmpegSupport		((bool (WINAPI*)(LPTSTR* missingdllname))isffmpeg_procs[1])
//
//LPMEDIAFILE movie = NULL, movie2 = NULL;
//LONGLONG videoframe = 4000000;//4150000;
//uint8_t* buffer;
//#endif

void __stdcall PrintFunc(String message)
{
	cle->PrintD3D(message);
}

#ifdef SND_DEVELOP
SoundDoneCallbackResult __stdcall AudioStreamingCallback(LPWADSTREAMEDSOUND snd, AudioDataPtr data, LPVOID user)
{
Result rlt;
bool finishedstreaming;
/*if(movie)
{
	int audiodatalen = (int)snd->bufferlen;
	IFFAILED(movie->StreamAudioDirect(&finishedstreaming, data.sdata, &audiodatalen))
		cle->Print(rlt.GetLastResult());
}*/

/*static bool dec_initialized = false;
static LPENCRYPTION dec;
if(!dec_initialized)
{
	dec_initialized = true;
	cpp->CreateEncryption(CM_CFB, EM_DECRYPT, &dec);
	BYTE key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	BYTE iv[] = {0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0};
	dec->SetKeyWithIV(key, 16, iv, 16);
}*/

static AVPacket pkt = {0}; // Make size of first packet 0
static uint8_t* pktdata = NULL;
static int cipherptr = 0;
if(movie)
{
	int audiodatalen;
	do
	{
		audiodatalen = (int)snd->bufferlen;

		if(pkt.size == 0)
		{
			if(pktdata)
			{
				// Restore data pointer for memory freeing
				pkt.data = pktdata;
				// Free old packet
				ffm->FreePacket(&pkt);
			}

			// Read new packet
			IFFAILED(movie->ReadRawPacket(&pkt, NULL, &finishedstreaming))
				cle->Print(rlt.GetLastResult());

			// Store data pointer
			pktdata = pkt.data;
		}

		/*// Directly take over header to decryption result
		dec->GetResult().assign((char*)pkt.data, 4);

		// Decrypt message body
		dec->Put(pkt.data + 4, pkt.size - 4);

		// Reset packet data pointer from the encrypted to the unencrypted data
		pkt.data = (uint8_t*)dec->GetResult().c_str() + cipherptr;*/

		// Decode unencrypted data
		double time;
		IFFAILED(movie->DecodeAudioPacket(&pkt, data.sdata, &audiodatalen, &time))
			cle->Print(rlt.GetLastResult());

		//movie->FreePacket(&pkt);
	}
	while(audiodatalen == 0);
}
return SDCR_OK;
}

void __stdcall cmdPause_OnClick(LPCONTROL sender)
{
	/*if(sender->Text == String("Pause"))
	{
		sender->Text = String("Play");
		if(movie)
			wadoutdev->Pause();
	}
	else
	{
		sender->Text = String("Pause");
		if(movie)
			wadoutdev->Resume();
	}*/
}
#endif

#ifdef GUI_DEVELOP
void __stdcall cmdPauseNew_OnClick(LPGUICONTROL sender, const MouseEventArgs& args, LPVOID user)
{
	cle->Print("PAUSE CLICKED");
}
void __stdcall sldVolume_OnValueChanged(LPSLIDERCONTROL sender, float newvalue, LPVOID user)
{
	cle->Print(newvalue); cle->Print("\n");
}
void __stdcall c0_MouseMove(LPGUICONTROL sender, const MouseEventArgs& args, LPVOID user)
{
	cle->Print("0");
}
void __stdcall c1_MouseMove(LPGUICONTROL sender, const MouseEventArgs& args, LPVOID user)
{
	cle->Print("1");
}
#endif

void __stdcall OnDragEnter(IDataObject* dataobj, DWORD keystate, Point<int>& mousepos, DWORD* dropeffect, LPVOID user)
{
	*dropeffect = DROPEFFECT_MOVE | DROPEFFECT_SCROLL;
}

#ifdef WSK_DEVELOP
#include <fstream>
std::ofstream* ofs = NULL;
void __stdcall OnWSKReiceive(char* buffer, int buflen, LPWSUNIFIED wsu)
{
	/*cle->Print("Received: ");
	cle->PrintLine(buffer);*/
	if(ofs)
		ofs->write(buffer, buflen);
}

void __stdcall OnApplicationFound(const String& clientname, const String& address, LPVOID user)
{
	MessageBox(NULL, String("Server: Client application found on PC ") << clientname << String(" with address ") << address, "", 0);
	if(wsk->AcceptLanConnection(IP_V4, address, 31414, 31415) == R_OK)
		MessageBox(NULL, String("Server: Connection established with ") << address, "", 0);
}

void __stdcall OnApplicationAccepted(const String& address, LPVOID user)
{
	MessageBox(NULL, String("Client: Connection established with ") << address, "", 0);
}

bool __stdcall EngWndProc2(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam, LPVOID user)
{
	if(msg == WM_APP + 123)
		wsk->Update();

	return true;
}
#endif

void __stdcall MenuItem_OnClick(IMenuItem* sender, LPVOID user)
{
	fms->CloseForm(0);
	//cle->Print(SendMessage(fms->GetHwnd(0), WM_CLOSE, 0, 0));
	//PostQuitMessage(0);//cle->PrintLine(String("Menu item ") << sender->Text << String(" says hi!") );
}

#ifdef GUI_DEVELOP
Point<int> mdownpos;
Point<float> mdowntranslation;
void __stdcall Form1_OnMouseDown(IWinForm* sender, const MouseEventArgs& args, LPVOID user)
{
	if(args.rdown)
	{
		LPGUIOUTPUTWINDOW wnd = (LPGUIOUTPUTWINDOW)user;
		mdownpos = args.mousepos;
		mdowntranslation = wnd->Translation;
	}
}
void __stdcall Form1_OnMouseMove(IWinForm* sender, const MouseEventArgs& args, LPVOID user)
{
	if(args.rdown)
	{
		LPGUIOUTPUTWINDOW wnd = (LPGUIOUTPUTWINDOW)user;
		wnd->Translation = mdowntranslation + Point<float>((float)(args.mousepos.x - mdownpos.x), (float)(args.mousepos.y - mdownpos.y));
		wnd->Invalidate();
	}
}
void __stdcall Form1_OnMouseWheel(IWinForm* sender, const MouseEventArgs& args, LPVOID user)
{
	LPGUIOUTPUTWINDOW wnd = (LPGUIOUTPUTWINDOW)user;
	float scale = wnd->Scale.x;
	scale *= 1.0f + args.wheelpos / 1200.0f;
	wnd->Scale = Point<float>(scale, scale);
	wnd->Invalidate();
}
#endif

#ifdef PSE_DEVELOP
void __stdcall OnPythonException(const FilePath& scriptname, int linenumber, PythonDictionary& globals, PythonDictionary& locals, const String& extype, const String& exstring, LPVOID user)
{
	MessageBox(NULL, exstring, scriptname << String(" @ line ") << String(linenumber), 0);
}
#endif

Result __stdcall Init()
{
	Result rlt;

/*FilePath dir("H:\\Ierbecher\\");
FilePath::FindHandle hfind;
WIN32_FIND_DATA fdata;
while(dir.GetDirectories(&hfind, &fdata))
{
	cle->Print("H:\\Ierbecher\\");
	cle->Print(fdata.cFileName);
	cle->Print("\n");
}
cle->Print("\n");
while(dir.GetFiles(&hfind, &fdata))
{
	cle->Print("H:\\Ierbecher\\");
	cle->Print(fdata.cFileName);
	cle->Print("\n");
}*/

#ifdef PSE_DEVELOP
	// Init PythonScriptEngine
	CHKALLOC(pse = CreatePythonScriptEngine()); pse->Sync(GLOBALVAR_LIST);

	RegisterScriptableEngineClasses(pse);
//#ifdef FMS_DEVELOP
	RegisterScriptableFormsClasses(pse);
//#endif
#ifdef D3D_DEVELOP
	RegisterScriptableDirect3DClasses(pse);
	RegisterScriptableDirectInputClasses(pse);
#endif
#ifdef HVK_DEVELOP
	RegisterScriptableHavokClasses(pse);
#endif
#ifdef WAD_DEVELOP
	RegisterScriptableWinAudioClasses(pse);
#endif
#ifdef XA2_DEVELOP
	RegisterScriptableXAudio2Classes(pse);
#endif

	// Init script engine with all scriptable classes registered up to this moment
	pse->Init();
	VLDMarkAllLeaksAsReported(); // Avoid printing leaks from boost python
	SyncEngineWithPython();
	pse->HookExceptions(OnPythonException, NULL);
#endif

	// Init Forms
	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));


	// Create WinForm
	FormSettings frmsettings;
	//frmsettings.hashscroll = frmsettings.hasvscroll = true;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(1024, 1024);//(512, 512);
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(0, 0);//(500, 256);
	//frmsettings.options = FS_MDICLIENT;
	//frmsettings.mdiwindowmenu = (LPMENUITEM*)new LPVOID();
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form1));

#ifdef PSE_DEVELOP
	CHKRESULT(pse->ExecuteIntoBuiltins("MAIN_WINDOW", String("Engine.MakeHWND(") << String((int)form1->GetHwnd()) << String(")")));
#endif

#ifdef FMS_DEVELOP
#ifndef PSE_DEVELOP
	/*LPOPENFILEDIALOG ofd = CreateOpenFileDialog();
	CHKRESULT(ofd->Init());
	IOpenFileDialog::FileType filetypes[] = {{"Text Document", "txt"}, {"All Documents", "*"}};
	CHKRESULT(ofd->SetFileTypes(filetypes, ARRAYSIZE(filetypes)));
	CHKRESULT(ofd->SetDefaultExtension("txt"));
	CHKRESULT(ofd->Show());
	if(ofd->numfiles) cle->PrintLine(ofd->files[0]);*/

	/*LPWINFORM form2;
	frmsettings.options = FS_MDICHILD;
	frmsettings.parent = form1;
	frmsettings.caption = "form2";
	frmsettings.clientsize = Size<int>(200, 200);
	frmsettings.windowpos = Point<int>(0, 0);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form2));

	LPWINFORM form3;
	frmsettings.options = FS_MDICHILD;
	frmsettings.parent = form1;
	frmsettings.caption = "form3";
	frmsettings.clientsize = Size<int>(200, 200);
	frmsettings.windowpos = Point<int>(256, 256);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form3));*/

	/*LPMENU titlemenu;
	LPMENUITEM titlemenuitem;
	CHKRESULT(form1->CreateTitleMenu(&titlemenu));
	CHKRESULT(titlemenu->AddMenuItem("test1", IMenuItem::MI_DROPDOWN, NULL, NULL, NULL, &titlemenuitem));
	CHKRESULT(titlemenu->AddMenuItem("test2", IMenuItem::MI_BUTTON, titlemenuitem));
	CHKRESULT(titlemenu->AddMenuItem("", IMenuItem::MI_SEPARATOR));
	CHKRESULT(titlemenu->AddMenuItem("test3", IMenuItem::MI_BUTTON, titlemenuitem));
	CHKRESULT(titlemenu->AddMenuItem(*frmsettings.mdiwindowmenu, NULL, "foo"));
	titlemenu->Show();*/

	/*LPTOOLBAR toolbar;
	LPTOOLBARITEM dropdown;
	CHKRESULT(form1->CreateToolbar(false, &toolbar));
	CHKRESULT(toolbar->AddButton("foo", IToolbarItem::TBI_WHOLEDROPDOWN, NULL, NULL, &dropdown));
	CHKRESULT(toolbar->AddButton("abc", IToolbarItem::TBI_BUTTON));

	LPMENU dropdownmenu;
	CHKRESULT(dropdown->CreateDropDownMenu(&dropdownmenu));
	CHKRESULT(dropdownmenu->AddMenuItem("test item"));*/

	/*LPTREEVIEW treeview = CreateTreeview();
	treeview->Size = form1->ClientSize;
	form1->AddControl(treeview);
	LPTREEVIEWITEM item1;
	treeview->AddItem("test", NULL, &item1);
	treeview->AddItem("test2", item1);
	treeview->AddItem("test3", item1);*/

	//// Create another WinForm
	//frmsettings.caption = String("Iron Sight Engine - Window 2");
	//frmsettings.windowpos = Point<int>(1004, 100);
	//CHKRESULT(fms->CreateForm(&frmsettings));

//fms->RemoveForm(0);

	/*form1->SetDragEnterCallback(OnDragEnter, NULL);
	CHKRESULT(form1->EnableDragDrop());*/

	/*LPTRAYICON trayicon;
	CHKRESULT(form1->CreateTrayIcon("Icon.ico", &trayicon));
	CHKRESULT(trayicon->SetToolTipText("This is a test"));
	CHKRESULT(trayicon->SetNotifycationBaloon("This is some text", "I am a title", "", ITrayIcon::IT_WARNING));

	LPMENU trayiconmenu;
	CHKRESULT(trayicon->CreateContextMenu(&trayiconmenu));
	CHKRESULT(trayiconmenu->AddMenuItem("test item", MenuItem_OnClick));*/
#endif
#endif

#ifdef MIDI_DEVELOP
	// Init WinMidi
	MidiDeviceTechnology mdv;
	CHKALLOC(mid = CreateWinMidi()); mid->Sync(GLOBALVAR_LIST);
	CHKRESULT(mid->Init());
	mid->GetOutputDeviceTechnology(0, &mdv); cle->Print(String(mdv));

	// Init win-audio input device
	CHKRESULT(mid->OpenInputDevice(0, &midindev));
	CHKRESULT(mid->OpenOutputDevice(0, &midoutdev));
	midoutdev->Volume = 0xFFFF;//0x1000;
#endif

#ifdef WSK_DEVELOP
	// Init WinSock
	CHKALLOC(wsk = CreateWinSock()); wsk->Sync(GLOBALVAR_LIST);
	CHKRESULT(wsk->Init(fms->GetHwnd(0), WM_APP + 123));
	CHKRESULT(eng->RegisterCustomWndProc(EngWndProc2, NULL));

LPTEXTBOX txt = CreateTextBox();
form1->AddControl(txt);

/* // LAN Connector
if(MessageBox(NULL, "", "", MB_YESNO) == IDYES)
{
	txt->SetText(String("Client"));

	LPLCCLIENT lcclient;
	CHKRESULT(wsk->CreateAndSetupLanConnectorClient(IP_V4, 31414, 31415, wsk->GetComputerName(), &lcclient));
	lcclient->SetConnectionAcceptCallback(OnApplicationAccepted, NULL);
}
else
{
	txt->SetText(String("Server"));
	CHKRESULT(wsk->QuerryLanConnections(IP_V4, "192.168.1.255", 31414, 31415, OnApplicationFound));
}*/

CHKRESULT(wsk->CreateAndSetupUnified(IP_V4, OnWSKReiceive, "127.0.0.1", "27015", &wsu));
if(wsu->IsServer())
{
	wsu->SendFile("H:\\Backup D\\Pictures\\Clock2.bmp");
}
else
{
	ofs = new std::ofstream("out.bmp");
}

/*CHKRESULT(wsk->CreateUdpBridge(IP_V4, OnWSKReiceive, "255.255.255.255", 8080, 8080, &wsu));
CHKRESULT(wsu->SendData("client -> server", strlen("client -> server")));*/

	/*//CHKRESULT(wsk->CreateAndSetupUnified(IP_V4, OnWSKReiceive, "192.168.1.6", "27015", &wsu)); //"fe80::99ac:5c78:66a3:b3b6%10"
	CHKRESULT(wsk->CreateUdpBridge(IP_V4, OnWSKReiceive, "192.168.1.2", 8080, 8081, &wsu));
	//CHKRESULT(wsu->SendData("this is a test", strlen("this is a test")));
	//DWORD ping;
	//CHKRESULT(wsu->Ping(ping));
	//cle->Print("Ping: "); cle->Print((int)ping); cle->Print("\n");


	LPWSUDPBRIDGE wsu2;
	CHKRESULT(wsk->CreateUdpBridge(IP_V4, OnWSKReiceive, "192.168.1.2", 8081, 8080, &wsu2));
	CHKRESULT(wsu->SendData("this is a test", strlen("this is a test")));
	DWORD ping;
	CHKRESULT(wsu->Ping(ping));
	cle->Print("Ping: "); cle->Print((int)ping); cle->Print("\n");*/
#endif

#ifdef GUI_DEVELOP
	// Init GuiFactory
	CHKALLOC(gui = CreateGuiFactory()); gui->Sync(GLOBALVAR_LIST);
	CHKRESULT(gui->Init());

	// Create output window
	LPGUIOUTPUTWINDOW wnd;
	CHKRESULT(gui->CreateOutputWindow(fms->GetHwnd(0), &wnd));

	GradientStop stops[] = {GradientStop(0.00f, Color(0.898f, 0.933f, 1.0f, 0.2f)),
							GradientStop(0.65f, Color(0.749f, 0.835f, 1.0f, 0.1f)),
							GradientStop(1.00f, Color(0.639f, 0.769f, 1.0f, 0.1f))};

	LPGUIBRUSH sbr;
	CHKRESULT(wnd->CreateSolidBrush(0.290f, 0.494f, 0.733f, 0.3f, &sbr));

LPGUIBRUSH s0, s1;
CHKRESULT(wnd->CreateSolidBrush(0.290f, 0.494f, 0.733f, 1.0f, &s0));
CHKRESULT(wnd->CreateSolidBrush(0.733f, 0.290f, 0.290f, 1.0f, &s1));

LPGUITEXTFORMAT txtfmt;
CHKRESULT(gui->CreateTextFormat("Verdana", 15, false, &txtfmt));
wnd->DrawTextField(txtfmt, "this is a test", s0, 100.0f, 100.0f, 200.0f, 100.0f);

form1->SetMouseDownCallback(Form1_OnMouseDown, wnd);
form1->SetMouseMoveCallback(Form1_OnMouseMove, wnd);
form1->SetMouseWheelCallback(Form1_OnMouseWheel, wnd);

/*LPGUICONTROL c0, c1;
LPGUICOMPOSITEIMAGE c0c, c1c;
CHKRESULT(wnd->CreateControl(&c0));
CHKRESULT(wnd->CreateCompositeImage(&c0c));
c0c->AddFilledRectangle(s0, 0, 0, 100, 100);
c0->Location = Point<float>(50.0f, 50.0f);
c0->SetImage(c0c);
c0c->SetSize(Size<float>(100.0f, 100.0f));
CHKRESULT(wnd->CreateControl(&c1));
CHKRESULT(wnd->CreateCompositeImage(&c1c));
c1c->AddFilledRectangle(s1, 0, 0, 100, 100);
c1->Location = Point<float>(100.0f, 50.0f);
c1->SetImage(c1c);
c1c->SetSize(Size<float>(100.0f, 100.0f));

c0->SetMouseMoveCallback(c0_MouseMove, NULL);
c1->SetMouseMoveCallback(c1_MouseMove, NULL);

c1->BringToFront();*/

	/*LPGUICONTROL cp;
	LPGUICOMPOSITEIMAGE cpc;
	CHKRESULT(wnd->CreateControl(&cp));
	CHKRESULT(wnd->CreateCompositeImage(&cpc));

	Point<float> vertices[] = {Point<float>(100.0f, 10.0f),
							   Point<float>( 10.0f, 10.0f),
							   Point<float>( 55.0f, 80.0f),
							   Point<float>(100.0f, 10.0f),
							   Point<float>( 55.0f, 80.0f),
							   Point<float>(100.0f, 80.0f)};
	cpc->AddFilledGeometry(s0, vertices, 2);
	cp->SetImage(cpc);
	cp->Location = Point<float>(100.0f, 100.0f);
	cpc->SetSize(Size<float>(500.0f, 500.0f));
	cp->SetSize(Size<float>(500.0f, 500.0f));*/

	LPGUIBITMAP img;
	CHKRESULT(wnd->CreateImageFromFile("Pause.png", &img));

	LPGUITEXTFORMAT tfmt;
	CHKRESULT(gui->CreateTextFormat(String("Calibri"), 15.0f, false, &tfmt));

	float w1 = 300.0f, h1 = 16.0f;
	LPGUICONTROL ctrl1;
	LPGUIBRUSH lgbr1;
	LPGUICOMPOSITEIMAGE ctrl1_img;
	CHKRESULT(wnd->CreateLinearGradientBrush(stops, ARRAYSIZE(stops), Point<float>(0.0f, 0.0f), Point<float>(0.0f, h1), &lgbr1));
	CHKRESULT(wnd->CreateCompositeImage(&ctrl1_img));
	CHKRESULT(wnd->CreateControl(&ctrl1));
	ctrl1_img->AddFilledRectangle(lgbr1, 0.0f, 0.0f, w1, h1, Point<float>(5.0f, 5.0f)); //, DT_MIDDLELEFT
	ctrl1_img->AddDrawnRectangle(sbr, 0.0f, 0.0f, w1, h1, Point<float>(5.0f, 5.0f)); //, DT_MIDDLELEFT
	ctrl1_img->Size = Size<float>(w1, h1); ctrl1->Size = Size<float>(w1, h1);
	ctrl1->SetImage(ctrl1_img);
	ctrl1->Location = Point<float>(0, 0);//(150.0f, 300.0f);
	ctrl1->CreateTextField(tfmt, sbr, 0.0f, 0.0f, w1, h1);
	ctrl1->SetText("This is a test");

wnd->DrawLine(sbr, 500, 500, 800, 550);

	/*float w2 = 348.0f, h2 = 46.0f, bgr = 38.0f, imgxo = 44.0f;
	LPGUICONTROL ctrl2;
	LPGUIBRUSH lgbr2, lgbr3;
	LPGUISHAPE ctrl2_ellipse;
	LPGUICOMPOSITEIMAGE ctrl2_img;
	CHKRESULT(wnd->CreateCompositeImage(&ctrl2_img));
	CHKRESULT(wnd->CreateLinearGradientBrush(stops, ARRAYSIZE(stops), Point<float>(0.0f, 0.0f), Point<float>(0.0f, h2), &lgbr2));
	CHKRESULT(wnd->CreateLinearGradientBrush(stops, ARRAYSIZE(stops), Point<float>(0.0f, 0.0f), Point<float>(0.0f, 2.0f * bgr), &lgbr3));
	CHKRESULT(wnd->CreateControl(&ctrl2));
	//ctrl2_img->AddFilledRectangle(lgbr2, 0.0f, 0.0f, w2, h2, Point<float>(5.0f, 5.0f), DT_MIDDLELEFT, DT_MIDDLELEFT);
	ctrl2_img->AddDrawnRectangle(sbr, 0.0f, 0.0f, w2, h2, Point<float>(5.0f, 5.0f), DT_MIDDLELEFT, DT_MIDDLELEFT);
	//ctrl2_img->AddFilledEllipse(lgbr3, imgxo, 0.0f, bgr, bgr, DT_MIDDLELEFT, DT_MIDDLECENTER);
	ctrl2_img->AddDrawnEllipse(sbr, imgxo, 0.0f, bgr, bgr, DT_MIDDLELEFT, DT_MIDDLECENTER, &ctrl2_ellipse);
	ctrl2_img->AddBitmap(img, imgxo, 0.0f, img->Width, img->Height, DT_MIDDLELEFT, DT_MIDDLECENTER);
	ctrl2->Location = Point<float>(150.0f, 500.0f);
	ctrl2_img->Size = Size<float>(w2, 2.0f * bgr); ctrl2->Size = Size<float>(w2, 2.0f * bgr);
	ctrl2->SetImage(ctrl2_img);
	ctrl2->SetMouseDownCallback(cmdPauseNew_OnClick, NULL);

	LPGUIBITMAP imgvolumeslider, imgvolumebg;
	LPGUICONTROL volumeslider, volumebg;

	CHKRESULT(wnd->CreateControl(DT_TOPLEFT, DT_MIDDLELEFT, &volumebg));
	CHKRESULT(wnd->CreateImageFromFile("K:\\C++\\MediaPlayerX\\Output\\VolumeBG.png", &imgvolumebg));
	volumebg->SetImage(imgvolumebg);
	volumebg->Location = Point<float>(550.0f, 490.0f);
	volumebg->Size = Size<float>(imgvolumebg->Width, imgvolumebg->Height);

	CHKRESULT(wnd->CreateControl(DT_TOPLEFT, DT_MIDDLECENTER, &volumeslider));
	CHKRESULT(wnd->CreateImageFromFile("K:\\C++\\MediaPlayerX\\Output\\VolumeSlider.png", &imgvolumeslider));
//volumeslider->AddDrawnRectangle(sbr, 0.0f, 0.0f, imgvolumeslider->Width, imgvolumeslider->Height, Point<float>(0.0f, 0.0f), DT_MIDDLECENTER, DT_MIDDLECENTER);
	volumeslider->SetImage(imgvolumeslider);
	volumeslider->Location = Point<float>(570.0f, 490.0f);
	volumeslider->Size = Size<float>(imgvolumeslider->Width, imgvolumeslider->Height);

	LPSLIDERCONTROL slider;
	CHKRESULT(gui->CreateSlider(volumebg, volumeslider, OT_HORIZONTAL, volumebg->Left + 12.0f, volumebg->Left + volumebg->Width - 12.0f, &slider));
	slider->EnableSnapBack = false;
	slider->SetValueChangedCallback(sldVolume_OnValueChanged, NULL);*/

	/*// Create media scrollbar control
	LPGUICONTROL mediascrollslider, mediascrollbg;

	CHKRESULT(gui->CreateControl(DT_BOTTOMCENTER, DT_MIDDLECENTER, &mediascrollbg));
	mediascrollbg->AddDrawnRectangle(sbr, 0.0f, 0.0f, 500.0f, 10.0f, Point<float>(0.0f, 0.0f), DT_MIDDLECENTER, DT_MIDDLECENTER);
	mediascrollbg->Location = Point<float>(0.0f, 50.0f);
	mediascrollbg->Size = Size<float>(500.0f, 10.0f);

	CHKRESULT(gui->CreateControl(DT_BOTTOMCENTER, DT_MIDDLECENTER, &mediascrollslider));
	mediascrollslider->AddImage(imgvolumeslider, 0.0f, 0.0f, imgvolumeslider->Width, imgvolumeslider->Height, DT_MIDDLECENTER, DT_MIDDLECENTER);
	mediascrollslider->Size = Size<float>(imgvolumeslider->Width, imgvolumeslider->Height);
	mediascrollslider->Top = 50.0f;

	LPSLIDERCONTROL slider2;
	CHKRESULT(gui->CreateSlider(mediascrollbg, mediascrollslider, OT_HORIZONTAL, -250.0f + 12.0f, 250.0f - 12.0f, &slider2));
	slider2->EnableSnapBack = false;
	slider2->Minimum = 0.0f;
	slider2->Maximum = 1.0f;

	/*LPGUIANIMATION ani;
	CHKRESULT(gui->CreateAnimation(&ani));
	CHKRESULT(ani->AddMove(volumebg, 100.0f, 100.0f, 50.0f));
	CHKRESULT(ani->Schedule());*/



	/*LPGUIBRUSH sbr1, sbr2;
	CHKRESULT(gui->CreateSolidBrush(0.0f, 0.0f, 1.0f, 1.0f, &sbr1));
	CHKRESULT(gui->CreateSolidBrush(1.0f, 0.0f, 0.0f, 1.0f, &sbr2));

	LPGUICOMPOSITEIMAGE ctrl_img;
	CHKRESULT(gui->CreateCompositeImage(&ctrl_img));
	ctrl_img->Size = Size<float>(100.0f, 100.0f);
	ctrl_img->AddFilledRectangle(sbr1, 0.0f, 0.0f, 100.0f, 100.0f, Point<float>(5.0f, 5.0f), DT_TOPLEFT, DT_TOPLEFT);

	LPGUICOMPOSITEIMAGE overimg;
	CHKRESULT(gui->CreateCompositeImage(&overimg));
	overimg->Size = Size<float>(100.0f, 100.0f);
	overimg->AddFilledRectangle(sbr2, 0.0f, 0.0f, 100.0f, 100.0f, Point<float>(5.0f, 5.0f), DT_TOPLEFT, DT_TOPLEFT);

	LPGUICONTROL ctrl;
	CHKRESULT(gui->CreateControl(&ctrl));
	ctrl->SetImage(ctrl_img);
	ctrl->Location = Point<float>(350.0f, 250.0f);
	ctrl->SetMouseOverEffect(overimg, NULL, TransitionDescription(TransitionScript::T_Instantaneous, TransitionScript::Instantaneous(0.0)));*/
#endif

#ifdef XA2_DEVELOP
	// Init XAudio2
	CHKALLOC(xa2 = CreateXAudio2()); xa2->Sync(GLOBALVAR_LIST);

#ifndef PSE_DEVELOP
	CHKRESULT(xa2->Init());
	LPXA2SOUND snd;
	CreateSound("test.wav", &snd);
	CHKRESULT(snd->Play());
#endif
#endif

#ifdef D3D_DEVELOP
	// Init DirectInput
	CHKALLOC(dip = CreateDirectInput()); dip->Sync(GLOBALVAR_LIST);

//#ifndef PSE_DEVELOP
	CHKRESULT(dip->Init());
	CHKRESULT(dip->EnableMouse(true, false));
	CHKRESULT(dip->EnableKeyboard(true, false));
	dip->AddKeyHandler(Key::Space, OnSpace, NULL);
	dip->AddKeyHandler(Key::Return, OnEnter, NULL);
	dip->AddKeyHandler(Key::Add, OnPlus, NULL);
	dip->AddKeyHandler(Key::Subtract, OnMinus, NULL);

	//CHKRESULT(dip->ChangeKeyboardCooperativeLevel(false, false)); //EDIT
	dip->DisableKeyboard();
	CHKRESULT(dip->EnableKeyboard(false, false));
//#endif

	// Init Direct3D
	CHKALLOC(d3d = CreateDirect3D()); d3d->Sync(GLOBALVAR_LIST);
	D3dStartupSettings d3dsettings;
	d3dsettings.isfullscreen = false;
	CHKRESULT(d3d->Init(&d3dsettings));

	IOutputWindow::Settings wndsettings;
	wndsettings.backbuffersize = Size<UINT>(1920, 1200);
	wndsettings.wnd = form1->GetHwnd();
	wndsettings.enablemultisampling = true;
	wndsettings.screenidx = 0;
	CHKRESULT(d3d->CreateOutputWindow(&wndsettings, &d3dwnd));

	d3dwnd->MakeConsoleTarget(11, 11);
	d3dwnd->ShowFps();

	cle->PrintD3D(123);

	LPCAMERA cam;
CHKRESULT(d3d->CreateCamera(90.0f * PI / 180.0f, 0.001f, 200.0f, &cam));
//cam->pos = Vector3(0.0f, 0.0f, -2.0f);
cam->pos = Vector3(2.0f, 0.0f, 2.0f);
cam->rot.y = -1.25f * PI;
	cam->viewmode = ICamera::VM_FLY;
	d3dwnd->SetCamera(cam);
	
#ifndef PSE_DEVELOP
	/*IOutputWindow::Settings wndsettings;
	wndsettings.backbuffersize = Size<UINT>(1920, 1200);
	wndsettings.wnd = form1->GetHwnd();
	wndsettings.enablemultisampling = true;
	wndsettings.screenidx = 0;
	CHKRESULT(d3d->CreateOutputWindow(&wndsettings, &d3dwnd));

	d3dwnd->MakeConsoleTarget(11, 11);
	d3dwnd->ShowFps();

	cle->PrintD3D(123);

	LPCAMERA cam;
	//CHKRESULT(d3d->CreateCamera(0.0f, 0.001f, 200.0f, &cam));
CHKRESULT(d3d->CreateCamera(90.0f * PI / 180.0f, 0.001f, 200.0f, &cam));
	//cam->pos = Vector3(0.0f, 5.0f, -2.0f);
//cam->pos = Vector3(0.0f, 0.0f, -2.0f);
cam->pos = Vector3(2.0f, 0.0f, 2.0f);
cam->rot.y = -1.25f * PI;
	cam->viewmode = ICamera::VM_FLY;
	d3dwnd->SetCamera(cam);*/

	LPRENDERSHADER sdr;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("shader.fx"), &sdr));

d3dwnd->RegisterForRendering(Render, RT_DEFAULT);

	/*LPTEXTURE texrt;
	//CHKRESULT(d3dwnd->CreateTexture(256, 256, ITexture::TU_RENDERTARGET, false, DXGI_FORMAT_R8G8B8A8_UNORM, &texrt));
CHKRESULT(d3dwnd->CreateTexture("Pause.png", ITexture::TU_SKIN_NOMIPMAP, false, &texrt));

	LPHUD hud;
	CHKRESULT(d3dwnd->CreateHUD(&hud));
	hud->CreateElement(texrt, 0, 0, DT_TOPLEFT, 0xFFFFFFFF, Rect<int>(25, 25, 50, 50), 0);
	hud->CreateElement(texrt, 0, 0, DT_TOPLEFT, 0xFFFFFFFF, Rect<int>(25, 25, 50, 50), 1);
	d3dwnd->RegisterForRendering(hud, RT_FOREGROUND);*/

/*CHKRESULT(d3dwnd->CreateObject("Lamborghini.x", false, &obj));
obj->SetShader(sdr);
d3dwnd->RegisterForRendering(obj, RT_DEFAULT);

D3dSphereShapeDesc sdesc;
sdesc.radius = 0.1f;
sdesc.slices = sdesc.stacks = 8;
CHKRESULT(d3dwnd->CreateObject(sdesc, &obj2));
obj2->SetShader(sdr);
d3dwnd->RegisterForRendering(obj2, RT_DEFAULT);

cam->pos = Vector3(0.0f, 0.0f, 1.0f);
cam->rot.y = PI;
cam->viewmode = ICamera::VM_DEFAULT;*/

/*obj->pos = Vector3(0.0f, -0.35f, -2.0f);
obj->scl = Vector3(0.001f, 0.001f, 0.001f);
obj->rot.x = 20.0f * PI / 180.0f;
obj->rot.y = 60.0f * PI / 180.0f;*/

CHKRESULT(d3dwnd->CreateObject(FilePath("Lamborghini.x"), false, &obj));
//CHKRESULT(obj->ComputeNormals());
obj->SetShader(sdr);
obj->pos = Vector3(0.0f, 10.0f, 10.0f);
obj->scl = Vector3(0.001f, 0.001f, 0.001f);//Vector3(0.1f, 0.1f, 0.1f);
d3dwnd->RegisterForRendering(obj, RT_DEFAULT);

/*LPOBJECT track;
CHKRESULT(d3dwnd->CreateObject(FilePath("dubai.x"), false, &track));
track->SetShader(sdr);
//track->scl = Vector3(0.1f, 0.1f, 0.1f);
d3dwnd->RegisterForRendering(track, RT_DEFAULT);*/

/*D3dSphereShapeDesc sdesc;
sdesc.radius = 1.0f;
sdesc.slices = sdesc.stacks = 5;

D3dCylinderShapeDesc cdesc;
cdesc.v0 = Vector3(0.0f, 0.0f, 0.5f);
cdesc.v1 = Vector3(0.0f, 0.0f, 1.5f);
cdesc.radius = 0.5f;
cdesc.hasfrontfaces = true;
cdesc.slices = 5;

const D3dShapeDesc* shapedescs[] = {&sdesc, &cdesc};
CHKRESULT(d3dwnd->CreateObject(shapedescs, 2, &obj));
//CHKRESULT(d3dwnd->CreateObject(cdesc, &obj));
d3dwnd->RegisterForRendering(obj, RT_DEFAULT);
obj->SetShader(sdr);*/

/*LPTEXTURE texboxes;
CHKRESULT(d3dwnd->CreateTexture("MincraftDefaultTexture.png", ITexture::TU_SKIN, false, &texboxes));

IBoxedLevel::BoxType* boxtypes = new IBoxedLevel::BoxType[2];
boxtypes[0].SetTextureIndices(3);
boxtypes[0].textureindex[4] = 146;
boxtypes[0].shape = IBoxedLevel::SHAPE_CUBIC;

boxtypes[1].SetTextureIndices(3);
boxtypes[1].textureindex[4] = 146;
boxtypes[1].shape = IBoxedLevel::SHAPE_STAIRS;

std::vector<IBoxedLevel::BoxDesc> boxes;
for(int z = -5; z <= 5; z++)
	for(int x = -5; x <= 5; x++)
		boxes.push_back(IBoxedLevel::BoxDesc(x, 0, z, 0));
for(int x = 0; x <= 11; x++)
	for(int y = 1; y <= x; y++)
		boxes.push_back(x == y ? IBoxedLevel::BoxDesc(x - 6, y, 5, 1, 1) : IBoxedLevel::BoxDesc(x - 6, y, 5, 0));

CHKRESULT(d3dwnd->CreateBoxedLevel(texboxes, FilePath("K:\\Resources\\Shader\\BLevel.fx"), &boxes[0], boxes.size(), &blevel));
blevel->SetBoxTypes(boxtypes, 2);
d3dwnd->RegisterForRendering(blevel, RT_DEFAULT);*/






/*LPRENDERSHADER brokenshader;
CHKRESULT(d3dwnd->CreateRenderShader(FilePath("K:\\Resources\\Shader\\broken_cube.fx"), &brokenshader));
const BYTE subtextureindices[] = {3, 3, 3, 3, 146, 3, 0, 0};
brokenshader->GetVariableBySemantic("SubtextureIndices")->SetRawValue((LPVOID)subtextureindices, 0, 8);

LPTEXTURE texboxes;
CHKRESULT(d3dwnd->CreateTexture(FilePath("K:\\Resources\\Images\\MincraftDefaultTexture.png"), ITexture::TU_SKIN, false, &texboxes));*/

/*D3dBoxShapeDesc sdesc;
sdesc.pos = Vector3(0.0f, 0.0f, 0.0f);
sdesc.size = Vector3(1.0f, 1.0f, 1.0f);
CHKRESULT(d3dwnd->CreateObject(sdesc, &obj2));
obj2->mats[0].tex = texboxes;
obj2->SetShader(brokenshader);
d3dwnd->RegisterForRendering(obj2, RT_DEFAULT);*/



//#define DEBUG_OBJECT_BREAK
#ifdef DEBUG_OBJECT_BREAK
/*D3dSphereShapeDesc sdesc;
sdesc.radius = 1.0f;
sdesc.slices = sdesc.stacks = 10;*/
//D3dCylinderShapeDesc sdesc;
//sdesc.v0 = Vector3(1.0f, 0.0f, 0.0f);
//sdesc.v1 = Vector3(-1.0f, 0.0f, 0.0f);
//sdesc.radius = 1.0f;
//sdesc.slices = 4;
//sdesc.hasfrontfaces = false;
D3dBoxShapeDesc sdesc;
sdesc.pos = Vector3(0.0f, 0.0f, 0.0f);
sdesc.size = Vector3(1.0f, 1.0f, 1.0f);
CHKRESULT(d3dwnd->CreateObject(sdesc, &obj2));
obj2->mats[0].tex = texboxes;
//obj2->SetShader(sdr);
//d3dwnd->RegisterForRendering(obj2, RT_DEFAULT);
//obj2->pos = Vector3(0.0f, 0.0f, 0.0f);

/*LPOBJECT first, second;
D3DXPLANE splitplane(1.0f, 0.0f, 0.0f, 0.0f);
D3DXPlaneNormalize(&splitplane, &splitplane);
CHKRESULT(obj2->Split(&splitplane, &first, &second));
if(first)
{
	first->SetShader(sdr);
	d3dwnd->RegisterForRendering(first, RT_DEFAULT);
	first->pos = Vector3(-0.3f, 0.0f, 0.0f);
}
if(second)
{
	second->SetShader(sdr);
	d3dwnd->RegisterForRendering(second, RT_DEFAULT);
	second->pos = Vector3(0.3f, 0.0f, 0.0f);
}*/

LPOBJECT splitparts[64] = {0};
int numparts;
CHKRESULT(obj2->Break(2/*16*/, splitparts, &numparts));
for(int i = 0; i < 64; i++)
	if(splitparts[i])
	{
		//splitparts[i]->SetShader(sdr);
		splitparts[i]->SetShader(brokenshader);
		d3dwnd->RegisterForRendering(splitparts[i], RT_DEFAULT);
	}
#endif




//
//float t;
//
///*StartMemLog();
//LPOBJECT o = NULL;
//CHKRESULT(d3dwnd->CreateObject(FilePath("cube.x"), true, &o));
//d3dwnd->RegisterForRendering(o, RT_DEFAULT);
//o->SetShader(sdr);
//d3dwnd->RemoveObject(o);
//EndMemLog();
//return ERR("");*/
//
//eng->time.Step();
//t = eng->time.t;

/*	LPRENDERSHADER sdrBlend;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("shader withblending.fx"), &sdrBlend));

	CHKRESULT(d3dwnd->CreateObject(FilePath("human_withbones.x"), true, &obj));
//eng->time.Step();
//cle->PrintLine(String("Player: ") << String((eng->time.t - t) * 1000.0f) << String("ms"));
	obj->pos = Vector3(0.0f, 0.0f, 0.0f);
	//obj->scl = Vector3(0.01f, 0.01f, 0.01f);
	//obj->scl = Vector3(0.02f, 0.02f, 0.02f);
	d3dwnd->RegisterForRendering(obj, RT_DEFAULT);
	obj->SetShader(sdrBlend);*/
//
//	if(obj)
//		{CHKRESULT(d3dwnd->CreateObject(obj, true, &obj2));}
//	else
//		{CHKRESULT(d3dwnd->CreateObject(FilePath("human_withbones.x"), true, &obj2));}
//	obj2->pos = Vector3(0.0f, 4.3f, -3.0f);
//	d3dwnd->RegisterForRendering(obj2, RT_DEFAULT);
//	obj2->SetShader(sdr);
//
//	LPOBJECT objLevel = NULL;
//eng->time.Step();
//t = eng->time.t;
//	CHKRESULT(d3dwnd->CreateObject(FilePath("Level\\Level.x"), true, &objLevel));
//eng->time.Step();
//cle->PrintLine(String("Level: ") << String((eng->time.t - t) * 1000.0f) << String("ms"));
////objLevel->ComputeNormals();
//	d3dwnd->RegisterForRendering(objLevel, RT_DEFAULT);
//	objLevel->SetShader(sdr);
//
//	D3dSphereShapeDesc sdesc;
//	sdesc.radius = 0.3f;
//	sdesc.stacks = 10;
//	sdesc.slices = 10;
//	CHKRESULT(d3dwnd->CreateObject(sdesc, &sphere));
//	d3dwnd->RegisterForRendering(sphere, RT_DEFAULT);
//	sphere->SetShader(sdr);
//
//	/*LPSKYBOX skybox;
//	CHKRESULT(d3dwnd->CreateSkyBox(&skybox));
//	CHKRESULT(skybox->CreateLayer(FilePath("SkyBox\\North_Sky.bmp"), FilePath("SkyBox\\East_Sky.bmp"), FilePath("SkyBox\\South_Sky.bmp"),
//								  FilePath("SkyBox\\West_Sky.bmp"), FilePath("SkyBox\\Top.bmp"), FilePath("SkyBox\\Bottom.bmp"), 0));
//	CHKRESULT(skybox->CreateLayer(FilePath("SkyBox\\North_Scene.dds"), FilePath("SkyBox\\East_Scene.dds"), FilePath("SkyBox\\South_Scene.dds"),
//								  FilePath("SkyBox\\West_Scene.dds"), FilePath(""), FilePath(""), 8));
//	d3dwnd->RegisterForRendering(skybox, RT_BACKGROUND);
//	d3dwnd->SetBackColor(false, (DWORD)0); // Disable screen clearing*/
//
/*	LPLANDSCAPE landscape;
	CHKRESULT(d3dwnd->CreateLandscape(FilePath("heightmap_32x32.png"), sdr, &landscape));
	d3dwnd->RegisterForRendering(landscape, RT_DEFAULT);*/
//
//	/*LPWINDOWLESSDEVICE wldev;
//	CHKRESULT(d3d->CreateWindowlessDevice(&wldev));
//
//	LPTEXTURE texsrc;
//	CHKRESULT(wldev->CreateTexture(FilePath("Cover\\Preview2.jpg"), ITexture::TU_SOURCE, false, &texsrc));
//	wldev->AddSourceTexture(texsrc, String("SourceImage"), String("SourceImageSize"));
//	LPTEXTURE texsrc2;
//	CHKRESULT(wldev->CreateTexture(FilePath("Cover\\Glass.png"), ITexture::TU_SOURCE, false, &texsrc2));
//	wldev->AddSourceTexture(texsrc2, String("OverlayImage"), String("OverlayImageSize"));
//
//	LPTEXTURE texdest;
//	CHKRESULT(wldev->CreateTexture(200, 200, ITexture::TU_RENDERTARGET, false, DXGI_FORMAT_R8G8B8A8_UNORM, &texdest));
//	wldev->SetDestinationTexture(texdest, String("DestinationImageSize"));
//
//	LPRENDERSHADER shd;
//	CHKRESULT(wldev->CreateRenderShader(FilePath("Cover\\ThumbGen.fx"), &shd));
//	CHKRESULT(wldev->SetShader(shd));
//
//	wldev->Render(1.0f, 0.0f, 0.0f);
//	CHKRESULT(texdest->Save(FilePath("Cover\\out2.png"), D3DX10_IFF_PNG));
//	PostQuitMessage(0);*/
//
///*LPOBJECT objConvex;
//CHKRESULT(d3dwnd->CreateObject(FilePath("K:\\Resources\\Objects\\out.x"), true, &objConvex));
//objConvex->pos = Vector3(2.0f, 2.0f, 2.0f);
////objConvex->scl = Vector3(0.001f, 0.001f, 0.001f);
//d3dwnd->RegisterForRendering(objConvex, RT_DEFAULT);
//objConvex->SetShader(sdr);*/
#endif
#endif

	/*LPRENDERSHADER sdrBlend;
	CHKRESULT(d3dwnd->CreateRenderShader(FilePath("shader withblending.fx"), &sdrBlend));

	CHKRESULT(d3dwnd->CreateObject(FilePath("human_withbones.x"), true, &obj));
	obj->pos = Vector3(0.0f, 0.0f, 0.0f);
	d3dwnd->RegisterForRendering(obj, RT_DEFAULT);
	obj->SetShader(sdrBlend);*/

#ifdef HVK_DEVELOP
	// Init Havok
	CHKALLOC(hvk = CreateHavok()); hvk->Sync(GLOBALVAR_LIST);
	//LPRENDERSHADER sdrPhysics;
	//CHKRESULT(d3dwnd->CreateRenderShader(FilePath("shader physics.fx"), &sdrPhysics));
	//CHKRESULT(hvk->Init(VT_NONE, NULL, NULL));
	CHKRESULT(hvk->Init(Vector3(-1000.0f, -1000.0f, -1000.0f), Vector3(1000.0f, 1000.0f, 1000.0f), VT_VISUALDEBUGGER, NULL, NULL));
	//CHKRESULT(hvk->Init(VT_LOCAL, d3dwnd, sdrPhysics));
	//hvk->EnableViewer(HV_SHAPEDISPLAY);
	//hvk->EnableViewer(HV_BROADPHASE);
	//Ahvk->EnableViewer(HV_ACTIVECONTACTPOINT);
	//hvk->EnableViewer(HV_CONSTRAINT);
	//hvk->EnableViewer(HV_PHANTOMDISPLAY);
	//hvk->EnableViewer(HV_VEHICLE);

	LPREGULARENTITY entityFloor;
	HvkPlaneShapeDesc planedesc;
	planedesc.mtype = MT_FIXED;
	planedesc.vcenter = Vector3(0.0f, -1.5f, 0.0f);
	planedesc.vdir = Vector3(0.0f, 1.0f, 0.0f);
	planedesc.vhalfext = Vector3(200, 200.0f, 200.0f);
	CHKRESULT(hvk->CreateRegularEntity(&planedesc, IHavok::LAYER_LANDSCAPE, &Vector3(0.0f, 0.0f, 0.0f), &Quaternion(0.0f, 0.0f, 0.0f, 1.0f), &entityFloor)); //Vector3(0.0f, 3.0f, 0.0f)

#ifndef PSE_DEVELOP

/*if(blevel)
{
	hkpBoxShapeDesc bdesc;
	bdesc.size = Vector3(1.0f, 1.0f, 1.0f);
	bdesc.mtype = MT_FIXED;
	LPREGULARENTITY entityBox;
	for(int z = -5; z <= 5; z++)
		for(int x = -5; x <= 5; x++)
			CHKRESULT(hvk->CreateRegularEntity(&bdesc, IHavok::LAYER_LANDSCAPE, &Vector3((float)x, 0.0f, (float)z), &Quaternion(0.0f, 0.0f, 0.0f, 1.0f), &entityBox));
}*/

	/*LPREGULARENTITY entityAttachment;
	HvkCapsuleShapeDesc qdesc;
	qdesc.mtype = MT_KEYFRAMED;
	qdesc.v0 = Vector3(0.0f, 0.0f, 0.0f);
	qdesc.v1 = Vector3(0.0f, 0.1f, 0.0f);
	qdesc.radius = 0.05f;
	CHKRESULT(hvk->CreateRegularEntity(&qdesc, IHavok::LAYER_HANDHELD, Vector3(0.0f, 2.0f, 2.0f), Quaternion(0.0f, 0.0f, 0.0f, 1.0f), &entityAttachment));*/

	/*LPREGULARENTITY entityLevel;
	if(objLevel)
		CHKRESULT(objLevel->CreateCollisionMesh((LPVOID*)&entityLevel));*/

IObject::Vertex* vertices;

CHKRESULT(obj->MapVertexData(&vertices));

HvkConvexVerticesShapeDesc desc;
desc.mtype = MT_DYNAMIC;
desc.mass = 5.0f;
desc.numvertices = obj->GetVertexCount();

obj->orientmode = OM_QUAT;
float* scaledvertices;
CHKALLOC(scaledvertices = new float[desc.numvertices * 3]);
for(int i = 0; i < desc.numvertices; i++)
{
	scaledvertices[3 * i + 0] = vertices[i].pos.x * obj->scl.x;//((float*)&vertices[36 * i])[0] * obj->scl.x; //EDIT: Remove hard-coded stride
	scaledvertices[3 * i + 1] = vertices[i].pos.y * obj->scl.y;//((float*)&vertices[36 * i])[1] * obj->scl.y;
	scaledvertices[3 * i + 2] = vertices[i].pos.z * obj->scl.z;//((float*)&vertices[36 * i])[2] * obj->scl.z;
}
desc.vertices = (BYTE*)scaledvertices;
desc.fvfstride = 3 * sizeof(float);
VehicleDesc vdesc;
vdesc.chassis = &desc;
vdesc.wheels.resize(4);
vdesc.wheels[0].SetDefault(0);
vdesc.wheels[1].SetDefault(0);
vdesc.wheels[2].SetDefault(1);
vdesc.wheels[3].SetDefault(1);
vdesc.engine.SetDefault();
CHKRESULT(hvk->CreateVehicle(&vdesc, &obj->pos, &obj->qrot, &vehicle));
delete[] scaledvertices;
CHKRESULT(obj->UnmapData(vertices));

	/*hkpBoxShapeDesc bdesc;
	bdesc.size = Vector3(1.0f, 1.0f, 1.0f);
	bdesc.mtype = MT_DYNAMIC;
	LPVEHICLE vehicle;
	CHKRESULT(hvk->CreateVehicle(&bdesc, &Vector3(0.0f, 10.0f, 4.0f), &Quaternion(0.0f, 0.0f, 0.0f, 1.0f), &vehicle));*/

//
//	/*LPREGULARENTITY entity2;
//	if(obj2)
//		CHKRESULT(obj2->CreateCollisionCapsule(0.5f, &entity2));*/
//
//	if(obj2)
//	{
//		CHKRESULT(obj2->CreateCollisionRagdoll(0.3f, (LPVOID*)&entity2_ragdoll));
//		/*D3DXMATRIX m;
//		D3DXMatrixTranslation(&m, 0.7f, 0.31f, 0.0f);
//		entity2_ragdoll->AttachEntity(entityAttachment, "LowerRightArm", m);*/
////entity2_ragdoll->jump = true;
//
//CHKRESULT(entity2_ragdoll->CreatePose(&entity2_pose));
///*Quaternion rot;
//D3DXQuaternionRotationAxis(&rot, &Vector3(0.0f, 0.0f, 1.0f), -PI / 4.0f);
//entity2_pose->SetRotation(rot, 5);
//D3DXQuaternionRotationAxis(&rot, &Vector3(0.0f, 1.0f, 0.0f), PI / 2.0f);
//entity2_pose->SetRotation(rot, 2);*/
////entity2_ragdoll->ApplyPose(entity2_pose);
//LPANIMATION ani;
//entity2_ragdoll->CreateAnimation(entity2_pose, 1.0f, &ani);
//CHKRESULT(ani->Serialize("out.xml"));
//	}
//
///*LPREGULARENTITY entityConvex;
//if(objConvex)
//	objConvex->CreateConvexCollisionHull((LPVOID*)&entityConvex);*/
//
	//CHKRESULT(hvk->CreateWeapon(&weapon));
#endif // !PSE_DEVELOP

//	if(obj)
//	{
//		LPRAGDOLLENTITY entity1_ragdoll;
//		CHKRESULT(obj->CreateCollisionRagdoll(0.3f, true, &entity1_ragdoll));
//
//		//cam->SetPlayerModel(obj, entity1_ragdoll);
//		//cam->viewmode = ICamera::VM_THIRDPERSON;
//
///*Quaternion rotX, rotY, rotZ, rot;
//#define SET_BONE_ROTATION(pose, boneidx, rotx, roty, rotz) \
//{ \
//	D3DXQuaternionRotationYawPitchRoll(&rotX, (rotx) * PI / 180.0f, 0.0f, 0.0f); \
//	D3DXQuaternionRotationYawPitchRoll(&rotY, 0.0f, (roty) * PI / 180.0f, 00.0f); \
//	D3DXQuaternionRotationYawPitchRoll(&rotZ, 0.0f, 0.0f, (rotz) * PI / 180.0f); \
//	D3DXQuaternionMultiply(&rot, D3DXQuaternionMultiply(&rot, &rotY, &rotX), &rotZ); \
//	(pose)->SetRotation(&rot, (boneidx)); \
//}
////#define SET_BONE_ROTATION(pose, boneidx, yaw, pitch, roll) {D3DXQuaternionRotationYawPitchRoll(&rot, (yaw), (pitch), (roll)); (pose)->SetRotation(&rot, (boneidx));}
//#define TORSOR_POS(time) (0.0148147555844855f * sin(1.3996930828088f * (time) - 0.92081890941623f) - 0.0149605824201595f)
//
//LPPOSE frames[9] = {0};
//for(int i = 0; i < 9; i++)
//	CHKRESULT(entity1_ragdoll->CreatePose(&frames[i]));
//
//// Frame 0
//SET_BONE_ROTATION(frames[0], 5, 0.0f, -30.0f, 0.0f); // Upper left leg
//SET_BONE_ROTATION(frames[0], 6, 0.0f, 25.0f, 0.0f); // Lower left leg
//SET_BONE_ROTATION(frames[0], 7, 0.0f, 15.0f, 0.0f); // Upper right leg
//SET_BONE_ROTATION(frames[0], 8, 0.0f, 20.0f, 0.0f); // Lower right leg
//
//SET_BONE_ROTATION(frames[0], 1, -15.0f, 0.0f, 70.0f); // Upper left arm
//SET_BONE_ROTATION(frames[0], 2, 0.0f, 0.0f, 20.0f); // Lower left arm
//SET_BONE_ROTATION(frames[0], 3, -30.0f, 0.0f, -70.0f); // Upper right arm
//SET_BONE_ROTATION(frames[0], 4, 0.0f, 0.0f, -20.0f); // Lower right arm
//
//// Frame 1
//SET_BONE_ROTATION(frames[1], 5, 0.0f, -15.0f, 0.0f); // Upper left leg
//SET_BONE_ROTATION(frames[1], 6, 0.0f, 15.0f, 0.0f); // Lower left leg
//SET_BONE_ROTATION(frames[1], 7, 0.0f, 0.0f, 0.0f); // Upper right leg
//SET_BONE_ROTATION(frames[1], 8, 0.0f, 40.0f, 0.0f); // Lower right leg
//
//SET_BONE_ROTATION(frames[1], 1, 0.0f, 0.0f, 70.0f); // Upper left arm
//SET_BONE_ROTATION(frames[1], 2, 0.0f, 0.0f, 20.0f); // Lower left arm
//SET_BONE_ROTATION(frames[1], 3, -15.0f, 0.0f, -70.0f); // Upper right arm
//SET_BONE_ROTATION(frames[1], 4, 0.0f, 0.0f, -20.0f); // Lower right arm
//
//// Frame 2
//SET_BONE_ROTATION(frames[2], 5, 0.0f, 0.0f, 0.0f); // Upper left leg
//SET_BONE_ROTATION(frames[2], 6, 0.0f, 0.0f, 0.0f); // Lower left leg
//SET_BONE_ROTATION(frames[2], 7, 0.0f, -15.0f, 0.0f); // Upper right leg
//SET_BONE_ROTATION(frames[2], 8, 0.0f, 65.0f, 0.0f); // Lower right leg
//
//SET_BONE_ROTATION(frames[2], 1, 15.0f, 0.0f, 70.0f); // Upper left arm
//SET_BONE_ROTATION(frames[2], 2, 0.0f, 0.0f, 20.0f); // Lower left arm
//SET_BONE_ROTATION(frames[2], 3, 0.0f, 0.0f, -70.0f); // Upper right arm
//SET_BONE_ROTATION(frames[2], 4, 0.0f, 0.0f, -20.0f); // Lower right arm
//
//// Frame 3
//SET_BONE_ROTATION(frames[3], 5, 0.0f, 15.0f, 0.0f); // Upper left leg
//SET_BONE_ROTATION(frames[3], 6, 0.0f, 5.0f, 0.0f); // Lower left leg
//SET_BONE_ROTATION(frames[3], 7, 0.0f, -35.0f, 0.0f); // Upper right leg
//SET_BONE_ROTATION(frames[3], 8, 0.0f, 40.0f, 0.0f); // Lower right leg
//
//SET_BONE_ROTATION(frames[3], 1, 35.0f, 0.0f, 70.0f); // Upper left arm
//SET_BONE_ROTATION(frames[3], 2, 0.0f, 0.0f, 20.0f); // Lower left arm
//SET_BONE_ROTATION(frames[3], 3, 15.0f, 0.0f, -70.0f); // Upper right arm
//SET_BONE_ROTATION(frames[3], 4, 0.0f, 0.0f, -20.0f); // Lower right arm
//
//// Frame 4
//SET_BONE_ROTATION(frames[4], 5, 0.0f, 25.0f, 0.0f); // Upper left leg
//SET_BONE_ROTATION(frames[4], 6, 0.0f, 0.0f, 0.0f); // Lower left leg
//SET_BONE_ROTATION(frames[4], 7, 0.0f, -30.0f, 0.0f); // Upper right leg
//SET_BONE_ROTATION(frames[4], 8, 0.0f, 25.0f, 0.0f); // Lower right leg
//
//SET_BONE_ROTATION(frames[4], 1, 30.0f, 0.0f, 70.0f); // Upper left arm
//SET_BONE_ROTATION(frames[4], 2, 0.0f, 0.0f, 20.0f); // Lower left arm
//SET_BONE_ROTATION(frames[4], 3, 25.0f, 0.0f, -70.0f); // Upper right arm
//SET_BONE_ROTATION(frames[4], 4, 0.0f, 0.0f, -20.0f); // Lower right arm
//
//// Frame 5
//SET_BONE_ROTATION(frames[5], 5, 0.0f, 15.0f, 0.0f); // Upper left leg
//SET_BONE_ROTATION(frames[5], 6, 0.0f, 20.0f, 0.0f); // Lower left leg
//SET_BONE_ROTATION(frames[5], 7, 0.0f, -15.0f, 0.0f); // Upper right leg
//SET_BONE_ROTATION(frames[5], 8, 0.0f, 15.0f, 0.0f); // Lower right leg
//
//SET_BONE_ROTATION(frames[5], 1, 15.0f, 0.0f, 70.0f); // Upper left arm
//SET_BONE_ROTATION(frames[5], 2, 0.0f, 0.0f, 20.0f); // Lower left arm
//SET_BONE_ROTATION(frames[5], 3, 15.0f, 0.0f, -70.0f); // Upper right arm
//SET_BONE_ROTATION(frames[5], 4, 0.0f, 0.0f, -20.0f); // Lower right arm
//
//// Frame 6
//SET_BONE_ROTATION(frames[6], 5, 0.0f, 0.0f, 0.0f); // Upper left leg
//SET_BONE_ROTATION(frames[6], 6, 0.0f, 40.0f, 0.0f); // Lower left leg
//SET_BONE_ROTATION(frames[6], 7, 0.0f, 0.0f, 0.0f); // Upper right leg
//SET_BONE_ROTATION(frames[6], 8, 0.0f, 0.0f, 0.0f); // Lower right leg
//
//SET_BONE_ROTATION(frames[6], 1, 0.0f, 0.0f, 70.0f); // Upper left arm
//SET_BONE_ROTATION(frames[6], 2, 0.0f, 0.0f, 20.0f); // Lower left arm
//SET_BONE_ROTATION(frames[6], 3, 0.0f, 0.0f, -70.0f); // Upper right arm
//SET_BONE_ROTATION(frames[6], 4, 0.0f, 0.0f, -20.0f); // Lower right arm
//
//// Frame 7
//SET_BONE_ROTATION(frames[7], 5, 0.0f, -15.0f, 0.0f); // Upper left leg
//SET_BONE_ROTATION(frames[7], 6, 0.0f, 65.0f, 0.0f); // Lower left leg
//SET_BONE_ROTATION(frames[7], 7, 0.0f, 15.0f, 0.0f); // Upper right leg
//SET_BONE_ROTATION(frames[7], 8, 0.0f, 5.0f, 0.0f); // Lower right leg
//
//SET_BONE_ROTATION(frames[7], 1, -15.0f, 0.0f, 70.0f); // Upper left arm
//SET_BONE_ROTATION(frames[7], 2, 0.0f, 0.0f, 20.0f); // Lower left arm
//SET_BONE_ROTATION(frames[7], 3, -15.0f, 0.0f, -70.0f); // Upper right arm
//SET_BONE_ROTATION(frames[7], 4, 0.0f, 0.0f, -20.0f); // Lower right arm
//
//// Frame 8
//SET_BONE_ROTATION(frames[8], 5, 0.0f, -35.0f, 0.0f); // Upper left leg
//SET_BONE_ROTATION(frames[8], 6, 0.0f, 40.0f, 0.0f); // Lower left leg
//SET_BONE_ROTATION(frames[8], 7, 0.0f, 25.0f, 0.0f); // Upper right leg
//SET_BONE_ROTATION(frames[8], 8, 0.0f, 0.0f, 0.0f); // Lower right leg
//
//SET_BONE_ROTATION(frames[8], 1, -35.0f, 0.0f, 70.0f); // Upper left arm
//SET_BONE_ROTATION(frames[8], 2, 0.0f, 0.0f, 20.0f); // Lower left arm
//SET_BONE_ROTATION(frames[8], 3, -25.0f, 0.0f, -70.0f); // Upper right arm
//SET_BONE_ROTATION(frames[8], 4, 0.0f, 0.0f, -20.0f); // Lower right arm
//
//// Torsor movement
//for(int i = 0; i < 9; i++)
//	frames[i]->SetPosition(&Vector3(0.0f, max(TORSOR_POS((float)i / 9.0f), TORSOR_POS((float)((i + 4) % 9) / 9.0f)), 0.0f), 0);
//frames[0]->SetPosition(&Vector3(0.0f, TORSOR_POS(0.0f), 0.0f), 0);
//
//CHKRESULT(entity1_ragdoll->CreateAnimation(2.0f, &ani));
//for(int i = 0; i < 9; i++)
//	ani->AddFrame(frames[i]);
//ani->AddFrame(frames[0]);
//ani->Enabled = true;*/
//
//
//
///*LPPOSE frames2[3] = {0};
//for(int i = 0; i < ARRAYSIZE(frames2); i++)
//	CHKRESULT(entity1_ragdoll->CreatePose(&frames2[i]));*/
//
///*LPPOSE poseHoldWeapon = NULL;
//CHKRESULT(entity1_ragdoll->CreatePose(&poseHoldWeapon));
//SET_BONE_ROTATION(poseHoldWeapon, 1, 1.66f, -0.103f, 0.499f);
//SET_BONE_ROTATION(poseHoldWeapon, 2, 0.205f, 6.69e-002f, -0.356f);
//SET_BONE_ROTATION(poseHoldWeapon, 3, -1.67f, 0.262f, -0.936f);
//SET_BONE_ROTATION(poseHoldWeapon, 4, 0.109f, 0.535f, 1.41f);
//
//LPPOSE pose2 = NULL;
//CHKRESULT(entity1_ragdoll->CreatePose(&pose2));
//SET_BONE_ROTATION(pose2, 5, 0.0f, 0.0f, 1.0f);
//SET_BONE_ROTATION(pose2, 2, 0.0f, 1.0f, 0.0f);
//
//CHKRESULT(entity1_ragdoll->CreateAnimation(pose2, 1.0f, &ani));
//ani->AddFrame(poseHoldWeapon);
//ani->AddFrame(pose2);*/
//
///*CHKRESULT(entity1_ragdoll->CreateAnimation(1.0f, &ani2));
//ani2->AddFrame(frames2[0]);
//ani2->AddFrame(frames2[1]);
//ani2->Enabled = true;*/
//
//		LPPYTHONSCRIPT script;
//		CHKRESULT(pse->LoadScript("human_animations.py", &script));
//		int numfunctions;
//		PyObject** scriptfunctions;
//		String* scriptfuncnames;
//		script->GetFunctions(&numfunctions, &scriptfunctions, &scriptfuncnames);
//		for(int i = 0; i < numfunctions; i++)
//			if(scriptfuncnames[i].Equals("CreateAnimation"))
//			{
//				std::shared_ptr<void> pyhandle = entity1_ragdoll->GetPyPtr();
//				PyGet(pse->Call(scriptfunctions[i], "Os", pyhandle.get(), "Walking"), &ani2);
//				PyGet(pse->Call(scriptfunctions[i], "Os", pyhandle.get(), "Idle"), &ani);
//			}
////ani2->GetPyPtr();
////entity1_ragdoll->GetPyPtr();
//	}

	hvk->InitDone();
return R_OK; //DELETE
#endif // HVK_DEVELOP

#ifdef SND_DEVELOP
	/*// Init ID3Lib
	LPTSTR missingdllname;
	if(CheckID3LibSupport(&missingdllname) == false)
		return ERR(String("Library not found: ") << String(missingdllname));
	CHKALLOC(id3 = CreateID3Lib()); id3->Sync(GLOBALVAR_LIST);
	CHKRESULT(id3->Init());*/

	// Init WinAudio
	AudioFormat* formats;
	CHKALLOC(wad = CreateWinAudio()); wad->Sync(GLOBALVAR_LIST);
	CHKRESULT(wad->Init());
	CHKRESULT(wad->GetSupportedOutputFormats(0, &formats));

CHKRESULT(LoadFFmpeg()); //UNQUOTE

	/*// Init CryptoPP
	CHKALLOC(cpp = CreateCryptoPP(GLOBALVAR_LIST));
	CHKRESULT(cpp->Init());
	BYTE key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	BYTE iv[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	LPENCRYPTION enc;
	CHKRESULT(cpp->CreateEncryption(CM_CFB, &enc));
	enc->SetKeyWithIV(key, 16, iv, 16);
	enc->Put((BYTE*)"this is a test", strlen("this is a test"));
	enc->Finalize();

	LPDECRYPTION dec;
	CHKRESULT(cpp->CreateDecryption(CM_CFB, &dec));
	dec->SetKeyWithIV(key, 16, iv, 16);
	dec->Put((BYTE*)enc->GetCipherText().c_str(), enc->GetCipherText().size());
	dec->Finalize();*/
#endif

#ifdef WAD_DEVELOP
	// Init WinAudio
	AudioFormat* formats;
	CHKALLOC(wad = CreateWinAudio()); wad->Sync(GLOBALVAR_LIST);

#ifndef PSE_DEVELOP
	CHKRESULT(wad->Init());
	int numformats;
	CHKRESULT(wad->GetSupportedOutputFormats(0, &formats, &numformats));
	LPWADOUTDEVICE wadoutdev;
	CHKRESULT(wad->OpenOutputDevice(0, AudioFormat::FromChannelMask(14914, 8, SPEAKER_FRONT_CENTER), &wadoutdev));
wadoutdev->Volume = 0xA000;
	LPWADSOUND sound;
	//CHKRESULT(wadoutdev->CreateSound("test.wav", 0, &sound));
	CHKRESULT(wadoutdev->CreateSound("H:\\Games\\Portable DX-Ball 2\\Sounds\\Bang.wav", 0, &sound));
	/*CHKRESULT(wadoutdev->CreateSound(sizeof(short) * 10000, 10, &sound));
	float foo = 0.0f;
	for(int i = 0; i < 10000; i++)
	{
		foo = sin((float)i / 100.0f);
		sound->data.sdata[i] = (short)(foo * 0x7FFF);
	}*/
	sound->Play();
#endif
#endif

#ifdef MOVIE_DEVELOP
	CHKRESULT(LoadFFmpeg());
	SetTimer(fms->GetHwnd(0), 0, 200, NULL);
#endif

#ifdef SQL_DEVELOP
	// Init SQLite
	LPTSTR missingdllname;
	if(CheckSQLiteSupport(&missingdllname) == false)
		return ERR(String("Library not found: ") << String(missingdllname));
	CHKALLOC(sql = CreateSQLite()); sql->Sync(GLOBALVAR_LIST);

	LPDATABASE database;
	CHKRESULT(sql->Open("_test.db", &database));
#endif

#ifdef PDF_DEVELOP
	// Init HaruPdf
	LPTSTR missingdllname;
	if(CheckHaruPdfSupport(&missingdllname) == false)
		return ERR(String("Library not found: ") << String(missingdllname));
	CHKALLOC(pdf = CreateHaruPdf()); pdf->Sync(GLOBALVAR_LIST);

	/*LPDATABASE database;*/
	CHKRESULT(pdf->Open("test.pdf"));
#endif

#ifdef PSE_DEVELOP
/*PythonDictionary* globals = pse->GetLocals();
cle->PrintLine(globals->ToString());
delete globals;*/
	LPPYTHONSCRIPT script;
	CHKRESULT(pse->LoadScript("sample3.py", &script));

	/*pse->Execute("from Direct3D import *");
	pse->Execute("settings = OutputWindowSettings()");
	pse->Execute("settings.backbuffersize.width = 640");
	pse->Execute("settings.backbuffersize.height = 480");
	pse->Execute("settings.screenidx = 0");
	pse->Execute(String("settings.wnd = Engine.MakeHWND(") << String((int)form1->GetHwnd()) << String(")"));
	pse->Execute("d3dwnd = d3d.CreateOutputWindow(settings)");

	pse->Execute("d3dwnd.MakeConsoleTarget(11, 11)");
	pse->Execute("d3dwnd.ShowFps()");

	cle->PrintD3D(123);

	pse->Execute("cam = d3d.CreateCamera(0.0, 0.001, 200.0)");
	pse->Execute("cam.pos = Vector3(0.0, 0.0, -2.0)");
	pse->Execute("cam.viewmode = ViewMode.Fly");
	pse->Execute("d3dwnd.SetCamera(cam)");

	pse->Execute("sdr = d3dwnd.CreateRenderShader(\"shader withblending.fx\")");

	pse->Execute("sdesc = SphereShapeDesc()");
	pse->Execute("sdesc.radius = 1.0");
	pse->Execute("sdesc.slices = sdesc.stacks = 5");
	pse->Execute("obj = d3dwnd.CreateObject(sdesc)");
	pse->Execute("d3dwnd.RegisterForRendering(obj, RenderType.Default)");
	pse->Execute("obj.SetShader(sdr)");*/
#endif

	return R_OK;
}

#ifdef FMS_DEVELOP
#include <richedit.h> //DELETE

#include <Commctrl.h> //DELETE
#pragma comment (lib, "Comctl32.lib") //DELETE

void fill_richedit( HWND hwnd )
{
PARAFORMAT pf;
CHARFORMAT cf;

/*SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "this is a test\n");

// add text on the right
memset( &pf, 0, sizeof pf );
pf.cbSize = sizeof pf;
pf.dwMask = PFM_ALIGNMENT;
pf.wAlignment = PFA_RIGHT;
SendMessage( hwnd, EM_SETPARAFORMAT, FALSE, (LPARAM) &pf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "right\n");

// add text in the center
memset( &pf, 0, sizeof pf );
pf.cbSize = sizeof pf;
pf.dwMask = PFM_ALIGNMENT;
pf.wAlignment = PFA_CENTER;
SendMessage( hwnd, EM_SETPARAFORMAT, FALSE, (LPARAM) &pf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "centre\n");

// add text on the left
memset( &pf, 0, sizeof pf );
pf.cbSize = sizeof pf;
pf.dwMask = PFM_ALIGNMENT;
pf.wAlignment = PFA_LEFT;
SendMessage( hwnd, EM_SETPARAFORMAT, FALSE, (LPARAM) &pf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "left\n");

// add bold text
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_BOLD;
cf.dwEffects = CFE_BOLD;
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "bold ");

// add italic text
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_ITALIC | CFM_BOLD;
cf.dwEffects = CFE_ITALIC;
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "italic  ");

// add underlined text
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_ITALIC | CFM_BOLD | CFM_UNDERLINE;
cf.dwEffects = CFE_UNDERLINE;
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "underline ");

// add strikeout text
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_ITALIC | CFM_BOLD | CFM_UNDERLINE | CFM_STRIKEOUT;
cf.dwEffects = CFE_STRIKEOUT;
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "strikeout");

// add normal text
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT;
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) " normal\n");*/

// add red text
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_COLOR;
cf.crTextColor = RGB(255,0,0);
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "red ");

// add green text
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_COLOR;
cf.crTextColor = RGB(0,255,0);
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "green ");

// add blue text
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_COLOR;
cf.crTextColor = RGB(0,0,255);
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "blue\n");

/*// try some fonts
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_FACE | CFM_SIZE;
cf.yHeight = 300;
strcpy(cf.szFaceName, "Tahoma");
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "tahoma ");

// try some fonts
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_FACE;
strcpy(cf.szFaceName, "Helvetica");
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "Helvetica ");

// try some fonts
memset( &cf, 0, sizeof cf );
cf.cbSize = sizeof cf;
cf.dwMask = CFM_FACE;
strcpy(cf.szFaceName, "Courier");
SendMessage( hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage( hwnd, EM_REPLACESEL, FALSE, (LPARAM) "Courier ");*/
}
#endif

void __stdcall TabControl_TabChanged(ITabControl* sender, int tabindex, LPVOID tag, LPVOID user)
{
	cle->PrintLine(String("Tab selected: ") << String(tabindex));
}

String __stdcall Button_ShowTooltip(ITooltip* sender, HWND controlhandle)
{
	return "something";
}

Result __stdcall PostInit()
{
	Result rlt;

/*CHKRESULT(D3DXCreateTexture(d3d->GetDevice(0), movie->GetVideoWidth(0), movie->GetVideoHeight(0), 1, D3DUSAGE_DYNAMIC, D3DFMT_R8G8B8, D3DPOOL_DEFAULT, &texvideo));

// Create screen quad vertexbuffer
CHKRESULT(d3d->GetDevice(0)->CreateVertexBuffer(4 * (sizeof(D3DXVECTOR4) + 2 * sizeof(float)),
												  D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_TEX1,
												  D3DPOOL_DEFAULT, &vbscreenquad, NULL));
struct vfscreenquad{D3DXVECTOR4 p; float u, v;} *vtx;
Size<float>fsize((float)d3d->GetBackbufferSize(0).width, (float)d3d->GetBackbufferSize(0).height);
vbscreenquad->Lock(0, 0, (void**)&vtx, 0);
{
	vtx[0].p = D3DXVECTOR4(-0.5f, fsize.height - 0.5f, 0.0f, 1.0f);
	vtx[1].p = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
	vtx[2].p = D3DXVECTOR4(fsize.width - 0.5f, fsize.height - 0.5f, 0.0f, 1.0f);
	vtx[3].p = D3DXVECTOR4(fsize.width - 0.5f, -0.5f, 0.0f, 1.0f);
	vtx[0].u = 0.0f; vtx[0].v = 1.0f;
	vtx[1].u = 0.0f; vtx[1].v = 0.0f;
	vtx[2].u = 1.0f; vtx[2].v = 1.0f;
	vtx[3].u = 1.0f; vtx[3].v = 0.0f;
}
vbscreenquad->Unlock();

// Create effect
LPD3DXBUFFER errbuf;
IFFAILED(D3DXCreateEffectFromFile(d3d->GetDevice(0), "Video.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &effect, &errbuf))
{
	if(errbuf)
		return ERR((LPSTR)errbuf->GetBufferPointer());
	else
		return rlt;
}
effect->SetTexture("Texture", texvideo);

// Create effect2
IFFAILED(D3DXCreateEffectFromFile(d3d->GetDevice(0), "Video2.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &effect2, &errbuf))
{
	if(errbuf)
		return ERR((LPSTR)errbuf->GetBufferPointer());
	else
		return rlt;
}
effect2->SetTexture("Texture", texvideo);*/

#ifdef SND_DEVELOP
LPBUTTON cmdPause = CreateButton();
cmdPause->Enabled = true;
cmdPause->Text = String("Pause");
cmdPause->Location = Point<int>(100, 100);
cmdPause->RegisterForMouseClick(cmdPause_OnClick);
form1->AddControl((IControl*)cmdPause);
#endif

#ifdef FMS_DEVELOP
fms->DisableVisualStates();

LPLABEL lblTest = CreateLabel();
lblTest->Text = String("I'm a label :D");
form1->AddControl(lblTest);

/*LPNUMERICUPDOWN nud = CreateNumericUpDown();
nud->SetText("15");
form1->AddControl(nud);
short foo = nud->Value;
cle->Print(foo);*/

/*LPLISTVIEW lv = CreateListview();
lv->Bounds = Rect<int>(100, 0, 200, 600);
form1->AddControl(lv);

lv->InsertGroup("Some Group");

lv->InsertColumn("Parameter", 100);
lv->InsertColumn("Value", 100);

lv->InsertItem("item 1;subitem 1", ';', 0);
lv->InsertItem("item 2;subitem 2", ';', 0);
lv->InsertItem("item 3;subitem 3", ';', 0);*/

//form1->HasMaximizeBox = false;

/*LPBUTTON cmdTest1 = CreateButton();
cmdTest1->Location = Point<int>(400, 0);
form1->AddControl((IControl*)cmdTest1);
//cmdTest1->Enabled = false;
cmdTest1->Visible = false;
//cmdTest1->Visible = true;
cmdTest1->Text = String("Button2");*/

/*LPCHECKBOX cmdTest2 = CreateCheckBox();
cmdTest2->CState = CS_INDETERMINATE;
cmdTest2->Text = String("Checkbox");
cmdTest2->Location = Point<int>(100, 100);
form1->AddControl((IControl*)cmdTest2);
cmdTest2->Bounds = Rect<int>(200, 100, 100, 50);

LPRADIOBUTTON cmdTest3 = CreateRadioButton();
cmdTest3->Checked = true;
cmdTest3->Text = String("Radio button");
form1->AddControl((IControl*)cmdTest3);
cmdTest3->Location = Point<int>(200, 200);

LPGROUPBOX gbTest = CreateGroupBox();
gbTest->Text = String("Group box");
form1->AddControl((IControl*)gbTest);
gbTest->Bounds = Rect<>(250, 250, 500, 500);*/

/*InitCommonControls(); //DELETE

LPBUTTON cmdTest1 = CreateButton();
cmdTest1->Location = Point<int>(300, 300);
form1->AddControl((IControl*)cmdTest1);
cmdTest1->Text = String("Button2");

//HWND abc = CreateWindow("BUTTON", "abc", WS_VISIBLE | ES_MULTILINE | WS_CHILD, 300, 400, 80, 23, form1->GetHwnd(), (HMENU)0, fms->GetInstance(), NULL);

LPTEXTBOX txtTest = CreateTextBox();
txtTest->Text = String("Text box");
txtTest->Bounds = Rect<>(100, 100, 300, 28);
form1->AddControl((IControl*)txtTest);

// Create context menu
LPMENU ctxmenu;
CHKRESULT(txtTest->CreateContextMenu(&ctxmenu));
ctxmenu->AddMenuItem("Test");*/

/*LPMENU ctxmenu;
CHKRESULT(form1->CreateContextMenu(&ctxmenu));
ctxmenu->AddMenuItem("Test");*/


/*LPTABCONTROL tabs = CreateTabControl();
tabs->Bounds = Rect<int>(100, 0, 200, 600);
tabs->SetTabChangedCallback(TabControl_TabChanged, NULL);
form1->AddControl(tabs);
tabs->InsertTab("testtab", 0);
tabs->InsertTab("testtab2", 1);

Rect<> clientbounds;
tabs->GetClientBounds(&clientbounds);

LPRICHEDIT redit = CreateRichEdit(true, true);
redit->Bounds = Rect<int>(0, 0, 400, 600);
form1->AddControl(redit);

redit->SetFormat(RGB(0, 255, 0), RGB(255, 255, 128), "Courier");
redit->InsertText("this is a test");

redit->SetFormat(RGB(0, 0, 255), RGB(255, 255, 255));
redit->InsertText("\nBLA");*/

//fill_richedit(redit->GetHwnd());*/

/*PARAFORMAT2 pf;
ZeroMemory(&pf, sizeof(PARAFORMAT2));
pf.cbSize = sizeof(PARAFORMAT2);
pf.dwMask = PFM_LINESPACING;
pf.bLineSpacingRule = 2;
pf.dyLineSpacing = 0;
DWORD r = SendMessage(handle, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
cle->PrintLine((int)r);
err = GetLastError();
cle->PrintLine((int)err);*/



/*HWND hwndButton = CreateWindow(WC_BUTTON, "Start", WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON, 12, 532, 150, 52, fms->GetHwnd(0), (HMENU)0, __hInst, NULL);

HTHEME hTheme = NULL;
hTheme = OpenThemeData(hwndButton, VSCLASS_BUTTON);
HDC hDC = GetWindowDC(fms->GetHwnd(0));
HFONT fnt = CreateFont(-MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Microsoft Sans Serif");
SelectObject(hDC, fnt);
DrawMyControl(hDC, hwndButton, hTheme, PBS_NORMAL);
DrawMyControl(hDC, hwndButton, hTheme, PBS_HOT);
DrawMyControl(hDC, hwndButton, hTheme, PBS_PRESSED);
DrawMyControl(hDC, hwndButton, hTheme, PBS_DISABLED);
DrawMyControl(hDC, hwndButton, hTheme, PBS_DEFAULTED);
DrawMyControl(hDC, hwndButton, hTheme, PBS_DEFAULTED_ANIMATING);
ReleaseDC(fms->GetHwnd(0), hDC);
if(hTheme)
	CloseThemeData(hTheme);*/
#endif

	return R_OK;
}

Result LoadFFmpeg()
{
	Result rlt;

	UnloadFFmpeg();

#if defined(MOVIE_DEVELOP) || defined(SND_DEVELOP)
	// Load FFmpeg
#ifdef _DEBUG
	if(!(isffmpeg_lib = LoadLibrary("ISFFmpeg_d.dll")))
		return ERR("ISFFmpeg_d.dll not found");
	if(!(isffmpeg_procs[0] = GetProcAddress(isffmpeg_lib, "CreateFFmpeg")))
		return ERR("CreateFFmpeg not found in ISFFmpeg_d.dll");
	if(!(isffmpeg_procs[1] = GetProcAddress(isffmpeg_lib, "CheckFFmpegSupport")))
		return ERR("CheckFFmpegSupport not found in ISFFmpeg_d.dll");
#else
	if(!(isffmpeg_lib = LoadLibrary("ISFFmpeg.dll")))
		return ERR("ISFFmpeg.dll not found");
	if(!(isffmpeg_procs[0] = GetProcAddress(isffmpeg_lib, "CreateFFmpeg")))
		return ERR("CreateFFmpeg not found in ISFFmpeg.dll");
	if(!(isffmpeg_procs[1] = GetProcAddress(isffmpeg_lib, "CheckFFmpegSupport")))
		return ERR("CheckFFmpegSupport not found in ISFFmpeg.dll");
#endif

	// Init FFmpeg
	LPTSTR missingdllname;
	if(CheckFFmpegSupport(&missingdllname) == false)
		return ERR(String("Library not found: ") << String(missingdllname));
	CHKALLOC(ffm = CreateFFmpeg()); ffm->Sync(GLOBALVAR_LIST);

	CHKRESULT(ffm->Init());
#endif

#ifdef SND_DEVELOP
	// Init movie
	//CHKRESULT(ffm->LoadFile("H:\\Music\\Metal\\ACDC\\FLAC\\Back In Black (1980)\\03 - What Do You Do For Money Honey.flac", &movie));
	//CHKRESULT(ffm->LoadFile("H:\\Music\\Metal\\ACDC\\FLAC\\Back In Black (1980)\\01 - Hells Bells.flac", &movie));
	CHKRESULT(ffm->LoadFile("H:\\Music\\Metal\\ACDC\\Back In Black (1980)\\03 - What Do You Do For Money Honey.mp3", &movie));
	movie->EnableStream(0, CODEC_TYPE_AUDIO);

	/*// Init CryptoPP
	CHKALLOC(cpp = CreateCryptoPP()); cpp->Sync(GLOBALVAR_LIST);
	CHKRESULT(cpp->Init());*/

	/*// Load tags
	LPID3TAG tag;
	CHKRESULT(id3->LoadTagFromFile(FilePath("H:\\Music\\Metal\\ACDC\\Back In Black (1980)\\test.mp3"), &tag));
	const Mp3_Headerinfo* mp3header = tag->ReadHeader();*/

	/*// Init movie2
	CHKRESULT(ffm->CreateNew("H:\\Music\\Metal\\ACDC\\Back In Black (1980)\\03 - What Do You Do For Money Honey.mp3x", &movie2, &String("mp3")));

	LPENCRYPTION enc;
	CHKRESULT(cpp->CreateEncryption(CM_CFB, CM_ENCRYPT, &enc));
	BYTE key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	BYTE iv[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	enc->SetKeyWithIV(key, 16, iv, 16);

	AVPacket pkt;
	bool finished;
	int ctr = 0;
	size_t cipherptr = 0;
	while(1)
	{
		IFFAILED(movie->ReadRawPacket(&pkt, NULL, &finished))
			break;
		if(finished)
			break;

		enc->Put(pkt.data + 4, pkt.size - 4);
		memcpy(pkt.data + 4, enc->GetResult().c_str() + cipherptr, pkt.size - 4);
		cipherptr += pkt.size - 4;

		CHKRESULT(movie2->WriteRawPacket(&pkt));
		movie2->FreePacket(&pkt);
		ctr++;
	}
	enc->Finalize();
	cle->Print(String(ctr) << String(" packets written"));
	PostQuitMessage(0);*/



	/*CHKRESULT(movie->PrepareVideoStreaming(0, NULL, PIX_FMT_BGR24, SWS_BICUBIC, &buffer));
	movie->SetVideoFrame(0, videoframe);

	CHKRESULT(ffm->PrepareBlittingToWnd(fms->GetHwnd(0), new Rect<int>(0, 0, movie->GetVideoWidth(0), movie->GetVideoHeight(0))));
	//CHKRESULT(ffm->PrepareBlittingToWnd(fms->GetHwnd(0), new Rect<int>(0, 0, 800, 600)));*/

	// Init win-audio output device
	CHKRESULT(wad->OpenOutputDevice(0, AudioFormat::FromChannelMask(44100, 16, SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT), &wadoutdev));
wadoutdev->Volume = 0xA000;
/*LPWADSOUND snd;
CHKRESULT(wadoutdev->CreateSound(0x2000000, 0, &snd));*/
LPWADSTREAMEDSOUND snd;
CHKRESULT(wadoutdev->CreateStreamedSound(AVCODEC_MAX_AUDIO_FRAME_SIZE, 0x1200, 4, AudioStreamingCallback, NULL, &snd)); //0x1200 * 80, 0x1200, 0x4000

/*float t = 0.0f;
for(DWORD i = 0; i < snd->datalen / sizeof(short) / 2; i++)
{
	snd->data.sdata[i * 2 + 0] = (short)((snd->datalen / sizeof(short)) * sin(t));
	snd->data.sdata[i * 2 + 1] = (short)((snd->datalen / sizeof(short)) * sin(t * 1.5f));
	t += 0.1f;
}*/
/*int16_t* bla;
int blabla;
bool finishedstreaming;

int dataptr = 0;
//while(1)
for(int i = 0; i < 20; i++)
	movie->StreamContent(eng->timer.dt, &finishedstreaming, &bla, &blabla);
for(int i = 0; i < 3000; i++)
{
	IFFAILED(movie->StreamContent(eng->timer.dt, &finishedstreaming, &bla, &blabla))
		continue;
	if(dataptr + blabla >= (int)snd->datalen)
		break;

	for(int j = 0; j < blabla / 2; j += 2)
	{
		//bla[j + 1] = bla[j];
		//bla[j] = (int16_t)((uint32_t)bla[j] - 0x8000);
		//cle->Print(bla[j]);
	}

	memcpy(snd->data.cdata + dataptr, bla, blabla);
	dataptr += blabla;
	//for(int k = 0; k < blabla / 2; k++)
	//	cle->Print(bla[k]);
	delete[] bla;
}*/

snd->Play();
#endif
#ifdef MOVIE_DEVELOP
	// Init movie
	//CHKRESULT(ffm->LoadFile("G:\\HD Movies\\Dumm und Dümmer Extended Cut.mkv", &movie)); //"knife.mpg"
	//CHKRESULT(ffm->LoadFile("D:\\Movies\\Family Guy\\Season 1\\01 - Der großzügige Wohlfahrtsscheck.avi", &movie));
	CHKRESULT(ffm->LoadFile("G:\\HD Movies\\Black Hawk Down.mkv", &movie));

	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	srand(t.LowPart);
//videoframe = (rand() % 100) * 80;//(rand() % 100) * 80000;

	PixelFormat pixfmt = PIX_FMT_BGR24;//PIX_FMT_YUV420P;
	CHKRESULT(movie->PrepareVideoStreaming(0, &Size<int>(fms->GetClientWidth(0), fms->GetClientHeight(0)), pixfmt, SWS_AREA, &buffer)); //SWS_BICUBIC
	//CHKRESULT(movie->PrepareVideoStreaming(0, &Size<int>(640, 480), pixfmt, SWS_AREA, &buffer)); //SWS_BICUBIC
//movie->SetVideoFrame(0, videoframe);

	//CHKRESULT(ffm->PrepareBlittingToWnd(fms->GetHwnd(0), new Rect<int>(0, 0, movie->GetVideoWidth(0), movie->GetVideoHeight(0))));
	CHKRESULT(ffm->PrepareBlittingToWnd(fms->GetHwnd(0), new Rect<int>(0, 0, fms->GetClientWidth(0), fms->GetClientHeight(0))));

/*float t = 0.0f;
for(DWORD i = 0; i < snd->datalen / sizeof(short) / 2; i++)
{
	snd->data.sdata[i * 2 + 0] = (short)((snd->datalen / sizeof(short)) * sin(t));
	snd->data.sdata[i * 2 + 1] = (short)((snd->datalen / sizeof(short)) * sin(t * 1.5f));
	t += 0.1f;
}*/
/*int16_t* bla;
int blabla;
bool finishedstreaming;

int dataptr = 0;
//while(1)
for(int i = 0; i < 20; i++)
	movie->StreamContent(eng->timer.dt, &finishedstreaming, &bla, &blabla);
for(int i = 0; i < 3000; i++)
{
	IFFAILED(movie->StreamContent(eng->timer.dt, &finishedstreaming, &bla, &blabla))
		continue;
	if(dataptr + blabla >= (int)snd->datalen)
		break;

	for(int j = 0; j < blabla / 2; j += 2)
	{
		//bla[j + 1] = bla[j];
		//bla[j] = (int16_t)((uint32_t)bla[j] - 0x8000);
		//cle->Print(bla[j]);
	}

	memcpy(snd->data.cdata + dataptr, bla, blabla);
	dataptr += blabla;
	//for(int k = 0; k < blabla / 2; k++)
	//	cle->Print(bla[k]);
	delete[] bla;
}*/
#endif

	return R_OK;
}

void UnloadFFmpeg()
{
#if defined(MOVIE_DEVELOP) || defined(SND_DEVELOP)
	Result rlt;

	if(!isffmpeg_lib)
		return;

	// Destroy movies
	if(movie)
	{
		videoframe = movie->GetVideoFrame(0);
		movie->EndVideoStreaming(0);
		movie = NULL;
	}
	if(movie2)
		movie = NULL;

	// Destroy FFmpeg
	if(ffm)
	{
		ffm->EndBlittingToWnd();
		RELEASE(ffm)
	}

	// Unload FFmpeg
	FreeLibrary(isffmpeg_lib);
#endif

#ifdef SND_DEVELOP
	// Destroy win-audio output device
	if(wad)
		RELEASE(wadoutdev);
#endif
}

void OnPaint()
{
#ifdef MOVIE_DEVELOP
	if(ffm && movie)
	{
		bool finishedstreaming;
		movie->StreamContent(eng->time.dt, &finishedstreaming);
		ffm->BlitToWnd(buffer);
		if(finishedstreaming)
			PostQuitMessage(0);
		//OutputDebugString(String((int)movie->GetVideoFrame(0)) << String('\n'));
	}
#endif
}

/*float tx = -0.7f, ty = -0.35f, tz = -2.5f;
float sx = 0.0015f, sy = 0.0015f, sz = 0.0015f;
//float rx = 18.5f, ry = -28.7f, rz = -12.5f;
//float rx = 20.4f, ry = 35.7f, rz = 12.1f;
float rx = 23.4f, ry = 49.6f, rz = 20.3f;*/

/*float tx = -0.8f, ty = 0.3f, tz = -2.5f;
float sx = 0.0017f, sy = 0.0017f, sz = 0.0017f;
float rx = 29.2f, ry = 38.1f, rz = 15.2f;

float lx = 0.321f, ly = 1.3f, lz = -1.5f;*/

void __stdcall OnEnter(int key, LPVOID user)
{
	/*cle->PrintLine("\t\t\t<transform>");
	cle->PrintLine(String("\t\t\t\t<translate x=\"") << String(tx) << String("\" y=\"") << String(ty) << String("\" z=\"") << String(tz) << String("\"/>"));
	cle->PrintLine(String("\t\t\t\t<scale x=\"") << String(sx) << String("\" y=\"") << String(sy) << String("\" z=\"") << String(sz) << String("\"/>"));
	cle->PrintLine(String("\t\t\t\t<rotateX theta=\"") << String(rx) << String("\"/>"));
	cle->PrintLine(String("\t\t\t\t<rotateY theta=\"") << String(ry) << String("\"/>"));
	cle->PrintLine(String("\t\t\t\t<rotateZ theta=\"") << String(rz) << String("\"/>"));
	cle->PrintLine("\t\t\t</transform>");
	cle->PrintLine("");
	cle->PrintLine("<point_light>");
	cle->PrintLine("\t<color r=\"0.0\" g=\"0.63\" b=\"0.91\"/>");
	cle->PrintLine(String("\t<position x=\"") << String(lx) << String("\" y=\"") << String(ly) << String("\" z=\"") << String(lz) << String("\"/>"));
	cle->PrintLine("</point_light>");*/
}

void __stdcall Cyclic()
{
	Result rlt;



/*float sign = dip->keys[Key::LShift] ? -1.0f : 1.0f;
float speed = dip->keys[Key::Space] ? 10.0f : 5.0f;
if(dip->keys[Key::X])
	rx += eng->time.dt * 10.0f * speed * sign;
if(dip->keys[Key::Z])
	ry += eng->time.dt * 10.0f * speed * sign;
if(dip->keys[Key::Y])
	rz += eng->time.dt * 10.0f * speed * sign;
cle->PrintD3D(String("x = ") << String(rx));
cle->PrintD3D(String("y = ") << String(ry));
cle->PrintD3D(String("z = ") << String(rz));

if(dip->keys[Key::L])
{
	if(dip->keys[Key::Up])
		ly += eng->time.dt * 1.0f;
	if(dip->keys[Key::Down])
		ly -= eng->time.dt * 1.0f;
	if(dip->keys[Key::Left])
		lx += eng->time.dt * 1.0f;
	if(dip->keys[Key::Right])
		lx -= eng->time.dt * 1.0f;
}



D3DXMATRIX mRotX, mRotY, mRotZ, mTrans;
D3DXMatrixTranslation(&mTrans, tx, ty, tz);
obj->scl = Vector3(sx, sy, sz);
D3DXMatrixRotationX(&mRotX, rx * PI / 180.0f);
D3DXMatrixRotationY(&mRotY, ry * PI / 180.0f);
D3DXMatrixRotationZ(&mRotZ, rz * PI / 180.0f);
obj->transform = mRotX * mRotY * mRotZ * mTrans;
obj->orientmode = OM_MATRIX;

D3DXMatrixTranslation(&obj2->transform, lx, ly, lz);
obj2->orientmode = OM_MATRIX;*/





/*D3DLOCKED_RECT lrect;
IFFAILED(texvideo->LockRect(0, &lrect, NULL, D3DLOCK_DISCARD))
{
	PostQuitMessage(0);
	return;
}

DWORD x, y;
DWORD w = movie->GetVideoWidth(0);
DWORD h = movie->GetVideoHeight(0);
for(x = 0; x < w; x++)
	for(y = 0; y < h; y++)
	{
		((BYTE*)lrect.pBits)[x * 4 + y * lrect.Pitch + 0] = buffer[(y * w + x) * 3 + 0];
		((BYTE*)lrect.pBits)[x * 4 + y * lrect.Pitch + 1] = buffer[(y * w + x) * 3 + 1];
		((BYTE*)lrect.pBits)[x * 4 + y * lrect.Pitch + 2] = buffer[(y * w + x) * 3 + 2];
	}

IFFAILED(texvideo->UnlockRect(0))
{
	PostQuitMessage(0);
	return;
}*/



	//if(wip)
	//	wip->Update();

#ifdef D3D_DEVELOP
	if(dip)
		dip->Update();

#endif

#ifdef HVK_DEVELOP
	if(hvk)
	{
/*if(entity2_ragdoll)
{
	entity2_ragdoll->LookAt(obj->pos, 1.0f);
}*/
		if(vehicle)
		{
			vehicle->acceleration = (dip->keys[Key::Down] != 0) * 0.5f - (dip->keys[Key::Up] != 0) * 0.5f;
			vehicle->steering = (dip->keys[Key::Left] != 0) * 1.0f - (dip->keys[Key::Right] != 0) * 1.0f;
			vehicle->handbrakepulled = dip->keys[Key::Numpad0] != 0;
		}

		if(eng->time.t >= 1.5f)
			hvk->Update();

if(entity2_pose)
{
	Quaternion rot(0.0f, 0.0f, sin(eng->time.t), cos(eng->time.t));
	entity2_pose->SetRotation(&rot, 2);
}
	}
#endif

#ifdef D3D_DEVELOP
	/*pse->Execute("v0 = Vector2(10, 10)");
	pse->Execute("v1 = v0 + Vector2(500, 500)");
	pse->Execute("c0 = Color()\nc0.r = 1\nc0.a = 1");
	pse->Execute("c1 = Color()\nc1.b = 1\nc1.a = 1");
	pse->Execute("d3dwnd.DrawLine(v0, v1, Color(0xFF0000FF), c1)");*/

	if(d3d)
		d3d->Render();
#endif

#ifdef HVK_DEVELOP
static bool mdown = false;
if(dip->mouse.rgbButtons[0])
{
	POINT mousepos;
	GetCursorPos(&mousepos);
	ScreenToClient(fms->GetHwnd(0), &mousepos);

	//float dist;
	Vector3 vdir;
	//d3dwnd->GetCamera()->ComputeViewDir(&vdir);
	d3dwnd->GetCamera()->ComputePixelDir(Point<int>(mousepos.x, mousepos.y), &vdir);
	Vector3 vpos = d3dwnd->GetCamera()->pos;
	if(mdown)
		hvk->MousePickDrag(vpos, vdir);
	else
	{
		hvk->MousePickGrab(vpos, vdir);
		mdown = true;
	}
}
else if(mdown)
{
	hvk->MousePickRelease();
	mdown = false;
}
#endif
}
CYCLICFUNC cyclic = Cyclic;
//CYCLICFUNC cyclic = NULL;

#ifdef D3D_DEVELOP
void __stdcall Render(IOutputWindow* wnd)
{
/*D3DXMATRIX w;
D3DXMatrixIdentity(&w);
d3dwnd->BeginDrawLine(w);
//d3dwnd->DrawLine(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), D3DXCOLOR(0xFF00FFFF));
d3dwnd->DrawLine(D3DXVECTOR2(100.0f, 100.0f), D3DXVECTOR2(200.0f, 200.0f), D3DXCOLOR(0xFF00FFFF), D3DXCOLOR(0xFF000000));
d3dwnd->DrawLine(D3DXVECTOR2(200.0f, 200.0f), D3DXVECTOR2(300.0f, 100.0f), D3DXCOLOR(0xFF000000), D3DXCOLOR(0xFFFFFFFF));

d3dwnd->DrawLine(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 100.0f, 0.0f), D3DXCOLOR(0xFF000000), D3DXCOLOR(0xFFFFFFFF));
d3dwnd->EndDrawLine();*/

/*d3d->GetDevice(0)->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
d3d->GetDevice(0)->SetStreamSource(0, vbscreenquad, 0, sizeof(D3DXVECTOR4) + 2 * sizeof(float));
d3d->GetDevice(0)->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
d3d->GetDevice(0)->SetTexture(0, texvideo);

UINT passcount, pass;
effect->Begin(&passcount, NULL);
for(pass = 0; pass < passcount; pass++)
{
	effect->BeginPass(pass);
	d3d->GetDevice(0)->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	effect->EndPass();
}
effect->End();*/
}
void __stdcall Render2(IOutputWindow* wnd)
{
/*d3d->GetDevice(1)->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
d3d->GetDevice(1)->SetStreamSource(0, vbscreenquad, 0, sizeof(D3DXVECTOR4) + 2 * sizeof(float));
d3d->GetDevice(1)->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
d3d->GetDevice(1)->SetTexture(0, texvideo);

UINT passcount, pass;
effect2->Begin(&passcount, NULL);
for(pass = 0; pass < passcount; pass++)
{
	effect2->BeginPass(pass);
	d3d->GetDevice(1)->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	effect2->EndPass();
}
effect2->End();*/
}
#endif

INT16 round(float x)
{
	INT16 sign = x < 0 ? -1 : 1;
	x *= sign;
	float remainder = x - (float)(INT16)x;
	return sign * (remainder < 0.5f ? (INT16)x : (INT16)x + 1);
}

void __stdcall OnSpace(int key, LPVOID user)
{
#ifdef D3D_DEVELOP
/*POINT mousepos;
GetCursorPos(&mousepos);
ScreenToClient(fms->GetHwnd(0), &mousepos);

Vector3 vdir;
//d3dwnd->GetCamera()->ComputeViewDir(&vdir);
d3dwnd->GetCamera()->ComputePixelDir(Point<int>(mousepos.x, mousepos.y), &vdir);
Vector3 vpos = d3dwnd->GetCamera()->pos;

UINT subset, blendpart;
obj2->RayTrace(&vpos, &vdir, NULL, &subset, &blendpart);
cle->PrintLine(String((int)blendpart) << " - " << String((int)subset));*/

if(weapon && obj)
{
	//Quaternion rot;
	//::D3DXQuaternionRotationAxis(&rot, &Vector3(0.0f, 1.0f, 0.0f), 0.0f);
	weapon->Fire(&(Vector3)(obj->pos + Vector3(0.0f, 1.0f, 0.0f)), &obj->qrot);
}

if(blevel)
{
	Vector3 vdir;
	d3dwnd->GetCamera()->ComputeViewDir(&vdir);
	Vector3 vpos = d3dwnd->GetCamera()->pos;
	/*vpos.x += vpos.y * (vdir.x / vdir.y);
	vpos.z += vpos.y * (vdir.z / vdir.y);
	vpos.y = 0.0f;
	cle->PrintD3D(String((INT16)vpos.x) << String(", ") << String((INT16)vpos.y) << String(", ") << String((INT16)vpos.z));*/
	vpos.x += vdir.x;
	vpos.y += vdir.y;
	vpos.z += vdir.z;

/*	INT16 x = round(vpos.x), y = round(vpos.y), z = round(vpos.z);
	IBoxedLevel::BoxDesc newbox(x, y, z, 0, 0);
	if(blevel->GetBoxTypeIdx(newbox) == (UINT16)-1)
	{
		blevel->AddBox(newbox);

		HvkBoxShapeDesc bdesc;
		bdesc.size = Vector3(1.0f, 1.0f, 1.0f);
		bdesc.mtype = MT_FIXED;
		LPREGULARENTITY entityBox;
		hvk->CreateRegularEntity(&bdesc, IHavok::LAYER_LANDSCAPE, &Vector3((float)x, (float)y, (float)z), &Quaternion(0.0f, 0.0f, 0.0f, 1.0f), &entityBox);
	}*/
	//blevel->RemoveBox(1, 0, 1);
}

if(ani)
{
	ani->Enabled = true;//SetTime(0.0f);
}

	/*static bool foo = false;

	foo = !foo;

	if(foo)
		for(int i = 0; i < 19; i++)
			hvk->EnableViewer((HvkViewer)i);
	else
		for(int i = 0; i < 19; i++)
			hvk->DisableViewer((HvkViewer)i);*/
//entity2_ragdoll->ApplyPose(NULL);
#endif
}

void __stdcall OnPlus(int key, LPVOID user)
{
	if(ani)
	{
		ani->Weight = min(1.0f, ani->Weight + 1.1f);
		ani2->Weight = 1.0f - ani->Weight;
		cle->PrintD3D(String("ani weight = ") << String(ani->Weight) << String(" - ani2 weight = ") << String(ani2->Weight));
	}
}

void __stdcall OnMinus(int key, LPVOID user)
{
	if(ani)
	{
		ani->Weight = max(0.0f, ani->Weight - 1.1f);
		ani2->Weight = 1.0f - ani->Weight;
		cle->PrintD3D(String("ani weight = ") << String(ani->Weight) << String(" - ani2 weight = ") << String(ani2->Weight));
	}
}

void __stdcall End()
{
#ifdef PSE_DEVELOP
	RELEASE(pse);
#endif

#ifdef SND_DEVELOP
	UnloadFFmpeg();
	RELEASE(ffm);
	//RELEASE(cpp);
	RELEASE(wad);
#endif
#ifdef WAD_DEVELOP
	RELEASE(wad)
#endif
#ifdef XA2_DEVELOP
	RELEASE(xa2)
#endif
#ifdef MOVIE_DEVELOP
	UnloadFFmpeg();
	RELEASE(ffm);
#endif
	/*RELEASE(dip);
	RELEASE(wip);*/
#ifdef D3D_DEVELOP
	RELEASE(d3d);
#endif
#ifdef HVK_DEVELOP
	RELEASE(hvk);
#endif
	RELEASE(cle);
	RELEASE(fms);
#ifdef MIDI_DEVELOP
	RELEASE(mid);
#endif
#ifdef WSK_DEVELOP
	RELEASE(wsk);
#endif
#ifdef SQL_DEVELOP
	RELEASE(sql);
#endif
#ifdef PDF_DEVELOP
	RELEASE(pdf);
#endif
#ifdef GUI_DEVELOP
	RELEASE(gui);
#endif
}