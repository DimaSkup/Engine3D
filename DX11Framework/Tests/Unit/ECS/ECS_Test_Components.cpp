#include "ECS_Test_Components.h"

#include "UnitTestUtils.h"


void ECS_Test_Components::TestAddTransformComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the transform component to entities

	try
	{
		EntityManager entityMgr;
		UnitTestUtils utils;
		const std::string componentID{ "Transform" };
		std::vector<EntityID> entityIDs;                   // all entities IDs

		utils.EntitiesCreationHelper(entityMgr, entityIDs);
		utils.AddTransformComponentHelper(entityMgr, entityIDs);
		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, componentID);

	}
	catch (EngineException& e)
	{
		Log::Error(e);
		return;
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestAddMovementComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the movement component to entities

	try
	{
		EntityManager entityMgr;
		UnitTestUtils utils;

		const std::string transformComponentID{ "Transform" };
		const std::string movementComponentID{ "Movement" };
		std::vector<EntityID> entityIDs;                   // all entities IDs

		utils.EntitiesCreationHelper(entityMgr, entityIDs);
		utils.AddTransformComponentHelper(entityMgr, entityIDs);
		utils.AddMovementComponentHelper(entityMgr, entityIDs);

		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, transformComponentID);
		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, movementComponentID);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		return;
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestAddMeshComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the mesh component to entities

	try
	{
		EntityManager entityMgr;
		UnitTestUtils utils;

		const std::string transformComponentID{ "Transform" };
		const std::string meshComponentID{ "MeshComponent" };
		const std::vector<MeshID> meshesIDs{ "meshID" };
		std::vector<EntityID> entityIDs;                   // all entities IDs

		utils.EntitiesCreationHelper(entityMgr, entityIDs);
		utils.AddTransformComponentHelper(entityMgr, entityIDs);
		entityMgr.AddMeshComponents(entityIDs, meshesIDs);

		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, transformComponentID);
		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, meshComponentID);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		return;
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestAddRenderComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the render component to entities

	try
	{
		EntityManager entityMgr;
		UnitTestUtils utils;

		const std::string transformComponentID{ "Transform" };
		const std::string meshComponentID{ "MeshComponent" };
		const std::string renderComponentID{ "Rendered" };
		const std::vector<MeshID> meshesIDs{ "meshID" };
		std::vector<EntityID> entityIDs;                   // all entities IDs
		

		utils.EntitiesCreationHelper(entityMgr, entityIDs);
		utils.AddTransformComponentHelper(entityMgr, entityIDs);
		entityMgr.AddMeshComponents(entityIDs, meshesIDs);
		entityMgr.AddRenderingComponents(entityIDs);

		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, transformComponentID);
		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, meshComponentID);
		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, renderComponentID);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		return;
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}