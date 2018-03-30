#include "ISEngine.h"
#include <cstdio>


//-----------------------------------------------------------------------------
// Name: Console()
// Desc: Constructor
//-----------------------------------------------------------------------------
Console::Console() : target(CT_DEBUGSTREAM)
{
}

/*//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize Console
//-----------------------------------------------------------------------------
Result Console::Init(LPD3DDEVICE device, DWORD numlines, DWORD historysize)
{
}*/

//-----------------------------------------------------------------------------
// Name: Print()
// Desc: Output text to the standard output
//-----------------------------------------------------------------------------
void Console::Print(const char* format, ...)
{
	if(target == CT_STANDARDOUTPUT)
		printf(format);
	else
		OutputDebugString(format); //EDIT
}
void Console::Print(const String& message)
{
	if(target == CT_STANDARDOUTPUT)
		printf(message.ToCharString());
	else
		OutputDebugString(message.ToCharString());
}
void Console::Print(int message)
{
	Print(String(message));
}
void Console::Print(double message)
{
	Print(String(message));
}

//-----------------------------------------------------------------------------
// Name: ~Console()
// Desc: Destructor
//-----------------------------------------------------------------------------
Console::~Console()
{
}