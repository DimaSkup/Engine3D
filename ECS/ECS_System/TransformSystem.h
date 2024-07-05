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
	~TransformSystem() {}

	void Serialize(std::ofstream& fout, size_t& offset);
	void Deserialize(std::ifstream& fin, const size_t offset);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs, 
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMVECTOR>& dirQuats,      // direction quaternions
		const std::vector<float>& uniformScales);

	void RemoveRecords(const std::vector<EntityID>& enttsIDs);


	// 
	// PUBLIC GETTERS
	// 

	void GetTransformDataOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<ptrdiff_t>& outDataIdxs,
		std::vector<XMFLOAT3>& outPositions,
		std::vector<XMVECTOR>& outDirQuats,      // direction quaternions
		std::vector<float>& outUniformScales);

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
		const std::vector<XMVECTOR>& positions,
		const std::vector<XMVECTOR>& dirQuats,      // direction quaternions
		const std::vector<float>& uniformScales);

	void SetTransformDataByDataIdxs(
		const std::vector<ptrdiff_t>& dataIdxs,
		const std::vector<XMVECTOR>& newPositions,
		const std::vector<XMVECTOR>& newDirQuats,
		const std::vector<float>& newUniformScales);
#if 0
	void SetTransformDataByDataIdxs(
		const std::vector<ptrdiff_t>& dataIdxs,
		const std::vector<XMVECTOR>& posAndUniformScales,
		const std::vector<XMVECTOR>& dirQuats);     // direction quaternions
#endif

	void SetWorldMatricesByIDs(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMMATRIX>& newWorldMatrices);

private:
	void AddRecordsToTransformComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMVECTOR>& dirQuats,      // direction quaternions
		const std::vector<float>& uniformScales);

	void AddRecordsToWorldMatrixComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMVECTOR>& dirQuats,      // direction quaternions
		const std::vector<float>& uniformScales);

	void AddRecordsToWorldMatrixComponent(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMFLOAT4>& posAndUniformScales,
		const std::vector<XMVECTOR>& dirQuats);     // direction quaternions



private:
	Transform* pTransform_ = nullptr;   // a ptr to the Transform component
	WorldMatrix* pWorldMat_ = nullptr;  // a ptr to the WorldMatrix component
};
