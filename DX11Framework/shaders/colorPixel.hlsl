/*
float4 main(): SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
*/

/////////////////////////////////////////////////////////////////////
// Filename: color.ps
/////////////////////////////////////////////////////////////////////

/////////////////////////////
// TYPEDEFS
/////////////////////////////
struct PS_INPUT
{
	float4 position	: SV_POSITION;
	float4 color	: COLOR;
};



/////////////////////////////
// PIXEL SHADER
/////////////////////////////
float4 main(PS_INPUT input) : SV_TARGET
{
	// PAY ATTENTION that in the HLSL shader in some cases we use only alpha value not the whole color;
	// instead of it we use own color of the vertex;
	return input.color;
};
