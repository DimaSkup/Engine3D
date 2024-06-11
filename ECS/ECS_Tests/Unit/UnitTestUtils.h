// *********************************************************************************
// Filename:     UnitTestUtils.h
// Description:  contains different utils for ECS components testing
// 
// Created:      26.05.24
// *********************************************************************************

#pragma once


#include <vector>
#include <algorithm>

#include "../../ECS_Entity/EntityManager.h"
#include "../../ECS_Common/MathHelper.h"
#include "../../ECS_Common/log.h"
#include "../../ECS_Common/LIB_Exception.h"


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

static bool CheckComponentKnowsAboutEntities(
	EntityManager& entityMgr,
	const std::vector<EntityID>& enttsIDsToCheck,
	const ComponentType& componentType)
{
	// check if a component with componentType constains records with entities
	// from the input entitiesIDs array;
	// return: true - if the component knows each entity; 
	//         false - if the component doesn't know some entity

	std::vector<EntityID> enttsInComponent;

	switch (componentType)
	{
		case ComponentType::TransformComponent:
		{
			entityMgr.transformSystem_.GetEnttsIDsFromTransformComponent(enttsInComponent);
			break;
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
		default:
		{
			ECS::Log::Error("Unknown component type: " + std::to_string(componentType));
			THROW_ERROR("can't check if the component has records with such entities");
		}
	}

	// check if the component with componentType has records about input entities
	const auto beg = enttsInComponent.begin();
	const auto end = enttsInComponent.end();

	for (const EntityID& enttID : enttsIDsToCheck)
	{
		// if there is such an ID just continue loop-iteration
		if (std::find(beg, end, enttID) != end)
			continue;  

		// if we got here we didn't manage to find an ID
		ECS::Log::Error(LOG_MACRO, "There is no such entity ID: " + enttID);
		return false;  
	}

	return true;
}


}  // namespace Utils
