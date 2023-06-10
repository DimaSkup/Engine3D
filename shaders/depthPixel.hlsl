////////////////////////////////////////////////////////////////////
// Filename: depthPixel.hlsl
// Created:  10.06.23
////////////////////////////////////////////////////////////////////

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};


//////////////////////////////////
// PixelShader
//////////////////////////////////
float4 main(PS_INPUT input): SV_TARGET
{
	float depthValue;
	float4 color;

	

	// get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate
	depthValue = input.depthPosition.z / input.depthPosition.w;

	//color = float4(depthValue, depthValue, depthValue, 1.0f);
	//return color;

	// first 10% of the depth buffer color red
	if (depthValue < 0.9f)
	{
		color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	// the next 0.025% portion of the depth buffer color green
	if (depthValue > 0.9f)
	{
		color = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}

	// the remainder of the depth buffer color blue
	if (depthValue > 0.925f)
	{
		color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	return color;
}
