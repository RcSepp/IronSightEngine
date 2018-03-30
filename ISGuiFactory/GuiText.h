#ifndef __GUITEXT_H
#define __GUITEXT_H

//-----------------------------------------------------------------------------
// Name: GuiTextFormat
// Desc: Wrapper class for an IDWriteTextFormat
//-----------------------------------------------------------------------------
class GuiTextFormat : public IGuiTextFormat, public IManagedObject
{
private:
	IDWriteTextFormat *fmt;

public:

	GuiTextFormat()
	{
		fmt = NULL;
	}

	Result Create(const String& family, float size, bool italic, GuiFontWeight weight, GuiFontStretch stretch);
	void Release();

	operator IDWriteTextFormat*() const {return fmt;}
};

#endif