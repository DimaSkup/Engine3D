// **********************************************************************************
// Filename:      TranformSystem.cpp
// Description:   implementation of the TransformSystem's functional
// 
// Created:       20.05.24
// **********************************************************************************
#include "TransformSystem.h"
#include "../Common/LIB_Exception.h"
#include "../Common/Utils.h"
#include "../Common/log.h"

#include <stdexcept>
#include <algorithm>

using namespace DirectX;
using namespace ECS;
using namespace Utils;


TransformSystem::TransformSystem(
	Transform* pTransform,
	WorldMatrix* pWorld)
{
	ASSERT_NOT_NULLPTR(pTransform, "ptr to the Transform component == nullptr");
	ASSERT_NOT_NULLPTR(pWorld, "ptr to the WorldMatrix component == nullptr");

	pTransform_ = pTransform;
	pWorldMat_ = pWorld;
}

// ********************************************************************************
// 
//                PUBLIC SERIALIZATION / DESERIALIZATION API
// 
// ********************************************************************************

void TransformSystem::Serialize(std::ofstream& fout, u32& offset)
{
	// serialize all the data from the Transform component into the data file

	// store offset of this data block so we will use it later for deserialization
	offset = static_cast<u32>(fout.tellp()); 

	Transform& t = *pTransform_;
	const u32 dataBlockMarker = static_cast<u32>(t.type_);
	const size dataCount = std::ssize(t.ids_);

	// write serialized data into the file
	Utils::FileWrite(fout, &dataBlockMarker);
	Utils::FileWrite(fout, &dataCount);

	Utils::FileWrite(fout, t.ids_);
	Utils::FileWrite(fout, t.posAndUniformScale_);
	Utils::FileWrite(fout, t.dirQuats_);
}

///////////////////////////////////////////////////////////

void TransformSystem::Deserialize(std::ifstream& fin, const u32 offset)
{
	// deserialize all the data from the data file into the Transform component

	// read data starting from this offset
	fin.seekg(offset, std::ios_base::beg);

	// check if we read the proper data block
	u32 dataBlockMarker = 0;
	Utils::FileRead(fin, &dataBlockMarker);

	const bool isProperDataBlock = (dataBlockMarker == static_cast<u32>(ComponentType::TransformComponent));
	ASSERT_TRUE(isProperDataBlock, "read wrong data block during deserialization of the Transform component data from a file");

	// ------------------------------------------

	size dataCount = 0;
	Utils::FileRead(fin, &dataCount);

	Transform& t = *pTransform_;
	std::vector<EntityID>& ids = t.ids_;
	std::vector<XMFLOAT4>& pos = t.posAndUniformScale_;
	std::vector<XMVECTOR>& dir = t.dirQuats_;

	// prepare enough amount of memory for data
	ids.resize(dataCount);
	pos.resize(dataCount);
	dir.resize(dataCount);

	// read data from a file right into the component
	Utils::FileRead(fin, ids);
	Utils::FileRead(fin, pos);
	Utils::FileRead(fin, dir);

	// clear data of the component and build world matrices 
	// from deserialized component data
	pWorldMat_->ids_.clear();
	pWorldMat_->worlds_.clear();

	AddRecordsToWorldMatrixComponent(
		t.ids_, 
		t.posAndUniformScale_,
		t.dirQuats_);
}

///////////////////////////////////////////////////////////

void TransformSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMVECTOR>& dirQuats,      // direction quaternions
	const std::vector<float>& uniformScales)
{
	AddRecordsToTransformComponent(enttsIDs, positions, dirQuats, uniformScales);
	AddRecordsToWorldMatrixComponent(enttsIDs, positions, dirQuats, uniformScales);
}

///////////////////////////////////////////////////////////

void TransformSystem::RemoveRecords(const std::vector<EntityID>& enttsIDs)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}



// ********************************************************************************
// 
//                         PUBLIC GETTERS API
// 
// ********************************************************************************

void TransformSystem::GetTransformDataOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<ptrdiff_t>& outDataIdxs,
	std::vector<XMFLOAT3>& outPositions,
	std::vector<XMVECTOR>& outDirQuats,      // direction quaternions
	std::vector<float>& outUniformScales)
{
	// get a component data by ID from the Transform component
	//
	// in:     arr of entts IDs which data we will get
	// out: 1. arr of data idxs to each entt
	//      2. arr of positions
	//      3. arr of NORMALIZED direction quaternions
	//      4. arr of uniform scales

	try
	{
		Transform& comp = *pTransform_;
		bool areThereEntts = true;

		const ptrdiff_t enttsCount = std::ssize(enttsIDs);
		outDataIdxs.reserve(enttsCount);
		outPositions.reserve(enttsCount);
		outDirQuats.reserve(enttsCount);
		outUniformScales.reserve(enttsCount);

		// get enttities data indices into arrays inside the Transform component
		for (const EntityID id : enttsIDs)
		{
			areThereEntts |= Utils::BinarySearch(comp.ids_, id);
			outDataIdxs.push_back(Utils::GetIdxInSortedArr(comp.ids_, id));
		}

		// check if there are entities by such IDs
		ASSERT_TRUE(areThereEntts, "there is some entity which doesn't have the Transform component so we can't get its transform data");

		GetTransformDataByDataIdxs(outDataIdxs, outPositions, outDirQuats, outUniformScales);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, false);
		THROW_ERROR("can't find transform data by some of entities IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::GetTransformDataByDataIdxs(
	std::vector<ptrdiff_t>& dataIdxs,
	std::vector<XMFLOAT3>& outPositions,
	std::vector<XMVECTOR>& outDirQuats,      // direction quaternions
	std::vector<float>& outUniformScales)
{
	Transform& comp = *pTransform_;

	// get entities positions
	for (const ptrdiff_t idx : dataIdxs)
	{
		const DirectX::XMFLOAT4& pos = comp.posAndUniformScale_[idx];
		outPositions.emplace_back(pos.x, pos.y, pos.z);
	}

	// get uniform scales (each value is stored in the w-component of XMFLOAT4)
	for (const ptrdiff_t idx : dataIdxs)
		outUniformScales.push_back(comp.posAndUniformScale_[idx].w);

	// get entities direction quaternions
	for (const ptrdiff_t idx : dataIdxs)
		outDirQuats.push_back(comp.dirQuats_[idx]);
}

///////////////////////////////////////////////////////////

void TransformSystem::GetWorldMatricesOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<DirectX::XMMATRIX>& outWorldMatrices)
{
	// get world matrices of entities by its IDs from the WorldMatrix component
	// 
	// in:  array of entities IDs
	// out: array of world matrices related to these entities

	std::vector<ptrdiff_t> idxs;
	const WorldMatrix& comp = *pWorldMat_;
	const ptrdiff_t enttsCount = std::ssize(enttsIDs);
	bool areIdxsValid = true;

	idxs.reserve(enttsCount);
	outWorldMatrices.reserve(enttsCount);

	// get data idx by each entt ID
	for (const EntityID& id : enttsIDs)
	{
		areIdxsValid |= Utils::BinarySearch(comp.ids_, id);
		idxs.push_back(Utils::GetIdxInSortedArr(comp.ids_, id));
	}

	// check if we have the proper data idxs
	ASSERT_TRUE(areIdxsValid, "we want to get a world matrix of some entity which doesn't have the WorldMatrix component");

	GetWorldMatricesByDataIdxs(idxs, outWorldMatrices);
}

///////////////////////////////////////////////////////////

void TransformSystem::GetWorldMatricesByDataIdxs(
	const std::vector<ptrdiff_t>& dataIdxs,
	std::vector<XMMATRIX>& outWorldMatrices)
{
	// get world matrices by input data idxs

	const WorldMatrix& comp = *pWorldMat_;

	for (const ptrdiff_t idx : dataIdxs)
		outWorldMatrices.emplace_back(comp.worlds_[idx]);
}




// ********************************************************************************
// 
//                         PUBLIC SETTERS API
// 
// ********************************************************************************

void TransformSystem::SetTransformDataByIDs(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMVECTOR>& newPositions,
	const std::vector<XMVECTOR>& newDirQuats,      // direction quaternions
	const std::vector<float>& newUniformScales)
{
	// setup position, direction, scale for all the input entities by its IDs;
	// NOTE: input data has XMFLOAT3 type so we can write it directly

	const ptrdiff_t enttsCount = std::ssize(enttsIDs);
	ASSERT_NOT_ZERO(enttsCount, "entities IDs arr is empty");
	ASSERT_TRUE(enttsCount == newPositions.size(), "array size of entts IDs and positions are not equal");
	ASSERT_TRUE(enttsCount == newDirQuats.size(), "array size of entts IDs and directions are not equal");
	ASSERT_TRUE(enttsCount == newUniformScales.size(), "array size of entts IDs and scales are not equal");

	Transform& comp = *pTransform_;
	std::vector<ptrdiff_t> dataIdxs;
	bool areThereEntts = true;

	dataIdxs.reserve(std::ssize(enttsIDs));

	// get enttities data indices into arrays inside the Transform component
	for (const EntityID id : enttsIDs)
	{
		areThereEntts |= Utils::BinarySearch(comp.ids_, id);
		dataIdxs.push_back(Utils::GetIdxInSortedArr(comp.ids_, id));
	}

	// check if there are entities by such IDs
	ASSERT_TRUE(areThereEntts, "there is some entity which doesn't have the Transform component so we can't set transform data for it");

	SetTransformDataByDataIdxs(dataIdxs, newPositions, newDirQuats, newUniformScales);
}

///////////////////////////////////////////////////////////

void TransformSystem::SetTransformDataByDataIdxs(
	const std::vector<ptrdiff_t>& dataIdxs,
	const std::vector<XMVECTOR>& newPositions,
	const std::vector<XMVECTOR>& newDirQuats,
	const std::vector<float>& newUniformScales)
{
	const ptrdiff_t idxsCount = std::ssize(dataIdxs);
	ASSERT_NOT_ZERO(idxsCount, "data idxs arr is empty");
	ASSERT_TRUE(idxsCount == newPositions.size(), "arr of idxs and arr of positions are not equal");
	ASSERT_TRUE(idxsCount == newDirQuats.size(), "arr of idxs and arr of directions are not equal");
	ASSERT_TRUE(idxsCount == newUniformScales.size(), "arr of idxs and arr of scales are not equal");

	Transform& comp = *pTransform_;

	// set new positions by idxs
	for (ptrdiff_t posIdx = 0; ptrdiff_t dataIdx : dataIdxs)
		DirectX::XMStoreFloat4(&comp.posAndUniformScale_[dataIdx], newPositions[posIdx]);

	// set new uniform scales by idxs
	for (ptrdiff_t scaleIdx = 0; ptrdiff_t dataIdx : dataIdxs)
		comp.posAndUniformScale_[dataIdx].w = newUniformScales[scaleIdx++];

	// the Transform component stores only normalized direction quaternions so just do it
	for (ptrdiff_t quatIdx = 0; ptrdiff_t dataIdx : dataIdxs)
		comp.dirQuats_[dataIdx] = DirectX::XMQuaternionNormalize(newDirQuats[quatIdx]);
}

///////////////////////////////////////////////////////////

void TransformSystem::SetWorldMatricesByIDs(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMMATRIX>& newWorldMatrices)
{
	// set new world matrix for each input entity by its ID

	const ptrdiff_t enttsCount = (std::ssize(enttsIDs));
	assert((enttsCount > 0) && "entities IDs arr is empty");
	assert((enttsIDs.size() == newWorldMatrices.size()) && "array size of entts IDs and new world matrices are not equal");

	WorldMatrix& comp = *pWorldMat_;
	std::vector<ptrdiff_t> idxs;
	bool areIdxsValid = true;

	idxs.reserve(std::ssize(enttsIDs));

	// get data idx of each entt ID
	for (const EntityID& id : enttsIDs)
	{
		areIdxsValid |= Utils::BinarySearch(comp.ids_, id);   // if some entt doesn't have the component its data idx will be invalid
		idxs.push_back(Utils::GetIdxInSortedArr(comp.ids_, id));
	}

	// check if we have the proper idxs
	ASSERT_TRUE(areIdxsValid, "we want to set a new world matrix for some entity which doesn't have the WorldMatrix component");

	SetWorldMatricesByDataIdxs(idxs, newWorldMatrices);
}

///////////////////////////////////////////////////////////

void TransformSystem::SetWorldMatricesByDataIdxs(
	const std::vector<ptrdiff_t>& dataIdxs,
	const std::vector<XMMATRIX>& newWorldMatrices)
{
	// store world matrices by input data idxs

	WorldMatrix& comp = *pWorldMat_;

	ASSERT_TRUE(std::ssize(comp.worlds_) >= std::ssize(newWorldMatrices), "count of new matrices can't be bigger than the number of matrices in the WorldMatrix component");

	for (ptrdiff_t newMatIdx = 0; const ptrdiff_t idx : dataIdxs)
		comp.worlds_[idx] = newWorldMatrices[newMatIdx++];
}



// ********************************************************************************
//
//                            PRIVATE HELPERS
// 
// ********************************************************************************

void TransformSystem::AddRecordsToTransformComponent(
	const std::vector<EntityID>& ids,
	const std::vector<XMFLOAT3>& pos,
	const std::vector<XMVECTOR>& dirQuats,      // direction quaternions
	const std::vector<float>& uniformScales)
{
	// store transformation data of entities into the Transform component

	Transform& component = *pTransform_;

	for (u32 idx = 0; const EntityID& id : ids)
	{
		// if there is no record with such entity ID
		// we execute sorted insertion into the data arrays
		if (!BinarySearch(component.ids_, id))
		{
			const ptrdiff_t insertAt = GetPosForID(component.ids_, id);
			const XMFLOAT4 transAndUniScale = { pos[idx].x, pos[idx].y, pos[idx].z, uniformScales[idx] };
		
			// NOTE: we build a single XMFLOAT4 from position and uniform scale
			// NOTE: we normalize the direction quaternion before storing
			InsertAtPos<EntityID>(component.ids_, insertAt, id);
			InsertAtPos<XMFLOAT4>(component.posAndUniformScale_, insertAt, transAndUniScale);
			InsertAtPos<XMVECTOR>(component.dirQuats_, insertAt, XMVector3Normalize(dirQuats[idx]));
		}

		++idx;
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::AddRecordsToWorldMatrixComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& pos,
	const std::vector<XMVECTOR>& dirQuats,      // direction quaternions
	const std::vector<float>& uniformScales)
{
	// compute and store world matrices into the WorldMatrix component
	
	WorldMatrix& comp = *pWorldMat_;
	const ptrdiff_t worldMatricesCount = std::ssize(pos);

	std::vector<XMMATRIX> worldMatrices;
	worldMatrices.reserve(worldMatricesCount);

	// compute world matrices
	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
	{
		worldMatrices.emplace_back(
			XMMatrixScaling(uniformScales[idx], uniformScales[idx], uniformScales[idx]) *
			XMMatrixRotationQuaternion(dirQuats[idx]) *
			XMMatrixTranslation(pos[idx].x, pos[idx].y, pos[idx].z)
		);
	}
		
	// store records ['entt_id' => 'world_matrix'] into the WorldMatrix componemt	
	for (u32 data_idx = 0; const EntityID id : enttsIDs)
	{
		const ptrdiff_t insertAtIdx = Utils::GetPosForID(comp.ids_, id);

		Utils::InsertAtPos(comp.ids_, insertAtIdx, id);
		Utils::InsertAtPos(comp.worlds_, insertAtIdx, worldMatrices[data_idx++]);
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::AddRecordsToWorldMatrixComponent(
	const std::vector<EntityID>& ids,
	const std::vector<XMFLOAT4>& posAndUniScales,
	const std::vector<XMVECTOR>& dirQuats)     // direction quaternions
{
	// compute and store world matrices into the WorldMatrix component

	WorldMatrix& comp = *pWorldMat_;
	const size worldMatricesCount = std::ssize(posAndUniScales);

	std::vector<XMMATRIX> scaleMatrices;
	std::vector<XMMATRIX> rotationMatrices;
	std::vector<XMMATRIX> translationMatrices;
	std::vector<XMMATRIX> worldMatrices;
	std::vector<ptrdiff_t> dataIdxs;

	scaleMatrices.reserve(worldMatricesCount);
	rotationMatrices.reserve(worldMatricesCount);
	translationMatrices.reserve(worldMatricesCount);
	worldMatrices.reserve(worldMatricesCount);
	dataIdxs.reserve(worldMatricesCount);

	// 1. compute matrices for uniform scaling (we use only w component of XMFLOAT4)
	// 2. compute rotation matrices
	// 3. compute translation matrices (we use x,y,z of XMFLOAT4)
	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		scaleMatrices.emplace_back(DirectX::XMMatrixScaling(posAndUniScales[idx].w, posAndUniScales[idx].w, posAndUniScales[idx].w));

	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		rotationMatrices.emplace_back(DirectX::XMMatrixRotationQuaternion(dirQuats[idx]));

	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		translationMatrices.emplace_back(DirectX::XMMatrixTranslation(posAndUniScales[idx].x, posAndUniScales[idx].y, posAndUniScales[idx].z));

	// compute world matrices
	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		worldMatrices.emplace_back(scaleMatrices[idx] * rotationMatrices[idx] * translationMatrices[idx]);

	// store records ['entt_id' => 'world_matrix'] into the WorldMatrix componemt
	for (u32 data_idx = 0; const EntityID id : ids)
	{
		const ptrdiff_t insertAt = Utils::GetPosForID(comp.ids_, id);

		Utils::InsertAtPos(comp.ids_, insertAt, id);
		Utils::InsertAtPos(comp.worlds_, insertAt, worldMatrices[data_idx++]);
	}
}
