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

	TestEntitiesCreation();

	// test adding of each ECS component
	testComponents.TestAddTransformComponent();
	testComponents.TestAddMovementComponent();
	testComponents.TestAddMeshComponent();
	testComponents.TestAddRenderComponent();
}

///////////////////////////////////////////////////////////

void ECS_Main_Unit_Test::TestEntitiesCreation()
{
	// UNIT TEST: check if we can create some amount of entities 
	//            inside the entity manager

	EntityManager entityMgr;
	UnitTestUtils utils;
	std::vector<EntityID> entityIDs;

	// create entities with such IDs
	utils.EntitiesCreationHelper(entityMgr, entityIDs);

	// check if there is such entity inside the manager
	for (const EntityID& entityID : entityIDs)
	{
		const bool has = entityMgr.CheckEntityExist(entityID);
		ASSERT_TRUE(has, "there is no entity by such ID: " + entityID);
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////
