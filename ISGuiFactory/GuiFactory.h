#ifndef __GUIFACTORY_H
#define __GUIFACTORY_H

#include "ISGuiFactory.h"
#include "..\\global\\HResult.h"

#include <map>

// Direct2D headers
#include <d3d11_1.h>
#include <D2D1_1.h>
#include <D2D1_1helper.h>
// DirectWrite headers
#include <DWrite_1.h>
// UIAnimation headers
#include <UIAnimation.h>
// Windows Imaging Component headers
#include <Wincodec.h>

// Direct2D libraries
//#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
// DirectWrite libraries
#pragma comment(lib, "dwrite.lib")
// Windows Imaging Component headers
#pragma comment(lib, "WindowsCodecs.lib")


//-----------------------------------------------------------------------------
// EXTERN VARS
//-----------------------------------------------------------------------------
extern ID2D1Factory1 *d2dfactory;
extern IDWriteFactory1* dwtfactory;
extern IWICImagingFactory2* wicfactory;
extern IUIAnimationManager *animanager;
extern IUIAnimationTimer *anitimer;
extern IUIAnimationTransitionLibrary *anitranslib;


struct RenderTarget
{
	IDXGIDevice *dxgidevice;
	ID2D1DeviceContext* context;
	D2D1::Matrix3x2F transform;

	~RenderTarget()
	{
		RELEASE(context);
		RELEASE(dxgidevice);
	}
};


//-----------------------------------------------------------------------------
// Name: IDestroyable
// Desc: Interface to class with device dependent ressources
//-----------------------------------------------------------------------------
class IDestroyable
{
public:
	virtual Result OnCreateDevice(RenderTarget *rendertarget) = 0;
	virtual void OnDestroyDevice() = 0;
};


#include "GuiAnimation.h"
#include "MouseEventRouter.h"
//#include "GuiMouseRegion.h"
#include "GuiText.h"
#include "GuiBrush.h"
#include "GuiShape.h"
#include "GuiImage.h"
#include "GuiControl.h"
#include "InteractiveControls.h"


class GuiOutputWindow;

//-----------------------------------------------------------------------------
// Name: RenderArea
// Desc: Base class for clipped render areas, directly used for the main render area
//-----------------------------------------------------------------------------
class RenderArea : public IGuiRenderArea, public MouseEventRouter
{
private:
	void UpdateContentSpan(const Rect<float>& bounds);
	void UpdateContentSpan();

public:
	std::list<GuiControl*> ctrls;
	std::list<Shape*> primitives;
	GuiOutputWindow* parent;
	bool invalidated;
	::Size<float> contentspan;

	RenderArea(GuiOutputWindow* parent) : parent(parent), invalidated(false), contentspan(0.0f, 0.0f)
	{
		dock = DT_TOPLEFT; //EDIT: Delete as soon as render area docks are implemented
		refdock = DT_TOPLEFT; //EDIT: Delete as soon as render area docks are implemented
	}

	virtual Point<float>& GetLocation() {return *(new Point<float>());}
	virtual void SetLocation(const Point<float>& val) {}
	virtual float GetLeft() {return 0.0f;}
	virtual void SetLeft(float val) {}
	virtual float GetTop() {return 0.0f;}
	virtual void SetTop(float val) {}

	virtual ::Size<float>& GetSize() {return *(new ::Size<float>());}
	virtual void SetSize(const ::Size<float>& val) {}
	virtual float GetWidth() {return 0.0f;}
	virtual void SetWidth(float val) {}
	virtual float GetHeight() {return 0.0f;}
	virtual void SetHeight(float val) {}

//	virtual bool TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds);
	virtual void Render(RenderTarget *rendertarget);
	void Invalidate();
	Result CreateControl(DockType dock, DockType refdock, IGuiControl** ctrl);
	void RemoveControl(IGuiControl* ctrl);
	void RemoveAllControls();

	void FillRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius, DockType dock, DockType refdock);
	void DrawRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius, DockType dock, DockType refdock);
	void FillEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock);
	void DrawEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock);
	void DrawPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed, float strokewidth, DockType dock, DockType refdock);
	void FillPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed, DockType dock, DockType refdock);
	void DrawLine(IGuiBrush* brush, const Point<float>& p0, const Point<float>& p1, float strokewidth, DockType dock, DockType refdock);
	void DrawBitmap(IGuiBitmap* bmp, const Rect<float>& bounds, DockType dock, DockType refdock);
	void DrawTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Rect<float>& bounds, DockType dock, DockType refdock, IGuiShape** primitive);
	void ClearPrimitives();

	virtual void Release();
};

//-----------------------------------------------------------------------------
// Name: ClippedRenderArea
// Desc: Specifies a region on the rendertarget for clipping controls
//-----------------------------------------------------------------------------
class ClippedRenderArea : public RenderArea, public IManagedObject
{
private:
	D2D1_ANTIALIAS_MODE aliasmode;

public:

	ClippedRenderArea(GuiOutputWindow* parent, Rect<float>& bounds, bool antialiased) : RenderArea(parent)
	{
		this->bounds = bounds;
		aliasmode = antialiased ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED;
	}

	Point<float>& GetLocation() {return bounds.location;}
	void SetLocation(const Point<float>& val) {bounds.location = val;}
	float GetLeft() {return bounds.x;}
	void SetLeft(float val) {bounds.x = val;}
	float GetTop() {return bounds.y;}
	void SetTop(float val) {bounds.y = val;}

	::Size<float>& GetSize() {return bounds.size;}
	void SetSize(const ::Size<float>& val) {bounds.size = val;}
	float GetWidth() {return bounds.width;}
	void SetWidth(float val) {bounds.width = val;}
	float GetHeight() {return bounds.height;}
	void SetHeight(float val) {bounds.height = val;}

//	bool TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds);
	void Render(RenderTarget *rendertarget);
	void Release();
};

/*//-----------------------------------------------------------------------------
// Name: GuiDirectContent
// Desc: Contains backbuffered image data to be drawn directly and before any Direct2D drawing
//-----------------------------------------------------------------------------
class GuiDirectContent : public IGuiDirectContent, public IManagedObject, public IDestroyable
{
public:
	bool visible;

	bool GetVisible() {return visible;}
	void SetVisible(bool val) {visible = val;}

	virtual Result PrepareContent(Size<UINT>& srcsize) = 0;
	virtual Result SetContent(byte* imgdata) = 0;
	virtual void Render(RenderTarget *rendertarget, HDC hdc, const Rect<int> &rect) = 0;
	virtual Result OnCreateDevice(RenderTarget *rendertarget) {return R_OK;};
	virtual void OnDestroyDevice() {};
	virtual void Release() = 0;
};
class D2dDirectContent : public GuiDirectContent
{
private:
	ID2D1Bitmap* d2dbmp;
	Size<UINT> srcsize;
	const Rect<int> destrect;

public:

	D2dDirectContent(const Rect<int>& destrect) : srcsize(0, 0), destrect(destrect)
	{
		d2dbmp = NULL;
	}

	Result PrepareContent(Size<UINT>& srcsize);
	Result SetContent(byte* imgdata);
	void Render(RenderTarget *rendertarget, HDC hdc, const Rect<int> &rect);
	Result OnCreateDevice(RenderTarget *rendertarget);
	void OnDestroyDevice();
	void Release();
};
class GdiDirectContent : public GuiDirectContent
{
private:
	HDC hmemDC;
	HBITMAP hmemBMP;
	BITMAPINFO bmpinfo;
	Size<UINT> srcsize;
	const Rect<int> destrect;

public:

	GdiDirectContent(const Rect<int>& destrect) : destrect(destrect)
	{
		hmemDC = NULL;
		hmemBMP = NULL;
	}

	Result PrepareContent(Size<UINT>& srcsize);
	Result SetContent(byte* imgdata);
	void Render(RenderTarget *rendertarget, HDC hdc, const Rect<int> &rect);
	void Release();
};*/

//-----------------------------------------------------------------------------
// Name: GuiOutputWindow
// Desc: Direct2D client
//-----------------------------------------------------------------------------
class GuiOutputWindow : public IGuiOutputWindow, public IObjectManager, public MouseEventRouter
{
private:
	RESIZERENDERTARGET_CALLBACK resizecbk;
	LPVOID resizecbkuser;
	ID2D1Brush *defaultbgbrush;
	RenderArea* mainrenderarea;
	Point<float> scale, translation, scrolltranslation, contentpadding;
	Size<float> contentspan;

	Result CreateDevice();
	void DestroyDevice();
	void UpdateScrollbars();

public:
	RenderTarget *rendertarget;
	IDXGISwapChain1 *swapchain;
	ID2D1Bitmap1 *backbuffer;
	ID2D1Device *device;
	HWND targetwnd;
	ID3D11Device *d3ddevice;
	std::list<IDestroyable*> destroyableobjects;
	std::list<RenderArea*> renderareas;

	GuiOutputWindow() : scale(1.0f, 1.0f), translation(0.0f, 0.0f), scrolltranslation(0.0f, 0.0f), contentpadding(0.0f, 0.0f), contentspan(0.0f, 0.0f)
	{
		resizecbk = NULL;
		mainrenderarea = NULL;
		rendertarget = NULL;
		swapchain = NULL;
		backbuffer = NULL;
		device = NULL;
		defaultbgbrush = NULL;
		bgbrush = NULL;
	};

	Result Init(HWND targetwnd);
	Result Init(HWND targetwnd, ID3D11Device *d3ddevice);
	HWND GetTargetWnd() {return targetwnd;}
////	bool TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds) {return true;}
	void Invalidate() {mainrenderarea->Invalidate();}
	Point<float> GetScale() const {return scale;}
	void SetScale(const Point<float>& val);
	Point<float> GetTranslation() const {return translation;}
	void SetTranslation(const Point<float>& val);
	Point<float> GetContentPadding() const {return contentpadding;}
	void SetContentPadding(const Point<float>& val);
	Result OnPaint(HDC hdc, const Rect<> &rect);
	Result OnResize(::Size<UINT>& newsize);
	void OnHScrolling(short newpos);
	void OnVScrolling(short newpos);
	void OnHScrolled(short newpos);
	void OnVScrolled(short newpos);
	Result TakeScreenshot(const FilePath& filename);
	Result TakeScreenshot(BYTE** imgbuffer, UINT* imgoffset, UINT* width, UINT* height);
	void OnMouseDown(const MouseEventArgs& args);
	void OnMouseUp(const MouseEventArgs& args);
	void OnMouseMove(const MouseEventArgs& args);
	void OnMouseDoubleClick(const MouseEventArgs& args);
	Result CreateRenderArea(Rect<float>& bounds, bool antialiased, IGuiRenderArea** area);
	Result CreateControl(DockType dock, DockType refdock, IGuiControl** ctrl) {return mainrenderarea->CreateControl(dock, refdock, ctrl);}
	void RemoveControl(IGuiControl* ctrl) {mainrenderarea->RemoveControl(ctrl);}
	void RemoveAllControls() {mainrenderarea->RemoveAllControls();}
	void FillRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius, DockType dock, DockType refdock)
		{return mainrenderarea->FillRectangle(brush, bounds, radius, dock, refdock);}
	void DrawRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius, DockType dock, DockType refdock)
		{return mainrenderarea->DrawRectangle(brush, bounds, radius, dock, refdock);}
	void FillEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock)
		{return mainrenderarea->FillEllipse(brush, center, radius, dock, refdock);}
	void DrawEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock)
		{return mainrenderarea->DrawEllipse(brush, center, radius, dock, refdock);}
	void DrawPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed, float strokewidth, DockType dock, DockType refdock)
		{return mainrenderarea->DrawPolygon(brush, vertices, numvertices, isclosed, strokewidth, dock, refdock);}
	void FillPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed, DockType dock, DockType refdock)
		{return mainrenderarea->FillPolygon(brush, vertices, numvertices, isclosed, dock, refdock);}
	void DrawLine(IGuiBrush* brush, const Point<float>& p0, const Point<float>& p1, float strokewidth, DockType dock, DockType refdock)
		{return mainrenderarea->DrawLine(brush, p0, p1, strokewidth, dock, refdock);}
	void DrawBitmap(IGuiBitmap* bmp, const Rect<float>& bounds, DockType dock, DockType refdock)
		{return mainrenderarea->DrawBitmap(bmp, bounds, dock, refdock);}
	void DrawTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Rect<float>& bounds, DockType dock, DockType refdock, IGuiShape** primitive)
		{return mainrenderarea->DrawTextField(format, text, brush, bounds, dock, refdock, primitive);}
	void ClearPrimitives() {mainrenderarea->ClearPrimitives();}
	Result CreateSolidBrush(const Color& clr, IGuiBrush** brush);
	Result CreateLinearGradientBrush(GradientStop* stops, UINT numstops, const Point<float> startpos, const Point<float> endpos, IGuiBrush** brush);
	Result CreateImageFromFile(const FilePath& filename, IGuiBitmap** img);
	Result CreateImageFromMemory(LPVOID data, DWORD datasize, IGuiBitmap** img);
	Result CreateImageFromISImage(const Image* image, IGuiBitmap** img);
	Result CreateCompositeImage(IGuiCompositeImage** img);
	//void RemoveImage(IGuiBitmap* img);
	//void RemoveImage(IGuiCompositeImage* img);
//	Result CreateDirectContent(const Rect<int>& destrect, bool usegdi, IGuiDirectContent** gdc);
	void SetRendertargetResizeCallback(const RESIZERENDERTARGET_CALLBACK resizecbk, LPVOID user) {this->resizecbk = resizecbk; resizecbkuser = user;}
	void OnContentSpanIncrease(const Size<float>& newspan);
	void OnContentSpanDecrease(const Size<float>& oldspan);
	void Release();
};

//-----------------------------------------------------------------------------
// Name: GuiFactory
// Desc: API to Direct2D, DirectWrite and UIAnimation
//-----------------------------------------------------------------------------
class GuiFactory : public IGuiFactory, public IObjectManager//, public MouseEventRouter
{
public:
	std::map<HWND, GuiOutputWindow*> wnds;
	std::list<GuiAnimation*> animations;

	GuiFactory()
	{
		animanager = NULL;
		anitimer = NULL;
		anitranslib = NULL;
	};

	static void TransformBounds(DockType dock, DockType refdock, const Rect<float>& parentbounds, const Rect<float>& localbounds, Rect<float>* finalbounds);
	static void TransformBoundsBack(DockType dock, DockType refdock, const Rect<float>& parentbounds, const Rect<float>& localbounds, Rect<float>* finalbounds);

	void Sync(GLOBALVARDEF_LIST);
	Result Init();
//	bool TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds) {return true;}
	Result OnPaint(HWND wnd, HDC hdc, const Rect<> &rect, bool& painthandled);
	void OnResize(HWND wnd, ::Size<UINT>& newsize);
	void OnHScrolling(HWND wnd, short newpos);
	void OnVScrolling(HWND wnd, short newpos);
	void OnHScrolled(HWND wnd, short newpos);
	void OnVScrolled(HWND wnd, short newpos);
	void OnMouseDown(HWND wnd, const MouseEventArgs& args);
	void OnMouseUp(HWND wnd, const MouseEventArgs& args);
	void OnMouseMove(HWND wnd, const MouseEventArgs& args);
	void OnMouseDoubleClick(HWND wnd, const MouseEventArgs& args);
	Result CreateOutputWindow(HWND targetwnd, IGuiOutputWindow** wnd);
	Result CreateOutputWindow(HWND targetwnd, ID3D11Device *d3ddevice, IGuiOutputWindow** wnd);
	Result CreateTextFormat(const String& family, float size, bool italic, GuiFontWeight weight, GuiFontStretch stretch, IGuiTextFormat** fmt);
	Result CreateSlider(IGuiControl* bgctrl, IGuiControl* sliderctrl, Orientation orientation, float startpos, float endpos, ISliderControl** slider);
	Result CreateAnimation(IGuiAnimation** ani);
	void Release();
};

#endif