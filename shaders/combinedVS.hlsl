////////////////////////////////////////////////////////////////////
// Filename:    combinedVS.hlsl
// Description: 
//
// Created:     28.01.23
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

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float  padding;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VS_INPUT
{
	float4 pos    : POSITION;
	float2 tex    : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos    : SV_POSITION;
	float2 tex    : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};


//////////////////////////////////
// Vertex shader
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4 worldPosition; // the position of the vertex in the world

	// change the position vector to be 4 units for proper matrix calculations
	input.pos.w = 1.0f;

	// calculate the vertex position agains the world, view, and projection matrices
	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;


	// calculate the normal vector against the world matrix and normalize the final value
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// calculate the vertex position against the world matrix
	worldPosition = mul(input.pos, worldMatrix);

	// calculate the view direction vector based on the camera position
	// and position of the vertex in the world
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// normalize the view direction vector
	output.viewDirection = normalize(output.viewDirection);

	return output;
}