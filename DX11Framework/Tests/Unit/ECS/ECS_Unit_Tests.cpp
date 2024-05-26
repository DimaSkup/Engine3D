#include "ECS_Unit_Tests.h"


#include "../../../Engine/log.h"
#include "../../../ECS_Components/Transform.h"

#include <vector>
#include <DirectXMath.h>
#include <algorithm>


ECS_Unit_Tests::ECS_Unit_Tests()
{
	Log::Print("---------------- UNIT TESTS: ECS -----------------");
	Log::Print("");
}

void ECS_Unit_Tests::Test()
{
	TestEntitiesCreation();
	TestAddTransformComponent();
	TestAddMovementComponent();
}

///////////////////////////////////////////////////////////

void ECS_Unit_Tests::TestEntitiesCreation()
{
	// check if we can create some amount of entities inside the entity manager

	EntityManager entityMgr;
	std::vector<EntityID> entityIDs;

	EntitiesCreationHelper(entityMgr, entityIDs);

	// check if there is such entity inside the manager
	for (const EntityID& entityID : entityIDs)
	{
		if (!entityMgr.HasEntity(entityID))
		{
			Log::Error(LOG_MACRO, "there is no entity by such ID: " + entityID);
			exit(-1);
		}
	}
	
	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Unit_Tests::TestAddTransformComponent()
{
	try
	{
		EntityManager entityMgr;
		const std::string componentID{ "Transform" };
		std::vector<EntityID> entityIDs;                   // all entities IDs
		std::vector<EntityID> entitiesHaveComponent;
		std::vector<EntityID> entitiesDontHaveComponent;


		EntitiesCreationHelper(entityMgr, entityIDs);
		AddTransformComponentHelper(entityMgr, entityIDs);

		// check if we add a component to the proper number of entities
		const std::set<EntityID> entitiesWithComponent = entityMgr.GetComponent(componentID)->GetEntitiesIDsSet();
		ASSERT_TRUE(entityIDs.size() == entitiesWithComponent.size(), "the number of supposed entities with the transform component is not equal to the actual number");

		// check if the component know about each entity
		for (const EntityID& entityID : entityIDs)
		{
			ASSERT_TRUE(entitiesWithComponent.contains(entityID), "there is no record with such entity (" + entityID + ") inside the transform component");
		}

		// check if there is a records ['entity_id' => 'component_id] inside the manager
		for (const EntityID& entityID : entityIDs)
		{
			const bool has = entityMgr.HasComponent(entityID, componentID);
			ASSERT_TRUE(has, "entity (" + entityID + ") must have the " + componentID + " component");
		}
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		return;
	}

	Log::Print(LOG_MACRO, "\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Unit_Tests::TestAddMovementComponent()
{
	try
	{
		EntityManager entityMgr;
		const std::string componentID{ "Movement" };
		std::vector<EntityID> entityIDs;                   // all entities IDs
		std::vector<EntityID> entitiesHaveComponent;


		EntitiesCreationHelper(entityMgr, entityIDs);
		AddTransformComponentHelper(entityMgr, entityIDs);
		AddMovementComponentHelper(entityMgr, entityIDs);

		// check if we add a component to the proper number of entities
		const std::set<EntityID> entitiesWithComponent = entityMgr.GetComponent(componentID)->GetEntitiesIDsSet();
		ASSERT_TRUE(entityIDs.size() == entitiesWithComponent.size(), "the number of supposed entities with the transform component is not equal to the actual number");

		// check if the component know about each entity
		for (const EntityID& entityID : entityIDs)
		{
			ASSERT_TRUE(entitiesWithComponent.contains(entityID), "there is no record with such entity (" + entityID + ") inside the transform component");
		}

		// check if entities have the transform component
		for (const EntityID& entityID : entitiesHaveComponent)
		{
			const bool has = entityMgr.HasComponent(entityID, componentID);
			ASSERT_TRUE(has, "entity (" + entityID + ") must have the " + componentID + " component");
		}
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		return;
	}

	Log::Print(LOG_MACRO, "\tPASSED");
}


// ********************************************************************************
// 
//                           PRIVATE HELPERS
// 
// ********************************************************************************

void ECS_Unit_Tests::EntitiesCreationHelper(
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

void ECS_Unit_Tests::AddTransformComponentHelper(
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

void ECS_Unit_Tests::AddMovementComponentHelper(
	EntityManager& entityMgr,
	const std::vector<EntityID>& entityIDs)
{
	// prepare default movement data for the entities
	std::vector<DirectX::XMFLOAT3> translations(entityIDs.size(), { 0,0,0 });
	std::vector<DirectX::XMFLOAT4> rotQuats(entityIDs.size(), { 0,0,0,0 });
	std::vector<DirectX::XMFLOAT3> scaleChanges(entityIDs.size(), { 1,1,1 });

	entityMgr.AddMovementComponents(entityIDs, translations, rotQuats, scaleChanges);
}