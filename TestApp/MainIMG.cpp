#include "main.h"
#include <ISImage.h>

LPWINFORM form;

Result __stdcall Init()
{
	Result rlt;

	// >>> Init Forms

	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create forms
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(256, 256);
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(100, 100);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form));

	return R_OK;
}

Result __stdcall PostInit()
{
	Result rlt;

	Image* img;
	//CHKRESULT(LoadImage("K:\\C++\\IronSightEngine\\Output\\pbrt.tga", &img)); // tga
	//CHKRESULT(LoadImage("K:\\C++\\IronSightEngine\\Output\\heightmap.png", &img)); // bpp == 1
	//CHKRESULT(LoadImage("K:\\C++\\IronSightEngine\\Output\\MincraftDefaultTexture.png", &img)); // bpp == 4
	//CHKRESULT(LoadImage("D:\\Backup D\\Pictures\\Clock.bmp", &img)); // bpp == 3
	CHKRESULT(LoadImage("K:\\pbrt-v2\\bin\\pbrt.exr", &img)); // exr

	form->SetClientSize(Size<>(img->width, img->height));

	HDC hdc = GetDC(form->GetHwnd());
	for(int y = 0; y < img->height; y++)
		for(int x = 0; x < img->width; x++)
		{
			unsigned char* clrptr = img->data + (y * img->width + x) * img->bpp;
			DWORD clr = 0;
			memcpy(&clr, clrptr, min(img->bpp, 4));
			clr &= 0x00FFFFFF;
			SetPixel(hdc, x, y, clr);
		}
	ReleaseDC(form->GetHwnd(), hdc);

	//CHKRESULT(SaveImage("out.png", img));

	return R_OK;
}

CYCLICFUNC cyclic = NULL;

void __stdcall End()
{
	RELEASE(cle);
	RELEASE(fms);
}