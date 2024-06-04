// *********************************************************************************
// Filename:     Movement.h
// Description:  an ECS component which contains movement data of entities;
// 
// Created:
// *********************************************************************************
#pragma once



#include "../ECS_Entity/ECS_Types.h"

#include <DirectXMath.h>
#include <map>


class Movement
{
public:
	struct ComponentData
	{
		DirectX::XMFLOAT3 translation_{0,0,0};
		DirectX::XMFLOAT4 rotationQuat_{0,0,0,0};                // rotation quatertion 
		DirectX::XMFLOAT3 scaleChange_{1,1,1};
	};

public:
	Movement() {}

public:
	ComponentType type_ = ComponentType::MovementComponent;
	std::map<EntityID, ComponentData> entityToData_;   // pairs of: ['entity_id' => 'data_structure'] 
};
