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

void LightStore::CreateNewDirectionalLight(
	const DirectX::XMFLOAT4 & ambient,                // the amount of ambient light emitted by the light source
	const DirectX::XMFLOAT4 & diffuse,                // the amount of diffuse light emitted by the light source
	const DirectX::XMFLOAT4 & specular,               // the amount of specular light emitted by the light source
	const DirectX::XMFLOAT3 & direction)           // the direction of the light
{
	// create a new directional light source

	DirectionalLightsStore & storage = dirLightsStore_;

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
	storage.dirLightsArr_.push_back(dirLight);

	// increase the number of directed lights
	++storage.numOfDirectionalLights_;
}

///////////////////////////////////////////////////////////

void LightStore::CreateNewPointLight(
	const DirectX::XMFLOAT4 & ambient,                // the amount of ambient light emitted by the light source
	const DirectX::XMFLOAT4 & diffuse,                // the amount of diffuse light emitted by the light source
	const DirectX::XMFLOAT4 & specular,               // the amount of specular light emitted by the light source
	const DirectX::XMFLOAT3 & position,               // the position of the light
	const float range,                                // the range of the light
	const DirectX::XMFLOAT3 & attenuation)         // params for controlling how light intensity falls off with distance
{
	// create a new point light source

	PointLightsStore & storage = pointLightsStore_;

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
	storage.pointLightsArr_.push_back(light);

	++storage.numOfPointLights_;
}

///////////////////////////////////////////////////////////

void LightStore::CreateNewSpotLight(
	const DirectX::XMFLOAT4 & ambient,                 // the amount of ambient light emitted by the light source
	const DirectX::XMFLOAT4 & diffuse,                 // the amount of diffuse light emitted by the light source
	const DirectX::XMFLOAT4 & specular,                // the amount of specular light emitted by the light source
	const DirectX::XMFLOAT3 & position,                // the position of the light
	const float range,                                 // the range of the light
	const DirectX::XMFLOAT3 & direction,               // the direction of the light
	const float spotExponent,                          // the exponent used in the spotlight calculation to control the spotlight cone
	const DirectX::XMFLOAT3 & att)                     // attenuation: params for controlling how light intensity falls off with distance
{
	// create a new spotlight source

	SpotLightsStore & storage = spotLightsStore_;

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
	storage.spotLightsArr_.push_back(light);

	++storage.numOfSpotLights_;
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
#if 0
	const UINT numOfPointLights = pointLightsStore_.numOfPointLights_;
	PointLightsStore & store = pointLightsStore_;

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

void LightStore::SetAmbientForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newAmbient)
{
	// set amount of ambient light emitted by the light source
	dirLightsStore_.dirLightsArr_[index].ambient = newAmbient;
}

void LightStore::SetDiffuseForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newDiffuse)
{
	// set amount of diffuse light emitted by the light source
	dirLightsStore_.dirLightsArr_[index].diffuse = newDiffuse;
}

void LightStore::SetSpecularForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newSpecular)
{
	// set amount of specular light emitted by the light source
	dirLightsStore_.dirLightsArr_[index].specular = newSpecular;
}

void LightStore::SetDirectionForDirectionalLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newDirection)
{
	// set direction of the light
	dirLightsStore_.dirLightsArr_[index].direction = newDirection;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//                   PUBLIC MODIFICATION API FOR POINT LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

void LightStore::SetAmbientForPointLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newAmbient)
{
	// set amount of ambient light emitted by the light source
	dirLightsStore_.dirLightsArr_[index].ambient = newAmbient;
}

void LightStore::SetDiffuseForPointLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newDiffuse)
{
	// set amount of diffuse light emitted by the light source
	dirLightsStore_.dirLightsArr_[index].diffuse = newDiffuse;
}

void LightStore::SetSpecularForPointLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newSpecular)
{
	// set amount of specular light emitted by the light source
	dirLightsStore_.dirLightsArr_[index].specular = newSpecular;
}

void LightStore::SetPositionForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newPosition)
{
	// set position of the light source
	pointLightsStore_.pointLightsArr_[index].position = newPosition;
}

void LightStore::AdjustPositionForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & adjustPos)
{

	// adjust position of the light source by some values (adjustPos)
	DirectX::XMFLOAT3 & pos = pointLightsStore_.pointLightsArr_[index].position;

	pos.x += adjustPos.x;
	pos.y += adjustPos.y;
	pos.z += adjustPos.z;
}

void LightStore::SetRangeForPointLightByIndex(const UINT index, const float newRange)
{
	// set range of the light. A point whose distance from the light source is greater 
	// than the range is not lit
	pointLightsStore_.pointLightsArr_[index].range = newRange;
}

void LightStore::SetAttenuationForPointLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newAtt)
{
	// set attenuation of the light. Attenuation stores the three attenuation constants in
	// the format (a0, a1, a2) that control how light intensity falls off with distance
	pointLightsStore_.pointLightsArr_[index].att = newAtt;
}





////////////////////////////////////////////////////////////////////////////////////////////////
//                   PUBLIC MODIFICATION API FOR SPOTLIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

void LightStore::SetAmbientForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newAmbient)
{
	// set amount of ambient light emitted by the light source
	spotLightsStore_.spotLightsArr_[index].ambient = newAmbient;
}

void LightStore::SetDiffuseForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newDiffuse)
{
	// set amount of diffuse light emitted by the light source
	spotLightsStore_.spotLightsArr_[index].diffuse = newDiffuse;
}

void LightStore::SetSpecularForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT4 & newSpecular)
{
	// set amount of specular light emitted by the light source
	spotLightsStore_.spotLightsArr_[index].specular = newSpecular;
}

void LightStore::SetPositionForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newPosition)
{
	// set position of the light source
	spotLightsStore_.spotLightsArr_[index].position = newPosition;
}

void LightStore::AdjustPositionForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & adjustPos)
{
	// adjust position of the light source by some values (adjustPos)
	DirectX::XMFLOAT3 & pos = spotLightsStore_.spotLightsArr_[index].position;

	pos.x += adjustPos.x;
	pos.y += adjustPos.y;
	pos.z += adjustPos.z;
}

void LightStore::SetRangeForSpotLightByIndex(const UINT index, const float newRange)
{
	// set range of the light. A point whose distance from the light source is greater 
	// than the range is not lit
	spotLightsStore_.spotLightsArr_[index].range = newRange;
}

void LightStore::SetDirectionForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newDirection)
{
	// set direction of the light
	spotLightsStore_.spotLightsArr_[index].direction = newDirection;
}

void LightStore::SetSpotExponentForTheSpotLightByIndex(const UINT index, const float newSpotExp)
{
	// set the exponent which is used in the spotlight calculation to control
	// the spotlight cone
	spotLightsStore_.spotLightsArr_[index].spot = newSpotExp;
}

void LightStore::SetAttenuationForSpotLightByIndex(const UINT index, const DirectX::XMFLOAT3 & newAtt)
{
	// set attenuation of the light. Attenuation stores the three attenuation constants in
	// the format (a0, a1, a2) that control how light intensity falls off with distance
	spotLightsStore_.spotLightsArr_[index].att = newAtt;
}





////////////////////////////////////////////////////////////////////////////////////////////////
//                      PUBLIC QUERY API FOR DIRECTIONAL LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////

const UINT LightStore::GetNumOfDiffuseLights() const
{
	return dirLightsStore_.numOfDirectionalLights_;
}

const UINT LightStore::GetNumOfPointLights() const
{
	return pointLightsStore_.numOfPointLights_;
}

const UINT LightStore::GetNumOfSpotLights() const
{
	return spotLightsStore_.numOfSpotLights_;
}

#if 0
const DirectX::XMFLOAT3 & LightStore::GetAmbientColorOfDiffuseLightByIndex(const UINT index) const
{
	return dirLightsStore_.ambientColors_[index];
}

const DirectX::XMFLOAT3 & LightStore::GetDiffuseColorOfDiffuseLightByIndex(const UINT index) const
{
	return dirLightsStore_.diffuseColors_[index];
}

const DirectX::XMVECTOR & LightStore::GetDirectionOfDiffuseLightByIndex(const UINT index) const
{
	return dirLightsStore_.directions_[index];
}

const DirectX::XMFLOAT3 & LightStore::GetSpecularColorOfDiffuseLightByIndex(const UINT index) const
{
	return dirLightsStore_.specularColors_[index];
}

const float LightStore::GetSpecularPowerOfDiffuseLightByIndex(const UINT index) const
{
	return dirLightsStore_.specularPowers_[index];
}

#endif




////////////////////////////////////////////////////////////////////////////////////////////////
//                       PUBLIC QUERY API FOR POINT LIGHT SOURCES
////////////////////////////////////////////////////////////////////////////////////////////////
#if 0


const DirectX::XMVECTOR & LightStore::GetPositionOfPointLightByIndex(const UINT index) const
{
	return pointLightsStore_.positions_[index];
}

const DirectX::XMFLOAT3 & LightStore::GetColorOfPointLightByIndex(const UINT index) const
{
	return pointLightsStore_.colors_[index];
}
#endif



////////////////////////////////////////////////////////////////////////////////////////////////

// memory allocation
void* LightStore::operator new(size_t i)
{
	if (void* ptr = _aligned_malloc(i, 16))
	{
		return ptr;
	}

	Log::Error(LOG_MACRO, "can't allocate the memory for object");
	throw std::bad_alloc{};
}

void LightStore::operator delete(void* p)
{
	_aligned_free(p);
}