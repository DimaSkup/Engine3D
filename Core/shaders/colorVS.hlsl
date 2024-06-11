////////////////////////////////////////////////////////////////////////////////
// Filename:    colorVertex.hlsl
// Description: it is a Vertex Shader
////////////////////////////////////////////////////////////////////////////////



//////////////////////////
// CONST BUFFERS
//////////////////////////
cbuffer cbPerObject : register(b0)
{
	matrix gWorldViewProj;    // world_matrix * view_matrix * projection_matrix
	float3 gRGBColor;
	float  gUseVertexColor;   // do we for painting an own color of the vertex ?
};

///////////////////////////
// TYPEDEFS
///////////////////////////
struct VS_INPUT
{ 
	float3 posL   : POSITION;       // position of the vertex in local space
	float4 color  : COLOR;          // color of the vertex
};

struct VS_OUTPUT
{
	float4 posH   : SV_POSITION;    // homogeneous position of the vertex 
	float4 color  : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VS_OUTPUT VS(VS_INPUT vin)
{
	VS_OUTPUT vout;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	vout.posH = mul(float4(vin.posL, 1.0f), gWorldViewProj);

	// define what color will we use for painting of the geometry
	if (gUseVertexColor)
	{
		vout.color = vin.color;
	}
	else
	{
		vout.color = float4(gRGBColor, 1.0f);
	}

	return vout;
}