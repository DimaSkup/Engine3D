//////////////////////////////////
// Filename: texture.vs
//////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer MatrixBuffer : register(b0)
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
// VERTEX SHADER
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// Change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view and projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader
	output.tex = input.tex;

	return output;
}