////////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      LightStore.h
// Description:   The purpose of this class is only to maintain
//                the light sources parameters
//
// Created:       16.04.22
////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <DirectXMath.h>
#include <vector>

#include "../Engine/macros.h"
#include "../Engine/Log.h"

struct LightSourceDiffuseStore
{
	UINT numOfDiffuseLights_ = 0;
	std::vector<UINT> IDs_;
	std::vector<DirectX::XMFLOAT3> ambientColors_;            // a common colour of the scene
	std::vector<DirectX::XMFLOAT3> diffuseColors_;            // a light colour of the light source (a main directed colour)
	std::vector<DirectX::XMFLOAT3> specularColors_;           // the specular colour is the reflected colour of the object's highlights
	std::vector<DirectX::XMVECTOR> directions_;               // a direction of the diffuse light
	std::vector<float>             specularPowers_;           // specular intensity
};

struct LightSourcePointStore
{
	UINT numOfPointLights_ = 0;
	std::vector<UINT> IDs_;
	std::vector<DirectX::XMVECTOR> positions_;                // a position of the point light
	std::vector<DirectX::XMFLOAT3> colors_;                   // a light colour of the light source (a main directed colour)
	std::vector<DirectX::XMVECTOR> positionModificators_;     // contains translation data for each point light
};


//////////////////////////////////
// Class name: LightStore
//////////////////////////////////
class LightStore
{
public:
	LightStore();
	~LightStore();


	// Public creation API
	void CreateDiffuseLight(
		const DirectX::XMFLOAT3 & ambientColor,             // a common colour of the scene
		const DirectX::XMFLOAT3 & diffuseColor,             // a light colour of the light source (a main directed colour)
		const DirectX::XMFLOAT3 & specularColor,            // the specular colour is the reflected colour of the object's highlights
		const DirectX::XMVECTOR & direction,                // a direction of the diffuse light
		const float specularPower);                         // specular intensity)

	void CreatePointLight(
		const DirectX::XMVECTOR & position,
		const DirectX::XMFLOAT3 & diffuseColor,
		const DirectX::XMVECTOR & positionModificator);

	// Public update API
	void UpdateDiffuseLights(const float deltaTime);
	void UpdatePointLights(const float deltaTime);


	// Public modification API for DIFFUSE light sources
	void SetAmbientColorForDiffuseLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newColor);
	void SetDiffuseColorForDiffuseLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newColor);

	void SetDirectionForDiffuseLightByIndex(const UINT index, const DirectX::XMVECTOR & newDirection);
	void SetSpecularColorForDiffuseLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newColor);
	void SetSpecularPowerForDiffuseLightByIndex(const UINT index, const float power);


	// Public modification API for POINT light sources
	void SetPositionForPointLightByIndex(const UINT index, const DirectX::XMVECTOR & newPosition);
	void AdjustPositionForPointLightByIndex(const UINT index, const DirectX::XMVECTOR & adjustPos);
	void SetColorForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newColor);


	// Public query API for DIFFUSE light sources
	const UINT GetNumOfDiffuseLights() const;
	const DirectX::XMFLOAT3 & GetAmbientColorOfDiffuseLightByIndex(const UINT index) const;
	const DirectX::XMFLOAT3 & GetDiffuseColorOfDiffuseLightByIndex(const UINT index) const;
	const DirectX::XMVECTOR & GetDirectionOfDiffuseLightByIndex(const UINT index) const;
	const DirectX::XMFLOAT3 & GetSpecularColorOfDiffuseLightByIndex(const UINT index) const;
	const float GetSpecularPowerOfDiffuseLightByIndex(const UINT index) const;


	// Public query API for POINT light sources
	const UINT GetNumOfPointLights() const;
	const DirectX::XMVECTOR & GetPositionOfPointLightByIndex(const UINT index) const;
	const DirectX::XMFLOAT3 & GetColorOfPointLightByIndex(const UINT index) const;


	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);


private:  // restrict a copying of this class instance 
	LightStore(const LightStore & obj);
	LightStore & operator=(const LightStore & obj);

public:
	LightSourceDiffuseStore diffuseLightsStore_;
	LightSourcePointStore   pointLightsStore_;
};