// **********************************************************************************
// Filename:      TranformSystem.cpp
// Description:   implementation of the TransformSystem's functional
// 
// Created:       20.05.24
// **********************************************************************************
#include "TransformSystem.h"
#include <stdexcept>
#include "../Engine/log.h"



TransformSystem::TransformSystem(Transform* pTransform)
{
	ASSERT_NOT_NULLPTR(pTransform, "ptr to the transform component == nullptr");

	pTransform_ = pTransform;
}

///////////////////////////////////////////////////////////

void TransformSystem::AddRecord(const EntityID& entityID)
{
	const auto res = pTransform_->entityToData_.insert({ entityID, {} });
	ASSERT_TRUE(res.second, "can't create a record for entity: " + entityID);
}

///////////////////////////////////////////////////////////

void TransformSystem::RemoveRecord(const EntityID& entityID)
{
	pTransform_->entityToData_.erase(entityID);
}

///////////////////////////////////////////////////////////

std::set<EntityID> TransformSystem::GetEntitiesIDsSet() const
{
	// return a set of all the entities which have the transform component

	std::set<EntityID> entityIDs;

	for (const auto& it : pTransform_->entityToData_)
		entityIDs.insert(it.first);

	return entityIDs;
}

///////////////////////////////////////////////////////////

void TransformSystem::GetTransformDataOfEntity(
	const EntityID& entityID,
	DirectX::XMFLOAT3& outPosition,
	DirectX::XMFLOAT3& outDirection,
	DirectX::XMFLOAT3& outScale)
{
	try
	{
		const Transform::ComponentData& data = pTransform_->entityToData_.at(entityID);

		outPosition = data.position_;
		outDirection = data.direction_;
		outScale = data.scale_;
	}
	catch (const std::out_of_range& e)
	{
		(void)e;
		THROW_ERROR("There is no transform data for entity by ID: " + entityID);
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::SetWorld(
	const EntityID& entityID,
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& direction,      // (pitch,yaw,roll)
	const DirectX::XMFLOAT3& scale)
{
	// build world matrix by input params for a SINGLE entity by ID

	try
	{
		Transform::ComponentData& data = pTransform_->entityToData_.at(entityID);

		data.position_ = position;
		data.direction_ = direction;
		data.scale_ = scale;

		// world = SRT
		data.world_ =
			DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
			DirectX::XMMatrixRotationRollPitchYaw(direction.x, direction.y, direction.z) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("Transform component doesn't know about such entity: " + entityID);
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::SetWorlds(
	const std::vector<EntityID>& entityIDs,
	const std::vector<DirectX::XMFLOAT3>& positions,
	const std::vector<DirectX::XMFLOAT3>& directions,      // (pitch,yaw,roll)
	const std::vector<DirectX::XMFLOAT3>& scales)
{
	// for each input entity setup position, direction, scale;
	// build world matrix using this data;

	try
	{
		for (size_t idx = 0; idx < entityIDs.size(); ++idx)
		{
			Transform::ComponentData& data = pTransform_->entityToData_.at(entityIDs[idx]);

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
		for (const EntityID& id : entityIDs)
			errorMsg += {id + "; "};

		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR(errorMsg);
	}
}