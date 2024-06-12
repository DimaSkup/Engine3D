// **********************************************************************************
// Filename:      TranformSystem.cpp
// Description:   implementation of the TransformSystem's functional
// 
// Created:       20.05.24
// **********************************************************************************
#include "TransformSystem.h"
#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Utils.h"
#include "../ECS_Common/log.h"

#include <stdexcept>
#include <algorithm>

using namespace DirectX;
using namespace ECS;

TransformSystem::TransformSystem(
	Transform* pTransform,
	WorldMatrix* pWorld)
{
	ASSERT_NOT_NULLPTR(pTransform, "ptr to the Transform component == nullptr");
	ASSERT_NOT_NULLPTR(pWorld, "ptr to the WorldMatrix component == nullptr");

	pTransform_ = pTransform;
	pWorldMat_ = pWorld;
}

///////////////////////////////////////////////////////////

void TransformSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
	const std::vector<XMFLOAT3>& scales)
{
	AddRecordsToTransformComponent(enttsIDs, positions, directions, scales);
	AddRecordsToWorldMatrixComponent(enttsIDs, positions, directions, scales);
}

///////////////////////////////////////////////////////////

void TransformSystem::RemoveRecords(const std::vector<EntityID>& enttsIDs)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}

///////////////////////////////////////////////////////////

const std::vector<EntityID>& TransformSystem::GetEnttsIDsFromTransformComponent() const
{
	// get a bunch of all the entities IDs which the transform component has
	// return: a reference to the array of entities IDs
	return pTransform_->ids_;
}

///////////////////////////////////////////////////////////

void TransformSystem::GetEnttsIDsFromWorldMatrixComponent(
	std::vector<EntityID>& outEnttsIDs)
{
	// get all the entities IDs from the WorldMatrix component
	outEnttsIDs.reserve(pWorldMat_->worlds_.size());

	for (const auto& it : pWorldMat_->worlds_)
		outEnttsIDs.push_back(it.first);
}

///////////////////////////////////////////////////////////


void PrintPositionByID(const EntityID& enttID)
{
	assert("TODO: IMPLEMENT IT!" && 0);
#if 0
	try
	{

	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't find a position by entity ID: " + std::to_string(enttID));
	}
#endif
}

///////////////////////////////////////////////////////////

void PrintWorldMatrixByID(const EntityID& enttID)
{
	assert("TODO: IMPLEMENT IT!" && 0);
#if 0
	try
	{

	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't find a world matrix by entity ID: " + std::to_string(enttID));
	}
#endif
}

///////////////////////////////////////////////////////////

void TransformSystem::GetTransformDataOfEntt(
	const EntityID& enttID,
	XMFLOAT3& outPosition,
	XMFLOAT3& outDirection,
	XMFLOAT3& outScale)
{
	// get a transform data by ID from the Transform component

	try
	{
		Transform& t = *pTransform_;
		const ptrdiff_t idx = Utils::GetIdxOfID(t.ids_, enttID);

		outPosition = t.positions_[idx];
		outDirection = t.directions_[idx];
		outScale = t.scales_[idx];
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't find transform data by entity ID: " + std::to_string(enttID));
	}
}

void TransformSystem::GetWorldMatrixOfEntt(
	const EntityID& enttID,
	XMMATRIX& outWorldMatrix)
{
	// get a world matrix by ID from the WorldMatrix component
	try
	{
		outWorldMatrix = pWorldMat_->worlds_.at(enttID);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't find a world matrix by entity ID: " + std::to_string(enttID));
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::SetupEnttTransformData(
	const EntityID& enttID,
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& direction,      // (pitch,yaw,roll)
	const DirectX::XMFLOAT3& scale)
{
	// setup position, direction, scale, and world matrix 
	// for a single entity in terms of arrays
	assert("TODO: IMPLEMENT IT!" && 0);
}

///////////////////////////////////////////////////////////

void TransformSystem::SetupEnttsData(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<DirectX::XMFLOAT3>& positions,
	const std::vector<DirectX::XMFLOAT3>& directions,      // (pitch,yaw,roll)
	const std::vector<DirectX::XMFLOAT3>& scales)
{
	// for each input entity setup position, direction, scale;
	// build world matrix using this data;

	assert("TODO: FIX IT!" && 0);

#if 0
	try
	{
		for (size_t idx = 0; idx < enttsNames.size(); ++idx)
		{
			Transform::ComponentData& data = pTransform_->entityToData_.at(enttsNames[idx]);

			data.position_ = positions[idx];
			data.direction_ = directions[idx];
			data.scale_ = scales[idx];

			// world = SRT
			data.world_ =
				DirectX::XMMatrixScaling(scales[idx].x, scales[idx].y, scales[idx].z) *
				DirectX::XMMatrixRotationRollPitchYaw(directions[idx].x, directions[idx].y, directions[idx].z) *
				DirectX::XMMatrixTranslation(positions[idx].x, positions[idx].y, positions[idx].z);
		}
	}
	catch (const std::out_of_range& e)
	{
		std::string errorMsg = "Something went out of range for some of these entities: ";
		for (const entitiesName& id : entitiesNames)
			errorMsg += {id + "; "};

		ECS::Log::Error(ECS::Log_MACRO, e.what());
		THROW_ERROR(errorMsg);
	}
#endif
}


// ********************************************************************************
//
//                            PRIVATE HELPERS
// 
// ********************************************************************************

void TransformSystem::AddRecordsToTransformComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
	const std::vector<XMFLOAT3>& scales)
{
	// store transformation data of entities into the Transform component

	Transform& transform = *pTransform_;

	for (size_t idx = 0; idx < enttsIDs.size(); ++idx)
	{
		// check if there is no record with such entity ID
		if (!std::binary_search(transform.ids_.begin(), transform.ids_.end(), enttsIDs[idx]))
		{
			// execute sorted insertion into the data arrays
			const ptrdiff_t insertAtPos = Utils::GetPosForID(transform.ids_, enttsIDs[idx]);
		
			Utils::InsertAtPos<EntityID>(transform.ids_, insertAtPos, enttsIDs[idx]);
			Utils::InsertAtPos<XMFLOAT3>(transform.positions_, insertAtPos, positions[idx]);
			Utils::InsertAtPos<XMFLOAT3>(transform.directions_, insertAtPos, directions[idx]);
			Utils::InsertAtPos<XMFLOAT3>(transform.scales_, insertAtPos, scales[idx]);
		}
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::AddRecordsToWorldMatrixComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
	const std::vector<XMFLOAT3>& scales)
{
	// compute and store world matrices into the WorldMatrix component
	
	const ptrdiff_t worldMatricesCount = std::ssize(positions);

	std::vector<XMMATRIX> worldMatrices;
	worldMatrices.reserve(worldMatricesCount);

	// compute world matrices
	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		worldMatrices.push_back(
			XMMatrixScaling(scales[idx].x, scales[idx].y, scales[idx].z) *
			XMMatrixRotationRollPitchYaw(directions[idx].x, directions[idx].y, directions[idx].z) *
			XMMatrixTranslation(positions[idx].x, positions[idx].y, positions[idx].z)
		);

	// store world matrices
	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		pWorldMat_->worlds_.try_emplace(enttsIDs[idx], std::move(worldMatrices[idx]));
}
