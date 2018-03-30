struct VsInput
{
	float3 pos : POSITION0;
	float3 nml : NORMAL;
	float2 uv : TEXCOORD0;
	uint blendidx : BLENDINDEX0;
};

struct PsInput
{
	float4 pos : SV_POSITION;
	float3 nml : NORMAL;
	float2 uv : TEXCOORD0;
};

/*Texture2D<float3> tex : Texture;

SamplerState samplestage
{
	Filter = MIN_MAG_POINT_MIP_LINEAR;
    AddressU = Mirror;
    AddressV = Mirror;
};*/

cbuffer cbFrameDef
{
	matrix viewproj : ViewProj, worldviewproj : WorldViewProj, worldviewprojarray[16] : WorldViewProjArray, worldinvtransarray[16] : WorldInvTransArray, viewposinverse : ViewPosInverse;
	float4 ambient : Ambient, diffuse : Diffuse, specular : Specular;
	float power : Power, time : Time;
	/*bool hastex : HasTexture;*/
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void vs(in VsInput IN, out PsInput OUT)
{
	OUT.pos = mul(float4(IN.pos, 1.0f), worldviewprojarray[IN.blendidx]);
	OUT.nml = mul(IN.nml, worldinvtransarray[IN.blendidx]).xyz;
	OUT.uv = IN.uv;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
// Simple diffuse shader (without transformations)
float4 ps(in PsInput IN) : SV_Target
{
	float3 N = normalize(IN.nml);
	float3 L = normalize(float3(-0.5f, -0.5f, -1.0f));

	float3 clr = diffuse.rgb * dot(N, L);
	/*if(hastex)
		clr *= tex.Sample(samplestage, IN.uv).rgb;*/
	return float4(clr, 1.0f);
}


technique10 Render
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ps()));
	}
}