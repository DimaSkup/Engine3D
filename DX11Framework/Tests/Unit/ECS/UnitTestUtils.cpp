#include "UnitTestUtils.h"
#include <algorithm>

using namespace DirectX;


void UnitTestUtils::EntitiesCreationHelper(
	EntityManager& entityMgr,
	std::vector<EntityID>& entityIDs)
{
	// generate unique ID for each entity and create entities inside the manager

	const std::vector<std::string> prefixes{ "sphere_", "cylinder_", "cube_" };
	const UINT entitiesCountPerPrefix = 10;

	// IDs generation
	for (const std::string& prefix : prefixes)
	{
		for (UINT idx = 0; idx < entitiesCountPerPrefix; ++idx)
		{
			entityIDs.push_back({ prefix + std::to_string(idx) });
		}
	}

	// create this bunch of entities
	entityMgr.CreateEntities(entityIDs);
}

///////////////////////////////////////////////////////////

void UnitTestUtils::PrepareRandomDataForArray(
	const size_t arrSize,
	std::vector<DirectX::XMFLOAT3>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.resize(arrSize);
	
	for (DirectX::XMFLOAT3& elem : outArr)
		elem = { MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF() };
}

void UnitTestUtils::PrepareRandomDataForArray(
	const size_t arrSize,
	std::vector<DirectX::XMFLOAT4>& outArr)
{
	// go through each element of the array and generate for it random data
	outArr.resize(arrSize);

	for (DirectX::XMFLOAT4& elem : outArr)
		elem = { MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF() };
}

///////////////////////////////////////////////////////////

bool UnitTestUtils::CheckEntitiesHaveComponent(
	EntityManager& entityMgr,
	const std::vector<EntityID>& entityIDs,
	const ComponentType& componentType)
{
	// check if there is a records ['entity_id' => 'component_type'] inside the manager
	for (const EntityID& entityID : entityIDs)
	{
		if (!entityMgr.CheckEntityHasComponent(entityID, componentType))
			return false;	
	}
	return true;
}

///////////////////////////////////////////////////////////

bool UnitTestUtils::CheckComponentKnowsAboutEntities(
	EntityManager& entityMgr,
	const std::vector<EntityID>& entitiesIDs,
	const ComponentType& componentType)
{
	// check if a component with componentType constains records with entities
	// from the input entitiesIDs array;
	// return: true - if the component knows each entity; 
	//         false - if the component doesn't know some entity

	std::set<EntityID> enttInComponent;

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
	const std::set<EntityID> entitiesIDsSet{ entitiesIDs.begin(), entitiesIDs.end() };
	return std::includes(enttInComponent.begin(), enttInComponent.end(), entitiesIDsSet.begin(), entitiesIDsSet.end());
}
