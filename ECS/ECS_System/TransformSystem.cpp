// **********************************************************************************
// Filename:      TranformSystem.cpp
// Description:   implementation of the TransformSystem's functional
// 
// Created:       20.05.24
// **********************************************************************************
#include "TransformSystem.h"
#include <stdexcept>
#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Utils.h"

using namespace DirectX;


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

void TransformSystem::GetEnttsIDsFromTransformComponent(std::vector<EntityID>& outEnttsIDs)
{
	// get a bunch of all the entities IDs which the transform component has
	// out: array of entities IDs

	outEnttsIDs.reserve(std::ssize(pTransform_->enttIdToDataIdx_));

	for (const auto& it : pTransform_->enttIdToDataIdx_)
		outEnttsIDs.push_back(it.first);	
}

///////////////////////////////////////////////////////////

void TransformSystem::GetTransformDataOfEntity(
	const EntityID& enttID,
	DirectX::XMFLOAT3& outPosition,
	DirectX::XMFLOAT3& outDirection,
	DirectX::XMFLOAT3& outScale)
{
	assert("TODO: IMPLEMENT IT!" && 0);
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

	// make pairs 'entity_id' => 'data_idx_into_arrays'  (keys => data_idx)
	ptrdiff_t data_idx = std::ssize(transform.positions_);

	for (const EntityID& enttID : enttsIDs)
		transform.enttIdToDataIdx_.try_emplace(enttID, data_idx++);

	// store the transform data
	Utils::AppendArray(transform.positions_, positions);
	Utils::AppendArray(transform.directions_, directions);
	Utils::AppendArray(transform.scales_, scales);
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
