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

	// NOTE: currently we don't have any speed correction according to deltaTime
	for (size_t idx = 0; idx < dataCount; ++idx)
		outTranslations.push_back(XMVectorScale(XMLoadFloat3(&inTranslations[idx]), noSpeedCorrection));

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

void ComputeWorldMatrices(
	const std::vector<XMVECTOR>& translations,
	const std::vector<XMVECTOR>& rotQuats,
	const std::vector<XMVECTOR>& scaleChanges,
	std::vector<XMMATRIX>& inOutWorldMatrices)
{
	// rebuild world matrices according to the input movement data;

	for (size_t idx = 0; idx < inOutWorldMatrices.size(); ++idx)
	{
		inOutWorldMatrices[idx] *= XMMatrixAffineTransformation(
			scaleChanges[idx],                                  // scaling change
			inOutWorldMatrices[idx].r[3],                       // rotation origin (around itself)
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
}
