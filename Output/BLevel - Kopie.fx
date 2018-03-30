struct VsInput
{
	uint vertexId : SV_VertexID;
};

struct GsInput
{
	float4 pos : POSITION;
};

struct PsInput
{
	float4 pos : SV_Position;
};

cbuffer cbFrameDef
{
	float4 pos[256] : BoxPositions;
	matrix worldviewproj : WorldViewProj;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void vs(in VsInput IN, out GsInput OUT)
{
	OUT.pos = pos[0/*IN.vertexId*/];

	/*OUT.clr = float4(1.0f, 1.0f, 1.0f, 1.0f);

	OUT.vertexId = IN.vertexId;*/
}

//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------
[maxvertexcount(14)]
void gs(point GsInput IN[1], inout TriangleStream<PsInput> TriStream)
{
    PsInput OUT;

	OUT.pos = mul(IN[0].pos + float4(-0.5f, -0.5f, -0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(-0.5f, 0.5f, -0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(0.5f, -0.5f, -0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(0.5f, 0.5f, -0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(0.5f, 0.5f, 0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(-0.5f, 0.5f, -0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(-0.5f, 0.5f, 0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(-0.5f, -0.5f, -0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(-0.5f, -0.5f, 0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(0.5f, -0.5f, -0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(0.5f, -0.5f, 0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(0.5f, 0.5f, 0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(-0.5f, -0.5f, 0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);
	OUT.pos = mul(IN[0].pos + float4(-0.5f, 0.5f, 0.5f, 0.0f), worldviewproj); TriStream.Append(OUT);

	TriStream.RestartStrip();
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 ps(in PsInput IN) : SV_Target
{
	return float4(0.8f, 0.5f, 0.7f, 1.0f);
}

technique10 Default
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, vs()));
		SetGeometryShader(CompileShader(gs_4_0, gs()));
		SetPixelShader(CompileShader(ps_4_0, ps()));
	}
}