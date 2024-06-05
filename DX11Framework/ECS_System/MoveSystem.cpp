// **********************************************************************************
// Filename:      MoveSystem.h
// Description:   implementation of the MoveSystem's functional
// 
// Created:       23.05.24
// **********************************************************************************
#include "MoveSystem.h"

#include "./Helpers/MoveSystemUpdateHelpers.h"

using namespace DirectX;


MoveSystem::MoveSystem(
	Transform* pTransformComponent,
	Movement* pMoveComponent)
{
	ASSERT_NOT_NULLPTR(pTransformComponent, "ptr to the Transform component == nullptr");
	ASSERT_NOT_NULLPTR(pMoveComponent, "ptr to the Movement component == nullptr");

	pTransformComponent_ = pTransformComponent;
	pMoveComponent_ = pMoveComponent;
}

///////////////////////////////////////////////////////////

void MoveSystem::UpdateAllMoves(const float deltaTime)
{
	auto& movementDataContainer = pMoveComponent_->entityToData_;
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
		GetTransformDataToUpdate(entityIDsToUpdate, pTransformComponent_->entityToData_, transformData);
		ComputeTransformData(deltaTime, movementData, transformData);
		StoreTransformData(entityIDsToUpdate, transformData, pTransformComponent_->entityToData_);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("Went out of range during movement updating");
	}
}

///////////////////////////////////////////////////////////

void MoveSystem::AddRecord(const EntityID& entityID)
{
	const auto res = pMoveComponent_->entityToData_.insert({ entityID, {} });
	ASSERT_TRUE(res.second, "can't create a record for entity: " + entityID);
}

///////////////////////////////////////////////////////////

void MoveSystem::RemoveRecord(const EntityID& entityID)
{
	pMoveComponent_->entityToData_.erase(entityID);
}

///////////////////////////////////////////////////////////

std::set<EntityID> MoveSystem::GetEntitiesIDsSet() const
{
	// return a set of all the entities which have the movement component

	std::set<EntityID> entityIDs;

	for (const auto& it : pMoveComponent_->entityToData_)
		entityIDs.insert(it.first);

	return entityIDs;
}

///////////////////////////////////////////////////////////

void MoveSystem::SetTranslationsByIDs(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT3>& newTranslations)
{
	const size_t entityIDsArrSize = entityIDs.size();
	assert(entityIDsArrSize == newTranslations.size());

	try
	{
		// store new translations by entity IDs into the movement component 
		for (size_t idx = 0; idx < entityIDsArrSize; ++idx)
			 pMoveComponent_->entityToData_.at(entityIDs[idx]).translation_ = newTranslations[idx];
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

void MoveSystem::SetRotationQuatsByIDs(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT4>& newRotationQuats)
{
	const size_t entityIDsArrSize = entityIDs.size();
	assert(entityIDsArrSize == newRotationQuats.size());

	try
	{
		auto& dataContainer = pMoveComponent_->entityToData_;

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

void MoveSystem::SetScaleFactorsByIDs(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT3>& newScaleFactors)
{
	const size_t entityIDsArrSize = entityIDs.size();
	assert(entityIDsArrSize == newScaleFactors.size());

	try
	{
		auto& dataContainer = pMoveComponent_->entityToData_;

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
