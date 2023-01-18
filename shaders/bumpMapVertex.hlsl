////////////////////////////////////////////////////////////////////
// Filename:     bumpMapVertex.hlsl
// Description:  a vertex shader for bump mapping
// 
// Created:      18.01.23
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
	float4 pos      : POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
};

struct VS_OUTPUT
{
	float4 pos      : SV_POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
};


//////////////////////////////////
// Vertex Shader
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// change the position vectore to be 4 units for proper matrix calculations
	intput.pos.w = 1.0f;

	// calculate the position of the vertex against the world, view, and projection matrices
	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	// store the texture coords for the pixel shader
	output.tex = input.tex;

	// calculate the normal vector against the world matrix only and the normalize the final value
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// calculate the tangent vector against the world matrix only and the normalize the final value
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	// calculate the binormal vector against the world matrix only and the normalize the final value
	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	return output;
}