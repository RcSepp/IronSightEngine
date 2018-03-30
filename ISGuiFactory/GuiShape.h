#ifndef __GUISHAPE_H
#define __GUISHAPE_H

class GuiBitmap;
class GuiCompositeImage;

class Shape : public IGuiShape
{
public:
	union Texture
	{
		GuiBrush* b;
		GuiBitmap* i;
	};
protected:
	DockType dock, refdock;
	Texture tex;
	GuiCompositeImage* parent;

	virtual void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const = 0;

	Shape(GuiCompositeImage* parent, DockType dock, DockType refdock) : parent(parent), dock(dock), refdock(refdock) {}
	/*~Shape()
	{
	}*/

	virtual bool CheckMousePos(const Point<float>& mousepos, const Rect<float>& bounds)
	{
		return mousepos.x >= bounds.left() && mousepos.x <= bounds.right() &&
			   mousepos.y >= bounds.top() && mousepos.y <= bounds.bottom();
	}

public:
	Rect<float> bounds;

	void Paint(RenderTarget *rendertarget, const Rect<float>& ctrlbounds, float alpha) const;

	Rect<float>& GetBounds() {return bounds;}
	void SetBounds(Rect<float>& val) {bounds = val;}

	Point<float>& GetLocation() {return bounds.location;}
	void SetLocation(const Point<float>& val) {bounds.location = val;}
	float GetLeft() {return bounds.x;}
	void SetLeft(float val) {bounds.x = val;}
	float GetTop() {return bounds.y;}
	void SetTop(float val) {bounds.y = val;}

	::Size<float>& GetSize() {return bounds.size;}
	virtual void SetSize(const ::Size<float>& val) {bounds.size = val;}
	float GetWidth() {return bounds.width;}
	virtual void SetWidth(float val) {bounds.width = val;}
	float GetHeight() {return bounds.height;}
	virtual void SetHeight(float val) {bounds.height = val;}
	LPGUIBRUSH GetBrush() {return tex.b;}
	LPGUIBRUSH SetBrush(LPGUIBRUSH val);

	DockType GetDock() {return dock;}
	void SetDock(DockType val) {dock = val;}

	virtual Result MeasureText(Rect<float>* bounds) {return ERR("MeasureText() called on a shape which is not a TextField");};

	virtual void Release() = 0;
};

class GuiRectangle : public Shape
{
protected:
	bool isrounded;
	Point<float> radius;
public:
	GuiRectangle(GuiCompositeImage* parent, const Rect<float>& bounds, const Point<float>& radius, IGuiBrush* brush, DockType dock, DockType refdock);
};
class FilledRectangle : public GuiRectangle
{
public:
	FilledRectangle(GuiCompositeImage* parent, const Rect<float>& bounds, const Point<float>& radius, IGuiBrush* brush, DockType dock, DockType refdock) :
		GuiRectangle(parent, bounds, radius, brush, dock, refdock) {}

	void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const;
	void Release() {delete this;}
};
class DrawnRectangle : public GuiRectangle
{
public:
	DrawnRectangle(GuiCompositeImage* parent, const Rect<float>& bounds, const Point<float>& radius, IGuiBrush* brush, DockType dock, DockType refdock) :
		GuiRectangle(parent, bounds, radius, brush, dock, refdock) {}

	void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const;
	void Release() {delete this;}
};

class GuiEllipse : public Shape
{
public:
	GuiEllipse(GuiCompositeImage* parent, const Point<float>& center, const Point<float>& radius, IGuiBrush* brush, DockType dock, DockType refdock);

	bool CheckMousePos(const Point<float>& mousepos, const Rect<float>& bounds);
};
class FilledEllipse : public GuiEllipse
{
public:
	FilledEllipse(GuiCompositeImage* parent, const Point<float>& center, const Point<float>& radius, IGuiBrush* brush, DockType dock, DockType refdock) :
		GuiEllipse(parent, center, radius, brush, dock, refdock) {}

	void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const;
	void Release() {delete this;}
};
class DrawnEllipse : public GuiEllipse
{
public:
	DrawnEllipse(GuiCompositeImage* parent, const Point<float>& center, const Point<float>& radius, IGuiBrush* brush, DockType dock, DockType refdock) :
		GuiEllipse(parent, center, radius, brush, dock, refdock) {}

	void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const;
	void Release() {delete this;}
};

class GuiPolygon : public Shape
{
protected:
	ID2D1PathGeometry* pathgeom;
public:
	GuiPolygon(GuiCompositeImage* parent, const Point<float>* vertices, size_t numvertices, bool isclosed, IGuiBrush* brush, DockType dock, DockType refdock);
};
class FilledPolygon : public GuiPolygon
{
public:
	FilledPolygon(GuiCompositeImage* parent, const Point<float>* vertices, size_t numvertices, bool isclosed, IGuiBrush* brush, DockType dock, DockType refdock) :
		GuiPolygon(parent, vertices, numvertices, isclosed, brush, dock, refdock) {}

	void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const;
	void Release() {delete this;}
};
class DrawnPolygon : public GuiPolygon
{
private:
	float strokewidth;

public:
	DrawnPolygon(GuiCompositeImage* parent, const Point<float>* vertices, size_t numvertices, bool isclosed, float strokewidth, IGuiBrush* brush, DockType dock, DockType refdock) :
		GuiPolygon(parent, vertices, numvertices, isclosed, brush, dock, refdock), strokewidth(strokewidth) {}

	void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const;
	void Release() {delete this;}
};

class FilledGeometry : public Shape
{
private:
	std::vector<D2D1_TRIANGLE> triangles;

public:
	FilledGeometry(GuiCompositeImage* parent, const Point<float>* vertices, size_t numtriangles, IGuiBrush* brush, DockType dock, DockType refdock);

	void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const;
	void Release() {delete this;}
};

class Line : public Shape
{
private:
	Point<float> p0, p1;
	float strokewidth;

public:
	Line(GuiCompositeImage* parent, const Point<float>& p0, const Point<float>& p1, float strokewidth, IGuiBrush* brush, DockType dock, DockType refdock);

	void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const;
	void Release() {delete this;}
};

class Bitmap : public Shape
{
public:
	Bitmap(GuiCompositeImage* parent, IGuiBitmap* bmp, const Rect<float>& bounds, DockType dock, DockType refdock);

	void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const;
	void Release() {delete this;}
};

class TextField : public Shape
{
	GuiTextFormat* format;
	IDWriteTextLayout* layout;
	GuiTextAllignment allignment;
	String text;
	/*Rect<float>* cachedtextbounds;*/
public:
	TextField(IGuiTextFormat* format, const Rect<float>& bounds, IGuiBrush* brush, DockType dock, DockType refdock);

	void PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const;

	String& GetText() {return text;}
	Result SetText(const String& text);
	GuiTextAllignment GetTextAllignment() {return allignment;}
	void SetTextAllignment(GuiTextAllignment allignment);
	Result MeasureText(Rect<float>* bounds);
	void SetSize(const ::Size<float>& val);
	void SetWidth(float val);
	virtual void SetHeight(float val);
	void Release() {delete this;}
};

#endif