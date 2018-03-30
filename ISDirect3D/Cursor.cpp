#include "Direct3D.h"


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize cursor
//-----------------------------------------------------------------------------
Result D3dCursor::Init(OutputWindow* wnd)
{
	Result rlt;

	this->wnd = wnd;
	if(!wnd)
		return ERR("Parameter wnd is NULL");

	// Create sprite container
	CHKRESULT(wnd->CreateSpriteContainer(1, &spritecontainer));
	spritecontainer->SetIdentityViewTransform();
	spritecontainer->SetIdentityProjectionTransform();

	ShowCursor(false);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Draw cursor to screen
//-----------------------------------------------------------------------------
void D3dCursor::Render(RenderType rendertype)
{
	if(!tex)
		return;

	POINT mousepos;
	GetCursorPos(&mousepos);
	ScreenToClient(wnd->GetHwnd(), &mousepos);
	Point<float> cursorpos((float)(mousepos.x - hotspot.x), (float)(mousepos.y - hotspot.y));
	const float bbw = (float)wnd->backbuffersize.width;
	const float bbh = (float)wnd->backbuffersize.height;
	const float tw = (float)tex->GetWidth();
	const float th = (float)tex->GetHeight();

	// Set texture
	sprtcursor.tex = tex;

	// Set texture subregion
	if(animated)
	{
float t = fmod(eng->time.t, 1.0f);
		int numframes = tex->GetWidth() / tex->GetHeight();
		float currentframe = floor(numframes * t);

		sprtcursor.texsize.x = th / tw;
		sprtcursor.texcoord.x = currentframe * sprtcursor.texsize.x;
	}
	else
	{
		sprtcursor.texcoord.x = 0.0f;
		sprtcursor.texsize.x = 1.0f;
	}

	// Set world matrix
	MatrixTranslation(&sprtcursor.worldmatrix, -1.0f + 2.0f * cursorpos.x / bbw, 1.0f - 2.0f * cursorpos.y / bbh, 0.0f);
	MatrixScale(&sprtcursor.worldmatrix, &sprtcursor.worldmatrix, 2.0f * sprtcursor.texsize.x * tw / bbw, 2.0f * th / bbh, 1.0f);
	MatrixTranslate(&sprtcursor.worldmatrix, &sprtcursor.worldmatrix, 0.5f, -0.5f, 0.0f);

	spritecontainer->Begin();
	spritecontainer->DrawSpriteImmediate(&sprtcursor);
	spritecontainer->End();
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void D3dCursor::Release()
{
	wnd->cursors.remove(this);
	if(wnd->cursors.empty())
		ShowCursor(true);

	RELEASE(spritecontainer);

	delete this;
}