// *********************************************************************************
// Filename:     Transform.h
// Description:  an ECS component which contains transformation data of entities;
// 
// Created:
// *********************************************************************************
#pragma once


#include "../ECS_Entity/ECS_Types.h"
#include <DirectXMath.h>
#include <unordered_map>
#include <vector>


class Transform
{
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	Transform() {}

public:
	ComponentType type_ = ComponentType::TransformComponent;

	std::unordered_map<EntityID, size_t> enttIdToDataIdx_;   // pairs ['entity_id' => 'data_idx']
	std::vector<XMFLOAT3> positions_;
	std::vector<XMFLOAT3> directions_;
	std::vector<XMFLOAT3> scales_;
};
