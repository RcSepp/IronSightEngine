struct VsInput
{
	float3 pos : POSITION0;
	float3 nml : NORMAL;
	float2 uv : TEXCOORD0;
};

/*struct GsInput
{
	float3 pos : POSITION;
	float3 nml : NORMAL;
	float2 tex : TEXCOORD0;
};*/

struct PsInput
{
	float4 pos : SV_POSITION;
	float3 nml : NORMAL;
	float2 uv : TEXCOORD0;
	float4 clr : COLOR0;
};

/*Texture2D<float3> tex : Texture;

SamplerState samplestage
{
	Filter = MIN_MAG_POINT_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};*/

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
	OUT.clr = diffuse;//mul(mul(float4(IN.pos, 1.0f), world), view).z;
}

//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------
/*[maxvertexcount(3)]
void gs(triangle GsInput IN[3], inout TriangleStream<PsInput> TriStream)
{
    PsInput OUT;

	float3 velo = float3(IN[0].pos + IN[1].pos + IN[2].pos);
	float velo_ = length(velo);
	velo = velo / velo_ / velo_;

	float t = time % 2.0f;
	if(t < 1.0f)
		velo *= 20000.0f * sqrt(t);
	else
		velo = float3(0.0f, 0.0f, 0.0f);

	OUT.pos = mul(mul(mul(float4(IN[0].pos + velo, 1.0f), world), view), proj);
	OUT.nml = IN[0].nml;
	OUT.tex = IN[0].tex;
	TriStream.Append(OUT);
	OUT.pos = mul(mul(mul(float4(IN[1].pos + velo, 1.0f), world), view), proj);
	OUT.nml = IN[1].nml;
	OUT.tex = IN[1].tex;
	TriStream.Append(OUT);
	OUT.pos = mul(mul(mul(float4(IN[2].pos + velo, 1.0f), world), view), proj);
	OUT.nml = IN[2].nml;
	OUT.tex = IN[2].tex;
	TriStream.Append(OUT);
	TriStream.RestartStrip();
}*/


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
/*// Old diffuse shader
float4 ps(in PsInput IN) : SV_Target
{
	float3 lightdir =  float3(1.0f, 1.0f, 1.0f);


	//float diffuse = saturate(dot(mul(IN.nml, (float3x3)world), lightdir));

	if(IN.nml.x < 0.01f && IN.nml.y > 0.99f && IN.nml.z < 0.01f)
		return float4(80.0f / 255.0f, 110.0f / 255.0f, 60.0f / 255.0f, 1.0f);
	else
		return float4(IN.nml, 1.0f); //float4(float3(0.2f, 0.2f, 0.2f) + float3(0.5f, 0.5f, 0.5f) * diffuse, 1.0f);
}*/

// Simple diffuse shader (without transformations)
float4 ps(in PsInput IN) : SV_Target
{
	float3 N = mul(IN.nml, worldinvtrans).xyz;
	float3 L = normalize(float3(0.0f, -0.5f, -1.0f));

	float3 clr = diffuse.rgb * dot(N, L) * 0.5 + 0.5;
	return float4(clr, 1.0f);
}
float4 psLine(in PsInput IN) : SV_Target
{
	float3 N = mul(IN.nml, worldinvtrans).xyz;
	float3 L = normalize(float3(0.0f, -0.5f, -1.0f));

	float3 clr = float3(0.5f, 0.7f, 1.0f) * dot(N, L);
	return float4(clr, 1.0f);
}

RasterizerState RenderSolid
{
    FillMode = SOLID;
};
RasterizerState RenderWireframe
{
    FillMode = WIREFRAME;
};


technique10 Render
{
	pass P0
	{
		SetRasterizerState(RenderSolid);
		SetVertexShader(CompileShader(vs_4_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ps()));
	}
	pass P1
	{
		SetRasterizerState(RenderWireframe);
		SetVertexShader(CompileShader(vs_4_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, psLine()));
	}
}