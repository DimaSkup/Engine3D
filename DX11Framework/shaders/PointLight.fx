///////////////////////////////////////////////////////////////////////////////////////////
// Filename:      pointLight.fx
// Description:   an effect file which combines: point lighting, diffuse lighting,
//                and texturing of models
//
// Created:       14.03.24
///////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
// DEFINES
//////////////////////////////////
#define NUM_LIGHTS 25    // the number of point light sources


//////////////////////////////////
// GLOBALS
//////////////////////////////////
Texture2D shaderTexture : register(t0);
SamplerState sampleType : register(s0);


//////////////////////////////////
// CONSTANT BUFFERS
//////////////////////////////////

cbuffer cbMatrixBuffer : register(b0)
{
	matrix gWorldMatrix;
	matrix gWorldViewProj;
};

// an array with positions of point light sources
cbuffer cbLightPositionBuffer : register(b1)
{
	float4 gPointLightPos[NUM_LIGHTS];
};

// an array for the colours of the point lights
cbuffer cbPointLightColorBuffer : register(b2)
{
	float4 gPointLightColor[NUM_LIGHTS];
};

cbuffer cbDiffuseLightBuffer : register(b3)
{
	float3 gAmbientColor;	     // a common colour for the scene
	float  gAmbientLightStrength; // the power of ambient light
	float4 gDiffuseLightColor;         // a main directed colour (this colour and texture pixel colour are blending and make a final texture pixel colour of the model)
	float4 gDiffuseLightDirection;       // a direction of the diffuse colour
};


//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
struct VS_IN
{
	float4 pos      : POSITION;
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
};

struct VS_OUT
{
	float4 posH     : SV_POSITION;           // H - means homogeneous
	float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 toLight[NUM_LIGHTS] : TEXCOORD1;  // vectors from world vertex to point light source
};



//////////////////////////////////
// VERTEX SHADER
//////////////////////////////////
VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;
	float4 posW;   // position of vertex in the world
	int i = 0;

	// change the position vector to be 4 units for proper matrix calculations
	vin.pos.w = 1.0f;

	// calculate the position of the vertex agains the world, view, and projection matrices
	vout.posH = mul(vin.pos, gWorldViewProj);

	// calculate the position of the vertex in the world
	posW = mul(vin.pos, gWorldMatrix);

	// store the texture coords for the pixel shader
	vout.tex = vin.tex;

	// calculate the normal vector against the world matrix only and normalize it
	vout.normal = normalize(mul(vin.normal, (float3x3)gWorldMatrix));

	// the positions of the light sources in the world in relation to the vertex
	// must be calculated, normalized, and then sent into the pixel shader
	[unroll] for (i = 0; i < NUM_LIGHTS; i++)
	{
		// determine the light position vector based on the position of the light and 
		// the position of the vertex in the world;
		vout.toLight[i] = gPointLightPos[i].xyz - posW.xyz;
	}

	return vout;

} // end VS


///////////////////////////////////////
//  PIXEL SHADER
///////////////////////////////////////
float4 PS(VS_OUT pin, uniform bool gDebugNormals) : SV_Target
{
	// if we want to use normal value as color of the pixel
	if (gDebugNormals)
	{
		return float4(pin.normal, 1.0f);
	}
	else
	{
		float4 textureColor;
		float4 colorSum = float4(0.0f, 0.0f, 0.0, 1.0f);  // final light color for this pixel                                   
		int i;
		float4 color;

		// sample the texture pixel at this location
		textureColor = shaderTexture.Sample(sampleType, pin.tex);

		// the light intensity of each of the point lights is calculated using the position
		// of the light and the normal vector. The amount of colour contributed by each
		// point light is calculated from the intensity of the point light and the light colour.
		[unroll] for (i = 0; i < NUM_LIGHTS; i++)
		{
			// Phong diffuse	
			float NDotL = saturate(dot(normalize(pin.toLight[i]), pin.normal));

			// attenuation
			float DistToLightNorm = 1.0f - saturate(length(pin.toLight[i]) * 0.05f);

			// add all of the light colours up
			// sum += light_color * light_dir_dot_plane * attenuation (dist_to_light ^ 2);
			colorSum += gPointLightColor[i] * NDotL * DistToLightNorm * DistToLightNorm;
		}



		// set the default output colour to the ambient colour value
		float3 finalLight = float3(0.2f, 0.2f, 0.2f);

		// invert the light direction value for proper calculations
		float4 lightDir = -gDiffuseLightDirection;

		// calculate the amount of light on this pixel
		float lightIntensity = saturate(dot(pin.normal, lightDir.xyz));

		// if the N dot L is greater than zero we add the diffuse colour to the ambient colour
		if (lightIntensity > 0.0f)
		{
			// calculate the final diffuse colour based on the diffuse colour and light intensity
			finalLight += (float3(1.0f, 1.0f, 1.0f) * lightIntensity);

			// saturate the ambient and diffuse colour
			finalLight = saturate(finalLight);
		}

		// multiply the texture pixel by the combination of all
		// four light colours to get the final result
		float4 finalColorLight = saturate((float4(finalLight, 1.0f)) + saturate(colorSum));

		return finalColorLight;
		// return the final colour of the pixel
		return  finalColorLight * textureColor;
	}

} // end PS


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
technique11 PointLightingTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(false)));

		SetRasterizerState(SolidRS);
	}
};

technique11 DebugNormalsTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true))); // use normal value of vertex as color of the pixel

		SetRasterizerState(SolidRS);
	}
};