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
	matrix world : World, viewproj : ViewProj, worldviewproj : WorldViewProj, worldinvtrans : WorldInvTrans, viewposinverse : ViewPosInverse;
	float4 ambient : Ambient, diffuse : Diffuse, specular : Specular;
	float power : Power, time : Time;
	bool hastex : HasTexture;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void vs(in VsInput IN, out PsInput OUT)
{
	OUT.pos = mul(float4(IN.pos, 1.0f), worldviewproj);
	OUT.nml = IN.nml;
	OUT.uv = IN.uv;
	OUT.clr = float4(1.0f, 1.0f, 1.0f, 1.0f);//mul(mul(float4(IN.pos, 1.0f), world), view).z;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 ps(in PsInput IN) : SV_Target
{
	float3 N = mul(IN.nml, worldinvtrans).xyz;
	float3 L = normalize(float3(0.0f, -0.5f, -1.0f));

	float3 clr = diffuse.rgb * dot(N, L);
	return float4(diffuse.rgb * dot(N, L), 1.0f);
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