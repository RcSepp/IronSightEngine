#include "Direct3D.h"
#include <DWrite.h>
#include <ISForms.h>

#pragma comment(lib, "dwrite.lib")

#define BMP_SIZE	1024 // Maximum width/height of generated font maps

IDWriteFactory* TextRenderer::dwtfactory = NULL;
IDWriteGdiInterop* TextRenderer::dwtgdiinterop = NULL;
struct DWriteRenderer : IDWriteTextRenderer
{
	IDWriteBitmapRenderTarget* bmprendertarget;
	IDWriteRenderingParams* renderparams;
	RECT dirtyrect;

	STDMETHODIMP DrawGlyphRun(__maybenull void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_MEASURING_MODE measuringMode, __in DWRITE_GLYPH_RUN const* glyphRun, __in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription, IUnknown* clientDrawingEffect)
	{
		return bmprendertarget->DrawGlyphRun(baselineOriginX, baselineOriginY, measuringMode, glyphRun, renderparams, RGB(255, 255, 255), &dirtyrect);
	}
	STDMETHOD(DrawUnderline)(_In_opt_ void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, _In_ DWRITE_UNDERLINE const* underline, _In_opt_ IUnknown* clientDrawingEffect)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(DrawStrikethrough)(_In_opt_ void* clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, _In_ DWRITE_STRIKETHROUGH const* strikethrough, _In_opt_ IUnknown* clientDrawingEffect)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(DrawInlineObject)(_In_opt_ void* clientDrawingContext, FLOAT originX, FLOAT originY,  IDWriteInlineObject* inlineObject, BOOL isSideways, BOOL isRightToLeft, _In_opt_ IUnknown* clientDrawingEffect)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(IsPixelSnappingDisabled)(_In_opt_ void* clientDrawingContext, _Out_ BOOL* isDisabled)
	{
		*isDisabled = FALSE;
		return S_OK;
	}
	STDMETHOD(GetCurrentTransform)(_In_opt_ void* clientDrawingContext, _Out_ DWRITE_MATRIX* transform)
	{
		return bmprendertarget->GetCurrentTransform(transform);
	}
	STDMETHOD(GetPixelsPerDip)( _In_opt_ void* clientDrawingContext, _Out_ FLOAT* pixelsPerDip)
	{
		*pixelsPerDip = bmprendertarget->GetPixelsPerDip();
		return S_OK;
	}

	// >>> Simple IUnknown implementation
	DWORD iunknownrefctr;
	DWriteRenderer() : iunknownrefctr(1) {}
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
		{if(ppvObject) return S_OK; else return E_POINTER;}
	ULONG STDMETHODCALLTYPE AddRef()
		{return ++iunknownrefctr;}
	ULONG STDMETHODCALLTYPE Release()
	{
		long temp = --iunknownrefctr;
		if(!temp)
		{
			if(bmprendertarget)
				bmprendertarget->Release();
			if(renderparams)
				renderparams->Release();
			delete this;
		}
		return temp;
	}
	// <<< Simple IUnknown implementation
	}* TextRenderer::dwtrenderer = NULL;

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize TextRenderer
//-----------------------------------------------------------------------------
Result TextRenderer::Init(OutputWindow* wnd)
{
	Result rlt;

	this->wnd = wnd;

	if(!dwtfactory)
	{
		// Create DirectWrite factory
		CHKRESULT(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwtfactory)));
		CHKRESULT(dwtfactory->GetGdiInterop(&dwtgdiinterop));

		// Create GDI-compatible rendertarget
		HDC hdcscreen = GetDC(NULL);
		dwtrenderer = new DWriteRenderer();
		CHKRESULT(dwtgdiinterop->CreateBitmapRenderTarget(hdcscreen, BMP_SIZE, BMP_SIZE, &dwtrenderer->bmprendertarget));
		ReleaseDC(NULL, hdcscreen);

		// Create monitor rendering params, but with cleartype disabled
		const POINT pt = {0, 0};
		CHKRESULT(dwtfactory->CreateMonitorRenderingParams(MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &dwtrenderer->renderparams));
		float gamma = dwtrenderer->renderparams->GetGamma();
		float ehcctr = dwtrenderer->renderparams->GetEnhancedContrast();
		DWRITE_PIXEL_GEOMETRY geometry = dwtrenderer->renderparams->GetPixelGeometry();
		DWRITE_RENDERING_MODE rmode = dwtrenderer->renderparams->GetRenderingMode();
		dwtrenderer->renderparams->Release();
		CHKRESULT(dwtfactory->CreateCustomRenderingParams(gamma, ehcctr, 0.0f, geometry, rmode, &dwtrenderer->renderparams));
	}
	else if(dwtrenderer)
		dwtrenderer->AddRef();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateFont()
// Desc: Create a font map from the given font
//-----------------------------------------------------------------------------
Result TextRenderer::CreateD3dFont(const FontType& type, ID3dFont** font)
{
	if(!dwtrenderer)
		return ERR("Can't generate font. DirectWrite renderer unavailable");
	Result rlt;

	std::map<FontType, Font*>::iterator pair = fonts.find(type);
	if(pair != fonts.end())
	{
		*font = pair->second;
		return R_OK; // Font map of given type already created
	}

	*font = NULL;
	Font* newfont = new Font(this, type);
	newfont->linespacing = 0.0f;

	// Clear memory DC to erase any previously drawn font maps
	RECT rct;
	rct.left = rct.top = 0;
	rct.right = rct.bottom = BMP_SIZE;
	HBRUSH transparentbrush = CreateSolidBrush(0x00000000);
	FillRect(dwtrenderer->bmprendertarget->GetMemoryDC(), &rct, transparentbrush);
	DeleteObject(transparentbrush);

	// Create text format
	IDWriteTextFormat *fmt;
	CHKRESULT(dwtfactory->CreateTextFormat(type.family.ToWCharString(), NULL, type.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
										   type.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, type.size, L"en-us", &fmt));

	// Draw font map
	IDWriteTextLayout *layout;
	Vector2 pos(0.0f, 0.0f);
	Size<UINT> bmpsize(0, 0);
	UINT numprintablecharacters = 0;
	WCHAR wchr;
	DWRITE_TEXT_METRICS metrics;
	for(UINT i = 0; i < 256; i++)
	{
		// Draw string
		mbtowc(&wchr, (char*)&i, 1);
		CHKRESULT(dwtfactory->CreateTextLayout(&wchr, 1, fmt, type.size, type.size, &layout));
		CHKRESULT(layout->Draw(NULL, dwtrenderer, pos.x, pos.y));

		// Inflate texture size
		bmpsize.width = max(bmpsize.width, (UINT)dwtrenderer->dirtyrect.right);
		bmpsize.height = max(bmpsize.height, (UINT)dwtrenderer->dirtyrect.bottom);

		// Get text metrics
		CHKRESULT(layout->GetMetrics(&metrics));

		// Define character properties
		Font::Character& chr = newfont->chars[i];
		chr.charspacing = metrics.widthIncludingTrailingWhitespace;

		if(dwtrenderer->dirtyrect.right > dwtrenderer->dirtyrect.left) // If the character is printable
		{
			// Define printable character properties
			chr.isprintable = true;
			Vec2Set(&chr.texcoord, (float)dwtrenderer->dirtyrect.left, floor(pos.y));
			Vec2Set(&chr.texsize, (float)(dwtrenderer->dirtyrect.right - dwtrenderer->dirtyrect.left + 1), (float)dwtrenderer->dirtyrect.bottom - floor(pos.y) + 1.0f); // +1 ... tolearance
			newfont->linespacing = max(newfont->linespacing, metrics.height);

			// Update draw position for next character
			pos.x = ceil((float)dwtrenderer->dirtyrect.right) + 2.0f; // +2 ... tolearance
			if(pos.x + type.size > (float)BMP_SIZE || (numprintablecharacters && ((numprintablecharacters & 0x1F) == 0))) // If line is full or contains 32 characters
			{
				// Line feed
				pos.x = 0.0f;
				pos.y += metrics.height;
			}

			numprintablecharacters++;
		}
		else // If the character isn't printable
			chr.isprintable = false;
	}

	// Make size a power of two
	auto GetNextPow2 = [](UINT v) -> UINT
	{
		UINT p2 = 1;
		while(p2 < v)
			p2 <<= 1;
		return p2;
	};
	bmpsize.width = GetNextPow2(bmpsize.width);
	bmpsize.height = GetNextPow2(bmpsize.height);

	// Scale texcoords and texsize's
	Vector2 invsize(1.0f / (float)bmpsize.width, 1.0f / (float)bmpsize.height);
	for(UINT i = 0; i < 256; i++)
		if(newfont->chars[i].isprintable)
		{
			newfont->chars[i].texcoord *= invsize;
			newfont->chars[i].texsize *= invsize;
		}
	

	// Get pixel data
	UINT32* bmppixels = new UINT32[BMP_SIZE * BMP_SIZE];
	CHKALLOC(bmppixels);
	HBITMAP bmp = (HBITMAP)GetCurrentObject(dwtrenderer->bmprendertarget->GetMemoryDC(), OBJ_BITMAP);
	int irlt = GetBitmapBits(bmp, 4 * BMP_SIZE * BMP_SIZE, bmppixels);

	// Copy pixel data from (BMP_SIZE * BMP_SIZE) source to (bmpsize.width * bmpsize.heigh) destination
	UINT32* texpixels = new UINT32[bmpsize.width * bmpsize.height];
	CHKALLOC(texpixels);
	UINT32 pixel;
	for(UINT y = 0; y < bmpsize.height; y++)
		for(UINT x = 0; x < bmpsize.width; x++)
		{
			pixel = bmppixels[y * BMP_SIZE + x];
			texpixels[y * bmpsize.width + x] = 0xFFFFFF | max(pixel & 0xFF, max((pixel >> 8) & 0xFF, (pixel >> 16) & 0xFF)) << 24; // alpha = max(r, g, b); rgb = 0xFF
//texpixels[y * bmpsize.width + x] = bmppixels[y * BMP_SIZE + x] | 0xFF000000; // For debugging
		}
	delete[] bmppixels;

	// Create texture
	CHKRESULT(wnd->CreateTexture(bmpsize.width, bmpsize.height, ITexture::TU_SKIN_NOMIPMAP, false, DXGI_FORMAT_R8G8B8A8_UNORM, (LPVOID)texpixels, &newfont->tex));
	delete[] texpixels;
//CHKRESULT(newfont->tex->Save("font.png", D3DX11_IFF_PNG)); // For debugging

	fonts[type] = newfont;
	*font = newfont;

	return R_OK;
}

void TextRenderer::Font::DrawTextImmediate(LPSPRITECONTAINER spritecontainer, const String text, const Rect<float>& bounds, const Color& clr, UINT flags)
{
	if(!text.IsEmpty())
		DrawTextInternal(spritecontainer, text, bounds, clr, flags, 0);
}
void TextRenderer::Font::DrawTextDelayed(LPSPRITECONTAINER spritecontainer, const String text, const Rect<float>& bounds, const Color& clr, UINT flags)
{
	if(!text.IsEmpty())
		DrawTextInternal(spritecontainer, text, bounds, clr, flags, 1);
}
void TextRenderer::Font::MeasureText(const String text, float fixedwidth, UINT flags, Size<float>* measuredsize)
{
	measuredsize->width = measuredsize->height = 0.0f;
	if(!text.IsEmpty())
	{
		if(fixedwidth <= 0.0f)
			fixedwidth = 1e20f;
		DrawTextInternal(NULL, text, Rect<float>(0.0f, 0.0f, fixedwidth, 1e20f), Color(), flags, 2, measuredsize);
	}
}
void TextRenderer::Font::DrawTextInternal(LPSPRITECONTAINER spritecontainer, const String text, const Rect<float>& bounds, const Color& clr, UINT flags, BYTE drawtype, Size<float>* measuredsize)
{
	Sprite sprtchar;
	sprtchar.color = clr;
	sprtchar.tex = tex;

	const float bbw = (float)parent->wnd->backbuffersize.width;
	const float bbh = (float)parent->wnd->backbuffersize.height;
	const float tw = (float)tex->GetWidth();
	const float th = (float)tex->GetHeight();

	bool NOCLIP = (flags & DF_NOCLIP) != 0;
	bool SINGLELINE = (flags & DF_SINGLELINE) != 0;
	bool BREAKWORDS = (flags & DF_BREAKWORDS) != 0;

	auto getcharend = [&](char chr, float chrpos) -> float
	{
		if(chr == '\t')
		{
			float tabsize = chars['\t'].charspacing;
			return (floor(chrpos / tabsize) + 1.0f) * tabsize;
		}
		else
			return chrpos + chars[chr].charspacing;
	};

	float x = bounds.x, y = bounds.y, charend, wordend;
	const unsigned char *c = (const unsigned char*)text.ToCharString();
	bool breakword = false;
	while(*c)
	{
		// >>> Determine whether to perform a line break

		if(!NOCLIP)
		{
			// Handle newline character
			if(!SINGLELINE && *c == '\n')
			{
				x = bounds.x;
				y += linespacing;
				if(y + linespacing > bounds.bottom())
					break; // Quit drawing text if it exceeded the horizontal bounds
				c++; // Character handled
				continue;
			}

			// Handle breaking of full lines
			charend = getcharend(*c, x);
			if(!BREAKWORDS && isalnum(*c))
			{
				// wordend = begin of next alphanumeric character sequence
				wordend = x;
				const unsigned char* cc;
				for(cc = c; *cc && isalnum(*(const char*)cc) && wordend <= bounds.right(); cc++)
					wordend = getcharend(*cc, wordend);
				for(; *cc && !isalnum(*(const char*)cc) && wordend <= bounds.right(); cc++)
					wordend = getcharend(*cc, wordend);
			}
			else
				wordend = charend; // wordend = end of character to be drawn
			if(wordend > bounds.right())
			{
				if(SINGLELINE)
					break; // Quit drawing text if single-line text exceeded the horizontal bounds
				if(x == bounds.x || (breakword && charend <= bounds.right()))
				{
					if(charend > bounds.right())
						break; // Quit drawing text if less than one character fits in bounds.width
					breakword = true; // Skip line break if a single word doesn't fit on the line, but the next character does
				}
				else
				{
					// Break line
					breakword = false;
					x = bounds.x;
					y += linespacing;
					if(y + linespacing > bounds.bottom())
						break; // Quit drawing text if it exceeded the vertical bounds
					continue;
				}
			}
		}

		// >>> Print character

		if(chars[*c].isprintable)
		{
			if(drawtype < 2) // DrawText...() called
			{
				sprtchar.texcoord = chars[*c].texcoord;
				sprtchar.texsize = chars[*c].texsize;

				Vector2 scl(1.0f / (sprtchar.texsize.x * tw), 1.0f / (sprtchar.texsize.y * th));
				Matrix mattodevicespace;
				MatrixTranslation(&mattodevicespace, -1.0f + 2.0f * x / bbw, 1.0f - 2.0f * y / bbh, 0.0f);
				MatrixScale(&mattodevicespace, &mattodevicespace, 2.0f / (bbw * scl.x), 2.0f / (bbh * scl.y), 1.0f);
				MatrixTranslate(&mattodevicespace, &mattodevicespace, 0.5f, -0.5f, 0.0f);
				sprtchar.worldmatrix = mattodevicespace;
				if(drawtype)
					spritecontainer->DrawSpriteDelayed(&sprtchar);
				else
					spritecontainer->DrawSpriteImmediate(&sprtchar);
			}
			else // MeasureText() called
			{
				measuredsize->width = max(measuredsize->width, x + chars[*c].charspacing);
				measuredsize->height = max(measuredsize->height, y + linespacing);
			}
		}

		// >>> Advance character pointer and position

		x = getcharend(*c, x);
		c++;
	}
}

TextRenderer::~TextRenderer()
{
	while(fonts.size())
		fonts.begin()->second->Release();
	//if(dwtrenderer)
	//	dwtrenderer->Release(); //EDIT: Remove static vars using reference counting
}