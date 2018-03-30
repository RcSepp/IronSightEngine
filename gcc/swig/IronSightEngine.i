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
