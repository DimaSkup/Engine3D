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
cbuffer LightBuffer
{
	float3 diffuseColor;         // a main directed colour (this colour and texture pixel colour are blending and make a final texture pixel colour of the model)
	float3 lightDirection;       // a direction of the diffuse colour
	float3 ambientColor;	     // a common colour for the scene
	float  ambientLightStrength; // the power of ambient light
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 position      : SV_POSITION;
	float2 texCoord      : TEXCOORD0;
	float3 normal        : NORMAL;
	//float3 viewDirection : TEXCOORD1;
};

//////////////////////////////////
// Pixel Shader
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float3 textureColor;    // a pixel color from the texture by these coordinates
	float3 lightDir;        // an inverted light direction
	float  lightIntensity;  // an amount of the light on this pixel
	float4 finalColor;         

	// sample the pixel colour from the texture using the sampler by these texture coordinates
	//textureColor = shaderTexture.Sample(sampleType, input.texCoord);
	textureColor = input.normal;

	/////////////////////////////////////
	float3 ambientLight = ambientColor * ambientLightStrength;
	float3 color = textureColor * ambientLight;
	return float4(color, 1.0f);
		
/*


	// set the default output colour to the ambient colour value
	finalColor = ambientColor * ambientLightStrength;

	// invert the light direction value for proper calculations
	lightDir = -lightDirection;

	// calculate the amount of light on this pixel
	lightIntensity = saturate(dot(input.normal, lightDir));

	// if the N dot L is greater than zero we add the diffuse colour to the ambient colour
	if (lightIntensity > 0.0f)
	{
		// calculate the final diffuse colour based on the diffuse colour and light intensity
		finalColor += (diffuseColor * lightIntensity);

		// saturate the ambient and diffuse colour
		finalColor = saturate(finalColor);
	}

	// multiply the final diffuse colour and texture colour to get the final pixel colour
	return finalColor * textureColor;

*/
}