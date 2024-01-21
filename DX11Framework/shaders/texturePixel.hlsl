//////////////////////////////////
// Filename: texture.ps
//////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


//////////////////////////////////
// CONSTANT BUFFERS
//////////////////////////////////
cbuffer CameraBuffer: register(b0)
{
	float3 cameraPosition;
	float  padding;
};


//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 position  : SV_POSITION;
	float3 positionW : POSITION;   // world position of the vertex
	float2 tex       : TEXCOORD0;
};

//////////////////////////////////
// PIXEL SHADER
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float4 finalColor;
	float4 fogColor = float4(0.5f, 0.5f, 0.5f, 1.0f);
	float fogStart = 2.0f;
	float fogRange = 20.0f;

	/////////////////////////  TEXTURE  ////////////////////////

	// Sample the pixel color from the texture using the sampler
	// at this texture coordinate location
	finalColor = shaderTexture.Sample(SampleType, input.tex);
	//textureColor.a = 1.0f;

	clip(finalColor.a - 0.1f);


	/////////////////////////   FOG   ///////////////////////////

	// the toEye vector is used in lighting
	float3 toEye = cameraPosition - input.positionW;

	// cache the distance to the eye from this surface point
	float distToEye = length(toEye);

	// normalize
	toEye = normalize(toEye);

	float fogLerp = saturate((distToEye - fogStart) / fogRange);

	// blend the fog color and the lit color
	finalColor = lerp(finalColor, fogColor, fogLerp);


	

	return finalColor;
}