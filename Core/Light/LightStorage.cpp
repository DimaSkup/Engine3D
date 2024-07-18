////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: LightStorage.cpp
////////////////////////////////////////////////////////////////////////////////////////////////
#include "LightStorage.h"
#include "LightStoreUpdateHelpers.h"

using namespace DirectX;


LightStorage::LightStorage()
{
}

LightStorage::~LightStorage()
{
	Log::Debug(LOG_MACRO);
}


////////////////////////////////////////////////////////////////////////////////////////////////
//                                PUBLIC CREATION API
////////////////////////////////////////////////////////////////////////////////////////////////

void LightStorage::CreateNewDirectionalLight(
	const XMFLOAT4 & ambient,                // the amount of ambient light emitted by the light source
	const XMFLOAT4 & diffuse,                // the amount of diffuse light emitted by the light source
	const XMFLOAT4 & specular,               // the amount of specular light emitted by the light source
	const XMFLOAT3 & direction)           // the direction of the light
{
	// create a new directional light source

	DirectionalLightsStorage & storage = dirLights_;

	// generate an index (ID) for new light source
	const UINT idx = (UINT)storage.IDs_.size();
	storage.IDs_.push_back(idx);

	// create an instance of directional light source
	DirectionalLight dirLight;

	// setup the directional light
	dirLight.ambient = ambient;
	dirLight.diffuse = diffuse;
	dirLight.specular = specular;
	dirLight.direction = direction;

	// store this directional light source
	storage.data_.push_back(dirLight);
}

///////////////////////////////////////////////////////////

void LightStorage::CreateNewPointLight(
	const XMFLOAT4 & ambient,                // the amount of ambient light emitted by the light source
	const XMFLOAT4 & diffuse,                // the amount of diffuse light emitted by the light source
	const XMFLOAT4 & specular,               // the amount of specular light emitted by the light source
	const XMFLOAT3 & position,               // the position of the light
	const float range,                                // the range of the light
	const XMFLOAT3 & attenuation)         // params for controlling how light intensity falls off with distance
{
	// create a new point light source

	PointLightsStorage & storage = pointLights_;

	// generate an index (ID) for new light source
	const UINT idx = (UINT)storage.IDs_.size();
	storage.IDs_.push_back(idx);

	// create and setup an instance of the point light source
	PointLight light;

	light.ambient = ambient;
	light.diffuse = diffuse;
	light.specular = specular;
	light.position = position;
	light.range = range;
	light.att = attenuation;

	// store this point light source
	storage.data_.push_back(light);
}

///////////////////////////////////////////////////////////

void LightStorage::CreateNewSpotLight(
	const XMFLOAT4 & ambient,                 // the amount of ambient light emitted by the light source
	const XMFLOAT4 & diffuse,                 // the amount of diffuse light emitted by the light source
	const XMFLOAT4 & specular,                // the amount of specular light emitted by the light source
	const XMFLOAT3 & position,                // the position of the light
	const float range,                                 // the range of the light
	const XMFLOAT3 & direction,               // the direction of the light
	const float spotExponent,                          // the exponent used in the spotlight calculation to control the spotlight cone
	const XMFLOAT3 & att)                     // attenuation: params for controlling how light intensity falls off with distance
{
	// create a new spotlight source

	SpotLightsStorage & storage = spotLights_;

	// generate an index (ID) for new light source
	const UINT idx = (UINT)storage.IDs_.size();
	storage.IDs_.push_back(idx);

	// create and setup an instance of the spotlight source
	SpotLight light;

	light.ambient = ambient;
	light.diffuse = diffuse;
	light.specular = specular;
	light.position = position;
	light.range = range;
	light.direction = direction;
	light.spot = spotExponent;

	// setup attenuation params
	light.att.x = (att.x) ? (1.0f / att.x) : 0.0f;
	light.att.y = (att.y) ? (1.0f / att.y) : 0.0f;
	light.att.z = (att.z) ? (1.0f / att.z) : 0.0f;

	// store this point light source
	storage.data_.push_back(light);
}




////////////////////////////////////////////////////////////////////////////////////////////////
//                          PUBLIC UPDATE API FOR LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

void LightStorage::UpdateDiffuseLights(const float deltaTime)
{
	// TODO
}

void LightStorage::UpdatePointLights(const float deltaTime)
{
#if 0
	const UINT numOfPointLights = pointLights_.numOfPointLights_;
	PointLightsStorage & store = pointLights_;

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
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////
//                 PUBLIC MODIFICATION API FOR DIRECTIONAL LIGHT SOURCES
//////////////////////////////////////////////////////////////////////////////////////////////

void LightStorage::SetAmbientForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newAmbient)
{
	// set amount of ambient light emitted by the light source
	dirLights_.data_[index].ambient = newAmbient;
}

void LightStorage::SetDiffuseForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newDiffuse)
{
	// set amount of diffuse light emitted by the light source
	dirLights_.data_[index].diffuse = newDiffuse;
}

void LightStorage::SetSpecularForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newSpecular)
{
	// set amount of specular light emitted by the light source
	dirLights_.data_[index].specular = newSpecular;
}

void LightStorage::SetDirectionForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newDirection)
{
	// set direction of the light
	dirLights_.data_[index].direction = newDirection;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//                   PUBLIC MODIFICATION API FOR POINT LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

void LightStorage::SetAmbientForPointLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newAmbient)
{
	// set amount of ambient light emitted by the light source
	dirLights_.data_[index].ambient = newAmbient;
}

void LightStorage::SetDiffuseForPointLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newDiffuse)
{
	// set amount of diffuse light emitted by the light source
	dirLights_.data_[index].diffuse = newDiffuse;
}

void LightStorage::SetSpecularForPointLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newSpecular)
{
	// set amount of specular light emitted by the light source
	dirLights_.data_[index].specular = newSpecular;
}

void LightStorage::SetPositionForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newPosition)
{
	// set position of the light source
	pointLights_.data_[index].position = newPosition;
}

void LightStorage::AdjustPositionForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & adjustPos)
{

	// adjust position of the light source by some values (adjustPos)
	DirectX::XMFLOAT3 & pos = pointLights_.data_[index].position;

	pos.x += adjustPos.x;
	pos.y += adjustPos.y;
	pos.z += adjustPos.z;
}

void LightStorage::SetRangeForPointLightByIndex(const UINT index, const float newRange)
{
	// set range of the light. A point whose distance from the light source is greater 
	// than the range is not lit
	pointLights_.data_[index].range = newRange;
}

void LightStorage::SetAttenuationForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newAtt)
{
	// set attenuation of the light. Attenuation stores the three attenuation constants in
	// the format (a0, a1, a2) that control how light intensity falls off with distance
	pointLights_.data_[index].att = newAtt;
}





////////////////////////////////////////////////////////////////////////////////////////////////
//                   PUBLIC MODIFICATION API FOR SPOTLIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

void LightStorage::SetAmbientForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newAmbient)
{
	// set amount of ambient light emitted by the light source
	spotLights_.data_[index].ambient = newAmbient;
}

void LightStorage::SetDiffuseForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newDiffuse)
{
	// set amount of diffuse light emitted by the light source
	spotLights_.data_[index].diffuse = newDiffuse;
}

void LightStorage::SetSpecularForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newSpecular)
{
	// set amount of specular light emitted by the light source
	spotLights_.data_[index].specular = newSpecular;
}

void LightStorage::SetPositionForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newPosition)
{
	// set position of the light source
	spotLights_.data_[index].position = newPosition;
}

void LightStorage::AdjustPositionForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & adjustPos)
{
	// adjust position of the light source by some values (adjustPos)
	DirectX::XMFLOAT3 & pos = spotLights_.data_[index].position;

	pos.x += adjustPos.x;
	pos.y += adjustPos.y;
	pos.z += adjustPos.z;
}

void LightStorage::SetRangeForSpotLightByIndex(const UINT index, const float newRange)
{
	// set range of the light. A point whose distance from the light source is greater 
	// than the range is not lit
	spotLights_.data_[index].range = newRange;
}

void LightStorage::SetDirectionForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newDirection)
{
	// set direction of the light
	spotLights_.data_[index].direction = newDirection;
}

void LightStorage::SetSpotExponentForTheSpotLightByIndex(const UINT index, const float newSpotExp)
{
	// set the exponent which is used in the spotlight calculation to control
	// the spotlight cone
	spotLights_.data_[index].spot = newSpotExp;
}

void LightStorage::SetAttenuationForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newAtt)
{
	// set attenuation of the light. Attenuation stores the three attenuation constants in
	// the format (a0, a1, a2) that control how light intensity falls off with distance
	spotLights_.data_[index].att = newAtt;
}





////////////////////////////////////////////////////////////////////////////////////////////////
//                      PUBLIC QUERY API FOR DIRECTIONAL LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

const ptrdiff_t LightStorage::GetNumOfDiffuseLights() const
{
	return std::ssize(dirLights_.IDs_);
}

const ptrdiff_t LightStorage::GetNumOfPointLights() const
{
	return std::ssize(pointLights_.IDs_);
}

const ptrdiff_t LightStorage::GetNumOfSpotLights() const
{
	return std::ssize(spotLights_.IDs_);
}

#if 0
const DirectX::XMFLOAT3 & LightStorage::GetAmbientColorOfDiffuseLightByIndex(const UINT index) const
{
	return dirLights_.ambientColors_[index];
}

const DirectX::XMFLOAT3 & LightStorage::GetDiffuseColorOfDiffuseLightByIndex(const UINT index) const
{
	return dirLights_.diffuseColors_[index];
}

const DirectX::XMVECTOR & LightStorage::GetDirectionOfDiffuseLightByIndex(const UINT index) const
{
	return dirLights_.dirQuats_[index];
}

const DirectX::XMFLOAT3 & LightStorage::GetSpecularColorOfDiffuseLightByIndex(const UINT index) const
{
	return dirLights_.specularColors_[index];
}

const float LightStorage::GetSpecularPowerOfDiffuseLightByIndex(const UINT index) const
{
	return dirLights_.specularPowers_[index];
}

#endif




////////////////////////////////////////////////////////////////////////////////////////////////
//                       PUBLIC QUERY API FOR POINT LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////
#if 0


const DirectX::XMVECTOR & LightStorage::GetPositionOfPointLightByIndex(const UINT index) const
{
	return pointLights_.positions_[index];
}

const DirectX::XMFLOAT3 & LightStorage::GetColorOfPointLightByIndex(const UINT index) const
{
	return pointLights_.colors_[index];
}
#endif



////////////////////////////////////////////////////////////////////////////////////////////////

// memory allocation
void* LightStorage::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}

	Log::Error(LOG_MACRO, "can't allocate the memory for object");
	throw std::bad_alloc{};
}

void LightStorage::operator delete(void* p)
{
	_aligned_free(p);
}