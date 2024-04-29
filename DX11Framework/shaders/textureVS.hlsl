//////////////////////////////////
// Filename: texture.vs
//////////////////////////////////


//////////////////////////////////
// GLOBALS
//////////////////////////////////
cbuffer cbPerObj : register(b0)
{
	matrix gWorld;
	matrix gWorldViewProj;
	matrix gTexTransform;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VS_INPUT
{
	float4 posL     : POSITION;      // vertex position in a local space
	float2 tex      : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 posH      : SV_POSITION;  // homogeneous position of the vertex
	float4 posW      : POSITION;     // world position of the vertex
	float2 tex       : TEXCOORD0;
};


//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
VS_OUTPUT VS(VS_INPUT vin)
{
	VS_OUTPUT vout;

	// Change the position vector to be 4 units for proper matrix calculations
	vin.posL.w = 1.0f;

	// Calculate the position of the vertex against the world, view and projection matrices
	vout.posH = mul(vin.posL, gWorldViewProj);
	
	// store the position of the vertex in the world for later computation in the pixel shader
	vout.posW = mul(vin.posL, gWorld);

	// output vertex attributes for interpolation across triangle
	vout.tex = mul(float4(vin.tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}