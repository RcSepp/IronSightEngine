#include "ISTypes.h"

//-----------------------------------------------------------------------------
// Name: Substring
// Desc: Return substring, starting at start
//-----------------------------------------------------------------------------
String* String::Substring(UINT start, String* result) const
{
	if(start < len)
		*result = String(chars + start);
	else
		*result = String();

	return result;
}

//-----------------------------------------------------------------------------
// Name: Substring
// Desc: Return substring, starting at start and with a length of length
//-----------------------------------------------------------------------------
String* String::Substring(UINT start, UINT length, String* result) const
{
	if(start < len)
	{
		if(start + length < len)
		{
			LPTSTR newchars = new TCHAR[length + 1];
			if(!newchars)
			{
				*result = String();
			}
			else
			{
				_tcsncpy_s(newchars, length + 1, chars + start, length);
				newchars[length] = '\0';
				*result = String(newchars);
				delete[] newchars;
			}
		}
		else
			*result = String(chars + start);
	}
	else
		*result = String();

	return result;
}

//-----------------------------------------------------------------------------
// Name: Replace
// Desc: Return string with all occurences of oldval replaced with newval
//-----------------------------------------------------------------------------
String* String::Replace(const String& oldval, const String& newval, String* result) const
{
	*result = this->chars;

	if(oldval.len > result->len)
		return result;

	for(size_t i = 0; i <= result->len - oldval.len; i++)
		if(_tcsncmp(result->chars + i, oldval.chars, oldval.len) == 0)
		{
			if(newval.len != oldval.len)
			{
				if(newval.len > oldval.len)
					result->chars = (LPTSTR)_realloc(result->chars, result->len + 1, result->len + 1 + newval.len - oldval.len); // +1 ... \0
				memmove(result->chars + i + newval.len, result->chars + i + oldval.len, (result->len - i - oldval.len + 1) * sizeof(TCHAR)); // +1 ... \0
				result->len += newval.len - oldval.len;
			}
			memcpy(result->chars + i, newval.chars, newval.len * sizeof(TCHAR));
			i += newval.len - 1;
		}

	return result;
}