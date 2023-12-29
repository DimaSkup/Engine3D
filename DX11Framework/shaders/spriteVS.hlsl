//////////////////////////////////
// Filename: spriteVS.hlsl
//////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer MatrixBuffer : register(b0)
{
	matrix WVO;   // world_matrix * base_view_matrix * orhto_matrix
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

	// just copy input position (WVP) as we've already computed it before
	output.position = input.position;

	// Store the texture coordinates for the pixel shader
	output.tex = input.tex;

	return output;
}