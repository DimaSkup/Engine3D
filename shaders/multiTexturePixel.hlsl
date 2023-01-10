////////////////////////////////////////////////////////////////////
// Filename:     multiTexturePixel.hlsl
// Description:  a pixel shader for handling a multitexturing
// Created:      09.01.23
////////////////////////////////////////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTextures[2];           // a two element texture array is needed for the two different textures that will be bleded together
SamplerState SampleType;


//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 tex      : TEXCOORD0;
};


/*
	We take a sample of the pixel from both textures at this current texture coordinate.
	After that we combine them using multiplication since they are non-linear due to
	gamma correction. We also multiply by a gamma value, we have used 2.0 here as it is 
	close to most monitor's gamma value. Once we have blended pixel we saturate it
	and then return it as out final result.
*/

//////////////////////////////////
// Pixel Shader
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 color1;
	float4 color2;
	float4 blendColor;
	float4 gammaValue = 2.0f;

	// get the pixel color from the first texture
	color1 = shaderTextures[0].Sample(SampleType, input.tex);

	// get the pixel color from the second texture
	color2 = shaderTextures[1].Sample(SampleType, input.tex);

	// blend the two pixels together and multiply by the gamma value
	blendColor = color1 * color2 * gammaValue;

	// saturate the final color
	blendColor = saturate(blendColor);

	return blendColor;
}