//////////////////////////////////
// Filename: light.vs
// Revising: 11.05.22
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
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 viewDirection : TEXCOORD1;
};


struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
PixelInputType VertexLightShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	// change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// calculate the vertex position against the world, view, and projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	// calculate the normal vector against the world matrix
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// normalize the normal vector
	output.normal = normalize(output.normal);

	// calculate the vertex position in the world
	worldPosition = mul(input.position, worldMatrix);

	// determine the viewing direction based on the camera position and
	// the position of the vertex in the world
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// normalize the view direction vector
	output.viewDirection = normalize(output.viewDirection);

	return output;
}