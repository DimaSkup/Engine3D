//////////////////////////////////
// Filename: light.vs
// Revising: 16.05.22
//////////////////////////////////

//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float  padding;
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
	float3 viewDirection : TEXCOORD1;
};

//////////////////////////////////
// Vertex Shader
//////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4 worldPosition;	// the position of the vertex in the world

	// change the position vector to be 4 units for proper calculations
	input.position.w = 1.0f;

	// calculate the position against the world, view, and projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// store the texture coordinates
	output.tex = input.tex;

	// calculate the normal vector against the world matrix and normalize the final value
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// calculate the vertex position in the world matrix
	worldPosition = mul(input.position, worldMatrix);

	// calculate the view direction vector based on the camera position 
	// and position of the vertex in the world
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// normalize the view direction vector
	output.viewDirection = normalize(output.viewDirection);

	return output;
}
