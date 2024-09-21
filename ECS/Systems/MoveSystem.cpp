// **********************************************************************************
// Filename:      MoveSystem.h
// Description:   implementation of the MoveSystem's functional
// 
// Created:       23.05.24
// **********************************************************************************
#include "MoveSystem.h"

#include "../Common/LIB_Exception.h"
#include "../Common/log.h"
#include "../Common/Utils.h"
#include "./Helpers/MoveSystemUpdateHelpers.h"

#include <stdexcept>
#include <fstream>

using namespace Utils;

namespace ECS
{
 

MoveSystem::MoveSystem(
	Transform* pTransformComponent,
	WorldMatrix* pWorldMatrixComponent,
	Movement* pMoveComponent)
{
	Assert::NotNullptr(pTransformComponent, "ptr to the Transform component == nullptr");
	Assert::NotNullptr(pWorldMatrixComponent, "ptr to the WorldMatrix component == nullptr");
	Assert::NotNullptr(pMoveComponent, "ptr to the Movement component == nullptr");

	pTransformComponent_ = pTransformComponent;
	pWorldMatComponent_ = pWorldMatrixComponent;
	pMoveComponent_ = pMoveComponent;
}


// ********************************************************************************
// 
//                PUBLIC SERIALIZATION / DESERIALIZATION API
// 
// ********************************************************************************

void MoveSystem::Serialize(std::ofstream& fout, u32& offset)
{
	// serialize all the data from the Movement component into the data file

	// store offset of this data block so we will use it later for deserialization
	offset = static_cast<u32>(fout.tellp());
	
	const Movement& move = *pMoveComponent_;	
	const u32 dataBlockMarker = static_cast<u32>(move.type_);
	const u32 dataCount = static_cast<u32>(std::ssize(move.ids_));

	// write movement data into the file
	Utils::FileWrite(fout, &dataBlockMarker);
	Utils::FileWrite(fout, &dataCount);

	Utils::FileWrite(fout, move.ids_);
	Utils::FileWrite(fout, move.translationAndUniScales_);
	Utils::FileWrite(fout, move.rotationQuats_);
}

///////////////////////////////////////////////////////////

void MoveSystem::Deserialize(std::ifstream& fin, const u32 offset)
{
	// deserialize the data from the data file into the Movement component

	// read data starting from this offset
	fin.seekg(offset, std::ios_base::beg);

	// check if we read the proper data block
	u32 dataBlockMarker = 0;
	Utils::FileRead(fin, &dataBlockMarker);

	const bool isProperDataBlock = (dataBlockMarker == static_cast<u32>(ComponentType::MoveComponent));
	Assert::True(isProperDataBlock, "read wrong data during deserialization of the Movement component data");

	// ------------------------------------------

	u32 dataCount = 0;
	Movement& component = *pMoveComponent_;

	std::vector<EntityID>& ids = component.ids_;
	std::vector<XMFLOAT4>& transAndUniScales = component.translationAndUniScales_;
	std::vector<XMVECTOR>& rotQuats = component.rotationQuats_;

	// read in how much data will we have
	Utils::FileRead(fin, &dataCount);

	// prepare enough amount of memory for data
	ids.resize(dataCount);
	transAndUniScales.resize(dataCount);
	rotQuats.resize(dataCount);

	// read data from a file right into the component
	Utils::FileRead(fin, ids);
	Utils::FileRead(fin, transAndUniScales);
	Utils::FileRead(fin, rotQuats);
}



// ********************************************************************************
// 
//                              PUBLIC UPDATING API
// 
// ********************************************************************************

void MoveSystem::UpdateAllMoves(
	const float deltaTime,
	TransformSystem& transformSys)
{
	const std::vector<EntityID>& enttsToMove = pMoveComponent_->ids_;

	// if we don't have any entities to move we just go out
	if (enttsToMove.size() == 0)
		return;

	try
	{
		Transform& transform = *pTransformComponent_;
		Movement& movement = *pMoveComponent_;

		std::vector<XMFLOAT3> positions;
		std::vector<XMVECTOR> dirQuats;
		std::vector<float> uniformScales;

		std::vector<XMMATRIX> worldMatricesToUpdate;
		std::vector<ptrdiff_t> transformDataIdxs;

		// current transform data of entities as XMVECTORs
		std::vector<XMVECTOR> positionsVec;
		std::vector<XMVECTOR> scalesVec;

		// current movement data (scaled according to the deltaTime)
		std::vector<XMVECTOR> translationsVec;
		std::vector<XMVECTOR> rotQuatsVec;        
		std::vector<float> uniformScaleFactors;

		// get entities transform data to update for this frame
		transformSys.GetTransformDataOfEntts(
			enttsToMove,
			transformDataIdxs,
			positions, 
			dirQuats, 
			uniformScales);

		PrepareTransformData(
			positions,
			positionsVec);

		PrepareMovementData( 
			deltaTime,
			movement.translationAndUniScales_,  // (x: trans_x, y: trans_y, z: trans_z, w: uniform_scale)
			movement.rotationQuats_,
			translationsVec,
			rotQuatsVec,
			uniformScaleFactors);

		// compute new values of transform data using the movement data
		ComputeTransformData(
			deltaTime,
			translationsVec,
			rotQuatsVec,
			uniformScaleFactors,
			positionsVec,
			dirQuats, 
			uniformScales);

		// write updated transform data into the Transform component
		transformSys.SetTransformDataByDataIdxs(
			transformDataIdxs, 
			positionsVec, 
			dirQuats, 
			uniformScales);

		// ------------------------------------------------------

		// get world matrices which will be updated according to new transform data;
		transformSys.GetWorldMatricesByDataIdxs(transformDataIdxs, worldMatricesToUpdate);

		// rebuild world matrices of that entities which were moved
		ComputeWorldMatrices(
			translationsVec, 
			rotQuatsVec, 
			uniformScaleFactors,
			worldMatricesToUpdate);

		// apply updated world matrices
		transformSys.SetWorldMatricesByDataIdxs(
			transformDataIdxs, 
			worldMatricesToUpdate);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		throw LIB_Exception("Went out of range during movement updating");
	}
}



// ********************************************************************************
// 
//                      PUBLIC CREATION / DELETING API
// 
// ********************************************************************************

void MoveSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& translations,
	const std::vector<XMVECTOR>& rotationQuats,
	const std::vector<float>& uniformScaleChanges)
{
	Movement& component = *pMoveComponent_;

	// check if there is no record with such entity ID
	bool areEnttsIDsUnique = true;

	for (const EntityID& id : enttsIDs)
		areEnttsIDsUnique &= (!BinarySearch(component.ids_, id));
	
	Assert::True(areEnttsIDsUnique, "there is already a record with some input ID (key)");


	// execute sorted insertion into the data arrays
	for (u32 data_idx = 0; const EntityID& id : enttsIDs)
	{	
		const XMFLOAT4 translationAndUniScale{ 
			translations[data_idx].x,
			translations[data_idx].y, 
			translations[data_idx].z, 
			uniformScaleChanges[data_idx] };

		const ptrdiff_t insertAtPos = Utils::GetPosForID(component.ids_, id);

		InsertAtPos(component.ids_, insertAtPos, id);
		InsertAtPos(component.translationAndUniScales_, insertAtPos, translationAndUniScale);
		InsertAtPos(component.rotationQuats_, insertAtPos, DirectX::XMQuaternionNormalize(rotationQuats[data_idx]));

		++data_idx;
	}
}

///////////////////////////////////////////////////////////

void MoveSystem::RemoveRecords(const std::vector<EntityID>& enttsIDs)
{
	throw LIB_Exception("TODO: IMPLEMENT IT!");
}

}