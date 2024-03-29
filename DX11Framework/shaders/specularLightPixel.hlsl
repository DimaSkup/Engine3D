//////////////////////////////////
// Filename: light.ps
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
	float3 specularColor;   // the specular colour is the reflected colour of the object's highlights
	float  specularPower;   // specular intensity
	float3 ambientColor;	// a common colour for the scene
	float  ambientStrength; // intensity of the ambient light
	float3 diffuseColor;    // a main directed colour (this colour and texture pixel colour are blending and make a final texture pixel colour of the model)
	float3 lightDirection;  // a direction of the diffuse colour
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 position      : SV_POSITION;
	float2 texCoord      : TEXCOORD0;
	float3 normal        : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

//////////////////////////////////
// Pixel Shader
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 textureColor;    // a pixel color from the texture by these coordinates
	float3 lightDir;        // an inverted light direction
	float  lightIntensity;  // an amount of the light on this pixel

	float3 specular;        // a specular light (color)
	float3 reflection;      // a reflection vector
	float3 finalColor;       

	// sample the pixel colour from the texture using the sampler by these texture coordinates
	textureColor = shaderTexture.Sample(sampleType, input.texCoord);

	// set the default output colour to the ambient colour value
	finalColor = ambientColor * ambientStrength;

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

		// calculate the reflection vector based on the light intensity, normal vector and light direction
		reflection = normalize(2 * lightIntensity * input.normal - lightDir);

		// calculate the specular light based on the reflection vector, view direction and specular power
		specular = pow(saturate(dot(input.viewDirection, reflection)), specularPower);
	}
	else
	{
		// initialize the specular colour
		specular = specularColor;
	}

	

	// add the specular component last to the output colour 
	finalColor = saturate(finalColor + specular);

	// multiply the final diffuse colour and texture colour to get the final pixel colour
	return float4(finalColor, 1.0f) * textureColor;

}