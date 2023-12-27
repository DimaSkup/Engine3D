////////////////////////////////////////////////////////////////////////////////
// Filename:    colorVertex.hlsl
// Description: it is a Vertex Shader
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// PAY ATTENTION that in the HLSL shader in some cases we use only alpha value not the whole color;
// instead of it we use own color of the vertex;
cbuffer ColorBuffer
{
	float4 rgbaColor;                // color from the data container
};



//////////////
// TYPEDEFS //
//////////////
//////////////
struct VS_INPUT
{ 
	float4 position : POSITION;       // position of the vertex
	float4 color    : COLOR;          // color of the vertex
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;


	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the input color for the pixel shader to use;
	// PAY ATTENTION that in the HLSL shader in some cases we use only alpha value not the whole color;
	// instead of it we use own color of the vertex;
	output.color = rgbaColor;

	return output;
}