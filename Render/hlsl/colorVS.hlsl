////////////////////////////////////////////////////////////////////////////////
// Filename:    colorVertex.hlsl
// Description: it is a Vertex Shader
////////////////////////////////////////////////////////////////////////////////



//////////////////////////
// CONST BUFFERS
//////////////////////////
cbuffer cbPerFrame : register(b0)
{
	matrix gViewProj;
};

///////////////////////////
// TYPEDEFS
///////////////////////////
struct VS_INPUT
{ 
	float3 posL   : POSITION;       // position of the vertex in local space
	row_major float4x4 world : WORLD;
	float4 color  : COLOR;          // color of the vertex
	uint instanceID : SV_InstanceID;
};

struct VS_OUTPUT
{
	float4 posH   : SV_POSITION;    // homogeneous position of the vertex 
	float4 color  : COLOR;
	float3 posW   : POSITION;       // vertex position in world space
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VS_OUTPUT VS(VS_INPUT vin)
{
	VS_OUTPUT vout;
	
	// transform pos from local to world space
	vout.posW = mul(float4(vin.posL, 1.0f), vin.world).xyz;

	// transform to homogeneous clip space
	vout.posH = mul(float4(vout.posW, 1.0f), gViewProj);

	// output vertex attributes for interpolation across triangle
	vout.color = vin.color;

	return vout;
}