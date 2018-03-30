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
	float4 wpos : COLOR1;
};

Texture2D<float4> tex : Texture;

SamplerState samplestage
{
	Filter = MIN_MAG_POINT_MIP_LINEAR;
    AddressU = Wrap;//Mirror;
    AddressV = Wrap;//Mirror;
};

struct LightParams
{
	float4 lightcolor;
	float3 lightpos;
	int lighttype;
	float3 lightdir;
	int padding0;
	float4 lightattenuation;
} lightparams[16] : LightParams;

cbuffer cbFrameDef
{
	matrix world : World, viewproj : ViewProj, worldviewproj : WorldViewProj, worldinvtrans : WorldInvTrans;
	float4 viewpos : ViewPos;
	float4 ambient : Ambient, diffuse : Diffuse, specular : Specular;
	int numlights : NumLights;
	float power : Power, time : Time;
	bool hastex : HasTexture;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void vs(in VsInput IN, out PsInput OUT)
{
	OUT.pos = mul(float4(IN.pos, 1.0f), worldviewproj);
	OUT.wpos = mul(float4(IN.pos, 1.0f), world);
	OUT.nml = IN.nml;
	OUT.uv = IN.uv;
	OUT.clr = diffuse;
}

#define LT_DIRECTIONAL	0
#define LT_POINT		1
#define LT_SPOT			2
void Shade(in float3 position, in float3 normal, out float3 diffambclr, out float3 specclr)
{
	float3 worldVertPos = position;

	float3 AllLightDiff = 0;
	float3 AllLightSpec = 0;
	float3 L;

	float3 N = normal; //normal vector
	float3 E = normalize(worldVertPos - viewpos.xyz); //eye vector
	float NdotL;

	for(int i = 0; i < numlights; i++)
	{
		switch(lightparams[i].lighttype)
		{
		case LT_DIRECTIONAL:
			L = lightparams[i].lightdir.xyz;
			NdotL = dot(N, L);
			if(NdotL < 0.0f)
			{
				AllLightDiff += lightparams[i].lightcolor.rgb * max(0.0f, -NdotL);
				AllLightSpec += lightparams[i].lightcolor.rgb * pow(max(0.0f, -dot(E, reflect(L, N))), power);
			}
			break;

		case LT_POINT:
			L = normalize(worldVertPos.xyz - lightparams[i].lightpos.xyz);
			NdotL = dot(N, L);
			if(NdotL < 0.0f)
			{
				AllLightDiff += lightparams[i].lightcolor.rgb * max(0.0f, -NdotL);
				AllLightSpec += lightparams[i].lightcolor.rgb * pow(max(0.0f, -dot(E, reflect(L, N))), power);
			}
			break;

		case LT_SPOT:
			L = worldVertPos.xyz - lightparams[i].lightpos.xyz;
			float lightdist = length(L);
			L /= lightdist;
			NdotL = dot(N, L);
			if(NdotL < 0.0f)
			{
				float spot = pow(max(0.0f, dot(L, lightparams[i].lightdir)), lightparams[i].lightattenuation.w);
				float att = spot / dot(lightparams[i].lightattenuation.xyz, float3(1.0f, lightdist, lightdist*lightdist));
				AllLightDiff += lightparams[i].lightcolor.rgb * max(0.0f, -NdotL) * att;
				AllLightSpec += lightparams[i].lightcolor.rgb * pow(max(0.0f, -dot(E, reflect(L, N))), power) * att;
			}
			break;
		}
	}

	diffambclr = diffuse.rgb * AllLightDiff + ambient.rgb;
	specclr = specular.rgb * AllLightSpec;

/*float3 _L = -normalize(float3(0.0f, -0.5f, -1.0f));
float f = saturate(-dot(E, reflect(L, N)));
diffambclr = float3(f, f, f);//E * 0.5 + 0.5;//float3(1.0f, 0.0f, 0.0f);
specclr = float3(0.0f, 0.0f, 0.0f);*/
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 ps(in PsInput IN) : SV_Target
{
	float3 N = normalize(mul(float4(IN.nml, 1.0f), worldinvtrans).xyz); //EDIT: Do that in vertex shader

	float3 diffambclr, specclr;
	Shade(IN.wpos.xyz, N, diffambclr, specclr);
	if(hastex)
		return float4(diffambclr, diffuse.a) * tex.Sample(samplestage, IN.uv) + float4(specclr, diffuse.a);
	else
		return float4(diffambclr + specclr, diffuse.a);

	/*float3 N = mul(float4(IN.nml, 1.0f), worldinvtrans).xyz; //EDIT: Do that in vertex shader
	float3 L = normalize(float3(0.0f, -0.5f, -1.0f));

	float4 clr = float4(ambient.rgb + diffuse.rgb * saturate(-dot(N, L)), diffuse.a);
	if(hastex)
		clr *= tex.Sample(samplestage, IN.uv);
	return clr;*/
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