//////////////////////////////////
// Filename: texture.vs
//////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer MatrixBuffer : register(b0)
{
	matrix gWorldMatrix;
	matrix gWorldViewProj;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex      : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 posH      : SV_POSITION;
	float4 posW      : POSITION;   // world position of the vertex
	float2 tex       : TEXCOORD0;
};


//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// Change the position vector to be 4 units for proper matrix calculations
	input.pos.w = 1.0f;

	// Calculate the position of the vertex against the world, view and projection matrices
	output.posH = mul(input.pos, gWorldViewProj);
	
	// store the position of the vertex in the world for later computation in the pixel shader
	output.posW = mul(input.pos, gWorldMatrix);

	// Store the texture coordinates for the pixel shader
	output.tex = input.tex;

	return output;
}