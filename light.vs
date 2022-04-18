//////////////////////////////////
// Filename: light.vs
// Revising: 18.04.22
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
};

//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;

	// change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// calculate the position of the vertex against the world, view, and projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	// calculate the normal vector against the world matrix only
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// normalize the normal vector
	output.normal = normalize(output.normal);

	return output;
}