struct PS_IN
{
	float3 Normal				: TEXCOORD1;
	float2 UV					: TEXCOORD2;
	float4 Color0				: TEXCOORD3;
	float4 Shadow12				: TEXCOORD4;
	float4 Shadow34				: TEXCOORD5;
	float3 Shadow5				: TEXCOORD6;
	float2 TexCoord				: TEXCOORD7;
};
struct PS_OUTPUT
{
	float4 RGBColor : COLOR0;  // Pixel color    
};

sampler2D g_DiffuseTex		:	register(s0);
sampler2D g_shinyTex		:	register(s2);
sampler2D g_texToLerpTo		:	register(s3);

float g_TevMode_0 : register(c0);
float4 g_LightAllergy : register(c70);

float shinyJewelMonotone : register(c78);
float lerpValue : register(c82);

float4 getLerpedTexture(in float4 texA, in float2 uv) {
	if (lerpValue <= 0) return texA;

	float4 texB = tex2D(g_texToLerpTo, uv);
	return texA + (texB - texA) * lerpValue;
}

PS_OUTPUT main(PS_IN input)
{
	PS_OUTPUT output;
	output.RGBColor = float4(0, 0, 0, 0);

	float2 mainUV;

	//tevmode 8 doesnt actually exist, but nor does 7 on gamecube so i guess im allowed to cheat
	if (g_TevMode_0 == 8)
		mainUV = input.UV;
	else
		mainUV = input.TexCoord;

	float4 mainTex = tex2D(g_DiffuseTex, mainUV);

	if (g_TevMode_0 == 4) {
		output.RGBColor = input.Color0;
	}
	else if (g_TevMode_0 == 7) //shiny monotone
	{
        if (shinyJewelMonotone < 0.5f)
        {
            output.RGBColor = getLerpedTexture(mainTex, mainUV) + input.Color0;
        }
        else
        {
            output.RGBColor = mainTex + input.Color0;
        }
		
		output.RGBColor.w = input.Color0.w;
	}
	else {
		output.RGBColor = mainTex * input.Color0;
	}

	if (g_TevMode_0 == 8 || shinyJewelMonotone == 1) {
		float4 shinyTex = tex2D(g_shinyTex, input.TexCoord);
		output.RGBColor.xyz += getLerpedTexture(shinyTex, input.TexCoord);
	}

	//gamma shit
	output.RGBColor.xyz = log(output.RGBColor.xyz);
	output.RGBColor.xyz *= 0.9f;
	output.RGBColor.xyz = exp(output.RGBColor.xyz);

	output.RGBColor.xyz *= g_LightAllergy;

	return output;
}