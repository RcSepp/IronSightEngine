#include "GuiFactory.h"

void Shape::Paint(RenderTarget *rendertarget, const Rect<float>& ctrlbounds, float alpha) const
{
	if(alpha == 0.0f)
		return;

	Rect<float> finalbounds;
	GuiFactory::TransformBounds(dock, refdock, ctrlbounds, bounds, &finalbounds);
	D2D1::Matrix3x2F matTrans = D2D1::Matrix3x2F::Translation(finalbounds.x, finalbounds.y);
	rendertarget->context->SetTransform(matTrans * rendertarget->transform);

	/*float dfade = downfade.GetVal();
	if(dfade)
		PaintInternal(rendertarget, downtex, dfade, alpha);
	else
		PaintInternal(rendertarget, overtex, overfade.GetVal(), alpha);*/
Texture bla; bla.b = NULL;
	PaintInternal(rendertarget, bla, 0.0f, 1.0f);
}

LPGUIBRUSH Shape::SetBrush(LPGUIBRUSH val)
{
	if(tex.b != (GuiBrush*)val)
	{
		tex.b = (GuiBrush*)val;
		if(parent)
			parent->Recreate();
	}
	return tex.b;
}

GuiRectangle::GuiRectangle(GuiCompositeImage* parent, const Rect<float>& bounds, const Point<float>& radius, IGuiBrush* brush, DockType dock, DockType refdock) : Shape(parent, dock, refdock)
{
	this->bounds = Rect<float>(bounds);
	this->radius = Point<float>(radius);
	isrounded = radius.x != 0.0f || radius.y != 0.0f;
	tex.b = (GuiBrush*)brush;
}

void FilledRectangle::PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const
{
	D2D_RECT_F rect = D2D1::RectF(0.0f, 0.0f, bounds.width, bounds.height);

	if(isrounded)
	{
		if(efftexopacity)
			rendertarget->context->FillRoundedRectangle(D2D1::RoundedRect(rect, radius.x, radius.y), efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
		efftexopacity = 1.0f - efftexopacity;
		if(efftexopacity)
			rendertarget->context->FillRoundedRectangle(D2D1::RoundedRect(rect, radius.x, radius.y), tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
	}
	else
	{
		if(efftexopacity)
			rendertarget->context->FillRectangle(rect, efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
		efftexopacity = 1.0f - efftexopacity;
		if(efftexopacity)
			rendertarget->context->FillRectangle(rect, tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
	}
}

void DrawnRectangle::PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const
{
	D2D_RECT_F rect = D2D1::RectF(0.0f, 0.0f, bounds.width, bounds.height);

	if(isrounded)
	{
		if(efftexopacity)
			rendertarget->context->DrawRoundedRectangle(D2D1::RoundedRect(rect, radius.x, radius.y), efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
		efftexopacity = 1.0f - efftexopacity;
		if(efftexopacity)
			rendertarget->context->DrawRoundedRectangle(D2D1::RoundedRect(rect, radius.x, radius.y), tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
	}
	else
	{
		if(efftexopacity)
			rendertarget->context->DrawRectangle(rect, efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
		efftexopacity = 1.0f - efftexopacity;
		if(efftexopacity)
			rendertarget->context->DrawRectangle(rect, tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
	}
}

GuiEllipse::GuiEllipse(GuiCompositeImage* parent, const Point<float>& center, const Point<float>& radius, IGuiBrush* brush, DockType dock, DockType refdock) : Shape(parent, dock, refdock)
{
	bounds = Rect<float>(center.x, center.y, 2.0f * radius.x, 2.0f * radius.y);
	tex.b = (GuiBrush*)brush;
}

bool GuiEllipse::CheckMousePos(const Point<float>& mousepos, const Rect<float>& bounds)
{
	const Point<float> r(bounds.x + bounds.width / 2.0f, bounds.y + bounds.height / 2.0f);
	Point<float> pos = mousepos - r;
	pos.x /= r.x;
	pos.y /= r.y;
	return pos.x*pos.x + pos.y*pos.y <= 1.0f;
}

void FilledEllipse::PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const
{
	float rx = bounds.width / 2.0f;
	float ry = bounds.height / 2.0f;

	if(efftexopacity)
		rendertarget->context->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rx, ry), rx, ry), efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
	efftexopacity = 1.0f - efftexopacity;
	if(efftexopacity)
		rendertarget->context->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rx, ry), rx, ry), tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
}

void DrawnEllipse::PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const
{
	float rx = bounds.width / 2.0f;
	float ry = bounds.height / 2.0f;

	if(efftexopacity)
		rendertarget->context->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(rx, ry), rx, ry), efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
	efftexopacity = 1.0f - efftexopacity;
	if(efftexopacity)
		rendertarget->context->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(rx, ry), rx, ry), tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
}

GuiPolygon::GuiPolygon(GuiCompositeImage* parent, const Point<float>* vertices, size_t numvertices, bool isclosed, IGuiBrush* brush, DockType dock, DockType refdock) : Shape(parent, dock, refdock)
{
	// Get bounds through min and max coordinates
	Point<float> minpos = vertices[0];
	Point<float> maxpos = vertices[0];
	for(size_t i = 1; i < numvertices; i++)
	{
		minpos.x = min(minpos.x, vertices[i].x);
		minpos.y = min(minpos.y, vertices[i].y);
		maxpos.x = max(maxpos.x, vertices[i].x);
		maxpos.y = max(maxpos.y, vertices[i].y);
	}
	this->bounds = Rect<float>(minpos.x, minpos.y, maxpos.x - minpos.x, maxpos.y - minpos.y);

	// Create a path geometry
	ID2D1GeometrySink *geomsink = NULL;
	d2dfactory->CreatePathGeometry(&pathgeom);
	pathgeom->Open(&geomsink);

	// Define geometry
	geomsink->BeginFigure(D2D1::Point2F(vertices[0].x - minpos.x, vertices[0].y - minpos.y), D2D1_FIGURE_BEGIN_FILLED);
	for(size_t i = 1; i < numvertices; i++)
		geomsink->AddLine(D2D1::Point2F(vertices[i].x - minpos.x, vertices[i].y - minpos.y));
	geomsink->EndFigure(isclosed ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
	geomsink->Close();
	geomsink->Release();

	tex.b = (GuiBrush*)brush;
}

void FilledPolygon::PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const
{
	D2D_RECT_F rect = D2D1::RectF(0.0f, 0.0f, bounds.width, bounds.height);

	if(efftexopacity)
		rendertarget->context->FillGeometry(pathgeom, efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
	efftexopacity = 1.0f - efftexopacity;
	if(efftexopacity)
		rendertarget->context->FillGeometry(pathgeom, tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
}

void DrawnPolygon::PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const
{
	D2D_RECT_F rect = D2D1::RectF(0.0f, 0.0f, bounds.width, bounds.height);

	if(efftexopacity)
		rendertarget->context->DrawGeometry(pathgeom, efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget), strokewidth);
	efftexopacity = 1.0f - efftexopacity;
	if(efftexopacity)
		rendertarget->context->DrawGeometry(pathgeom, tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget), strokewidth);
}

FilledGeometry::FilledGeometry(GuiCompositeImage* parent, const Point<float>* vertices, size_t numtriangles, IGuiBrush* brush, DockType dock, DockType refdock) : Shape(parent, dock, refdock)
{
	// Get bounds through min and max coordinates
	Point<float> minpos = vertices[0];
	Point<float> maxpos = vertices[0];
	for(size_t i = 1; i < 3 * numtriangles; i++)
	{
		minpos.x = min(minpos.x, vertices[i].x);
		minpos.y = min(minpos.y, vertices[i].y);
		maxpos.x = max(maxpos.x, vertices[i].x);
		maxpos.y = max(maxpos.y, vertices[i].y);
	}
	this->bounds = Rect<float>(minpos.x, minpos.y, maxpos.x - minpos.x, maxpos.y - minpos.y);

	// Add triangles
	D2D1_TRIANGLE triangle;
	for(size_t i = 0; i < numtriangles; i++)
	{
		triangle.point1 = D2D1::Point2F(vertices[i * 3 + 0].x - minpos.x, vertices[i * 3 + 0].y - minpos.y);
		triangle.point2 = D2D1::Point2F(vertices[i * 3 + 1].x - minpos.x, vertices[i * 3 + 1].y - minpos.y);
		triangle.point3 = D2D1::Point2F(vertices[i * 3 + 2].x - minpos.x, vertices[i * 3 + 2].y - minpos.y);
		triangles.push_back(triangle);
	}

	tex.b = (GuiBrush*)brush;
}
void FilledGeometry::PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const
{
	// Create mesh
	ID2D1Mesh* mesh;
	rendertarget->context->CreateMesh(&mesh);

	// Add triangles
	ID2D1TessellationSink *tsink;
	mesh->Open(&tsink);
	tsink->AddTriangles(&triangles.front(), triangles.size());
	tsink->Close();
	tsink->Release();

	// Deactivate antialiasing
	rendertarget->context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	if(efftexopacity)
		rendertarget->context->FillMesh(mesh, efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
	efftexopacity = 1.0f - efftexopacity;
	if(efftexopacity)
		rendertarget->context->FillMesh(mesh, tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));

	// Reactivate antialiasing
	rendertarget->context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	// Release mesh
	mesh->Release();
}

Line::Line(GuiCompositeImage* parent, const Point<float>& p0, const Point<float>& p1, float strokewidth, IGuiBrush* brush, DockType dock, DockType refdock) : Shape(parent, dock, refdock)
{
	float xmin = min(p0.x, p1.x), ymin = min(p0.y, p1.y);
	this->bounds = Rect<float>(xmin, ymin, max(p0.x, p1.x) - xmin, max(p0.y, p1.y) - ymin);
	this->p0 = Point<float>(p0.x - xmin, p0.y - ymin);
	this->p1 = Point<float>(p1.x - xmin, p1.y - ymin);
	this->strokewidth = strokewidth;
	tex.b = (GuiBrush*)brush;
}

void Line::PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const
{
	if(efftexopacity)
		rendertarget->context->DrawLine(D2D1::Point2F(p0.x, p0.y), D2D1::Point2F(p1.x, p1.y), efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget), strokewidth);
	efftexopacity = 1.0f - efftexopacity;
	if(efftexopacity)
		rendertarget->context->DrawLine(D2D1::Point2F(p0.x, p0.y), D2D1::Point2F(p1.x, p1.y), tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget), strokewidth);
}


Bitmap::Bitmap(GuiCompositeImage* parent, IGuiBitmap* bmp, const Rect<float>& bounds, DockType dock, DockType refdock) : Shape(parent, dock, refdock)
{
	this->bounds = Rect<float>(bounds);
	tex.i = (GuiBitmap*)bmp;
}

void Bitmap::PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const
{
	// Scale image
	D2D1_MATRIX_3X2_F matTrans;
	rendertarget->context->GetTransform(&matTrans);
	matTrans._11 *= bounds.width / tex.i->GetWidth();
	matTrans._22 *= bounds.height / tex.i->GetHeight();
	rendertarget->context->SetTransform(matTrans);

	if(efftexopacity)
		rendertarget->context->DrawImage(tex.i->Paint(rendertarget), D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, D2D1_COMPOSITE_MODE_SOURCE_OVER);
		//rendertarget->context->DrawBitmap(efftex.i->Paint(rendertarget), D2D1::RectF(0.0f, 0.0f, bounds.width, bounds.height), alpha * efftexopacity);
	efftexopacity = 1.0f - efftexopacity;
	if(efftexopacity)
		rendertarget->context->DrawImage(tex.i->Paint(rendertarget), D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, D2D1_COMPOSITE_MODE_SOURCE_OVER);
		//rendertarget->context->DrawBitmap(tex.i->Paint(rendertarget), D2D1::RectF(0.0f, 0.0f, bounds.width, bounds.height), alpha * efftexopacity);
}

TextField::TextField(IGuiTextFormat* format, const Rect<float>& bounds, IGuiBrush* brush, DockType dock, DockType refdock) : Shape(parent, dock, refdock)
{
	this->bounds = Rect<float>(bounds);
	this->format = (GuiTextFormat*)format;
	tex.b = (GuiBrush*)brush;
	layout = NULL;
	allignment = GTA_LEADING;
	/*cachedtextbounds = NULL;*/
}

void TextField::PaintInternal(RenderTarget *rendertarget, Texture efftex, float efftexopacity, float alpha) const
{
	if(!layout)
		return;

//rendertarget->context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

	if(efftexopacity)
		rendertarget->context->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), layout, efftex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
	efftexopacity = 1.0f - efftexopacity;
	if(efftexopacity)
		rendertarget->context->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), layout, tex.b->GetD2DBrush(alpha * efftexopacity, rendertarget));
}

Result TextField::SetText(const String& text)
{
	/*if(cachedtextbounds)
		delete cachedtextbounds; // Invalidate old text bounds*/
	this->text = text;
	Result rlt = Error(dwtfactory->CreateTextLayout(this->text.ToWCharString(), this->text.length(), *format, bounds.width, bounds.height, &layout));
	/*DWRITE_TEXT_RANGE range = {0, this->text.length()};
	layout->SetCharacterSpacing(0.0f, 0.0f, 0.0f, range);*/
	return rlt;
}
void TextField::SetTextAllignment(GuiTextAllignment allignment)
{
	this->allignment = allignment;
	if(layout)
		layout->SetTextAlignment((DWRITE_TEXT_ALIGNMENT)allignment);
}
Result TextField::MeasureText(Rect<float>* bounds)
{
	/*if(cachedtextbounds)
	{
		bounds->x = cachedtextbounds->x;
		bounds->y = cachedtextbounds->y;
		bounds->width = cachedtextbounds->width;
		bounds->height = cachedtextbounds->height;
		return R_OK;
	}*/

	if(!layout)
		return ERR("MeasureText() called on a textfield without text.");

	DWRITE_TEXT_METRICS metrics;
	Result rlt;

	CHKRESULT(layout->GetMetrics(&metrics));
	bounds->x = metrics.left + this->bounds.x;
	bounds->y = metrics.top + this->bounds.y;
	bounds->width = metrics.width;
	bounds->height = metrics.height;
	/*cachedtextbounds = new Rect<float>(*bounds);*/

	return R_OK;
}
void TextField::SetSize(const ::Size<float>& val)
{
	bounds.size = val;
	if(layout)
	{
		layout->SetMaxWidth(val.width);
		layout->SetMaxHeight(val.height);
	}
}
void TextField::SetWidth(float val)
{
	bounds.width = val;
	if(layout)
		layout->SetMaxWidth(val);
}
void TextField::SetHeight(float val)
{
	bounds.height = val;
	if(layout)
		layout->SetMaxHeight(val);
}

/*void TextField::Release()
{
	if(cachedtextbounds)
		delete cachedtextbounds;
}*/