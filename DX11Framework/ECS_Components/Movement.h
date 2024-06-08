// *********************************************************************************
// Filename:     Movement.h
// Description:  an ECS component which contains movement data of entities;
// 
// Created:
// *********************************************************************************
#pragma once



#include "../ECS_Entity/ECS_Types.h"

#include <DirectXMath.h>
#include <vector>


class Movement
{
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;

public:
	Movement() {}

public:
	ComponentType type_ = ComponentType::MoveComponent;
	
	std::vector<EntityID> enttsIDs_;
	std::vector<XMFLOAT3> translations_;
	std::vector<XMFLOAT4> rotationQuats_;  // rotation quatertion {0, pitch, yaw, roll}
	std::vector<XMFLOAT3> scaleChanges_;
};
