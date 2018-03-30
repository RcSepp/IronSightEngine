#include "RayTracer.h"


float _fmod(float val, float div)
{
	if(val < 0.0f)
		return div + val + floor(-val / div) * div;
	else
		return val - floor(val / div) * div;
}
float round(float f)
{
	return floor(f + 0.5f);
	//return f - floor(f) < 0.5f ? floor(f) : ceil(f);
}
float* TextureSampler::GetPixel(float* fResult, const Image* texture, const Point<int>* pos)
{
	unsigned char* colorptr = texture->data + ((pos->y * texture->width + pos->x) * texture->bpp);
	fResult[0] = colorptr[0] / 255.0f;
	fResult[1] = colorptr[1] / 255.0f;
	fResult[2] = colorptr[2] / 255.0f;
	if(texture->bpp > 3)
		fResult[3] = colorptr[3] / 255.0f;
	return fResult;
}
Color* TextureSampler::Sample(Color* cResult, const Image* texture, const Vector2* uv)
{
	ClrSet(cResult, Sample(rgb, texture, uv));
	return cResult;
}

float* PointTextureSampler::Sample(float* fResult, const Image* texture, const Vector2* uv)
{
	float u = _fmod(uv->x, 1.0f);
	float v = _fmod(uv->y, 1.0f);
	pt.x = (int)round((float)(texture->width - 1) * u);
	pt.y = (int)round((float)(texture->height - 1) * v);
	GetPixel(fResult, texture, &pt);
	return fResult;
}

/*Color* LinearTextureSampler::Sample(Color* cResult, const Image* texture, const Vector2* uv)
{
	float u = _fmod(uv->x, 1.0f);
	float v = _fmod(uv->y, 1.0f);
	Vec2Set(&fpos, (float)(texture->width - 1) * u, (float)(texture->height - 1) * v);
	Vec2Set(&f, fpos.x - floor(fpos.x), fpos.y - floor(fpos.y));
	pt[0].x = pt[2].x = (int)fpos.x;
	pt[1].x = pt[3].x = (int)ceil(fpos.x);
	pt[0].y = pt[1].y = (int)fpos.y;
	pt[2].y = pt[3].y = (int)ceil(fpos.y);
	c[0].SetFromTexture(texture, &pt[0]);
	c[1].SetFromTexture(texture, &pt[1]);
	c[2].SetFromTexture(texture, &pt[2]);
	c[3].SetFromTexture(texture, &pt[3]);
	Color::Lerp(&lerpclr1, &c[0], &c[1], f.x);
	Color::Lerp(&lerpclr2, &c[2], &c[3], f.x);
	Color::Lerp(cResult, &lerpclr1, &lerpclr2, f.y);
	return cResult;
}*/
float* LinearTextureSampler::Sample(float* fResult, const Image* texture, const Vector2* uv)
{
	float u = _fmod(uv->x, 1.0f);
	float v = _fmod(uv->y, 1.0f);
	Vec2Set(&fpos, (float)(texture->width - 1) * u, (float)(texture->height - 1) * v);
	Vec2Set(&f, fpos.x - floor(fpos.x), fpos.y - floor(fpos.y));
	pt[0].x = pt[2].x = (int)fpos.x;
	pt[1].x = pt[3].x = (int)ceil(fpos.x);
	pt[0].y = pt[1].y = (int)fpos.y;
	pt[2].y = pt[3].y = (int)ceil(fpos.y);
	GetPixel(rgba[0], texture, &pt[0]);
	GetPixel(rgba[1], texture, &pt[1]);
	GetPixel(rgba[2], texture, &pt[2]);
	GetPixel(rgba[3], texture, &pt[3]);

	// 2D lerp between rgba[0], rgba[1], rgba[2] and rgba[3]
	float fx = f.x, gx = 1.0f - fx, fy = f.y, gy = 1.0f - fy;
	fResult[0] = (rgba[0][0] * gx + rgba[1][0] * fx) * gy + (rgba[2][0] * gx + rgba[3][0] * fx) * fy;
	fResult[1] = (rgba[0][1] * gx + rgba[1][1] * fx) * gy + (rgba[2][1] * gx + rgba[3][1] * fx) * fy;
	fResult[2] = (rgba[0][2] * gx + rgba[1][2] * fx) * gy + (rgba[2][2] * gx + rgba[3][2] * fx) * fy;
	fResult[3] = (rgba[0][3] * gx + rgba[1][3] * fx) * gy + (rgba[2][3] * gx + rgba[3][3] * fx) * fy;

	return fResult;
}