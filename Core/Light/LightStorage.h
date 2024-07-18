////////////////////////////////////////////////////////////////////////////////////////////////
// Filename:      LightStorage.h
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
#include "LightHelper.h"      // contains data structures: DirectionalLight, PointLight, SpotLight


//////////////////////////////////
// Class name: LightStorage
//////////////////////////////////
class LightStorage
{
public:
	LightStorage();
	~LightStorage();


	// Public creation API
	void CreateNewDirectionalLight(
		const DirectX::XMFLOAT4 & ambient,                // the amount of ambient light emitted by the light source
		const DirectX::XMFLOAT4 & diffuse,                // the amount of diffuse light emitted by the light source
		const DirectX::XMFLOAT4 & specular,               // the amount of specular light emitted by the light source
		const DirectX::XMFLOAT3 & direction);             // the direction of the light
	
	void CreateNewPointLight(
			const DirectX::XMFLOAT4 & ambient,             // the amount of ambient light emitted by the light source
			const DirectX::XMFLOAT4 & diffuse,             // the amount of diffuse light emitted by the light source
			const DirectX::XMFLOAT4 & specular,            // the amount of specular light emitted by the light source
			const DirectX::XMFLOAT3 & position,            // the position of the light
			const float range,                             // the range of the light
			const DirectX::XMFLOAT3 & attenuation);        // params for controlling how light intensity falls off with distance

	void CreateNewSpotLight(
		const DirectX::XMFLOAT4 & ambient,                 // the amount of ambient light emitted by the light source
		const DirectX::XMFLOAT4 & diffuse,                 // the amount of diffuse light emitted by the light source
		const DirectX::XMFLOAT4 & specular,                // the amount of specular light emitted by the light source
		const DirectX::XMFLOAT3 & position,                // the position of the light
		const float range,                                 // the range of the light
		const DirectX::XMFLOAT3 & direction,               // the direction of the light
		const float spotExponent,                          // the exponent used in the spotlight calculation to control the spotlight cone
		const DirectX::XMFLOAT3 & attenuation);            // params for controlling how light intensity falls off with distance


	// Public update API
	void UpdateDiffuseLights(const float deltaTime);
	void UpdatePointLights(const float deltaTime);


	// Public modification API for DIRECTIONAL light sources
	void SetAmbientForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newAmbient);
	void SetDiffuseForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newDiffuse);
	void SetSpecularForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newSpecular);
	void SetDirectionForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newDirection);

	
	// Public modification API for POINT light sources
	void SetAmbientForPointLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newAmbient);
	void SetDiffuseForPointLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newDiffuse);
	void SetSpecularForPointLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newSpecular);
	
	void SetPositionForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newPosition);
	void AdjustPositionForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & adjustPos);
	void SetRangeForPointLightByIndex(const UINT index, const float newRange);
	void SetAttenuationForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newAtt);

	// Public modification API for SPOTLIGHT sources
	void SetAmbientForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newAmbient);
	void SetDiffuseForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newDiffuse);
	void SetSpecularForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newSpecular);

	void SetPositionForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newPosition);
	void AdjustPositionForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & adjustPos);
	void SetRangeForSpotLightByIndex(const UINT index, const float newRange);

	void SetDirectionForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newDirection);
	void SetSpotExponentForTheSpotLightByIndex(const UINT index, const float newSpotExp);
	void SetAttenuationForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newAtt);




	// Public query API 
	inline const std::vector<DirectionalLight>& GetDirLightsData() const { return dirLights_.data_; }
	inline const std::vector<PointLight>& GetPointLightsData() const { return pointLights_.data_; }
	inline const std::vector<SpotLight>& GetSpotLightsData() const { return spotLights_.data_; }

	const ptrdiff_t GetNumOfPointLights() const;
	const ptrdiff_t GetNumOfDiffuseLights() const;
	const ptrdiff_t GetNumOfSpotLights() const;

#if 0
	const DirectX::XMFLOAT3 & GetAmbientColorOfDiffuseLightByIndex(const UINT index) const;
	const DirectX::XMFLOAT3 & GetDiffuseColorOfDiffuseLightByIndex(const UINT index) const;
	const DirectX::XMVECTOR & GetDirectionOfDiffuseLightByIndex(const UINT index) const;
	const DirectX::XMFLOAT3 & GetSpecularColorOfDiffuseLightByIndex(const UINT index) const;
	const float GetSpecularPowerOfDiffuseLightByIndex(const UINT index) const;

	// Public query API for POINT light sources
	const DirectX::XMVECTOR & GetPositionOfPointLightByIndex(const UINT index) const;
	const DirectX::XMFLOAT3 & GetColorOfPointLightByIndex(const UINT index) const;
#endif

	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* p);


private:  // restrict a copying of this class instance 
	LightStorage(const LightStorage & obj);
	LightStorage & operator=(const LightStorage & obj);

public:
	DirectionalLightsStorage dirLights_;
	PointLightsStorage       pointLights_;
	SpotLightsStorage        spotLights_;
};