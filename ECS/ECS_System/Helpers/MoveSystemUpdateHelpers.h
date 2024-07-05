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

void PrepareTransformData(
	const std::vector<XMFLOAT3>& inPositions,
	std::vector<XMVECTOR>& outPositions)
{
	// convert the input transform data into XMVECTOR type

	outPositions.reserve(std::ssize(inPositions));

	for (const XMFLOAT3& pos : inPositions)
	{
		outPositions.emplace_back(XMLoadFloat3(&pos));
		//outPositions.back().m128_f32[3] = 1;
	}
}

///////////////////////////////////////////////////////////

void PrepareMovementData(
	const float deltaTime,
	const std::vector<XMFLOAT4>& inTranslationsAndUniScales,
	const std::vector<XMVECTOR>& inRotQuats,
	std::vector<XMVECTOR>& outTranslations,    
	std::vector<XMVECTOR>& outRotQuats,         
	std::vector<float>& outScaleChanges)        
{
	// convert the movement data into XMVECTOR and 
	// scale its magnitude according to the delta time

	ASSERT_TRUE(std::ssize(inTranslationsAndUniScales) == std::ssize(inRotQuats), "number of translations/uniform scales must be equal to the number of rotation quaternions");

	const float noSpeedCorrection = 1.0f;
	const size_t dataCount = inTranslationsAndUniScales.size();

	outTranslations.reserve(dataCount);
	outRotQuats.reserve(dataCount);
	outScaleChanges.reserve(dataCount);

	// prepare translations
	for (size_t idx = 0; idx < dataCount; ++idx)
	{
		outTranslations.emplace_back(XMVectorScale(
				XMLoadFloat4(&inTranslationsAndUniScales[idx]),   
				deltaTime));
	}

	// prepare uniform scale changes (get w-component from translations)
	// NOTE: we don't need to scale these values because it's already been done before
	for (const XMVECTOR& translation : outTranslations)
		outScaleChanges.push_back(XMVectorGetW(translation));


	// NOTE: currently we don't have any speed correction 
	//       for rotation quaternionsaccording to deltaTime
	for (size_t idx = 0; idx < dataCount; ++idx)
		outRotQuats.emplace_back(inRotQuats[idx]);
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

///////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////

void ComputeNewScales(
	const std::vector<float>& uniformScaleChanges,
	std::vector<float>& inOutUniformScales)
{
	// go through each scale and modify it with the uniformScaleChange factor

	for (size_t idx = 0; idx < inOutUniformScales.size(); ++idx)
		inOutUniformScales[idx] *= uniformScaleChanges[idx];
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
	const std::vector<float>& inUniformScaleChanges,
	std::vector<XMVECTOR>& inOutPositions,
	std::vector<XMVECTOR>& inOutDirQuats,
	std::vector<float>& inOutUniformScales)
{
	// compute new transform data (position / direction_quaternion / uniform_scale) using
	// movement data (translation / rotation_quaternion / uniform_scale_factor)
	
	ComputeNewPositions(inTranslations, inOutPositions);
	ComputeNewDirections(inRotQuats, inOutDirQuats);
	ComputeNewScales(inUniformScaleChanges, inOutUniformScales);
}
