// **********************************************************************************
// Filename:      MoveSystem.h
// Description:   Entity-Component-System (ECS) system
//                for controlling of the entities movement;
// 
// Created:       .05.24
// **********************************************************************************

#pragma once

// components
#include "../ECS_Components/Movement.h"
#include "../ECS_Components/Transform.h"
#include "../ECS_Components/WorldMatrix.h"

#include "../Engine/log.h"

#include <vector>
#include <set>
#include <DirectXMath.h>
#include <stdexcept>

class MoveSystem
{
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;

public:
	MoveSystem(
		Transform* pTransformComponent,
		WorldMatrix* pWorldMatComponent_,
		Movement* pMoveComponent);

	void UpdateAllMoves(const float deltaTime);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& translations,
		const std::vector<XMFLOAT4>& rotationQuats,  // rotation quaternions (but stored as XMFLOAT4)
		const std::vector<XMFLOAT3>& scaleChanges);

	void RemoveRecords(const std::vector<EntityID>& enttsIDs);

	// for debug/unit-test purposes
	std::set<EntityID> GetEnttsIDsSet() const;

private:
	Transform* pTransformComponent_ = nullptr;
	WorldMatrix* pWorldMatComponent_ = nullptr;
	Movement* pMoveComponent_ = nullptr;
};
