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
#define NUM_LIGHTS 6    // the number of point light sources



//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	//matrix worldViewProj;  // world * view * projection
	matrix viewMatrix;
	matrix projectionMatrix;
};



// an array with positions of point light sources
cbuffer PointLightPositionBuffer : register(b1)
{
	vector<float, 3> pointLightPos[NUM_LIGHTS];
	unsigned int numPointLights = 4;   // actual number of point light sources on the scene at the moment 
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
	float4 posW : POSITION;   // world position of the vertex

	float4 color : COLOR;   // RGBA
	float depthValue : TEXTURE1;
	float distanceToPointLight[NUM_LIGHTS] : TEXTURE2;
	//float4 depthPosition : TEXTURE0;

	float3 normal : NORMAL;
	float3 tangent : TANGENT;

	float3 binormal : BINORMAL;
	float3 pointLightVector[NUM_LIGHTS] : TEXCOORD1;

	float2 tex : TEXCOORD0;

	
};


//////////////////////////////////
// FUNCTIONS / PROTOTYPES
//////////////////////////////////

void ComputePointLightsDistance(inout VS_OUTPUT output, in vector<float, 3> worldPosition);


//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	vector<float, 3> worldPosition;
	float3x3 float3x3WorldMatrix = (float3x3)worldMatrix;

	// change the position vector to be 4 units for proper matrix calculations
	input.pos.w = 1.0f;


	// calculate the position of the vertex agains the world, view, and projection matrices
	output.pos = mul(input.pos, worldMatrix);

	// store the position of the vertex in the world for later computation in the pixel shader
	output.posW = output.pos;

	// get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate
	output.depthValue = output.pos.z / output.pos.w;

	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	

	// calculate the position of the vertex agains the world, view, and projection matrices
	//output.pos = mul(input.pos, worldViewProj);

	// store the position value in a second input value for depth value calculations
	//output.depthPosition = output.pos;

	// store the position of the vertex in the world for later computation in the pixel shader
	// (for instance: when we compute the fog effect)
	//output.posW = mul(input.pos, worldMatrix);


	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	// calculate the normal vector agains the world matrix and normalize the final value
	output.normal = normalize(mul(input.normal, float3x3WorldMatrix));

	// calculate the tangent vector against the world matrix only and then normalize the final value
	output.tangent = normalize(mul(input.tangent, float3x3WorldMatrix));

	// calculate the binormal vector against the world matrix only and then normalize the final value
	output.binormal = normalize(mul(input.binormal, float3x3WorldMatrix));

	// store the input color for the pixel shader to use
	output.color = input.color;

	// compute all the distances from the point light sources to this vertex
	ComputePointLightsDistance(output, output.posW.xyz);

	return output;

} // end main

///////////////////////////////////////////////////////////

void ComputePointLightsDistance(inout VS_OUTPUT output, in vector<float, 3> worldPosition)
{
	// the positions of the light sources in the world in relation to the vertex
	// must be calculated, normalized, and then sent into the pixel shader
	[unroll] for (uint i = 0; i < NUM_LIGHTS; i++)
	{
		// determine the light position vector based on the position of the light and 
		// the position of the vertex in the world;
		output.pointLightVector[i] = pointLightPos[i] - worldPosition;

		// normalize the light position vector
		output.pointLightVector[i] = normalize(output.pointLightVector[i]);
	}



	// calculate distances from this vertex to each point light on the scene
	[unroll] for (uint it = 0; it < numPointLights; it++)
	{
		//vector<float, 3> wp = { worldPosition.x, worldPosition.y, worldPosition.z };
		float dist = distance(pointLightPos[it], worldPosition);

		output.distanceToPointLight[it] = dist;
	}

	return;

}