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
#include "../Engine/log.h"
#include "../Engine/EngineException.h"


namespace Utils
{

static void GenerateEnttsNames(
	EntityManager& entityMgr,
	std::vector<EntityName>& outEnttsNames)
{
	// generate unique ID for each entity and create entities inside the manager
	// out: array of entities names;

	const std::vector<std::string> prefixes{ "sphere_", "cylinder_", "cube_" };
	const UINT enttsCountPerPrefix = 10;

	outEnttsNames.reserve(prefixes.size() * enttsCountPerPrefix);

	// generation of names
	for (const std::string& prefix : prefixes)
	{
		for (UINT idx = 0; idx < enttsCountPerPrefix; ++idx)
		{
			outEnttsNames.push_back({ prefix + std::to_string(idx) });
		}
	}
}

///////////////////////////////////////////////////////////

static void PrepareRandomDataForArray(
	const size_t arrSize,
	std::vector<XMFLOAT3>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.resize(arrSize);
	
	for (DirectX::XMFLOAT3& elem : outArr)
		elem = { MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF() };
}

///////////////////////////////////////////////////////////

static void PrepareRandomDataForArray(
	const size_t arrSize,
	std::vector<XMFLOAT4>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.resize(arrSize);

	for (DirectX::XMFLOAT4& elem : outArr)
		elem = { MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF() };
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
			const std::vector<EntityID>& enttsInComponent = entityMgr.transformSystem_.GetEnttsIDsFromTransformComponent();
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
			entityMgr.transformSystem_.GetEnttsIDsFromWorldMatrixComponent(enttsInComponent);
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

}  // namespace Utils
