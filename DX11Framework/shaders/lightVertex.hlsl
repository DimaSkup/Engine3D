//////////////////////////////////
// Filename: light.hlsl
// Revising: 16.05.22
//////////////////////////////////

//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix worldViewProj;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
};

struct VS_OUTPUT
{
	float4 position  : SV_POSITION;
	float4 positionW : POSITION;
	float2 tex       : TEXCOORD0;
	float3 normal    : NORMAL;
};

//////////////////////////////////
// Vertex Shader
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// change the position vector to be 4 units for proper calculations
	input.position.w = 1.0f;

	// calculate the position against the world, view, and projection matrices
	output.position = mul(input.position, worldViewProj);

	// store the position of the vertex in the world for later computation in the pixel shader
	output.positionW = mul(input.position, worldMatrix);

	// store the texture coordinates
	output.tex = input.tex;

	// calculate the normal vector against the world matrix and normalize the final value
	output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));

	return output;
}
