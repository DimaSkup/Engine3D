////////////////////////////////////////////////////////////////////
// Filename:     terrainPixel.hlsl
// Description:  a pixel shader for rendering the terrain; we need to 
//               have a particular shader because the rendering of the terrain
//               is going to be very specialized;
// Created:      11.04.23
////////////////////////////////////////////////////////////////////

//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture : TEXTURE : register(t0);
SamplerState sampleType : SAMPLER : register(s0);

//////////////////////////////////
// CONSTANT BUFFERS
//////////////////////////////////
cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 color : COLOR;   // RGBA
};

//////////////////////////////////
// PIXEL SHADER
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 textureColor;
	float3 lightDir;       // light direction
	float  lightIntensity;
	float4 color;          // a final color of the vertex

	// sample the pixel color from the texture using the sampler at this texture coordinate location
	textureColor = shaderTexture.Sample(sampleType, input.tex);

	// set the default output colour to the ambient color value
	color = ambientColor;

	// combine the colour map value into the texture color
	textureColor = saturate(textureColor * input.color * 2.0f);

	// invert the light direction for calculation
	lightDir = -lightDirection;

	// calculate the amount of light on this pixel
	lightIntensity = saturate(dot(input.normal, lightDir));

	// determine the final amount of diffuse color based on the diffuse colour combined with the light intensity
	color += saturate(diffuseColor * lightIntensity);

	// multiply the texture pixel and the final diffuse colour to get the final pixel colour result
	color = color * textureColor;

	return color;


}
