#include "ECS_Main_Unit_Test.h"
#include "ECS_Entity/EntityManager.h"
#include "../Engine/log.h"

// tests related stuff
#include "UnitTestUtils.h"
#include "ECS_Test_Components.h"
#include "ECS_Test_Systems.h"
#include "ECS_Test_Entity_Mgr.h"

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

void ECS_Main_Unit_Test::Run()
{
	ECS_Test_Entity_Mgr testEntityMgr;
	ECS_Test_Components testComponents;  // unit tests for the ECS components
	ECS_Test_Systems testSystems;

	try
	{
		

		// test each ECS component
		testComponents.TestTransformComponent();
		testComponents.TestNameComponent();
		testComponents.TestMoveComponent();
		testComponents.TestMeshComponent();
		testComponents.TestRenderComponent();

		// test each ECS system
		testSystems.TestSerializationDeserialization();

		// test the EntityManager
		Log::Print("----------------  TESTS: EntityManager -----------------");
		Log::Print("");

		testEntityMgr.TestEntityMgrEntitiesCreation();
		testEntityMgr.TestEntityMgrSerializationDeserialization();

		Log::Print("");
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		exit(-1);
	}
}

///////////////////////////////////////////////////////////
