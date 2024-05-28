#include "UnitTestUtils.h"




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

void UnitTestUtils::CheckEntitiesHaveComponent(
	EntityManager& entityMgr,
	const std::vector<EntityID>& entityIDs,
	const ComponentID& componentID)
{
	// 1. check if we add a component to the proper number of entities
	const std::set<EntityID> entitiesWithComponent = entityMgr.GetComponent(componentID)->GetEntitiesIDsSet();
	ASSERT_TRUE(entityIDs.size() == entitiesWithComponent.size(), "the number of supposed entities with the transform component is not equal to the actual number");

	// 2. check if the component know about each entity
	for (const EntityID& entityID : entityIDs)
	{
		ASSERT_TRUE(entitiesWithComponent.contains(entityID), "there is no record with such entity (" + entityID + ") inside the transform component");
	}

	// 3. check if there is a records ['entity_id' => 'component_id] inside the manager
	for (const EntityID& entityID : entityIDs)
	{
		const bool has = entityMgr.CheckEntityHasComponent(entityID, componentID);
		ASSERT_TRUE(has, "entity (" + entityID + ") must have the " + componentID + " component");
	}
}

///////////////////////////////////////////////////////////

void UnitTestUtils::AddTransformComponentHelper(
	EntityManager& entityMgr,
	const std::vector<EntityID>& entityIDs)  // to these entities we will add the component
{
	// prepare default transform data for the entities
	std::vector<DirectX::XMFLOAT3> positions(entityIDs.size(), { 0,0,0 });
	std::vector<DirectX::XMFLOAT3> directions(entityIDs.size(), { 0,0,0 });
	std::vector<DirectX::XMFLOAT3> scales(entityIDs.size(), { 1,1,1 });

	entityMgr.AddTransformComponents(entityIDs, positions, directions, scales);
}

///////////////////////////////////////////////////////////

void UnitTestUtils::AddMovementComponentHelper(
	EntityManager& entityMgr,
	const std::vector<EntityID>& entityIDs)
{
	// prepare default movement data for the entities
	std::vector<DirectX::XMFLOAT3> translations(entityIDs.size(), { 0,0,0 });
	std::vector<DirectX::XMFLOAT4> rotQuats(entityIDs.size(), { 0,0,0,0 });
	std::vector<DirectX::XMFLOAT3> scaleChanges(entityIDs.size(), { 1,1,1 });

	entityMgr.AddMovementComponents(entityIDs, translations, rotQuats, scaleChanges);
}