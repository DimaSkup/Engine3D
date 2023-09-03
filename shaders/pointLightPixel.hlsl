////////////////////////////////////////////////////////////////////
// Filename:      pointLightPixel.hlsl
// Description:   this is a vertex shader for point lighting;
//                there can also be multiple light sources if the
//                NUM_LIGHTS > 1;
//
// Created:       27.08.23
////////////////////////////////////////////////////////////////////

//////////////////////////////////
// DEFINES
//////////////////////////////////
#define NUM_LIGHTS 4   // the number of point light sources


//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture : register(t0);
SamplerState sampleType : register(s0);


//////////////////////////////////
// CONSTANT BUFFERS
//////////////////////////////////

// an array for the colours of the point lights
cbuffer LightColorBuffer
{
	float4 diffuseColor[NUM_LIGHTS];
};


//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 lightPos[NUM_LIGHTS] : TEXCOORD1;
};


//////////////////////////////////
// Pixel Shader
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 textureColor;
	float  lightIntensity[NUM_LIGHTS];                // a light intensity on the current vertex
	float4 colorArray[NUM_LIGHTS];                    // the diffuse colour amount of each of the lights
	float4 colorSum = float4(0.0f, 0.0f, 0.0, 1.0f);  // final light color for this pixel                                   
	int i;
	float4 ambientLight = float4(0.2f, 0.2f, 0.2f, 0.2f);
	float4 color;
	
	//return float4(0.2f, 0.4f, 0.6f, 1.0f);

	// sample the texture pixel at this location
	textureColor = shaderTexture.Sample(sampleType, input.tex);

	//return saturate(textureColor * ambientLight);

	// the light intensity of each of the point lights is calculated using the position
	// of the light and the normal vector. The amount of colour contributed by each
	// point light is calculated from the intensity of the point light and the light colour.
	for (i = 0; i < NUM_LIGHTS; i++)
	{
		// calculate the different amounts of light on this pixel based on the position of the lights
		lightIntensity[i] = saturate(dot(input.normal, input.lightPos[i]));

		// determine the diffuse colour amount of each of the lights
		colorArray[i] = diffuseColor[i] * lightIntensity[i];
		
	}
	

	// add all of the light colours up
	for (i = 0; i < NUM_LIGHTS; i++)
	{
		colorSum.r += colorArray[i].r;
		colorSum.g += colorArray[i].g;
		colorSum.b += colorArray[i].b;
	}
	color =  saturate(colorSum) * textureColor;
	return color;

	// multiply the texture pixel by the combination of all
	// four light colours to get the final result
	return saturate(colorSum) * textureColor * ambientLight;   // return the final colour of the pixel
}