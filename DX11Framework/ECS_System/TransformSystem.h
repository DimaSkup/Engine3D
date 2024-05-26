// **********************************************************************************
// Filename:      TranformSystem.h
// Description:   Entity-Component-System (ECS) system for control 
//                transform data of entities
// 
// Created:       20.05.24
// **********************************************************************************

#pragma once

#include "../ECS_Entity/ECS_Types.h"

#if ECS_VER_0

#include "BaseSystem.h"
#include "../ECS_Components/BaseComponent.h"

//#include <vector>
#include <map>
#include <DirectXMath.h>

typedef unsigned int UINT;

class TransformSystem : public BaseSystem
{
public:
	TransformSystem(BaseComponent* pTransform) : 
		BaseSystem{ "TransformSystem", pTransform }
	{
	}

	void AddEntity(const EntityID)
	void RemoveEntity(const EntityID& entityID) override;

	void SetPositionsByIdxs(
		const std::vector<UINT>& idxs,
		const std::vector<DirectX::XMFLOAT3>& inPositions);

	void SetDirectionsByIdxs(
		const std::vector<UINT>& idxs,
		const std::vector<DirectX::XMFLOAT3>& inRotations);

	void SetScalesByIdxs(
		const std::vector<UINT>& idxs,
		const std::vector<DirectX::XMFLOAT3>& inScales);

	void SetWorldByIdx(
		const UINT idxs,
		const DirectX::XMFLOAT3& scale,
		const DirectX::XMFLOAT3& direction,
		const DirectX::XMFLOAT3& position);

	void SetPosRotScaleByIdxs(
		const std::vector<UINT>& idxs,
		const std::vector<DirectX::XMVECTOR>& inPositions,
		const std::vector<DirectX::XMVECTOR>& inRotations,
		const std::vector<DirectX::XMVECTOR>& inScales);
};

#elif ECS_VER_1

#include "BaseSystem.h"
#include "../ECS_Components/Transform.h"
#include <DirectXMath.h>
#include <vector>

typedef unsigned int UINT;

class TransformSystem : public BaseSystem
{
public:
	TransformSystem() :	BaseSystem("TransformSystem") {}

	void SetWorlds(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT3>& positions,       
		const std::vector<DirectX::XMFLOAT3>& directions,      // (pitch,yaw,roll)
		const std::vector<DirectX::XMFLOAT3>& scales,
		Transform& transform);
};
#endif