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

cbuffer cbPerFrame : register(b1)
{
	DirectionalLight  gDirLights;
	PointLight        gPointLights;
	SpotLight         gSpotLights;
	float3            gEyePosW;             // eye position in world space
};

cbuffer cbRareChanged : register(b2)
{
	// some flags for controlling the rendering process

	float             gDebugNormals;        // do we paint vertices using its normals data?
	float             gFogEnabled;          // do we use fog effect?
	float             gTurnOnFlashLight;    // are we using a flashlight now?
	//float             gFogStart;
	//float4            gFogColor;
}

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_IN
{
	float4 posH    : SV_POSITION;  // homogeneous position
	float3 posW    : POSITION;     // position in world
	float3 normalW : NORMAL;       // normal in world
};



//////////////////////////////////
// PIXEL SHADER
//////////////////////////////////
float4 PS(PS_IN pin, uniform bool gDebugNormals) : SV_Target
{
	if (gDebugNormals)
	{
		return float4(pin.normalW, 1.0f);
	}
	else
	{
		// a vector in the world space from vertex to eye position 
		float3 toEyeW = normalize(gEyePosW - pin.posW);

		// start with a sum of zero
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// sum the light contribution from each light source (ambient, diffuse, specular)
		float4 A, D, S;

		ComputeDirectionalLight(gMaterial, gDirLights,
			pin.normalW,
			toEyeW,
			A, D, S);

		ambient += A;
		diffuse += D;
		spec += S;


		ComputePointLight(gMaterial, gPointLights,
			pin.posW,
			pin.normalW,
			toEyeW,
			A, D, S);

		ambient += A;
		diffuse += D;
		spec += S;

		if (gTurnOnFlashLight)
		{
			ComputeSpotLight(gMaterial, gSpotLights,
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
/*
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
*/