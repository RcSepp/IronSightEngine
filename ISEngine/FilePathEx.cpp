#include "ISTypes.h"
#include <fstream>

char* GetLastSlash(char* str)
{
	char* sp = _tcsrchr(str, '/');
	char* bsp = _tcsrchr(str, '\\');
	return max(sp, bsp);
}

//-----------------------------------------------------------------------------
// Name: TrimExtension
// Desc: Trim filepath by ".*"
//-----------------------------------------------------------------------------
FilePath* FilePath::TrimExtension(FilePath* result) const
{
	LPTSTR ptpos = _tcsrchr(chars, '.');
	if(!ptpos)
	{
		*result = *this;
		return result;
	}

	result->len = ptpos - chars;
	result->chars = new TCHAR[result->len + 1];
	memcpy(result->chars, chars, (result->len + 1) * sizeof(TCHAR));
	result->chars[result->len] = '\0';
	return result;
}
void FilePath::TrimExtension()
{
	LPTSTR ptpos = _tcsrchr(chars, '.');
	if(!ptpos)
		return;

	size_t newlen = ptpos - chars;
	chars = (LPTSTR)_realloc(chars, len + 1, newlen + 1);
	len = newlen;
	chars[len] = '\0';
}

//-----------------------------------------------------------------------------
// Name: GetExtension
// Desc: Get filepath at the last '.'
//-----------------------------------------------------------------------------
String* FilePath::GetExtension(String* result) const
{
	LPTSTR ptpos = _tcsrchr(chars, '.');
	if(!ptpos)
		*result = FilePath();
	else
		*result = FilePath(ptpos + 1);

	return result;
}

//-----------------------------------------------------------------------------
// Name: AddSuffix
// Desc: Add the given string before the last '.'
//-----------------------------------------------------------------------------
FilePath* FilePath::AddSuffix(String suffix, FilePath* result) const
{
	String ext;
	this->GetExtension(&ext);

	*result = String(chars);
	result->TrimExtension();

	*result <<= suffix << FilePath('.') << ext;

	return result;
}

//-----------------------------------------------------------------------------
// Name: TrimDir
// Desc: Trim filepath by directory ("*\\")
//-----------------------------------------------------------------------------
FilePath* FilePath::TrimDir(FilePath* result) const
{
	// Run _tcsrchr() ignoring last character (for directories ending with '\\')
	TCHAR lastchar = chars[len - 1];
	chars[len - 1] = '\0';
	LPTSTR slpos = GetLastSlash(chars);
	chars[len - 1] = lastchar;

	if(!slpos)
		*result = *this;
	else
		*result = FilePath(slpos + 1);

	return result;
}

//-----------------------------------------------------------------------------
// Name: GetParentDir
// Desc: Get parent directory and return if there is any
//-----------------------------------------------------------------------------
bool FilePath::GetParentDir(FilePath* result) const
{
	// Run _tcsrchr() ignoring last character (for directories ending with '\\')
	TCHAR lastchar = chars[len - 1];
	chars[len - 1] = '\0';
	LPTSTR slpos = GetLastSlash(chars);
	chars[len - 1] = lastchar;

	if(!slpos)
	{
		*result = FilePath();
		return false;
	}

	slpos++; // Keep trailing slash

	result->chars = (LPTSTR)_realloc(result->chars, (result->len + 1) * sizeof(TCHAR), (slpos - chars + 1) * sizeof(TCHAR));
	result->len = slpos - chars;
	memcpy(result->chars, chars, result->len * sizeof(TCHAR));
	result->chars[result->len] = '\0';
	return true;
}
bool FilePath::GetParentDir()
{
	// Run _tcsrchr() ignoring last character (for directories ending with '\\')
	TCHAR lastchar = chars[len - 1];
	chars[len - 1] = '\0';
	LPTSTR slpos = GetLastSlash(chars);
	chars[len - 1] = lastchar;

	if(!slpos)
		return false;

	slpos++; // Keep trailing slash

	*slpos = '\0';
	len = slpos - chars;
	return true;
}

//-----------------------------------------------------------------------------
// Name: Exists
// Desc: Check if the filepath points to a file that exists and is readable
//-----------------------------------------------------------------------------
bool FilePath::Exists() const
{
	if(IsEmpty())
		return false;

	std::ifstream fs(chars);
	bool good = fs.good();
	fs.close();

	return good;
}

#ifdef _WIN32
//-----------------------------------------------------------------------------
// Name: GetFiles
// Desc: Get files and directories filtered by wanted and unwanted attributes
//-----------------------------------------------------------------------------
bool FilePath::GetFiles(FindHandle* hfind, DWORD wantedattr, DWORD unwantedattr, WIN32_FIND_DATA* fdata) const
{
	if(wantedattr == 0x0)
		wantedattr = 0xFFFFFFFF;

	if(hfind->h == NULL)
	{
		// Compose query string
		LPTSTR querystr;
		if(!chars) // Relative path; directory is empty
		{
			querystr = new TCHAR[2];
			querystr[0] = '*';
			querystr[1] = '\0';
		}
		else if(chars[len - 1] == '\\')
		{
			querystr = new TCHAR[len + 2];
			memcpy(querystr, chars, len * sizeof(TCHAR));
			querystr[len] = '*';
			querystr[len + 1] = '\0';
		}
		else
		{
			querystr = new TCHAR[len + 3];
			memcpy(querystr, chars, len * sizeof(TCHAR));
			querystr[len] = '\\';
			querystr[len + 1] = '*';
			querystr[len + 2] = '\0';
		}

		// Find first file
		hfind->h = FindFirstFile(querystr[0] == '\\' ? querystr + 1 : querystr, fdata);
		delete[] querystr;
		if(hfind->h == INVALID_HANDLE_VALUE)
		{
			hfind->h = NULL;
			return false;
		}

		if((fdata->dwFileAttributes & wantedattr) && !(fdata->dwFileAttributes & unwantedattr) &&
		   strcmp(fdata->cFileName, "..") != 0 && strcmp(fdata->cFileName, ".") != 0)
		   return true;
	}

	while(FindNextFile(hfind->h, fdata))
		if((fdata->dwFileAttributes & wantedattr) && !(fdata->dwFileAttributes & unwantedattr) &&
		   strcmp(fdata->cFileName, "..") != 0 && strcmp(fdata->cFileName, ".") != 0)
			return true;

	// Release finished find handle
	if(hfind->h == INVALID_HANDLE_VALUE)
		hfind->h = NULL;
	else
		hfind->Release();

	return false;
}

//-----------------------------------------------------------------------------
// Name: GetFiles
// Desc: Get files
//-----------------------------------------------------------------------------
bool FilePath::GetFiles(FindHandle* hfind, WIN32_FIND_DATA* fdata) const
{
	return GetFiles(hfind, NULL, FILE_ATTRIBUTE_DIRECTORY, fdata);
}

//-----------------------------------------------------------------------------
// Name: GetDirectories
// Desc: Get directories
//-----------------------------------------------------------------------------
bool FilePath::GetDirectories(FindHandle* hfind, WIN32_FIND_DATA* fdata) const
{
	return GetFiles(hfind, FILE_ATTRIBUTE_DIRECTORY, NULL, fdata);
}

bool RecursiveFileSearch(const FilePath &dir, WIN32_FIND_DATA &fdata, FilePath& file)
{
	FilePath::FindHandle hdirfind;
	while(dir.GetDirectories(&hdirfind, &fdata))
	{
		FilePath newdir(dir << fdata.cFileName << '\\');

		FilePath::FindHandle hfilefind;
		while(newdir.GetFiles(&hfilefind, &fdata))
			if(strcmp(fdata.cFileName, file) == 0)
			{
				file = newdir << file;
				return true;
			}

		if(RecursiveFileSearch(newdir, fdata, file))
			return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Name: Find
// Desc: Find file inside it's subdirectories
//-----------------------------------------------------------------------------
bool FilePath::Find(FilePath* result) const
{
	FilePath dir;
	if(!GetParentDir(&dir))
		dir = FilePath(); // Relative path; directory is empty
	TrimDir(result);

	// Search in original directory
	WIN32_FIND_DATA fdata;
	FilePath::FindHandle hfilefind;
	while(dir.GetFiles(&hfilefind, &fdata))
		if(strcmp(fdata.cFileName, *result) == 0)
			return true; // File found in the original directory

	// Search in subdirectories
	return RecursiveFileSearch(dir, fdata, *result);
}
bool FilePath::Find()
{
	FilePath dir, fname;
	if(!GetParentDir(&dir))
		dir = FilePath(); // Relative path; directory is empty
	TrimDir(&fname);

	// Search in original directory
	WIN32_FIND_DATA fdata;
	FilePath::FindHandle hfilefind;
	while(dir.GetFiles(&hfilefind, &fdata))
		if(strcmp(fdata.cFileName, fname) == 0)
			return true; // File found in the original directory

	// Search in subdirectories
	if(RecursiveFileSearch(dir, fdata, fname))
	{
		*this = fname;
		return true;
	}
	else
		return false; // File not found
}

//-----------------------------------------------------------------------------
// Name: GetLastWriteTime
// Desc: Check when the file was last modified
//-----------------------------------------------------------------------------
bool FilePath::GetLastWriteTime(ULARGE_INTEGER* time) const
{
	if(IsEmpty())
		return false;

	WIN32_FIND_DATA data;
	HANDLE findhdl = FindFirstFile(chars, &data);
	if(findhdl == INVALID_HANDLE_VALUE)
		return false;

	memcpy(time, &data.ftLastWriteTime, sizeof(ULARGE_INTEGER));

	FindClose(findhdl);
	return true;
}
bool FilePath::GetLastWriteTime(LPSYSTEMTIME time) const
{
	if(IsEmpty())
		return false;

	WIN32_FIND_DATA data;
	HANDLE findhdl = FindFirstFile(chars, &data);
	if(findhdl == INVALID_HANDLE_VALUE)
		return false;

	if(!FileTimeToSystemTime(&data.ftLastWriteTime, time))
	{
		FindClose(findhdl);
		return false;
	}

	FindClose(findhdl);
	return true;
}
#else
#include <sys/stat.h>
//-----------------------------------------------------------------------------
// Name: GetLastWriteTime
// Desc: Check when the file was last modified
//-----------------------------------------------------------------------------
bool FilePath::GetLastWriteTime(time_t* time) const
{
	if(IsEmpty())
		return false;

	struct stat fileattr;
	stat(chars, &fileattr);
	*time = fileattr.st_mtime;

	return true;
}
bool FilePath::GetLastWriteTime(struct tm** time) const
{
	if(IsEmpty())
		return false;

	struct stat fileattr;
	stat(chars, &fileattr);
	*time = localtime(&fileattr.st_mtime);

	return true;
}
#endif