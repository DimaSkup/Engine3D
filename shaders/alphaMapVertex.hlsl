////////////////////////////////////////////////////////////////////
// Filename:    alphaMapVertex.hlsl
// Description: alpha mapping in DirectX is the process of using 
//              alpha layer of a texture to determine the blending
//              amount for each pixel when combining two textures;
//
// Created:     15.01.23
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
// Vertex shader
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// change the position vector to be 4 units for proper matrix calculations
	input.pos.w = 1.0f;

	// calculate the vertex position agains the world, view, and projection matrices
	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	return output;
}