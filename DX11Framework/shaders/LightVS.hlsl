#include "LightHelper.hlsli"


///////////////////////////////////////
// CONSTANT BUFFERS
///////////////////////////////////////

cbuffer cbPerObject : register(b0)
{
	matrix            gWorld;
	matrix            gWorldInvTranspose;
	matrix            gWorldViewProj;
	Material          gMaterial;
};

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VS_IN
{
	float3 posL    : POSITION;     // vertex position in local space
	float3 normalL : NORMAL;       // vertex normal in local space
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;  // homogeneous position
	float3 posW    : POSITION;     // position in world
	float3 normalW : NORMAL;       // normal in world
};



//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;

	// transform to world space
	vout.posW = mul(float4(vin.posL, 1.0f), gWorld).xyz;

	// interpolating normal can unnormalize it, so normalize it
	vout.normalW = normalize(mul(vin.normalL, (float3x3)gWorldInvTranspose));

	// transform to homogeneous clip space
	vout.posH = mul(float4(vin.posL, 1.0f), gWorldViewProj);

	return vout;
}
