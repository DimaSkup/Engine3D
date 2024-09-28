#include "LightHelper.hlsli"


//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D    gTextures[2] : register(t0);
SamplerState gSampleType   : register(s0);


///////////////////////////////////////
// CONSTANT BUFFERS
///////////////////////////////////////

cbuffer cbPerFrame    : register(b0)
{
	DirectionalLight  gDirLights[3];
	PointLight        gPointLights[25];
	SpotLight         gSpotLights;
	float3            gEyePosW;             // eye position in world space
};

cbuffer cbRareChanged : register(b1)
{
	// some flags for controlling the rendering process and
	// params which are changed very rarely

	float  gDebugMode;           // if we render some stuff in the debug purpose
	float  gDebugNormals;        // do we paint vertices using its normals data?
	float  gDebugTangents;
	float  gDebugBinormals;

	float3 gFogColor;            // what is the color of fog?
	float  gFogEnabled;          // do we use fog effect?
	float  gFogStart;            // how far from camera the fog starts?
	float  gFogRange;            // how far from camera the object is fully fogged?

	float  gTurnOnFlashLight;    // are we using a flashlight now?
	float  gNumOfDirLights;
	float  gAlphaClipping;
}

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct PS_IN
{
	float4x4 material  : MATERIAL;
	float4   posH      : SV_POSITION;  // homogeneous position
	float3   posW      : POSITION;     // position in world
	float3   normalW   : NORMAL;       // normal in world
	float3   tangentW  : TANGENT;      // tangent in world
	float3   binormalW : BINORMAL;     // binormal in world
	float2   tex       : TEXCOORD;
};



//////////////////////////////////
// PIXEL SHADERS
//////////////////////////////////

float4 PS(PS_IN pin) : SV_Target
{
	// DEFAULT PIXEL SHADER
	
	// Sample the pixel color from the texture using the sampler
	// at this texture coordinate location
	float4 textureColor = gTextures[0].Sample(gSampleType, pin.tex);
	float4 specularColor = gTextures[1].Sample(gSampleType, pin.tex);
	//float4 bumpMapColor = gTextures[5].Sample(gSampleType, pin.tex);
	//float4 lightMapColor = gTextures[10].Sample(gSampleType, pin.tex);

	// ------------------  ALPHA CLIPPING   --------------------

	if (gAlphaClipping)
	{
		clip(textureColor.a - 0.1f);
	}

	// --------------------  LIGHT   --------------------

	// a vector in the world space from vertex to eye position 
	float3 toEyeW = normalize(gEyePosW - pin.posW);

	// start with a sum of zero
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// sum the light contribution from each light source (ambient, diffuse, specular)
	float4 A, D, S;

	// compute ambient, diffuse, and specular for this pixel
	//ComputeLighting((Material)pin.material, A, D, S);

	// sum the light contribution from each light source
	for (int i = 0; i < gNumOfDirLights; ++i)
	{
		ComputeDirectionalLight(
			(Material)pin.material,
			gDirLights[i],
			pin.normalW,
			toEyeW,
			specularColor.x,
			A, D, S);

		ambient += A;
		diffuse += D;
		spec += S;
	}


	
	for (i = 0; i < 3; ++i)
	{
		ComputePointLight(
			(Material)pin.material,
			gPointLights[i],
			pin.posW,
			pin.normalW,
			toEyeW,
			A, D, S);

		ambient += A;
		diffuse += D;
		spec += S;
	}
	
	
	if (gTurnOnFlashLight)
	{
		ComputeSpotLight(
			(Material)pin.material, 
			gSpotLights,
			pin.posW,
			pin.normalW,
			toEyeW,
			A, D, S);

		ambient += A;
		diffuse += D;
		spec += S;
	}
	
	// modulate with late add
	float4 litColor = textureColor * (ambient + diffuse + spec);


	// ---------------------  FOG  ----------------------

	if (gFogEnabled)
	{
		float distToEye = length(gEyePosW - pin.posW);
		float fogLerp = saturate((distToEye - gFogStart) / gFogRange);

		// blend the fog color and the lit color
		litColor = lerp(litColor, float4(gFogColor, 1.0f), fogLerp);
	}


	// common to take alpha from diffuse material and texture
	litColor.a = ((Material)pin.material).diffuse.a * textureColor.a;

	return litColor;
	
}



// ***********************************************************************************
// 
//                         PIXEL SHADERS FOR DEBUGGING
// 
// ***********************************************************************************

float4 PS_DebugNormals(PS_IN pin) : SV_Target
{
	// visualize normals as colors
	return float4(pin.normalW, 1.0f);
}

float4 PS_DebugTangents(PS_IN pin) : SV_Target
{
	// visualize tangents as colors
	return float4(pin.tangentW, 1.0f);
}

float4 PS_DebugBinormals(PS_IN pin) : SV_Target
{
	// visualize binormals as colors
	return float4(pin.binormalW, 1.0f);
}