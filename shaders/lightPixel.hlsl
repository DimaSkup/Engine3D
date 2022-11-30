//////////////////////////////////
// Filename: light.ps
// Revising: 16.05.22
//////////////////////////////////

//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture : TEXTURE : register(t0);
SamplerState sampleType : SAMPLER : register(s0);

cbuffer LightBuffer
{
	float4 ambientColor;	// a common colour for the scene
	float4 diffuseColor;    // a main directed colour (this colour and texture pixel colour are blending and make a final texture pixel colour of the model)
	float3 lightDirection;  // a direction of the diffuse colour
	float  specularPower;   // specular intensity
	float4 specularColor;   // the specular colour is the reflected colour of the object's highlights
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

	float3 reflection;      // a reflection vector
	float4 specular;        // a specular light (color)
	float4 color;           // a final colour

	// sample the pixel colour from the texture using the sampler by these texture coordinates
	textureColor = shaderTexture.Sample(sampleType, input.texCoord);

	// set the default output colour to the ambient colour value
	color = ambientColor;

	// invert the light direction value for proper calculations
	lightDir = -lightDirection;

	// initialize the specular colour
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// calculate the amount of light on this pixel
	lightIntensity = saturate(dot(input.normal, lightDir));

	// if the N dot L is greater than zero we add the diffuse colour to the ambient colour
	if (lightIntensity > 0.0f)
	{
		// calculate the final diffuse colour based on the diffuse colour and light intensity
		color += (diffuseColor * lightIntensity);

		// saturate the ambient and diffuse colour
		color = saturate(color);

		// calculate the reflection vector based on the light intensity, normal vector and light direction
		reflection = normalize(2 * lightIntensity * input.normal - lightDir);

		// calculate the specular light based on the reflection vector, view direction and specular power
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	}

	// multiply the final diffuse colour and texture colour to get the final pixel colour
	color = (color * textureColor);

	// add the specular component last to the output colour 
	color = saturate(color + specular);

	return color;
}