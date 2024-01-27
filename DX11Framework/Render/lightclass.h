////////////////////////////////////////////////////////////////////
// Filename:      lightclass.h
// Description:   The purpose of this class is only to maintain
//                the light souce parameters
//
// Created:       16.04.22
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <DirectXMath.h>

#include "../Engine/macros.h"
#include "../Engine/Log.h"


//////////////////////////////////
// Class name: LightClass
//////////////////////////////////
class LightClass
{
public:
	LightClass();
	~LightClass();


	// setters
	void SetAmbientColor(const DirectX::XMFLOAT4 & newColor);
	void SetAmbientColor(const DirectX::XMFLOAT3 & newColor);
	void SetDiffuseColor(const DirectX::XMFLOAT3 & newColor);

	void SetAmbientColor(const float red, const float green, const float blue);
	void SetDiffuseColor(const float red, const float green, const float blue);
	void SetDirection(const float x, const float y, const float z);
	void SetSpecularColor(const float red, const float green, const float blue);
	void SetSpecularPower(float power);

	void SetPosition(const float x, const float y, const  float z);
	void SetPosition(const DirectX::XMFLOAT3 & newPos);

	void AdjustPosition(const float x, const float y, const float z);
	void AdjustPosition(const DirectX::XMFLOAT3 & adjustment);

	void AdjustPosX(const float x);
	void AdjustPosY(const float y);
	void AdjustPosZ(const float z);

	// getters
	const DirectX::XMFLOAT3 & GetAmbientColor() const;
	const DirectX::XMFLOAT3 & GetDiffuseColor() const;
	const DirectX::XMFLOAT3 & GetDirection() const;
	const DirectX::XMFLOAT3 & GetSpecularColor() const;
	float GetSpecularPower() const;
	const DirectX::XMFLOAT4 & GetPosition() const;

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);


private:  // restrict a copying of this class instance (currently we can't copy light sources)
	LightClass(const LightClass & obj);
	LightClass & operator=(const LightClass & obj);

private:
	DirectX::XMFLOAT4 position_;                // position of the point light
	DirectX::XMFLOAT3 ambientColor_;            // a common colour of the scene
	DirectX::XMFLOAT3 diffuseColor_;            // a light colour of the light source (a main directed colour)
	DirectX::XMFLOAT3 specularColor_;           // the specular colour is the reflected colour of the object's highlights
	DirectX::XMFLOAT3 direction_;               // a direction of the diffuse light
	float             specularPower_ = 0.0f;    // specular intensity
	
};