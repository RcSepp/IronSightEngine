#include "Direct3D.h"
#include <ISDirectInput.h>


//-----------------------------------------------------------------------------
// Name: HUD()
// Desc: Constructor
//-----------------------------------------------------------------------------
HUD::HUD()
{
	wnd = NULL;
	spritecontainer = NULL;
	memset(mousebuttonslastframe, 0, sizeof(mousebuttonslastframe));
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize HUD
//-----------------------------------------------------------------------------
Result HUD::Init(OutputWindow* wnd)
{
	Result rlt;

	this->wnd = wnd;
	if(!wnd)
		return ERR("Parameter wnd is NULL");

	// Create sprite container
	CHKRESULT(wnd->CreateSpriteContainer(0, &spritecontainer));
	spritecontainer->SetIdentityViewTransform();
	spritecontainer->SetIdentityProjectionTransform();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateElement()
// Desc: Create and store HUD element
//-----------------------------------------------------------------------------
Result HUD::CreateElement(ITexture* tex, int x, int y, DockType dock, Color color, int zorder, LPIMAGEELEMENT* element, LPVOID elementuser)
{
	if(!tex)
		return ERR("Parameter tex can't be NULL");

	ImageElement* newelement;

	if(element)
		*element = NULL;

	CHKALLOC(newelement = new ImageElement(zorder)); // Allocate memory
	newelement->sprt.tex = tex; // Set texture

	// Set misc
	newelement->tex = (Texture*)tex;
	newelement->visible = true;
	newelement->color = color;
	newelement->_x = x;
	newelement->_y = y;
	newelement->_dock = dock;
	newelement->subregion = Rect<int>(0, 0, newelement->tex->width, newelement->tex->height);
	newelement->poschanged = true;

	// Store element
	elements.insert(newelement);

	if(element)
	{
		newelement->user = elementuser;
		*element = newelement;
	}

	return R_OK;
}
Result HUD::CreateElement(ITexture* tex, int x, int y, DockType dock, Color color, const Rect<int>& subregion, int zorder, LPIMAGEELEMENT* element, LPVOID elementuser)
{
	if(!tex)
		return ERR("Parameter tex can't be NULL");

	ImageElement* newelement;

	if(element)
		*element = NULL;

	CHKALLOC(newelement = new ImageElement(zorder)); // Allocate memory
	newelement->sprt.tex = tex; // Set texture

	/*// Set empty text
	newelement->text.Clear();
	newelement->textfont = NULL;
	newelement->textcolor = 0xFFFFFFFF;
	newelement->textflags = 0;*/

	// Set misc
	newelement->tex = (Texture*)tex;
	newelement->visible = true;
	newelement->color = color;
	newelement->_x = x;
	newelement->_y = y;
	newelement->_dock = dock;
	newelement->subregion = subregion;
	newelement->poschanged = true;

	// Store element
	elements.insert(newelement);

	if(element)
	{
		newelement->user = elementuser;
		*element = newelement;
	}

	return R_OK;
}
Result HUD::CreateElement(String text, LPD3DFONT font, int x, int y, DockType dock, Color color, const Size<int>& size, int zorder, UINT textflags, LPTEXTELEMENT* element, LPVOID elementuser)
{
	TextElement* newelement;

	if(element)
		*element = NULL;

	CHKALLOC(newelement = new TextElement(zorder)); // Allocate memory

	// Set text
	newelement->text = text;
	newelement->font = font;
	newelement->color = color;
	newelement->textflags = textflags;

	// Set misc
	newelement->visible = true;
	newelement->_x = x;
	newelement->_y = y;
	newelement->_dock = dock;
	if(size.width || size.height)
		newelement->size = size;
	else if(font)
	{
		Size<float> measuredtextsize;
		font->MeasureText(text, 0.0f, textflags, &measuredtextsize);
		newelement->size.width = (int)ceil(measuredtextsize.width);
		newelement->size.height = (int)ceil(measuredtextsize.height);
	}
	else
		return ERR("Can't measure text size if font is NULL");
	newelement->textbounds = Rect<float>(0.0f, 0.0f, 0.0f, 0.0f);
	newelement->poschanged = true;

	// Store element
	elements.insert(newelement);

	if(element)
	{
		newelement->user = elementuser;
		*element = newelement;
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: RemoveElement()
// Desc: Remove HUD element
//-----------------------------------------------------------------------------
void HUD::RemoveElement(LPELEMENT element)
{
	elements.remove(element);
	delete element; //EDIT: Does destructor get called properly?
}

//-----------------------------------------------------------------------------
// Name: RegisterMouse...Listener()
// Desc: Add MouseEventListener entry to mouselisteners or update existing one with new callback
//-----------------------------------------------------------------------------
HUD::CMouseEventListener* HUD::FindCMouseEventListener(LPELEMENT element)
{
	CMouseEventListener* listener;
	std::map<std::pair<IElement*, bool>, MouseEventListener*>::iterator pair = mouselisteners.find(std::pair<IElement*, bool>(element, false));
	if(pair == mouselisteners.end())
		mouselisteners[std::pair<IElement*, bool>(element, false)] = listener = new CMouseEventListener();
	else
		listener = (CMouseEventListener*)pair->second;
	return listener;
}
void HUD::RegisterMouseDownListener(IElement* element, MOUSEEVENT_CALLBACK cbk) {FindCMouseEventListener(element)->mousedowncbk = cbk;}
void HUD::RegisterMouseUpListener(IElement* element, MOUSEEVENT_CALLBACK cbk) {FindCMouseEventListener(element)->mouseupcbk = cbk;}
void HUD::RegisterMouseMoveListener(IElement* element, MOUSEEVENT_CALLBACK cbk) {FindCMouseEventListener(element)->mousemovecbk = cbk;}
void HUD::RegisterMouseEnterListener(IElement* element, MOUSEEVENT_CALLBACK cbk) {FindCMouseEventListener(element)->mouseentercbk = cbk;}
void HUD::RegisterMouseLeaveListener(IElement* element, MOUSEEVENT_CALLBACK cbk) {FindCMouseEventListener(element)->mouseleavecbk = cbk;}
HUD::PyMouseEventListener* HUD::FindPyMouseEventListener(LPELEMENT element)
{
	PyMouseEventListener* listener;
	std::map<std::pair<IElement*, bool>, MouseEventListener*>::iterator pair = mouselisteners.find(std::pair<IElement*, bool>(element, true));
	if(pair == mouselisteners.end())
		mouselisteners[std::pair<IElement*, bool>(element, true)] = listener = new PyMouseEventListener();
	else
		listener = (PyMouseEventListener*)pair->second;
	return listener;
}
void HUD::RegisterPyMouseDownListener(IElement* element, PyObject* pycbk) {FindPyMouseEventListener(element)->mousedownpycbk = pycbk;}
void HUD::RegisterPyMouseUpListener(IElement* element, PyObject* pycbk) {FindPyMouseEventListener(element)->mouseuppycbk = pycbk;}
void HUD::RegisterPyMouseMoveListener(IElement* element, PyObject* pycbk) {FindPyMouseEventListener(element)->mousemovepycbk = pycbk;}
void HUD::RegisterPyMouseEnterListener(IElement* element, PyObject* pycbk) {FindPyMouseEventListener(element)->mouseenterpycbk = pycbk;}
void HUD::RegisterPyMouseLeaveListener(IElement* element, PyObject* pycbk) {FindPyMouseEventListener(element)->mouseleavepycbk = pycbk;}

//-----------------------------------------------------------------------------
// Name: Render(RenderType rendertype)
// Desc: Draw HUD to screen
//-----------------------------------------------------------------------------
void HUD::Render(RenderType rendertype)
{
	if(elements.empty())
		return;
	ImageElement* ielement;
	TextElement* telement;

	spritecontainer->Begin();
	std::list<IElement*>::const_iterator element_itr;
	LIST_FOREACH(elements, element_itr)
		if((*element_itr)->visible)
		{
			if((*element_itr)->type == ET_IMAGE)
			{
				ielement = (ImageElement*)*element_itr;
				if(ielement->poschanged)
				{
					// Rescale to screen coordinates and set position
					int docksignx = (ielement->_dock & 0xF) - 1;
					int docksigny = 1 - ((ielement->_dock >> 4) & 0xF);
					Matrix mTranslate1, mTranslate2, mTranslate3, mScale1, mScale2;
					MatrixTranslation(&mTranslate1, -0.5f * docksignx, -0.5f * docksigny, 0.0f); // Set reference point on sprite (0.5f = left, 0.0f = center, -0.5f = right)
					MatrixScaling(&mScale1, (float)ielement->subregion.width, (float)ielement->subregion.height, 0.0f); // Scale sprite
					MatrixTranslation(&mTranslate2, (float)ielement->_x * (docksignx == 1 ? -1 : 1), (float)ielement->_y * (docksigny == -1 ? 1 : -1), 0.0f); // Set offset
					MatrixScaling(&mScale2, 2.0f / (float)wnd->GetBackbufferSize().width, 2.0f / (float)wnd->GetBackbufferSize().height, 0.0f); // Scale offset
					MatrixTranslation(&mTranslate3, 1.0f * docksignx, 1.0f * docksigny, 0.0f); // Set docking point on frame (-1.0f = left, 0.0f = center, 1.0f = right)
					ielement->sprt.worldmatrix = mTranslate1 * mScale1 * mTranslate2 * mScale2 * mTranslate3;
					ielement->sprt.texcoord = Vector2((float)ielement->subregion.x / (float)ielement->tex->width, (float)ielement->subregion.y / (float)ielement->tex->height);
					ielement->sprt.texsize = Vector2((float)ielement->subregion.width / (float)ielement->tex->width, (float)ielement->subregion.height / (float)ielement->tex->height);
					ielement->poschanged = false;
				}
//ielement->tex->Render(wnd->devspec->device);
				ielement->sprt.color = ielement->color;
				spritecontainer->DrawSpriteImmediate(&ielement->sprt);
			}
			else // (*element_itr)->type == ET_TEXT
			{
				telement = (TextElement*)*element_itr;
				if(telement->poschanged)
				{
					switch(telement->dock & 0xF)
					{
					case 0: // DockType == DT_...LEFT
						telement->textbounds.x = (float)telement->_x;
						break;
					case 1: // DockType == DT_...CENTER
						telement->textbounds.x = (float)((wnd->backbuffersize.width - telement->size.width) / 2 + telement->_x);
						break;
					case 2: // DockType == DT_...RIGHT
						telement->textbounds.x = (float)(wnd->backbuffersize.width - telement->size.width - telement->_x);
						break;
					}
					switch((telement->dock >> 4) & 0xF)
					{
					case 0: // DockType == DT_TOP...
						telement->textbounds.y = (float)telement->_y;
						break;
					case 1: // DockType == DT_MIDDLE...
						telement->textbounds.y = (float)((wnd->backbuffersize.height - telement->size.height) / 2 + telement->_y);
						break;
					case 2: // DockType == DT_BOTTOM...
						telement->textbounds.y = (float)(wnd->backbuffersize.height - telement->size.height - telement->_y);
						break;
					}
					telement->textbounds.width = (float)telement->size.width;
					telement->textbounds.height = (float)telement->size.height;
					telement->poschanged = false;
				}
				if(telement->font && !telement->text.IsEmpty())
					telement->font->DrawTextImmediate(spritecontainer, telement->text, telement->textbounds, telement->color, telement->textflags);
			}
		}
	spritecontainer->End();
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Call mouse event listeners. This should be called during input acquiring (usually at the beginning of the frame)
//-----------------------------------------------------------------------------
void HUD::Update()
{
	if(!dip || mouselisteners.empty())
		return;
	ImageElement* ielement;
	TextElement* telement;

	// Get cursor position
	POINT mousepos;
	GetCursorPos(&mousepos);
	POINT clientpos = {0, 0};
	ClientToScreen(wnd->GetHwnd(), &clientpos);
	mousepos.x -= clientpos.x;
	mousepos.y -= clientpos.y;

	// Get mouse button state
	bool mousedown = false, mouseup = false;
	for(BYTE i = 0; i < ARRAYSIZE(mousebuttonslastframe); i++)
	{
		if(dip->mouse.rgbButtons[i] && !mousebuttonslastframe[i])
			mousedown = true;
		if(mousebuttonslastframe[i] && !dip->mouse.rgbButtons[i])
			mouseup = true;
	}
	memcpy(mousebuttonslastframe, dip->mouse.rgbButtons, sizeof(mousebuttonslastframe));

	std::map<std::pair<IElement*, bool>, MouseEventListener*>::iterator event_itr;
	bool elementcontainscursor;
	LIST_FOREACH(mouselisteners, event_itr)
		if(event_itr->first.first->visible)
		{
			// Check whether the element contains the mouse cursor
			if(event_itr->first.first->type == ET_IMAGE)
			{
				ielement = (ImageElement*)event_itr->first.first;
				Rect<int> imagebounds(ielement->_x, ielement->_y, ielement->subregion.width, ielement->subregion.height); //EDIT: Precompute when poschanged == true
				elementcontainscursor = imagebounds.Contains(mousepos.x, mousepos.y);
			}
			else // event_itr->first->type == ET_TEXT
			{
				telement = (TextElement*)event_itr->first.first;
				elementcontainscursor = telement->textbounds.Contains((float)mousepos.x, (float)mousepos.y);
			}

			// Fire events
			if(elementcontainscursor)
			{
				if(!event_itr->second->mouseoverlastframe)
					event_itr->second->RaiseMouseEnter(event_itr->first.first);
				if(dip->mouse.lX || dip->mouse.lY)
					event_itr->second->RaiseMouseMove(event_itr->first.first);
				if(mousedown)
					event_itr->second->RaiseMouseDown(event_itr->first.first);
				if(mouseup)
					event_itr->second->RaiseMouseUp(event_itr->first.first);
				event_itr->second->mouseoverlastframe = true;
			}
			else
			{
				if(event_itr->second->mouseoverlastframe)
					event_itr->second->RaiseMouseLeave(event_itr->first.first);
				event_itr->second->mouseoverlastframe = false;
			}
		}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void HUD::Release()
{
	wnd->DeregisterFromRendering(this);
	wnd->huds.remove(this);

	std::list<IElement*>::iterator iter;
	LIST_FOREACH(elements, iter)
		delete *iter;
	RELEASE(spritecontainer);

	delete this;
}