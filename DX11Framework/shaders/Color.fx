cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
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

	// transform to homogeneous clip space
	vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);

	// just pass vertex color into the pixel shader
	vout.Color = vin.Color;

	return vout;
}


///////////////////////////////////////
//  PIXEL SHADER
///////////////////////////////////////
float4 PS(VS_OUT pin, uniform bool gUseColor): SV_Target
{
	if (gUseColor)
	{
		return pin.Color;
	}
	else
	{
		return float4(0, 0, 0, 1);
	}
}


///////////////////////////////////////
//  RASTERIZER STATES
///////////////////////////////////////
RasterizerState WireframeRS
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;

	// default values used for any properties we do not set
};


///////////////////////////////////////
//  TECHNIQUE
///////////////////////////////////////
technique11 ColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader( vs_5_0, VS() ));
		SetPixelShader(CompileShader( ps_5_0, PS(true) ));

		SetRasterizerState(WireframeRS);
	}
};

technique11 NoColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(false)));
	}
};