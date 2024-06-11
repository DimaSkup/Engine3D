// *********************************************************************************
// Filename:     WorldMatrix.h
// Description:  an ECS component which contains world matrices of entities;
// 
// Created:      08.06.24
// *********************************************************************************
#pragma once

#include "../ECS_Common/ECS_Types.h"
#include <map>

struct WorldMatrix
{
	ComponentType type_ = ComponentType::WorldMatrixComponent;
	std::map<EntityID, DirectX::XMMATRIX> worlds_;    
};
