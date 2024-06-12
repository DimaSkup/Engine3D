// *********************************************************************************
// Filename:     Transform.h
// Description:  an ECS component which contains transformation data of entities;
// 
// Created:
// *********************************************************************************
#pragma once


#include "../ECS_Common/ECS_Types.h"
#include <unordered_map>


struct Transform
{
	Transform()
	{
		// reserve memory for some amount of data
		const size_t newCapacity = 100;

		ids_.reserve(newCapacity);
		positions_.reserve(newCapacity);
		directions_.reserve(newCapacity);
		scales_.reserve(newCapacity);
	}

	ComponentType type_ = ComponentType::TransformComponent;

	std::vector<EntityID> ids_; 
	std::vector<XMFLOAT3> positions_;
	std::vector<XMFLOAT3> directions_;
	std::vector<XMFLOAT3> scales_;
};
