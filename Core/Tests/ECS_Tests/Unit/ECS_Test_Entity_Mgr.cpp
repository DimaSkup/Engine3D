#include "ECS_Test_Entity_Mgr.h"
#include "UnitTestUtils.h"

#include "ECS_Entity/EntityManager.h"
#include "../Engine/EngineException.h"
#include "../Engine/log.h"

void ECS_Test_Entity_Mgr::TestEntityMgrEntitiesCreation()
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

void ECS_Test_Entity_Mgr::TestEntityMgrSerializationDeserialization()
{
	// test the EntityManager for correct serialization/deserialization 
	// of its own data (all the entities IDs / related to entts components / etc.)

	EntityManager entityMgr;
	const UINT enttsCount = 10;                      // how many entts will we create
	const std::vector<ComponentType> componentTypes  // which components each entity will have
	{
		ComponentType::TransformComponent,
		ComponentType::MeshComp,
		ComponentType::RenderedComponent
	};

	// create entities and add components to them
	const std::vector<EntityID> enttsIDs = entityMgr.CreateEntities(enttsCount);

	for (const ComponentType& type : componentTypes)
		entityMgr.SetEnttsHaveComponent(enttsIDs, type);

	// store the original data of the entity mgr
	const std::vector<EntityID> origEnttsIDs = entityMgr.ids_;
	const std::vector<uint32_t> origComponentsFlags = entityMgr.componentFlags_;

	// serialize and deserialize data from the entity manager
	entityMgr.Serialize();
	entityMgr.Deserialize();

	//
	// check if deserialized data is correct
	//
	const std::vector<EntityID>& deserialIDs = entityMgr.ids_;
	const std::vector<ComponentFlagsType>& deserialCompFlags = entityMgr.componentFlags_;

	const bool isIDsDataCorrect = Utils::ContainerCompare(deserialIDs, origEnttsIDs);
	const bool isCompFlagsDataCorrect = Utils::ContainerCompare(deserialCompFlags, origComponentsFlags);

	ASSERT_TRUE(isIDsDataCorrect, "TEST ENTITY MANAGER: deserialized entities IDs data isn't correct");
	ASSERT_TRUE(isCompFlagsDataCorrect, "TEST ENTITY MANAGER: deserialized component flags data isn't correct");

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////
#if 0
void ECS_Test_Entity_Mgr::SeedEnttMgr(
	EntityManager& entityMgr,
	const UINT enttsCount,
	const std::vector<ComponentType>& componentTypes)
{
	// seed the input entity manager with fake data in purpose of testing;
	// create entities in quantity enttsCount and add to each of it the 
	// components by types from componentTypes arr; each component are filled in
	// with randomly generated data;


}

#endif