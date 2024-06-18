// *********************************************************************************
// Filename:     UnitTestUtils.h
// Description:  contains different utils for ECS components testing
// 
// Created:      26.05.24
// *********************************************************************************

#pragma once


#include <vector>
#include <algorithm>

#include "ECS_Entity/EntityManager.h"
#include "MathHelper.h"
#include "HelperTypes.h"

#include "../Engine/log.h"
#include "../Engine/EngineException.h"



namespace Utils
{

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

///////////////////////////////////////////////////////////

template<class T, class Pred>
static bool ContainerCompare(
	const T& lhs,
	const T& rhs,
	Pred pred)
{
	// check two STL containers (vector, map, set, etc.) for complete equality
	// using the passed predicate (pred);
	// return: true -- if two containers are completely equal

	return (lhs.size() == rhs.size()) &&
		std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
}

///////////////////////////////////////////////////////////

static void GenerateEnttsNames(std::vector<EntityName>& outEnttsNames)
{
	// generate unique ID for each entity and create entities inside the manager
	// out: array of entities names;
	
	const UINT enttsCountPerPrefix = 10;
	const std::vector<std::string> prefixes{ "sphere_", "cylinder_", "cube_" };
	std::vector<std::string> suffixes;
	

	// precompute suffixes values
	for (size_t idx = 0; idx < enttsCountPerPrefix; ++idx)
		suffixes.push_back(std::to_string(idx));

	// generation of names
	for (const std::string& prefix : prefixes)
	{
		for (UINT idx = 0; idx < enttsCountPerPrefix; ++idx)
		{
			outEnttsNames.push_back({ prefix + suffixes[idx] });
		}
	}
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
	std::vector<RENDERING_SHADERS>& outShaderTypes,
	std::vector<D3D11_PRIMITIVE_TOPOLOGY>& outPrimTopologyTypes)
{
	outShaderTypes.resize(elemCount, RENDERING_SHADERS::COLOR_SHADER);
	outPrimTopologyTypes.resize(elemCount, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

static bool CheckComponentKnowsAboutEntities(
	EntityManager& entityMgr,
	const std::vector<EntityID>& enttsIDsToCheck,
	const ComponentType& componentType)
{
	// check if a component with componentType constains records with entities
	// from the input entitiesIDs array;
	// return: true  - if the component knows each entity; 
	//         false - if the component doesn't know some entity

	std::vector<EntityID> enttsInComponent;

	switch (componentType)
	{
		case ComponentType::TransformComponent:
		{
			const std::vector<EntityID>& enttsInComponent = entityMgr.transformSystem_.GetAllEnttsIDsFromTransformComponent();
			return CheckArrContainsIDs(enttsInComponent, enttsIDsToCheck);
		}
		case ComponentType::MoveComponent:
		{
			entityMgr.moveSystem_.GetEnttsIDsFromMoveComponent(enttsInComponent);
			break;
		}
		case ComponentType::MeshComp:
		{
			entityMgr.meshSystem_.GetEnttsIDsFromMeshComponent(enttsInComponent);
			break;
		}
		case ComponentType::RenderedComponent:
		{
			entityMgr.renderSystem_.GetEnttsIDsFromRenderedComponent(enttsInComponent);
			break;
		}
		case ComponentType::WorldMatrixComponent:
		{
			entityMgr.transformSystem_.GetAllEnttsIDsFromWorldMatrixComponent(enttsInComponent);
			break;
		}
		default:
		{
			Log::Error(LOG_MACRO, "Unknown component type: " + std::to_string(componentType));
			THROW_ERROR("can't check if the component has records with such entities");
		}
	}

	std::sort(enttsInComponent.begin(), enttsInComponent.end());
	return CheckArrContainsIDs(enttsInComponent, enttsIDsToCheck);
}

///////////////////////////////////////////////////////////

static XMMATRIX BuildMatrix(const XMFLOAT3& pos, const XMFLOAT3& dir, const XMFLOAT3& scale)
{
	return	DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scale)) *
			DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&dir)) *
			DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&pos));
}

bool static CheckFloat3Equal(const XMFLOAT3& f1, const XMFLOAT3& f2)
{
	return (f1.x == f2.x) && (f1.y == f2.y) && (f1.z == f2.z);
}

bool static CheckFloat4Equal(const XMFLOAT4& f1, const XMFLOAT4& f2)
{
	return (f1.x == f2.x) && (f1.y == f2.y) && (f1.z == f2.z) && (f1.w == f2.w);
}

};  // namespace Utils
