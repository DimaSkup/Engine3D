////////////////////////////////////////////////////////////////////
// Filename:     terrainVertex.hlsl
// Description:  a vertex shader for rendering the terrain; we need to 
//               have a particular shader because the rendering of the terrain
//               is going to be very specialized;
// Created:      11.04.23
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
	float3 normal : NORMAL;
	float3 color : COLOR;  // RGBA
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 color : COLOR;  // RGBA
};


//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// change the position vector to be 4 units for proper matrix calculations
	input.pos.w = 1.0f;

	// calculate the position of the vertex agains the world, view, and projection matrices
	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	// calculate the normal vector agains the world matrix and normalize the final value
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// store the input color for the pixel shader to use
	output.color = float4(input.color, 1.0f);

	return output;
}
