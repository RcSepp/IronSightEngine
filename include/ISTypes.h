#ifndef __ISTYPES_H
#define __ISTYPES_H

#include <windows.h>
#include <tchar.h>
#include <memory>

void* __cdecl _realloc(void* memory, size_t memorysize, size_t newsize);

#ifndef __ISTYPES_H_STRING
#define __ISTYPES_H_STRING
//-----------------------------------------------------------------------------
// Name: String
// Desc: Extended string class with easy concatination, stored string length, full TCHAR support, etc.
//-----------------------------------------------------------------------------
class String
{
private:
	void RemoveReference();

protected:
	LPTSTR chars;
	size_t len;
	UINT* refcounter;
public:

	String& operator=(const String& src);
	String operator<<(const String& src) const;
	String& operator<<=(const String& src);
	String operator<<(TCHAR src) const;
	String operator<<(int src) const;
	bool operator==(const String& s) const;
	bool operator!=(const String& s) const;
	bool operator<(const String& s) const;
	bool operator>(const String& s) const;
	bool Equals(const String& s) const;
	operator LPTSTR() const;
	LPTSTR CopyChars() const;
	int ToInt() const;
	float ToFloat() const;
	double ToDouble() const;
	LPTSTR ToTCharString() const;
	LPSTR ToCharString() const;
	LPWSTR ToWCharString() const;
	String(const String& src);
	String(LPCSTR chars); String(LPCWSTR chars);
	String(LPSTR chars); String(LPWSTR chars);
	String(LPCSTR chars, size_t len); String(LPCWSTR chars, size_t len);
	String(LPSTR chars, size_t len); String(LPWSTR chars, size_t len);
	String(CHAR chr); String(WCHAR chr);
	String(int num);
	String(double num);
	String();
	static String StealReference(LPTSTR chars);
	void Clear();
	String* Copy(String* result) const;
	~String();
	// StringEx
	String* Substring(UINT start, String* result) const;
	String* Substring(UINT start, UINT length, String* result) const;
	String* Replace(const String& oldval, const String& newval, String* result) const;

	__forceinline size_t length() const
	{
		return len;
	};
	__forceinline bool IsEmpty() const
	{
		return !len;
	};

	friend class FilePath;
};
#endif

#ifndef __ISTYPES_H_FILEPATH
#define __ISTYPES_H_FILEPATH
//-----------------------------------------------------------------------------
// Name: FilePath
// Desc: String class with extended functionality
//-----------------------------------------------------------------------------
class FilePath : public String
{
public:
#ifdef _WIN32
	struct FindHandle
	{
		friend class FilePath;
	private:
		HANDLE h;
	public:
		FindHandle() {h = NULL;}
		~FindHandle() {Release();}
		void Release() {if(h) {FindClose(h); h = NULL;}}
	};
#endif

	// FilePathEx
	FilePath* TrimExtension(FilePath* result) const;
	void TrimExtension();
	String* GetExtension(String* result) const;
	FilePath* AddSuffix(String suffix, FilePath* result) const;
	FilePath* TrimDir(FilePath* result) const;
	bool GetParentDir(FilePath* result) const;
	bool GetParentDir();
	bool Exists() const;

#ifdef _WIN32
	bool Find(FilePath* result) const;
	bool Find();

	bool GetLastWriteTime(ULARGE_INTEGER* time) const;
	bool GetLastWriteTime(LPSYSTEMTIME time) const;

	bool GetFiles(FindHandle* hfind, DWORD wantedattr, DWORD unwantedattr, WIN32_FIND_DATA* fdata) const;
	bool GetFiles(FindHandle* hfind, WIN32_FIND_DATA* fdata) const;
	bool GetDirectories(FindHandle* hfind, WIN32_FIND_DATA* fdata) const;
#else
	bool GetLastWriteTime(time_t* time) const;
	bool GetLastWriteTime(struct tm** time) const;
#endif

	FilePath& operator=(const String& src);
	FilePath(LPCSTR chars) : String(chars) {}; FilePath(LPCWSTR chars) : String(chars) {};
	FilePath(LPSTR chars) : String(chars) {}; FilePath(LPWSTR chars) : String(chars) {};
	FilePath(LPCSTR chars, size_t len) : String(chars, len) {}; FilePath(LPCWSTR chars, size_t len) : String(chars, len) {};
	FilePath(LPSTR chars, size_t len) : String(chars, len) {}; FilePath(LPWSTR chars, size_t len) : String(chars, len) {};
	FilePath(CHAR chr) : String(chr) {}; FilePath(WCHAR chr) : String(chr) {};
	FilePath(int num) : String(num) {};
	FilePath(double num) : String(num) {};
	FilePath() : String() {};
};
#endif

#ifndef __ISTYPES_H_RESULT
#define __ISTYPES_H_RESULT
//-----------------------------------------------------------------------------
// Name: Result
// Desc: Supports inline error strings. Can be extended for supporting HRESULT, MMRESULT, ...
//-----------------------------------------------------------------------------
struct Result
{
private:
	void RemoveReference();

public:
	struct Details
	{
		friend struct Result;
	private:
		String msg, file, func;
		int line;
		bool iscaught;

		Details(LPCTSTR msg, LPCTSTR file, LPCTSTR func, int line)
		{
			this->msg = String(msg);
			this->file = String(file);
			this->func = String(func);
			this->line = line;
			iscaught = false;
		}
		Details(String& msg, LPCTSTR file, LPCTSTR func, int line)
		{
			this->msg = msg;
			this->file = String(file);
			this->func = String(func);
			this->line = line;
			iscaught = false;
		}
		Details(LPCTSTR msg)
		{
			this->msg = String(msg);
			line = -1;
			iscaught = false;
		}
		Details(String msg)
		{
			this->msg = msg;
			line = -1;
			iscaught = false;
		}
	}* details;
	UINT* refcounter;

	Result();
	Result(LPCTSTR msg);
	Result(String& msg);
	Result(LPCTSTR msg, LPCTSTR file, LPCTSTR func, int line);
	Result(String& msg, LPCTSTR file, LPCTSTR func, int line);
	~Result();

	static void PrintLogMessage(String msg, String file, String func, int line)
	{
		OutputDebugStringA("Log at line "); OutputDebugStringA(String(line).ToCharString());
		OutputDebugStringA(" of "); OutputDebugStringA(file.ToCharString());
		OutputDebugStringA(" in "); OutputDebugStringA(func.ToCharString());
		OutputDebugStringA(": "); OutputDebugStringA(msg.ToCharString());
		OutputDebugStringA("\n");
	}
	static void PrintLogMessage(String msg)
	{
		OutputDebugStringA("Log: "); OutputDebugStringA(msg.ToCharString());
		OutputDebugStringA("\n");
	}
	static void PrintLogMessage(const Details* details)
	{
		if(details->line == -1)
		{
			OutputDebugStringA("Log at line "); OutputDebugStringA(String(details->line).ToCharString());
			OutputDebugStringA(" of "); OutputDebugStringA(details->file.ToCharString());
			OutputDebugStringA(" in "); OutputDebugStringA(details->func.ToCharString());
			OutputDebugStringA(": "); OutputDebugStringA(details->msg.ToCharString());
		}
		else
			OutputDebugStringA("Log: "); OutputDebugStringA(details->msg.ToCharString());
		OutputDebugStringA("\n");
	}

	bool operator==(const Result& r);
	Result& operator=(const Result& src);
	Result(const Result& copy);
	String GetLastResult(bool msgonly = false) const;
};
Result Error(LPCTSTR msg, LPCTSTR file, LPCTSTR func, int line);
Result Error(String msg, LPCTSTR file, LPCTSTR func, int line);
__forceinline Result CheckResult(Result newrlt)
	{return newrlt;}
#define CHKRESULT(command)		if((rlt = CheckResult(command)).details) return rlt
#define CHKALLOC(alloc)			if(!(alloc)) return ERR("Out of memory") //E_OUTOFMEMORY
#define LOGERRORS(command)		if((rlt = CheckResult(command)).details) Result::PrintLogMessage(rlt.details)
#define IFFAILED(command)		if((rlt = CheckResult(command)).details)
#define IFSUCCEEDED(command)	if(!(rlt = CheckResult(command)).details)
#define R_OK					Result()
//#define R_ERROR				Result()
#define ERR(msg)				Result(msg, __FILE__, __FUNCTION__, __LINE__)
#define LOG(msg)				Result::PrintLogMessage(msg, __FILE__, __FUNCTION__, __LINE__)
#endif

//-----------------------------------------------------------------------------
// Name: std::list
// Desc: A doubly linked list of elements
//-----------------------------------------------------------------------------
#include <list>
#define LIST_FOREACH(list, iterator)			for((iterator) = (list).begin(); (iterator) != (list).end(); (iterator)++)
#define LIST_FOREACH_I(list, iterator, i)		for((iterator) = (list).begin(), (i) = 0; (iterator) != (list).end(); (iterator)++, (i)++)
#define LPLIST_FOREACH(list, iterator)			for((iterator) = (list)->begin(); (iterator) != (list)->end(); (iterator)++)
#define LPLIST_FOREACH_I(list, iterator, i)		for((iterator) = (list)->begin(), (i) = 0; (iterator) != (list)->end(); (iterator)++, (i)++)

//-----------------------------------------------------------------------------
// Name: std::stack
// Desc: A linked list of elements (accessed LIFO)
//-----------------------------------------------------------------------------
#include <stack>

//-----------------------------------------------------------------------------
// Name: std::stack
// Desc: A linked list of elements (accessed FIFO)
//-----------------------------------------------------------------------------
#include <queue>

#include <map>

#ifndef __ISTYPES_H_PTRWRAPPER
#define __ISTYPES_H_PTRWRAPPER
//-----------------------------------------------------------------------------
// Name: PtrWrapper
// Desc: Encapsulates a void pointer
//-----------------------------------------------------------------------------
struct PtrWrapper
{
	void* ptr;
	PtrWrapper(void* ptr) : ptr(ptr) {}
	operator void*() const {return ptr;}
};
#endif

#ifndef __ISTYPES_H_PRIORITYQUEUE
#define __ISTYPES_H_PRIORITYQUEUE
//-----------------------------------------------------------------------------
// Name: PriorityQueue
// Desc: A queue that primarily orders by priority (defined by '<' operator) and secodarily by insertion order
//-----------------------------------------------------------------------------
template<class type, class comp = type> class PriorityQueue : public std::list<type>
{
public:
	void insert(const type& v)
	{
		typename std::list<type>::const_iterator iter;
		comp cmp;
		for(iter = std::list<type>::cbegin(); iter != std::list<type>::cend() && cmp(*iter, v); iter++) {}
		std::list<type>::insert(iter, v);
	}
};
#endif

#ifndef __ISTYPES_H_SYNCEDPTR
#define __ISTYPES_H_SYNCEDPTR
//-----------------------------------------------------------------------------
// Name: SyncedPtr
// Desc: A pointer extension that syncs multiple instances of the same pointer
//-----------------------------------------------------------------------------
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

	SyncedPtr& operator=(const SyncedPtr& src)
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
	}
	type* operator->()
		{return *ptr;}
	const type* operator->() const
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
	}
};
#endif

#ifndef __ISTYPES_H_ROTARYBUFFER
#define __ISTYPES_H_ROTARYBUFFER
//-----------------------------------------------------------------------------
// Name: RotaryBuffer
// Desc: A buffer with read an write methodes, that simply wraps around on overflow
//-----------------------------------------------------------------------------
template<class type = byte> class RotaryBuffer
{
private:
	type* buf;
	size_t buflen, readpos, writepos;
	bool isempty;

public:

	RotaryBuffer(size_t size)
	{
		buf = new type[buflen = size];
		clear();
	}
	~RotaryBuffer()
	{
		delete[] buf;
	}
	void clear()
	{
		readpos = writepos = 0;
		isempty = true;
	}
	size_t size()
	{
		if(isempty)
			return 0;
		else
			return writepos - readpos + (writepos <= readpos) * buflen;
	}
	bool empty()
	{
		return isempty;
	}
	type* readptr()
	{
		return buf + readpos;
	}
	size_t writespaceavailable()
	{
		if(writepos == readpos)
			return isempty ? buflen : 0;
		else if(writepos > readpos)
			return std::max(buflen - writepos, readpos);
		else
			return readpos - writepos;
	}
	void read(type* data, size_t datalen)
	{
		if(!datalen)
			return;
		size_t copylen = std::min(datalen, buflen - readpos);
		memcpy(data, buf + readpos, copylen * sizeof(type));
		if(copylen < datalen)
		{
			readpos = datalen - copylen;
			memcpy(data + copylen, buf, readpos * sizeof(type));
		}
		else
			readpos += copylen;
		if(readpos == writepos)
			isempty = true;
	}
	void pop(size_t datalen)
	{
		if(!datalen)
			return;
		size_t copylen = std::min(datalen, buflen - readpos);
		if(copylen < datalen)
			readpos = datalen - copylen;
		else
			readpos += copylen;
		if(readpos == writepos)
			isempty = true;
	}
	void write(const type* data, size_t datalen)
	{
		if(!datalen)
			return;
		size_t copylen = std::min(datalen, buflen - writepos);
		memcpy(buf + writepos, data, copylen * sizeof(type));
		if(copylen < datalen)
		{
			writepos = datalen - copylen;
			memcpy(buf, data + copylen, writepos * sizeof(type));
		}
		else
			writepos += copylen;
		isempty = false;
	}
};
#endif

#ifndef __ISTYPES_H_SEMIROTARYBUFFER
#define __ISTYPES_H_SEMIROTARYBUFFER
//-----------------------------------------------------------------------------
// Name: SemiRotaryBuffer
// Desc: A rotary buffer, that supports by pointer access for writing
//-----------------------------------------------------------------------------
template<class type = byte> class SemiRotaryBuffer
{
private:
	type* buf;
	size_t buflen, readpos, writepos, usedsize;
	bool isempty;

public:

	SemiRotaryBuffer(size_t size)
	{
		buf = new type[buflen = size];
		clear();
	}
	~SemiRotaryBuffer()
	{
		delete[] buf;
	}
	void clear()
	{
		readpos = writepos = 0;
		usedsize = buflen;
		isempty = true;
	}
	size_t size()
	{
		if(isempty)
			return 0;
		else
			return writepos - readpos + (writepos <= readpos) * usedsize;
	}
	size_t writespaceavailable()
	{
		if(writepos == readpos)
			return isempty ? buflen : 0;
		else if(writepos > readpos)
			return std::max(buflen - writepos, readpos);
		else
			return readpos - writepos;
	}
	void read(type* data, size_t datalen)
	{
		if(!datalen)
			return;
		size_t copylen = std::min(datalen, usedsize - readpos);
		memcpy(data, buf + readpos, copylen * sizeof(type));
		if(copylen < datalen)
		{
			readpos = datalen - copylen;
			memcpy(data + copylen, buf, readpos * sizeof(type));
		}
		else
			readpos += copylen;
		if(readpos == writepos)
			isempty = true;
	}
	type* getwriteptr(size_t maxdatalen)
	{
		return buf + (buflen - writepos >= maxdatalen) * writepos;
	}
	void finalizewrite(type* writeptr, size_t sizewritten)
	{
		if(!sizewritten)
			return;
		if(writepos && writeptr == buf)
			usedsize = writepos;
		writepos = writeptr - buf + sizewritten;
		isempty = false;
	}
};
#endif

#ifndef __ISTYPES_H_MINIQUEUE
#define __ISTYPES_H_MINIQUEUE
//-----------------------------------------------------------------------------
// Name: MiniQueue
// Desc: A LIFO buffer with fixed size
//-----------------------------------------------------------------------------
template<class type, class sizetype = size_t> class MiniQueue
{
public: //DELETE
	type* queue;
	sizetype queuelen, frontidx, backidx;

public:
	MiniQueue(sizetype size)
	{
		queue = (type*)new LPVOID[this->queuelen = size + 1]; // +1 ... This simple buffer assumes itself to be empty if frontidx == backidx. Therefore the last slot can't be used
		frontidx = backidx = 0;
	}
	~MiniQueue()
	{
		delete[] queue;
	}
	sizetype size()
	{
		return frontidx - backidx + (backidx > frontidx) * queuelen;
	}
	bool empty()
	{
		return frontidx == backidx;
	}
	void push(type val)
	{
		sizetype nextidx = (frontidx + 1) % queuelen;
		if(nextidx == backidx)
			return; // Queue is full
		queue[frontidx] = val;
		frontidx = nextidx;
	}
	type pop()
	{
		if(frontidx == backidx)
			return 0; // Queue is empty
		type val = queue[backidx];
		++backidx %= queuelen;
		return val;
	}
	type front()
	{
		if(frontidx == backidx)
			return 0; // Queue is empty
		return queue[backidx];
	}
};
#endif

#ifndef __ISTYPES_H_STATICCONTENTMINIQUEUE
#define __ISTYPES_H_STATICCONTENTMINIQUEUE
//-----------------------------------------------------------------------------
// Name: StaticContentMiniQueue
// Desc: A MiniQueue returning only pointers to a fixed content
//-----------------------------------------------------------------------------
template<class type, class sizetype = size_t> class StaticContentMiniQueue
{
	sizetype queuelen, front, back;

public:
	type* content;

	StaticContentMiniQueue(sizetype size)
	{
		content = (type*)new LPVOID[this->queuelen = size + 1]; // +1 ... This simple buffer assumes itself to be empty if front == back. Therefore the last slot can't be used
		front = back = 0;
	}
	~StaticContentMiniQueue()
	{
		delete[] content;
	}
	sizetype size()
	{
		return front - back + (back > front) * queuelen;
	}
	sizetype contentsize()
	{
		return queuelen;
	}
	bool empty()
	{
		return front == back;
	}
	type push()
	{
		sizetype next = (front + 1) % queuelen;
		if(next == back)
			return 0; // Queue is full
		type val = content[front];
		front = next;
		return val;
	}
	type pop()
	{
		if(front == back)
			return 0; // Queue is empty
		type val = content[back];
		++back %= queuelen;
		return val;
	}
};
#endif

#if(!defined(__ISTYPES_H_POINT) && !defined(ISTYPES_NO_POINT))
#define __ISTYPES_H_POINT
//-----------------------------------------------------------------------------
// Name: Point
// Desc: Simple point representation with variable type
//-----------------------------------------------------------------------------
template<class type = int> struct Point
{
	type x, y;

	Point(){};
	Point(type x, type y)
	{
		this->x = x;
		this->y = y;
	}
	bool operator==(const Point<type>& p) const
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
	}

	inline String ToString() const
		{return String("Point(x = ") << x << String(", y = ") << y << String(")");}
};
#endif

#if(!defined(__ISTYPES_H_SIZE) && !defined(ISTYPES_NO_SIZE))
#define __ISTYPES_H_SIZE
//-----------------------------------------------------------------------------
// Name: Size
// Desc: Simple size representation with variable type
//-----------------------------------------------------------------------------
template<class type = int> struct Size
{
	type width, height;

	Size(){};
	Size(type width, type height)
	{
		this->width = width;
		this->height = height;
	}
	bool operator==(const Size<type>& s) const
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
	}

	inline String ToString() const
		{return String("Size(width = ") << (int)width << String(", height = ") << (int)height << String(")");}
};
#endif

#if(!defined(__ISTYPES_H_RECT) && !defined(ISTYPES_NO_POINT) && !defined(ISTYPES_NO_SIZE))
#define __ISTYPES_H_RECT
//-----------------------------------------------------------------------------
// Name: Rect
// Desc: Simple rectangle representation with variable type
//-----------------------------------------------------------------------------
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
	Rect(const LPRECT rect)
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
	}

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
#endif

#endif
