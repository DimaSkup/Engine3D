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

#include <set>

class MoveSystem final
{
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

	void GetEnttsIDsFromMoveComponent(std::vector<EntityID>& outEnttsIDs);

private:
	Transform* pTransformComponent_ = nullptr;
	WorldMatrix* pWorldMatComponent_ = nullptr;
	Movement* pMoveComponent_ = nullptr;
};
