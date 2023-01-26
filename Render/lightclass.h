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
	LightClass(void);
	LightClass(const LightClass&);
	~LightClass(void);


	// setters
	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	// getters
	DirectX::XMFLOAT4 GetAmbientColor(void);
	DirectX::XMFLOAT4 GetDiffuseColor(void);
	DirectX::XMFLOAT3 GetDirection(void);
	DirectX::XMFLOAT4 GetSpecularColor(void);
	float GetSpecularPower(void);

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);

private:
	DirectX::XMFLOAT4 ambientColor_;      // a common colour of the scene
	DirectX::XMFLOAT4 diffuseColor_;      // a light colour of the light source (a main directed colour)
	DirectX::XMFLOAT3 direction_;         // a direction of the diffuse light
	DirectX::XMFLOAT4 specularColor_;     // the specular colour is the reflected colour of the object's highlights
	float             specularPower_;     // specular intensity
};
