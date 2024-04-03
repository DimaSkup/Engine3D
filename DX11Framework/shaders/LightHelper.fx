

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
// COMPUTE DIRECTIONAL LIGHT
///////////////////////////////////////
void ComputeDirectionalLight(Material mat, DirectionalLight L,
	float3 normal,
	float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// this HLSL function outputs the lit color of a point given a material, directional 
	// light source, surface normal, and the unit vector from the surface being lit to the eye

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

	// flatten to avoit dynamic branching
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFactor * mat.diffuse * L.diffuse;
		spec = specFactor * mat.specular * L.specular;
	}
}


///////////////////////////////////////
// COMPUTE POINT LIGHT
///////////////////////////////////////
void ComputePointLight(Material mat, PointLight L,
	float3 pos,
	float3 normal,
	float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// this HLSL function outputs the lit color of a point given a material, point light
	// source, surface position, surface normal, and the unit vector from the surface
	// point being lit to the eye

	// initialize output
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// the vector from the surface to the light
	float3 lightVec = L.position - pos;

	// the distance from surface to light
	float d = length(lightVec);

	// range test
	if (d > L.range)
		return;

	// normalize the light vector
	lightVec /= d;

	// ambient term
	ambient = mat.ambient * L.ambient;

	// add diffuse and specular term, provided the surface is in
	// the line of site of the light
	float diffuseFactor = dot(lightVec, normal);

	// flatten to avoid dynamic branching
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFactor * mat.diffuse * L.diffuse;
		spec = specFactor * mat.specular * L.specular;
	}

	// attenuate
	float att = 1.0f / dot(L.att, float3(1.0f, d, pow(d, 2)));

	diffuse *= att;
	ambient *= att;
	spec *= att;
}


///////////////////////////////////////
// COMPUTE SPOTLIGHT
///////////////////////////////////////
void ComputeSpotLight(Material mat, SpotLight L,
	float3 pos,
	float3 normal,
	float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// this HLSL function outputs the lit color of a point given a material, spotlight
	// source, surface position, surface normal, and the unit vector from the surface
	// point being lit to the eye

	// initialize outputs
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	/*
	
	// the vector from the surface to the light
	float3 lightVec = L.position - pos;

	// the distance from sufrace to light
	float d = length(lightVec);

	// range test
	if (d > L.range)
		return;

	// normalize the light vector
	lightVec /= d;
	*/
	
	// the vector from the surface to the light
	float3 lightVec = L.position - pos;

	float distSqr = dot(lightVec, lightVec);
	
	// range test (just compare squares of the distance and range)
	if (distSqr > pow(L.range, 2))
		return;

	// 1.0f / sqrt(distSqrt)
	float distInv = rsqrt(distSqr);

	// normalize the light vector
	lightVec = lightVec * distInv;
	

	// ambient term
	ambient = mat.ambient * L.ambient;

	// add diffuse and specular term, provided the surface is in
	// the line of site of the light
	float diffuseFactor = dot(lightVec, normal);

	// flatten to avoid dynamic branching
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.specular.w);

		diffuse = diffuseFactor * mat.diffuse * L.diffuse;
		spec = specFactor * mat.specular * L.specular;
	}

	// scale by spotlight factor and attenuate
	float spot = pow(max(dot(-lightVec, L.direction), 0.0f), L.spot);

	float att = spot / (L.att.x + dot(L.att.yz, float2(1.0f/distInv, distSqr)));
	//float att = spot / dot(L.att, float3(1.0f, d, d*d));
	
	ambient *= att;
	diffuse *= att;
	spec *= att;
}