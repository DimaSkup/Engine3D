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
#define NUM_LIGHTS 25   // the number of point light sources


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
	float3 toLight[NUM_LIGHTS] : TEXCOORD1;
};


//////////////////////////////////
// Pixel Shader
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 textureColor;
	//float  lightIntensity[NUM_LIGHTS];                // a light intensity on the current vertex
	float4 colorArray[NUM_LIGHTS];                    // the diffuse colour amount of each of the lights
	float4 colorSum = float4(0.0f, 0.0f, 0.0, 1.0f);  // final light color for this pixel                                   
	int i;
	float4 color;

	// sample the texture pixel at this location
	textureColor = shaderTexture.Sample(sampleType, input.tex);

	// the light intensity of each of the point lights is calculated using the position
	// of the light and the normal vector. The amount of colour contributed by each
	// point light is calculated from the intensity of the point light and the light colour.
	for (i = 0; i < NUM_LIGHTS; i++)
	{
		// determine the diffuse colour amount of each of the lights
		//colorArray[i] = diffuseColor[i] * );

		float distToLight = length(input.toLight[i]);

		// Phong diffuse
		float3 toLightNormalized = normalize(input.toLight[i]);
		float NDotL = saturate(dot(toLightNormalized, input.normal));
		float4 finalColor = diffuseColor[i] * NDotL;

		// attenuation
		float DistToLightNorm = 1.0f - saturate(distToLight * 0.05f);
		float Attn = DistToLightNorm * DistToLightNorm;

		colorArray[i] = finalColor * Attn;
	}
	

	// add all of the light colours up
	[unroll] for (i = 0; i < NUM_LIGHTS; i++)
	{
		colorSum += colorArray[i];
		//colorSum.r += colorArray[i].r;
		//colorSum.g += colorArray[i].g;
		//colorSum.b += colorArray[i].b;
	}

	// multiply the texture pixel by the combination of all
	// four light colours to get the final result
	return saturate(colorSum); //* textureColor;   // return the final colour of the pixel
}