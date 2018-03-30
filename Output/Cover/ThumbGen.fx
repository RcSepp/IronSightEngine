#define PI				3.14159265f

struct VsInput
{
	float3 pos : POSITION0;
	float2 uv : TEXTURE0;
};

struct PsInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

Texture2D<float3> texsrc : SourceImage, texovl : OverlayImage;
float4 texsrcsize : SourceImageSize, texovlsize : OverlayImageSize, texdestsize : DestinationImageSize;

SamplerState samplestage
{
	Filter = MIN_MAG_POINT_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

cbuffer cbFrameDef
{
	matrix world : World, viewproj : ViewProj, worldviewproj : WorldViewProj, worldinverse : WorldInverse, viewposinverse : ViewPosInverse;
	float time : Time;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void vs(in VsInput IN, out PsInput OUT)
{
	OUT.pos.xy = IN.pos.xy;
	OUT.pos.z = 0.5f;
	OUT.pos.w = 1;
	OUT.uv = IN.uv;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float GetAngle(float2 p)
{
	if(p.x)
	{
		float ang = -atan(p.y / p.x);
		if(p.x < 0.0f)
			return ang + PI;
		else
			return ang;
	}
	else
	{
		if(p.y)
			return -PI / 2.0F * sign(p.y);
		else
			return 0.0f;
	}
}
float3 DownSample(Texture2D<float3> tex, float2 uv, float2 srcsize, float2 destsize)
{
	float _w = srcsize.x / (destsize.x * 2.0f);
	float _h = srcsize.y / (destsize.y * 2.0f);

	float3 samples;
	int c = 0;
	for(float x = -_w; x < _w; x++)
		for(float y = -_h; y < _h; y++)
		{
			samples += tex.Sample(samplestage, float2(uv.x + x / srcsize.x, uv.y + y / srcsize.y));
			c++;
		}
	return samples / c;
}
float4 ps(in PsInput IN) : SV_Target
{
	float x = 2.0f * IN.uv.x - 1.0f;
	float y = 2.0f * IN.uv.y - 1.0f;
	float d = sqrt(x*x + y*y);

	const float alphafade = 2.0f; // Number of pixels used as alpha gradient in the destination image
	float alpha = 1.0f - clamp((d - 1.0f) * max(texdestsize.x, texdestsize.y) / (2.0f * alphafade) + 1.0f, 0.0f, 1.0f);

	float a = GetAngle(float2(x, y)) + 0.3f * pow(d, 5.0f);
	float brightness = 1.0f - 0.8f * pow(d, 3.0f);
	d /= 2.0f;

	float3 clr = DownSample(texsrc, float2(0.5f + d * cos(a), 0.5f - d * sin(a)), texsrcsize.xy, texdestsize.xy);
	float3 clr2 = DownSample(texovl, IN.uv, texovlsize.xy, texdestsize.xy);
	return float4(brightness * clr + clr2, alpha);
}


BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

RasterizerState DisableCulling
{
	CullMode = NONE;
};

technique10 Default
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ps()));

		SetBlendState(NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
        SetDepthStencilState(DisableDepth, 0);
        SetRasterizerState(DisableCulling);
	}
}
