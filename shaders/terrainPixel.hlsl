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
#define NUM_LIGHTS 4    // the number of point light sources


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
cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
};


// an array for the colours of the point lights
cbuffer PointLightColorBuffer
{
	float4 pointLightColor[NUM_LIGHTS];
};


//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;   // RGBA
	float4 depthPosition : TEXTURE0;
	float3 lightPos[NUM_LIGHTS] : TEXCOORD1;
};




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
	float3 lightDir;       // light direction
	float4 bumpMap;        // a pixel color from the normal map
	float3 bumpNormal;     // a normal for the normal map lighting
	float  lightIntensity;
	float  pointLightIntensity[NUM_LIGHTS];
	float4 color;          // a final color of the vertex
	float depthValue;

	// get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate
	depthValue = input.depthPosition.z / input.depthPosition.w;

	// sample the pixel color from the texture using the sampler at this texture coordinate location
	textureColor = shaderTexture.Sample(sampleType, input.tex);

	// set the default output colour to the ambient color value
	color = ambientColor;

	// invert the light direction for calculation
	lightDir = -lightDirection;

	if (depthValue < 0.9f)   // execute bamp mapping
	{
		// calculate the amount of light on this pixel using the normal map
		bumpMap = normalTexture.Sample(sampleType, input.tex);
		bumpMap = (bumpMap * 2.0f) - 1.0f;
		bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
		bumpNormal = normalize(bumpNormal);
		lightIntensity = saturate(dot(bumpNormal, lightDir));
	}
	else
	{
		// calculate the amount of light on this pixel
		lightIntensity = saturate(dot(input.normal, lightDir));
	}


	if (lightIntensity > 0.0f)
	{
		// determine the final amount of diffuse color based on the diffuse colour combined with the light intensity
		color += (diffuseColor * lightIntensity);

		// saturate the final light color
		color = saturate(color);   
	}

	// multiply the texture pixel and the final diffuse colour to get the final pixel colour result
	color *= textureColor;

	// combine the colour map value into the final output color
	color = saturate(color * input.color * 2.0f);

	return color;


}
