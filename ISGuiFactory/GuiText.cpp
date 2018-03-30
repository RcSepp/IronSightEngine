#include "GuiFactory.h"

//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create D2D text format
//-----------------------------------------------------------------------------
Result GuiTextFormat::Create(const String& family, float size, bool italic, GuiFontWeight weight, GuiFontStretch stretch)
{
	Result rlt;

	CHKRESULT(dwtfactory->CreateTextFormat(family.ToWCharString(), NULL, (DWRITE_FONT_WEIGHT)weight,
							   italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
							   (DWRITE_FONT_STRETCH)stretch, size, L"en-us", &fmt));

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void GuiTextFormat::Release()
{
	Unregister();

	RELEASE(fmt);

	delete this;
}