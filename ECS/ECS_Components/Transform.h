// *********************************************************************************
// Filename:     Transform.h
// Description:  an ECS component which contains transformation data of entities;
// 
// Created:
// *********************************************************************************
#pragma once


#include "../ECS_Common/ECS_Types.h"
#include <vector>

struct Transform
{
	Transform()
	{
		// reserve memory for some amount of data
		const size_t newCapacity = 100;

		ids_.reserve(newCapacity);
		posAndUniformScale_.reserve(newCapacity);
		dirQuats_.reserve(newCapacity);
	}

	ComponentType type_ = ComponentType::TransformComponent;

	std::vector<EntityID> ids_; 
	std::vector<XMFLOAT4> posAndUniformScale_;  // pos (x,y,z); uniform scale (w)
	std::vector<XMVECTOR> dirQuats_;            // normalized direction quaternion

};
