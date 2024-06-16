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


///////////////////////////////////////////////////////////

void GetTransformDataToUpdate(
	const std::vector<EntityID>& enttsIDs,
	Transform& transform,
	std::vector<ptrdiff_t>& outDataIdxs,
	std::vector<XMFLOAT3>& outPositions,
	std::vector<XMFLOAT3>& outDirections,
	std::vector<XMFLOAT3>& outScales)
{
	// get transform data of input entities by its IDs

	std::vector<ptrdiff_t> dataIdxs;
	const ptrdiff_t enttsCount = std::ssize(enttsIDs);
	outDataIdxs.reserve(enttsCount);
	outPositions.reserve(enttsCount);
	outDirections.reserve(enttsCount);
	outScales.reserve(enttsCount);

	// get data indices into arrays 
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


// *********************************************************************************

void GetTransformDataAsArraysOfXMVectors(
	const std::vector<XMFLOAT3>& inPositions,
	const std::vector<XMFLOAT3>& inDirections,
	const std::vector<XMFLOAT3>& inScales,
	std::vector<XMVECTOR>& outPositions,
	std::vector<XMVECTOR>& outDirections,
	std::vector<XMVECTOR>& outScales)
{
	// convert all the input transform data from XMFLOAT3 into XMVECTOR type
	const size_t dataCount = inPositions.size();

	outPositions.reserve(dataCount);
	outDirections.reserve(dataCount);
	outScales.reserve(dataCount);

	for (size_t idx = 0; idx < dataCount; ++idx)
		outPositions.push_back(XMLoadFloat3(&inPositions[idx]));

	for (size_t idx = 0; idx < dataCount; ++idx)
		outDirections.push_back(XMLoadFloat3(&inDirections[idx]));

	for (size_t idx = 0; idx < dataCount; ++idx)
		outScales.push_back(XMLoadFloat3(&inScales[idx]));
}

///////////////////////////////////////////////////////////

void GetMovementDataAsArraysOfXMVectors(
	const float deltaTime,
	const std::vector<XMFLOAT3>& inTranslations,
	const std::vector<XMFLOAT4>& inRotQuats,
	const std::vector<XMFLOAT3>& inScaleChanges,
	std::vector<XMVECTOR>& outTranslations,
	std::vector<XMVECTOR>& outRotQuats,
	std::vector<XMVECTOR>& outScaleChanges)
{
	// convert the movement data into XMVECTOR and 
	// scale it according to the delta time

	const float noSpeedCorrection = 1.0f;
	const size_t dataCount = inTranslations.size();

	outTranslations.reserve(dataCount);
	outRotQuats.reserve(dataCount);
	outScaleChanges.reserve(dataCount);

	for (size_t idx = 0; idx < dataCount; ++idx)
		outTranslations.push_back(XMVectorScale(XMLoadFloat3(&inTranslations[idx]), deltaTime));

	for (size_t idx = 0; idx < dataCount; ++idx)
		outRotQuats.push_back(XMVectorScale(XMLoadFloat4(&inRotQuats[idx]), noSpeedCorrection));

	for (size_t idx = 0; idx < dataCount; ++idx)
		outScaleChanges.push_back(XMVectorScale(XMLoadFloat3(&inScaleChanges[idx]), noSpeedCorrection));
}

///////////////////////////////////////////////////////////

void ComputeNewPositions(
	const std::vector<XMVECTOR>& inTranslations,
	std::vector<XMVECTOR>& inOutPos)
{
	// go through each position and modify it with translation
	for (size_t idx = 0; idx < inOutPos.size(); ++idx)
		inOutPos[idx] = XMVectorAdd(inOutPos[idx], inTranslations[idx]);
}

void ComputeNewDirections(
	const std::vector<XMVECTOR>& inRotQuats,
	std::vector<XMVECTOR>& inOutDir)
{
	// go through each direction vector and modify it with quaterion

	const XMVECTOR minRange{ -XM_PI, -XM_PI, -XM_PI };
	const XMVECTOR maxRange{ XM_PI, XM_PI, XM_PI };

	// compute new direction and clamp it between [-PI, PI]
	for (size_t idx = 0; idx < inOutDir.size(); ++idx)
		inOutDir[idx] = XMVectorClamp(XMVector3Rotate(inOutDir[idx], inRotQuats[idx]), minRange, maxRange);
}

void ComputeNewScales(
	const std::vector<XMVECTOR>& scaleChanges,
	std::vector<XMVECTOR>& inOutScales)
{
	// go through each scale and modify it with the scaleChange factors

	for (size_t idx = 0; idx < inOutScales.size(); ++idx)
		inOutScales[idx] = XMVectorMultiply(inOutScales[idx], scaleChanges[idx]);
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
	const std::vector<XMVECTOR>& inTranslations,
	const std::vector<XMVECTOR>& inRotQuats,
	const std::vector<XMVECTOR>& inScaleChanges,
	std::vector<XMVECTOR>& inOutPositions,
	std::vector<XMVECTOR>& inOutDirections,
	std::vector<XMVECTOR>& inOutScales)
{
	// compute new transform data (position / direction / scale) using
	// movement data (translation / rotation_quaternion / scale_factor)
	
	ComputeNewPositions(inTranslations, inOutPositions);
	ComputeNewDirections(inRotQuats, inOutDirections);
	ComputeNewScales(inScaleChanges, inOutScales);

	ComputeAndApplyWorldMatrices(translations, rotQuats, scaleChanges, outTransformData);
}


// ***********************************************************************************

void ApplyPositions(
	const std::vector<XMVECTOR>& posToApply,
	const std::vector<ptrdiff_t>& dataIdxs,
	std::vector<XMFLOAT3>& posToUpdate)
{
	// write updated positions by idxs into the Transform component
	UINT data_idx = 0;
	for (const ptrdiff_t idx : dataIdxs)
		XMStoreFloat3(&posToUpdate[idx], std::move(posToApply[data_idx++]));
}

///////////////////////////////////////////////////////////

void ApplyDirections(
	const std::vector<XMVECTOR>& dirToApply,
	const std::vector<ptrdiff_t>& dataIdxs,
	std::vector<XMFLOAT3>& dirToUpdate)
{
	// write updated directions by idxs into the Transform component
	UINT data_idx = 0;
	for (const ptrdiff_t idx : dataIdxs)
		XMStoreFloat3(&dirToUpdate[idx], std::move(dirToApply[data_idx++]));
}

///////////////////////////////////////////////////////////

void ApplyScales(
	const std::vector<XMVECTOR>& scalesToApply,
	const std::vector<ptrdiff_t>& dataIdxs,
	std::vector<XMFLOAT3>& scalesToUpdate)
{
	// write updated scales by idxs into the Transform component
	UINT data_idx = 0;
	for (const ptrdiff_t idx : dataIdxs)
		XMStoreFloat3(&scalesToUpdate[idx], std::move(scalesToApply[data_idx++]));
}

///////////////////////////////////////////////////////////

void ApplyTransformData(
	Transform& transformComponent,
	const std::vector<ptrdiff_t>& dataIdxs,
	const std::vector<XMVECTOR>& positions,
	const std::vector<XMVECTOR>& directions,
	const std::vector<XMVECTOR>& scales)
{
	// store new transform data into the Transform component

	ApplyPositions(positions, dataIdxs, transformComponent.positions_);
	ApplyPositions(directions, dataIdxs, transformComponent.directions_);
	ApplyPositions(scales, dataIdxs, transformComponent.scales_);
}

