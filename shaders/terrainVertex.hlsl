////////////////////////////////////////////////////////////////////
// Filename:     terrainVertex.hlsl
// Description:  a vertex shader for rendering the terrain; we need to 
//               have a particular shader because the rendering of the terrain
//               is very specialized;
// Created:      11.04.23
////////////////////////////////////////////////////////////////////


//////////////////////////////////
// DEFINES
//////////////////////////////////
#define NUM_LIGHTS 4    // the number of point light sources





//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};



// an array with positions of point light sources
cbuffer PointLightPositionBuffer : register(b1)
{
	float4 pointLightPos[NUM_LIGHTS];
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
	float4 color : COLOR;   // RGBA
	float4 distanceToPointLight : TEXTURE1;
	float4 depthPosition : TEXTURE0;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;

	float3 binormal : BINORMAL;
	float3 pointLightVector[NUM_LIGHTS] : TEXCOORD1;

	float2 tex : TEXCOORD0;
};


void SetIdx(inout float4 indices[4], uint i, uint val) {
	switch (i % 4) {
	case 0: indices[i / 4].x = val; break;
	case 1: indices[i / 4].y = val; break;
	case 2: indices[i / 4].z = val; break;
	case 3: indices[i / 4].w = val; break;
	}
}

//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float3x3 float3x3WorldMatrix = (float3x3)worldMatrix;
	float4 worldPosition;
	
	int i;

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

	// calculate the position of the vertex in the world
	worldPosition = mul(input.pos, worldMatrix);

	
	
	// the positions of the light sources in the world in relation to the vertex
	// must be calculated, normalized, and then sent into the pixel shader
	for (i = 0; i < NUM_LIGHTS; i++)
	{
		// determine the light position vector based on the position of the light and 
		// the position of the vertex in the world;
		output.pointLightVector[i] = pointLightPos[i].xyz - worldPosition.xyz;

		// normalize the light position vector
		output.pointLightVector[i] = normalize(output.pointLightVector[i]);
	}



	output.distanceToPointLight = float4(1000.0f, 1000.0f, 1000.0f, 1000.0f);
	for (uint it = 0; it < NUM_LIGHTS; it++)
	{
		
		//output.distanceToPointLight[i] = 0.0f;
		vector<float, 3> plp = { pointLightPos[it].x, pointLightPos[it].y, pointLightPos[it].z };
		vector<float, 3> wp = { worldPosition.x, worldPosition.y, worldPosition.z };
		float dist = distance(plp, wp);

		switch (it % 4)
		{
			case 0: output.distanceToPointLight.x = dist; break;
			case 1: output.distanceToPointLight.y = dist; break;
			case 2: output.distanceToPointLight.z = dist; break;
			case 3: output.distanceToPointLight.w = dist; break;
		}
	}
	

	return output;
}
