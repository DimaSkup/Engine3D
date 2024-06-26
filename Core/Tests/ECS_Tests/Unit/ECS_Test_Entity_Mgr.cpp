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

void CheckDeserializedEnttMgrData(
	const EntityManager& entityMgr,
	const std::vector<EntityID>& origEnttsIDs,
	const std::vector<ComponentFlagsType>& origCompFlags)
{
	// test data from the EntityManager: entities IDs, component flags, etc.
	bool isIDsDataCorrect = Utils::ContainerCompare(entityMgr.ids_, origEnttsIDs);
	const bool isCompFlagsDataCorrect = Utils::ContainerCompare(entityMgr.componentFlags_, origCompFlags);

	ASSERT_TRUE(isIDsDataCorrect, "TEST ENTITY MANAGER: deserialized entities IDs data isn't correct");
	ASSERT_TRUE(isCompFlagsDataCorrect, "TEST ENTITY MANAGER: deserialized component flags data isn't correct");
}

void CheckDeserializedTransformData(
	const EntityManager& entityMgr,
	const TransformData& transform)
{
	// test deserialized  data from the Transform component
	const bool isPosDataEqual = Utils::ContainerCompare(entityMgr.transform_.positions_, transform.positions);
	const bool isDirDataEqual = Utils::ContainerCompare(entityMgr.transform_.directions_, transform.directions);
	const bool isScaleDataEqual = Utils::ContainerCompare(entityMgr.transform_.scales_, transform.scales);

	ASSERT_TRUE(isPosDataEqual, "positions data of entts isn't equal to the expected values");
	ASSERT_TRUE(isDirDataEqual, "directions data of entts isn't equal to the expected values");
	ASSERT_TRUE(isScaleDataEqual, "scales data of entts isn't equal to the expected values");
}

void CheckDeserializedNameData(
	const EntityManager& entityMgr,
	const std::vector<EntityID>& enttsIDs,
	const std::vector<EntityName>& enttsNames)
{
	// test deserialized  data from the Name component
	const Name& nameComponent = entityMgr.names_;
	const bool isIDsDataCorrect = Utils::ContainerCompare(enttsIDs, nameComponent.ids_);
	const bool isNamesDataCorrect = Utils::ContainerCompare(enttsNames, nameComponent.names_);

	ASSERT_TRUE(isIDsDataCorrect, "the Name component doen't have a record about some entity (its ID)");
	ASSERT_TRUE(isNamesDataCorrect, "names data from the Name component isn't correct");
}

void CheckDeserializedMeshComponentData(
	EntityManager& entityMgr,
	const std::vector<EntityID>& enttsIDs, 
	const std::vector<MeshID>& meshesIDs)
{
	// test deserialized  data from the Mesh component
	std::vector<EntityID> enttsIDsFromMeshComponent;
	std::vector<MeshID> meshesIDsFromMeshComponent;

	entityMgr.meshSystem_.GetEnttsIDsFromMeshComponent(enttsIDsFromMeshComponent);
	entityMgr.meshSystem_.GetAllMeshesIDsFromMeshComponent(meshesIDsFromMeshComponent);

	const bool isEnttsIDsDataFromMeshCompCorrect = Utils::ContainerCompare(enttsIDs, enttsIDsFromMeshComponent);
	const bool isMeshesIDsDataFromMeshCompCorrect = Utils::ContainerCompare(meshesIDs, meshesIDsFromMeshComponent);

	ASSERT_TRUE(isEnttsIDsDataFromMeshCompCorrect, "the Mesh component doesn't have a record about some entity");
	ASSERT_TRUE(isMeshesIDsDataFromMeshCompCorrect, "the Mesh component doesn't have a record about some mesh");
}

void CheckDeserializedRenderedComponentData(
	EntityManager& entityMgr,
	const std::vector<EntityID>& enttsIDs,
	const RenderedData& renderedData)
{
	// test deserialized data from the Rendered component
	const Rendered& component = entityMgr.renderComponent_;
	const bool isIDsDataCorrect = Utils::ContainerCompare(component.ids_, enttsIDs);
	const bool isShaderTypesDataCorrect = Utils::ContainerCompare(component.shaderTypes_, renderedData.shaderTypes);
	const bool isTopologiesDataCorrect = Utils::ContainerCompare(component.primTopologies_, renderedData.primTopologyTypes);

	ASSERT_TRUE(isIDsDataCorrect, "TEST SYSTEMS: deserialized IDs data isn't correct");
	ASSERT_TRUE(isShaderTypesDataCorrect, "TEST SYSTEMS: deserialized shader types data isn't correct");
	ASSERT_TRUE(isTopologiesDataCorrect, "TEST SYSTEMS: deserialized primitive topologies data isn't correct");
}

///////////////////////////////////////////////////////////

void ECS_Test_Entity_Mgr::TestEntityMgrSerializationDeserialization()
{
	// test the EntityManager for correct serialization/deserialization 
	// of its own data (all the entities IDs / related to entts components / etc.)

	const UINT enttsCount = 50;                        // how many entts will we create
	const std::vector<MeshID> meshesIDs{ 1,2,3,4,5 };  // like each entity has these meshes
	std::vector<EntityName> enttsNames;

	TransformData transform;
	MoveData move;
	RenderedData rendered;
	EntityManager origEntityMgr;
	EntityManager deserialEntityMgr;


	// prepare data for the components
	Utils::PrepareRandomTransformData(enttsCount, transform);
	Utils::GenerateEnttsNames(enttsCount, 10, enttsNames);
	Utils::PrepareRandomMovementData(enttsCount, move);
	Utils::PrepareRandomRenderedData(enttsCount, rendered);

	// create entities and add components to them
	const std::vector<EntityID> enttsIDs = origEntityMgr.CreateEntities(enttsCount);
	origEntityMgr.AddTransformComponent(enttsIDs, transform.positions, transform.directions, transform.scales);
	origEntityMgr.AddNameComponent(enttsIDs, enttsNames);
	origEntityMgr.AddMoveComponent(enttsIDs, move.translations, move.rotQuats, move.scaleChanges);
	origEntityMgr.AddMeshComponent(enttsIDs, meshesIDs);
	origEntityMgr.AddRenderingComponent(enttsIDs, rendered.shaderTypes, rendered.primTopologyTypes);

	// ---------------------------------------------

	// store some origin data before serialization so later we'll use it for testing
	const std::vector<ComponentFlagsType> origComponentFlags = origEntityMgr.componentFlags_;
	
	// serialize and deserialize data from the entity manager
	const std::string dataFilepath = "test_entity_mgr_serialization.bin";
	origEntityMgr.Serialize(dataFilepath);         // serialize all the data from the origin entity manager
	deserialEntityMgr.Deserialize(dataFilepath);   // deserialize data into the new entity manager
	Utils::RemoveFile(dataFilepath);

	// ---------------------------------------------

	// check if all the deserialized data is correct
	CheckDeserializedEnttMgrData(deserialEntityMgr, enttsIDs, origComponentFlags);
	CheckDeserializedTransformData(deserialEntityMgr, transform);
	CheckDeserializedNameData(deserialEntityMgr, enttsIDs, enttsNames);
	CheckDeserializedMeshComponentData(deserialEntityMgr, enttsIDs, meshesIDs);
	CheckDeserializedRenderedComponentData(deserialEntityMgr, enttsIDs, rendered);

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