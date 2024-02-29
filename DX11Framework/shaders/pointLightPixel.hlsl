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
cbuffer PointLightColorBuffer : register(b0)
{
	float4 pointColor[NUM_LIGHTS];
};

cbuffer DiffuseLightBuffer : register(b1)
{
	float3 ambientColor;	     // a common colour for the scene
	float  ambientLightStrength; // the power of ambient light
	float3 diffuseColor;         // a main directed colour (this colour and texture pixel colour are blending and make a final texture pixel colour of the model)
	float  padding_1;
	float3 lightDirection;       // a direction of the diffuse colour
	float  padding_2;
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
	float4 colorSum = float4(0.0f, 0.0f, 0.0, 1.0f);  // final light color for this pixel                                   
	int i;
	float4 color;

	// sample the texture pixel at this location
	textureColor = shaderTexture.Sample(sampleType, input.tex);

	// the light intensity of each of the point lights is calculated using the position
	// of the light and the normal vector. The amount of colour contributed by each
	// point light is calculated from the intensity of the point light and the light colour.
	[unroll] for (i = 0; i < NUM_LIGHTS; i++)
	{
		// Phong diffuse	
		float NDotL = saturate(dot(normalize(input.toLight[i]), input.normal));

		// attenuation
		float DistToLightNorm = 1.0f - saturate(length(input.toLight[i]) * 0.05f);
	
		// add all of the light colours up
		// sum += light_color * light_dir_dot_plane * attenuation (dist_to_light ^ 2);
		colorSum += pointColor[i] * NDotL * DistToLightNorm * DistToLightNorm;
	}



	// set the default output colour to the ambient colour value
	float3 finalLight = float3(0.2f, 0.2f, 0.2f);

	// invert the light direction value for proper calculations
	float3 lightDir = -lightDirection;

	// calculate the amount of light on this pixel
	float lightIntensity = saturate(dot(input.normal, lightDir));

	// if the N dot L is greater than zero we add the diffuse colour to the ambient colour
	if (lightIntensity > 0.0f)
	{
		// calculate the final diffuse colour based on the diffuse colour and light intensity
		finalLight += (float3(1.0f, 1.0f, 1.0f) * lightIntensity);

		// saturate the ambient and diffuse colour
		finalLight = saturate(finalLight);
	}

	// multiply the texture pixel by the combination of all
	// four light colours to get the final result
	float4 finalColorLight = saturate((float4(finalLight, 1.0f)) + saturate(colorSum));

	// return the final colour of the pixel
	return  finalColorLight * textureColor;   
}