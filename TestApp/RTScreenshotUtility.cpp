#include <ISRayTracer.h>
#include <ISForms.h>
#include <ISImage.h>
#include "RTScreenshotUtility.h"

namespace RTSU
{
	LPWINFORM rtrform;
	ISRayTracer::IScene* scene;
	Image* img;
	static int imgidx = 0;

	void __stdcall OnRayTracerFormClosed(IWinForm* sender, LPVOID user)
	{
		rtrform = NULL;
	}

	void OnSetPixel(const Point<>& pos, unsigned long color)
	{
		if(rtrform)
		{
			img->data[(pos.y * img->width + pos.x) * img->bpp + 0] = (color >> 0) & 0xFF;
			img->data[(pos.y * img->width + pos.x) * img->bpp + 1] = (color >> 8) & 0xFF;
			img->data[(pos.y * img->width + pos.x) * img->bpp + 2] = (color >> 16) & 0xFF;
			if(img->bpp == 4)
				img->data[(pos.y * img->width + pos.x) * img->bpp + 3] = (color >> 24) & 0xFF;

			/*HDC hdc = GetDC(rtrform->GetHwnd());
			SetPixel(hdc, pos.x, pos.y, color);
			ReleaseDC(rtrform->GetHwnd(), hdc);*/
		}
	}

	unsigned long WINAPI RayTracerThread(LPVOID args)
	{
		ISRayTracer::IScene* scene = (ISRayTracer::IScene*)args;

		img = Image::New(scene->GetCamera()->size.width, scene->GetCamera()->size.height, 3);

		// Perform ray-tracing
		RayTracingOptions options;
	options.technique = RayTracingOptions::RTT_WHITTED_RT;//RTT_SHOW_NORMALS;//cmdline.technique;
	//options.quietmodeenabled = cmdline.quietmodeenabled;
	//options.usethinrefractions = cmdline.usethinrefractions;
	//options.batchviewrays = cmdline.batchviewrays;
	options.supersamplingfactor = 1;//cmdline.supersamplingfactor;
	//options.texsampling = cmdline.texsampling;
	//options.startframe = cmdline.startframe;
	options.onsetpixel = OnSetPixel;
	options.onframedone = NULL;
		rtr->Render(scene, options);

		SaveImage((FilePath)(String("out") << String(++imgidx) << String(".png")), img);

		// Clean up
		scene->Release();
		//EDIT: Release camera, surfaces, ...
		delete img;

		return 0;
	}
}
using namespace RTSU;

void __stdcall RTScreenshotUtility::OnKeyTakeScreenshot(int key, LPVOID user)
{
	if(!rtr)
	{
		rtr = CreateRayTracer();
		rtr->Sync(GLOBALVAR_LIST);
	}

	if(!rtrform)
	{
		// Create WinForm
		FormSettings frmsettings;
		frmsettings.fbstyle = FBS_DEFAULT;
		frmsettings.caption = "RayTracer";
		frmsettings.clientsize = Size<int>(512, 512);
		//frmsettings.clientsize = Size<int>(256, 256);
		frmsettings.windowpos = Point<int>(512, 0);
		//frmsettings.windowpos = Point<int>(1124, 0);
		fms->CreateForm(&frmsettings, NULL, &rtrform);
		rtrform->SetFormClosedCallback(OnRayTracerFormClosed, NULL);
		rtrform->Show();
	}

	rtr->CreateScene((IOutputWindow*)user, &scene);
	scene->GetCamera()->size = rtrform->ClientSize;
	scene->GetCamera()->region = Rect<>(0, 0, rtrform->ClientWidth, rtrform->ClientHeight);

	RayTracerThread(scene);//CreateThread(NULL, 0, RayTracerThread, scene, 0, NULL);
}

RTScreenshotUtility::~RTScreenshotUtility()
{
	//RELEASE(rtr);
}