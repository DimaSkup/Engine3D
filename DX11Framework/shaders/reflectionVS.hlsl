////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     reflectionVS.hlsl
// Description:  this is a HLSL vertex shader which is used for rendering
//               a basic planar reflections.
//               
////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// a constant buffer to hold the reflection matrix
cbuffer ReflectionBuffer
{
	matrix reflectionMatrix;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflectionPosition : TEXCOORD1;  // this variable is used to hold the projected reflection texture input position
};


////////////////////////////////////////////////////////////////////
// VERTEX SHADER
////////////////////////////////////////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	matrix reflectProjectWorld;

	// change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	// calculate the position of the vertex agains the world, view, and projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// store the texture coordinates for the pixel shader
	output.tex = input.tex;

	// create a matrix for transforming the input position values into the projected 
	// reflection position. This matrix is a combination of the reflection matrix,
	// the projection matrix, and the world matrix
	reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
	reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	// now transform the input position into the projected reflection position. These
	// transformed coordinates will be used in the pixel shader to derive where to map our
	// projected reflection texture to.
	output.reflectionPosition = mul(input.position, reflectProjectWorld);
	
	return output;
}