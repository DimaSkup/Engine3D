// **********************************************************************************
// Filename:      MovementSystem.h
// Description:   implementation of the MovementSystem's functional
// 
// Created:       23.05.24
// **********************************************************************************
#include "MovementSystem.h"

#include "./Helpers/MovementSystemUpdateHelpers.h"

using namespace DirectX;


void MovementSystem::Update(
	const float deltaTime,
	Transform& transformComponent,
	Movement& moveComponent)
{
	auto& movementDataContainer = moveComponent.GetRefToData();
	const size_t entitiesCountToUpdate = movementDataContainer.size();

	if (entitiesCountToUpdate == 0)
		return;

	UINT data_idx = 0;
	std::vector<EntityID> entityIDsToUpdate(entitiesCountToUpdate);
	std::vector<Movement::ComponentData> movementData(entitiesCountToUpdate);
	std::vector<Transform::ComponentData> transformData(entitiesCountToUpdate);

	try
	{
		GetEntitiesToUpdate(movementDataContainer, entityIDsToUpdate);
		GetMovementDataToUpdate(movementDataContainer, movementData);
		GetTransformDataToUpdate(entityIDsToUpdate, transformComponent.entityToData_, transformData);
		ComputeTransformData(deltaTime, movementData, transformData);
		//ApplyTransformData();
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR("Went out of range during movement updating");
	}
	
	// store new transform data
	for (size_t idx = 0; idx < entitiesCountToUpdate; ++idx)
	{
		transformComponent.entityToData_[entityIDsToUpdate[idx]] = transformData[idx];
	}
}

///////////////////////////////////////////////////////////

void MovementSystem::SetTranslationsByIDs(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT3>& newTranslations,
	Movement& moveComponent)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}

///////////////////////////////////////////////////////////

void MovementSystem::SetRotationQuatsByIDs(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT4>& newRotationQuats,
	Movement& moveComponent)
{
	const size_t entityIDsArrSize = entityIDs.size();
	assert(entityIDsArrSize == newRotationQuats.size());

	try
	{
		auto& dataContainer = moveComponent.GetRefToData();
		std::vector<DirectX::XMFLOAT4*> rotationQuats(entityIDsArrSize);

		// get references to the necessary data
		for (size_t idx = 0; idx < entityIDsArrSize; ++idx)
		{
			rotationQuats[idx] = &(dataContainer.at(entityIDs[idx]).rotationQuat_);
		}

		// store new data
		for (size_t idx = 0; idx < entityIDsArrSize; ++idx)
		{
			*rotationQuats[idx] = newRotationQuats[idx];
		}
	}
	catch (const std::out_of_range& e)
	{
		std::string errorMsg = "Something went out of range for some of these entities: ";
		for (const EntityID& id : entityIDs)
			errorMsg += {id + "; "};

		Log::Error(LOG_MACRO, e.what());
		COM_ERROR(errorMsg);
	}
}

///////////////////////////////////////////////////////////

void MovementSystem::SetScaleFactorsByIDs(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT3>& newScaleFactors,
	Movement& moveComponent)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}

///////////////////////////////////////////////////////////
