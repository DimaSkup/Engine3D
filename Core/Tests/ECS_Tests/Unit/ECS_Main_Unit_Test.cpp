#include "ECS_Main_Unit_Test.h"
#include "ECS_Entity/EntityManager.h"
#include "../Engine/log.h"

// tests related stuff
#include "UnitTestUtils.h"
#include "ECS_Test_Components.h"
#include "ECS_Test_Systems.h"

#include <vector>
#include <DirectXMath.h>
#include <algorithm>


ECS_Main_Unit_Test::ECS_Main_Unit_Test()
{
	Log::Print("");
	Log::Print("---------------- UNIT TESTS: ECS -----------------");
	Log::Print("");
}

ECS_Main_Unit_Test::~ECS_Main_Unit_Test()
{
	Log::Print("--------------------------------------------------");
	Log::Print("");
}

// *********************************************************************************

void ECS_Main_Unit_Test::Test()
{
	ECS_Test_Components testComponents;  // unit tests for the ECS components
	ECS_Test_Systems testSystems;

	try
	{
		TestEntitiesCreation();

		// test each ECS component
		testComponents.TestTransformComponent();
		testComponents.TestMovementComponent();
		testComponents.TestMeshComponent();
		testComponents.TestRenderComponent();

		// test each ECS system
		testSystems.TestSerializationDeserialization();

		Log::Print("");
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		exit(-1);
	}
}

///////////////////////////////////////////////////////////

void ECS_Main_Unit_Test::TestEntitiesCreation()
{
	// UNIT TEST: check if we can correctly create some amount of empty entities 
	//            inside the entity manager

	const size_t newEnttsCount = 50;    // create this number of new empty entities
	EntityManager entityMgr;

	// create a bunch of entities
	entityMgr.CreateEntities(newEnttsCount);
	const std::vector<EntityID>& createdEnttsIDs = entityMgr.GetAllEnttsIDs();

	// test if everything is OK using the EntityManager functional
	ASSERT_TRUE(entityMgr.CheckEnttsByIDsExist(createdEnttsIDs), "the CheckEnttsByIDsExist() doens't work correctly");

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////
