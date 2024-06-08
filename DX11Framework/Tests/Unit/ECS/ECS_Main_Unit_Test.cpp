#include "ECS_Main_Unit_Test.h"


#include "../../../Engine/log.h"
#include "../../../ECS_Components/Transform.h"

// tests related stuff
#include "UnitTestUtils.h"
#include "ECS_Test_Components.h"

#include <vector>
#include <DirectXMath.h>
#include <algorithm>


ECS_Main_Unit_Test::ECS_Main_Unit_Test()
{
	Log::Print("---------------- UNIT TESTS: ECS -----------------");
	Log::Print("");
}

void ECS_Main_Unit_Test::Test()
{
	ECS_Test_Components testComponents;  // unit tests for the ECS components

	try
	{

		TestEntitiesCreation();

		// test adding of each ECS component
		testComponents.TestTransformComponent();
		testComponents.TestMovementComponent();
		testComponents.TestMeshComponent();
		testComponents.TestRenderComponent();

		//exit(-1);
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
	// UNIT TEST: check if we can correctly create some amount of entities 
	//            inside the entity manager

	EntityManager entityMgr;
	UnitTestUtils utils;
	std::vector<entitiesName> entitiesIDs;

	// create entities with such IDs
	utils.EntitiesCreationHelper(entityMgr, entitiesIDs);

	// independently test if everything is OK 
	const std::set<entitiesName> createdEnttsIDs = entityMgr.GetAllEntitiesIDs();
	const std::set<entitiesName> expectedEnttsIDs{ entitiesIDs.begin(), entitiesIDs.end() };

	const bool enttMgrHasEntts = std::includes(createdEnttsIDs.begin(), createdEnttsIDs.end(), expectedEnttsIDs.begin(), expectedEnttsIDs.end());
	ASSERT_TRUE(enttMgrHasEntts, "the entity manager hasn't a record about some expected entity");

	// now test using the entity manager functional
	ASSERT_TRUE(entityMgr.CheckEntitiesExist(entitiesIDs), "the CheckEntitiesExist() doens't work correctly");
	ASSERT_TRUE(entityMgr.CheckEntityExist(entitiesIDs[0]), "the CheckEntityExist() doens't work correctly");
	ASSERT_TRUE(entityMgr.CheckEntityExist(entitiesIDs.back()), "the CheckEntityExist() doens't work correctly");




	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////
