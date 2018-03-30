#include "DirectRenderFilm.h"

#define PBRT_PROBES_NONE
#include <core/probes.h>
#include <core/memory.h>
#include <core/film.h>
#include <core/filter.h>
#include <core/spectrum.h>
#include <core/sampler.h>

#include <core/imageio.h>

struct Pixel
{
	Pixel()
	{
		for (int i = 0; i < 3; ++i) Lrgb[i] = splatRGB[i] = 0.f;
		weightSum = 0.f;
	}
	float Lrgb[3];
	float weightSum;
	float splatRGB[3];
	float pad;
};

class DRFilm : public Film
{
private:
	DirectRenderFilm* parent;

	Filter *filter;
	float cropWindow[4];
	string filename;
	int xPixelStart, yPixelStart, xPixelCount, yPixelCount;
	float *filterTable;

public:
	BlockedArray<Pixel> *pixels;
	struct UpdateRegion
	{
		int xmin, xmax, ymin, ymax;
		UpdateRegion() {Reset();}
		void Reset()
		{
			xmin = ymin = 0x7FFFFFFF;
			xmax = ymax = -0x7FFFFFFF;
		}
	} updateregion;

	DRFilm(DirectRenderFilm* parent, const ImageFilmDesc* desc, Filter* filter) : Film(desc->xresolution, desc->yresolution)
	{
		this->parent = parent;
		this->filter = filter;
		cropWindow[0] = Clamp(min(desc->cropwindow[0], desc->cropwindow[1]), 0., 1.);
		cropWindow[1] = Clamp(max(desc->cropwindow[0], desc->cropwindow[1]), 0., 1.);
		cropWindow[2] = Clamp(min(desc->cropwindow[2], desc->cropwindow[3]), 0., 1.);
		cropWindow[3] = Clamp(max(desc->cropwindow[2], desc->cropwindow[3]), 0., 1.);
		filename = desc->filename;

		// Compute film image extent
		xPixelStart = Ceil2Int(xResolution * cropWindow[0]);
		xPixelCount = max(1, Ceil2Int(xResolution * cropWindow[1]) - xPixelStart);
		yPixelStart = Ceil2Int(yResolution * cropWindow[2]);
		yPixelCount = max(1, Ceil2Int(yResolution * cropWindow[3]) - yPixelStart);

		// Allocate film image storage
		pixels = new BlockedArray<Pixel>(xPixelCount, yPixelCount);

		// Precompute filter weight table
#define FILTER_TABLE_SIZE 16
		filterTable = new float[FILTER_TABLE_SIZE * FILTER_TABLE_SIZE];
		float *ftp = filterTable;
		for (int y = 0; y < FILTER_TABLE_SIZE; ++y) {
			float fy = ((float) y + .5f) * filter->yWidth / FILTER_TABLE_SIZE;
			for (int x = 0; x < FILTER_TABLE_SIZE; ++x) {
				float fx = ((float) x + .5f) * filter->xWidth / FILTER_TABLE_SIZE;
				*ftp++ = filter->Evaluate(fx, fy);
			}
		}

		DirectRenderFilm::Options_OutputSizeChanged(desc->xresolution, desc->yresolution, parent);
		parent->film = this;
	}
	~DRFilm()
	{
		delete pixels;
		delete filter;
		delete[] filterTable;
	}
	void AddSample(const CameraSample &sample, const Spectrum &L)
	{
		// Compute sample's raster extent
		float dimageX = sample.imageX - 0.5f;
		float dimageY = sample.imageY - 0.5f;
		int x0 = Ceil2Int(dimageX - filter->xWidth);
		int x1 = Floor2Int(dimageX + filter->xWidth);
		int y0 = Ceil2Int(dimageY - filter->yWidth);
		int y1 = Floor2Int(dimageY + filter->yWidth);
		x0 = max(x0, xPixelStart);
		x1 = min(x1, xPixelStart + xPixelCount - 1);
		y0 = max(y0, yPixelStart);
		y1 = min(y1, yPixelStart + yPixelCount - 1);
		if ((x1 - x0) < 0 || (y1 - y0) < 0) {
			PBRT_SAMPLE_OUTSIDE_IMAGE_EXTENT(const_cast<CameraSample *>(&sample));
			return;
		}

		// Loop over filter support and add sample to pixel arrays
		float rgb[3];
		L.ToRGB(rgb);

		// Precompute $x$ and $y$ filter table offsets
		int *ifx = ALLOCA(int, x1 - x0 + 1);
		for (int x = x0; x <= x1; ++x) {
			float fx = fabsf((x - dimageX) * filter->invXWidth * FILTER_TABLE_SIZE);
			ifx[x - x0] = min(Floor2Int(fx), FILTER_TABLE_SIZE - 1);
		}
		int *ify = ALLOCA(int, y1 - y0 + 1);
		for (int y = y0; y <= y1; ++y) {
			float fy = fabsf((y - dimageY) * filter->invYWidth * FILTER_TABLE_SIZE);
			ify[y - y0] = min(Floor2Int(fy), FILTER_TABLE_SIZE - 1);
		}
		bool syncNeeded = (filter->xWidth > 0.5f || filter->yWidth > 0.5f);
		for (int y = y0; y <= y1; ++y) {
			for (int x = x0; x <= x1; ++x) {
				// Evaluate filter value at $(x,y)$ pixel
				int offset = ify[y - y0] * FILTER_TABLE_SIZE + ifx[x - x0];
				float filterWt = filterTable[offset];

				// Update pixel values with filtered sample contribution
				Pixel &pixel = (*pixels)(x - xPixelStart, y - yPixelStart);
				if (!syncNeeded) {
					pixel.Lrgb[0] += filterWt * rgb[0];
					pixel.Lrgb[1] += filterWt * rgb[1];
					pixel.Lrgb[2] += filterWt * rgb[2];
					pixel.weightSum += filterWt;
				} else {
					// Safely update _Lxyz_ and _weightSum_ even with concurrency
					AtomicAdd(&pixel.Lrgb[0], filterWt * rgb[0]);
					AtomicAdd(&pixel.Lrgb[1], filterWt * rgb[1]);
					AtomicAdd(&pixel.Lrgb[2], filterWt * rgb[2]);
					AtomicAdd(&pixel.weightSum, filterWt);
				}
			}
		}

		auto min = [](int a, int b) {return a < b ? a : b;};
		auto max = [](int a, int b) {return a > b ? a : b;};

		// Trigger OpenGL texture update
		updateregion.xmin = min(updateregion.xmin, x0);
		updateregion.xmax = max(updateregion.xmax, x1);
		updateregion.ymin = min(updateregion.ymin, y0);
		updateregion.ymax = max(updateregion.ymax, y1);
	}
    void Splat(const CameraSample &sample, const Spectrum &L)
	{
		if (L.HasNaNs()) {
			Warning("ImageFilm ignoring splatted spectrum with NaN values");
			return;
		}
		float xyz[3];
		L.ToXYZ(xyz);
		int x = Floor2Int(sample.imageX), y = Floor2Int(sample.imageY);
		if (x < xPixelStart || x - xPixelStart >= xPixelCount || y < yPixelStart
				|| y - yPixelStart >= yPixelCount)
			return;
		Pixel &pixel = (*pixels)(x - xPixelStart, y - yPixelStart);
		AtomicAdd(&pixel.splatRGB[0], xyz[0]);
		AtomicAdd(&pixel.splatRGB[1], xyz[1]);
		AtomicAdd(&pixel.splatRGB[2], xyz[2]);
	}
    void GetSampleExtent(int *xstart, int *xend, int *ystart, int *yend) const
	{
		*xstart = Floor2Int(xPixelStart + 0.5f - filter->xWidth);
		*xend = Floor2Int(xPixelStart + 0.5f + xPixelCount + filter->xWidth);

		*ystart = Floor2Int(yPixelStart + 0.5f - filter->yWidth);
		*yend = Floor2Int(yPixelStart + 0.5f + yPixelCount + filter->yWidth);
	}
    void GetPixelExtent(int *xstart, int *xend, int *ystart, int *yend) const
	{
		*xstart = xPixelStart;
		*xend = xPixelStart + xPixelCount;
		*ystart = yPixelStart;
		*yend = yPixelStart + yPixelCount;
	}
    void WriteImage(float splatScale)
	{
		// Convert image to RGB and compute final pixel values
		int nPix = xPixelCount * yPixelCount;
		float *rgb = new float[3 * nPix];
		int offset = 0;
		for (int y = 0; y < yPixelCount; ++y) {
			for (int x = 0; x < xPixelCount; ++x) {
				// Copy pixel to rgb
				memcpy(&rgb[3 * offset], (*pixels)(x, y).Lrgb, 3 * sizeof(float));

				// Normalize pixel with weight sum
				float weightSum = (*pixels)(x, y).weightSum;
				if (weightSum != 0.f) {
					float invWt = 1.f / weightSum;
					rgb[3 * offset] = max(0.f, rgb[3 * offset] * invWt);
					rgb[3 * offset + 1] = max(0.f, rgb[3 * offset + 1] * invWt);
					rgb[3 * offset + 2] = max(0.f, rgb[3 * offset + 2] * invWt);
				}

				// Add splat value at pixel
				float splatRGB[3];
				memcpy(splatRGB, (*pixels)(x, y).splatRGB, 3 * sizeof(float));
				rgb[3 * offset] += splatScale * splatRGB[0];
				rgb[3 * offset + 1] += splatScale * splatRGB[1];
				rgb[3 * offset + 2] += splatScale * splatRGB[2];
				++offset;
			}
		}

		// Write RGB image
		::WriteImage(filename, rgb, NULL, xPixelCount, yPixelCount, xResolution,
				yResolution, xPixelStart, yPixelStart);

		// Release temporary image memory
		delete[] rgb;
	}
    void UpdateDisplay(int x0, int y0, int x1, int y1, float splatScale)
	{
	}
};

Result Pbrt::CreateDirectRenderView(IOutputWindow* wnd)
{
	drfilm = new DirectRenderFilm((D3D::IOutputWindow*)wnd);
	return drfilm->Init();
}
void Pbrt::RemoveDirectRenderView()
{
	RELEASE(drfilm);
}
Film* Pbrt::MakeFilm(const ImageFilmDesc* desc, Filter* filter)
{
	return drfilm ? new DRFilm(drfilm, desc, filter) : MakeFilm(desc, filter);
}
void Pbrt::UnhookFilm()
{
	if(drfilm->film)
	{
		drfilm->unhookfilm = true;
		while(drfilm->film)
			Sleep(1);
	}
}

Result DirectRenderFilm::Init()
{
	Result rlt;

	// Prepare renderable
	options.invgamma = 1.0f / 2.2f;

	d3dwnd->RegisterForUpdating(this);

	return R_OK;
}

void DirectRenderFilm::Options_OutputSizeChanged(int width, int height, LPVOID user)
{
	Result rlt;
	DirectRenderFilm* wnd = (DirectRenderFilm*)user;

	// Recreate texture and HUD
	if(wnd->d3dwnd && (!wnd->d3dtex || wnd->d3dtex->GetWidth() != width || wnd->d3dtex->GetHeight() != height))
	{
		if(wnd->d3dhud)
			wnd->d3dhud->Release();
		if(wnd->d3dtex)
			wnd->d3dtex->Release();
		IFFAILED(wnd->d3dwnd->CreateTexture(width, height, D3D::ITexture::TU_CPUWRITE, false, D3D::DXGI_FORMAT_R8G8B8A8_UNORM, &wnd->d3dtex))
		{
			Error(rlt.GetLastResult());
			return;
		}
		IFFAILED(wnd->d3dwnd->CreateHUD(&wnd->d3dhud))
		{
			Error(rlt.GetLastResult());
			return;
		}
		IFFAILED(wnd->d3dhud->CreateElement(wnd->d3dtex, 0, 0, D3D::DT_TOPLEFT, D3D::Color(0xFFFFFFFF)))
		{
			Error(rlt.GetLastResult());
			return;
		}
		wnd->d3dwnd->RegisterForRendering(wnd->d3dhud, D3D::RT_FOREGROUND); //RT_BACKGROUND
	}
}

void DirectRenderFilm::Update()
{
	if(!film)
		return;

	//bool renderwindow = false;
	int xmin = film->updateregion.xmin, xmax = film->updateregion.xmax, ymin = film->updateregion.ymin, ymax = film->updateregion.ymax;

	if(d3dtex && xmin <= xmax && ymin <= ymax)
	{
		//renderwindow = true;

		//unsigned int* currentpixel = dwpixels;
		uint32_t* dwpixels;
		UINT stride;
		d3dtex->MapPixelData(false, true, &stride, (LPVOID*)&dwpixels);
		stride /= 4;
		for (int y = ymin; y <= ymax; ++y)
			for (int x = xmin; x <= xmax; ++x) {
				Pixel &pixel = (*film->pixels)(x, y);
				float invWt = pixel.weightSum ? 1.0f / pixel.weightSum : 0.0f;
				float frgb[] = { max(0.0f,
						max(0.f, pixel.Lrgb[0]) * invWt + pixel.splatRGB[0]),
						max(0.0f,
								max(0.f, pixel.Lrgb[1]) * invWt
										+ pixel.splatRGB[1]), max(0.0f,
								max(0.f, pixel.Lrgb[2]) * invWt
										+ pixel.splatRGB[2]) };
				/*if (!shader) // Only do tonemapping and gamma when no shader is used*/
				{
					frgb[0] = powf(frgb[0], options.invgamma);
					frgb[1] = powf(frgb[1], options.invgamma);
					frgb[2] = powf(frgb[2], options.invgamma);
					float m = max(max(frgb[0], frgb[1]), frgb[2]);
					if (m > 1.0f) {
						frgb[0] /= m;
						frgb[1] /= m;
						frgb[2] /= m;
					}
				}
				const unsigned int rgb[] = { (unsigned int) (min(1.0f, frgb[0])
						* 255.0f), (unsigned int) (min(1.0f, frgb[1]) * 255.0f),
						(unsigned int) (min(1.0f, frgb[2]) * 255.0f) };
				//const unsigned int dwpixel = 0xFF000000 | rgb[2] << 16 | rgb[1] << 8 | rgb[0] << 0;
				unsigned int dwpixel = rgb[2] << 16 | rgb[1] << 8 | rgb[0] << 0;
				if (dwpixel)
					dwpixel |= 0xFF000000;
				//*(currentpixel++) = dwpixel;
				dwpixels[y * stride + x] = dwpixel;
			}
		d3dtex->UnmapData();
	}

	if(unhookfilm)
	{
		unhookfilm = false;
		film = NULL;
	}

	//return renderwindow;
}

void DirectRenderFilm::Release()
{
	if(d3d) // Do not deregister from d3dwnd if d3d has already been released
		d3dwnd->DeregisterFromUpdating(this);

	if(film)
		delete film;

	delete this;
}