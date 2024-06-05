// *********************************************************************************
// Filename:     Transform.h
// Description:  an ECS component which contains transformation data of entities;
// 
// Created:
// *********************************************************************************
#pragma once


#include "../ECS_Entity/ECS_Types.h"
#include <DirectXMath.h>
#include <map>


class Transform
{
public:
	struct ComponentData
	{
		DirectX::XMFLOAT3 position_{ 0,0,0 };
		DirectX::XMFLOAT3 direction_{ 0,0,0 };
		DirectX::XMFLOAT3 scale_{ 1,1,1 };
		DirectX::XMMATRIX world_ = DirectX::XMMatrixIdentity();
	};

public:
	Transform() {}

public:
	ComponentType type_ = ComponentType::TransformComponent;
	std::map<EntityID, ComponentData> entityToData_;   // pairs of: ['entity_id' => 'data_structure'] 
};
