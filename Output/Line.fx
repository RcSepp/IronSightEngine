struct VsInput
{
	uint vertexId : SV_VertexID;
};

struct PsInput
{
	float4 pos : SV_Position;
	float4 clr : color;
};

cbuffer cbFrameDef
{
	float4 pos[256] : Positions;
	float4 clr[256] : Colors;
	matrix worldviewproj : WorldViewProj;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void vsTransformed(in VsInput IN, out PsInput OUT)
{
	OUT.pos = mul(pos[IN.vertexId], worldviewproj);

	OUT.clr = clr[IN.vertexId];
}

void vsUntransformed(in VsInput IN, out PsInput OUT)
{
	if(!(IN.vertexId & 0x1))
		OUT.pos = float4(pos[IN.vertexId >> 1].xy, 0.5f, 1.0f);
	else
		OUT.pos = float4(pos[IN.vertexId >> 1].zw, 0.5f, 1.0f);

	OUT.clr = clr[IN.vertexId];
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 ps(in PsInput IN) : SV_Target
{
	return IN.clr;
}

technique10 Transformed
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, vsTransformed()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ps()));
	}
}

technique10 Untransformed
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, vsUntransformed()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ps()));
	}
}