/////////////////////////////////////////////////////////////////////
// Filename: color.ps
/////////////////////////////////////////////////////////////////////

/////////////////////////////
// TYPEDEFS
/////////////////////////////
struct PS_INPUT
{
	float4 posH   : SV_POSITION;    // homogeneous position of the vertex 
	float3 posW   : POSITION;       // vertex position in world space
	float4 color  : COLOR;
};


/////////////////////////////
// PIXEL SHADER
/////////////////////////////
float4 PS(PS_INPUT pin) : SV_TARGET
{
	return pin.color;
};
