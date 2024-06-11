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
	ComponentType type_ = ComponentType::TransformComponent;

	std::unordered_map<EntityID, size_t> enttIdToDataIdx_;   // pairs ['entity_id' => 'data_idx']
	std::vector<XMFLOAT3> positions_;
	std::vector<XMFLOAT3> directions_;
	std::vector<XMFLOAT3> scales_;
};
