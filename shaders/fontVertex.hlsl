//////////////////////////////////
// Filename: font.vs
// Revising: 10.06.22
//////////////////////////////////

//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix orthoMatrix;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex      : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex      : TEXCOORD0;
};

//////////////////////////////////
// Vertex Shader
//////////////////////////////////
PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	// change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// calculate the vertex position against the world, view, and projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, orthoMatrix);

	// set the texture coordinates for the pixel shader
	output.tex = input.tex;

	return output;
}