// **********************************************************************************
// Filename:      TranformSystem.h
// Description:   Entity-Component-System (ECS) system for handling 
//                transform data of entities
// 
// Created:       20.05.24
// **********************************************************************************
#pragma once

#include "../Components/Transform.h"
#include "../Components/WorldMatrix.h"

#include <set>
#include <fstream>

namespace ECS
{

class TransformSystem final
{
public:
	TransformSystem(Transform* pTransform, WorldMatrix* pWorld);
	~TransformSystem() {}

	// public serialization / deserialization API
	void Serialize(std::ofstream& fout, u32& offset);
	void Deserialize(std::ifstream& fin, const u32 offset);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs, 
		const std::vector<XMFLOAT3>& positions,
		const std::vector<XMVECTOR>& dirQuats,      // direction quaternions
		const std::vector<float>& uniformScales);

	void RemoveRecords(const std::vector<EntityID>& enttsIDs);


	// -------------------------------------------------------
	// PUBLIC GETTERS API
	
	void GetTransformDataOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<ptrdiff_t>& outDataIdxs,
		std::vector<XMFLOAT3>& outPositions,
		std::vector<XMVECTOR>& outDirQuats,      // direction quaternions
		std::vector<float>& outUniformScales);

	void GetTransformDataByDataIdxs(
		std::vector<ptrdiff_t>& dataIdxs,
		std::vector<XMFLOAT3>& outPositions,
		std::vector<XMVECTOR>& outDirQuats,      // direction quaternions
		std::vector<float>& outUniformScales);

	DirectX::XMMATRIX GetWorldMatrixOfEntt(const EntityID id);

	void GetWorldMatricesOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<DirectX::XMMATRIX>& outWorldMatrices);

	void GetWorldMatricesByDataIdxs(
		const std::vector<ptrdiff_t>& dataIdxs,
		std::vector<XMMATRIX>& outWorldMatrices);

	// inline getters
	inline const std::vector<EntityID>& GetAllEnttsIDsFromTransformComponent() const { return pTransform_->ids_; }
	inline void GetAllEnttsIDsFromWorldMatrixComponent(std::vector<EntityID>& outEnttsIDs) { outEnttsIDs = pWorldMat_->ids_; }


	// -------------------------------------------------------
	// PUBLIC SETTERS API

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

	void SetWorldMatricesByIDs(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<XMMATRIX>& newWorldMatrices);

	void SetWorldMatricesByDataIdxs(
		const std::vector<ptrdiff_t>& dataIdxs,
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

}