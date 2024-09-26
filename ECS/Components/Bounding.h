// *********************************************************************************
// Filename:     Bounding.h
// Description:  an ECS component stores Bounding data for entities
// 
// Created:      26.09.24
// *********************************************************************************
#pragma once

#include "../Common/Types.h"
#include <vector>
#include <DirectXCollision.h>

namespace ECS
{

enum BoundingType
{
	SPHERE,
	AABB,
};

struct BoundingData
{
	DirectX::XMFLOAT3 center{0,0,0};            // Center of the box.
	DirectX::XMFLOAT3 extents{0,0,0};           // Distance (it can be radius) from the center to each side. 
};

struct Bounding
{
	ComponentType componentType_ = ComponentType::BoundingComponent;

	std::vector<EntityID> ids_;
	std::vector<BoundingData> data_;
	std::vector<BoundingType> types_;
};


} // namespace ECS