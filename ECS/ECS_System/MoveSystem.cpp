// **********************************************************************************
// Filename:      MoveSystem.h
// Description:   implementation of the MoveSystem's functional
// 
// Created:       23.05.24
// **********************************************************************************
#include "MoveSystem.h"

#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Utils.h"
#include "./Helpers/MoveSystemUpdateHelpers.h"

#include <stdexcept>

using namespace DirectX;


MoveSystem::MoveSystem(
	Transform* pTransformComponent,
	WorldMatrix* pWorldMatrixComponent,
	Movement* pMoveComponent)
{
	ASSERT_NOT_NULLPTR(pTransformComponent, "ptr to the Transform component == nullptr");
	ASSERT_NOT_NULLPTR(pWorldMatrixComponent, "ptr to the WorldMatrix component == nullptr");
	ASSERT_NOT_NULLPTR(pMoveComponent, "ptr to the Movement component == nullptr");

	pTransformComponent_ = pTransformComponent;
	pWorldMatComponent_ = pWorldMatrixComponent;
	pMoveComponent_ = pMoveComponent;
}

///////////////////////////////////////////////////////////

void MoveSystem::UpdateAllMoves(const float deltaTime)
{
	return;
#if 0
	auto& movementDataContainer = pMoveComponent_->entityToData_;
	const size_t entitiesCountToUpdate = movementDataContainer.size();

	if (entitiesCountToUpdate == 0)
		return;

	UINT data_idx = 0;
	std::vector<entitiesName> entitiesNamesToUpdate(entitiesCountToUpdate);
	std::vector<Movement::ComponentData> movementData(entitiesCountToUpdate);
	std::vector<Transform::ComponentData> transformData(entitiesCountToUpdate);

	try
	{
		GetEntitiesToUpdate(movementDataContainer, entitiesNamesToUpdate);
		GetMovementDataToUpdate(movementDataContainer, movementData);
		GetTransformDataToUpdate(entitiesNamesToUpdate, pTransformComponent_->entityToData_, transformData);
		ComputeTransformData(deltaTime, movementData, transformData);
		StoreTransformData(entitiesNamesToUpdate, transformData, pTransformComponent_->entityToData_);
	}
	catch (const std::out_of_range& e)
	{
		ECS::Log::Error(ECS::Log_MACRO, e.what());
		THROW_ERROR("Went out of range during movement updating");
	}
#endif
}

///////////////////////////////////////////////////////////

void MoveSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& translations,
	const std::vector<XMFLOAT4>& rotationQuats,  // rotation quaternions (but stored as XMFLOAT4)
	const std::vector<XMFLOAT3>& scaleChanges)
{
	Movement& move = *pMoveComponent_;

	Utils::AppendArray<EntityID>(move.enttsIDs_, enttsIDs);
	Utils::AppendArray<XMFLOAT3>(move.translations_, translations);
	Utils::AppendArray<XMFLOAT4>(move.rotationQuats_, rotationQuats);
	Utils::AppendArray<XMFLOAT3>(move.scaleChanges_, scaleChanges);
}

///////////////////////////////////////////////////////////

void MoveSystem::RemoveRecords(const std::vector<EntityID>& enttsIDs)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}

///////////////////////////////////////////////////////////

void MoveSystem::GetEnttsIDsFromMoveComponent(std::vector<EntityID>& outEnttsIDs)
{
	// get a bunch of all the entities IDs which have the Move component
	// out: array of entities IDs

	outEnttsIDs = pMoveComponent_->enttsIDs_;
}

///////////////////////////////////////////////////////////