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
	Utils::FileWrite(fout, t.posAndUniformScale_);
	Utils::FileWrite(fout, t.dirQuats_);
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
	outEnttsIDs = pWorldMat_->ids_;
}

///////////////////////////////////////////////////////////

void TransformSystem::GetTransformDataOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<ptrdiff_t>& outDataIdxs,
	std::vector<XMFLOAT3>& outPositions,
	std::vector<XMVECTOR>& outDirQuats,      // direction quaternions
	std::vector<float>& outUniformScales)
{
	// get a component data by ID from the Transform component

	try
	{
		Transform& transform = *pTransform_;
		bool areThereEntts = true;

		const ptrdiff_t enttsCount = std::ssize(enttsIDs);
		outDataIdxs.reserve(enttsCount);
		outPositions.reserve(enttsCount);
		outDirQuats.reserve(enttsCount);
		outUniformScales.reserve(enttsCount);

		
		// get enttities data indices into arrays inside the Transform component
		for (const EntityID id : enttsIDs)
		{
			areThereEntts |= Utils::BinarySearch(transform.ids_, id);
			outDataIdxs.push_back(Utils::GetIdxOfVal_InSortedArr(transform.ids_, id));
		}

		// check if there are entities by such IDs
		ASSERT_TRUE(areThereEntts, "there is some entity which doesn't have the Transform component so we can't get its transform data");

		// get entities positions and uniform scales
		for (const ptrdiff_t idx : outDataIdxs)
		{
			outPositions.push_back({
				transform.posAndUniformScale_[idx].x,
				transform.posAndUniformScale_[idx].y,
				transform.posAndUniformScale_[idx].z });

			outUniformScales.push_back(transform.posAndUniformScale_[idx].w);
		}

		// get entities direction quaternions
		for (const ptrdiff_t idx : outDataIdxs)
			outDirQuats.emplace_back(transform.dirQuats_[idx]);
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
	// 
	// in:  array of entities IDs
	// out: array of world matrices related to these entities

	std::vector<ptrdiff_t> idxs;
	const WorldMatrix& comp = *pWorldMat_;
	const ptrdiff_t enttsCount = std::ssize(enttsIDs);

	idxs.reserve(enttsCount);
	outWorldMatrices.reserve(enttsCount);

	// get data idx of each entt ID
	for (const EntityID& id : enttsIDs)
	{
		if (Utils::BinarySearch(comp.ids_, id))
			idxs.push_back(Utils::GetIdxOfVal_InSortedArr(comp.ids_, id));
	}

	// check if we have the proper number of idxs
	ASSERT_TRUE(enttsCount == std::ssize(idxs), "we want to get a world matrix of some entity which doesn't have the WorldMatrix component");

	// get world matrices
	for (const ptrdiff_t idx : idxs)
		outWorldMatrices.emplace_back(comp.worlds_[idx]);
}

///////////////////////////////////////////////////////////

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
		dataIdxs.push_back(Utils::GetIdxOfVal_InSortedArr(comp.ids_, id));
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
	{

		DirectX::XMStoreFloat4(&comp.posAndUniformScale_[dataIdxs[idx]], newPositions[idx]);
		
	}

	// set new uniform scales by idxs
	for (ptrdiff_t scaleIdx = 0; ptrdiff_t dataIdx : dataIdxs)
	{
		comp.posAndUniformScale_[dataIdx].w = newUniformScales[scaleIdx++];
	}

	// the Transform component stores only normalized direction quaternions so just do it
	for (ptrdiff_t idx = 0; idx < std::ssize(dataIdxs); ++idx)
		t.dirQuats_[dataIdxs[idx]] = DirectX::XMQuaternionNormalize(newDirQuats[idx]);
}

#if 0
void TransformSystem::SetTransformDataByDataIdxs(
	const std::vector<ptrdiff_t>& dataIdxs,
	const std::vector<XMVECTOR>& posAndUniformScales,
	const std::vector<XMVECTOR>& dirQuats)     // direction quaternions
{
	// setup position, direction, scale for all the input entities by its data idxs
	// into arrays inside the Transform component;
	// 
	// NOTE: input data has XMVECTOR type so we have to convert it 
	//       into XMFLOAT3 for writing

	const ptrdiff_t enttsCount = std::ssize(dataIdxs);
	ASSERT_NOT_ZERO(enttsCount, "entities data idxs arr is empty");
	ASSERT_TRUE(enttsCount == posAndUniformScales.size(), "count of entities and positions are not equal");
	ASSERT_TRUE(enttsCount == dirQuats.size(), "count of entities and directions are not equal");
	
	std::vector<XMFLOAT4>& posAndScalesArr = pTransform_->posAndUniformScale_;
	std::vector<XMVECTOR>& dirQuatsArr = pTransform_->dirQuats_;
	UINT data_idx = 0;

	// set new position/uniform scale by idx
	for (const ptrdiff_t idx : dataIdxs)
		DirectX::XMStoreFloat4(&posAndScalesArr[idx], posAndUniformScales[data_idx++]);

	// set new direction quaternion by idx
	data_idx = 0;
	for (const ptrdiff_t idx : dataIdxs)
		dirQuatsArr[idx] = dirQuats[data_idx++];
}
#endif

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
		idxs.push_back(Utils::GetIdxOfVal_InSortedArr(comp.ids_, id));
	}

	// check if we have the proper idxs
	ASSERT_TRUE(areIdxsValid, "we want to set a new world matrix for some entity which doesn't have the WorldMatrix component");

	// store world matrices
	UINT data_idx = 0;
	for (const ptrdiff_t idx : idxs)
		comp.worlds_[idx] = newWorldMatrices[data_idx++];
}


// ********************************************************************************
//
//                            PRIVATE HELPERS
// 
// ********************************************************************************

void TransformSystem::AddRecordsToTransformComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMVECTOR>& dirQuats,      // direction quaternions
	const std::vector<float>& uniformScales)
{
	// store transformation data of entities into the Transform component

	Transform& component = *pTransform_;

	for (size_t idx = 0; idx < enttsIDs.size(); ++idx)
	{
		// check if there is no record with such entity ID
		if (!Utils::BinarySearch(component.ids_, enttsIDs[idx]))
		{
			// execute sorted insertion into the data arrays
			const ptrdiff_t insertAtPos = Utils::GetPosForID(component.ids_, enttsIDs[idx]);
		
			// NOTE: we build a single XMFLOAT4 from position and uniform scale
			Utils::InsertAtPos<EntityID>(component.ids_, insertAtPos, enttsIDs[idx]);
			Utils::InsertAtPos<XMFLOAT4>(component.posAndUniformScale_, insertAtPos, { positions[idx].x, positions[idx].y, positions[idx].z, uniformScales[idx]});

			// NOTE: we normalize the direction quaternion before storing
			Utils::InsertAtPos<XMVECTOR>(component.dirQuats_, insertAtPos, XMVector3Normalize(dirQuats[idx]));
		}
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::AddRecordsToWorldMatrixComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& positions,
	const std::vector<XMVECTOR>& dirQuats,      // direction quaternions
	const std::vector<float>& uniformScales)
{
	// compute and store world matrices into the WorldMatrix component
	
	WorldMatrix& comp = *pWorldMat_;
	const ptrdiff_t worldMatricesCount = std::ssize(positions);

	std::vector<XMMATRIX> worldMatrices;
	worldMatrices.reserve(worldMatricesCount);

	// compute world matrices
	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
	{
		worldMatrices.emplace_back(
			XMMatrixScaling(uniformScales[idx], uniformScales[idx], uniformScales[idx]) *
			XMMatrixRotationQuaternion(dirQuats[idx]) *
			XMMatrixTranslation(positions[idx].x, positions[idx].y, positions[idx].z)
		);
	}
		
	// store records ['entt_id' => 'world_matrix'] into the WorldMatrix componemt
	UINT data_idx = 0;
	
	for (const EntityID id : enttsIDs)
	{
		const ptrdiff_t insertAtIdx = Utils::GetPosForID(comp.ids_, id);

		Utils::InsertAtPos(comp.ids_, insertAtIdx, id);
		Utils::InsertAtPos(comp.worlds_, insertAtIdx, worldMatrices[data_idx++]);
	}
}

///////////////////////////////////////////////////////////

void TransformSystem::AddRecordsToWorldMatrixComponent(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT4>& posAndUniformScales,
	const std::vector<XMVECTOR>& dirQuats)     // direction quaternions
{
	// compute and store world matrices into the WorldMatrix component

	WorldMatrix& comp = *pWorldMat_;
	const ptrdiff_t worldMatricesCount = std::ssize(posAndUniformScales);

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
		scaleMatrices.emplace_back(DirectX::XMMatrixScaling(posAndUniformScales[idx].w, posAndUniformScales[idx].w, posAndUniformScales[idx].w));

	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		rotationMatrices.emplace_back(DirectX::XMMatrixRotationQuaternion(dirQuats[idx]));

	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		translationMatrices.emplace_back(DirectX::XMMatrixTranslation(posAndUniformScales[idx].x, posAndUniformScales[idx].y, posAndUniformScales[idx].z));

	// compute world matrices
	for (ptrdiff_t idx = 0; idx < worldMatricesCount; ++idx)
		worldMatrices.emplace_back(scaleMatrices[idx] * rotationMatrices[idx] * translationMatrices[idx]);

	// store records ['entt_id' => 'world_matrix'] into the WorldMatrix componemt
	UINT data_idx = 0;

	for (const EntityID id : enttsIDs)
	{
		const ptrdiff_t insertAtIdx = Utils::GetPosForID(comp.ids_, id);

		Utils::InsertAtPos(comp.ids_, insertAtIdx, id);
		Utils::InsertAtPos(comp.worlds_, insertAtIdx, worldMatrices[data_idx++]);
	}
}
