#include "Direct3D.h"


//-----------------------------------------------------------------------------
// Name: D3DConsole()
// Desc: Constructor
//-----------------------------------------------------------------------------
D3DConsole::D3DConsole(UINT numlines, UINT historysize)
{
	spritecontainer = NULL;
	font = NULL;
	msghistory = NULL;
	visiblemsgs = NULL;
	historyfull = false;
	writemsgidx = (UINT)-1;
	fpstimer = 0.0f;
	framecounter = 0xFFFFFFFF;

	this->msghistorysize = historysize;
	this->numlines = numlines;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize D3DConsole
//-----------------------------------------------------------------------------
Result D3DConsole::Init(OutputWindow* wnd)
{
	Result rlt;

	this->wnd = wnd;
	if(!wnd)
		return ERR("Parameter wnd is NULL");

	if(msghistorysize == 0)
		msghistorysize = numlines; // History not in use

	if(numlines > msghistorysize)
		return ERR("The number of visible console messages cannot exceed the history size");
	if(numlines + msghistorysize < msghistorysize)
		return ERR("The number of visible console messages plus the history size may not exceed 0xFFFFFFFF (4294967295)");

	CHKALLOC(msghistory = new String[msghistorysize]);
	CHKALLOC(visiblemsgs = (String**)new LPVOID[numlines]);
	for(UINT i = 0; i < numlines; i++)
	{
		//msghistory[msghistorysize - numlines + i] = String("");
		visiblemsgs[i] = &msghistory[msghistorysize - numlines + i];
	}

	// Create sprite
	CHKRESULT(wnd->CreateSpriteContainer(0, &spritecontainer));


	// Create font
	CHKRESULT(wnd->CreateD3dFont(FontType("Verdana", 12, true), &font));


	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: PrintD3D()
// Desc: Output text to the D3D device
//-----------------------------------------------------------------------------
void D3DConsole::PrintD3D(const String& message)
{
	// Advance write pointer
	if(++writemsgidx >= msghistorysize)
	{
		writemsgidx -= msghistorysize;
		historyfull = true;
	}

	// Store new string
	msghistory[writemsgidx] = message;

	// Update visible lines
	ScrollTo(writemsgidx);
}
void D3DConsole::PrintD3D(int message)
{
	PrintD3D(String(message));
}
void D3DConsole::PrintD3D(double message)
{
	PrintD3D(String(message));
}

//-----------------------------------------------------------------------------
// Name: ScrollTo()
// Desc: Scroll visible messages to idx in the message history
//-----------------------------------------------------------------------------
void D3DConsole::ScrollTo(UINT idx)
{
	UINT tempidx;

	// idx must be within the history
	if(idx >= msghistorysize)
		return;

	if(historyfull)
	{
		// idx must be between GetTopScrollBound() and GetBottomScrollBound()
		if(idx > GetTopScrollBound() && idx < GetBottomScrollBound() + ((GetBottomScrollBound() > GetTopScrollBound()) ? 0 : msghistorysize))
			return;
	}
	else
	{
		// idx must be between GetBottomScrollBound() and GetTopScrollBound()
		if(idx < GetBottomScrollBound() || idx > GetTopScrollBound())
			return;
	}

	for(UINT i = 0; i < numlines; i++)
	{
		tempidx = idx - i;
		if(tempidx > idx) // UINT gone negative
			tempidx += msghistorysize;

		visiblemsgs[i] = &msghistory[tempidx];
	}
}

//-----------------------------------------------------------------------------
// Name: GetBottomScrollBound()
// Desc: Get lowest scrollable index
//-----------------------------------------------------------------------------
inline UINT D3DConsole::GetBottomScrollBound()
{
	return historyfull ? (writemsgidx + numlines) % msghistorysize : min(numlines - 1, writemsgidx);
}

//-----------------------------------------------------------------------------
// Name: GetTopScrollBound()
// Desc: Get highest scrollable index
//-----------------------------------------------------------------------------
inline UINT D3DConsole::GetTopScrollBound()
{
	return writemsgidx;
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Draw console messages to the device
//-----------------------------------------------------------------------------
void D3DConsole::Render(RenderType rendertype)
{
	spritecontainer->Begin();
	for(UINT y = 0; y < numlines; y++)
		if(!visiblemsgs[y]->IsEmpty())
			font->DrawTextImmediate(spritecontainer, *visiblemsgs[y], Rect<float>(7, (float)(wnd->GetBackbufferSize().height - (y + 1) * 20), 0, 0), 0xFF000000, ID3dFont::DF_NOCLIP);
	if(framecounter != 0xFFFFFFFF)
	{
		static char strfps[16];
		fpstimer += eng->time.dt;
		framecounter++;

		// Update frame count every second
		if(fpstimer >= 1.0f)
		{
			sprintf_s(strfps, "%d FPS", framecounter);
			framecounter = 0;
			fpstimer = 0;
		}

		font->DrawTextImmediate(spritecontainer, strfps, Rect<float>(7, 7, 0, 0), 0xFF000000, ID3dFont::DF_NOCLIP);
	}
	spritecontainer->End();
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void D3DConsole::Release()
{
	// Switch back to normal console
	cle = new Console();
	cle->target = this->target;

	RELEASE(spritecontainer);
	RELEASE(font);

	delete this;
}