////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: LightStore.cpp
////////////////////////////////////////////////////////////////////////////////////////////////
#include "LightStore.h"

LightStore::LightStore(void)
{
}

LightStore::~LightStore(void)
{
	Log::Debug(LOG_MACRO);
}



////////////////////////////////////////////////////////////////////////////////////////////////
//                                PUBLIC CREATION API
////////////////////////////////////////////////////////////////////////////////////////////////

void LightStore::CreateDiffuseLight(
	const DirectX::XMFLOAT3 & ambientColor,             // a common colour of the scene
	const DirectX::XMFLOAT3 & diffuseColor,             // a light colour of the light source (a main directed colour)
	const DirectX::XMFLOAT3 & specularColor,            // the specular colour is the reflected colour of the object's highlights
	const DirectX::XMVECTOR & direction,                // a direction of the diffuse light
	const float specularPower)
{
	// create new new diffuse light source

	LightSourceDiffuseStore & store = diffuseLightsStore_;

	const UINT idx = store.IDs_.size();
	store.IDs_.push_back(idx);

	store.ambientColors_.push_back(ambientColor);
	store.diffuseColors_.push_back(diffuseColor);
	store.specularColors_.push_back(specularColor);
	store.directions_.push_back(direction);
	store.specularPowers_.push_back(specularPower);

	++store.numOfDiffuseLights_;
}

///////////////////////////////////////////////////////////

void LightStore::CreatePointLight(
	const DirectX::XMFLOAT3 & position,
	const DirectX::XMFLOAT3 & color)
{
	// create new new point light source

	LightSourcePointStore & store = pointLightsStore_;

	const UINT idx = store.IDs_.size();
	store.IDs_.push_back(idx);

	store.positions_.push_back(position);
	store.colors_.push_back(color);

	++store.numOfPointLights_;
}




////////////////////////////////////////////////////////////////////////////////////////////////
//                   PUBLIC MODIFICATION API FOR DIFFUSE LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

void LightStore::SetAmbientColorForDiffuseLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newColor)
{
	diffuseLightsStore_.ambientColors_[index] = newColor;
}

void LightStore::SetDiffuseColorForDiffuseLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newColor)
{
	diffuseLightsStore_.diffuseColors_[index] = newColor;
}

void LightStore::SetDirectionForDiffuseLightByIndex(const UINT index, const DirectX::XMVECTOR & newDirection)
{
	diffuseLightsStore_.directions_[index] = newDirection;
}

void LightStore::SetSpecularColorForDiffuseLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newColor)
{
	diffuseLightsStore_.specularColors_[index] = newColor;
}

void LightStore::SetSpecularPowerForDiffuseLightByIndex(const UINT index, const float power)
{
	diffuseLightsStore_.specularPowers_[index] = power;
}





////////////////////////////////////////////////////////////////////////////////////////////////
//                   PUBLIC MODIFICATION API FOR POINT LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

void LightStore::SetPositionForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newPosition)
{
	pointLightsStore_.positions_[index] = newPosition;
}

void LightStore::AdjustPositionForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & adjustPos)
{
	pointLightsStore_.positions_[index].x += adjustPos.x;
	pointLightsStore_.positions_[index].y += adjustPos.y;
	pointLightsStore_.positions_[index].z += adjustPos.z;
}

void LightStore::SetColorForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newColor)
{
	pointLightsStore_.colors_[index] = newColor;
}






////////////////////////////////////////////////////////////////////////////////////////////////
//                       PUBLIC QUERY API FOR DIFFUSE LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

const UINT LightStore::GetNumOfDiffuseLights() const
{
	return diffuseLightsStore_.numOfDiffuseLights_;
}

const DirectX::XMFLOAT3 & LightStore::GetAmbientColorOfDiffuseLightByIndex(const UINT index) const
{
	return diffuseLightsStore_.ambientColors_[index];
}

const DirectX::XMFLOAT3 & LightStore::GetDiffuseColorOfDiffuseLightByIndex(const UINT index) const
{
	return diffuseLightsStore_.diffuseColors_[index];
}

const DirectX::XMVECTOR & LightStore::GetDirectionOfDiffuseLightByIndex(const UINT index) const
{
	return diffuseLightsStore_.directions_[index];
}

const DirectX::XMFLOAT3 & LightStore::GetSpecularColorOfDiffuseLightByIndex(const UINT index) const
{
	return diffuseLightsStore_.specularColors_[index];
}

const float LightStore::GetSpecularPowerOfDiffuseLightByIndex(const UINT index) const
{
	return diffuseLightsStore_.specularPowers_[index];
}






////////////////////////////////////////////////////////////////////////////////////////////////
//                       PUBLIC QUERY API FOR POINT LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////
const UINT LightStore::GetNumOfPointLights() const
{
	return pointLightsStore_.numOfPointLights_;
}

const DirectX::XMFLOAT3 & LightStore::GetPositionOfPointLightByIndex(const UINT index) const
{
	return pointLightsStore_.positions_[index];
}

const DirectX::XMFLOAT3 & LightStore::GetColorOfPointLightByIndex(const UINT index) const
{
	return pointLightsStore_.colors_[index];
}




////////////////////////////////////////////////////////////////////////////////////////////////

// memory allocation
void* LightStore::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);
	COM_ERROR_IF_FALSE(ptr, "can't allocate the memory for object");

	return ptr;
}

void LightStore::operator delete(void* p)
{
	_aligned_free(p);
}