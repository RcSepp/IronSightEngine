//-----------------------------------------------------------------------------
// GLOBAL VARIABLES
//-----------------------------------------------------------------------------
texture Texture : Texture;

//-----------------------------------------------------------------------------
// STRUCT
//-----------------------------------------------------------------------------
struct PixelInput
{
	float2 uv : TEXCOORD0;
};

//-----------------------------------------------------------------------------
// SAMPLER
//-----------------------------------------------------------------------------
sampler tsampler = sampler_state 
{
	texture = <Texture>;
	AddressU  = CLAMP;
	AddressV  = CLAMP;
	AddressW  = CLAMP;
	MIPFILTER = POINT;
	MINFILTER = ANISOTROPIC;
	MAGFILTER = ANISOTROPIC;
};

const float kb = 0.114f;
const float kr = 0.299f;

inline void RGB_To_YCbCr(in float3 rgb, out float y, out float cb, out float cr)
{
	y = kr * rgb.r + (1.0f - kr - kb) * rgb.g + kb * rgb.b;
	cb = 0.5f * (rgb.b - y) / (1.0f - kb);
	cr = 0.5f * (rgb.r - y) / (1.0f - kr);
}
inline void YCbCr_To_RGB(out float3 rgb, in float y, in float cb, in float cr)
{
	rgb.r = 2.0f * (cr * (1.0f - kr)) + y;
	rgb.b = 2.0f * (cb * (1.0f - kb)) + y;
	rgb.g = (y - kr * rgb.r - kb * rgb.b) / (1.0f - kr - kb);
}

//-----------------------------------------------------------------------------
// PIXEL SHADER
//-----------------------------------------------------------------------------
float4 ps(PixelInput IN): COLOR
{
	float3 rgb;
	float y, cb, cr;

	rgb = tex2D(tsampler, IN.uv);
	RGB_To_YCbCr(rgb, y, cb, cr);

	y += 0.5f / 6.0f;

	// Shrink color palette (toon shader)
	y = trunc(y * 6.0f) / 6.0f;

	YCbCr_To_RGB(rgb, y, cb, cr);

	return float4(rgb, 1.0f);
}

//-----------------------------------------------------------------------------
// TECHNIQUE
//-----------------------------------------------------------------------------
technique Default
{
	pass p0
	{
		PixelShader  = compile ps_2_0 ps();
	}
}