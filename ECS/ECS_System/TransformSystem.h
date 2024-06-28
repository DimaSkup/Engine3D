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
#include <fstream>


class TransformSystem final
{
public:
	TransformSystem(Transform* pTransform, WorldMatrix* pWorld);

	void Serialize(std::ofstream& fout, size_t& offset);
	void Deserialize(std::ifstream& fin, const size_t offset);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs, 
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
		const std::vector<XMFLOAT3>& scales);

	void RemoveRecords(const std::vector<EntityID>& enttsIDs);


	// 
	// PUBLIC GETTERS
	// 

	void GetTransformDataOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<ptrdiff_t>& outDataIdxs,
		std::vector<XMFLOAT3>& outPositions,
		std::vector<XMFLOAT3>& outDirections,
		std::vector<XMFLOAT3>& outScales);

	void GetWorldMatricesByIDs(
		const std::vector<EntityID>& enttsIDs,
		std::vector<DirectX::XMMATRIX>& outWorldMatrices);

	const std::vector<EntityID>& GetAllEnttsIDsFromTransformComponent() const;
	void GetAllEnttsIDsFromWorldMatrixComponent(std::vector<EntityID>& outEnttsIDs);


	// 
	// PUBLIC SETTERS
	// 
	void SetTransformDataByIDs(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
		const std::vector<XMFLOAT3>& scales);

	void SetTransformDataByDataIdxs(
		const std::vector<ptrdiff_t>& dataIdxs,
		std::vector<XMVECTOR>& positions,
		std::vector<XMVECTOR>& directions,      // (pitch,yaw,roll)
		std::vector<XMVECTOR>& scales);

	void SetWorldMatricesByIDs(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMMATRIX>& newWorldMatrices);

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
