
//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture : TEXTURE: register(t0);
SamplerState sampleType : SAMPLER: register(s0);



//////////////////////////////////
// CONSTANT BUFFERS
//////////////////////////////////
cbuffer MatrixBuffer : register(b0)
{
	matrix gWorldMatrix;
	matrix gWorldViewProj;
};

cbuffer cbLightPerFrame : register(b1)
{
	float4 gAmbientColor;	         // a common colour for the scene
	float4 gDiffuseLightColor;       // a main directed colour (this colour and texture pixel colour are blending and make a final texture pixel colour of the model)
	float3 gDiffuseLightDirection;   // a direction of the diffuse colour
	float  gDiffuseLightStrength;    // the power/intensity of the diffuse light
	float  gAmbientLightStrength;    // the power of ambient light
	float  padding_2;
};

cbuffer CameraBuffer : register(b2)
{
	float3 gCameraPos;
	float  padding;
}

cbuffer cbRareChanged : register(b3)
{
	// allow application to change for parameters once per frame.
	// For example, we may only use fog for certain times of day.

	float  gFogEnabled;    // do we use fog effect?
	float  gDebugNormals;  // do we paint vertices using its normals data?
};

cbuffer cbFogData : register(b4)
{
	//float  gFogRange_inv;  // inversed value of the fog range (1 / range)
	float  gFogStart;      // how far from us the fog starts
	float  gFogRange;      // distance from the fog start position where the fog completely hides the surface point
	float4 gFogColor;      // the colour of the fog (usually it's a degree of grey)
}



//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VS_IN
{
	float4 position : POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
};

struct VS_OUT
{
	float4 position  : SV_POSITION;
	float4 positionW : POSITION;
	float2 tex       : TEXCOORD0;
	float3 normal    : NORMAL;
};


//////////////////////////////////
// Vertex Shader
//////////////////////////////////
VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;

	// change the position vector to be 4 units for proper calculations
	vin.position.w = 1.0f;

	// calculate the position against the world, view, and projection matrices
	vout.position = mul(vin.position, gWorldViewProj);

	// store the position of the vertex in the world for later computation in the pixel shader
	vout.positionW = mul(vin.position, gWorldMatrix);

	// store the texture coordinates
	vout.tex = vin.tex;

	// calculate the normal vector against the world matrix and normalize the final value
	vout.normal = normalize(mul(vin.normal, (float3x3)gWorldMatrix));

	return vout;
}



//////////////////////////////////
// Pixel Shader
//////////////////////////////////
float4 PS(VS_OUT pin): SV_TARGET
{
	float4 textureColor;    // a pixel color from the texture by these coordinates
	float3 lightDir;        // an inverted light direction
	float  lightIntensity;  // an amount of the light on this pixel
	float4 finalLight;     
	float4 finalColor;         


	/////////////////////////////////////

	// if we want to use normal value as color of the pixel
	//if (true) 
	if (gDebugNormals)
	{
		return float4(pin.normal, 1.0f);
	}
	
	/////////////////////////////////////
		
	// sample the pixel colour from the texture using the sampler by these texture coordinates
	textureColor = shaderTexture.Sample(sampleType, pin.tex);

	//return float4(textureColor.xyz, 1.0f);

	// set the default output colour to the ambient colour value
	finalLight = gAmbientColor * gAmbientLightStrength;

	// invert the light direction value for proper calculations
	lightDir = -gDiffuseLightDirection;

	// calculate the amount of light on this pixel
	lightIntensity = max(saturate(dot(pin.normal, lightDir)), 0); //* clamp(gDiffuseLightStrength, 0.0f, 1.0f);

	// if the N dot L is greater than zero we add the diffuse colour to the ambient colour
	if (lightIntensity > 0.0f)
	{
		// calculate the final diffuse colour based on the diffuse colour and light intensity
		finalLight += (gDiffuseLightColor * lightIntensity);

		// saturate the ambient and diffuse colour
		finalLight = saturate(finalLight);
	}

	// multiply the final diffuse light and texture colour to get the final pixel colour
	finalColor = finalLight;
	// finalColor = finalLight * textureColor;

	/////////////////////////   FOG   ///////////////////////////

	if (gFogEnabled)
	//if (false)
	{
		// the toEye vector is used in lighting
		float3 toEye = gCameraPos - pin.positionW.xyz;
		float distToEye = length(toEye);
	
		float fogLerp = saturate((distToEye - gFogStart) / gFogRange);

		// blend the fog color and the lit color
		finalColor = lerp(finalColor, gFogColor, fogLerp);
	}


	/////////////////////////////////////////////////////////////

	return finalColor;
}



///////////////////////////////////////
//  RASTERIZER STATES
///////////////////////////////////////
RasterizerState SolidRS
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;

	// default values used for any properties we do not set
};



///////////////////////////////////////
//  TECHNIQUE
///////////////////////////////////////
technique11 DiffuseLightTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));

		SetRasterizerState(SolidRS);
	}
};