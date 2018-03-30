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
	float4 diffuse : Diffuse;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void vsSolid(in VsInput IN, out PsInput OUT)
{
	OUT.pos = mul(float4(IN.pos, 1.0f), worldviewproj);
	OUT.nml = IN.nml;
	OUT.uv = IN.uv;
	OUT.clr = float4(0.2f, 0.2f, 1.0f, 0.5f);
}
void vsLines(in VsInput IN, out PsInput OUT)
{
	OUT.pos = mul(float4(IN.pos, 1.0f), worldviewproj);
	OUT.nml = IN.nml;
	OUT.uv = IN.uv;
	OUT.clr = float4(0.5f, 0.2f, 1.0f, 1.0f);
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 ps(in PsInput IN) : SV_Target
{
	float3 N = mul(IN.nml, worldinvtrans).xyz;
	float3 L = normalize(float3(0.0f, -0.5f, -1.0f));

	return float4(diffuse.rgb * dot(N, L), diffuse.a);
//return float4(IN.nml * 0.5f + 0.5f, 1.0f);
//float x = IN.nml.x * 0.5f + 0.5f;
//return float4(x, x, x, 1.0f);
}

RasterizerState RS_Default
{
    FillMode = SOLID;
};
RasterizerState RS_DrawLines
{
    FillMode = WIREFRAME;
};

technique10 Render
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, vsSolid()));
		SetPixelShader(CompileShader(ps_4_0, ps()));
		//SetRasterizerState(RS_Default);
	}
	/*pass P1
	{
		SetVertexShader(CompileShader(vs_4_0, vsLines()));
		SetPixelShader(CompileShader(ps_4_0, ps()));
		SetRasterizerState(RS_DrawLines);
	}
	pass P2
	{
		SetRasterizerState(RS_Default);
	}*/
}