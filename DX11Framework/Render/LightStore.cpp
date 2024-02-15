////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: LightStore.cpp
////////////////////////////////////////////////////////////////////////////////////////////////
#include "LightStore.h"
#include "LightStoreUpdateHelpers.h"

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
	// create a new diffuse light source

	LightSourceDiffuseStore & store = diffuseLightsStore_;

	const UINT idx = (UINT)store.IDs_.size();
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
	const DirectX::XMVECTOR & position,
	const DirectX::XMFLOAT3 & color,
	const DirectX::XMVECTOR & positionModificator)
{
	// create a new point light source

	LightSourcePointStore & store = pointLightsStore_;

	const UINT idx = (UINT)store.IDs_.size();
	store.IDs_.push_back(idx);

	store.positions_.push_back(position);
	store.colors_.push_back(color);
	store.positionModificators_.push_back(positionModificator);

	++store.numOfPointLights_;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//                          PUBLIC UPDATE API FOR LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

void LightStore::UpdateDiffuseLights(const float deltaTime)
{
	// TODO
}

void LightStore::UpdatePointLights(const float deltaTime)
{
	const UINT numOfPointLights = pointLightsStore_.numOfPointLights_;
	LightSourcePointStore & store = pointLightsStore_;

	std::vector<UINT> lightsToUpdate;   // an array of ids to point lights which will be updated
	std::vector<DirectX::XMVECTOR> posToUpdate;
	std::vector<DirectX::XMVECTOR> posModificators;
	std::vector<DirectX::XMVECTOR> newPositionsData;

	SelectPointLightsToUpdate(numOfPointLights, store.IDs_, lightsToUpdate);
	PreparePointLightsPositionsToUpdate(lightsToUpdate, store.positions_, posToUpdate);
	PreparePointLightsPositionModificatorsToUpdate(lightsToUpdate, store.positionModificators_, posModificators);
	ComputePointLightsPositionsToUpdate(posToUpdate, posModificators, newPositionsData);


	// apply new positions data
	UINT data_idx = 0;

	for (const UINT light_idx : lightsToUpdate)
	{
		store.positions_[light_idx] = newPositionsData[data_idx];
		++data_idx;
	}
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

void LightStore::SetPositionForPointLightByIndex(const UINT index, const DirectX::XMVECTOR & newPosition)
{
	pointLightsStore_.positions_[index] = newPosition;
}

void LightStore::AdjustPositionForPointLightByIndex(const UINT index, const DirectX::XMVECTOR & adjustPos)
{
	pointLightsStore_.positions_[index] = DirectX::XMVectorAdd(pointLightsStore_.positions_[index], adjustPos);
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

const DirectX::XMVECTOR & LightStore::GetPositionOfPointLightByIndex(const UINT index) const
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