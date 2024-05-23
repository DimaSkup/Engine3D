// **********************************************************************************
// Filename:      TranformSystem.cpp
// Description:   implementation of the TransformSystem's functional
// 
// Created:       20.05.24
// **********************************************************************************
#include "TransformSystem.h"


///////////////////////////////////////////////////////////

void TransformSystem::SetWorld(
	const EntityID& entityID,
	const DirectX::XMFLOAT3& scale,
	const DirectX::XMFLOAT3& dir,      // direction (pitch,yaw,roll)
	const DirectX::XMFLOAT3& pos,      // position
	Transform& transform)
{
	// setup position, direction, scale with input params;
	// build world matrix using this data;

	const auto it = transform.entityToData_.find(entityID);

	if (it != transform.entityToData_.end())
	{
		Transform::ComponentData& data = it->second;

		data.position_ = pos;
		data.direction_ = dir;
		data.scale_ = scale;

		// world = SRT
		data.world_ =
			DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
			DirectX::XMMatrixRotationRollPitchYaw(dir.x, dir.y, dir.z) *
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	}
	else
	{
		COM_ERROR("there is no record with entity by such ID: " + entityID);
	}
}