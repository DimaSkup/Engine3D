// *********************************************************************************
// Filename:     WorldMatrix.h
// Description:  an ECS component which contains world matrices of entities;
// 
// Created:      08.06.24
// *********************************************************************************
#pragma once

#include "../ECS_Entity/ECS_Types.h"
#include <DirectXMath.h>
#include <unordered_map>

class WorldMatrix
{
public:
	WorldMatrix() {}

	ComponentType type_ = ComponentType::WorldMatrixComponent;
	std::unordered_map<EntityID, DirectX::XMMATRIX> worlds_;    
};
