// **********************************************************************************
// Filename:      TranformSystem.h
// Description:   Entity-Component-System (ECS) system for control 
//                transform data of entities
// 
// Created:       20.05.24
// **********************************************************************************

#pragma once

#include "../ECS_Common/ECS_Types.h"

#include "../ECS_Components/Transform.h"
#include "../ECS_Components/WorldMatrix.h"

#include <set>

class TransformSystem final
{
public:
	TransformSystem(Transform* pTransform, WorldMatrix* pWorld);

	void Serialize(const std::string& dataFilepath);
	void Deserialize(const std::string& dataFilepath);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs, 
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
		const std::vector<XMFLOAT3>& scales);

	void RemoveRecords(const std::vector<EntityID>& enttsIDs);

	void GetTransformDataOfEntt(
		const EntityID& enttID,
		XMFLOAT3& outPosition,
		XMFLOAT3& outDirection,
		XMFLOAT3& outScale);

	void GetWorldMatricesOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<DirectX::XMMATRIX>& outWorldMatrices);

	void SetupEnttTransformData(
		const EntityID& enttsIDs,
		const XMFLOAT3& position,
		const XMFLOAT3& direction,                    // (pitch,yaw,roll)
		const XMFLOAT3& scale);

	void SetupEnttsData(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& positions,       
		const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
		const std::vector<XMFLOAT3>& scales);

	const std::vector<EntityID>& GetEnttsIDsFromTransformComponent() const;
	void GetEnttsIDsFromWorldMatrixComponent(std::vector<EntityID>& outEnttsIDs);

	void PrintPositionByID(const EntityID& enttID);
	void PrintWorldMatrixByID(const EntityID& enttID);

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
