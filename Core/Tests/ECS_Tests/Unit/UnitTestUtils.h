// *********************************************************************************
// Filename:     UnitTestUtils.h
// Description:  contains different utils for ECS components testing
// 
// Created:      26.05.24
// *********************************************************************************

#pragma once


#include <vector>
#include <algorithm>
#include <random>


#include "ECS_Entity/EntityManager.h"
#include "MathHelper.h"
#include "HelperTypes.h"

#include "../Engine/log.h"
#include "../Engine/EngineException.h"

#include <filesystem>



namespace Utils
{

// *******************************************************************************

namespace fs = std::filesystem;


static void RemoveFile(const std::string& filepath)
{
	if (fs::exists(filepath))
	{
		if (!fs::remove(fs::path(filepath)))
			Log::Error(LOG_MACRO, "can't remove a file by path: " + filepath);
	}
}

static bool CheckEnttsHaveComponent(
	EntityManager& entityMgr,
	const std::vector<EntityID>& ids,
	const ComponentType componentType)
{
	// get component flags by IDs from the EntityManager and check if they have set 
	// particular bits by componentTypes

	std::vector<ComponentFlagsType> componentFlags;
	entityMgr.GetComponentFlagsByIDs(ids, componentFlags);
	
	const ComponentFlagsType bitmask = (1 << componentType);
	uint32_t haveComponent = bitmask;

	for (const ComponentFlagsType flag : componentFlags)
		haveComponent &= (flag & bitmask);

	return haveComponent;
}

///////////////////////////////////////////////////////////

static std::string GetRandomAlnumString(size_t length)
{
	const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	thread_local std::mt19937 rg{ std::random_device{}() };
	thread_local std::uniform_int_distribution<std::string::size_type> pick(0, charset.size() - 1);

	std::string str;
	str.reserve(length);

	while (length--) str += charset[pick(rg)];

	return str;
}

///////////////////////////////////////////////////////////
#if 0
template<class T>
static bool ContainerCompare(
	const T& lhs,
	const T& rhs)
{
	// check two STL containers (vector, map, set, etc.) for complete equality;
	// return: true -- if two containers are completely equal
	// 
	// note: no predicate needed because there is operator== for pairs already

	return (lhs.size() == rhs.size()) &&
		std::equal(lhs.begin(), lhs.end(), rhs.begin());
}
#endif

///////////////////////////////////////////////////////////

template<class T, class Pred = std::equal_to<>>
static bool ContainerCompare(
	const T& lhs,
	const T& rhs,
	Pred pred = {})
{
	// check two STL containers (vector, map, set, etc.) for complete equality
	// using the passed predicate (pred);
	// return: true -- if two containers are completely equal

	return (lhs.size() == rhs.size()) &&
		std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
}

///////////////////////////////////////////////////////////

static void GenerateEnttsNames(
	const size_t enttsCount,
	const size_t nameLength,
	std::vector<EntityName>& outEnttsNames)
{
	// generate unique names for entities in quantity enttsCount
	// out: array of entities names;

	for (UINT idx = 0; idx < enttsCount; ++idx)
		outEnttsNames.emplace_back(GetRandomAlnumString(nameLength));
}

///////////////////////////////////////////////////////////

static void PrepareRandomDataForArray(
	const size_t arrSize,
	std::vector<XMFLOAT3>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.reserve(arrSize);
	
	for (size_t idx = 0; idx < arrSize; ++idx)
		outArr.emplace_back(MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF());
}

///////////////////////////////////////////////////////////

static void PrepareRandomDataForArray(
	const size_t arrSize,
	std::vector<XMFLOAT4>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.reserve(arrSize);

	for (size_t idx = 0; idx < arrSize; ++idx)
		outArr.emplace_back(MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF());
}

///////////////////////////////////////////////////////////

static void PrepareRandomTransformData(
	const size_t elemCount,
	TransformData& outTransform)
{
	// generate random values for positions/directions/scales
	PrepareRandomDataForArray(elemCount, outTransform.positions);
	PrepareRandomDataForArray(elemCount, outTransform.directions);
	PrepareRandomDataForArray(elemCount, outTransform.scales);
}

///////////////////////////////////////////////////////////

static void PrepareRandomMovementData(
	const size_t elemCount,
	MoveData& outMove)
{
	// generate random values for translations/rotations_quaternions/scale_changes
	Utils::PrepareRandomDataForArray(elemCount, outMove.translations);
	Utils::PrepareRandomDataForArray(elemCount, outMove.rotQuats);
	Utils::PrepareRandomDataForArray(elemCount, outMove.scaleChanges);
}

///////////////////////////////////////////////////////////

static void PrepareRandomRenderedData(
	const size_t elemCount,
	RenderedData& rendered)
{
	rendered.shaderTypes.resize(elemCount, ECS::RENDERING_SHADERS::COLOR_SHADER);
	rendered.primTopologyTypes.resize(elemCount, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

///////////////////////////////////////////////////////////

static bool CheckArrContainsIDs(
	const std::vector<EntityID>& examinedArr,
	const std::vector<EntityID>& searchForIDs)
{
	// check if examinedArr constains searched data
	// 
	// in: 1) SORTED arr which will be examined
	//     2) SORTED arr which data will be used for the search
	const auto beg = examinedArr.begin();
	const auto end = examinedArr.end();

	for (const EntityID& ID : searchForIDs)
	{
		// if there is such an ID just continue loop-iteration
		if (std::binary_search(beg, end, ID))
			continue;

		// if we got here we didn't manage to find an ID
		Log::Error(LOG_MACRO, "There is no such entity ID: " + std::to_string(ID));
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

static XMMATRIX BuildMatrix(const XMFLOAT3& pos, const XMFLOAT3& dir, const XMFLOAT3& scale)
{
	return	DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scale)) *
			DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&dir)) *
			DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&pos));
}


};  // namespace Utils
