////////////////////////////////////////////////////////////////////
// Filename:      pointLightVertex.hlsl
// Description:   this is a vertex shader for point lighting;
//                there can also be multiple light sources if the
//                NUM_LIGHTS > 1;
//
// Created:       27.08.23
////////////////////////////////////////////////////////////////////


//////////////////////////////////
// DEFINES
//////////////////////////////////
#define NUM_LIGHTS 25    // the number of point light sources


//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix worldViewProj;
};

// an array with positions of point light sources
cbuffer LightPositionBuffer
{
	float4 lightPosition[NUM_LIGHTS];
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
	float4 position : SV_POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 toLight[NUM_LIGHTS] : TEXCOORD1;
};


//////////////////////////////////
// Vertex Shader
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4 worldPosition;
	int i = 0;

	// change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// calculate the position of the vertex agains the world, view, and projection matrices
	output.position = mul(input.position, worldViewProj);

	// calculate the position of the vertex in the world
	worldPosition = mul(input.position, worldMatrix);

	// store the texture coords for the pixel shader
	output.tex = input.tex;

	// calculate the normal vector against the world matrix only
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// normalize the normal vector
	output.normal = normalize(output.normal);


	// the positions of the light sources in the world in relation to the vertex
	// must be calculated, normalized, and then sent into the pixel shader
	[unroll] for (i = 0; i < NUM_LIGHTS; i++)
	{
		// determine the light position vector based on the position of the light and 
		// the position of the vertex in the world;
		output.toLight[i] = lightPosition[i].xyz - worldPosition.xyz;
	}

	return output;
}