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
	void SetDiffuseColor(const DirectX::XMFLOAT4 & newColor);

	void SetAmbientColor(float red, float green, float blue, float alpha);
	void SetDiffuseColor(float red, float green, float blue, float alpha);
	void SetDirection(float x, float y, float z);
	void SetSpecularColor(float red, float green, float blue, float alpha);
	void SetSpecularPower(float power);
	void SetPosition(float x, float y, float z);

	// getters
	const DirectX::XMFLOAT4 & GetAmbientColor(void) const;
	const DirectX::XMFLOAT4 & GetDiffuseColor(void) const;
	const DirectX::XMFLOAT3 & GetDirection(void) const;
	const DirectX::XMFLOAT4 & GetSpecularColor(void) const;
	float GetSpecularPower(void) const;
	const DirectX::XMFLOAT4 & GetPosition() const;

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);


private:  // restrict a copying of this class instance (currently we can't copy light sources)
	LightClass(const LightClass & obj);
	LightClass & operator=(const LightClass & obj);

private:
	DirectX::XMFLOAT4 ambientColor_;            // a common colour of the scene
	DirectX::XMFLOAT4 diffuseColor_;            // a light colour of the light source (a main directed colour)
	DirectX::XMFLOAT4 specularColor_;           // the specular colour is the reflected colour of the object's highlights
	DirectX::XMFLOAT3 direction_;               // a direction of the diffuse light
	float             specularPower_ = 0.0f;    // specular intensity
	DirectX::XMFLOAT4 position_;                // position of the point light
};