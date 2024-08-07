// *********************************************************************************
// Filename:     LightHelper.h
// Description:  contains different data structures for lighting computations
//
// Created:      29.03.24
// *********************************************************************************
#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include <assimp/types.h>


struct Material
{
	DirectX::XMFLOAT4 ambient_;
	DirectX::XMFLOAT4 diffuse_;
	DirectX::XMFLOAT4 specular_;   // w = specPower (specular power)
	DirectX::XMFLOAT4 reflect_;


	Material() :
		ambient_ { 1,1,1,1 },
		diffuse_ { 1,1,1,1 },
		specular_{ 0,0,0,0 },
		reflect_ { 0.5f, 0.5f, 0.5f, 1 } {}

	Material(
		const DirectX::XMFLOAT4& ambient,
		const DirectX::XMFLOAT4& diffuse,
		const DirectX::XMFLOAT4& specular,
		const DirectX::XMFLOAT4& reflect) :
		ambient_(ambient),
		diffuse_(diffuse),
		specular_(specular),
		reflect_(reflect) {}

	// -----------------------------------------------------------------------
	inline void SetMatColorWithAiColor3D(DirectX::XMFLOAT4& color, const aiColor3D& newMatColor)
	{
		color.x = newMatColor.r;
		color.y = newMatColor.g;
		color.z = newMatColor.b;
		color.w = 1.0f;
	}
	// -----------------------------------------------------------------------
	void SetAmbient(const aiColor3D& newAmbient)
	{
		SetMatColorWithAiColor3D(ambient_, newAmbient);
	}
	// -----------------------------------------------------------------------
	void SetDiffuse(const aiColor3D& newDiffuse)
	{
		SetMatColorWithAiColor3D(diffuse_, newDiffuse);
	}
	// -----------------------------------------------------------------------
	void SetSpecular(const aiColor3D& newSpecular)
	{
		SetMatColorWithAiColor3D(specular_, newSpecular);
	}
	// -----------------------------------------------------------------------
	void SetSpecularPower(const float power)
	{
		specular_.w = power;
	}
};


// *********************************************************************************
//   STRUCTURES TO REPRESENT THE THREE TYPES OF LIGHTS (DIRECTIONAL, POINT, SPOT)
// *********************************************************************************

// Common description for members:
// 1. ambient:     the amount of ambient light emitted by the light source
// 2. diffuse:     the amount of diffuse light emitted by the light source
// 3. specular:    the amount of specular light emitted by the light source
// 4. direction:   the direction of the light
// 5. position:    the position of the light
// 6. range:       the range of the light. A point whose distance from the light source is 
//                 greater than the range is not lit.
// 7. attenuation: stores the three attenuation constant in the format (a0, a1, a2) that
//                 control how light intensity falls off with distance
// 8. spot:        the exponent used in the spotlight calculation to control the spotlight cone

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }
	

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	DirectX::XMFLOAT3 direction;
	float pad;                    // pad the last float so we can array of light if we wanted
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	// packed into 4D vector: (position, range)
	DirectX::XMFLOAT3 position;
	float range;

	// packed into 4D vector: (A0, A1, A2, pad)
	DirectX::XMFLOAT3 att;
	float pad;                    // pad the last float so we can array of light if we wanted
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	// packed into 4D vector: (position, range)
	DirectX::XMFLOAT3 position;
	float range;

	// packed into 4D vector: (direction, spot)
	DirectX::XMFLOAT3 direction;
	float spot;

	// packed into 4D vector: (att, pad)
	DirectX::XMFLOAT3 att;
	float pad;                    // pad the last float so we can array of light if we wanted
};




// *********************************************************************************
//   STRUCTURES TO REPRESENT STORAGES FOR THE TYPES OF LIGHTS
// *********************************************************************************


struct DirectionalLightsStorage
{
	std::vector<UINT> IDs_;
	std::vector<DirectionalLight> data_;
};

struct PointLightsStorage
{
	std::vector<UINT> IDs_;
	std::vector<PointLight> data_;
};

struct SpotLightsStorage
{
	std::vector<UINT> IDs_;
	std::vector<SpotLight> data_;
};
