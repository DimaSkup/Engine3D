#include "UnitTestMain.h"
#include "Entity/EntityManager.h"
#include "../Engine/log.h"

// tests related stuff
#include "Utils.h"
#include "TestComponents.h"
#include "TestSystems.h"
#include "TestEntityMgr.h"

#include <vector>
#include <DirectXMath.h>
#include <algorithm>


UnitTestMain::UnitTestMain()
{
	Log::Print("");
	Log::Print("---------------  UNIT TESTS: ECS  ----------------");
	Log::Print("");
}

UnitTestMain::~UnitTestMain()
{
	Log::Print("--------------------------------------------------");
	Log::Print("");
}

// *********************************************************************************

void UnitTestMain::Run()
{
	TestEntityMgr testEntityMgr;
	TestComponents testComponents;  // unit tests for the ECS components
	TestSystems testSystems;

	try
	{
		// test each ECS component
		testComponents.Run();

		// test each ECS system
		testSystems.Run();

		// test the EntityManager
		Log::Print("----------------  TESTS: EntityManager -----------------");
		Log::Print("");

		testEntityMgr.TestEntitiesCreation();
		testEntityMgr.TestSerialDeserial();

		Log::Print("");
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		exit(-1);
	}
}

///////////////////////////////////////////////////////////
