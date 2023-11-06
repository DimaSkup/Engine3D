////////////////////////////////////////////////////////////////////
// Filename:    alphaMapPixel.hlsl
// Description: alpha mapping in DirectX is the process of using 
//              alpha layer of a texture to determine the blending
//              amount for each pixel when combining two textures;
//
// Created:     15.01.23
////////////////////////////////////////////////////////////////////

//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D textureArray[3];
SamplerState samplerType;

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

//////////////////////////////////
// Pixel shader
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 texColor1;
	float4 texColor2;
	float4 alphaValue;
	float4 blendColor;


	// get the pixel color from the 1st texture
	texColor1 = textureArray[0].Sample(samplerType, input.tex);

	// get the pixel color from the 2nd texture
	texColor2 = textureArray[1].Sample(samplerType, input.tex);

	// get the alpha value from the alpha map texture
	alphaValue = textureArray[2].Sample(samplerType, input.tex);


	// calculate the final color of two combined pixels
	blendColor = (alphaValue * texColor1) + ((1.0f - alphaValue) * texColor2);

	// saturate the final color value
	blendColor = saturate(blendColor);

	return blendColor;
}