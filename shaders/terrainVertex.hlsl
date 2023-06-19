////////////////////////////////////////////////////////////////////
// Filename:     terrainVertex.hlsl
// Description:  a vertex shader for rendering the terrain; we need to 
//               have a particular shader because the rendering of the terrain
//               is very specialized;
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
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;  // RGBA
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;  // RGBA
	float4 depthPosition : TEXTURE0;
};


//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float3x3 float3x3WorldMatrix = (float3x3)worldMatrix;

	// change the position vector to be 4 units for proper matrix calculations
	input.pos.w = 1.0f;

	// calculate the position of the vertex agains the world, view, and projection matrices
	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	// store the position value in a second input value for depth value calculations
	output.depthPosition = output.pos;

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	// calculate the normal vector agains the world matrix and normalize the final value
	output.normal = mul(input.normal, float3x3WorldMatrix);
	output.normal = normalize(output.normal);

	// calculate the tangent vector against the world matrix only and then normalize the final value
	output.tangent = mul(input.tangent, float3x3WorldMatrix);
	output.tangent = normalize(output.tangent);

	// calculate the binormal vector against the world matrix only and then normalize the final value
	output.binormal = mul(input.binormal, float3x3WorldMatrix);
	output.binormal = normalize(output.binormal);

	// store the input color for the pixel shader to use
	output.color = input.color;

	return output;
}
