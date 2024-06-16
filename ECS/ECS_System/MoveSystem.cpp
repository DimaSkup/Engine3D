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
	const std::vector<EntityID>& enttsToMove = pMoveComponent_->ids_;

	// if we don't have any entities to move we just go out
	if (enttsToMove.size() == 0)
		return;

	try
	{
		Transform& transform = *pTransformComponent_;
		Movement& movement = *pMoveComponent_;

		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> directions;
		std::vector<XMFLOAT3> scales;
		std::vector<ptrdiff_t> transformDataIdxs;

		// current transform data of entities as XMVECTORs
		std::vector<DirectX::XMVECTOR> positionsVec;
		std::vector<DirectX::XMVECTOR> directionsVec;
		std::vector<DirectX::XMVECTOR> scalesVec;

		// current movement data of entities as XMVECTORs
		std::vector<DirectX::XMVECTOR> translationsVec;
		std::vector<DirectX::XMVECTOR> rotQuatsVec;
		std::vector<DirectX::XMVECTOR> scaleChangesVec;

		// get entities transform data to update for this frame
		GetTransformDataToUpdate(enttsToMove,
			transform,
			transformDataIdxs,
			positions, 
			directions, 
			scales);

		GetTransformDataAsArraysOfXMVectors(
			positions,
			directions, 
			scales, 
			positionsVec,
			directionsVec, 
			scalesVec);

		GetMovementDataAsArraysOfXMVectors( 
			deltaTime,
			movement.translations_,
			movement.rotationQuats_,
			movement.scaleChanges_,
			translationsVec,
			rotQuatsVec,
			scaleChangesVec);

		// compute new values of transform data using the movement data
		ComputeTransformData(
			deltaTime,
			translationsVec,
			rotQuatsVec,
			scaleChangesVec,
			positions,
			directions, 
			scales);

		// write updated transform data into the Transform component
		ApplyTransformData();

		// rebuild world matrices of that entities which were moved
		ComputeAndApplyWorldMatrices(translations, rotQuats, scaleChanges, outTransformData);
	}
	catch (const std::out_of_range& e)
	{
		ECS::Log::Error(ECS::Log_MACRO, e.what());
		THROW_ERROR("Went out of range during movement updating");
	}
}

///////////////////////////////////////////////////////////

void MoveSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& translations,
	const std::vector<XMFLOAT4>& rotationQuats,  // rotation quaternions (but stored as XMFLOAT4)
	const std::vector<XMFLOAT3>& scaleChanges)
{
	Movement& move = *pMoveComponent_;

	Utils::AppendArray<EntityID>(move.ids_, enttsIDs);
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

	outEnttsIDs = pMoveComponent_->ids;
}

///////////////////////////////////////////////////////////
