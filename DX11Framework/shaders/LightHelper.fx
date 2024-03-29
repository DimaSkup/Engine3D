

///////////////////////////////////////
// DATA STRUCTURES
///////////////////////////////////////
struct Material
{
	float4 ambient;
	float4 diffuse;
	float4 specular; // w = specPower
	float4 reflect;
};

struct DirectionalLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float3 direction;
	float pad;
};

struct PointLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;

	float3 position;
	float range;

	float3 att;
	float pad;
};

struct SpotLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;

	float3 position;
	float range;

	float3 direction;
	float spot;

	float3 att;
	float pad;
};

///////////////////////////////////////
// CONSTANT BUFFERS
///////////////////////////////////////
cbuffer cbPerFrame
{
	DirectionalLight gDirLight;
	PointLight       gPointLight;
	SpotLight        gSpotLight;
	float3           gEyePosW;     // eye position in world
};



///////////////////////////////////////
// COMPUTE DIRECTIONAL LIGHT
///////////////////////////////////////
void ComputeDirectionalLight(Material mat, DirectionalLight L,
	float3 normal,
	float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// initialize outputs
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// the light vector aims opposite the direction the light rays travel
	float3 lightVec = -L.direction;

	// add ambient term
	ambient = mat.ambient * L.ambient;

	// add diffuse and specular term, provided the surface is in 
	// the line of site of the light
	float diffuseFactor = dot(lightVec, normal);
}