// **********************************************************************************
// Filename:      MoveSystemUpdateHelpers.h
// Description:   contains helper updating functional for the MoveSystem (ECS)
// 
// Created:       23.05.24
// **********************************************************************************
#pragma once

#include <vector>
#include <map>
#include <cassert>
#include <DirectXMath.h>

#include "../../ECS_Common/ECS_Types.h"
#include "../../ECS_Components/Movement.h"
#include "../../ECS_Components/Transform.h"


using namespace DirectX;

#if 0
void GetEntitiesToUpdate(
	const std::map<entitiesName, Movement::ComponentData>& movement,
	std::vector<entitiesName>& outEntitiesIDs)
{
	// get IDs of all the entities which moves
	assert(movement.size() == outEntitiesIDs.size());

	UINT data_idx = 0;
	for (const auto& it : movement)
		outEntitiesIDs[data_idx++] = it.first;
}

///////////////////////////////////////////////////////////

void GetMovementDataToUpdate(
	const std::map<entitiesName, Movement::ComponentData>& movement,
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
	const std::vector<entitiesName>& entitiesNames,
	const std::map<entitiesName, Transform::ComponentData>& transform,
	std::vector<Transform::ComponentData>& outTransformData)
{
	// get transform data of entities which will be updated
	assert(entitiesNames.size() == outTransformData.size());

	UINT data_idx = 0;
	for (const entitiesName& entitiesName : entitiesNames)
		outTransformData[data_idx++] = transform.at(entitiesName);
}


// *********************************************************************************

void GetTransformDataAsArraysOfXMVectors(
	const std::vector<Transform::ComponentData>& inTransformData,
	std::vector<XMVECTOR>& outPositions,
	std::vector<XMVECTOR>& outDirections,
	std::vector<XMVECTOR>& outScales)
{
	for (UINT idx = 0; idx < inTransformData.size(); ++idx)
	{
		outPositions[idx]  = XMLoadFloat3(&inTransformData[idx].position_);
		outDirections[idx] = XMLoadFloat3(&inTransformData[idx].direction_);
		outScales[idx]     = XMLoadFloat3(&inTransformData[idx].scale_);
	}
}

///////////////////////////////////////////////////////////

void GetMovementDataAsArraysOfXMVectors(
	const float deltaTime,
	const std::vector<Movement::ComponentData>& inMovementData,
	std::vector<XMVECTOR>& outTranslations,
	std::vector<XMVECTOR>& outRotQuats,
	std::vector<XMVECTOR>& outScaleChanges)
{
	// convert movement data into XMVECTOR and scale it according to the delta time

	const float noSpeedCorrection = 1.0f;
	for (UINT idx = 0; idx < inMovementData.size(); ++idx)
	{
		outTranslations[idx] = XMVectorScale(XMLoadFloat3(&inMovementData[idx].translation_), deltaTime);
		outRotQuats[idx]     = XMVectorScale(XMLoadFloat4(&inMovementData[idx].rotationQuat_), noSpeedCorrection);
		outScaleChanges[idx] = XMVectorScale(XMLoadFloat3(&inMovementData[idx].scaleChange_), noSpeedCorrection);
	}
}

///////////////////////////////////////////////////////////

void ComputeNewPositions(
	const std::vector<XMVECTOR>& inTranslations,
	_Inout_ std::vector<XMVECTOR>& outPositions)
{
	// go through each position and modify it with translation
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

void ComputeNewScales(
	const std::vector<XMVECTOR>& scaleChanges,
	_Inout_ std::vector<XMVECTOR>& outScales)
{
	// go through each scale and modify it with the scaleChange factors

	UINT data_idx = 0;
	for (XMVECTOR& scale : outScales)
		scale = XMVectorMultiply(scale, scaleChanges[data_idx++]);
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

void ApplyScales(
	const std::vector<XMVECTOR>& scales,
	std::vector<Transform::ComponentData>& outTransformData)
{
	UINT data_idx = 0;
	for (auto& data : outTransformData)
		XMStoreFloat3(&data.scale_, scales[data_idx++]);
}

///////////////////////////////////////////////////////////

void ComputeAndApplyWorldMatrices(
	const std::vector<XMVECTOR>& translations,
	const std::vector<XMVECTOR>& rotQuats,
	const std::vector<XMVECTOR>& scaleChanges,
	_Inout_ std::vector<Transform::ComponentData>& outTransformData)
{
	// NOTICE: if scale change > 1 - entity is getting bigger; if < 1 -- entity is getting smaller;

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
	_Inout_ std::vector<Transform::ComponentData>& outTransformData)
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
	ComputeNewScales(scaleChanges, scales);

	ApplyPositions(positions, outTransformData);
	ApplyDirections(directions, outTransformData);
	ApplyScales(scales, outTransformData);

	ComputeAndApplyWorldMatrices(translations, rotQuats, scaleChanges, outTransformData);
}

///////////////////////////////////////////////////////////

void StoreTransformData(
	const std::vector<entitiesName>& entitiesNames,
	const std::vector<Transform::ComponentData>& inTransform,
	std::map<entitiesName, Transform::ComponentData>& outTransform)
{
	// store new transform data into the transform component
	UINT data_idx = 0;
	for (const entitiesName& entitiesName : entitiesNames)
		outTransform[entitiesName] = inTransform[data_idx++];
}

#endif