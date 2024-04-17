////////////////////////////////////////////////////////////////////
// Filename:     multiTextureVertex.hlsl
// Description:  a vertex shader for handling a multitexturing
// Created:      09.01.23
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
	float4 position : POSITION;
	float2 tex      : TEXCOORD0;
};


struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex      : TEXCOORD0;
};



//////////////////////////////////
// Vertex Shader
//////////////////////////////////

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	// change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// calculate the position of the vertex against the world, view, and proj matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	return output;
}