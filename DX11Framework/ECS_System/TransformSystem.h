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
#include "../ECS_Components/WorldMatrix.h"

#include <DirectXMath.h>
#include <vector>
#include <set>

typedef unsigned int UINT;

class TransformSystem
{
	using XMFLOAT3 = DirectX::XMFLOAT3;

public:
	TransformSystem(Transform* pTransform, WorldMatrix* pWorld);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs, 
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
		const std::vector<XMFLOAT3>& scales);

	void RemoveRecords(const std::vector<EntityID>& enttsIDs);

	void GetTransformDataOfEntity(
		const EntityID& enttID,
		XMFLOAT3& outPosition,
		XMFLOAT3& outDirection,
		XMFLOAT3& outScale);

	void SetupEnttData(
		const EntityID& enttsIDs,
		const XMFLOAT3& position,
		const XMFLOAT3& direction,                    // (pitch,yaw,roll)
		const XMFLOAT3& scale);

	void SetupEnttsData(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& positions,       
		const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
		const std::vector<XMFLOAT3>& scales);

	// for debug/unit-test purposes
	std::set<EntityID> GetEntitiesIDsSet() const;

private:
	void AddRecordsToTransformComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
		const std::vector<XMFLOAT3>& scales);

	void AddRecordsToWorldMatrixComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
		const std::vector<XMFLOAT3>& scales);

private:
	Transform* pTransform_ = nullptr;   // a ptr to the Transform component
	WorldMatrix* pWorldMat_ = nullptr;  // a ptr to the WorldMatrix component
};
