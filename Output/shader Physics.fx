struct VsInput
{
	float3 pos : POSITION0;
	float3 nml : NORMAL;
	float2 uv : TEXCOORD0;
};

struct PsInput
{
	float4 pos : SV_POSITION;
	float3 nml : NORMAL;
	float2 uv : TEXCOORD0;
	float4 clr : COLOR0;
};

cbuffer cbFrameDef
{
	matrix worldviewproj : WorldViewProj, worldinvtrans : WorldInvTrans;
	float4 diffuse : Diffuse;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void vs(in VsInput IN, out PsInput OUT)
{
	OUT.pos = mul(float4(IN.pos, 1.0f), worldviewproj);
	OUT.nml = mul(float4(IN.nml, 1.0f), worldinvtrans).xyz;
	OUT.uv = IN.uv;
	OUT.clr = float4(diffuse.rgb, min(0.6f, diffuse.a));
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 ps(in PsInput IN) : SV_Target
{
	float3 N = IN.nml;
	float3 L = normalize(float3(0.0f, -0.5f, -1.0f));
	float luminance = saturate(dot(N, L));
	luminance = luminance * 0.9f + 0.2f; // Simulate ambient
	return float4((IN.clr.rgb) * luminance, IN.clr.a);
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