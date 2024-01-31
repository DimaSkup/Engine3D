////////////////////////////////////////////////////////////////////
// Filename:     terrainPixel.hlsl
// Description:  a pixel shader for rendering the terrain; we need to 
//               have a particular shader because the rendering of the terrain
//               is going to be very specialized;
// Created:      11.04.23
////////////////////////////////////////////////////////////////////


//////////////////////////////////
// DEFINES
//////////////////////////////////
#define NUM_LIGHTS 6    // the number of point light sources


//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture : TEXTURE : register(t0);
Texture2D normalTexture : TEXTURE : register(t1);


//////////////////////////////////
// SAMPLERS
//////////////////////////////////
SamplerState sampleType : SAMPLER : register(s0);


//////////////////////////////////
// CONSTANT BUFFERS
//////////////////////////////////

cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;           // float3(color) + float(padding)
	float3 diffuseColor;
	float  padding_2;
	float3 lightDir_negative;      // negative light direction (-1 * lightDir; so we don't have to make it negative for each pixel)
};

// an array for the colours of the point lights
cbuffer PointLightColorBuffer : register(b1)
{
	float4 pointLightColor[NUM_LIGHTS];
	unsigned int numPointLights = 0;           // actual number of point light sources on the scene at the moment 
};

cbuffer CameraBuffer : register(b2)
{
	float3 cameraPosition;
};

cbuffer cbPerFrame : register(b3)
{
	// allow application to change for parameters once per frame.
	// For example, we may only use fog for certain times of day.
	float4 gFogColor;     // the colour of the fog (usually it's a degree of grey)

	float  gFogStart;     // how far from us the fog starts
	float  gFogRange;     // distance from the fog start position where the fog completely hides the surface point
	float  gFogRange_inv; // (1 / fogRange) inversed distance from the fog start position where the fog completely hides the surface point
	float  gFogEnabled;   // do we use fog or not?
	float  debugNormals;  // defines if we use normal vector value as a color of the pixel
};


//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 posW : POSITION;
	float4 color : COLOR;   // RGBA
	float  depthValue : TEXTURE1;    // distance from the camera position to this pixel
	float  distanceToPointLight[NUM_LIGHTS] : TEXTURE2;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;

	float3 binormal : BINORMAL;
	float3 pointLightVector[NUM_LIGHTS] : TEXCOORD1;

	float2 tex : TEXCOORD0;

};



//////////////////////////////////
// FUNCTIONS PROTOTYPES
//////////////////////////////////
float4 ComputePointLightsSum(in PS_INPUT input);



/*
	Some description of the main() function: 

	We sample the pixel from the normal map. We then 
	multiply the normal map value by two and then subtract one to move it into the
	-1.0 to +1.0 float range. We have to do this because the sampled value that is 
	presented to us in the 0.0 to +1.0 texture range which only covers half the range
	we need for bump map normal calculations. After that we then calculate the bump
	normal which uses the equation:
		bumpNormal = (bumpMap.x * input.tangent) + 
		             (bumpMap.y * input.binormal) + 
		             (bumpMap.z * input.normal);
	This bump normal is normalized and then used to determine the light intensity
	at this pixel by doing a dot product with the light direction. Once we have the
	light intensity at this pixel the bump mapping is now done. We use the light 
	intensity with the light colur and texture colour to get the final pixel colour.
*/
//////////////////////////////////
// PIXEL SHADER
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 textureColor;
	float  lightIntensity = 0;
	float  distToEye = length(cameraPosition - input.posW); // cache the distance to the eye from this surface point
	float4 finalColor = ambientColor;                       // set the default output color to the ambient light value for all pixels. 

	/////////////////////////////////////


	// if we want to use normal value as color of the pixel
	if (debugNormals)
	{
		return float4(input.normal, 1.0f);
	}

	/////////////////////////   FOG   ///////////////////////////
	
	if (gFogEnabled)
	{
		// (dist^2) > (start + range)^2
		// we don't have to compare precise lengths we can just compare squares of values
		if (pow(distToEye, 2) > pow(gFogStart + gFogRange, 2))
			return gFogColor;
	}

	/////////////////////////////////////

	// sample the pixel color from the texture using the sampler at this texture coordinate location
	textureColor = shaderTexture.Sample(sampleType, input.tex);

	// if this pixel is near than 0.9f we execute bump (normal) mapping
	if (distToEye < 0.9f)
	{
		// sample the pixel from the normal map
		float4 bumpMap = normalTexture.Sample(sampleType, input.tex);

		// expand the range of the normal value from (0, +1) to (-1, +1)
		bumpMap = (bumpMap * 2.0f) - 1.0f; 

		// calculate the normal from the data in the normal map
		float3 bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);

		// normalize the resulting bump normal 
		bumpNormal = normalize(bumpNormal);

		// calculate the amount of light on this pixel
		lightIntensity = saturate(dot(bumpNormal, lightDir_negative));
	}
	else
	{
		// calculate the amount of light on this pixel
		lightIntensity = saturate(dot(input.normal, lightDir_negative));
	}
	

	// determine the final amount of diffuse color based on 
	// the diffuse colour combined with the light intensity;
	// and saturate the final light color;
	if (lightIntensity > 0.0f)
	{
		finalColor = saturate(finalColor + float4(diffuseColor * lightIntensity, 1.0f));
	}

	
	// combine final pixel colour with the final diffuse color and 
	// with the colour map value into the final output color
	finalColor = saturate(finalColor * textureColor * input.color * 2.0f);

	//float4 colorSum = ComputePointLightsSum(input) / 5.0f;

	// combine the pixel color and the sum point lights colors on this pixel 
	//color = saturate(color + colorSum);



	/////////////////////////   FOG   ///////////////////////////


	if (gFogEnabled)
	{
		float fogLerp = saturate((distToEye - gFogStart) * gFogRange_inv);

		// blend the fog color and the lit color
		finalColor = lerp(finalColor, gFogColor, fogLerp);
	}



	return finalColor;

} // end main

///////////////////////////////////////////////////////////

float4 ComputePointLightsSum(in PS_INPUT input)
{
	// the light intensity of each of the point lights is calculated using the position
	// of the light and the normal vector of vertex. The amount of colour contributed by each
	// point light is calculated from the intensity of the point light and the light colour.

	float4 colorSum = float4(0.0f, 0.0f, 0.0, 1.0f);  // final color from point lights for this pixel  
	float4 colorArray[NUM_LIGHTS];                    // the diffuse colour amount of each of the lights
	float  pointLightIntensity[NUM_LIGHTS];           // a light intensity on the current vertex from the point light sources

	[unroll] for (uint it = 0; it < numPointLights; it++)
	{
		// calculate the different amounts of light on this pixel based on the position of the lights
		pointLightIntensity[it] = saturate(dot(input.normal, input.pointLightVector[it]));

		// determine the diffuse colour amount of each of the lights
		colorArray[it] = pointLightColor[it] *
			             pointLightIntensity[it] /
			             input.distanceToPointLight[it];
	} // end for

	// add up all of the light colours 
	for (it = 0; it < numPointLights; it++)
	{
		colorSum.r += colorArray[it].r;
		colorSum.g += colorArray[it].g;
		colorSum.b += colorArray[it].b;
	}

	return colorSum;

} // end ComputePointLightsSum