// **********************************************************************************
// Filename:      MovementSystemUpdateHelpers.h
// Description:   contains helper updating functional for the MovementSystem (ECS)
// 
// Created:       23.05.24
// **********************************************************************************
#pragma once

#include <vector>
#include <map>
#include <cassert>
#include <DirectXMath.h>

#include "../../ECS_Entity/ECS_Types.h"
#include "../../ECS_Components/Movement.h"
#include "../../ECS_Components/Transform.h"

typedef unsigned int UINT;
using namespace DirectX;



void GetEntitiesToUpdate(
	const std::map<EntityID, Movement::ComponentData>& movement,
	std::vector<EntityID>& outEntitiesIDs)
{
	// get entities (its IDs) which moves
	assert(movement.size() == outEntitiesIDs.size());

	UINT data_idx = 0;
	for (const auto& it : movement)
		outEntitiesIDs[data_idx++] = it.first;
}

///////////////////////////////////////////////////////////

void GetMovementDataToUpdate(
	const std::map<EntityID, Movement::ComponentData>& movement,
	std::vector<Movement::ComponentData>& outMovementData)
{
	// get movement data structures
	assert(movement.size() == outMovementData.size());

	UINT data_idx = 0;
	for (const auto& it : movement)
		outMovementData[data_idx++] = it.second;
}

///////////////////////////////////////////////////////////

void GetTransformDataToUpdate(
	const std::vector<EntityID>& entityIDs,
	const std::map<EntityID, Transform::ComponentData>& transform,
	std::vector<Transform::ComponentData>& outTransformData)
{
	// get transform data of entities which will be updated
	assert(entityIDs.size() == outTransformData.size());

	UINT data_idx = 0;
	for (const EntityID& entityID : entityIDs)
		outTransformData[data_idx++] = transform.at(entityID);
}

///////////////////////////////////////////////////////////

void GetTransformDataAsArraysOfXMVectors(
	const std::vector<Transform::ComponentData>& inTransformData,
	std::vector<XMVECTOR>& outPositions,
	std::vector<XMVECTOR>& outDirections,
	std::vector<XMVECTOR>& outScales)
{
	UINT data_idx = 0;
	for (const Transform::ComponentData& data : inTransformData)
	{
		outPositions[data_idx]  = XMLoadFloat3(&data.position_);
		outDirections[data_idx] = XMLoadFloat3(&data.direction_);
		outScales[data_idx]     = XMLoadFloat3(&data.scale_);
		++data_idx;
	}
}

///////////////////////////////////////////////////////////

void GetMovementDataAsArraysOfXMVectors(
	const std::vector<Movement::ComponentData>& inMovementData,
	std::vector<XMVECTOR>& outTranslations,
	std::vector<XMVECTOR>& outRotQuats,
	std::vector<XMVECTOR>& outScaleFactors)
{
	UINT data_idx = 0;
	for (const Movement::ComponentData& data : inMovementData)
	{
		outTranslations[data_idx] = XMLoadFloat3(&data.translation_);
		outRotQuats[data_idx]     = XMLoadFloat4(&data.rotationQuat_);
		outScaleFactors[data_idx] = XMLoadFloat3(&data.scaleChange_);
		++data_idx;
	}
}

///////////////////////////////////////////////////////////

void ComputeNewPositions(
	const float deltaTime,
	const std::vector<XMVECTOR>& inTranslations,
	_Inout_ std::vector<XMVECTOR>& outPositions)
{
	UINT data_idx = 0;
	for (XMVECTOR& position : outPositions)
		position = XMVectorAdd(position, XMVectorScale(inTranslations[data_idx++], deltaTime));
}

void ComputeNewDirections(
	const std::vector<XMVECTOR>& inRotQuats,
	_Inout_ std::vector<Transform::ComponentData>& outTransformData)
{
	// go through each transform data and update its 
	// directions according to movement data

	const XMVECTOR minRange{ -XM_PI, -XM_PI, -XM_PI };
	const XMVECTOR maxRange{ XM_PI, XM_PI, XM_PI };

	for (size_t idx = 0; idx < inRotQuats.size(); ++idx)
	{
		const DirectX::XMVECTOR dirVector = DirectX::XMLoadFloat3(&outTransformData[idx].direction_);

		// compute new direction
		XMVECTOR newDirVec = XMVector3Rotate(dirVector, inRotQuats[idx]);
		newDirVec = XMVectorClamp(newDirVec, minRange, maxRange);

		// store the new direction
		XMStoreFloat3(&outTransformData[idx].direction_, newDirVec);
	}
}

///////////////////////////////////////////////////////////

void ComputeTransformData(
	const float deltaTime,
	const std::vector<Movement::ComponentData>& inMovementData,
	std::vector<Transform::ComponentData>& outTransformData)
{
	const size_t movementDataSize = inMovementData.size();
	assert(movementDataSize == outTransformData.size());

	// current transform data of entities
	std::vector<DirectX::XMVECTOR> positions(movementDataSize);
	std::vector<DirectX::XMVECTOR> directions(movementDataSize);
	std::vector<DirectX::XMVECTOR> scales(movementDataSize);

	// current movement data of entities
	std::vector<DirectX::XMVECTOR> translations(movementDataSize);
	std::vector<DirectX::XMVECTOR> rotQuats(movementDataSize);
	std::vector<DirectX::XMVECTOR> scaleChanges(movementDataSize);

	GetTransformDataAsArraysOfXMVectors(outTransformData, positions, directions, scales);
	GetMovementDataAsArraysOfXMVectors(inMovementData, translations, rotQuats, scaleChanges);
	
	ComputeNewPositions(deltaTime, translations, positions);
	ComputeNewDirections(rotQuats, outTransformData);

	// compute new transform data
	for (size_t idx = 0; idx < movementDataSize; ++idx)
	{
		// compute new world matrix
		outTransformData[idx].world_ *= XMMatrixAffineTransformation(
			scaleChanges[idx],                                  // scaling change
			XMLoadFloat3(&outTransformData[idx].position_),     // rotation origin
			rotQuats[idx],                                      // rotation quaterion
			translations[idx]);                                 // translation
	}
}

///////////////////////////////////////////////////////////