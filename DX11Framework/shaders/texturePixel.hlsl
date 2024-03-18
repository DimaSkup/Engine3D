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
	float3 gCameraPosition;
	float  padding;
};

cbuffer cbRarelyChanged : register(b1)
{
	// allow application to change for parameters once per frame.
	// For example, we may only use fog for certain times of day.

	float  gFogEnabled;
	float  gUseAlphaClip;
	float  gFogStart;      // how far from us the fog starts
	//float  gFogRange_inv;  // inversed value of the fog range (1 / range)
	float  gFogRange;      // distance from the fog start position where the fog completely hides the surface point

	float4 gFogColor;      // the colour of the fog (usually it's a degree of grey)
};


//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 posH      : SV_POSITION;
	float4 posW      : POSITION;   // world position of the vertex
	float2 tex       : TEXCOORD0;
};

//////////////////////////////////
// PIXEL SHADER
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{

	/////////////////////////  TEXTURE  ////////////////////////

	// Sample the pixel color from the texture using the sampler
	// at this texture coordinate location
	float4 finalColor = shaderTexture.Sample(SampleType, input.tex);

	// the pixels with black (or lower that 0.1f) alpha values will be refected by
	// the clip function and not draw (this is used for rendering wires/fence/etc.);
	//
	// if the pixel was rejected we just return from the pixel shader since 
	// any further computations have no sense
	if (gUseAlphaClip)
	{
		clip(finalColor.a - 0.1f);
		return finalColor;
	}
	

	/////////////////////////   FOG   ///////////////////////////

	if (gFogEnabled)
	{
		// the toEye vector is used in lighting
		float3 toEye = gCameraPosition - input.posW.xyz;
		float distToEye = length(toEye);
		float fogLerp = saturate((distToEye - gFogStart) / gFogRange);

		// blend the fog color and the lit color
		finalColor = lerp(finalColor, gFogColor, fogLerp);
	}

	/////////////////////////////////////////////////////////////

	return finalColor;
}