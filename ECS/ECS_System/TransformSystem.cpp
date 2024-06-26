// **********************************************************************************
// Filename:      TranformSystem.cpp
// Description:   implementation of the TransformSystem's functional
// 
// Created:       20.05.24
// **********************************************************************************
#include "TransformSystem.h"
#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Utils.h"
#include "../ECS_Common/log.h"

#include <stdexcept>
#include <algorithm>

using namespace DirectX;
using namespace ECS;


TransformSystem::TransformSystem(
	Transform* pTransform,
	WorldMatrix* pWorld)
{
	ASSERT_NOT_NULLPTR(pTransform, "ptr to the Transform component == nullptr");
	ASSERT_NOT_NULLPTR(pWorld, "ptr to the WorldMatrix component == nullptr");

	pTransform_ = pTransform;
	pWorldMat_ = pWorld;
}

///////////////////////////////////////////////////////////

void TransformSystem::Serialize(std::ofstream& fout, size_t& offset)
{
	// serialize all the data from the Transform component into the data file

	// store offset of this data block so we will use it later for deserialization
	offset = static_cast<size_t>(fout.tellp()); 

	Transform& t = *pTransform_;
	const size_t dataBlockMarker = static_cast<size_t>(t.type_);
	const size_t dataCount = t.ids_.size();

	// write serialized data into the file
	Utils::FileWrite(fout, &dataBlockMarker);
	Utils::FileWrite(fout, &dataCount);

	Utils::FileWrite(fout, t.ids_);
	Utils::FileWrite(fout, t.positions_);
	Utils::FileWrite(fout, t.directions_);
	Utils::FileWrite(fout, t.scales_);
}

///////////////////////////////////////////////////////////

void TransformSystem::Deserialize(std::ifstream& fin, const size_t offset)
{
	// deserialize all the data from the data file into the Transform component

	// read data starting from this offset
	fin.seekg(offset, std::ios_base::beg);

	// check if we read the proper data block
	size_t dataBlockMarker = 0;
	Utils::FileRead(fin, &dataBlockMarker);

	const bool isProperDataBlock = (dataBlockMarker == static_cast<size_t>(ComponentType::TransformComponent));
	ASSERT_TRUE(isProperDataBlock, "read wrong data block during deserialization of the Transform component data from a file");

	// ------------------------------------------

	size_t dataCount = 0;
	Utils::FileRead(fin, &dataCount);

	Transform& t = *pTransform_;
	std::vector<EntityID>& ids = t.ids_;
	std::vector<XMFLOAT3>& pos = t.positions_;
	std::vector<XMFLOAT3>& dir = t.directions_;
	std::vector<XMFLOAT3>& scales = t.scales_;

	// prepare enough amount of memory for data
	ids.resize(dataCount);
	pos.resize(dataCount);
	dir.resize(dataCount);
	scales.resize(dataCount);

	Utils::FileRead(fin, ids);
	Utils::FileRead(fin, pos);
	Utils::FileRead(fin, dir);
	Utils::FileRead(fin, scales);

	// clear data of the component and build world matrices 
	// from deserialized component data
	pWorldMat_->worlds_.clear();
	AddRecordsToWorldMatrixComponent(t.ids_, t.positions_, t.directions_, t.scales_);
}

///////////////////////////////////////////////////////////

void TransformSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
	const std::vector<XMFLOAT3>& scales)
{
	AddRecordsToTransformComponent(enttsIDs, positions, directions, scales);
	AddRecordsToWorldMatrixComponent(enttsIDs, positions, directions, scales);
}

///////////////////////////////////////////////////////////

void TransformSystem::RemoveRecords(const std::vector<EntityID>& enttsIDs)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}

///////////////////////////////////////////////////////////

const std::vector<EntityID>& TransformSystem::GetAllEnttsIDsFromTransformComponent() const
{
	// get a bunch of all the entities IDs which the component component has
	// return: a reference to the array of entities IDs
	return pTransform_->ids_;
}

///////////////////////////////////////////////////////////

void TransformSystem::GetAllEnttsIDsFromWorldMatrixComponent(
	std::vector<EntityID>& outEnttsIDs)
{
	// get all the entities IDs from the WorldMatrix component
	outEnttsIDs.reserve(pWorldMat_->worlds_.size());

	for (const auto& it : pWorldMat_->worlds_)
		outEnttsIDs.push_back(it.first);
}

///////////////////////////////////////////////////////////

void TransformSystem::GetTransformDataOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<ptrdiff_t>& outDataIdxs,
	std::vector<XMFLOAT3>& outPositions,
	std::vector<XMFLOAT3>& outDirections,
	std::vector<XMFLOAT3>& outScales)
{
	// get a component data by ID from the Transform component

	try
	{
		Transform& transform = *pTransform_;

		const ptrdiff_t enttsCount = std::ssize(enttsIDs);
		outDataIdxs.reserve(enttsCount);
		outPositions.reserve(enttsCount);
		outDirections.reserve(enttsCount);
		outScales.reserve(enttsCount);

		// get enttities data indices into arrays inside the Transform component
		for (const EntityID id : enttsIDs)
			outDataIdxs.push_back(Utils::GetIdxOfID(transform.ids_, id));

		// get entities positions
		for (const ptrdiff_t idx : outDataIdxs)
			outPositions.push_back(transform.positions_[idx]);

		// get entities directions
		for (const ptrdiff_t idx : outDataIdxs)
			outDirections.push_back(transform.directions_[idx]);

		// get entities scales
		for (const ptrdiff_t idx : outDataIdxs)
			outScales.push_back(transform.scales_[idx]);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't find transform data by some of entities IDs: " + Utils::JoinArrIntoStr<EntityID>(enttsIDs));
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::GetWorldMatricesByIDs(
	const std::vector<EntityID>& enttsIDs,
	std::vector<DirectX::XMMATRIX>& outWorldMatrices)
{
	// get world matrices of entities by its IDs from the WorldMatrix component
	// in:  array of entities IDs
	// out: array of world matrices
	outWorldMatrices.reserve(std::ssize(enttsIDs));

	for (const EntityID& enttID : enttsIDs)
		outWorldMatrices.push_back(pWorldMat_->worlds_[enttID]);
}

///////////////////////////////////////////////////////////

void TransformSystem::SetTransformDataByIDs(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
	const std::vector<XMFLOAT3>& scales)
{
	// setup position, direction, scale for all the input entities by its IDs;
	// NOTE: input data has XMFLOAT3 type so we can write it directly

	const ptrdiff_t enttsCount = std::ssize(enttsIDs);
	ASSERT_NOT_ZERO(enttsCount, "entities IDs arr is empty");
	ASSERT_TRUE(enttsCount == positions.size(), "array size of entts IDs and positions are not equal");
	ASSERT_TRUE(enttsCount == directions.size(), "array size of entts IDs and directions are not equal");
	ASSERT_TRUE(enttsCount == scales.size(), "array size of entts IDs and scales are not equal");

	Transform& t = *pTransform_;
	std::vector<ptrdiff_t> dataIdxs;
	dataIdxs.reserve(std::ssize(enttsIDs));

	// get enttities data indices into arrays inside the Transform component
	for (const EntityID id : enttsIDs)
		dataIdxs.push_back(Utils::GetIdxOfID(t.ids_, id));

	// set new positions/directions/scales by idxs
	for (size_t idx = 0; idx < dataIdxs.size(); ++idx)
		t.positions_[dataIdxs[idx]] = positions[idx];

	for (size_t idx = 0; idx < dataIdxs.size(); ++idx)
		t.directions_[dataIdxs[idx]] = directions[idx];
	
	for (size_t idx = 0; idx < dataIdxs.size(); ++idx)
		t.scales_[dataIdxs[idx]] = scales[idx];
}

///////////////////////////////////////////////////////////

void TransformSystem::SetTransformDataByDataIdxs(
	const std::vector<ptrdiff_t>& dataIdxs,
	std::vector<XMVECTOR>& positions,
	std::vector<XMVECTOR>& directions,      // (pitch,yaw,roll)
	std::vector<XMVECTOR>& scales)
{
	// setup position, direction, scale for all the input entities by its data idxs
	// into arrays inside the Transform component;
	// 
	// NOTE: input data has XMVECTOR type so we have to convert it 
	//       into XMFLOAT3 for writing

	const ptrdiff_t enttsCount = std::ssize(dataIdxs);
	ASSERT_NOT_ZERO(enttsCount, "entities data idxs arr is empty");
	ASSERT_TRUE(enttsCount == positions.size(), "count of entities and positions are not equal");
	ASSERT_TRUE(enttsCount == directions.size(), "count of entities and directions are not equal");
	ASSERT_TRUE(enttsCount == scales.size(), "count of entities and scales are not equal");

	std::vector<XMFLOAT3>& posArr = pTransform_->positions_;
	std::vector<XMFLOAT3>& dirArr = pTransform_->directions_;
	std::vector<XMFLOAT3>& scalesArr = pTransform_->scales_;
	UINT data_idx = 0;

	// set new positions/directions/scales by idxs
	for (const ptrdiff_t idx : dataIdxs)
		XMStoreFloat3(&posArr[idx], std::move(positions[data_idx++]));

	data_idx = 0;
	for (size_t idx = 0; idx < dataIdxs.size(); ++idx)
		XMStoreFloat3(&dirArr[idx], std::move(directions[data_idx++]));

	data_idx = 0;
	for (size_t idx = 0; idx < dataIdxs.size(); ++idx)
		XMStoreFloat3(&scalesArr[idx], std::move(scales[data_idx++]));
}

///////////////////////////////////////////////////////////

void TransformSystem::SetWorldMatricesByIDs(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMMATRIX>& newWorldMatrices)
{
	assert((std::ssize(enttsIDs) > 0) && "entities IDs arr is empty");
	assert((enttsIDs.size() == newWorldMatrices.size()) && "array size of entts IDs and new world matrices are not equal");

	for (size_t idx = 0; idx < enttsIDs.size(); ++idx)
		pWorldMat_->worlds_[enttsIDs[idx]] = newWorldMatrices[idx];
}

// ********************************************************************************
//
//                            PRIVATE HELPERS
// 
// ********************************************************************************

void TransformSystem::AddRecordsToTransformComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
	const std::vector<XMFLOAT3>& scales)
{
	// store transformation data of entities into the Transform component

	Transform& component = *pTransform_;

	for (size_t idx = 0; idx < enttsIDs.size(); ++idx)
	{
		// check if there is no record with such entity ID
		if (!std::binary_search(component.ids_.begin(), component.ids_.end(), enttsIDs[idx]))
		{
			// execute sorted insertion into the data arrays
			const ptrdiff_t insertAtPos = Utils::GetPosForID(component.ids_, enttsIDs[idx]);
		
			Utils::InsertAtPos<EntityID>(component.ids_, insertAtPos, enttsIDs[idx]);
			Utils::InsertAtPos<XMFLOAT3>(component.positions_, insertAtPos, positions[idx]);
			Utils::InsertAtPos<XMFLOAT3>(component.directions_, insertAtPos, directions[idx]);
			Utils::InsertAtPos<XMFLOAT3>(component.scales_, insertAtPos, scales[idx]);
		}
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::AddRecordsToWorldMatrixComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMFLOAT3>& directions,      // (pitch,yaw,roll)
	const std::vector<XMFLOAT3>& scales)
{
	// compute and store world matrices into the WorldMatrix component
	
	const ptrdiff_t worldMatricesCount = std::ssize(positions);

	std::vector<XMMATRIX> worldMatrices;
	worldMatrices.reserve(worldMatricesCount);

	// compute world matrices
	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		worldMatrices.push_back(
			XMMatrixScaling(scales[idx].x, scales[idx].y, scales[idx].z) *
			XMMatrixRotationRollPitchYaw(directions[idx].x, directions[idx].y, directions[idx].z) *
			XMMatrixTranslation(positions[idx].x, positions[idx].y, positions[idx].z)
		);

	// store world matrices
	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		pWorldMat_->worlds_.try_emplace(enttsIDs[idx], std::move(worldMatrices[idx]));
}
