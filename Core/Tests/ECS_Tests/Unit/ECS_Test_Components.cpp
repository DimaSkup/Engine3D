#include "ECS_Test_Components.h"
#include "UnitTestUtils.h"
#include "HelperTypes.h"

using namespace DirectX;



// ********************************************************************************
//                          PUBLIC TESTS FUNCTIONS
// ********************************************************************************

void ECS_Test_Components::TestTransformComponent()
{
	// UNIT TEST: check behaviour of the ECS when we use the Transform component;

	try
	{
		TestAddingTransformComponent();
		TestTransformComponentData();
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test with the Transform component");
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestNameComponent()
{
	// here we test functional of the EntityManager about the Name component;

	try
	{
		const size_t enttsCount = 50;   // create this number of new empty entts
		const size_t nameLength = 10;
		EntityManager entityMgr;
		std::vector<EntityName> names;
		std::vector<EntityID> ids;

		// generate random unique names for the entities
		Utils::GenerateEnttsNames(enttsCount, nameLength, names);

		ids = entityMgr.CreateEntities(enttsCount);
		entityMgr.AddNameComponent(ids, names);

		//
		// test everything is OK
		//
		const Name& nameComponent = entityMgr.names_;

		const bool enttsHaveNames = Utils::CheckEnttsHaveComponent(entityMgr, ids, ComponentType::NameComponent);
		const bool isIDsDataCorrect = Utils::ContainerCompare(ids, nameComponent.ids_);
		const bool isNamesDataCorrect = Utils::ContainerCompare(names, nameComponent.names_);

		ASSERT_TRUE(enttsHaveNames, "some entity doesn't have the Name component");
		ASSERT_TRUE(isIDsDataCorrect, "the Name component doen't have a record about some entity (its ID)");
		ASSERT_TRUE(isNamesDataCorrect, "names data from the Name component isn't correct");
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test for the Name component");
	}

	Log::Print(LOG_MACRO, "\t\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestMoveComponent()
{
	// UNIT TEST: check behaviour of the ECS when we use the Move component;

	try
	{
		const size_t newEnttsCount = 50;   // create this number of new empty entts
		EntityManager entityMgr;
		MoveData move;
		std::vector<EntityID> ids;

		ids = entityMgr.CreateEntities(newEnttsCount);

		Utils::PrepareRandomMovementData(newEnttsCount, move);
		entityMgr.AddMoveComponent(ids, move.translations, move.rotQuats, move.scaleChanges);

		// TEST EVERYTHING IS OK
		const Movement& moveComponent = entityMgr.movement_;

		const bool enttsHaveMove             = Utils::CheckEnttsHaveComponent(entityMgr, ids, ComponentType::MoveComponent);
		const bool movementKnowsEntts        = Utils::ContainerCompare(ids, moveComponent.ids_);
		const bool isTranslationsDataCorrect = Utils::ContainerCompare(move.translations, moveComponent.translations_);
		const bool isRotQuatsDataCorrect     = Utils::ContainerCompare(move.rotQuats, moveComponent.rotationQuats_);
		const bool isScaleChangesDataCorrect = Utils::ContainerCompare(move.scaleChanges, moveComponent.scaleChanges_);

		ASSERT_TRUE(enttsHaveMove, "some entity doesn't have the Move component");
		ASSERT_TRUE(movementKnowsEntts, "the Move component doen't have a record about some entity");
		ASSERT_TRUE(isTranslationsDataCorrect, "translations data from the Movement component isn't correct");
		ASSERT_TRUE(isRotQuatsDataCorrect, "rotations quaternions data from the Movement component isn't correct");
		ASSERT_TRUE(isScaleChangesDataCorrect, "scale changes data from the Movement component isn't correct");
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test for the Move component");
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestMeshComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the mesh component to entities

	try
	{
		const size_t newEnttsCount = 50;   // create this number of new empty entts
		const std::vector<MeshID> meshesIDs{ 1,2,3,4 };
		EntityManager entityMgr;

		const std::vector<EntityID> enttsIDs = entityMgr.CreateEntities(newEnttsCount);
		entityMgr.AddMeshComponent(enttsIDs, meshesIDs);

		//
		// TEST EVERYTHING IS OK
		//
		std::vector<EntityID> enttsIDsFromMeshComponent;
		std::vector<MeshID> meshesIDsFromMeshComponent;

		entityMgr.meshSystem_.GetEnttsIDsFromMeshComponent(enttsIDsFromMeshComponent);
		entityMgr.meshSystem_.GetAllMeshesIDsFromMeshComponent(meshesIDsFromMeshComponent);

		// check if component flags are correct, 
		// also check if there is proper data inside the Mesh component
		const bool enttsHaveMeshComponent   = Utils::CheckEnttsHaveComponent(entityMgr, enttsIDs, ComponentType::MeshComp);
		const bool meshComponentKnowsEntts  = Utils::ContainerCompare(enttsIDs, enttsIDsFromMeshComponent);
		const bool meshComponentKnowsMeshes = Utils::ContainerCompare(meshesIDs, meshesIDsFromMeshComponent);

		ASSERT_TRUE(enttsHaveMeshComponent, "some entity doesn't have the Mesh component");
		ASSERT_TRUE(meshComponentKnowsEntts, "the Mesh component doesn't have a record about some entity");
		ASSERT_TRUE(meshComponentKnowsMeshes, "the Mesh component doesn't have a record about some mesh");
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test with the Mesh component");
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestRenderComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the render component to entities

	try
	{
		const size_t enttsCount = 50;   // create this number of new empty entts
		EntityManager entityMgr;
		RenderedData rendered;

		const std::vector<EntityID> enttsIDs = entityMgr.CreateEntities(enttsCount);

		Utils::PrepareRandomRenderedData(enttsCount, rendered);
		entityMgr.AddRenderingComponent(enttsIDs, rendered.shaderTypes, rendered.primTopologyTypes);

		//
		// TEST EVERYTHING IS OK
		//
		const Rendered& renderComponent = entityMgr.renderComponent_;

		const bool enttsHaveRenderedComponent = Utils::CheckEnttsHaveComponent(entityMgr, enttsIDs, ComponentType::RenderedComponent);
		const bool isIDsDataCorrect           = Utils::ContainerCompare(enttsIDs, renderComponent.ids_);
		const bool isShaderTypesDataCorrect   = Utils::ContainerCompare(rendered.shaderTypes, renderComponent.shaderTypes_);
		const bool isPrimTopologyTypesCorrect = Utils::ContainerCompare(rendered.primTopologyTypes, renderComponent.primTopologies_);

		ASSERT_TRUE(enttsHaveRenderedComponent, "some entity doesn't have the Rendered component");
		ASSERT_TRUE(isIDsDataCorrect, "ids data isn't correct");
		ASSERT_TRUE(isShaderTypesDataCorrect, "shader types data isn't correct");
		ASSERT_TRUE(isPrimTopologyTypesCorrect, "primitive topology types data isn't correct");
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test with the Rendered component");
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}




// ************************************************************************************
// 
//                              PRIVATE HELPERS
// 
// ************************************************************************************


void ECS_Test_Components::TestAddingTransformComponent()
{
	// here we check if everything works correct about adding 
	// the Transform and WorldMatrix components to the created entities

	const size_t newEnttsCount = 50;   // create this number of new empty entts
	EntityManager entityMgr;
	TransformData transform;

	entityMgr.CreateEntities(newEnttsCount);
	const std::vector<EntityID>& enttsIDs = entityMgr.GetAllEnttsIDs();

	Utils::PrepareRandomTransformData(newEnttsCount, transform);
	entityMgr.AddTransformComponent(enttsIDs, transform.positions, transform.directions, transform.scales);

	// TEST EVERYTHING IS OK
	std::vector<EntityID> enttsIDsFromWorldMatComponent;
	entityMgr.transformSystem_.GetAllEnttsIDsFromWorldMatrixComponent(enttsIDsFromWorldMatComponent);

	const bool enttsHaveTransform = Utils::CheckEnttsHaveComponent(entityMgr, enttsIDs, ComponentType::TransformComponent);
	const bool enttsHaveWorldMatrix = Utils::CheckEnttsHaveComponent(entityMgr, enttsIDs, ComponentType::TransformComponent);

	const bool isIDsDataFromTransformCorrect = Utils::ContainerCompare(enttsIDs, entityMgr.transform_.ids_);
	const bool isIDsDataFromWorldMatCorrect = Utils::ContainerCompare(enttsIDs, enttsIDsFromWorldMatComponent);

	ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
	ASSERT_TRUE(enttsHaveWorldMatrix, "some entity doesn't have the WorldMatrix component");
	ASSERT_TRUE(isIDsDataFromTransformCorrect, "the Transform component doesn't have a record about some entity");
	ASSERT_TRUE(isIDsDataFromWorldMatCorrect, "the WorldMatrix component doesn't have a record about some entity");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestTransformComponentData()
{
	// check if we can correctly store data into the Transform component and
	// correctly receive necessary data from this component;

	const UINT enttsCount = 10;
	EntityManager entityMgr;
	TransformData transformData;
	
	// create entities and add the Transform component to them
	const std::vector<EntityID> enttsIDs = entityMgr.CreateEntities(enttsCount);

	Utils::PrepareRandomTransformData(enttsCount, transformData);

	entityMgr.AddTransformComponent(
		enttsIDs,
		transformData.positions,
		transformData.directions,
		transformData.scales);

	// get data from the Transform component
	const std::vector<XMFLOAT3>& posArr = entityMgr.transform_.positions_;
	const std::vector<XMFLOAT3>& dirArr = entityMgr.transform_.directions_;
	const std::vector<XMFLOAT3>& scalesArr = entityMgr.transform_.scales_;

	//
	// check if transform data from the Transform component is equal to the expected values
	//
	const bool isPosDataEqual = Utils::ContainerCompare(posArr, transformData.positions);
	const bool isDirDataEqual = Utils::ContainerCompare(dirArr, transformData.directions);
	const bool isScaleDataEqual = Utils::ContainerCompare(scalesArr, transformData.scales);

	ASSERT_TRUE(isPosDataEqual, "TEST COMPONENTS: positions data of entts isn't equal to the expected values");
	ASSERT_TRUE(isDirDataEqual, "TEST COMPONENTS: directions data of entts isn't equal to the expected values");
	ASSERT_TRUE(isScaleDataEqual, "TEST COMPONENTS: scales data of entts isn't equal to the expected values");
}

