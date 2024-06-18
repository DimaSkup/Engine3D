// **********************************************************************************
// Filename:      MoveSystem.h
// Description:   implementation of the MoveSystem's functional
// 
// Created:       23.05.24
// **********************************************************************************
#include "MoveSystem.h"

#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/log.h"
#include "../ECS_Common/Utils.h"
#include "./Helpers/MoveSystemUpdateHelpers.h"

#include <stdexcept>
#include <fstream>
#include <sstream>

using namespace DirectX;
using namespace ECS;


MoveSystem::MoveSystem(
	Transform* pTransformComponent,
	WorldMatrix* pWorldMatrixComponent,
	Movement* pMoveComponent)
{
	ASSERT_NOT_NULLPTR(pTransformComponent, "ptr to the Transform component == nullptr");
	ASSERT_NOT_NULLPTR(pWorldMatrixComponent, "ptr to the WorldMatrix component == nullptr");
	ASSERT_NOT_NULLPTR(pMoveComponent, "ptr to the Movement component == nullptr");

	pTransformComponent_ = pTransformComponent;
	pWorldMatComponent_ = pWorldMatrixComponent;
	pMoveComponent_ = pMoveComponent;
}

///////////////////////////////////////////////////////////

void MoveSystem::Serialize(const std::string& dataFilepath)
{
	// serialize all the data from the Movement component into the data file

	ASSERT_NOT_EMPTY(dataFilepath.empty(), "path to the data file is empty");

	std::ofstream fout;

	try
	{
		fout.open(dataFilepath, std::ios::binary);

		if (!fout.is_open())
			THROW_ERROR("can't open a file for deserialization: " + dataFilepath);

		Movement& move = *pMoveComponent_;

		const ptrdiff_t dataCount = std::ssize(move.ids_);
		const std::string dataCountStr = { "movement_data_count: " + std::to_string(dataCount) };

		// write movement data into the file
		fout.write(dataCountStr.c_str(), dataCountStr.size());
		fout.write((const char*)move.ids_.data(), dataCount * sizeof(EntityID));
		fout.write((const char*)move.translations_.data(), dataCount * sizeof(XMFLOAT3));
		fout.write((const char*)move.rotationQuats_.data(), dataCount * sizeof(XMFLOAT4));
		fout.write((const char*)move.scaleChanges_.data(), dataCount * sizeof(XMFLOAT3));
	
	}
	catch (LIB_Exception& e)
	{
		fout.close();
		Log::Error(e, true);
		THROW_ERROR("something went wrong during serialization");
	}
}

///////////////////////////////////////////////////////////

void MoveSystem::Deserialize(const std::string& dataFilepath)
{
	// deserialize the data from the data file into the Movement component

	ASSERT_NOT_EMPTY(dataFilepath.empty(), "path to the data file is empty");

	try
	{
		std::ifstream fin(dataFilepath, std::ios::binary);

		if (!fin.is_open())
			THROW_ERROR("can't open a file for deserialization: " + dataFilepath);

		// read into the buffer all the content of the data file
		std::stringstream buffer;
		buffer << fin.rdbuf();
		fin.close();

		// define what amount of movement data will we read
		std::string ignore;
		UINT dataCount = 0;

		buffer >> ignore >> dataCount;

		// if we read wrong data block
		ASSERT_TRUE(ignore == "movement_data_count:", "read wrong data during deserialization of the Movement component data");
		ignore.clear();

		// ------------------------------------------

		Movement& component = *pMoveComponent_;

		std::vector<EntityID>& ids = component.ids_;
		std::vector<XMFLOAT3>& translations = component.translations_;
		std::vector<XMFLOAT4>& rotQuats = component.rotationQuats_;
		std::vector<XMFLOAT3>& scaleChanges = component.scaleChanges_;

		// clear the Movement component of previous data
		ids.clear();
		translations.clear();
		rotQuats.clear();
		scaleChanges.clear();

		// if we have any data for deserialization
		if (dataCount > 0)
		{
			// prepare enough amount of memory for data
			ids.resize(dataCount);
			translations.resize(dataCount);
			rotQuats.resize(dataCount);
			scaleChanges.resize(dataCount);

			// deserialize the Movement component data into the data arrays
			buffer.read((char*)ids.data(), dataCount * sizeof(EntityID));
			buffer.read((char*)translations.data(), dataCount * sizeof(XMFLOAT3));
			buffer.read((char*)rotQuats.data(), dataCount * sizeof(XMFLOAT4));
			buffer.read((char*)scaleChanges.data(), dataCount * sizeof(XMFLOAT3));
		}
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e, true);
		THROW_ERROR("something went wrong during deserialization");
	}
}

///////////////////////////////////////////////////////////

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
		std::vector<XMFLOAT3> directions;
		std::vector<XMFLOAT3> scales;
		std::vector<XMMATRIX> worldMatricesToUpdate;
		std::vector<ptrdiff_t> transformDataIdxs;

		// current transform data of entities as XMVECTORs
		std::vector<DirectX::XMVECTOR> positionsVec;
		std::vector<DirectX::XMVECTOR> directionsVec;
		std::vector<DirectX::XMVECTOR> scalesVec;

		// current movement data of entities as XMVECTORs
		std::vector<DirectX::XMVECTOR> translationsVec;
		std::vector<DirectX::XMVECTOR> rotQuatsVec;
		std::vector<DirectX::XMVECTOR> scaleChangesVec;

		// get entities transform data to update for this frame
		transformSys.GetTransformDataOfEntts(
			enttsToMove,
			transformDataIdxs,
			positions, 
			directions, 
			scales);

		GetTransformDataAsArraysOfXMVectors(
			positions,
			directions, 
			scales, 
			positionsVec,
			directionsVec, 
			scalesVec);

		GetMovementDataAsArraysOfXMVectors( 
			deltaTime,
			movement.translations_,
			movement.rotationQuats_,
			movement.scaleChanges_,
			translationsVec,
			rotQuatsVec,
			scaleChangesVec);

		// compute new values of transform data using the movement data
		ComputeTransformData(
			deltaTime,
			translationsVec,
			rotQuatsVec,
			scaleChangesVec,
			positionsVec,
			directionsVec, 
			scalesVec);

		// write updated transform data into the Transform component
		transformSys.SetTransformDataByDataIdxs(
			transformDataIdxs, 
			positionsVec, 
			directionsVec, 
			scalesVec);

		// ------------------------------------------------------

		// get world matrices which will be updated according to new transform data;
		transformSys.GetWorldMatricesByIDs(enttsToMove, worldMatricesToUpdate);

		// rebuild world matrices of that entities which were moved
		ComputeWorldMatrices(
			translationsVec, 
			rotQuatsVec, 
			scaleChangesVec,
			worldMatricesToUpdate);

		// apply updated world matrices
		transformSys.SetWorldMatricesByIDs(enttsToMove, worldMatricesToUpdate);
	}
	catch (const std::out_of_range& e)
	{
		ECS::Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("Went out of range during movement updating");
	}
}

///////////////////////////////////////////////////////////

void MoveSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<XMFLOAT3>& translations,
	const std::vector<XMFLOAT4>& rotationQuats,  // rotation quaternions (but stored as XMFLOAT4)
	const std::vector<XMFLOAT3>& scaleChanges)
{
	Movement& move = *pMoveComponent_;

	Utils::AppendArray<EntityID>(move.ids_, enttsIDs);
	Utils::AppendArray<XMFLOAT3>(move.translations_, translations);
	Utils::AppendArray<XMFLOAT4>(move.rotationQuats_, rotationQuats);
	Utils::AppendArray<XMFLOAT3>(move.scaleChanges_, scaleChanges);
}

///////////////////////////////////////////////////////////

void MoveSystem::RemoveRecords(const std::vector<EntityID>& enttsIDs)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}

///////////////////////////////////////////////////////////

void MoveSystem::GetEnttsIDsFromMoveComponent(std::vector<EntityID>& outEnttsIDs)
{
	// get a bunch of all the entities IDs which have the Move component
	// out: array of entities IDs

	outEnttsIDs = pMoveComponent_->ids_;
}

///////////////////////////////////////////////////////////
