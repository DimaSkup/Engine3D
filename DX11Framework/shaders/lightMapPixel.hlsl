////////////////////////////////////////////////////////////////////
// Filename:    lightMapPixel.hlsl
// Description: the light map pixel shader bultiplies the colour texture
//              pixel and the light map texture value to get the desired output;
//
// Created:     13.01.23
////////////////////////////////////////////////////////////////////

//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTextures[2];
SamplerState sampleType;

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};



//////////////////////////////////
// Pixel Shader
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 textureColor;
	float4 lightColor;
	float4 finalColor;

	// get the pixel colour from the colour texture
	textureColor = shaderTextures[0].Sample(sampleType, input.tex);

	// get the pixel colour from the light map
	lightColor = shaderTextures[1].Sample(sampleType, input.tex);

	// blend the two pixels together
	finalColor = textureColor * lightColor;

	return finalColor;
}