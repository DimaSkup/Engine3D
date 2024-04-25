#include "LightHelper.hlsli"


///////////////////////////////////////
// CONSTANT BUFFERS
///////////////////////////////////////

cbuffer cbPerObject : register(b0)
{
	matrix            gWorld;
	matrix            gWorldInvTranspose;
	matrix            gWorldViewProj;
	matrix            gTexTransform;
	Material          gMaterial;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VS_IN
{
	float3 posL    : POSITION;     // vertex position in local space
	float2 tex     : TEXCOORD;
	float3 normalL : NORMAL;       // vertex normal in local space
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;  // homogeneous position
	float3 posW    : POSITION;     // position in world
	float3 normalW : NORMAL;       // normal in world
	float2 tex     : TEXCOORD;
	
};



//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;

	// transform to homogeneous clip space
	vout.posH = mul(float4(vin.posL, 1.0f), gWorldViewProj);

	// transform to world space
	vout.posW = mul(float4(vin.posL, 1.0f), gWorld).xyz;

	// interpolating normal can unnormalize it, so normalize it
	vout.normalW = normalize(mul(vin.normalL, (float3x3)gWorldInvTranspose));

	// output vertex attributes for interpolation across triangle
	vout.tex = mul(float4(vin.tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}
