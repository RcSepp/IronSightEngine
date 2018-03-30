#ifndef __ISGUIFACTORY_H
#define __ISGUIFACTORY_H

#include <ISEngine.h>
#ifdef _WIN32
#include <dxgi.h>
#endif
#include "ISMath.h"


class IGuiControl;
class ISliderControl;
struct Image; // Image from ISImage


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_GUIFACTORY
	#define GUIFACTORY_EXTERN_FUNC		__declspec(dllexport)
#else
	#define GUIFACTORY_EXTERN_FUNC		__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
typedef void (__stdcall* MOUSE_CALLBACK)(IGuiControl* sender, const MouseEventArgs& args, LPVOID user);
typedef void (__stdcall* VALUECHANGED_CALLBACK)(ISliderControl* sender, float newvalue, LPVOID user);
typedef void (__stdcall* RESIZERENDERTARGET_CALLBACK)(Size<UINT>& oldsize, Size<UINT>& newsize, LPVOID user);


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum GuiFontWeight
{
	GFW_THIN          = 100,
	GFW_EXTRA_LIGHT   = 200,
	GFW_ULTRA_LIGHT   = 200,
	GFW_LIGHT         = 300,
	GFW_NORMAL        = 400,
	GFW_REGULAR       = 400,
	GFW_MEDIUM        = 500,
	GFW_DEMI_BOLD     = 600,
	GFW_SEMI_BOLD     = 600,
	GFW_BOLD          = 700,
	GFW_EXTRA_BOLD    = 800,
	GFW_ULTRA_BOLD    = 800,
	GFW_BLACK         = 900,
	GFW_HEAVY         = 900,
	GFW_EXTRA_BLACK   = 950,
	GFW_ULTRA_BLACK   = 950
};
enum GuiFontStretch
{
	GFS_UNDEFINED         = 0,
	GFS_ULTRA_CONDENSED   = 1,
	GFS_EXTRA_CONDENSED   = 2,
	GFS_CONDENSED         = 3,
	GFS_SEMI_CONDENSED    = 4,
	GFS_NORMAL            = 5,
	GFS_MEDIUM            = 5,
	GFS_SEMI_EXPANDED     = 6,
	GFS_EXPANDED          = 7,
	GFS_EXTRA_EXPANDED    = 8,
	GFS_ULTRA_EXPANDED    = 9
};
enum GuiTextAllignment
{
	GTA_LEADING  = 0,
	GTA_TRAILING = 1,
	GTA_CENTER   = 2
};


//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
/*struct Color
{
    float r, g, b, a;

	Color() {};
	Color(float r, float g, float b, float a = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
};*/

struct GradientStop
{
    float pos;
    const Color clr;
	GradientStop(float pos, const Color& clr) : pos(pos), clr(clr) {}
};

namespace TransitionScript
{
	enum Type
	{
		T_Unset,
		T_Instantaneous,
		T_Linear,
		T_AccelerateDecelerate
	};

	struct IScript
	{
		const Type type;
		double dest;

		IScript(Type type) : type(type) {}
		static IScript* Copy(const IScript& other);
	};
	struct Instantaneous : public IScript
	{
		Instantaneous(double dest) : IScript(T_Instantaneous) {this->dest = dest;}
	};
	struct Linear : public IScript
	{
		double duration;
		Linear(double dest, double duration) : IScript(T_Linear), duration(duration) {this->dest = dest;}
	};
	struct AccelerateDecelerate : public IScript
	{
		double duration, accratio, decratio;
		AccelerateDecelerate(double dest, double duration, double accratio, double decratio) : IScript(T_AccelerateDecelerate), duration(duration), accratio(accratio), decratio(decratio) {this->dest = dest;}
	};
}

/*struct TransitionDescription
{
    TransitionScript::Type type;
	TransitionScript::IScript script;

	TransitionDescription() : type(TransitionScript::T_Unset) {}
	TransitionDescription(TransitionScript::Type type, TransitionScript::IScript& script) : type(type), script(script) {}
};*/


/*//-----------------------------------------------------------------------------
// Name: IGuiMouseRegion
// Desc: Interface to a GuiMouseRegion
//-----------------------------------------------------------------------------
typedef class IGuiMouseRegion
{
public:
	virtual void SetMouseDownCallback(const MOUSE_CALLBACK downcbk, LPVOID user) = 0;
	virtual void SetMouseUpCallback(const MOUSE_CALLBACK upcbk, LPVOID user) = 0;
	virtual void SetMouseMoveCallback(const MOUSE_CALLBACK movecbk, LPVOID user) = 0;
	virtual void SetMouseEnterCallback(const MOUSE_CALLBACK entercbk, LPVOID user) = 0;
	virtual void SetMouseLeaveCallback(const MOUSE_CALLBACK leavecbk, LPVOID user) = 0;

	virtual void AddRectangle(const Rect<float>& bounds, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;
	void AddRectangle(const Point<float>& pos, float width, float height, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{AddRectangle(Rect<float>(pos.x, pos.y, width, height), dock, refdock);}
	void AddRectangle(float x, float y, float width, float height, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{AddRectangle(Rect<float>(x, y, width, height), dock, refdock);}

	virtual void AddEllipse(const Point<float>& center, const Point<float>& radius, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;
	void AddEllipse(const Point<float>& center, float rx, float ry, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{AddEllipse(center, Point<float>(rx, ry), dock, refdock);}
	void AddEllipse(float x, float y, float rx, float ry, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{AddEllipse(Point<float>(x, y), Point<float>(rx, ry), dock, refdock);}
}* LPGUIMOUSEREGION;*/

//-----------------------------------------------------------------------------
// Name: IGuiTextFormat
// Desc: Interface to a IDWriteTextFormat wrapper class
//-----------------------------------------------------------------------------
typedef class IGuiTextFormat
{
public:
}* LPGUITEXTFORMAT;

//-----------------------------------------------------------------------------
// Name: IGuiBrush
// Desc: Interface to a ID2D1Brush wrapper class
//-----------------------------------------------------------------------------
typedef class IGuiBrush
{
public:
}* LPGUIBRUSH;

//-----------------------------------------------------------------------------
// Name: IGuiShape
// Desc: Interface to the Shape struct
//-----------------------------------------------------------------------------
typedef struct IGuiShape
{
public:
	virtual Rect<float>& GetBounds() = 0;
	virtual void SetBounds(Rect<float>& val) = 0;

	virtual Point<float>& GetLocation() = 0;
	virtual void SetLocation(const Point<float>& val) = 0;
	virtual float GetLeft() = 0;
	virtual void SetLeft(float val) = 0;
	virtual float GetTop() = 0;
	virtual void SetTop(float val) = 0;

	virtual ::Size<float>& GetSize() = 0;
	virtual void SetSize(const ::Size<float>& val) = 0;
	virtual float GetWidth() = 0;
	virtual void SetWidth(float val) = 0;
	virtual float GetHeight() = 0;
	virtual void SetHeight(float val) = 0;

	virtual DockType GetDock() = 0;
	virtual void SetDock(DockType val) = 0;

	virtual LPGUIBRUSH GetBrush() = 0;
	virtual LPGUIBRUSH SetBrush(LPGUIBRUSH val) = 0;

	virtual Result MeasureText(Rect<float>* bounds) = 0;

#ifdef _WIN32
	__declspec(property(get=GetBounds, put=SetBounds)) Rect<float>& Bounds;
	__declspec(property(get=GetLocation, put=SetLocation)) Point<float> Location;
	__declspec(property(get=GetLeft, put=SetLeft)) float Left;
	__declspec(property(get=GetTop, put=SetTop)) float Top;
	__declspec(property(get=GetSize, put=SetSize)) ::Size<float>& Size;
	__declspec(property(get=GetWidth, put=SetWidth)) float Width;
	__declspec(property(get=GetHeight, put=SetHeight)) float Height;
	__declspec(property(get=GetDock, put=SetDock)) DockType Dock;
	__declspec(property(get=GetBrush, put=SetBrush)) LPGUIBRUSH Brush;
#endif
}* LPGUISHAPE;

//-----------------------------------------------------------------------------
// Name: IGuiBitmap
// Desc: Interface to a ID2D1Bitmap wrapper class
//-----------------------------------------------------------------------------
typedef class IGuiBitmap
{
public:
	virtual ::Size<float>& GetSize() = 0;
	virtual ::Size<float>& SetSize(const ::Size<float>& val) = 0;
	virtual float GetWidth() = 0;
	virtual void SetWidth(float val) = 0;
	virtual float GetHeight() = 0;
	virtual void SetHeight(float val) = 0;

#ifdef _WIN32
	__declspec(property(get=GetSize, put=SetSize)) ::Size<float>& Size;
	__declspec(property(get=GetWidth, put=SetWidth)) float Width;
	__declspec(property(get=GetHeight, put=SetHeight)) float Height;
#endif
	virtual void Release() = 0;
}* LPGUIBITMAP;

//-----------------------------------------------------------------------------
// Name: IGuiCompositeImage
// Desc: Interface to an image, made up of shapes
//-----------------------------------------------------------------------------
typedef class IGuiCompositeImage
{
public:
	virtual void AddFilledRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius = Point<float>(0.0f, 0.0f),
									DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL) = 0;
	void AddFilledRectangle(IGuiBrush* brush, const Point<float>& pos, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
							DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddFilledRectangle(brush, Rect<float>(pos.x, pos.y, width, height), radius, dock, refdock, shape);}
	void AddFilledRectangle(IGuiBrush* brush, float x, float y, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
							DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddFilledRectangle(brush, Rect<float>(x, y, width, height), radius, dock, refdock, shape);}

	virtual void AddDrawnRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius = Point<float>(0.0f, 0.0f),
								   DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL) = 0;
	void AddDrawnRectangle(IGuiBrush* brush, const Point<float>& pos, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
						   DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddDrawnRectangle(brush, Rect<float>(pos.x, pos.y, width, height), radius, dock, refdock, shape);}
	void AddDrawnRectangle(IGuiBrush* brush, float x, float y, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
						   DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddDrawnRectangle(brush, Rect<float>(x, y, width, height), radius, dock, refdock, shape);}

	virtual void AddFilledEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock = DT_TOPLEFT,
								  DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL) = 0;
	void AddFilledEllipse(IGuiBrush* brush, const Point<float>& center, float rx, float ry, DockType dock = DT_TOPLEFT,
						  DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddFilledEllipse(brush, center, Point<float>(rx, ry), dock, refdock, shape);}
	void AddFilledEllipse(IGuiBrush* brush, float x, float y, float rx, float ry, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddFilledEllipse(brush, Point<float>(x, y), Point<float>(rx, ry), dock, refdock, shape);}

	virtual void AddDrawnEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock = DT_TOPLEFT,
								 DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL) = 0;
	void AddDrawnEllipse(IGuiBrush* brush, const Point<float>& center, float rx, float ry, DockType dock = DT_TOPLEFT,
						 DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddDrawnEllipse(brush, center, Point<float>(rx, ry), dock, refdock, shape);}
	void AddDrawnEllipse(IGuiBrush* brush, float x, float y, float rx, float ry, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddDrawnEllipse(brush, Point<float>(x, y), Point<float>(rx, ry), dock, refdock, shape);}

	virtual void AddDrawnPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed = true, float strokewidth = 1.0f,
								 DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL) = 0;

	virtual void AddFilledPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed = true,
								 DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL) = 0;

	virtual void AddFilledGeometry(IGuiBrush* brush, const Point<float>* vertices, size_t numtriangles,
								 DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL) = 0;

	virtual void AddLine(IGuiBrush* brush, const Point<float>& p0, const Point<float>& p1, float strokewidth = 1.0f, DockType dock = DT_TOPLEFT,
						 DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL) = 0;
	void AddLine(IGuiBrush* brush, float x0, float y0, float x1, float y1, float strokewidth = 1.0f, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddLine(brush, Point<float>(x0, y0), Point<float>(x1, y1), strokewidth, dock, refdock, shape);}

	virtual void AddBitmap(IGuiBitmap* bmp, const Rect<float>& bounds, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL) = 0;
	void AddBitmap(IGuiBitmap* bmp, const Point<float>& pos, float width, float height, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddBitmap(bmp, Rect<float>(pos.x, pos.y, width, height), dock, refdock, shape);}
	void AddBitmap(IGuiBitmap* bmp, float x, float y, float width, float height, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddBitmap(bmp, Rect<float>(x, y, width, height), dock, refdock, shape);}

	virtual void AddTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Rect<float>& bounds, GuiTextAllignment allignment = GTA_LEADING, DockType dock = DT_TOPLEFT,
							  DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL) = 0;
	void AddTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Point<float>& pos, float width, float height, GuiTextAllignment allignment = GTA_LEADING,
					  DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddTextField(format, text, brush, Rect<float>(pos.x, pos.y, width, height), allignment, dock, refdock, shape);}
	void AddTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, float x, float y, float width, float height, GuiTextAllignment allignment = GTA_LEADING,
					  DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** shape = NULL)
		{AddTextField(format, text, brush, Rect<float>(x, y, width, height), allignment, dock, refdock, shape);}

	virtual ::Size<float>& GetSize() = 0;
	virtual ::Size<float>& SetSize(const ::Size<float>& val) = 0;
	virtual float GetWidth() = 0;
	virtual void SetWidth(float val) = 0;
	virtual float GetHeight() = 0;
	virtual void SetHeight(float val) = 0;

	virtual Result Recreate() = 0;

#ifdef _WIN32
	__declspec(property(get=GetSize, put=SetSize)) ::Size<float>& Size;
	__declspec(property(get=GetWidth, put=SetWidth)) float Width;
	__declspec(property(get=GetHeight, put=SetHeight)) float Height;
#endif
}* LPGUICOMPOSITEIMAGE;

//-----------------------------------------------------------------------------
// Name: ISliderControl
// Desc: Interface to a SliderControl
//-----------------------------------------------------------------------------
typedef class ISliderControl
{
public:
	virtual bool GetEnableSnapBack() = 0;
	virtual void SetEnableSnapBack(bool val) = 0;
	virtual float GetStartpos() = 0;
	virtual void SetStartpos(float val) = 0;
	virtual float GetEndpos() = 0;
	virtual void SetEndpos(float val) = 0;
	virtual float GetMinimum() = 0;
	virtual void SetMinimum(float val) = 0;
	virtual float GetMaximum() = 0;
	virtual void SetMaximum(float val) = 0;

	virtual void SetValue(float newvalue) = 0;
	virtual void SetValueChangedCallback(const VALUECHANGED_CALLBACK vchangecbk, LPVOID user) = 0;
	virtual void SetScrollCallback(const VALUECHANGED_CALLBACK scrollcbk, LPVOID user) = 0;

#ifdef _WIN32
	__declspec(property(get=GetEnableSnapBack, put=SetEnableSnapBack)) bool EnableSnapBack;
	__declspec(property(get=GetStartpos, put=SetStartpos)) float Startpos;
	__declspec(property(get=GetEndpos, put=SetEndpos)) float Endpos;
	__declspec(property(get=GetMinimum, put=SetMinimum)) float Minimum;
	__declspec(property(get=GetMaximum, put=SetMaximum)) float Maximum;
#endif
}* LPSLIDERCONTROL;

//-----------------------------------------------------------------------------
// Name: IGuiControl
// Desc: Interface to a GuiFactory control
//-----------------------------------------------------------------------------
typedef class IGuiControl
{
public:
	virtual void SetMouseDownCallback(const MOUSE_CALLBACK downcbk, LPVOID user) = 0;
	virtual void SetMouseUpCallback(const MOUSE_CALLBACK upcbk, LPVOID user) = 0;
	virtual void SetMouseMoveCallback(const MOUSE_CALLBACK movecbk, LPVOID user) = 0;
	virtual void SetMouseEnterCallback(const MOUSE_CALLBACK entercbk, LPVOID user) = 0;
	virtual void SetMouseLeaveCallback(const MOUSE_CALLBACK leavecbk, LPVOID user) = 0;

	virtual void SetMouseOverEffect(LPGUIBITMAP effectimage, LPGUIBRUSH effectbrush, TransitionScript::IScript& desc) = 0;
	virtual void SetMouseOverEffect(LPGUICOMPOSITEIMAGE effectimage, LPGUIBRUSH effectbrush, TransitionScript::IScript& desc) = 0;
	virtual void SetMouseDownEffect(LPGUIBITMAP effectimage, LPGUIBRUSH effectbrush, TransitionScript::IScript& desc) = 0;
	virtual void SetMouseDownEffect(LPGUICOMPOSITEIMAGE effectimage, LPGUIBRUSH effectbrush, TransitionScript::IScript& desc) = 0;

	virtual void CreateTextField(IGuiTextFormat* format, IGuiBrush* brush, const Rect<float>& bounds, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;
	void CreateTextField(IGuiTextFormat* format, IGuiBrush* brush, const Point<float>& pos, float width, float height, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{CreateTextField(format, brush, Rect<float>(pos.x, pos.y, width, height), dock, refdock);}
	void CreateTextField(IGuiTextFormat* format, IGuiBrush* brush, float x, float y, float width, float height,
						 DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{CreateTextField(format, brush, Rect<float>(x, y, width, height), dock, refdock);}

	virtual void BringToFront() = 0;

	virtual Point<float> GetLocation() const = 0;
	virtual void SetLocation(const Point<float>& val) = 0;
	virtual float GetLeft() const = 0;
	virtual void SetLeft(float val) = 0;
	virtual float GetTop() const = 0;
	virtual void SetTop(float val) = 0;

	virtual ::Size<float> GetSize() const = 0;
	virtual void SetSize(const ::Size<float>& val) = 0;
	virtual float GetWidth() const = 0;
	virtual void SetWidth(float val) = 0;
	virtual float GetHeight() const = 0;
	virtual void SetHeight(float val) = 0;

	virtual DockType GetDock() const = 0;
	virtual void SetDock(DockType val) = 0;
	virtual DockType GetRefDock() const = 0;
	virtual void SetRefDock(DockType val) = 0;

	virtual void SetImage(const LPGUIBITMAP img) = 0;
	virtual LPGUIBITMAP GetBitmapImage() = 0;
	virtual void SetImage(const LPGUICOMPOSITEIMAGE val) = 0;
	virtual LPGUICOMPOSITEIMAGE GetCompositeImage() = 0;

	virtual float GetAlpha() const = 0;
	virtual void SetAlpha(float val) = 0;

	virtual String& GetText() = 0;
	virtual void SetText(const String& val) = 0;
	virtual ::Size<float> GetTextSize() const = 0;
	virtual const ::Size<float>& SetTextSize(const ::Size<float>& val) = 0;
	virtual float GetTextWidth() const = 0;
	virtual float SetTextWidth(float val) = 0;
	virtual float GetTextHeight() const = 0;
	virtual float SetTextHeight(float val) = 0;
	virtual Result MeasureText(Rect<float>* bounds) = 0;
	virtual Result FitTextSizeToText() = 0;

	virtual LPGUIBRUSH GetTextBrush() const = 0;
	virtual void SetTextBrush(const LPGUIBRUSH val) = 0;

	virtual void Release() = 0;

#ifdef _WIN32
	__declspec(property(get=GetLocation, put=SetLocation)) Point<float> Location;
	__declspec(property(get=GetLeft, put=SetLeft)) float Left;
	__declspec(property(get=GetTop, put=SetTop)) float Top;
	__declspec(property(get=GetSize, put=SetSize)) ::Size<float>& Size;
	__declspec(property(get=GetWidth, put=SetWidth)) float Width;
	__declspec(property(get=GetHeight, put=SetHeight)) float Height;
	__declspec(property(get=GetDock, put=SetDock)) DockType Dock;
	__declspec(property(get=GetRefDock, put=SetRefDock)) DockType RefDock;
	__declspec(property(get=GetAlpha, put=SetAlpha)) float Alpha;
	__declspec(property(get=GetText, put=SetText)) String& Text;
	__declspec(property(get=GetTextSize, put=SetTextSize)) ::Size<float>& TextSize;
	__declspec(property(get=GetTextWidth, put=SetTextWidth)) float TextWidth;
	__declspec(property(get=GetTextHeight, put=SetTextHeight)) float TextHeight;
	__declspec(property(get=GetTextBrush, put=SetTextBrush)) LPGUIBRUSH Text;
#endif
}* LPGUICONTROL;

//-----------------------------------------------------------------------------
// Name: IGuiRenderArea
// Desc: Interface to the ClippedRenderArea class
//-----------------------------------------------------------------------------
typedef class IGuiRenderArea
{
public:

	virtual Point<float>& GetLocation() = 0;
	virtual void SetLocation(const Point<float>& val) = 0;
	virtual float GetLeft() = 0;
	virtual void SetLeft(float val) = 0;
	virtual float GetTop() = 0;
	virtual void SetTop(float val) = 0;

	virtual ::Size<float>& GetSize() = 0;
	virtual void SetSize(const ::Size<float>& val) = 0;
	virtual float GetWidth() = 0;
	virtual void SetWidth(float val) = 0;
	virtual float GetHeight() = 0;
	virtual void SetHeight(float val) = 0;

	virtual Result CreateControl(DockType dock, DockType refdock, IGuiControl** ctrl) = 0;
	Result CreateControl(IGuiControl** ctrl) {return CreateControl(DT_TOPLEFT, DT_TOPLEFT, ctrl);}
	virtual void RemoveControl(IGuiControl* ctrl) = 0;
	virtual void RemoveAllControls() = 0;

	virtual void FillRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius = Point<float>(0.0f, 0.0f),
									DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;
	void FillRectangle(IGuiBrush* brush, const Point<float>& pos, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
							DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{FillRectangle(brush, Rect<float>(pos.x, pos.y, width, height), radius, dock, refdock);}
	void FillRectangle(IGuiBrush* brush, float x, float y, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
							DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{FillRectangle(brush, Rect<float>(x, y, width, height), radius, dock, refdock);}

	virtual void DrawRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius = Point<float>(0.0f, 0.0f),
								   DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;
	void DrawRectangle(IGuiBrush* brush, const Point<float>& pos, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
						   DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawRectangle(brush, Rect<float>(pos.x, pos.y, width, height), radius, dock, refdock);}
	void DrawRectangle(IGuiBrush* brush, float x, float y, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
						   DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawRectangle(brush, Rect<float>(x, y, width, height), radius, dock, refdock);}

	virtual void FillEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock = DT_TOPLEFT,
								  DockType refdock = DT_TOPLEFT) = 0;
	void FillEllipse(IGuiBrush* brush, const Point<float>& center, float rx, float ry, DockType dock = DT_TOPLEFT,
						  DockType refdock = DT_TOPLEFT)
		{FillEllipse(brush, center, Point<float>(rx, ry), dock, refdock);}
	void FillEllipse(IGuiBrush* brush, float x, float y, float rx, float ry, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{FillEllipse(brush, Point<float>(x, y), Point<float>(rx, ry), dock, refdock);}

	virtual void DrawEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock = DT_TOPLEFT,
								 DockType refdock = DT_TOPLEFT) = 0;
	void DrawEllipse(IGuiBrush* brush, const Point<float>& center, float rx, float ry, DockType dock = DT_TOPLEFT,
						 DockType refdock = DT_TOPLEFT)
		{DrawEllipse(brush, center, Point<float>(rx, ry), dock, refdock);}
	void DrawEllipse(IGuiBrush* brush, float x, float y, float rx, float ry, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawEllipse(brush, Point<float>(x, y), Point<float>(rx, ry), dock, refdock);}

	virtual void DrawPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed = true, float strokewidth = 1.0f,
								 DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;

	virtual void FillPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed = true,
								 DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;

	virtual void DrawLine(IGuiBrush* brush, const Point<float>& p0, const Point<float>& p1, float strokewidth = 1.0f, DockType dock = DT_TOPLEFT,
						 DockType refdock = DT_TOPLEFT) = 0;
	void DrawLine(IGuiBrush* brush, float x0, float y0, float x1, float y1, float strokewidth = 1.0f, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawLine(brush, Point<float>(x0, y0), Point<float>(x1, y1), strokewidth, dock, refdock);}

	virtual void DrawBitmap(IGuiBitmap* bmp, const Rect<float>& bounds, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;
	void DrawBitmap(IGuiBitmap* bmp, const Point<float>& pos, float width, float height, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawBitmap(bmp, Rect<float>(pos.x, pos.y, width, height), dock, refdock);}
	void DrawBitmap(IGuiBitmap* bmp, float x, float y, float width, float height, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawBitmap(bmp, Rect<float>(x, y, width, height), dock, refdock);}

	virtual void DrawTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Rect<float>& bounds, DockType dock = DT_TOPLEFT,
							  DockType refdock = DT_TOPLEFT, IGuiShape** primitive = NULL) = 0;
	void DrawTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Point<float>& pos, float width, float height,
					  DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** primitive = NULL)
		{DrawTextField(format, text, brush, Rect<float>(pos.x, pos.y, width, height), dock, refdock, primitive);}
	void DrawTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, float x, float y, float width, float height,
					  DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** primitive = NULL)
		{DrawTextField(format, text, brush, Rect<float>(x, y, width, height), dock, refdock, primitive);}
	virtual void ClearPrimitives() = 0;

#ifdef _WIN32
	__declspec(property(get=GetLocation, put=SetLocation)) Point<float> Location;
	__declspec(property(get=GetLeft, put=SetLeft)) float Left;
	__declspec(property(get=GetTop, put=SetTop)) float Top;
	__declspec(property(get=GetSize, put=SetSize)) ::Size<float>& Size;
	__declspec(property(get=GetWidth, put=SetWidth)) float Width;
	__declspec(property(get=GetHeight, put=SetHeight)) float Height;
#endif
}* LPGUIRENDERAREA;

//-----------------------------------------------------------------------------
// Name: IGuiAnimation
// Desc: Interface to the GuiAnimation class
//-----------------------------------------------------------------------------
typedef class IGuiAnimation
{
public:

	virtual Result AddMove(LPGUICONTROL ctrl, const Point<float>& destpos, double acceleration) = 0;
	Result AddMove(LPGUICONTROL ctrl, float destx, float desty, double acceleration)
		{return  AddMove(ctrl, Point<float>(destx, desty), acceleration);}
	virtual Result AddBlend(LPGUICONTROL ctrl, float destalpha, double duration) = 0;
	virtual Result Schedule() = 0;
}* LPGUIANIMATION;

/*//-----------------------------------------------------------------------------
// Name: IGuiDirectContent
// Desc: Interface to the GuiDirectContent class
//-----------------------------------------------------------------------------
typedef class IGuiDirectContent
{
public:

	virtual bool GetVisible() = 0;
	virtual void SetVisible(bool val) = 0;
	__declspec(property(get=GetVisible, put=SetVisible)) bool Height;

	virtual Result PrepareContent(Size<UINT>& srcsize) = 0;
	virtual Result SetContent(byte* imgdata) = 0;
	virtual void Release() = 0;
}* LPGUIDIRECTCONTENT;*/

//-----------------------------------------------------------------------------
// Name: IGuiOutputWindow
// Desc: Interface to the GuiOutputWindow class
//-----------------------------------------------------------------------------
typedef class IGuiOutputWindow
{
public:
	LPGUIBRUSH bgbrush;

	virtual HWND GetTargetWnd() = 0;
	virtual void Invalidate() = 0;

	virtual Point<float> GetScale() const = 0;
	virtual void SetScale(const Point<float>& val) = 0;

	virtual Point<float> GetTranslation() const = 0;
	virtual void SetTranslation(const Point<float>& val) = 0;

	virtual Point<float> GetContentPadding() const = 0;
	virtual void SetContentPadding(const Point<float>& val) = 0;

	virtual Result TakeScreenshot(const FilePath& filename) = 0;
	virtual Result TakeScreenshot(BYTE** imgbuffer, UINT* imgoffset, UINT* width = NULL, UINT* height = NULL) = 0;
	virtual Result CreateRenderArea(Rect<float>& bounds, bool antialiased, IGuiRenderArea** area) = 0;
	virtual Result CreateControl(DockType dock, DockType refdock, IGuiControl** ctrl) = 0;
	Result CreateControl(IGuiControl** ctrl) {return CreateControl(DT_TOPLEFT, DT_TOPLEFT, ctrl);}
	virtual void RemoveControl(IGuiControl* ctrl) = 0;
	virtual void RemoveAllControls() = 0;

	virtual void FillRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius = Point<float>(0.0f, 0.0f),
									DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;
	void FillRectangle(IGuiBrush* brush, const Point<float>& pos, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
							DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{FillRectangle(brush, Rect<float>(pos.x, pos.y, width, height), radius, dock, refdock);}
	void FillRectangle(IGuiBrush* brush, float x, float y, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
							DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{FillRectangle(brush, Rect<float>(x, y, width, height), radius, dock, refdock);}

	virtual void DrawRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius = Point<float>(0.0f, 0.0f),
								   DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;
	void DrawRectangle(IGuiBrush* brush, const Point<float>& pos, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
						   DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawRectangle(brush, Rect<float>(pos.x, pos.y, width, height), radius, dock, refdock);}
	void DrawRectangle(IGuiBrush* brush, float x, float y, float width, float height, const Point<float>& radius = Point<float>(0.0f, 0.0f),
						   DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawRectangle(brush, Rect<float>(x, y, width, height), radius, dock, refdock);}

	virtual void FillEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock = DT_TOPLEFT,
								  DockType refdock = DT_TOPLEFT) = 0;
	void FillEllipse(IGuiBrush* brush, const Point<float>& center, float rx, float ry, DockType dock = DT_TOPLEFT,
						  DockType refdock = DT_TOPLEFT)
		{FillEllipse(brush, center, Point<float>(rx, ry), dock, refdock);}
	void FillEllipse(IGuiBrush* brush, float x, float y, float rx, float ry, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{FillEllipse(brush, Point<float>(x, y), Point<float>(rx, ry), dock, refdock);}

	virtual void DrawEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock = DT_TOPLEFT,
								 DockType refdock = DT_TOPLEFT) = 0;
	void DrawEllipse(IGuiBrush* brush, const Point<float>& center, float rx, float ry, DockType dock = DT_TOPLEFT,
						 DockType refdock = DT_TOPLEFT)
		{DrawEllipse(brush, center, Point<float>(rx, ry), dock, refdock);}
	void DrawEllipse(IGuiBrush* brush, float x, float y, float rx, float ry, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawEllipse(brush, Point<float>(x, y), Point<float>(rx, ry), dock, refdock);}

	virtual void DrawPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed = true, float strokewidth = 1.0f,
								 DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;

	virtual void FillPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed = true,
								 DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;

	virtual void DrawLine(IGuiBrush* brush, const Point<float>& p0, const Point<float>& p1, float strokewidth = 1.0f, DockType dock = DT_TOPLEFT,
						 DockType refdock = DT_TOPLEFT) = 0;
	void DrawLine(IGuiBrush* brush, float x0, float y0, float x1, float y1, float strokewidth = 1.0f, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawLine(brush, Point<float>(x0, y0), Point<float>(x1, y1), strokewidth, dock, refdock);}

	virtual void DrawBitmap(IGuiBitmap* bmp, const Rect<float>& bounds, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT) = 0;
	void DrawBitmap(IGuiBitmap* bmp, const Point<float>& pos, float width, float height, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawBitmap(bmp, Rect<float>(pos.x, pos.y, width, height), dock, refdock);}
	void DrawBitmap(IGuiBitmap* bmp, float x, float y, float width, float height, DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT)
		{DrawBitmap(bmp, Rect<float>(x, y, width, height), dock, refdock);}

	virtual void DrawTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Rect<float>& bounds, DockType dock = DT_TOPLEFT,
							  DockType refdock = DT_TOPLEFT, IGuiShape** primitive = NULL) = 0;
	void DrawTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Point<float>& pos, float width, float height,
					  DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** primitive = NULL)
		{DrawTextField(format, text, brush, Rect<float>(pos.x, pos.y, width, height), dock, refdock, primitive);}
	void DrawTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, float x, float y, float width, float height,
					  DockType dock = DT_TOPLEFT, DockType refdock = DT_TOPLEFT, IGuiShape** primitive = NULL)
		{DrawTextField(format, text, brush, Rect<float>(x, y, width, height), dock, refdock, primitive);}
	virtual void ClearPrimitives() = 0;

	virtual Result CreateSolidBrush(const Color& clr, IGuiBrush** brush) = 0;
	Result CreateSolidBrush(float r, float g, float b, float a, IGuiBrush** brush)
		{return CreateSolidBrush(Color(r, g, b, a), brush);}
	virtual Result CreateLinearGradientBrush(GradientStop* stops, UINT numstops, const Point<float> startpos, const Point<float> endpos, IGuiBrush** brush) = 0;

	virtual Result CreateImageFromFile(const FilePath& filename, IGuiBitmap** img) = 0;
	virtual Result CreateImageFromMemory(LPVOID data, DWORD datasize, IGuiBitmap** img) = 0;
	virtual Result CreateImageFromISImage(const Image* image, IGuiBitmap** img) = 0;
	virtual Result CreateCompositeImage(IGuiCompositeImage** img) = 0;
	//virtual void RemoveImage(IGuiBitmap* img) = 0;
	//virtual void RemoveImage(IGuiCompositeImage* img) = 0;
//	virtual Result CreateDirectContent(const Rect<int>& destrect, bool usegdi, IGuiDirectContent** gdc) = 0;
	virtual void SetRendertargetResizeCallback(const RESIZERENDERTARGET_CALLBACK resizecbk, LPVOID user) = 0;

#ifdef _WIN32
	__declspec(property(get=GetScale, put=SetScale)) Point<float> Scale;
	__declspec(property(get=GetTranslation, put=SetTranslation)) Point<float> Translation;
	__declspec(property(get=GetContentPadding, put=SetContentPadding)) Point<float> ContentPadding;
#endif
}* LPGUIOUTPUTWINDOW;

//-----------------------------------------------------------------------------
// Name: IGuiFactory
// Desc: Interface to the GuiFactory class
//-----------------------------------------------------------------------------
struct ID3D11Device;
typedef class IGuiFactory
{
public:
	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init() = 0;

	virtual Result OnPaint(HWND wnd, HDC hdc, const Rect<> &rect, bool& painthandled) = 0;
	virtual void OnResize(HWND wnd, Size<UINT>& newsize) = 0;
	virtual void OnHScrolling(HWND wnd, short newpos) = 0;
	virtual void OnVScrolling(HWND wnd, short newpos) = 0;
	virtual void OnHScrolled(HWND wnd, short newpos) = 0;
	virtual void OnVScrolled(HWND wnd, short newpos) = 0;
	virtual void OnMouseDown(HWND wnd, const MouseEventArgs& args) = 0;
	virtual void OnMouseUp(HWND wnd, const MouseEventArgs& args) = 0;
	virtual void OnMouseMove(HWND wnd, const MouseEventArgs& args) = 0;
	virtual void OnMouseDoubleClick(HWND wnd, const MouseEventArgs& args) = 0;

	virtual Result CreateOutputWindow(HWND targetwnd, IGuiOutputWindow** wnd) = 0;
	virtual Result CreateOutputWindow(HWND targetwnd, ID3D11Device *d3ddevice, IGuiOutputWindow** wnd) = 0;
	virtual Result CreateTextFormat(const String& family, float size, bool italic, GuiFontWeight weight, GuiFontStretch stretch, IGuiTextFormat** fmt) = 0;
	Result CreateTextFormat(const String& family, float size, bool italic, GuiFontWeight weight, IGuiTextFormat** fmt)
		{return CreateTextFormat(family, size, italic, weight, GFS_NORMAL, fmt);}
	Result CreateTextFormat(const String& family, float size, bool italic, IGuiTextFormat** fmt)
		{return CreateTextFormat(family, size, italic, GFW_NORMAL, GFS_NORMAL, fmt);}
	virtual Result CreateSlider(IGuiControl* bgctrl, IGuiControl* sliderctrl, Orientation orientation, float startpos, float endpos, ISliderControl** slider) = 0;
	virtual Result CreateAnimation(IGuiAnimation** ani) = 0;
	virtual void Release() = 0;
}* LPGUIFACTORY;

extern "C" GUIFACTORY_EXTERN_FUNC LPGUIFACTORY __cdecl CreateGuiFactory();
extern "C" GUIFACTORY_EXTERN_FUNC bool __cdecl CheckGuiFactorySupport(LPTSTR* missingdllname);

#endif