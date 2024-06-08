// *********************************************************************************
// Filename:     UnitTestUtils.h
// Description:  contains different utils for ECS components testing
// 
// Created:      26.05.24
// *********************************************************************************

#pragma once

#include "../../../ECS_Entity/EntityManager.h"
#include <vector>
#include <algorithm>
#include "../Common/MathHelper.h"


using XMFLOAT3 = DirectX::XMFLOAT3;
using XMFLOAT4 = DirectX::XMFLOAT4;

namespace Utils
{

void EntitiesCreationHelper(
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

	// create this bunch of entities
	entityMgr.CreateEntities(outEnttsNames);
}

///////////////////////////////////////////////////////////

void PrepareRandomDataForArray(
	const size_t arrSize,
	std::vector<XMFLOAT3>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.resize(arrSize);
	
	for (DirectX::XMFLOAT3& elem : outArr)
		elem = { MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF() };
}

///////////////////////////////////////////////////////////

void PrepareRandomDataForArray(
	const size_t arrSize,
	std::vector<XMFLOAT4>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.resize(arrSize);

	for (DirectX::XMFLOAT4& elem : outArr)
		elem = { MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF() };
}

///////////////////////////////////////////////////////////

bool CheckComponentKnowsAboutEntities(
	EntityManager& entityMgr,
	const std::vector<EntityID>& enttsIDs,
	const ComponentType& componentType)
{
	// check if a component with componentType constains records with entities
	// from the input entitiesIDs array;
	// return: true - if the component knows each entity; 
	//         false - if the component doesn't know some entity

	std::set<entitiesName> enttInComponent;

	switch (componentType)
	{
		case ComponentType::TransformComponent:
		{
			enttInComponent = entityMgr.transformSystem_.GetEntitiesIDsSet();
			break;
		}
		case ComponentType::MoveComponent:
		{
			enttInComponent = entityMgr.MoveSystem_.GetEntitiesIDsSet();
			break;
		}
		case ComponentType::MeshComp:
		{
			enttInComponent = entityMgr.meshSystem_.GetEntitiesIDsSet();
			break;
		}
		case ComponentType::RenderedComponent:
		{
			enttInComponent = entityMgr.renderSystem_.GetEntitiesIDsSet();
			break;
		}
		default:
		{
			Log::Error("Unknown component type: " + std::to_string(componentType));
			THROW_ERROR("can't check if the component has records with such entities");
		}
	}

	// check if the component with componentType has records about input entities
	const std::set<entitiesName> entitiesIDsSet{ entitiesIDs.begin(), entitiesIDs.end() };
	return std::includes(enttInComponent.begin(), enttInComponent.end(), entitiesIDsSet.begin(), entitiesIDsSet.end());
}


}  // namespace Utils
