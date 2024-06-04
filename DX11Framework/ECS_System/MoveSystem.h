// **********************************************************************************
// Filename:      MoveSystem.h
// Description:   Entity-Component-System (ECS) system
//                for controlling of the entities movement;
// 
// Created:       .05.24
// **********************************************************************************

#pragma once

#include "../ECS_Components/Movement.h"
#include "../ECS_Components/Transform.h"
#include "../Engine/log.h"

#include <vector>
#include <set>
#include <DirectXMath.h>
#include <stdexcept>

class MoveSystem
{
public:
	MoveSystem(
		Transform* pTransformComponent,
		Movement* pMoveComponent);

	void UpdateAllMoves(const float deltaTime);

	void AddRecord(const EntityID& entityID);
	void RemoveRecord(const EntityID& entityID);

	void SetTranslationsByIDs(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT3>& newTranslations);

	void SetRotationQuatsByIDs(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT4>& newRotationQuats);

	void SetScaleFactorsByIDs(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT3>& newScaleFactors);


	// for debug/unit-test purposes
	std::set<EntityID> GetEntitiesIDsSet() const;

private:
	Transform* pTransformComponent_ = nullptr;
	Movement* pMoveComponent_ = nullptr;
};
