#include "LightHelper.fx"


///////////////////////////////////////
// CONSTANT BUFFERS
///////////////////////////////////////
cbuffer cbPerFrame : register(b0)
{
	DirectionalLight gDirLight;
	PointLight       gPointLight;
	SpotLight        gSpotLight;
	float3           gEyePosW;     // eye position in world
};


cbuffer cbPerObject : register(b1)
{
	matrix gWorld;
	matrix gWorldInvTranspose;
	matrix gWorldViewProj;
	Material gMaterial;
};

cbuffer cbRareChanged : register(b2)
{
	// some flags for controlling the rendering process

	float  gFogEnabled;        // do we use fog effect?
	//float  gDebugNormals;      // do we paint vertices using its normals data?
	float  gTurnOnFlashLight;  // are we using a flashlight now?
}


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



//////////////////////////////////
// PIXEL SHADER
//////////////////////////////////
float4 PS(VS_OUT pin, uniform bool gDebugNormals) : SV_Target
{
	if (gDebugNormals)
	{
		return float4(pin.normalW, 1.0f);
	}
	else
	{
		float3 toEyeW = normalize(gEyePosW - pin.posW);

		// start with a sum of zero
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// sum the light contribution from each light source (ambient, diffuse, specular)
		float4 A, D, S;
		
		ComputeDirectionalLight(gMaterial, gDirLight,
			pin.normalW,
			toEyeW,
			A, D, S);

		ambient += A;
		diffuse += D;
		spec += S;

		
		ComputePointLight(gMaterial, gPointLight,
			pin.posW,
			pin.normalW,
			toEyeW,
			A, D, S);

		ambient += A;
		diffuse += D;
		spec += S;
		
		if (gTurnOnFlashLight)
		{
			ComputeSpotLight(gMaterial, gSpotLight,
				pin.posW,
				pin.normalW,
				toEyeW,
				A, D, S);

			ambient += A;
			diffuse += D;
			spec += S;
		}

		float4 litColor = ambient + diffuse + spec;

		// common to take alpha from diffuse material
		litColor.a = gMaterial.diffuse.a;

		return litColor;
	}

	
}


///////////////////////////////////////
//  TECHNIQUE
///////////////////////////////////////
technique11 LightTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(false)));
	}
};

technique11 DebugNormalsTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(true)));
	}
};