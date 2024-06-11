////////////////////////////////////////////////////////////////////
// Filename:     bumpMapPixel.hlsl
// Description:  a pixel shader for bump mapping
//
// Created:      18.01.23
////////////////////////////////////////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D texDiffuse      : TEXTURE : register(t0);
Texture2D texNormals      : TEXTURE : register(t1);
SamplerState sampleType   : SAMPLER : register(s0);

// just like most light shaders the direction and colour of the light
// is required for lighting calculations
cbuffer LightBuffer
{
	float3 diffuseColor;
	float  padding_1;
	float3 lightDirection;
	float  padding_2;
	float3 ambientColor;
	float  padding_3;
};


//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 pos      : SV_POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
};


/*
	First we sample the pixel from the colour texture and the normal map. We then 
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
// Pixel Shader
//////////////////////////////////
float4 PS(PS_INPUT input): SV_TARGET
{
	float4 textureColor;
	float4 bumpMap;
	float3 bumpNormal;
	float3 lightDir;
	float  lightIntensity;
	float3 finalColor;                                         // the final colour

	// set the default output colour to the ambient colour value
	finalColor = ambientColor;

	// sample the texture pixel at this location
	textureColor = texDiffuse.Sample(sampleType, input.tex);

	// sample the pixel in the bump map
	bumpMap = texNormals.Sample(sampleType, input.tex);

	// expand the range of the normal value float (0, +1) to (-1, +1)
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// calculate the normal float the data in the bump map
	bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);

	// normalize the resulting bump normal
	bumpNormal = normalize(bumpNormal);

	// invert the light direction for calculations
	lightDir = -lightDirection;

	// calculate the amount of light on this pixel based on the bump map normal value
	//lightIntensity = saturate(dot(bumpNormal, lightDir));
	lightIntensity = saturate(dot(bumpNormal, lightDir));

	// if the N dot L is greater than zero we add the diffuse colour to the ambient colour
	if (lightIntensity > 0.0f)
	{
		// determine the final diffuse color based on the diffuse colour and the amount of light intensity
		finalColor += (diffuseColor * lightIntensity);

		// saturate the ambient and diffuse colour
		finalColor = saturate(finalColor);
	}
	

	// combine the final bump light colour with the texture colour
	return float4(finalColor, 1.0f) * textureColor;
}