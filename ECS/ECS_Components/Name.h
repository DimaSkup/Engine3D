// *********************************************************************************
// Filename:     Name.h
// Description:  an ECS component which contains names of some entities;
// 
// Created:      12.06.24
// *********************************************************************************
#pragma once

#include "../ECS_Common/ECS_Types.h"
#include <vector>

// ECS component
struct Name
{
	ComponentType type_ = ComponentType::NameComponent;

	// both vectors have the same length because 
	// there is one to one records ['entity_id' => 'entity_name']
	std::vector<EntityID> ids_;
	std::vector<EntityName> names_;
};
