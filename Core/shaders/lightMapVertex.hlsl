////////////////////////////////////////////////////////////////////
// Filename:     lightMapVertex.hlsl
// Description:  light mapping in DirectX 11 is the process of using
//               a secondary texture or data file to create a fast look
//               up table to create unique lighting effects that require
//               very little processing. Because we use a secondary source
//               as the basis for our lighting we can remove any other 
//               light calculations from our application. This gains us
//               incredible speed.
//               With light mapping we require two textures. The first
//               texture is the base color texture. The second texture is the 
//               light map. Usually this is just a black and white texture 
//               representing the intensity of the light at each pixel.
//
// Created:      13.01.23
////////////////////////////////////////////////////////////////////

//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};


//////////////////////////////////
// Vertex Shader
//////////////////////////////////
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	// change the position vectore to be 4 units for proper matrix calculations
	input.pos.w = 1.0f;

	// calculate the position of the vertex agains the world, view, and projection matrices
	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	return output;
}