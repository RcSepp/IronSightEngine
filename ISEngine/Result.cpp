#include "ISTypes.h"
#include <ISPythonScriptEngine.h>

#define REMOVE(ptr)				{if(ptr) {delete (ptr); (ptr) = NULL;}}

Result::Result()
{
	this->refcounter = new UINT();
	*this->refcounter = 1;

	this->details = NULL;
}
Result::Result(LPCTSTR msg)
{
	this->refcounter = new UINT();
	*this->refcounter = 1;

	if(msg)
	{
		this->details = new Details(String(msg));

		if(pse)
			pse->Throw(*this);
	}
	else
		this->details = NULL;
}
Result::Result(String& msg)
{
	this->refcounter = new UINT();
	*this->refcounter = 1;

	if(msg.IsEmpty())
		this->details = NULL;
	else
	{
		this->details = new Details(msg);

		if(pse)
			pse->Throw(*this);
	}
}
Result::Result(LPCTSTR msg, LPCTSTR file, LPCTSTR func, int line)
{
	this->refcounter = new UINT();
	*this->refcounter = 1;

	if(msg)
	{
		this->details = new Details(String(msg), String(file), String(func), line);

		if(pse)
			pse->Throw(*this);
	}
	else
		this->details = NULL;
}
Result::Result(String& msg, LPCTSTR file, LPCTSTR func, int line)
{
	this->refcounter = new UINT();
	*this->refcounter = 1;

	if(msg.IsEmpty())
		this->details = NULL;
	else
	{
		this->details = new Details(msg, String(file), String(func), line);
		this->details->msg = msg;
		this->details->file = String(file);
		this->details->func = String(func);
		this->details->line = line;

		if(pse)
			pse->Throw(*this);
	}
}
Result::Result(const Result& src)
{
	//this->refcounter = new UINT();
	//*this->refcounter = 1;
	this->refcounter = src.refcounter;
	(*this->refcounter)++;

	if(src.details)
	{
		if(src.details->line == -1)
			this->details = new Details(src.details->msg);
		else
			this->details = new Details(src.details->msg, src.details->file, src.details->func, src.details->line);
		//this->details->msg = src.details->msg; //src.details->msg.Copy(&this->details->msg);
		//this->details->file = src.details->file; //src.details->file.Copy(&this->details->file);
		//this->details->func = src.details->func; //src.details->func.Copy(&this->details->func);
		//this->details->line = src.details->line;
	}
	else
		this->details = NULL;
}

bool Result::operator==(const Result& r)
{
	if(!this->details)
		return !r.details;
	else if(!r.details)
		return false;
	else
		return this->details->msg == r.details->msg;
}

Result& Result::operator=(const Result& src)
{
	/*if(&src == this) // Seems silly
		return *this;*/

	this->RemoveReference();

	this->details = src.details;
	this->refcounter = src.refcounter;
	(*this->refcounter)++;

	return *this;
}

void Result::RemoveReference()
{
	if(--(*refcounter) == 0)
	{
		if(details && !details->iscaught)
		{
			if(details->line == -1)
				OutputDebugStringA("Uncaught exception");
			else
			{
				OutputDebugStringA("Uncaught exception at line ");
				OutputDebugStringA(String(details->line).ToCharString());
				OutputDebugStringA(" of ");
				OutputDebugStringA(details->file.ToCharString());
				OutputDebugStringA(" in ");
				OutputDebugStringA(details->func.ToCharString());
			}
			OutputDebugStringA(": ");
			OutputDebugStringA(details->msg.ToCharString());
			OutputDebugStringA("\n");
		}
		REMOVE(details)
		REMOVE(refcounter)
	}
}

Result::~Result()
{
	RemoveReference();
}

String Result::GetLastResult(bool msgonly) const
{
	String msg;

	if(!details)
		msg = "Result: Success";
	else
	{
		details->iscaught = true;
		if(details->msg.IsEmpty())
			msg = "Undefined error";
		else
			msg = details->msg;
	}

	if(details->line != -1 && !msgonly)
		return (String("Error at line ") << details->line <<
			   String(" of ") << details->file <<
			   String(":\n\n") << details->func <<
			   String("() returned\n") << msg);

	return msg;
}

Result Error(LPCTSTR msg, LPCTSTR file, LPCTSTR func, int line)
{
	return Result(msg, file, func, line);
}
Result Error(String msg, LPCTSTR file, LPCTSTR func, int line)
{
	return Result(msg, file, func, line);
}