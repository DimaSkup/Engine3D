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

struct Bounding
{
	ComponentType componentType_ = ComponentType::BoundingComponent;

	std::vector<BoundingType> types_;
	std::vector<EntityID> ids_;

	// center  - center of the box / sphere; 
	// extents - Distance from the center to each side OR radius of the sphere
	std::vector<DirectX::BoundingBox> data_;    
	
};


} // namespace ECS