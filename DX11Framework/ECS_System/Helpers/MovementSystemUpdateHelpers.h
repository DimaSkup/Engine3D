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


// *********************************************************************************

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
	const float deltaTime,
	const std::vector<Movement::ComponentData>& inMovementData,
	std::vector<XMVECTOR>& outTranslations,
	std::vector<XMVECTOR>& outRotQuats,
	std::vector<XMVECTOR>& outScaleFactors)
{
	// convert movement data into XMVECTOR and scale it according to the delta time
	UINT data_idx = 0;
	const float noSpeedCorrection = 1.0f;
	for (const Movement::ComponentData& data : inMovementData)
	{
		outTranslations[data_idx] = XMVectorScale(XMLoadFloat3(&data.translation_), deltaTime);
		outRotQuats[data_idx]     = XMVectorScale(XMLoadFloat4(&data.rotationQuat_), noSpeedCorrection);
		outScaleFactors[data_idx] = XMVectorScale(XMLoadFloat3(&data.scaleChange_), noSpeedCorrection);
		++data_idx;
	}
}

///////////////////////////////////////////////////////////

void ComputeNewPositions(
	const std::vector<XMVECTOR>& inTranslations,
	_Inout_ std::vector<XMVECTOR>& outPositions)
{
	UINT data_idx = 0;
	for (XMVECTOR& position : outPositions)
		position = XMVectorAdd(position, inTranslations[data_idx++]);
}

void ComputeNewDirections(
	const std::vector<XMVECTOR>& inRotQuats,
	_Inout_  std::vector<XMVECTOR>& outDirections)
{
	// go through each direction vector and modify it with quaterion

	const XMVECTOR minRange{ -XM_PI, -XM_PI, -XM_PI };
	const XMVECTOR maxRange{ XM_PI, XM_PI, XM_PI };
	UINT data_idx = 0;

	// compute new direction and clamp it between [-PI, PI]
	for (XMVECTOR& dir : outDirections)
	{
		dir = XMVectorClamp(XMVector3Rotate(dir, inRotQuats[data_idx]), minRange, maxRange);
		++data_idx;
	}
}

///////////////////////////////////////////////////////////

void ApplyPositions(
	const std::vector<XMVECTOR>& positions,
	std::vector<Transform::ComponentData>& outTransformData)
{
	UINT data_idx = 0;
	for (auto& data : outTransformData)
		XMStoreFloat3(&data.position_, positions[data_idx++]);
}

void ApplyDirections(
	const std::vector<XMVECTOR>& directions,
	std::vector<Transform::ComponentData>& outTransformData)
{
	UINT data_idx = 0;
	for (auto& data : outTransformData)
		XMStoreFloat3(&data.direction_, directions[data_idx++]);
}

///////////////////////////////////////////////////////////

void ComputeAndApplyWorldMatrices(
	const std::vector<XMVECTOR>& translations,
	const std::vector<XMVECTOR>& rotQuats,
	const std::vector<XMVECTOR>& scaleChanges,
	_Inout_ std::vector<Transform::ComponentData>& outTransformData)
{
	for (size_t idx = 0; idx < outTransformData.size(); ++idx)
	{
		outTransformData[idx].world_ *= XMMatrixAffineTransformation(
			scaleChanges[idx],                                  // scaling change
			XMLoadFloat3(&outTransformData[idx].position_),     // rotation origin (around itself)
			rotQuats[idx],                                      // rotation quaterion
			translations[idx]);                                 // translation
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
	GetMovementDataAsArraysOfXMVectors(deltaTime, inMovementData, translations, rotQuats, scaleChanges);
	
	ComputeNewPositions(translations, positions);
	ComputeNewDirections(rotQuats, directions);
	//ComputeNewScales();

	ApplyPositions(positions, outTransformData);
	ApplyDirections(directions, outTransformData);
	//ApplyScales();

	ComputeAndApplyWorldMatrices(translations, rotQuats, scaleChanges, outTransformData);
}

///////////////////////////////////////////////////////////

void StoreTransformData(
	const std::vector<EntityID>& entityIDs,
	const std::vector<Transform::ComponentData>& inTransform,
	std::map<EntityID, Transform::ComponentData>& outTransform)
{
	// store new transform data into the transform component
	UINT data_idx = 0;
	for (const EntityID& entityID : entityIDs)
		outTransform[entityID] = inTransform[data_idx++];
}