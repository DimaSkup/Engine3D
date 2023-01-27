////////////////////////////////////////////////////////////////////
// Filename:    combinedPS.hlsl
// Description: 
//
// Created:     28.01.23
////////////////////////////////////////////////////////////////////

//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D textureArray[3] : TEXTURE;
SamplerState samplerType  : SAMPLER;


//////////////////////////////////
// CONSTANT BUFFERS
//////////////////////////////////
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
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

//////////////////////////////////
// Pixel shader
//////////////////////////////////
float4 main(PS_INPUT input) : SV_TARGET
{
	float4 texColor1;
	float4 texColor2;
	float4 alphaValue;
	float4 blendColor;

	float3 lightDir;        // an inverted light direction
	float  lightIntensity;  // an amount of the light on this pixel

	float3 reflection;      // a reflection vector
	float4 specular;        // a specular light (color)
	float4 color;           /* a final colour value */



	//////////////////////////// LIGHT CALCULATIONS ///////////////////////////////////

	// get the pixel color from the 1st texture
	texColor1 = textureArray[0].Sample(samplerType, input.tex);

	// get the pixel color from the 2nd texture
	texColor2 = textureArray[1].Sample(samplerType, input.tex);

	// get the alpha value from the alpha map texture
	alphaValue = textureArray[2].Sample(samplerType, input.tex);


	// calculate the final color of two combined pixels
	blendColor = (alphaValue * texColor1) + ((1.0f - alphaValue) * texColor2);

	// saturate the final color value
	blendColor = saturate(blendColor);



	//////////////////////////// LIGHT CALCULATIONS ///////////////////////////////////

	// set the default output colour to the ambient colour value
	color = ambientColor;

	// invert the light direction value for proper calculations
	lightDir = -lightDirection;

	// initialize the specular colour
	specular = specularColor;

	// calculate the amount of light on this pixel
	lightIntensity = saturate(dot(input.normal, lightDir));

	// if the N dot L is greater that zero we add the diffuse colour to the ambient colour
	if (lightIntensity > 0.0f)
	{
		// calculate the final diffuse colour based on the diffuse colour and light intensity
		color += (diffuseColor * lightIntensity);

		// saturate the ambient and diffuse colour
		color = saturate(color);

		// calculate the reflection vector based on the light intensity, normal vector and light direction
		reflection = normalize(2 * lightIntensity * input.normal - lightDir);

		// calculate the specular light base on the reflection vector, view direction and specular power
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	}

	// multiply the final diffuse colour and alpha textured colour to get the final pixel colour
	color = (color * blendColor);

	// add the specular component last to the output colour
	color = saturate(color + specular);
	
	return color;
}