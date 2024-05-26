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
		StoreTransformData(entityIDsToUpdate, transformData, transformComponent.entityToData_);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("Went out of range during movement updating");
	}
	

}

///////////////////////////////////////////////////////////

void MovementSystem::SetTranslationsByIDs(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT3>& newTranslations,
	Movement& moveComponent)
{
	const size_t entityIDsArrSize = entityIDs.size();
	assert(entityIDsArrSize == newTranslations.size());

	try
	{
		auto& dataContainer = moveComponent.GetRefToData();

		// store new translations by entity IDs into the movement component 
		for (size_t idx = 0; idx < entityIDsArrSize; ++idx)
			 dataContainer.at(entityIDs[idx]).translation_ = newTranslations[idx];
	}
	catch (const std::out_of_range& e)
	{
		std::string errorMsg = "Something went out of range for some of these entities: ";
		for (const EntityID& id : entityIDs)
			errorMsg += {id + "; "};

		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR(errorMsg);
	}
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

		// store new rotation quaternions by entity IDs into the movement component 
		for (size_t idx = 0; idx < entityIDsArrSize; ++idx)
			dataContainer.at(entityIDs[idx]).rotationQuat_ = newRotationQuats[idx];
	}
	catch (const std::out_of_range& e)
	{
		std::string errorMsg = "Something went out of range for some of these entities: ";
		for (const EntityID& id : entityIDs)
			errorMsg += {id + "; "};

		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR(errorMsg);
	}
}

///////////////////////////////////////////////////////////

void MovementSystem::SetScaleFactorsByIDs(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT3>& newScaleFactors,
	Movement& moveComponent)
{
	const size_t entityIDsArrSize = entityIDs.size();
	assert(entityIDsArrSize == newScaleFactors.size());

	try
	{
		auto& dataContainer = moveComponent.GetRefToData();

		// store new scale changes by entity IDs into the movement component 
		for (size_t idx = 0; idx < entityIDsArrSize; ++idx)
			dataContainer.at(entityIDs[idx]).scaleChange_ = newScaleFactors[idx];
	}
	catch (const std::out_of_range& e)
	{
		std::string errorMsg = "Something went out of range for some of these entities: ";
		for (const EntityID& id : entityIDs)
			errorMsg += {id + "; "};

		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR(errorMsg);
	}
}

///////////////////////////////////////////////////////////
