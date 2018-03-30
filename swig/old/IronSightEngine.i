/* File : IronSightEngine.i */
%include <windows.i>
%include <typemaps.i>

// These typemaps cause parameters of type TYPE named ARGNAME to become System.IntPtr in C#.
%define %cs_marshal_intptr(TYPE, ARGNAME...)
	%typemap(ctype)  TYPE ARGNAME "void*"
	%typemap(imtype) TYPE ARGNAME "System.IntPtr"
	%typemap(cstype) TYPE ARGNAME "System.IntPtr"
	%typemap(in)     TYPE ARGNAME %{ $1 = ($1_ltype)$input; /* System.IntPtr */ %}
	%typemap(csin)   TYPE ARGNAME "$csinput"
   
	%typemap(out)    TYPE ARGNAME %{ $result = $1; %}
	%typemap(csout, excode=SWIGEXCODE) TYPE ARGNAME {
		System.IntPtr cPtr = $imcall;$excode
		return cPtr;
	}
	%typemap(csvarout, excode=SWIGEXCODE2) TYPE ARGNAME %{
		get {
				System.IntPtr cPtr = $imcall;$excode
				return cPtr;
		}
	%}

	%typemap(ctype)  TYPE& ARGNAME "void**"
	%typemap(imtype) TYPE& ARGNAME "ref System.IntPtr"
	%typemap(cstype) TYPE& ARGNAME  "ref System.IntPtr"
	%typemap(in)     TYPE& ARGNAME %{ $1 = ($1_ltype)$input; %}
	%typemap(csin)   TYPE& ARGNAME "ref $csinput"
%enddef 

%cs_marshal_intptr(HINSTANCE)
%cs_marshal_intptr(HWND)




// Alternative char * typemaps.
%pragma(csharp) imclasscode=%{
	public class SWIGStringMarshal : System.IDisposable {
		public readonly System.Runtime.InteropServices.HandleRef swigCPtr;
		public SWIGStringMarshal(string str) {
			swigCPtr = new System.Runtime.InteropServices.HandleRef(this, System.Runtime.InteropServices.Marshal.StringToHGlobalAnsi(str));
		}
		public virtual void Dispose() {
			System.Runtime.InteropServices.Marshal.FreeHGlobal(swigCPtr.Handle);
			System.GC.SuppressFinalize(this);
		}
		~SWIGStringMarshal()
		{
			Dispose();
		}
	}
%}

%typemap(csin) String, String&, FilePath, FilePath& "new $imclassname.SWIGStringMarshal($csinput).swigCPtr"
%typemap(csout, excode=SWIGEXCODE) String, String&, FilePath, FilePath& {
	string ret = System.Runtime.InteropServices.Marshal.PtrToStringAnsi($imcall);$excode
	return ret;
}
%typemap(imtype, out="System.IntPtr") String, String&, FilePath, FilePath& "System.Runtime.InteropServices.HandleRef"
%typemap(cstype) String, String&, FilePath, FilePath& "/*cstype*/ string"
%typemap(csvarin, excode=SWIGEXCODE2) String, String&, FilePath, FilePath& %{
	set {
		$imcall;$excode
	}
%}
%typemap(csvarout, excode=SWIGEXCODE2) String, String&, FilePath, FilePath& %{
	get {
		string ret = System.Runtime.InteropServices.Marshal.PtrToStringAnsi($imcall);$excode
		return ret;
	}
%}
%typemap(in) String, FilePath %{ $1 = String((char*)$input); %}
%typemap(in) String& %{ $1 = new String((char*)$input); %}
%typemap(in) FilePath& %{ $1 = new FilePath((char*)$input); %}

/*%typemap(argout) String & 
%{
   *$input = $1.ToCharStr();
%}*/

/*%typemap(throws, canthrow=1) string&
%{
	// c# string to C++ String converter:
	return $null;
%}*/



%define %cs_callback(TYPE, CSTYPE, FOO)
	%typemap(ctype) TYPE, TYPE& "void*"
	%typemap(in) TYPE  %{ $1 = ($1_type)$input; %}
	%typemap(in) TYPE& %{ $1 = ($1_type)&$input; %}
	%typemap(imtype, out="IntPtr") TYPE, TYPE& "RefDelegate"
	%typemap(cstype, out="IntPtr") TYPE, TYPE& "CSTYPE"
	%typemap(csin, pre =
		"    RefDelegate $csinputwrapper = delegate() {\n"
		"      "
		FOO
		"\n"
		"    };"
	) TYPE, TYPE& "$csinputwrapper"
%enddef
%cs_callback(INITFUNC, InitFunc, "return Result.getCPtr(initfunc()).Handle;")
%cs_callback(CYCLICFUNC, CyclicFunc, "return System.IntPtr.Zero;")
%cs_callback(QUITFUNC, QuitFunc, "return System.IntPtr.Zero;")

%pragma(csharp) imclassimports=%{
	//public delegate global::System.Runtime.InteropServices.HandleRef RefDelegate();
	public delegate System.IntPtr RefDelegate();
	public delegate Result InitFunc();
	public delegate void CyclicFunc();
	public delegate void QuitFunc();

	public delegate void RenderFunc(IOutputWindow wnd);
	internal delegate void RenderFuncWrapper(global::System.IntPtr wndptr, global::System.IntPtr userptr);

	public delegate void NoteDownFunc(byte note, byte velo);
%}


struct Result
{
private:
	void RemoveReference();

public:
	struct Details
	{
	private:
		Details() {}
	}* details;
	UINT* refcounter;

	Result();
	Result(String& msg);
	Result(String& msg, LPCTSTR file, LPCTSTR func, int line);
	~Result();

	static void PrintLogMessage(String msg, String file, String func, int line);
	static void PrintLogMessage(String msg);
	static void PrintLogMessage(const Details* details);

	bool operator==(const Result& r);
	Result& operator=(const Result& src);
	Result(const Result& copy);
	String GetLastResult(bool msgonly = false) const;
};

/*%rename(assign) SyncedPtr& SyncedPtr<>::operator=(type* src);
//%csmethodmodifiers SyncedPtr::operator= "private"
%typemap(cscode) SyncedPtr %{
	public static SyncedPtr operator=(type src)
	{
		retrun this;
	}
%}*/

struct PtrWrapper
{
	void* ptr;
	PtrWrapper(void* ptr) : ptr(ptr) {}
	operator void*() const {return ptr;}
};

//%rename(SyncedPtrForms) SyncedPtr<IForms>;
template<class type> class SyncedPtr
{
private:
	type** ptr;
	UINT* refcounter;

public:
	SyncedPtr()
	{
		ptr = (type**)new LPVOID();
		*ptr = NULL;
		refcounter = new UINT();
		*refcounter = 1;
	}
	SyncedPtr(type* src)
	{
		ptr = (type**)new LPVOID();
		*ptr = src;
		refcounter = new UINT();
		*refcounter = 1;
	}
	~SyncedPtr()
	{
		if(--(*refcounter) == 0)
		{
			delete ptr;
			delete refcounter;
		}
	}

	/*SyncedPtr& operator=(const SyncedPtr& src)
	{
		if(ptr != src.ptr)
		{
			if(--(*refcounter) == 0)
			{
				delete ptr;
				delete refcounter;
			}
			ptr = src.ptr;
			this->refcounter = src.refcounter;
			(*this->refcounter)++;
		}
		return *this;
	}
	//%rename(assign) operator=(type* src);
	SyncedPtr& operator=(type* src)
	{
		if(ptr)
		{
			*ptr = src;
			return *this;
		}
		else
		{
			ptr = (type**)new LPVOID();
			*ptr = src;
			refcounter = new UINT();
			*refcounter = 1;
			return *this;
		}
	}*/
	type* operator->()
		{return *ptr;}
	/*const type* operator->() const
		{return *ptr;}
	operator type*()
		{return *ptr;}
	operator const type*() const
		{return *ptr;}
	bool operator==(type* src) const
	{
		return *ptr == src;
	}
	bool operator!=(type* src) const
	{
		return *ptr != src;
	}*/
};
%template(EnginePtr) SyncedPtr<Engine>;
%template(Direct3D) SyncedPtr<IDirect3D>;
%template(OpenGL) SyncedPtr<IOpenGL>;
%typemap(ctype) IEntity** OUTPUT "void *"
%typemap(imtype) IEntity** OUTPUT "out System.IntPtr"
%typemap(cstype) IEntity** OUTPUT "out IEntity"
%typemap(csin)   IEntity** OUTPUT "out $csinput"
%typemap(csin, pre =
	"    System.IntPtr $csinputptr = System.IntPtr.Zero;"
	, post =
	"    $csinput = new IRegularEntity($csinputptr, false);"
) IEntity** OUTPUT "out $csinputptr"
%typemap(in) IEntity** OUTPUT %{
	// FOO
	IEntity* _$1;
	$1 = &_$1;
%}
%typemap(argout) IEntity** OUTPUT %{
	// FOO2
	*(IEntity**)$input = *$1;
%}
%typemap(cscode) IEntity %{
  public static bool operator==(IEntity ptr0, IEntity ptr1)
  {
    return ((object)ptr0 == null && (object)ptr1 == null) || ((object)ptr0 != null && (object)ptr1 != null && ptr0.swigCPtr.Handle == ptr1.swigCPtr.Handle);
  }
  public static bool operator!=(IEntity ptr0, IEntity ptr1)
  {
    return ((object)ptr0 == null) != ((object)ptr1 == null) || ((object)ptr0 != null && (object)ptr1 != null && ptr0.swigCPtr.Handle != ptr1.swigCPtr.Handle);
  }
%}
%apply IEntity** OUTPUT { IEntity** };
%template(Havok) SyncedPtr<IHavok>;
%template(GuiFactory) SyncedPtr<IGuiFactory>;
%template(Forms) SyncedPtr<IForms>;
%template(DirectIpt) SyncedPtr<IDirectIpt>;
%template(WinIpt) SyncedPtr<IWinIpt>;
%template(FFmpeg) SyncedPtr<IFFmpeg>;
%template(CryptoPP) SyncedPtr<ICryptoPP>;
%template(ID3Lib) SyncedPtr<IID3Lib>;
%template(WinAudio) SyncedPtr<IWinAudio>;
%template(XAudio2) SyncedPtr<IXAudio2>;
%template(WinMidi) SyncedPtr<IWinMidi>;
%template(WinSock) SyncedPtr<IWinSock>;
%template(AsyncWorkers) SyncedPtr<IAsyncWorkers>;
%template(SQLite) SyncedPtr<ISQLite>;
%template(HaruPdf) SyncedPtr<IHaruPdf>;
%template(RayTracer) SyncedPtr<IRayTracer>;
%template(Pbrt) SyncedPtr<IPbrt>;
%template(PythonScriptEngine) SyncedPtr<IPythonScriptEngine>;
%template(Console) SyncedPtr<Console>;




template<class type = int> struct Point
{
	type x, y;

	Point(){};
	Point(type x, type y)
	{
		this->x = x;
		this->y = y;
	}
	/*bool operator==(const Point<type>& p) const
	{
		return this->x == p.x && this->y == p.y;
	}
	bool operator!=(const Point<type>& p) const
	{
		return this->x != p.x || this->y != p.y;
	}
	bool operator<(const Point<type>& p) const
	{
		return this->y < p.y || (this->y == p.y && this->x < p.x);
	}
	Point<type> operator+(const Point<type> &summand) const
	{
		return Point<type>(x + summand.x, y + summand.y);
	}
	Point<type> operator-(const Point<type> &subtrahend) const
	{
		return Point<type>(x - subtrahend.x, y - subtrahend.y);
	}
	Point<type>& operator+=(const Point<type> &summand)
	{
		x += summand.x; y += summand.y;
		return *this;
	}
	Point<type>& operator-=(const Point<type> &subtrahend)
	{
		x -= subtrahend.x; y -= subtrahend.y;
		return *this;
	}*/

	inline String ToString() const
		{return String("Point(x = ") << x << String(", y = ") << y << String(")");}
};
%template(PointI) Point<int>;
//%template(PointU) Point<UINT>;

template<class type = int> struct Size
{
	type width, height;

	Size(){};
	Size(type width, type height)
	{
		this->width = width;
		this->height = height;
	}
	/*bool operator==(const Size<type>& s) const
	{
		return this->width == s.width && this->height == s.height;
	}
	bool operator!=(const Size<type>& s) const
	{
		return this->width != s.width || this->height != s.height;
	}
	bool operator<(const Size<type>& s) const
	{
		return this->height < s.height || (this->height == s.height && this->width < s.width);
	}*/

	inline String ToString() const
		{return String("Size(width = ") << width << String(", height = ") << height << String(")");}
};
%template(SizeI) Size<int>;
%template(SizeU) Size<UINT>;

template<class type = int> struct Rect
{
	Point<type> location;
	Size<type> size;

	Rect(){};
	Rect(type x, type y, type width, type height)
	{
		this->location.x = x;
		this->location.y = y;
		this->size.width = width;
		this->size.height = height;
	};
	Rect(const Rect<type> &rect)
	{
		this->location.x = rect.location.x;
		this->location.y = rect.location.y;
		this->size.width = rect.size.width;
		this->size.height = rect.size.height;
	};
	Rect(const Point<type> &pos, const Size<type> &size)
	{
		this->location.x = pos.x;
		this->location.y = pos.y;
		this->size.width = size.width;
		this->size.height = size.height;
	};
	Rect(const Point<type> *pos, const Size<type> *size)
	{
		this->location.x = pos->x;
		this->location.y = pos->y;
		this->size.width = size->width;
		this->size.height = size->height;
	};
	Rect(type width, type height)
	{
		this->location.x = 0;
		this->location.y = 0;
		this->size.width = width;
		this->size.height = height;
	};
	/*Rect(const LPRECT rect)
	{
		this->location.x = (type)rect->left;
		this->location.y = (type)rect->top;
		this->size.width = (type)(rect->right - rect->left);
		this->size.height = (type)(rect->bottom - rect->top);
	};
	Rect(RECT& rect)
	{
		this->location.x = rect.left;
		this->location.y = rect.top;
		this->size.width = rect.right - rect.left;
		this->size.height = rect.bottom - rect.top;
	};

	bool operator==(const Rect<type>& r) const
	{
		return this->location.x == r.location.x && this->location.y == r.location.y &&
			   this->size.width == r.size.width && this->size.height == r.size.height;
	}
	bool operator!=(const Rect<type>& r) const
	{
		return this->location.x != r.location.x || this->location.y != r.location.y ||
			   this->size.width != r.size.width || this->size.height != r.size.height;
	}
	operator RECT() const
	{
		RECT result;
		SetRect(&result, (int)location.x, (int)location.y, (int)(location.x + size.width), (int)(location.y + size.height));
		return result;
	}
	operator LPRECT() const
	{
		RECT* result = new RECT();
		SetRect(result, (int)location.x, (int)location.y, (int)(location.x + size.width), (int)(location.y + size.height));
		return result;
	}*/

	__forceinline type GetX() const {return location.x;};
	__forceinline type SetX(type val) {return location.x = val;};
	__forceinline type GetY() const {return location.y;};
	__forceinline type SetY(type val) {return location.y = val;};
	__forceinline type GetWidth() const {return size.width;};
	__forceinline type SetWidth(type val) {return size.width = val;};
	__forceinline type GetHeight() const {return size.height;};
	__forceinline type SetHeight(type val) {return size.height = val;};
#ifdef _WIN32
	__declspec(property(get=GetX, put=SetX)) type x;
	__declspec(property(get=GetY, put=SetY)) type y;
	__declspec(property(get=GetWidth, put=SetWidth)) type width;
	__declspec(property(get=GetHeight, put=SetHeight)) type height;
#endif

	__forceinline type left() const
		{return location.x;};
	__forceinline type top() const
		{return location.y;};
	__forceinline type right() const
		{return location.x + size.width;};
	__forceinline type bottom() const
		{return location.y + size.height;};

	static inline Rect<type>Empty()
		{return Rect<type>(0, 0, 0, 0);}
	inline Rect<type> Offset(type offsetx, type offsety) const
		{return Rect<type>(location.x + offsetx, location.y + offsety, size.width, size.height);}
	inline Rect<type> Offset(const Point<type>& offset) const
		{return Rect<type>(location + offset, size);}
	inline bool Contains(type x, type y) const
		{return x >= location.x && x < location.x + size.width && y >= location.y && y < location.y + size.height;}
	inline bool Contains(const Point<type>& p) const
		{return p.x >= location.x && p.x < location.x + size.width && p.y >= location.y && p.y < location.y + size.height;}
	inline bool Intersects(const Rect<type>& r) const
		{return !(location.x > r.location.x + r.size.width || r.location.x > location.x + size.width ||
				  location.y > r.location.y + r.size.height || r.location.y > location.y + size.height);}
	inline String ToString() const
		{return String("Rectangle(x = ") << location.x << String(", y = ") << location.y <<
				String(", width = ") << size.width << String(", height = ") << size.height << String(")");}
	inline void Inflate(const Size<type>& s)
	{
		location.x -= s.width / 2.0f;
		location.y -= s.height / 2.0f;
		size.width += s.width;
		size.height += s.height;
	}
	inline void Inflate(float width, float height)
	{
		location.x -= width / 2.0f;
		location.y -= height / 2.0f;
		size.width += width;
		size.height += height;
	}
};
%template(RectI) Rect<int>;




%module ISMath
%{
#include <ISEngine.h>
%}

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

/*%nodefault Engine::Time;
%immutable Engine::time;*/
class Engine
{
public:
	/*class Engine::Time
	{
	private:
		Time();

	public:
		float t, dt;

		void Configure(bool frameratefixed, float value);
		void Step();
	} time;*/

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

class Console
{
private:

public:
	Console();
	~Console();

	virtual void Print(const String& message);
	virtual void Print(int message);
	virtual void Print(double message);
	virtual void PrintLine(const String& message) {Print(message); Print("\n");}
	virtual void PrintLine(int message) {Print(message); Print("\n");}
	virtual void PrintLine(double message) {Print(message); Print("\n");}
	virtual void PrintD3D(const String& message) {Print(message);}
	virtual void PrintD3D(int message) {Print(message);}
	virtual void PrintD3D(double message) {Print(message);}
	virtual void Release() {delete this;}
};


%module ISMath
%{
#include <ISMath.h>
%}

float saturate(float val);
Vector2* Vec2Set(Vector2* vout, float x, float y);
Vector2* Vec2Set(Vector2* vout, const float* xy);
Vector3* Vec3Set(Vector3* vout, float x, float y, float z);
Vector3* Vec3Set(Vector3* vout, const float* xyz);
Vector4* Vec4Set(Vector4* vout, float x, float y, float z, float w);
Vector4* Vec4Set(Vector4* vout, const float* xyzw);
Color* ClrSet(Color* cout, float r, float g, float b, float a = 1.0f);
Color* ClrSet(Color* cout, const float* rgba);
Color3* ClrSet(Color3* cout, float r, float g, float b);
Color3* ClrSet(Color3* cout, const float* rgb);
Quaternion* QuaternionSet(Quaternion* qout, float x, float y, float z, float w);
Quaternion* QuaternionSet(Quaternion* qout, const float* xyzw);
Color* ClrClone(Color* cout, const Color* c0);
Color3* ClrClone(Color3* cout, const Color3* c0);
float Vec2Dot(const Vector2 *v0, const Vector2 *v1);
float Vec3Dot(const Vector3 *v0, const Vector3 *v1);
float Vec4Dot(const Vector4 *v0, const Vector4 *v1);
Vector3* Vec3Cross(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1);
Vector3* Vec3Reflect(Vector3 *vResult, const Vector3 *n, const Vector3 *v);
Vector3* Vec3Refract(Vector3 *vResult, const Vector3 *n, const Vector3 *v, float ratio);
Vector2* GetUV(Vector2 t0, Vector2 t1, Vector2 t2, Vector2 tP, Vector2 *uv);
float Vec2Length(const Vector2 *v0);
float Vec2Length(const float x, const float y);
float Vec3Length(const Vector3 *v0);
float Vec3Length(const float x, const float y, const float z);
float Vec4Length(const Vector4 *v0);
float Vec4Length(const float x, const float y, const float z, const float w);
float Vec2LengthSq(const Vector2 *v0);
float Vec2LengthSq(const float x, const float y);
float Vec3LengthSq(const Vector3 *v0);
float Vec3LengthSq(const float x, const float y, const float z);
float Vec4LengthSq(const Vector4 *v0);
float Vec4LengthSq(const float x, const float y, const float z, const float w);
Vector2* Vec2Inverse(Vector2* vResult, const Vector2* v0);
Vector3* Vec3Inverse(Vector3* vResult, const Vector3* v0);
Vector4* Vec4Inverse(Vector4* vResult, const Vector4* v0);
Vector2* Vec2Invert(Vector2* v0);
Vector3* Vec3Invert(Vector3* v0);
Vector4* Vec4Invert(Vector4* v0);
Vector2* Vec2Normalize(Vector2 *vResult, const Vector2 *v0);
Vector2* Vec2Normalize(Vector2 *v0);
Vector3* Vec3Normalize(Vector3 *vResult, const Vector3 *v0);
Vector3* Vec3Normalize(Vector3 *v0);
Vector4* Vec4Normalize(Vector4 *vResult, const Vector4 *v0);
Vector4* Vec4Normalize(Vector4 *v0);
Vector4* Vec3Transform(Vector4 *vResult, const Vector3 *v0, const Matrix *m0);
Vector3* Vec3TransformCoord(Vector3 *vResult, const Vector3 *v0, const Matrix *m0);
Vector3* Vec3TransformNormal(Vector3 *vResult, const Vector3 *v0, const Matrix *m0);
Vector4* Vec4Transform(Vector4 *vResult, const Vector4 *v0, const Matrix *m0);
//Vector3* Vec3Unproject(Vector3 *vResult, const Vector3* v, const Matrix* worldviewproj);
Vector2* Vec2Lerp(Vector2 *vResult, const Vector2 *v0, const Vector2 *v1, float f);
Vector3* Vec3Lerp(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1, float f);
Color* ClrLerp(Color* cResult, const Color* c0, const Color* c1, float f);
Color3* ClrLerp(Color3* cResult, const Color3* c0, const Color3* c1, float f);
Vector2* Vec2BaryCentric(Vector2* vResult, const Vector2* v0, const Vector2* v1, const Vector2* v2, const Vector2* uv);
Vector3* Vec3BaryCentric(Vector3* vResult, const Vector3* v0, const Vector3* v1, const Vector3* v2, const Vector2* uv);
Vector2* Vec2InvBaryCentric(Vector2* vResult, const Vector2* v0, const Vector2* v1, const Vector2* v2, const Vector2* uv);
Vector3* Vec3Min(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1);
Vector3* Vec3Max(Vector3 *vResult, const Vector3 *v0, const Vector3 *v1);
Quaternion* QuaternionIdentity(Quaternion *qResult);
Quaternion* QuaternionMultiply(Quaternion *qResult, const Quaternion *q0, const Quaternion *q1);
Quaternion* QuaternionMultiply(Quaternion *qResult, const Quaternion *q1);
Quaternion* QuaternionInverse(Quaternion *qResult, const Quaternion *q0);
Quaternion* QuaternionInvert(Quaternion *q0);
Quaternion* QuaternionNormalize(Quaternion *qResult, const Quaternion *q0);
Quaternion* QuaternionNormalize(Quaternion *q0);
Quaternion* QuaternionRotationX(Quaternion *qResult, float pitch);
Quaternion* QuaternionRotationY(Quaternion *qResult, float yaw);
Quaternion* QuaternionRotationZ(Quaternion *qResult, float roll);
Quaternion* QuaternionRotationAxis(Quaternion *qResult, const Vector3* axis, float angle);
void QuaternionToAxisAngle(const Quaternion *q0, Vector3* axis, float* angle);
Plane* PlaneFromPointNormal(Plane *pResult, const Vector3 *vPt, const Vector3 *vNml);
Plane* PlaneNormalize(Plane *pResult, const Plane *p0);
Plane* PlaneNormalize(Plane *pResult);
float PlaneDotCoord(const Plane *p0,const Vector3 *v0);
Vector3* PlaneIntersectLine(Vector3 *vResult, const Plane *p0, const Vector3 *v0, const Vector3 *v1);
Matrix* MatrixIdentity(Matrix *mResult);
Matrix* MatrixTranslation(Matrix *mResult, float x, float y, float z);
Matrix* MatrixTranslation(Matrix *mResult, const Vector3 *v0);
Matrix* MatrixTranslate(Matrix *mResult, const Matrix *m0, float x, float y, float z);
Matrix* MatrixTranslate(Matrix *mResult, const Matrix *m0, const Vector3 *v0);
Matrix* MatrixScaling(Matrix *mResult, float x, float y, float z);
Matrix* MatrixScaling(Matrix *mResult, const Vector3 *v0);
Matrix* MatrixScale(Matrix *mResult, const Matrix *m0, float x, float y, float z);
Matrix* MatrixScale(Matrix *mResult, const Matrix *m0, const Vector3 *v0);
Matrix* MatrixTranspose(Matrix *mResult, const Matrix *m0);
Matrix* MatrixInverseTranspose(Matrix *mResult, const Matrix *m0);
float MatrixDeterminant(const Matrix *m0);
//Matrix* MatrixInverse(Matrix *mResult, float *det, const Matrix *m0);
Matrix* MatrixInverse(Matrix *mResult, const Matrix *m0);
Matrix* MatrixPerspectiveLH(Matrix *mResult, float width, float height, float zn, float zf);
Matrix* MatrixPerspectiveRH(Matrix *mResult, float width, float height, float zn, float zf);
Matrix* MatrixPerspectiveFovLH(Matrix *mResult, float fovy, float aspect, float zn, float zf);
Matrix* MatrixPerspectiveFovRH(Matrix *mResult, float fovy, float aspect, float zn, float zf);
Matrix* MatrixOrthographicLH(Matrix *mResult, float width, float height, float zn, float zf);
Matrix* MatrixOrthographicRH(Matrix *mResult, float width, float height, float zn, float zf);
Matrix* MatrixRotationX(Matrix *mResult, float angle);
Matrix* MatrixRotateX(Matrix* mResult, const Matrix* m0, float angle);
Matrix* MatrixRotationY(Matrix *mResult, float angle);
Matrix* MatrixRotateY(Matrix* mResult, const Matrix* m0, float angle);
Matrix* MatrixRotationZ(Matrix *mResult, float angle);
Matrix* MatrixRotateZ(Matrix* mResult, const Matrix* m0, float angle);
Matrix* MatrixRotationYawPitchRoll(Matrix *mResult, float yaw, float pitch, float roll);
Matrix* MatrixRotationQuaternion(Matrix *mResult, const Quaternion* qrot);
Matrix* MatrixLookAtLH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up);
Matrix* MatrixLookAtRH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up);
Matrix* MatrixInvLookAtLH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up);
Matrix* MatrixInvLookAtRH(Matrix *mResult, const Vector3* pos, const Vector3* target, const Vector3* up);
bool RayIntersectsBox(const Vector3* raypos, const Vector3* raydir, const Vector3* boxmin, const Vector3* boxmax);

%rename(add) Vector2::operator+(const Vector2& summand) const;
%csmethodmodifiers Vector2::operator+ "private"
%typemap(cscode) Vector2 %{
	public static Vector2 operator+(Vector2 v0, Vector2 v1)
	{
		return v0.add(v1);
	}
%}

struct Vector2
{
	float x, y;
	Vector2();
	Vector2(float x, float y);
	// Binary operators
	Vector2 operator+(const Vector2& summand) const;
	/*Vector2 operator-(const Vector2& subtrahend) const;
	Vector2 operator*(const Vector2& factor) const;
	Vector2 operator*(float factor) const;
	Vector2 operator/(const Vector2& divisor) const;
	Vector2 operator/(float divisor) const;
	// Assignment operators
	Vector2& operator+=(const Vector2& summand);
	Vector2& operator+=(float summand);
	Vector2& operator-=(const Vector2& subtrahend);
	Vector2& operator-=(float subtrahend);
	Vector2& operator*=(const Vector2& factor);
	Vector2& operator*=(float factor);
	Vector2& operator/=(const Vector2& divisor);
	Vector2& operator/=(float divisor);
	// Comparison operators
	bool operator==(const Vector2& reference) const;
	bool operator!=(const Vector2& reference) const;
	bool operator<(const Vector2& reference) const;
	bool operator>(const Vector2& reference) const;
	bool operator<=(const Vector2& reference) const;
	bool operator>=(const Vector2& reference) const;*/
};

struct Vector3
{
public:
	float x, y, z;
	Vector3(){};
	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	};
	Vector3(const float* xyz)
	{
		this->x = xyz[0];
		this->y = xyz[1];
		this->z = xyz[2];
	};
	/*operator float*() {return &x;}
	operator const float* () const {return &x;}
	Vector3(const hkVector4& hkvec);
	operator hkVector4() const;
	// Binary operators
	Vector3 operator+(const Vector3& summand) const
	{
		return Vector3(x + summand.x, y + summand.y, z + summand.z);
	};
	Vector3 operator-(const Vector3& subtrahend) const
	{
		return Vector3(x - subtrahend.x, y - subtrahend.y, z - subtrahend.z);
	};
	Vector3 operator*(const Vector3& factor) const
	{
		return Vector3(x * factor.x, y * factor.y, z * factor.z);
	};
	Vector3 operator*(float factor) const
	{
		return Vector3(x * factor, y * factor, z * factor);
	};
	Vector3 operator/(const Vector3& divisor) const
	{
		return Vector3(x / divisor.x, y / divisor.y, z / divisor.z);
	};
	Vector3 operator/(float divisor) const
	{
		float invdiv = 1.0f / divisor;
		return Vector3(x * invdiv, y * invdiv, z * invdiv);
	};
	// Assignment operators
	Vector3& operator+=(const Vector3& summand)
	{
		x += summand.x; y += summand.y; z += summand.z;
		return *this;
	};
	Vector3& operator+=(float summand)
	{
		x += summand; y += summand; z += summand;
		return *this;
	};
	Vector3& operator-=(const Vector3& subtrahend)
	{
		x -= subtrahend.x; y -= subtrahend.y; z -= subtrahend.z;
		return *this;
	};
	Vector3& operator-=(float subtrahend)
	{
		x -= subtrahend; y -= subtrahend; z -= subtrahend;
		return *this;
	};
	Vector3& operator*=(const Vector3& factor)
	{
		x *= factor.x; y *= factor.y; z *= factor.z;
		return *this;
	};
	Vector3& operator*=(float factor)
	{
		x *= factor; y *= factor; z *= factor;
		return *this;
	};
	Vector3& operator/=(const Vector3& divisor)
	{
		x /= divisor.x; y /= divisor.y; z /= divisor.z;
		return *this;
	};
	Vector3& operator/=(float divisor)
	{
		float invdiv = 1.0f / divisor;
		x *= invdiv; y *= invdiv; z *= invdiv;
		return *this;
	};
	// Comparison operators
	bool operator==(const Vector3& reference) const
	{
		return x == reference.x && y == reference.y && z == reference.z;
	};
	bool operator!=(const Vector3& reference) const
	{
		return x != reference.x || y != reference.y || z != reference.z;
	};
	bool operator<(const Vector3& reference) const
	{
		return x*x + y*y + z*z < reference.x*reference.x + reference.y*reference.y +
								 reference.z*reference.z;
	};
	bool operator>(const Vector3& reference) const
	{
		return x*x + y*y + z*z > reference.x*reference.x + reference.y*reference.y +
								 reference.z*reference.z;
	};
	bool operator<=(const Vector3& reference) const
	{
		return x*x + y*y + z*z <= reference.x*reference.x + reference.y*reference.y +
								  reference.z*reference.z;
	};
	bool operator>=(const Vector3& reference) const
	{
		return x*x + y*y + z*z >= reference.x*reference.x + reference.y*reference.y +
								  reference.z*reference.z;
	};*/
};
Vector3* Vec3Add(Vector3* vout, const Vector3* v0, const Vector3* v1);
Vector3* Vec3Sub(Vector3* vout, const Vector3* v0, const Vector3* v1);
Vector3* Vec3Scale(Vector3* vout, const Vector3* vin, float scale);
Vector3* Vec3AddScaled(Vector3* vout, const Vector3* v0, const Vector3* v1, float v1scale);

struct Color
{
public:
	float r, g, b, a;
	Color(){};
	Color(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	};
	Color(const float* rgba)
	{
		this->r = rgba[0];
		this->g = rgba[1];
		this->b = rgba[2];
		this->a = rgba[3];
	};
	Color(unsigned int argb)
	{
		this->r = ((argb >> 16) & 0xFF) / 255.0f;
		this->g = ((argb >>  8) & 0xFF) / 255.0f;
		this->b = ((argb >>  0) & 0xFF) / 255.0f;
		this->a = ((argb >> 24) & 0xFF) / 255.0f;
	};
	/*operator unsigned int() const
	{
		return ((unsigned int)(saturate(r) * 255.0f)) | ((unsigned int)(saturate(g) * 255.0f) << 8) | ((unsigned int)(saturate(b) * 255.0f) << 16) | ((unsigned int)(saturate(a) * 255.0f) << 24);
	}
	operator float*() {return &r;}
	operator const float* () const {return &r;}

	unsigned int ToDWORD() const
	{
		return (unsigned int)(saturate(r) * 255.0f) | ((unsigned int)(saturate(g) * 255.0f) << 8) | ((unsigned int)(saturate(b) * 255.0f) << 16);
	}
	unsigned int ToDWORD(float blendfactor) const
	{
		blendfactor = saturate(blendfactor);
		return (unsigned int)(saturate(r) * 255.0f * blendfactor) | ((unsigned int)(saturate(g) * 255.0f * blendfactor) << 8) | ((unsigned int)(saturate(b) * 255.0f * blendfactor) << 16);
	}

	bool operator==(const Color& other) const
		{return this->r == other.r && this->g == other.g && this->b == other.b && this->a == other.a;}
	bool operator!=(const Color& other) const
		{return this->r != other.r || this->g != other.g || this->b != other.b || this->a != other.a;}*/
};
Color* ClrAdd(Color* cout, const Color* c0, const Color* c1);
Color* ClrSub(Color* cout, const Color* c0, const Color* c1);
Color* ClrMul(Color* cout, const Color* c0, const Color* c1);
Color* ClrScale(Color* cout, const Color* cin, float scale);
Color* ClrAddScaled(Color* cout, const Color* c0, const Color* c1, float c1scale);
Color* ClrAddScaled(Color* cout, const Color* c0, const Color* c1, const Color* c1scale0, float c1scale1);
Color* ClrAddScaled(Color* cout, const Color* c0, const Color* c1, const Color* c1scale0, const Color* c1scale1, float c1scale2);

%rename(mul) Quaternion::operator*(const Quaternion& factor) const;
%csmethodmodifiers Quaternion::operator* "private"
%typemap(cscode) Quaternion %{
	public static Quaternion operator*(Quaternion q0, Quaternion q1)
	{
		return q0.mul(q1);
	}
%}

struct Quaternion
{
public:
	float x, y, z, w;
	Quaternion(){};
	Quaternion(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	};
	Quaternion(const float* xyzw)
	{
		this->x = xyzw[0];
		this->y = xyzw[1];
		this->z = xyzw[2];
		this->w = xyzw[3];
	};
	/*operator float*() {return &x;}
	operator const float* () const {return &x;}
	Quaternion(const hkQuaternion& hkquat);
	operator hkQuaternion() const;
	// Binary operators
	Quaternion operator+(const Quaternion& summand) const
	{
		return Quaternion(x + summand.x, y + summand.y, z + summand.z, w + summand.w);
	};
	Quaternion operator-(const Quaternion& subtrahend) const
	{
		return Quaternion(x - subtrahend.x, y - subtrahend.y, z - subtrahend.z, w - subtrahend.w);
	};*/
	Quaternion operator*(const Quaternion& factor) const
	{
		Quaternion qResult;
		QuaternionMultiply(&qResult, this, &factor);
		return qResult;
	};
	/*Quaternion operator*(float factor) const
	{
		return Quaternion(x * factor, y * factor, z * factor, w * factor);
	};
	Quaternion operator/(float divisor) const
	{
		float invdiv = 1.0f / divisor;
		return Quaternion(x * invdiv, y * invdiv, z * invdiv, w * invdiv);
	};
	// Assignment operators
	Quaternion& operator+=(const Quaternion& summand)
	{
		x += summand.x; y += summand.y; z += summand.z; w += summand.w;
		return *this;
	};
	Quaternion& operator-=(const Quaternion& subtrahend)
	{
		x -= subtrahend.x; y -= subtrahend.y; z -= subtrahend.z; w -= subtrahend.w;
		return *this;
	};
	Quaternion& operator*=(const Quaternion& factor)
	{
		QuaternionMultiply(this, &factor);
		return *this;
	};
	Quaternion& operator*=(float factor)
	{
		x *= factor; y *= factor; z *= factor; w *= factor;
		return *this;
	};
	Quaternion& operator/=(float divisor)
	{
		float invdiv = 1.0f / divisor;
		x *= invdiv; y *= invdiv; z *= invdiv; w *= invdiv;
		return *this;
	};
	// Comparison operators
	bool operator==(const Quaternion& reference) const
	{
		return x == reference.x && y == reference.y && z == reference.z && w == reference.w;
	};
	bool operator!=(const Quaternion& reference) const
	{
		return x != reference.x || y != reference.y || z != reference.z || w != reference.w;
	};*/

	Vector3* ComputeAxisWrapper() const;
	float ComputeAngleWrapper() const;
};

struct Matrix
{
public:
	float _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44;
	Matrix(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24,
		   float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44)
	{
		this->_11 = _11; this->_12 = _12; this->_13 = _13; this->_14 = _14;
		this->_21 = _21; this->_22 = _22; this->_23 = _23; this->_24 = _24;
		this->_31 = _31; this->_32 = _32; this->_33 = _33; this->_34 = _34;
		this->_41 = _41; this->_42 = _42; this->_43 = _43; this->_44 = _44;
	};
	Matrix(){};
	/*operator float*() {return &_11;}
	operator const float* () const {return &_11;}
	Matrix operator+(const Matrix &summand) const
	{
		return Matrix(_11 + summand._11, _12 + summand._12, _13 + summand._13, _14 + summand._14,
					  _21 + summand._21, _22 + summand._22, _23 + summand._23, _24 + summand._24,
					  _31 + summand._31, _32 + summand._32, _33 + summand._33, _34 + summand._34,
					  _41 + summand._41, _42 + summand._42, _43 + summand._43, _44 + summand._44);
	};
	Matrix operator-(const Matrix &subtrahend) const
	{
		return Matrix(_11 - subtrahend._11, _12 - subtrahend._12, _13 - subtrahend._13, _14 - subtrahend._14,
					  _21 - subtrahend._21, _22 - subtrahend._22, _23 - subtrahend._23, _24 - subtrahend._24,
					  _31 - subtrahend._31, _32 - subtrahend._32, _33 - subtrahend._33, _34 - subtrahend._34,
					  _41 - subtrahend._41, _42 - subtrahend._42, _43 - subtrahend._43, _44 - subtrahend._44);
	};
	Matrix operator*(const Matrix &factor) const
	{
		return Matrix(_11 * factor._11 + _12 * factor._21 + _13 * factor._31 + _14 * factor._41,
					  _11 * factor._12 + _12 * factor._22 + _13 * factor._32 + _14 * factor._42,
					  _11 * factor._13 + _12 * factor._23 + _13 * factor._33 + _14 * factor._43,
					  _11 * factor._14 + _12 * factor._24 + _13 * factor._34 + _14 * factor._44,
					  _21 * factor._11 + _22 * factor._21 + _23 * factor._31 + _24 * factor._41,
					  _21 * factor._12 + _22 * factor._22 + _23 * factor._32 + _24 * factor._42,
					  _21 * factor._13 + _22 * factor._23 + _23 * factor._33 + _24 * factor._43,
					  _21 * factor._14 + _22 * factor._24 + _23 * factor._34 + _24 * factor._44,
					  _31 * factor._11 + _32 * factor._21 + _33 * factor._31 + _34 * factor._41,
					  _31 * factor._12 + _32 * factor._22 + _33 * factor._32 + _34 * factor._42,
					  _31 * factor._13 + _32 * factor._23 + _33 * factor._33 + _34 * factor._43,
					  _31 * factor._14 + _32 * factor._24 + _33 * factor._34 + _34 * factor._44,
					  _41 * factor._11 + _42 * factor._21 + _43 * factor._31 + _44 * factor._41,
					  _41 * factor._12 + _42 * factor._22 + _43 * factor._32 + _44 * factor._42,
					  _41 * factor._13 + _42 * factor._23 + _43 * factor._33 + _44 * factor._43,
					  _41 * factor._14 + _42 * factor._24 + _43 * factor._34 + _44 * factor._44);
	};
	Vector4 operator*(const Vector4 &factor) const
	{
		return Vector4(_11 * factor.x + _12 * factor.y + _13 * factor.z + _14 * factor.w,
					   _21 * factor.x + _22 * factor.y + _23 * factor.z + _24 * factor.w,
					   _31 * factor.x + _32 * factor.y + _33 * factor.z + _34 * factor.w,
					   _41 * factor.x + _42 * factor.y + _43 * factor.z + _44 * factor.w);
	};*/
};





%module ISDirect3D
%{
#include <ISDirect3D.h>
%}

//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum RenderType
	{RT_BACKGROUND, RT_DEFAULT, RT_FAST, RT_FOREGROUND};
enum OrientationMode
	{OM_EULER, OM_QUAT, OM_MATRIX};
enum D3dShapeId
{
	DSID_PLANE, DSID_BOX, DSID_SPHERE, DSID_CAPSULE, DSID_CYLINDER, DSID_CONE
};

//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct D3dStartupSettings
{
	bool isfullscreen;
};
struct D3DMaterial
{
	Color ambient, diffuse, specular;
	float power;
	ITexture* tex;
	bool operator==(const D3DMaterial& other) const
		{return this->tex == other.tex && this->power == other.power && this->ambient == other.ambient && this->diffuse == other.diffuse && this->specular == other.specular;}
};
%include "std_vector.i"
%template(D3DMaterialVector) std::vector<D3DMaterial>;
struct Sprite
{
	Matrix worldmatrix;
	Vector2 texcoord;
	Vector2 texsize;
	Color color;
	ITexture* tex;
	Sprite() : texcoord(0.0f, 0.0f), texsize(1.0f, 1.0f), color(1.0f, 1.0f, 1.0f, 1.0f)
	{
		MatrixIdentity(&worldmatrix);
	}
};

struct D3dShapeDesc
{
protected:
	const D3dShapeId id;
	D3dShapeDesc(D3dShapeId id) : id(id), texcoordscale(1.0f, 1.0f) {}
public:
	Vector2 texcoordscale;
	D3dShapeId GetId() const {return id;}
	virtual UINT GetVertexCount() const = 0;
	virtual UINT GetFaceCount() const = 0;
};
struct D3dPlaneShapeDesc : public D3dShapeDesc
{
	D3dPlaneShapeDesc() : D3dShapeDesc(DSID_PLANE) {}
	Vector3 pos, nml;
	Vector2 size;

	UINT GetVertexCount() const {return 4;}
	UINT GetFaceCount() const {return 2;}
};
struct D3dBoxShapeDesc : public D3dShapeDesc
{
	D3dBoxShapeDesc() : D3dShapeDesc(DSID_BOX) {}
	Vector3 pos, size;

	UINT GetVertexCount() const {return 24;}
	UINT GetFaceCount() const {return 12;}
};
struct D3dSphereShapeDesc : public D3dShapeDesc
{
	D3dSphereShapeDesc() : D3dShapeDesc(DSID_SPHERE), pos(0.0f, 0.0f, 0.0f), rot_y(0.0f), theta(PI), phi(2.0f * PI) {}
	Vector3 pos;
	float rot_y, radius;
	UINT stacks, slices;
	float theta, phi;

	UINT GetVertexCount() const {return (slices + (UINT)(phi != 2.0f * PI)) * (stacks + 1);}
	UINT GetFaceCount() const {return slices * stacks * 2;}
};
struct D3dCapsuleShapeDesc : public D3dShapeDesc
{
	D3dCapsuleShapeDesc() : D3dShapeDesc(DSID_CAPSULE) {}
	Vector3 v0, v1;
	float radius;
	UINT stacks, slices;

	UINT GetVertexCount() const {return slices * (stacks + 2);}
	UINT GetFaceCount() const {return slices * (stacks + 1) * 2;}
};
struct D3dCylinderShapeDesc : public D3dShapeDesc
{
	D3dCylinderShapeDesc() : D3dShapeDesc(DSID_CYLINDER), hasfrontfaces(true) {}
	Vector3 v0, v1;
	float radius;
	UINT slices;
	bool hasfrontfaces;

	UINT GetVertexCount() const {return (hasfrontfaces ? 4 : 2) * slices;}
	UINT GetFaceCount() const {return 2 * slices + (hasfrontfaces ? 2 * (slices - 2) : 0);}
};
struct D3dConeShapeDesc : public D3dShapeDesc
{
	D3dConeShapeDesc() : D3dShapeDesc(DSID_CONE), hasgroundfaces(true) {}
	Vector3 v0, v1;
	float radius;
	UINT slices;
	bool hasgroundfaces;

	UINT GetVertexCount() const {return (hasgroundfaces ? 3 : 2) * slices;}
	UINT GetFaceCount() const {return 2 * slices + (hasgroundfaces ? slices - 2 : 0);}
};

//-----------------------------------------------------------------------------
// Name: IMoveable
// Desc: Interface to a class that can be moved by passing a transformation matrix
//-----------------------------------------------------------------------------
class IMoveable
{
public:
	virtual void SetTransform(const Matrix* transform) {}
};

//-----------------------------------------------------------------------------
// Name: IRenderable
// Desc: Interface to a class with render function
//-----------------------------------------------------------------------------
class IRenderable : public IMoveable
{
public:
	bool autorenderenabled;

	IRenderable()
	{
		autorenderenabled = true;
	};
	virtual void Render(RenderType rendertype) = 0;
	virtual bool GetRefPos(Vector3& pos) const {return false;}
	virtual Result GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype) {return cbk(this, cbkuser);};
	virtual LPOBJECT GetObject();
};

//-----------------------------------------------------------------------------
// Name: IUpdateable
// Desc: Interface to a class with update function
//-----------------------------------------------------------------------------
class IUpdateable
{
public:
	virtual void Update() = 0;
};

typedef class ITexture
{
public:
	enum Usage
		{TU_SKIN, TU_SKIN_NOMIPMAP, TU_SOURCE, TU_RENDERTARGET, TU_RENDERTARGETORSOURCE, TU_ALLACCESS, TU_CPUWRITE};

	virtual UINT GetWidth() = 0;
	virtual UINT GetHeight() = 0;
	virtual UINT GetNumMipLevels() = 0;
	virtual FilePath GetFilename() = 0;
	virtual Result Lock(D3D11_MAPPED_SUBRESOURCE* data) = 0;
	virtual void Unlock() = 0;
	virtual Result MapPixelData(bool read, bool write, UINT* stride, LPVOID* data) = 0;
	virtual void UnmapData() = 0;
	virtual Result Save(const FilePath& filename, D3DX_IMAGE_FILEFORMAT format) = 0;
	virtual Result Save(LPD3D10BLOB* datablob, D3DX_IMAGE_FILEFORMAT format) = 0;
	virtual ID3D11Texture2D* GetD3DTexture() = 0;
	virtual void Release() = 0;
}* LPTEXTURE;

typedef class IRenderShader
{
public:
	virtual LPD3D11EFFECTVARIABLE GetVariableBySemantic(LPCSTR semantic) = 0;
	virtual LPD3D11EFFECTVARIABLE GetVariableByName(LPCSTR name) = 0;
	virtual IEffectScalarVariable* GetScalarVariable(const String& semantic) = 0;
	virtual IEffectVectorVariable* GetVectorVariable(const String& semantic) = 0;
	virtual IEffectMatrixVariable* GetMatrixVariable(const String& semantic) = 0;
	virtual Result SetTechnique(UINT techidx) = 0;
}* LPRENDERSHADER;

typedef class IObject : public IRenderable
{
public:
	struct Vertex
	{
		Vector3 pos, nml;
		Vector2 texcoord;
		UINT blendidx;
		bool operator==(const Vertex& other) const
			{return this->pos == other.pos && this->nml == other.nml && this->texcoord == other.texcoord && this->blendidx == other.blendidx;}
	};
	struct AttributeRange
	{
		UINT attrid;
		UINT facestart, facecount;
		UINT vertexstart, vertexcount;
	};
	struct BoundingSphere
	{
		Vector3 center;
		float radius;
	};
	struct BoundingBox
	{
		Vector3 center, size;
	};
	OrientationMode orientmode;
	Matrix transform;
	std::vector<Matrix> bonetransform;
	Vector3 pos, rot, scl;
	Quaternion qrot;
	bool iscameradependent;

	virtual UINT GetNumBones() = 0;
	std::vector<D3DMaterial>* mats;

	virtual float RayTrace(const Vector3* pos, const Vector3* dir, Vector3* normal = NULL, UINT* subset = NULL, UINT* blendpart = NULL) = 0;
	virtual Result CreateCollisionMesh(LPREGULARENTITY* entity) = 0;
	virtual Result CreateConvexCollisionHull(LPREGULARENTITY* entity) = 0;
	virtual Result CreateCollisionCapsule(float radius, LPREGULARENTITY* entity) = 0;
	virtual Result CreateCollisionRagdoll(float radius, bool usedynamicmotion, LPRAGDOLLENTITY* entity) = 0;
	virtual Result SetShader(IRenderShader* shader) = 0;
	virtual IRenderShader* GetShader() const = 0;
	virtual Result SetVertexData(const Vertex* vertices) = 0;
	virtual Result SetIndexData(const UINT* indices) = 0;
	virtual UINT GetVertexCount() = 0;
	virtual UINT GetFaceCount() = 0;
	virtual UINT GetIndexCount() = 0;
	virtual Result MapVertexData(Vertex** vertices) = 0;
	virtual Result MapIndexData(UINT** indices) = 0;
	virtual Result UnmapData(LPVOID vertices) = 0;
	virtual void GetAttributeTable(AttributeRange* attributetable, UINT* attributetablesize) = 0;
	virtual Result SetAttributeTable(const AttributeRange* attributetable, UINT attributetablesize) = 0;
	virtual Result CommitChanges() = 0;
	virtual Result ComputeNormals() = 0;
	virtual Result Split(const Plane* splitplane, IObject** first, IObject** second) = 0;
	virtual Result Break(int targetnumparts, IObject** parts, int* numparts) = 0;
	virtual const BoundingSphere& GetBoundingSphere() const = 0;
	virtual const Vector3& GetBoundingSphereCenter() const = 0;
	virtual float GetBoundingSphereRadius() = 0;
	virtual const BoundingBox& GetBoundingBox() const = 0;
	virtual const Vector3& GetBoundingBoxCenter() const = 0;
	virtual const Vector3& GetBoundingBoxSize() = 0;
	virtual void SetBeforeRenderCallback(RENDER_CALLBACK cbk, LPVOID user) = 0;
}* LPOBJECT;

typedef class ICamera : public IRenderable//, public IUpdateable
{
public:
	enum ViewMode
		{VM_DEFAULT, VM_FLY, VM_FIRSTPERSON, VM_THIRDPERSON} viewmode;
	OrientationMode orientmode;
	Matrix transform;
	Vector3 pos, rot;
	Quaternion qrot;

	virtual void UpdateMatrices() = 0;
	virtual void SetClipNear(float clipnear) = 0;
	virtual void SetClipFar(float clipfar) = 0;
	virtual void SetPlayerModel(LPOBJECT obj, LPRAGDOLLENTITY ragdoll = NULL) = 0;
	virtual void SetPerspectiveProjection(float fov) = 0;
	virtual void SetOrthogonalProjection(float width, float height) = 0;
	virtual Frustum& GetViewFrustum() = 0;
	virtual Matrix& GetViewMatrix() = 0;
	virtual Matrix& GetProjMatrix() = 0;
	virtual Matrix& GetViewProjMatrix() = 0;
	virtual Vector3* ComputeViewDir(Vector3* vdir) const = 0;
	virtual Vector3* ComputePixelDir(const Point<int>& pixelpos, Vector3* vdir) const = 0;
}* LPCAMERA;

typedef class ILight : public IMoveable
{
public:
	enum LightType
		{LT_DIRECTIONAL, LT_POINT, LT_SPOT};

	virtual const Vector3& GetPos() = 0;
	virtual void SetPos(const Vector3& val) = 0;
	__declspec(property(get=GetPos, put=SetPos)) const Vector3& Pos;
	virtual const Vector3& GetDir() = 0;
	virtual void SetDir(const Vector3& val) = 0;
	__declspec(property(get=GetDir, put=SetDir)) const Vector3& Dir;
	virtual const PARENT_NAMESPACE::Color& GetColor() = 0;
	virtual void SetColor(const PARENT_NAMESPACE::Color& val) = 0;
	__declspec(property(get=GetColor, put=SetColor)) const PARENT_NAMESPACE::Color& Color;
	virtual LightType GetType() = 0;
	virtual void SetType(LightType val) = 0;
	__declspec(property(get=GetType, put=SetType)) LightType Type;
	virtual const Vector3& GetAttenuation() = 0;
	virtual void SetAttenuation(const Vector3& val) = 0;
	__declspec(property(get=GetAttenuation, put=SetAttenuation)) const Vector3& Attenuation;
	virtual float GetSpotPower() = 0;
	virtual void SetSpotPower(float val) = 0;
	__declspec(property(get=GetSpotPower, put=SetSpotPower)) float SpotPower;

	virtual bool GetEnabled() = 0;
	virtual void SetEnabled(bool val) = 0;
	__declspec(property(get=GetEnabled, put=SetEnabled)) bool Enabled;

	virtual void Release() = 0;
}* LPLIGHT;

typedef class ISpriteContainer : public IRenderable
{
public:
	virtual void SetViewTransform(const Matrix* viewmatrix) = 0;
	virtual void SetIdentityViewTransform() = 0;
	virtual void SetCameraViewTransform() = 0;
	virtual void SetProjectionTransform(const Matrix* projmatrix) = 0;
	virtual void SetIdentityProjectionTransform() = 0;
	virtual void SetCameraProjectionTransform() = 0;

	virtual void Begin() = 0;
	virtual void DrawSpriteImmediate(const Sprite* sprite) = 0;
	virtual void End() = 0;

	virtual void DrawSpriteDelayed(const Sprite* sprite) = 0;

	virtual void Release() = 0;
}* LPSPRITECONTAINER;

typedef struct IElement
{
	bool visible;
	Color color;
	const int zorder;
	const IHUD::ElementType type;
	LPVOID user;

	virtual int GetX() = 0;
	virtual int SetX(int val) = 0;
	__declspec(property(get=GetX, put=SetX)) int x;
	virtual int GetY() = 0;
	virtual int SetY(int val) = 0;
	__declspec(property(get=GetY, put=SetY)) int y;
	virtual DockType GetDock() = 0;
	virtual DockType SetDock(DockType val) = 0;
	__declspec(property(get=GetDock, put=SetDock)) DockType dock;

	IElement(int zorder, IHUD::ElementType type) : zorder(zorder), type(type) {}
	struct CompareFunctor
	{
		bool operator()(const IElement* left, const IElement* right) const
		{return left->zorder < right->zorder;}
	};
}* LPELEMENT;
typedef struct IImageElement : public IElement
{
	IImageElement(int zorder) : IElement(zorder, ET_IMAGE) {}
}* LPIMAGEELEMENT;
typedef struct ITextElement : public IElement
{
	String text;
	LPD3DFONT font;
	UINT textflags;

	ITextElement(int zorder) : IElement(zorder, ET_TEXT) {}
}* LPTEXTELEMENT;
%{
typedef IHUD::IElement IElement;
typedef IHUD::LPELEMENT LPELEMENT;
typedef IHUD::IImageElement IImageElement;
typedef IHUD::LPIMAGEELEMENT LPIMAGEELEMENT;
typedef IHUD::ITextElement ITextElement;
typedef IHUD::LPTEXTELEMENT LPTEXTELEMENT;
typedef IHUD::ElementType ElementType;
%}
typedef class IHUD : public IRenderable
{
public:
	enum ElementType {ET_IMAGE, ET_TEXT};

	typedef void (__stdcall* MOUSEEVENT_CALLBACK)(LPELEMENT element);

	virtual Result CreateElement(ITexture* tex, int x, int y, DockType dock, Color color, int zorder = 0, LPIMAGEELEMENT* element = NULL, LPVOID elementuser = NULL) = 0;
	virtual Result CreateElement(ITexture* tex, int x, int y, DockType dock, Color color, const Rect<int>& subregion, int zorder = 0, LPIMAGEELEMENT* element = NULL, LPVOID elementuser = NULL) = 0;
	virtual Result CreateElement(String text, LPD3DFONT font, int x, int y, DockType dock, Color color, const Size<int>& size,
								 int zorder = 0, UINT textflags = 0, LPTEXTELEMENT* element = NULL, LPVOID elementuser = NULL) = 0;
	virtual void RemoveElement(LPELEMENT element) = 0;
	virtual void RegisterMouseDownListener(IElement* element, MOUSEEVENT_CALLBACK cbk) = 0;
	virtual void RegisterMouseUpListener(IElement* element, MOUSEEVENT_CALLBACK cbk) = 0;
	virtual void RegisterMouseMoveListener(IElement* element, MOUSEEVENT_CALLBACK cbk) = 0;
	virtual void RegisterMouseEnterListener(IElement* element, MOUSEEVENT_CALLBACK cbk) = 0;
	virtual void RegisterMouseLeaveListener(IElement* element, MOUSEEVENT_CALLBACK cbk) = 0;
	virtual void Update() = 0;
	virtual LPSPRITECONTAINER GetSpriteContainer() = 0;
	virtual void Release() = 0;
}* LPHUD;

%extend IHUD {
	IImageElement* XCreateElement(ITexture* tex, int x, int y, DockType dock, Color color, int zorder = 0) {
		// CreateElement Wrapper
		IImageElement* ret;
		$self->CreateElement(tex, x, y, dock, color, zorder, &ret);
		return ret;
	}
	IImageElement* XCreateElement(ITexture* tex, int x, int y, DockType dock, Color color, const Rect<int>& subregion, int zorder = 0) {
		// CreateElement Wrapper
		IImageElement* ret;
		$self->CreateElement(tex, x, y, dock, color, subregion, zorder, &ret);
		return ret;
	}
};

%typemap(ctype) RENDERFUNC, RENDERFUNC& "void*"
%typemap(in) RENDERFUNC  %{ $1 = ($1_type)$input; %}
%typemap(in) RENDERFUNC& %{ $1 = ($1_type)&$input; %}
%typemap(imtype, out="IntPtr") RENDERFUNC, RENDERFUNC& "global::System.IntPtr"
%typemap(cstype, out="IntPtr") RENDERFUNC, RENDERFUNC& "RenderFunc"
%typemap(csin) RENDERFUNC, RENDERFUNC& "$csinput"
%typemap(csin, pre =
	"    RenderFuncWrapper $csinputwrapper = delegate(global::System.IntPtr wndptr, global::System.IntPtr userptr) {\n"
	"      $csinput(new IOutputWindow(wndptr, false));\n"
	"    };"
) RENDERFUNC, RENDERFUNC& "global::System.Runtime.InteropServices.Marshal.GetFunctionPointerForDelegate($csinputwrapper)"

typedef class IOutputWindow
{
public:
	struct Settings
	{
		Size<UINT> backbuffersize;
		HWND wnd;
		bool enablemultisampling;
		int screenidx;
	};
	const bool ismultisampled;
	const int screenidx;

	Color backcolor;
	bool backcolorenabled;

	virtual Size<UINT> GetBackbufferSize() = 0;
	virtual UINT GetBackbufferWidth() = 0;
	virtual UINT GetBackbufferHeight() = 0;
	virtual HWND GetHwnd() = 0;
	virtual ITexture* GetBackbuffer() = 0;
	virtual ITexture* SetBackbuffer(ITexture* texbackbuffer) = 0;
	__declspec(property(get=GetBackbuffer, put=SetBackbuffer)) ITexture* Backbuffer;
	virtual Result Resize(UINT width, UINT height) = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void RegisterForRendering(RENDERFUNC func, RenderType rendertype, LPVOID user = NULL) = 0;
	virtual void RegisterForRendering(IRenderable* cls, RenderType rendertype) = 0;
	virtual void DeregisterFromRendering(RENDERFUNC func, RenderType rendertype) = 0;
	virtual void DeregisterFromRendering(IRenderable* cls, RenderType rendertype) = 0;
	virtual void DeregisterFromRendering(IRenderable* cls) = 0;
	virtual void RegisterForUpdating(IUpdateable* cls) = 0;
	virtual void DeregisterFromUpdating(IUpdateable* cls) = 0;
	virtual void SortRegisteredClasses(RenderType rendertype, const Vector3& campos) = 0;
	virtual Result GetRegisteredClasses(GETREGISTEREDCLASSES_CALLBACK cbk, LPVOID cbkuser, RenderType rendertype) = 0;
	virtual void DrawLine(const Vector3& v0, const Vector3& v1, Color c0, Color c1, RenderType rendertype = RT_DEFAULT) = 0;
	virtual void DrawLine(const Vector2& v0, const Vector2& v1, Color c0, Color c1, RenderType rendertype = RT_DEFAULT) = 0;
	virtual void DrawSprite(const Sprite* sprite) = 0;
	virtual Result EnableSprites(const FilePath spriteshaderfilename) = 0;
	virtual Result EnableLineDrawing(const FilePath lineshaderfilename) = 0;
	virtual void ShowFps() = 0;
	virtual void HideFps() = 0;
	virtual void TakeScreenShot(const FilePath& filename, D3DX_IMAGE_FILEFORMAT fileformat) = 0;
	virtual void SetCamera(ICamera* cam) = 0;
	virtual ICamera* GetCamera() const = 0;
	virtual D3D11_VIEWPORT& GetViewport() = 0;
	virtual ID3D11Device* GetDevice() const = 0;
	virtual ID3D11DeviceContext* GetDeviceContext() const = 0;
	virtual Result MakeConsoleTarget(UINT numlines, UINT historysize) = 0;
	virtual Result CreateTexture(const FilePath& filename, ITexture::Usage usage, bool deviceindependent, ITexture** tex) = 0;
	virtual Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, ITexture** tex) = 0;
	virtual Result CreateTexture(UINT width, UINT height, ITexture::Usage usage, bool deviceindependent, DXGI_FORMAT format, LPVOID data, ITexture** tex) = 0;
	virtual Result CreateTexture(ITexture* source, ITexture::Usage usage, bool deviceindependent, ITexture** tex) = 0;
	virtual Result CreateRenderShader(const FilePath filename, IRenderShader** shader) = 0;
	virtual Result CreateObject(const FilePath& filename, bool loadtextures, IObject** obj) = 0;
	virtual Result CreateObject(IObject* srcobj, bool newmaterials, IObject** obj) = 0;
	virtual Result CreateObject(UINT numvertices, UINT numfaces, IObject** obj) = 0;
	virtual Result CreateObject(const D3dShapeDesc& shapedesc, IObject** obj) = 0;
	virtual Result CreateObject(const D3dShapeDesc* shapedescs[], UINT numshapedescs, IObject** obj) = 0;
	virtual void RemoveObject(IObject* obj) = 0;
	virtual void RemoveAllObjects() = 0;
	virtual Result CreateHUD(IHUD** hud) = 0;
	virtual Result CreateCursor(ID3dCursor** cursor) = 0;
	virtual Result CreateSpriteContainer(UINT buffersize, ISpriteContainer** spc) = 0;
	virtual Result CreateD3dFont(const FontType& type, ID3dFont** font) = 0;
	virtual Result CreateSceneManager(ISceneManager** mgr) = 0;
	virtual Result CreateSkyBox(ISkyBox** sky) = 0;
	virtual Result CreateBoxedLevel(const UINT (&chunksize)[3], UINT numchunkbuffers, LPTEXTURE texboxes, LPRENDERSHADER blevelshader, IBoxedLevel** blevel) = 0;
/*	virtual Result CreateLandscape(FilePath& filename, LPRENDERSHADER shader, ILandscape** landscape) = 0;*/ //EDIT11
	virtual void Release() = 0;

	IOutputWindow(const Settings& wndsettings) : ismultisampled(wndsettings.enablemultisampling), screenidx(wndsettings.screenidx) {}
}* LPOUTPUTWINDOW;

%extend IOutputWindow {
	void RegisterForUpdating(ICamera* cam) {
		$self->RegisterForUpdating(cam);
	}
	ITexture* XCreateTexture(const FilePath& filename, ITexture::Usage usage, bool deviceindependent) {
		// CreateTexture Wrapper
		ITexture* ret;
		$self->CreateTexture(filename, usage, deviceindependent, &ret);
		return ret;
	}
	IRenderShader* XCreateRenderShader(const FilePath& filename) {
		// CreateRenderShader Wrapper
		IRenderShader* ret;
		$self->CreateRenderShader(filename, &ret);
		return ret;
	}
	IObject* XCreateObject(const FilePath& filename, bool loadtextures) {
		// CreateObject Wrapper
		IObject* ret;
		$self->CreateObject(filename, loadtextures, &ret);
		return ret;
	}
	IObject* XCreateObject(IObject* srcobj, bool newmaterials) {
		// CreateObject Wrapper
		IObject* ret;
		$self->CreateObject(srcobj, newmaterials, &ret);
		return ret;
	}
	IObject* XCreateObject(const D3dShapeDesc& shapedesc) {
		// CreateObject Wrapper
		IObject* ret;
		$self->CreateObject(shapedesc, &ret);
		return ret;
	}
	IObject* XCreateObject(const D3dShapeDesc* shapedescs[], UINT numshapedescs) {
		// CreateObject Wrapper
		IObject* ret;
		$self->CreateObject(shapedescs, numshapedescs, &ret);
		return ret;
	}
	IHUD* XCreateHUD() {
		// CreateHUD Wrapper
		IHUD* ret;
		$self->CreateHUD(&ret);
		return ret;
	}
	ISpriteContainer* XCreateSpriteContainer(UINT buffersize) {
		// CreateSpriteContainer Wrapper
		ISpriteContainer* ret;
		$self->CreateSpriteContainer(buffersize, &ret);
		return ret;
	}
};

typedef class IDirect3D
{
public:
	virtual bool IsFullscreen() const = 0;

	virtual void Sync(SyncedPtr<Engine>& exeeng, SyncedPtr<IDirect3D>& exed3d, SyncedPtr<IOpenGL>& exeogl, SyncedPtr<IHavok>& exehvk, SyncedPtr<IGuiFactory>& exegui, \
					  SyncedPtr<IForms>& exefms, SyncedPtr<IDirectIpt>& exedip, SyncedPtr<IWinIpt>& exewip, SyncedPtr<IFFmpeg>& exeffm, SyncedPtr<ICryptoPP>& execpp, \
					  SyncedPtr<IID3Lib>& exeid3, SyncedPtr<IWinAudio>& exewad, SyncedPtr<IXAudio2>& exexa2, SyncedPtr<IWinMidi>& exemid, SyncedPtr<IWinSock>& exewsk, \
					  SyncedPtr<IAsyncWorkers>& exeaws, SyncedPtr<ISQLite>& exesql, SyncedPtr<IHaruPdf>& exepdf, SyncedPtr<IRayTracer>& exertr, SyncedPtr<IPbrt>& exepbrt, \
					  SyncedPtr<IPythonScriptEngine>& exepse, SyncedPtr<Console>& execle) = 0;
	virtual Result Init(const D3dStartupSettings* settings) = 0;
	virtual void Render() = 0;
	virtual bool CastRayTo(const Vector3& src, const Vector3& dest, float* dist) = 0;
	virtual bool CastRayDir(const Vector3& src, const Vector3& dir, float* dist) = 0;
	virtual void TakeScreenShots(const FilePath& filename, D3DX_IMAGE_FILEFORMAT fileformat) = 0;
	virtual void GetEnabledLights(std::list<ILight*>* lights) = 0;
	virtual void RemoveAllLights() = 0;
	virtual const std::list<IOutputWindow*>& GetOutputWindows() const = 0;
	virtual Result CreateOutputWindow(IOutputWindow::Settings* settings, IOutputWindow** wnd) = 0;
	virtual Result CreateWindowlessDevice(IWindowlessDevice** wldev) = 0;
	virtual Result CreateCamera(float fov, float clipnear, float clipfar, ICamera** cam) = 0;
	virtual Result CreateLight(ILight::LightType type, ILight** light) = 0;
	virtual void Release() = 0;
}* LPDIRECT3D;

%extend IDirect3D {
	IOutputWindow* XCreateOutputWindow(IOutputWindow::Settings* settings) {
		// CreateOutputWindow Wrapper
		IOutputWindow* ret;
		$self->CreateOutputWindow(settings, &ret);
		return ret;
	}
	ICamera* XCreateCamera(float fov, float clipnear, float clipfar) {
		// CreateCamera Wrapper
		ICamera* ret;
		$self->CreateCamera(fov, clipnear, clipfar, &ret);
		return ret;
	}
	ILight* XCreateLight(ILight::LightType type) {
		// CreateLight Wrapper
		ILight* ret;
		$self->CreateLight(type, &ret);
		return ret;
	}
};

extern "C" __declspec(dllexport) LPDIRECT3D __cdecl CreateDirect3D();



%module ISDirectInput
%{
#include <ISDirectInput.h>
%}

typedef class IDirectIpt
{
public:
	DIMOUSESTATE2 mouse;
	BYTE keys[256];
	DIJOYSTATE2* ctrls;

	virtual void OnCreateForm(HWND wnd) = 0;
	virtual void OnRemoveForm(HWND wnd) = 0;
	virtual void OnGainFocus(HWND wnd) = 0;
	virtual void OnLoseFocus(HWND wnd) = 0;

	virtual void Sync(SyncedPtr<Engine>& exeeng, SyncedPtr<IDirect3D>& exed3d, SyncedPtr<IOpenGL>& exeogl, SyncedPtr<IHavok>& exehvk, SyncedPtr<IGuiFactory>& exegui, \
					  SyncedPtr<IForms>& exefms, SyncedPtr<IDirectIpt>& exedip, SyncedPtr<IWinIpt>& exewip, SyncedPtr<IFFmpeg>& exeffm, SyncedPtr<ICryptoPP>& execpp, \
					  SyncedPtr<IID3Lib>& exeid3, SyncedPtr<IWinAudio>& exewad, SyncedPtr<IXAudio2>& exexa2, SyncedPtr<IWinMidi>& exemid, SyncedPtr<IWinSock>& exewsk, \
					  SyncedPtr<IAsyncWorkers>& exeaws, SyncedPtr<ISQLite>& exesql, SyncedPtr<IHaruPdf>& exepdf, SyncedPtr<IRayTracer>& exertr, SyncedPtr<IPbrt>& exepbrt, \
					  SyncedPtr<IPythonScriptEngine>& exepse, SyncedPtr<Console>& execle) = 0;
	virtual Result Init() = 0;
	virtual Result QueryGameControllers(std::vector<GameControllerDesc>& controllers, bool attachedonly = true, bool ffonly = false) = 0;
	virtual Result EnableMouse(bool isforeground, bool isexclusive) = 0;
	virtual Result EnableKeyboard(bool isforeground, bool isexclusive) = 0;
	virtual Result EnableGameController(const GUID &guid, bool isforeground, bool isexclusive, int *index = NULL) = 0;
	virtual void DisableMouse() = 0;
	virtual void DisableKeyboard() = 0;
	virtual void DisableGameController(int index) = 0;
	virtual bool IsMouseEnabled() = 0;
	virtual bool IsKeyboardEnabled() = 0;
	virtual int GetNumGameControllers() = 0;
	virtual Result ChangeMouseCooperativeLevel(bool isforeground, bool isexclusive) = 0;
	virtual Result ChangeKeyboardCooperativeLevel(bool isforeground, bool isexclusive) = 0;
	virtual Result ChangeGameControllerCooperativeLevel(bool isforeground, bool isexclusive) = 0;
	virtual void GetMouseCooperativeLevel(bool& isforeground, bool& isexclusive) = 0;
	virtual void GetKeyboardCooperativeLevel(bool& isforeground, bool& isexclusive) = 0;
	virtual void GetGameControllerCooperativeLevel(bool& isforeground, bool& isexclusive) = 0;
	virtual void Update() = 0;
	virtual void AddKeyHandler(Key::Key key, KEYDOWNCALLBACK keydowncbk, LPVOID keydowncbkuser) = 0;
	virtual void RemoveKeyHandler(Key::Key key) = 0;
	virtual void AddMouseHandler(int key, MOUSECALLBACK keydowncbk, LPVOID keydowncbkuser) = 0;
	virtual void RemoveMouseHandler(int key, MOUSECALLBACK cbk) = 0;
	virtual void Release() = 0;

	bool IsKeyPressed(Key::Key key) {return keys[key] != 0;}
}* LPDIRECTIPT;

extern "C" __declspec(dllexport) LPDIRECTIPT __cdecl CreateDirectInput();



%module ISHavok
%{
#include <ISHavok.h>
%}

//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum VisualizationType
{
	VT_NONE, VT_LOCAL, VT_VISUALDEBUGGER
};
enum HvkViewer
{
	HV_BROADPHASE, HV_CONSTRAINT, HV_ACTIVECONTACTPOINT, HV_INACTIVECONTACTPOINT, HV_CONVEXRADIUS, HV_INCONSISTENDWINDING, HV_MIDPHASE, HV_MOUSEPICKING, HV_PHANTOMDISPLAY, HV_RIGIDBODYCENTEROFMASS,
	HV_RIGIDBODYINERTIA, HV_SHAPEDISPLAY, HV_SIMULATIONISLAND, HV_SINGLEBODYCONSTRAINT, HV_SWEPTTRANSFORMDISPLAY, HV_TOICONTACTPOINT, HV_TOICOUNT, HV_VEHICLE, HV_WELDING
};
enum MotionType // Order matches hkpMotion::MotionType; don't change!
{
	MT_INVALID, MT_DYNAMIC, MT_SPHERE_INERTIA, MT_BOX_INERTIA,
	MT_KEYFRAMED, MT_FIXED, MT_THIN_BOX_INERTIA, MT_CHARACTER
};
enum CollidableQuality // Order matches hkpCollidableQualityType; don't change!
{
	CQ_INVALID = -1, CQ_FIXED, CQ_KEYFRAMED, CQ_DEBRIS, CQ_DEBRIS_SIMPLE_TOI, CQ_MOVING, CQ_CRITICAL,
	CQ_BULLET, CQ_USER, CQ_CHARACTER, CQ_KEYFRAMED_REPORTING
};
enum HvkShapeId
{
	HSID_CONVEX, HSID_SPHERE, HSID_BOX, HSID_CAPSULE, HSID_CYLINDER, HSID_PLANE, HSID_TRIANGLE, HSID_CONVEXVERTICES, HSID_CONVEXTRANSFORM,
	HSID_CONVEXTRANSLATE, HSID_MESH, HSID_EXTENDEDMESH, HSID_COMPRESSEDMESH, HSID_TRANSFORM, HSID_LIST, HSID_STATICCOMPOUND
};
enum ConstraintId
{
	CID_BALLANDSOCKET, CID_STIFFSPRING, CID_HINGE, CID_LIMITEDHINGE, CID_WHEEL, CID_PULLEY, CID_RAGDOLL
};

//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct HvkGeneralShapeDesc
{
	MotionType mtype;
	CollidableQuality qual;
	float mass;
	HvkGeneralShapeDesc() : mtype(MT_INVALID), qual(CQ_INVALID) {}
};

struct HvkShapeDesc : public HvkGeneralShapeDesc
{
protected:
	const HvkShapeId id;
	HvkShapeDesc(HvkShapeId id) : id(id) {}
public:
	HvkShapeId GetId() const {return id;}
};
struct HvkConvexShapeDesc : public HvkShapeDesc // Specialization of HvkShapeDesc
{
	HvkConvexShapeDesc(HvkShapeId id) : HvkShapeDesc(id) {}
};

struct HvkCapsuleShapeDesc : public HvkConvexShapeDesc
{
	HvkCapsuleShapeDesc() : HvkConvexShapeDesc(HSID_CAPSULE) {}
	Vector3 v0, v1;
	float radius;
};
struct HvkCylinderShapeDesc : public HvkConvexShapeDesc
{
	HvkCylinderShapeDesc() : HvkConvexShapeDesc(HSID_CYLINDER) {}
	Vector3 v0, v1;
	float radius;
};
struct HvkSphereShapeDesc : public HvkConvexShapeDesc
{
	HvkSphereShapeDesc() : HvkConvexShapeDesc(HSID_SPHERE) {}
	float radius;
};
struct HvkBoxShapeDesc : public HvkConvexShapeDesc
{
	HvkBoxShapeDesc() : HvkConvexShapeDesc(HSID_BOX) {}
	Vector3 size;
};
struct HvkPlaneShapeDesc : public HvkShapeDesc
{
	HvkPlaneShapeDesc() : HvkShapeDesc(HSID_PLANE) {}
	Vector3 vcenter, vdir, vhalfext;
};
struct HvkConvexVerticesShapeDesc : public HvkConvexShapeDesc
{
	HvkConvexVerticesShapeDesc() : HvkConvexShapeDesc(HSID_CONVEXVERTICES) {}
	BYTE* vertices;
	int numvertices, fvfstride;
	PtrWrapper GetVertexPtr() {return PtrWrapper(vertices);}
	void SetVerticesFromVertexPtr(PtrWrapper bytes) {vertices = (BYTE*)bytes.ptr;}
};
struct HvkCompressedMeshShapeDesc : public HvkShapeDesc
{
	HvkCompressedMeshShapeDesc() : HvkShapeDesc(HSID_COMPRESSEDMESH), numvertices(0), numfaces(0), numshapes(0) {}
	BYTE* vertexdata;
	UINT* indices;
	UINT numvertices, numfaces, fvfstride;
	struct TransformedShape
	{
		const HvkConvexShapeDesc* shape;
		Vector3 pos, scale;
		Quaternion qrot;
		TransformedShape() : shape(NULL), pos(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), qrot(0.0f, 0.0f, 0.0f, 1.0f) {}
	}* convexshapes;
	UINT numshapes;
	PtrWrapper GetVertexPtr() {return PtrWrapper(vertexdata);}
	void SetVerticesFromVertexPtr(PtrWrapper bytes) {vertexdata = (BYTE*)bytes.ptr;}
	PtrWrapper GetIndexPtr() {return PtrWrapper(indices);}
	void SetIndicesFromIndexPtr(PtrWrapper bytes) {indices = (UINT*)bytes.ptr;}
};
struct HvkStaticCompoundShapeDesc : public HvkShapeDesc
{
	HvkStaticCompoundShapeDesc() : HvkShapeDesc(HSID_STATICCOMPOUND), numshapes(0) {}
	struct TransformedShape
	{
		const HvkShapeDesc* shape;
		Vector3 pos, scale;
		Quaternion qrot;
		TransformedShape() : shape(NULL), pos(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), qrot(0.0f, 0.0f, 0.0f, 1.0f) {}
	}* shapes;
	UINT numshapes;
};

typedef class IWeapon
{
public:
	virtual void Fire(Vector3* vpos, Quaternion* qrot) = 0;
}* LPWEAPON;

typedef class IVehicle
{
public:
	float acceleration, steering;
	bool handbrakepulled;
}* LPVEHICLE;

typedef class IPose
{
public:
	virtual int GetNumBones() = 0;
	virtual void GetRotation(Quaternion* qrot, int boneidx) = 0;
	virtual void SetRotation(const Quaternion* qrot, int boneidx) = 0;
	virtual void GetPosition(Vector3* vpos, int boneidx) = 0;
	virtual void SetPosition(const Vector3* vpos, int boneidx) = 0;
}* LPPOSE;

typedef class IAnimation
{
public:
	virtual bool GetEnabled() {int abc = 0; return false;}//= 0;
	virtual bool SetEnabled(bool val) {int abc = 0; return false;}//= 0;
	__declspec(property(get=GetEnabled, put=SetEnabled)) bool Enabled;
	virtual float GetTime() = 0;
	virtual float SetTime(float val) = 0;
	__declspec(property(get=GetTime, put=SetTime)) float Time;
	virtual UINT GetLoops() = 0;
	virtual UINT SetLoops(UINT val) = 0;
	__declspec(property(get=GetLoops, put=SetLoops)) UINT Loops;
	virtual float GetDuration() = 0;
	virtual float SetDuration(float val) = 0;
	__declspec(property(get=GetDuration, put=SetDuration)) float Duration;
	virtual float GetWeight() = 0;
	virtual float SetWeight(float val) = 0;
	__declspec(property(get=GetWeight, put=SetWeight)) float Weight;

	virtual void AddFrame(IPose* frame) = 0;
	virtual Result Serialize(const FilePath& filename) = 0;
	virtual std::shared_ptr<void> GetPyPtr() = 0;
}* LPANIMATION;

typedef class IEntity
{
public:
	virtual void Release() = 0;
}* LPENTITY;

typedef class IPlayerEntity : public IEntity
{
public:

	virtual void Update(float dx, float dy, float rot, bool jump) = 0;
}* LPPLAYERENTITY;

typedef class IRegularEntity : public IEntity
{
public:

	virtual void applyForce(const Vector3* force) = 0;
	virtual void applyAngularImpulse(const Vector3* impulse) = 0;
	virtual void applyLinearImpulse(const Vector3* impulse) = 0;
	virtual void applyPointImpulse(const Vector3* impulse, const Vector3* pos) = 0;
	virtual void setMotionType(MotionType mtype) = 0;
}* LPREGULARENTITY;

typedef class ILevelEntity : public IEntity
{
public:

	virtual void EnableAll() = 0;
	virtual void DisableAll() = 0;
	virtual void EnableShape(UINT32 shapekey) = 0;
	virtual void DisableShape(UINT32 shapekey) = 0;
}* LPLEVELENTITY;

typedef class IBoxedLevelEntity : public IEntity
{
public:
	virtual void SetBoxTypes(BoxedLevelBoxType* boxtypes, UINT32 numtypes) = 0;
	virtual Result CreateBoxShapes(HvkShapeDesc** boxshapes, UINT32 numboxshapes) = 0;
	virtual void AssignChunk(BoxedLevelChunk* chunk) = 0;
	virtual void UnassignChunk(BoxedLevelChunk* chunk) = 0;
	virtual BoxedLevelChunk* ChunkFromShapeKey(UINT32 chunkshapekey) = 0;
	virtual void BoxPosFromShapeKey(UINT32 boxshapekey, UINT32 (*boxpos)[3]) = 0;
}* LPBOXEDLEVELENTITY;

typedef class IRagdollEntity : public IEntity
{
public:
	float dx, dz, rot;
	bool jump, onladder;

	virtual Result SetRootBone(const HvkShapeDesc* shapedesc, const String& shapekey, const Matrix* worldmatrix) = 0;
	virtual Result AttachShape(const HvkShapeDesc* shapedesc, const ConstraintDesc* constraintdesc, const String& newshapekey, const String& anchorshapekey, const Matrix* worldmatrix) = 0;
	virtual void AttachEntity(IRegularEntity* entity, const String& targetshapekey, const float* localtransform) = 0;
	virtual void DetachEntity(const IRegularEntity* entity) = 0;
	virtual Result AssembleRagdoll() = 0;
	virtual Result CreatePose(IPose** pose) = 0;
	virtual Result CreateAnimation(float duration, IAnimation** ani) = 0;
	virtual float* GetPivot(const String& shapekey) = 0;
	virtual float* GetPivot(int boneidx) = 0;
	virtual void LookAt(const float* vtargetpos, float gain) = 0;
	virtual void EnableFootIk() = 0;
	virtual void Update() = 0;
	virtual std::shared_ptr<void> GetPyPtr() = 0;
}* LPRAGDOLLENTITY;

typedef class IHavok
{
public:
	struct Time {float t, dt;} time;

	enum Layer
	{
		LAYER_DEFAULT,
		LAYER_LANDSCAPE, // Contains static geometry for the level
		LAYER_PROXY, // Contains the proxy for the character controller
		LAYER_DEBRIS, // Contains small dynamic objects in the scene. Each of these have different masses
		LAYER_MOVABLE_ENVIRONMENT, // Contains large dynamic objects in the scene
		LAYER_RAGDOLL_KEYFRAMED, // Contains the bodies of the ragdoll that are currently keyframed
		LAYER_RAYCAST, // This layer is used to control which items to raycast against
		LAYER_RAGDOLL_DYNAMIC, // Contains the rigid bodies for the ragdoll which are not keyframed. When the ragdoll falls all ragdoll bodies are switched into this layer
		LAYER_PICKING, // A layer that control which items are effected by the mouse pointer
		LAYER_HANDHELD // An attachable item
	};

	virtual void Sync(SyncedPtr<Engine>& exeeng, SyncedPtr<IDirect3D>& exed3d, SyncedPtr<IOpenGL>& exeogl, SyncedPtr<IHavok>& exehvk, SyncedPtr<IGuiFactory>& exegui, \
					  SyncedPtr<IForms>& exefms, SyncedPtr<IDirectIpt>& exedip, SyncedPtr<IWinIpt>& exewip, SyncedPtr<IFFmpeg>& exeffm, SyncedPtr<ICryptoPP>& execpp, \
					  SyncedPtr<IID3Lib>& exeid3, SyncedPtr<IWinAudio>& exewad, SyncedPtr<IXAudio2>& exexa2, SyncedPtr<IWinMidi>& exemid, SyncedPtr<IWinSock>& exewsk, \
					  SyncedPtr<IAsyncWorkers>& exeaws, SyncedPtr<ISQLite>& exesql, SyncedPtr<IHaruPdf>& exepdf, SyncedPtr<IRayTracer>& exertr, SyncedPtr<IPbrt>& exepbrt, \
					  SyncedPtr<IPythonScriptEngine>& exepse, SyncedPtr<Console>& execle) = 0;

	virtual Result Init(const Vector3& worldsize_min, const Vector3& worldsize_max, VisualizationType vtype, LPOUTPUTWINDOW localvizwnd = NULL, LPRENDERSHADER localvizshader = NULL) = 0;
	virtual void InitDone() = 0;

	virtual Result CreateRegularEntity(const HvkShapeDesc* shapedesc, Layer layer, Vector3* vpos, Quaternion* qrot, IRegularEntity** entity) = 0;
	virtual Result CreateLevelEntity(const HvkStaticCompoundShapeDesc* shapedesc, const Vector3& vpos, const Quaternion& qrot, ILevelEntity** entity) = 0;
	virtual Result CreateBoxedLevelEntity(const UINT (&chunksize)[3], IBoxedLevelEntity** entity) = 0;
	virtual Result CreateRagdollEntity(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot, IRagdollEntity** entity) = 0;
	virtual Result CreatePlayerEntity(const HvkShapeDesc* shapedesc, Vector3* vpos, Quaternion* qrot, bool isrigidbody, IPlayerEntity** entity) = 0;
	virtual Result CreateWeapon(IWeapon** weapon) = 0;
	virtual Result CreateVehicle(const VehicleDesc* vehicledesc, Vector3* vpos, Quaternion* qrot, IVehicle** vehicle) = 0;

	virtual Result CreateCollisionMesh(LPOBJECT obj, IRegularEntity** entity) = 0;
	virtual Result CreateConvexCollisionHull(const HvkGeneralShapeDesc* shapedesc, LPOBJECT obj, IRegularEntity** entity) = 0;
	virtual Result CreateCollisionCapsule(LPOBJECT obj, float radius, IRegularEntity** entity) = 0;
	virtual Result CreateCollisionRagdoll(LPOBJECT obj, float radius, IRagdollEntity** entity) = 0;

	virtual float CastRayTo(const Vector3& src, const Vector3& dest, LPENTITY* hitentity = NULL, UINT32 (*shapekeys)[8] = NULL, Vector3* hitnml = NULL) = 0;
	virtual float CastRayDir(const Vector3& src, const Vector3& dir, LPENTITY* hitentity = NULL, UINT32 (*shapekeys)[8] = NULL, Vector3* hitnml = NULL) = 0;

	virtual void EnableViewer(HvkViewer viewer) = 0;
	virtual void DisableViewer(HvkViewer viewer) = 0;
	virtual String GetViewerName(HvkViewer viewer) = 0;

	virtual void MousePickGrab(float* campos, float* camdir) = 0;
	virtual void MousePickDrag(float* campos, float* camdir) = 0;
	virtual void MousePickRelease() = 0;

	virtual void Update() = 0;
	virtual void Release() = 0;
}* LPHAVOK;

%extend IHavok {
	IRegularEntity* XCreateCollisionMesh(LPOBJECT obj) {
		// CreateCollisionMesh Wrapper
		IRegularEntity* ret;
		$self->CreateCollisionMesh(obj, &ret);
		return ret;
	}
	IRegularEntity* XCreateRegularEntity(const HvkShapeDesc* shapedesc, Layer layer, Vector3* vpos, Quaternion* qrot) {
		// CreateRegularEntity Wrapper
		IRegularEntity* ret;
		$self->CreateRegularEntity(shapedesc, layer, vpos, qrot, &ret);
		return ret;
	}
};

extern "C" __declspec(dllexport) LPHAVOK __cdecl CreateHavok();



%module ISWinMidi
%{
#include <ISWinMidi.h>
%}

typedef unsigned char byte;

enum SoundDoneCallbackResult
	{SDCR_OK, SDCR_FINISHED, SDCR_NODATA};
enum MidiDeviceTechnology
{
	MDT_UNDEFINED = 0,
	MDT_HARDWARE_PORT = 1,
	MDT_SYNTHESIZER = 2,
	MDT_SQUAREWAVE_SYNTHESIZER = 3,
	MDT_FM_SYNTHESIZER = 4,
	MDT_MAPPER = 5,
	MDT_WAVETABLE_SYNTHESIZER = 6,
	MDT_SOFTWARE_SYNTHESIZER = 7
};

%typemap(ctype) NOTEDOWNCALLBACK, NOTEDOWNCALLBACK& "void*"
%typemap(in) NOTEDOWNCALLBACK  %{ $1 = ($1_type)$input; %}
%typemap(in) NOTEDOWNCALLBACK& %{ $1 = ($1_type)&$input; %}
%typemap(imtype, out="IntPtr") NOTEDOWNCALLBACK, NOTEDOWNCALLBACK& "NoteDownFunc"
%typemap(cstype, out="IntPtr") NOTEDOWNCALLBACK, NOTEDOWNCALLBACK& "NoteDownFunc"
%typemap(csin) NOTEDOWNCALLBACK, NOTEDOWNCALLBACK& "$csinput"

typedef class IMidiOutDevice
{
public:

	virtual void SendNoteOn(byte channel, byte note, byte velo) = 0;
	virtual void SendNoteOff(byte channel, byte note, byte velo) = 0;
	virtual WORD GetVolume() = 0;
	virtual void SetVolume(WORD val) = 0;
	__declspec(property(get=GetVolume, put=SetVolume)) WORD Volume;
	virtual WORD GetLVolume() = 0;
	virtual void SetLVolume(WORD val) = 0;
	__declspec(property(get=GetLVolume, put=SetLVolume)) WORD LVolume;
	virtual WORD GetRVolume() = 0;
	virtual void SetRVolume(WORD val) = 0;
	__declspec(property(get=GetRVolume, put=SetRVolume)) WORD RVolume;
	virtual void Release() = 0;
}* LPMIDIOUTDEVICE;

typedef class IMidiInDevice
{
public:

	virtual void RegisterNoteDownCallback(NOTEDOWNCALLBACK cbk, LPVOID user) = 0;
	/*virtual WORD GetVolume() = 0;
	virtual void SetVolume(WORD val) = 0;
	__declspec(property(get=GetVolume, put=SetVolume)) WORD Volume;
	virtual WORD GetLVolume() = 0;
	virtual void SetLVolume(WORD val) = 0;
	__declspec(property(get=GetLVolume, put=SetLVolume)) WORD LVolume;
	virtual WORD GetRVolume() = 0;
	virtual void SetRVolume(WORD val) = 0;
	__declspec(property(get=GetRVolume, put=SetRVolume)) WORD RVolume;*/
	virtual void Release() = 0;
}* LPMIDIINDEVICE;

typedef class IWinMidi
{
public:

	virtual void Sync(SyncedPtr<Engine>& exeeng, SyncedPtr<IDirect3D>& exed3d, SyncedPtr<IOpenGL>& exeogl, SyncedPtr<IHavok>& exehvk, SyncedPtr<IGuiFactory>& exegui, \
					  SyncedPtr<IForms>& exefms, SyncedPtr<IDirectIpt>& exedip, SyncedPtr<IWinIpt>& exewip, SyncedPtr<IFFmpeg>& exeffm, SyncedPtr<ICryptoPP>& execpp, \
					  SyncedPtr<IID3Lib>& exeid3, SyncedPtr<IWinAudio>& exewad, SyncedPtr<IXAudio2>& exexa2, SyncedPtr<IWinMidi>& exemid, SyncedPtr<IWinSock>& exewsk, \
					  SyncedPtr<IAsyncWorkers>& exeaws, SyncedPtr<ISQLite>& exesql, SyncedPtr<IHaruPdf>& exepdf, SyncedPtr<IRayTracer>& exertr, SyncedPtr<IPbrt>& exepbrt, \
					  SyncedPtr<IPythonScriptEngine>& exepse, SyncedPtr<Console>& execle) = 0;
	virtual Result Init() = 0;
	virtual Result GetOutputDeviceTechnology(UINT deviceid, MidiDeviceTechnology* mdt) const = 0;
	virtual Result GetInputProductName(UINT deviceid, char (&name)[32]) const = 0;
	virtual Result OpenOutputDevice(UINT deviceid, IMidiOutDevice** outdev) = 0;
	virtual Result OpenInputDevice(UINT deviceid, IMidiInDevice** indev) = 0;
	virtual void Release() = 0;
}* LPWINMIDI;

%extend IWinMidi {
	MidiDeviceTechnology XGetOutputDeviceTechnology(UINT deviceid) {
		// GetOutputDeviceTechnology Wrapper
		MidiDeviceTechnology ret;
		$self->GetOutputDeviceTechnology(deviceid, &ret);
		return ret;
	}
	String XGetInputProductName(UINT deviceid) {
		// GetInputProductName Wrapper
		char productname[32];
		$self->GetInputProductName(deviceid, productname);
		return String(productname);
	}
	IMidiOutDevice* XOpenOutputDevice(UINT deviceid) {
		// OpenOutputDevice Wrapper
		IMidiOutDevice* ret;
		$self->OpenOutputDevice(deviceid, &ret);
		return ret;
	}
	IMidiInDevice* XOpenInputDevice(UINT deviceid) {
		// OpenInputDevice Wrapper
		IMidiInDevice* ret;
		$self->OpenInputDevice(deviceid, &ret);
		return ret;
	}
};

extern "C" __declspec(dllexport) LPWINMIDI __cdecl CreateWinMidi();
