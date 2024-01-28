//////////////////////////////////
// Filename: light.hlsl
// Revising: 16.05.22
//////////////////////////////////

//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture : TEXTURE : register(t0);
SamplerState sampleType : SAMPLER : register(s0);


//////////////////////////////////
// CONSTANT BUFFERS
//////////////////////////////////
cbuffer LightBuffer : register(b0)
{
	float3 ambientColor;	     // a common colour for the scene
	float  ambientLightStrength; // the power of ambient light
	float3 diffuseColor;         // a main directed colour (this colour and texture pixel colour are blending and make a final texture pixel colour of the model)
	float  padding_1;
	float3 lightDirection;       // a direction of the diffuse colour
	float  padding_2;
};

cbuffer CameraBuffer : register(b1)
{
	float3 cameraPosition;
	float  padding;
}

cbuffer BufferPerFrame : register(b2)
{
	// allow application to change for parameters once per frame.
	// For example, we may only use fog for certain times of day.
	float4 gFogColor;      // the colour of the fog (usually it's a degree of grey)
	float  gFogStart;      // how far from us the fog starts
	//float  gFogRange;      // distance from the fog start position where the fog completely hides the surface point
	float  gFogRange_inv;  // inversed value of the fog range (1 / range)

	bool   gFogEnabled;
	bool   debugNormals;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 position      : SV_POSITION;
	float4 positionW : POSITION;
	float2 texCoord      : TEXCOORD0;
	float3 normal        : NORMAL;
};

//////////////////////////////////
// Pixel Shader
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 textureColor;    // a pixel color from the texture by these coordinates
	float3 lightDir;        // an inverted light direction
	float  lightIntensity;  // an amount of the light on this pixel
	float3 finalLight;     
	float4 finalColor;         

	

	/////////////////////////////////////

	// if we want to use normal value as color of the pixel
	if (debugNormals)
	{
		return float4(input.normal, 1.0f);
	}
	
	/////////////////////////////////////
		
	// sample the pixel colour from the texture using the sampler by these texture coordinates
	textureColor = shaderTexture.Sample(sampleType, input.texCoord);


	// set the default output colour to the ambient colour value
	finalLight = ambientColor * ambientLightStrength;

	// invert the light direction value for proper calculations
	lightDir = -lightDirection;

	// calculate the amount of light on this pixel
	lightIntensity = saturate(dot(input.normal, lightDir));

	// if the N dot L is greater than zero we add the diffuse colour to the ambient colour
	if (lightIntensity > 0.0f)
	{
		// calculate the final diffuse colour based on the diffuse colour and light intensity
		finalLight += (diffuseColor * lightIntensity);

		// saturate the ambient and diffuse colour
		finalLight = saturate(finalLight);
	}

	// multiply the final diffuse light and texture colour to get the final pixel colour
	finalColor = float4(finalLight, 1.0f) * textureColor;

	/////////////////////////   FOG   ///////////////////////////

	if (gFogEnabled)
	{
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
		// the toEye vector is used in lighting
		float3 toEye = cameraPosition - input.positionW.xyz;
		float fogLerp = saturate((length(toEye) - gFogStart) * gFogRange_inv);

		// blend the fog color and the lit color
		return lerp(finalColor, gFogColor, fogLerp);
	}

	/////////////////////////////////////////////////////////////

	return finalColor;
}