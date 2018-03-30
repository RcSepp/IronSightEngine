#ifndef __GUIBITMAP_H
#define __GUIBITMAP_H


class GuiOutputWindow;
class GuiControl;
struct Image; // Image from ISImage

//-----------------------------------------------------------------------------
// Name: GuiImage
// Desc: Base class for bitmap and composite images
//-----------------------------------------------------------------------------
class GuiImage : public IManagedObject, public IDestroyable
{
protected:
	ID2D1Bitmap1* d2dbmp;
	GuiOutputWindow* parentwnd;

public:
	::Size<float> size;

	GuiImage(GuiOutputWindow* parentwnd) : parentwnd(parentwnd), d2dbmp(NULL), size(0.0f, 0.0f) {}

	virtual Result Recreate() {return R_OK;}
	virtual Result OnCreateDevice(RenderTarget *rendertarget) = 0;
	virtual void OnDestroyDevice() {RELEASE(d2dbmp);}
	virtual bool CheckMousePos(const Point<float>& mousepos, const Rect<float>& bounds) const
	{
		return mousepos.x >= bounds.left() && mousepos.x <= bounds.right() &&
			   mousepos.y >= bounds.top() && mousepos.y <= bounds.bottom();
	}
	ID2D1Bitmap* Paint(RenderTarget *rendertarget);
	virtual void Release() = 0;

	//operator ID2D1Bitmap*() const {return d2dbmp;}
};

//-----------------------------------------------------------------------------
// Name: GuiBitmap
// Desc: Wrapper class for an ID2D1Bitmap
//-----------------------------------------------------------------------------
class GuiBitmap : public IGuiBitmap, public GuiImage
{
private:
	IWICFormatConverter *fmtconverter;
	LPBYTE imagedata;

public:

	GuiBitmap(GuiOutputWindow* parentwnd) : GuiImage(parentwnd), fmtconverter(NULL), imagedata(NULL) {}

	Result Load(const FilePath& filename, RenderTarget *rendertarget);
	Result Load(LPVOID data, DWORD datasize, RenderTarget *rendertarget);
	Result Load(const Image* image);
	Result OnCreateDevice(RenderTarget *rendertarget);
	void Release();

	::Size<float>& GetSize() {return size;}
	::Size<float>& SetSize(const ::Size<float>& val) {return size = val;}
	float GetWidth() {return size.width;}
	void SetWidth(float val) {size.width = val;}
	float GetHeight() {return size.height;}
	void SetHeight(float val) {size.height = val;}
};

//-----------------------------------------------------------------------------
// Name: GuiCompositeImage
// Desc: Wrapper class for an ID2D1Bitmap
//-----------------------------------------------------------------------------
class GuiCompositeImage : public IGuiCompositeImage, public GuiImage
{
private:
	std::list<Shape*> shapes;
	RenderTarget *rendertarget;

public:
	GuiControl* parent;

	GuiCompositeImage(GuiOutputWindow* parentwnd) : GuiImage(parentwnd), parent(NULL), rendertarget(NULL) {}

	void AddFilledRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius, DockType dock, DockType refdock, IGuiShape** shape);
	void AddDrawnRectangle(IGuiBrush* brush, const Rect<float>& bounds, const Point<float>& radius, DockType dock, DockType refdock, IGuiShape** shape);
	void AddFilledEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock, IGuiShape** shape);
	void AddDrawnEllipse(IGuiBrush* brush, const Point<float>& center, const Point<float>& radius, DockType dock, DockType refdock, IGuiShape** shape);
	void AddDrawnPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed, float strokewidth, DockType dock, DockType refdock, IGuiShape** shape);
	void AddFilledPolygon(IGuiBrush* brush, const Point<float>* vertices, size_t numvertices, bool isclosed, DockType dock, DockType refdock, IGuiShape** shape);
	void AddFilledGeometry(IGuiBrush* brush, const Point<float>* vertices, size_t numtriangles, DockType dock, DockType refdock, IGuiShape** shape);
	void AddLine(IGuiBrush* brush, const Point<float>& p0, const Point<float>& p1, float strokewidth, DockType dock, DockType refdock, IGuiShape** shape);
	void AddBitmap(IGuiBitmap* bmp, const Rect<float>& bounds, DockType dock, DockType refdock, IGuiShape** shape);
	void AddTextField(IGuiTextFormat* format, const String& text, IGuiBrush* brush, const Rect<float>& bounds, GuiTextAllignment allignment, DockType dock, DockType refdock, IGuiShape** shape);

	Result Create(RenderTarget *rendertarget);
	Result Recreate();
	Result OnCreateDevice(RenderTarget *rendertarget);
	void Release();

	::Size<float>& GetSize() {return size;}
	::Size<float>& SetSize(const ::Size<float>& val) {return size = val;}
	float GetWidth() {return size.width;}
	void SetWidth(float val) {size.width = val;}
	float GetHeight() {return size.height;}
	void SetHeight(float val) {size.height = val;}
};

#endif