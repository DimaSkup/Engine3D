cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
	float4 gColor;
};

cbuffer cbPerFrame
{
	float gTime;     // the time passed since the start of the application
};

struct VS_IN
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
};

struct VS_OUT
{
	float4 PosH : SV_POSITION;    // H - means homogeneous
	float4 Color : COLOR;
};


///////////////////////////////////////
//  VERTEX SHADER
///////////////////////////////////////
VS_OUT VS(VS_IN vin)
{
	VS_OUT vout;

	vin.Pos.xy += 0.5f * sin(vin.Pos.x)*sin(3.0f*gTime);
	vin.Pos.z *= 0.6f + 0.4f*sin(2.0f*gTime);

	// transform to homogeneous clip space
	vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);

	// just pass vertex color into the pixel shader
	vout.Color = vin.Color;

	return vout;
}


///////////////////////////////////////
//  PIXEL SHADER
///////////////////////////////////////
float4 PS(VS_OUT pin, uniform bool gUseVertexColor): SV_Target
{
	if (gUseVertexColor)
	{
		return pin.Color;   // use color of the vertex
	}
	else
	{
		return gColor;      // use color from the constant buffer
	}
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

RasterizerState WireframeRS
{
	FillMode = Wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;

	// default values used for any properties we do not set
};


///////////////////////////////////////
//  TECHNIQUE
///////////////////////////////////////
technique11 VertexColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader( vs_5_0, VS() ));
		SetPixelShader(CompileShader( ps_5_0, PS(true) ));

		//SetRasterizerState(SolidRS);
	}
};

technique11 ConstantColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(false)));

		//SetRasterizerState(SolidRS);
	}
};