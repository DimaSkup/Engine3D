////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     reflectionVS.hlsl
// Description:  this is a HLSL pixel shader which is used for rendering
//               a basic planar reflections.
//               
////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture     : register(t0);
Texture2D reflectionTexture : register(t1);   // a texture variable for the scene reflection rendered to texture
SamplerState sampleType     : register(s0);

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflectionPosition : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////
// PIXEL SHADER
////////////////////////////////////////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 textureColor;
	float2 reflectTexCoord;
	float4 reflectionColor;
	float4 finalColor;       // the result colour of this pixel


	// sample the texture pixel at this location
	textureColor = shaderTexture.Sample(sampleType, input.tex);

	// the input reflection position homogenous coordinates need to be converted to proper
	// texture coordinates. To do so first divide by the W coordinate. This leaves us with
	// tu and tv coordinates in the -1 to +1 range, to fix it to map to a 0 to +1 range
	// just divide it by 2 and add 0.5.
	reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w * 0.5f + 0.5f;
	reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w * 0.5f + 0.5f;

	// now when we sample from the reflection texture we used the projected reflection 
	// coordinates that have been converted to get the right reflection pixel for this 
	// projected reflection position
	reflectionColor = reflectionTexture.Sample(sampleType, reflectTexCoord);

	// finally, we blend the texture from the reflection plane (or another reflection obj)
	// with the reflection texture to create the effect of the reflection. Here we use 
	// a linear interpolation between the two textures with a factor of 0.15. You can change
	// this to a different blend equation or change the factor amount for a different or
	// stronger effect.
	finalColor = lerp(textureColor, reflectionColor, 0.15f);

	return finalColor;
}