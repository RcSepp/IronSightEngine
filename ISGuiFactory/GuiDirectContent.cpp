#include "GuiFactory.h"


//-----------------------------------------------------------------------------
// Name: PrepareContent()
// Desc: Create backbuffer
//-----------------------------------------------------------------------------
Result D2dDirectContent::PrepareContent(Size<UINT>& srcsize)
{
	Result rlt;

	if(reinterpret_cast<GuiFactory*>(&*gui)->wnds.front()->rendertarget && (!d2dbmp || srcsize != this->srcsize))
	{
		this->srcsize = srcsize;
		CHKRESULT(OnCreateDevice(reinterpret_cast<GuiFactory*>(&*gui)->wnds.front()->rendertarget));
	}
	else
		this->srcsize = srcsize;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetContent()
// Desc: Store direct content to backbuffer
//-----------------------------------------------------------------------------
Result D2dDirectContent::SetContent(byte* imgdata)
{
	if(!d2dbmp)
		return R_OK;

	return Error(d2dbmp->CopyFromMemory(NULL, imgdata, srcsize.width * 4));
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Render direct content
//-----------------------------------------------------------------------------
void D2dDirectContent::Render(RenderTarget *rendertarget, HDC hdc, const Rect<int> &rect)
{
	if(visible && d2dbmp && rect.Intersects(destrect))
		rendertarget->DrawBitmap(d2dbmp);
}

//-----------------------------------------------------------------------------
// Name: OnCreateDevice()
// Desc: Recreate device dependend ressources
//-----------------------------------------------------------------------------
Result D2dDirectContent::OnCreateDevice(RenderTarget *rendertarget)
{
	if(!srcsize.width)
		return R_OK;

	D2D1_BITMAP_PROPERTIES props;
	rendertarget->GetDpi(&props.dpiX, &props.dpiY);
	props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
	props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	return Error(rendertarget->CreateBitmap(D2D1::SizeU(srcsize.width, srcsize.height), props, &d2dbmp));
}

//-----------------------------------------------------------------------------
// Name: OnDestroyDevice()
// Desc: Destroy device dependend ressources
//-----------------------------------------------------------------------------
void D2dDirectContent::OnDestroyDevice()
{
	RELEASE(d2dbmp);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void D2dDirectContent::Release()
{
	Unregister(); //reinterpret_cast<GuiFactory*>(&*gui)->gdcs.remove(this);

	OnDestroyDevice();

	delete this;
}


//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize GuiDirectContent
//-----------------------------------------------------------------------------
Result GdiDirectContent::PrepareContent(Size<UINT>& srcsize)
{
	HWND targetwnd = reinterpret_cast<GuiFactory*>(&*gui)->wnds.front()->targetwnd;

	this->srcsize = srcsize;

	// Remove old backbuffer
	if(hmemBMP)
		DeleteObject(hmemBMP);
	if(hmemDC)
		DeleteDC(hmemDC);

	// Set backbuffer bitmap properties
	ZeroMemory(&bmpinfo, sizeof(BITMAPINFO));
	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = srcsize.width;
	bmpinfo.bmiHeader.biHeight = srcsize.height;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 24;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biSizeImage = srcsize.width * srcsize.height * 3;

	// Get display DC
	HDC hdc = GetDC(targetwnd);

	// Create backbuffer bitmap
	hmemBMP = CreateCompatibleBitmap(hdc, srcsize.width, srcsize.height);
	if(!hmemBMP)
		return ERR("Error creating backbuffer bitmap");

	// Create off-screen DC compatible to displays DC
	hmemDC = CreateCompatibleDC(hdc);
	if(!hmemDC)
		return ERR("Error creating off-screen device context");

	// Release display DC
	ReleaseDC(targetwnd, hdc);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SetContent()
// Desc: Store direct content to backbuffer
//-----------------------------------------------------------------------------
Result GdiDirectContent::SetContent(byte* imgdata)
{
	// Select backbuffer into off-screen DC
	HGDIOBJ hobjOld = SelectObject(hmemDC, hmemBMP);

	// Draw onto backbuffer
	SetDIBits(hmemDC, hmemBMP, 0, srcsize.height, imgdata, &bmpinfo, DIB_RGB_COLORS);

	//// Restore previous GDI object
	//SelectObject(hmemDC, hobjOld);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Render direct content to hdc
//-----------------------------------------------------------------------------
void GdiDirectContent::Render(RenderTarget *rendertarget, HDC hdc, const Rect<int> &rect)
{
	if(visible && hmemDC && rect.Intersects(destrect))
		// Blit backbuffer to screen
		StretchBlt(hdc, destrect.x, destrect.y + destrect.height - 1, destrect.width, -destrect.height,
				   hmemDC, 0, 0, srcsize.width, srcsize.height, SRCCOPY);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void GdiDirectContent::Release()
{
	Unregister(); //reinterpret_cast<GuiFactory*>(&*gui)->gdcs.remove(this);

	if(hmemBMP)
		DeleteObject(hmemBMP);
	if(hmemDC)
		DeleteDC(hmemDC);

	delete this;
}