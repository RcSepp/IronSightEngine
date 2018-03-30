#ifndef __GUICONTROL_H
#define __GUICONTROL_H

class RenderArea;

//-----------------------------------------------------------------------------
// Name: GuiControl
// Desc: A generic control in GuiFactory space
//-----------------------------------------------------------------------------
class GuiControl : public IGuiControl, public MouseEventReceiver, public AnimatedPoint<float>::IChangeHandler,
				   public AnimatedSize<float>::IChangeHandler, public AnimatedValue<float>::IChangeHandler
{
private:
	class TextField
	{
		DockType dock, refdock;
		GuiTextFormat* format;
		IDWriteTextLayout* layout;
		Rect<float> bounds;
		String text;
		/*Rect<float>* cachedtextbounds;*/

	public:
		TextField(IGuiTextFormat* format, const Rect<float>& bounds, DockType dock, DockType refdock);

		void Paint(RenderTarget *rendertarget, GuiBrush* brush, float opacity, const Rect<float>& areabounds) const;

		bool CheckMousePos(const Point<float>& mousepos, const Rect<float>& bounds) const
		{
			return mousepos.x >= bounds.left() && mousepos.x <= bounds.right() &&
				   mousepos.y >= bounds.top() && mousepos.y <= bounds.bottom();
		}

		String& GetText() {return text;}
		Result SetText(const String& text);
		Result MeasureText(Rect<float>* bounds);
		::Size<float> GetSize() {return bounds.size;}
		const ::Size<float>& SetSize(const ::Size<float>& val);
		float GetWidth() {return bounds.width;}
		float SetWidth(float val);
		float GetHeight() {return bounds.height;}
		float SetHeight(float val);

		/*void Release();*/
	}* textfield;

public:
	RenderArea* parent;
	AnimatedPoint<float> pos;
	AnimatedSize<float> size;
	AnimatedValue<float> alpha;
	String text;

	MOUSE_CALLBACK downcbk, upcbk, movecbk, entercbk, leavecbk;
	LPVOID downcbkuser, upcbkuser, movecbkuser, entercbkuser, leavecbkuser;
	GuiImage *img, *overimg, *downimg;
	GuiBrush *brush, *overbrush, *downbrush;
	AnimatedValue<float> overfade, downfade;
	TransitionScript::IScript *overfadedesc, *downfadedesc;
	bool oldmouseover, mousedowninsideregion; // Used inside OnMouseMove

	GuiControl(RenderArea* parent, DockType dock, DockType refdock);

	bool TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds);
	void PointToClient(Point<float>& mousepos, Rect<float>* bounds = NULL);
	void Paint(RenderTarget *rendertarget, const Rect<float>& areabounds) const;

//	bool TransformMousePos(const Point<float>& mousepos, Rect<float>& bounds);
	bool OnMouseDown(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt);
	void OnMouseUp(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt);
	void OnMouseMove(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt);
	bool OnMouseDoubleClick(const MouseEventArgs& args, const Point<float>& mousepos, const Rect<float>& bounds, MouseEventResult** rlt);

	void BringToFront();

	void SetMouseDownCallback(const MOUSE_CALLBACK downcbk, LPVOID user) {this->downcbk = downcbk; downcbkuser = user;}
	void SetMouseUpCallback(const MOUSE_CALLBACK upcbk, LPVOID user) {this->upcbk = upcbk; upcbkuser = user;}
	void SetMouseMoveCallback(const MOUSE_CALLBACK movecbk, LPVOID user) {this->movecbk = movecbk; movecbkuser = user;}
	void SetMouseEnterCallback(const MOUSE_CALLBACK entercbk, LPVOID user) {this->entercbk = entercbk; entercbkuser = user;}
	void SetMouseLeaveCallback(const MOUSE_CALLBACK leavecbk, LPVOID user) {this->leavecbk = leavecbk; leavecbkuser = user;}

	void SetMouseOverEffect(LPGUIBITMAP effectimage, LPGUIBRUSH effectbrush, TransitionScript::IScript& desc)
		{overimg = (GuiBitmap*)effectimage; overbrush = (GuiBrush*)effectbrush; REMOVE(overfadedesc); overfadedesc = TransitionScript::IScript::Copy(desc);}
	void SetMouseOverEffect(LPGUICOMPOSITEIMAGE effectimage, LPGUIBRUSH effectbrush, TransitionScript::IScript& desc)
		{overimg = (GuiCompositeImage*)effectimage; overbrush = (GuiBrush*)effectbrush; REMOVE(overfadedesc); overfadedesc = TransitionScript::IScript::Copy(desc);}
	void SetMouseDownEffect(LPGUIBITMAP effectimage, LPGUIBRUSH effectbrush, TransitionScript::IScript& desc)
		{downimg = (GuiBitmap*)effectimage; downbrush = (GuiBrush*)effectbrush; REMOVE(downfadedesc); downfadedesc = TransitionScript::IScript::Copy(desc);}
	void SetMouseDownEffect(LPGUICOMPOSITEIMAGE effectimage, LPGUIBRUSH effectbrush, TransitionScript::IScript& desc)
		{downimg = (GuiCompositeImage*)effectimage; downbrush = (GuiBrush*)effectbrush; REMOVE(downfadedesc); downfadedesc = TransitionScript::IScript::Copy(desc);}

	void CreateTextField(IGuiTextFormat* format, IGuiBrush* brush, const Rect<float>& bounds, DockType dock, DockType refdock);

	void AnimatedPoint<float>::IChangeHandler::OnValueChanged(Point<float> oldpoint, Point<float> newpoint, ONANIMATE_CALLBACK cbkanimate, LPVOID cbkuser)
	{
		cbkanimate(cbkuser); //EDIT: Invalidate src and dest rects instead of the whole rendertarget
	}
	void AnimatedSize<float>::IChangeHandler::OnValueChanged(::Size<float> oldsize, ::Size<float> newsize, ONANIMATE_CALLBACK cbkanimate, LPVOID cbkuser)
	{
		cbkanimate(cbkuser); //EDIT: Invalidate src and dest rects instead of the whole rendertarget
	}
	void AnimatedValue<float>::IChangeHandler::OnValueChanged(float val, ONANIMATE_CALLBACK cbkanimate, LPVOID cbkuser)
	{
		cbkanimate(cbkuser);//InvalidateRect(gui->GetTargetWnd(), Rect<float>(pos.GetPoint(), size.GetSize()), FALSE);
	}

	void Release();

	Point<float> GetLocation() const {return pos.GetPoint();}
	void SetLocation(const Point<float>& val) {pos.SetPoint(val);}
	float GetLeft() const {return pos.GetX();}
	void SetLeft(float val) {pos.SetX(val);}
	float GetTop() const {return pos.GetY();}
	void SetTop(float val) {pos.SetY(val);}

	::Size<float> GetSize() const {return size.GetSize();}
	void SetSize(const ::Size<float>& val) {size.SetSize(val);}
	float GetWidth() const {return size.GetWidth();}
	void SetWidth(float val) {size.SetWidth(val);}
	float GetHeight() const {return size.GetHeight();}
	void SetHeight(float val) {size.SetHeight(val);}

	DockType GetDock() const {return dock;}
	void SetDock(DockType val) {dock = val;}
	DockType GetRefDock() const {return refdock;}
	void SetRefDock(DockType val) {refdock = val;}

	void SetImage(const LPGUIBITMAP val) {img = (GuiBitmap*)val;}
	LPGUIBITMAP GetBitmapImage() {return (GuiBitmap*)img;}
	void SetImage(const LPGUICOMPOSITEIMAGE val) {img = (GuiCompositeImage*)val;}
	LPGUICOMPOSITEIMAGE GetCompositeImage() {return (GuiCompositeImage*)img;}

	float GetAlpha() const {return alpha.GetVal();}
	void SetAlpha(float val) {alpha.SetVal(val);}

	void SetText(const String& val) {text = val; if(textfield) textfield->SetText(text);}
	String& GetText() {return text;}
	::Size<float> GetTextSize() const {return textfield->GetSize();}
	const ::Size<float>& SetTextSize(const ::Size<float>& val) {return textfield->SetSize(val);}
	float GetTextWidth() const {return textfield->GetWidth();}
	float SetTextWidth(float val) {return textfield->SetWidth(val);}
	float GetTextHeight() const {return textfield->GetHeight();}
	float SetTextHeight(float val) {return textfield->SetHeight(val);}
	Result MeasureText(Rect<float>* bounds) {if(textfield) return textfield->MeasureText(bounds); else return ERR("MeasureText() called on a control without any text fields.");}
	Result FitTextSizeToText();

	LPGUIBRUSH GetTextBrush() const {return brush;}
	void SetTextBrush(const LPGUIBRUSH val) {brush = (GuiBrush*)val;}
};

#endif