// **********************************************************************************
// Filename:      MovementSystem.h
// Description:   Entity-Component-System (ECS) system
//                for controlling of the entities movement;
// 
// Created:       .05.24
// **********************************************************************************

#pragma once

#include "BaseSystem.h"
#include "../ECS_Components/Movement.h"
#include "../ECS_Components/Transform.h"
#include "../Engine/log.h"

#include <vector>
#include <DirectXMath.h>
#include <stdexcept>

struct MovementSystem : public BaseSystem
{
public:
	MovementSystem() : BaseSystem("MovementSystem") {}

	void Update(
		const float deltaTime,
		Transform& transformComponent,
		Movement& moveComponent);

	void SetTranslationsByIDs(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT3>& newTranslations,
		Movement& moveComponent);

	void SetRotationQuatsByIDs(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT4>& newRotationQuats,
		Movement& moveComponent);

	void SetScaleFactorsByIDs(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT3>& newScaleFactors,
		Movement& moveComponent);
};
