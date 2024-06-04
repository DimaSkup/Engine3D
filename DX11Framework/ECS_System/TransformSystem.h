// **********************************************************************************
// Filename:      TranformSystem.h
// Description:   Entity-Component-System (ECS) system for control 
//                transform data of entities
// 
// Created:       20.05.24
// **********************************************************************************

#pragma once

#include "../ECS_Entity/ECS_Types.h"
#include "../ECS_Components/Transform.h"
#include <DirectXMath.h>
#include <vector>
#include <set>

typedef unsigned int UINT;

class TransformSystem
{
public:
	TransformSystem(Transform* pTransform);

	void AddRecord(const EntityID& entityID);
	void RemoveRecord(const EntityID& entityID);

	void GetTransformDataOfEntity(
		const EntityID& entityID,
		DirectX::XMFLOAT3& outPosition,
		DirectX::XMFLOAT3& outDirection,
		DirectX::XMFLOAT3& outScale);

	void SetWorld(
		const EntityID& entityID,
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT3& direction,                    // (pitch,yaw,roll)
		const DirectX::XMFLOAT3& scale);

	void SetWorlds(
		const std::vector<EntityID>& entityIDs,
		const std::vector<DirectX::XMFLOAT3>& positions,       
		const std::vector<DirectX::XMFLOAT3>& directions,      // (pitch,yaw,roll)
		const std::vector<DirectX::XMFLOAT3>& scales);

	// for debug/unit-test purposes
	std::set<EntityID> GetEntitiesIDsSet() const;

private:
	Transform* pTransform_ = nullptr;   // ptr to the Transform component
};
