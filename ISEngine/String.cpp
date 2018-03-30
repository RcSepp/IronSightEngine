#include "ISTypes.h"

#define REMOVE_ARRAY(ptr)		{if(ptr) {delete[] (ptr); (ptr) = NULL;}}
#define REMOVE(ptr)				{if(ptr) {delete (ptr); (ptr) = NULL;}}

String& String::operator=(const String& src)
{
	if(&src == this)
		return *this;

	// Remove this string from its current reference chain
	RemoveReference();

	// Attach this string to the reference chain of src
	this->len = src.len;
	this->chars = src.chars;
	this->refcounter = src.refcounter;
	(*this->refcounter)++;

	return *this;
}
FilePath& FilePath::operator=(const String& src)
{
	if(&src == this)
		return *this;

	// Remove this string from its current reference chain
	RemoveReference();

	// Attach this string to the reference chain of src
	this->len = src.len;
	this->chars = src.chars;
	this->refcounter = src.refcounter;
	(*this->refcounter)++;

	return *this;
}
String& String::operator<<=(const String& src)
{
	if(&src == this)
		return *this;

	if(!this->len)
		this->operator=(src);
	else if(src.len)
	{
		LPTSTR oldchars = this->chars;

		// Remove this string from its current reference chain
		if(--(*this->refcounter) == 0)
			this->chars = (LPTSTR)_realloc(this->chars, this->len + 1, this->len + src.len + 1);
		else
		{
			LPTSTR oldchars = this->chars;
			this->chars = new TCHAR(this->len + src.len + 1);
			_tcscpy_s(this->chars, this->len + src.len + 1, oldchars);
		}
		_tcscat_s(this->chars, this->len + src.len + 1, src.chars);
		this->len += src.len;

		// Create a new reference chain
		*this->refcounter = 1;
	}
	return *this;
}
String String::operator<<(const String& src) const
{
	if(src.IsEmpty())
		return String(*this);
	else
	{
		String newstr;
		newstr.len = this->len + src.len;
		newstr.chars = new TCHAR[newstr.len + 1];
		memcpy(newstr.chars, this->chars, this->len * sizeof(TCHAR));
		memcpy(newstr.chars + this->len, src.chars, (src.len + 1) * sizeof(TCHAR)); // +1 ... Also copy '\0'
		return newstr;
	}
}
String String::operator<<(TCHAR src) const
{
	String newstr;
	newstr.len = this->len + 1;
	newstr.chars = new TCHAR[newstr.len + 1];
	memcpy(newstr.chars, this->chars, this->len * sizeof(TCHAR));
	newstr.chars[this->len] = src;
	newstr.chars[newstr.len] = '\0';
	return newstr;
}
String String::operator<<(int src) const
{
	TCHAR buf[16];
	_itot_s(src, buf, 10);

	String newstr;
	newstr.len = this->len + _tcsclen(buf);
	newstr.chars = new TCHAR[newstr.len + 1];
	memcpy(newstr.chars, this->chars, this->len * sizeof(TCHAR));
	_tcscpy_s(newstr.chars + this->len, newstr.len - this->len + 1, buf);
	return newstr;
}
bool String::Equals(const String& s) const
{
	if(this->IsEmpty())
		return s.IsEmpty();
	else if(s.IsEmpty())
		return false;
	else
		return strcmp(this->chars, s.chars) == 0;
}
bool String::operator==(const String& s) const
{
	if(this->IsEmpty())
		return s.IsEmpty();
	else if(s.IsEmpty())
		return false;
	else
		return strcmp(this->chars, s.chars) == 0;
}
bool String::operator!=(const String& s) const
{
	if(this->IsEmpty())
		return !s.IsEmpty();
	else if(s.IsEmpty())
		return true;
	else
		return strcmp(this->chars, s.chars) != 0;
}
bool String::operator<(const String& s) const
{
	return std::strcmp(chars, s.chars) < 0;
}
bool String::operator>(const String& s) const
{
	return std::strcmp(chars, s.chars) > 0;
}

String::operator LPTSTR() const
{
	return chars;
}

LPTSTR String::ToTCharString() const
{
	return chars;
}
LPSTR String::ToCharString() const
{
#ifdef UNICODE
	char* nonwchars = new char[len + 1];
	WideCharToMultiByte(CP_ACP, NULL, chars, -1, nonwchars, len + 1, NULL, FALSE);
	nonwchars[len] = '\0';
	return nonwchars;
#else
	return chars;
#endif
}
LPWSTR String::ToWCharString() const
{
#ifdef UNICODE
	return chars;
#else
	WCHAR* wchars = new WCHAR[len + 1];
	MultiByteToWideChar(CP_ACP, NULL, chars, -1, wchars, len + 1);
	wchars[len] = '\0';
	return wchars;
#endif
}
LPTSTR String::CopyChars() const
{
	LPTSTR result = new TCHAR[len + 1];
	_tcscpy_s(result, len + 1, chars);
	return result;
}
int String::ToInt() const
{
	return _tstoi(chars);
}
float String::ToFloat() const
{
	return (float)_tstof(chars);
}
double String::ToDouble() const
{
	return _tstof(chars);
}
String::String(const String& src)
{
	// Attach this string to the reference chain of src
	this->len = src.len;
	this->chars = src.chars;
	this->refcounter = src.refcounter;
	(*this->refcounter)++;
}
String::String(LPCTSTR chars)
{
	if(!chars || chars[0] == '\0')
	{
		this->len = 0;
		this->chars = NULL;
	}
	else
	{
		len = _tcsclen(chars);
		this->chars = new TCHAR[len + 1];
		_tcscpy_s(this->chars, len + 1, chars);
	}

	refcounter = new UINT();
	*refcounter = 1;
}
String::String(LPTSTR chars)
{
	if(!chars || chars[0] == '\0')
	{
		this->len = 0;
		this->chars = NULL;
	}
	else
	{
		len = _tcsclen(chars);
		this->chars = new TCHAR[len + 1];
		_tcscpy_s(this->chars, len + 1, chars);
	}

	refcounter = new UINT();
	*refcounter = 1;
}
String::String(LPCTSTR chars, size_t len)
{
	if(!chars || chars[0] == '\0' || !len)
	{
		this->len = 0;
		this->chars = NULL;
	}
	else
	{
		size_t len2 = _tcsclen(chars);
		len = min(len, len2);

		this->len = len;
		this->chars = new TCHAR[len + 1];
		memcpy(this->chars, chars, len * sizeof(TCHAR));
		this->chars[len] = '\0';
	}

	refcounter = new UINT();
	*refcounter = 1;
}
String::String(LPTSTR chars, size_t len)
{
	if(!chars || chars[0] == '\0' || !len)
	{
		this->len = 0;
		this->chars = NULL;
	}
	else
	{
		size_t len2 = _tcsclen(chars);
		len = min(len, len2);

		this->len = len;
		this->chars = new TCHAR[len + 1];
		memcpy(this->chars, chars, len * sizeof(TCHAR));
		this->chars[len] = '\0';
	}

	refcounter = new UINT();
	*refcounter = 1;
}
String::String(TCHAR chr)
{
	len = 1;
	this->chars = new TCHAR[2];
	this->chars[0] = chr;
	this->chars[1] = '\0';
	refcounter = new UINT();
	*refcounter = 1;
}
#ifdef UNICODE
String::String(LPCSTR chars)
{
	int len = strlen(chars);
	WCHAR buf = new WCHAR[len + 1];
	MultiByteToWideChar(CP_ACP, NULL, chars, -1, buf, len);
	//EDIT
}
String::String(LPSTR chars)
{
	int len = strlen(chars);
	WCHAR buf = new WCHAR[len + 1];
	MultiByteToWideChar(CP_ACP, NULL, chars, -1, buf, len);
	//EDIT
}
String::String(LPCSTR chars, size_t len)
{
	//EDIT
}
String::String(LPSTR chars, size_t len)
{
	//EDIT
}
String::String(char chr)
{
	//EDIT
}
#else
String::String(LPCWSTR chars)
{
	if(!chars || chars[0] == '\0')
	{
		this->len = 0;
		this->chars = NULL;
	}
	else
	{
		this->len = wcslen(chars);
		this->chars = new char[len + 1];
		WideCharToMultiByte(CP_ACP, NULL, chars, -1, this->chars, len, NULL, NULL);
	}

	refcounter = new UINT();
	*refcounter = 1;
}
String::String(LPWSTR chars)
{
	if(!chars || chars[0] == '\0')
	{
		this->len = 0;
		this->chars = NULL;
	}
	else
	{
		this->len = wcslen(chars);
		this->chars = new char[len + 1];
		WideCharToMultiByte(CP_ACP, NULL, chars, -1, this->chars, len + 1, NULL, NULL);
	}

	refcounter = new UINT();
	*refcounter = 1;
}
String::String(LPCWSTR chars, size_t len)
{
	if(!chars || chars[0] == '\0' || !len)
	{
		this->len = 0;
		this->chars = NULL;
	}
	else
	{
		size_t len2 = wcslen(chars);
		len = min(len, len2);

		this->chars = new char[len + 1];
		WideCharToMultiByte(CP_ACP, NULL, chars, -1, this->chars, len, NULL, NULL);
		this->chars[len] = '\0';
	}

	refcounter = new UINT();
	*refcounter = 1;
}
String::String(LPWSTR chars, size_t len)
{
	if(!chars || chars[0] == '\0' || !len)
	{
		this->len = 0;
		this->chars = NULL;
	}
	else
	{
		size_t len2 = wcslen(chars);
		len = min(len, len2);

		this->chars = new char[len + 1];
		WideCharToMultiByte(CP_ACP, NULL, chars, -1, this->chars, len, NULL, NULL);
		this->chars[len] = '\0';
	}

	refcounter = new UINT();
	*refcounter = 1;
}
String::String(WCHAR chr)
{
	len = 1;
	this->chars = new char[2];
	WideCharToMultiByte(CP_ACP, NULL, &chr, -1, this->chars, len, NULL, NULL);
	this->chars[1] = '\0';
	refcounter = new UINT();
	*refcounter = 1;
}
#endif
String::String(int num)
{
	TCHAR buf[16];
	_itot_s(num, buf, 10);

	len = _tcsclen(buf);
	chars = new TCHAR[len + 1];
	_tcscpy_s(chars, len + 1, buf);
	refcounter = new UINT();
	*refcounter = 1;
}
String::String(double num)
{
	TCHAR buf[16];

#ifdef UNICODE
	char cbuf[16];
	_gcvt_s(cbuf, num, 3);
	len = strlen(buf);
	MultiByteToWideChar(CP_ACP, NULL, cbuf, -1, buf, len);
#else
	_gcvt_s(buf, num, 3);
	len = _tcsclen(buf);
#endif

	chars = new TCHAR[len + 1];
	_tcscpy_s(chars, len + 1, buf);
	refcounter = new UINT();
	*refcounter = 1;
}
String::String()
{
	len = 0;
	this->chars = NULL;
	refcounter = new UINT();
	*refcounter = 1;
}
String String::StealReference(LPTSTR chars)
{
	String str;
	str.chars = chars;
	str.len = strlen(chars);
	return str;
}
void String::Clear()
{
	len = 0;
	REMOVE_ARRAY(this->chars);
}
String* String::Copy(String* result) const
{
	*result = String(chars);
	return result;
}

void String::RemoveReference()
{
	if(--(*refcounter) == 0)
	{
		REMOVE_ARRAY(this->chars);
		REMOVE(refcounter);
	}
}

String::~String()
{
	RemoveReference();
}