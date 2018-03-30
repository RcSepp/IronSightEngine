#ifndef __ISENGINE_H
#define __ISENGINE_H

#include <windows.h>
#ifdef _WIN32
#include <windowsx.h>
#include <mmreg.h>
#else
#include <time.h>
#endif
#include <math.h>
#include <ISConstants.h>
#include <ISTypes.h>
#include <ISInterfaces.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#define PI									3.141592654f
#define REMOVE(ptr)							{if(ptr) {delete (ptr); (ptr) = NULL;}}
#define REMOVE_ARRAY(ptr)					{if(ptr) {delete[] (ptr); (ptr) = NULL;}}
#define RELEASE(ptr)						{if(ptr) {(ptr)->Release(); (ptr) = NULL;}}
#define READONLY(type, var, getfunc)		protected: type var; public: __forceinline type getfunc() const {return var;}
#define READONLY_BYREF(type, var, getfunc)	protected: type var; public: __forceinline type& getfunc() {return var;}
#define DEFAULT_WINDOWCLASS					"DefaultWinClass"


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum Orientation {OT_HORIZONTAL, OT_VERTICAL};
enum DockType
{
	DT_TOPLEFT = 0x00,
	DT_TOPCENTER = 0x01,
	DT_TOPRIGHT = 0x02,
	DT_MIDDLELEFT = 0x10,
	DT_MIDDLECENTER = 0x11,
	DT_MIDDLERIGHT = 0x12,
	DT_BOTTOMLEFT = 0x20,
	DT_BOTTOMCENTER = 0x21,
	DT_BOTTOMRIGHT = 0x22
};


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
typedef Result (CALLBACK* INITFUNC)();
typedef void (CALLBACK* CYCLICFUNC)();
typedef void (CALLBACK* QUITFUNC)();
typedef bool (CALLBACK* CUSTOMWNDPROC)(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam, LPVOID user);
typedef void (CALLBACK* TIMERELAPSED_CALLBACK)(LPVOID user);


//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct MouseEventArgs
{
public:
	UINT msg;
	WPARAM wparam;
	LPARAM lparam;
	Point<int> mousepos;
	short wheelpos;
	inline bool IsLeftButtonDown() const {return (wparam & MK_LBUTTON) != 0;}
	inline bool IsMiddleButtonDown() const {return (wparam & MK_MBUTTON) != 0;}
	inline bool IsRightButtonDown() const {return (wparam & MK_RBUTTON) != 0;}
	inline bool IsX1ButtonDown() const {return (wparam & MK_XBUTTON1) != 0;}
	inline bool IsX2ButtonDown() const {return (wparam & MK_XBUTTON2) != 0;}
	inline bool IsCtrlKeyDown() const {return (wparam & MK_CONTROL) != 0;}
	inline bool IsShiftKeyDown() const {return (wparam & MK_SHIFT) != 0;}
#ifdef _WIN32
	__declspec(property(get=IsLeftButtonDown)) bool ldown;
	__declspec(property(get=IsMiddleButtonDown)) bool mdown;
	__declspec(property(get=IsRightButtonDown)) bool rdown;
	__declspec(property(get=IsX1ButtonDown)) bool x1down;
	__declspec(property(get=IsX2ButtonDown)) bool x2down;
	__declspec(property(get=IsCtrlKeyDown)) bool ctrldown;
	__declspec(property(get=IsShiftKeyDown)) bool shiftdown;
#endif

	MouseEventArgs(UINT msg, WPARAM wparam, LPARAM lparam)
	{
		this->msg = msg;
		this->wparam = wparam;
		this->lparam = lparam;
		mousepos.x = GET_X_LPARAM(lparam);
		mousepos.y = GET_Y_LPARAM(lparam);
		wheelpos = GET_WHEEL_DELTA_WPARAM(wparam);
	}
};

class Timer
{
	friend class Engine;
private:
	UINT interval;
	TIMERELAPSED_CALLBACK cbk;
	LPVOID user;
	bool autoreset;

	UINT_PTR id()
		{return (UINT_PTR)this;}

	static Timer* FromId(UINT_PTR id)
		{return (Timer*)id;}

public:
	Timer(TIMERELAPSED_CALLBACK cbk, UINT interval, bool autoreset, LPVOID user)
	{
		this->cbk = cbk;
		this->autoreset = autoreset;
		this->user = user;
	}
};

//-----------------------------------------------------------------------------
// Name: Engine
// Desc: Main class. Responsible for starting subapplications
//-----------------------------------------------------------------------------
class Engine
{
private:
	std::list<Timer*> timers;
#ifdef _WIN32
	std::list<String> wndclasses;
	HINSTANCE instance;
#endif
	CYCLICFUNC cyclicfunc, _cyclicfunc;
	static std::vector<CUSTOMWNDPROC> customwndprocs;
	static std::vector<LPVOID> customwndprocsuser;

	static LRESULT CALLBACK EngWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK EngNonChildWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK EngChildWndProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	class Time
	{
		friend class Engine;
	private:
		bool frameratefixed;
		float invfps, timefactor;
#ifdef _WIN32
		LARGE_INTEGER freq, framestart;
#else
		timespec framestart;
#endif

		Time();

	public:
		float t, dt;

		void Configure(bool frameratefixed, float value);
		void Step();
	} time;

#ifdef _WIN32
	Engine(HINSTANCE instance);
#else
	Engine();
#endif
	~Engine();

#ifdef _WIN32
	__forceinline HINSTANCE GetHInstance() {return instance;}
#else
	__forceinline HINSTANCE GetHInstance() {return 0;}
#endif

	static bool FindDLL(const String& dllname, LPTSTR* missingdllname);
	void GetConsolePtr(SyncedPtr<Console>& execle); // Only needed when using engine from inside a shared library

#ifdef _WIN32
	Result RegisterWinClass(const String* classname = NULL, HBRUSH bgbrush = 0, bool ismdichildclass = false, HICON icon = NULL, HICON icon_small = NULL);
	Result RegisterCustomWndProc(CUSTOMWNDPROC customwndproc, LPVOID user); //ERROR: Works only from the main module (i.e. TestApp), because customwndprocs and numcustomwndprocs,
															   //		although static, can not be shared between modules!
	Result UnregisterCustomWndProc(CUSTOMWNDPROC customwndproc);
	void UnregisterWinClass(const String* classname = NULL);
	bool IsWinClassRegistered(const String* classname);
#endif

	Result Run(INITFUNC initfunc, INITFUNC postinitfunc, CYCLICFUNC cyclicfunc, QUITFUNC quitfunc);
	inline void ChangeCyclicFunc(CYCLICFUNC newcyclicfunc) {this->cyclicfunc ? this->cyclicfunc = newcyclicfunc : this->_cyclicfunc = newcyclicfunc;}
	bool Work();
	bool Wait();
	void WakeUp();
	Result CreateTimer(TIMERELAPSED_CALLBACK timercbk, UINT interval, bool autoreset, LPVOID user, Timer** tmr);
	Result ResetTimer(Timer* tmr, UINT newinterval);
	void RemoveTimer(Timer* tmr);
};

//-----------------------------------------------------------------------------
// Name: Console
// Desc: Interface for text in- and output
//-----------------------------------------------------------------------------
class Console
{
private:

public:
	enum Target
		{CT_DEBUGSTREAM, CT_STANDARDOUTPUT} target;
	Console();
	~Console();

	virtual void Print(const char* format, ...);
	virtual void Print(const String& message);
	virtual void Print(int message);
	virtual void Print(double message);
	virtual void PrintLine(const char* format, ...) {Print(format); Print("\n");}
	virtual void PrintLine(const String& message) {Print(message); Print("\n");}
	virtual void PrintLine(int message) {Print(message); Print("\n");}
	virtual void PrintLine(double message) {Print(message); Print("\n");}
	virtual void PrintD3D(const String& message) {Print(message);}
	virtual void PrintD3D(int message) {Print(message);}
	virtual void PrintD3D(double message) {Print(message);}
	virtual void Release() {delete this;}
};

void RegisterScriptableEngineClasses(const IPythonScriptEngine* pse);
void SyncEngineWithPython();

#endif
