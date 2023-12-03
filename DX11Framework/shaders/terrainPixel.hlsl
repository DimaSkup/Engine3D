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
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
};

// an array for the colours of the point lights
cbuffer PointLightColorBuffer : register(b1)
{
	float4 pointLightColor[NUM_LIGHTS];
	unsigned int numPointLights = 0;           // actual number of point light sources on the scene at the moment 
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;   // RGBA
	float distanceToPointLight[NUM_LIGHTS] : TEXTURE1;
	float4 depthPosition : TEXTURE0;

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
	float4 bumpMap;        // a pixel color from the normal map
	float4 color;          // a final color of the vertex
	float3 lightDir;       // light direction
	float3 bumpNormal;     // a normal for the normal map lighting
	float  lightIntensity;
	float depthValue;
	int i;
		

	// get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate
	depthValue = input.depthPosition.z / input.depthPosition.w;

	// sample the pixel color from the texture using the sampler at this texture coordinate location
	textureColor = shaderTexture.Sample(sampleType, input.tex);

	// Set the default output color to the ambient light value for all pixels.
	color = ambientColor;

	// invert the light direction for calculation
	lightDir = -lightDirection;

	
	// if this pixel is near than 1.5f we execute bamp (normal) mapping
	if (depthValue < 1.5f)   
	{
		// sample the pixel from the normal map
		bumpMap = normalTexture.Sample(sampleType, input.tex);

		// expand the range of the normal value from (0, +1) to (-1, +1)
		bumpMap = (bumpMap * 2.0f) - 1.0f; 

		// calculate the normal from the data in the normal map
		bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);

		// normalize the resulting bump normal 
		bumpNormal = normalize(bumpNormal);

		// calculate the amount of light on this pixel
		lightIntensity = saturate(dot(bumpNormal, lightDir));
	}
	else
	{
		// calculate the amount of light on this pixel
		lightIntensity = saturate(dot(input.normal, lightDir));
	}

	
	// determine the final amount of diffuse color based on 
	// the diffuse colour combined with the light intensity;
	// and saturate the final light color;
	if (lightIntensity > 0.0f)
	{
		color += (diffuseColor * lightIntensity);
	}

	color = saturate(color);

	// multiply the texture pixel and the final diffuse colour to get the final pixel colour result
	color *= textureColor;

	// combine the colour map value into the final output color
	color = saturate(color * input.color * 2.0f);

	float4 colorSum = ComputePointLightsSum(input) / 5.0f;

	// combine the pixel color and the sum point lights colors on this pixel 
	color = saturate(color + colorSum);

	return color;

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