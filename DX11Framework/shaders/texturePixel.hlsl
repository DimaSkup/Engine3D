//////////////////////////////////
// Filename: texture.ps
//////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


//////////////////////////////////
// CONSTANT BUFFERS
//////////////////////////////////
cbuffer alphaBuffer : register(b0)
{
	float alpha;
}

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex      : TEXCOORD0;
};

//////////////////////////////////
// PIXEL SHADER
//////////////////////////////////
float4 main(PixelInputType input): SV_TARGET
{
	float4 textureColor;

	// Sample the pixel color from the texture using the sampler
	// at this texture coordinate location
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	textureColor.a = alpha;

	return textureColor;
}