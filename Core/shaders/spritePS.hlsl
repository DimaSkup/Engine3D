//////////////////////////////////
// Filename: spritePS.hlsl
//////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 tex      : TEXCOORD0;
};

//////////////////////////////////
// PIXEL SHADER
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 textureColor;

	// Sample the pixel color from the texture using the sampler
	// at this texture coordinate location
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	textureColor.a = 1.0f;

	return textureColor;
}