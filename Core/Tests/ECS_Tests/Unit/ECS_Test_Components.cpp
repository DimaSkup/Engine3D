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

void ECS_Test_Components::TestMoveComponent()
{
	// UNIT TEST: check behaviour of the ECS when we use the Move component;

	try
	{
		const size_t newEnttsCount = 50;   // create this number of new empty entts
		EntityManager entityMgr;
		TransformData transform;
		MoveData move;

		const std::vector<EntityID> enttsIDs = entityMgr.CreateEntities(newEnttsCount);

		Utils::PrepareRandomMovementData(newEnttsCount, move);
		entityMgr.AddMoveComponent(enttsIDs, move.translations, move.rotQuats, move.scaleChanges);

		// TEST EVERYTHING IS OK
		const bool enttsHaveMove   = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::MoveComponent);
		const bool movementKnowsEntts = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::MoveComponent);

		ASSERT_TRUE(enttsHaveMove, "some entity doesn't have the Move component");
		ASSERT_TRUE(movementKnowsEntts, "the Move component doen't have a record about some entity");
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test with the Move component");
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

		// TEST EVERYTHING IS OK
		const bool enttsHaveMeshComponent  = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::MeshComp);
		const bool meshComponentKnowsEntts = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::MeshComp);

		ASSERT_TRUE(enttsHaveMeshComponent, "some entity doesn't have the Mesh component");
		ASSERT_TRUE(meshComponentKnowsEntts, "the Mesh component doesn't have a record about some entity");
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
		std::vector<RENDERING_SHADERS> shaderTypes;
		std::vector<D3D11_PRIMITIVE_TOPOLOGY> primTopologyTypes;

		const std::vector<EntityID> enttsIDs = entityMgr.CreateEntities(enttsCount);

		Utils::PrepareRandomRenderedData(enttsCount, shaderTypes, primTopologyTypes);
		entityMgr.AddRenderingComponent(enttsIDs, shaderTypes, primTopologyTypes);

		// TEST EVERYTHING IS OK
		const bool enttsHaveRendered  = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::RenderedComponent);
		const bool renderedKnowsEntts = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::RenderedComponent);

		ASSERT_TRUE(enttsHaveRendered, "some entity doesn't have the Rendered component");
		ASSERT_TRUE(renderedKnowsEntts, "the Rendered component doesn't have a record about some entity");
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
	// the Transform component to the created entities

	const size_t newEnttsCount = 50;   // create this number of new empty entts
	EntityManager entityMgr;
	TransformData transform;

	entityMgr.CreateEntities(newEnttsCount);
	const std::vector<EntityID>& enttsIDs = entityMgr.GetAllEnttsIDs();

	Utils::PrepareRandomTransformData(newEnttsCount, transform);
	entityMgr.AddTransformComponent(enttsIDs, transform.positions, transform.directions, transform.scales);

	// TEST EVERYTHING IS OK
	const bool enttsHaveTransform = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::TransformComponent);
	const bool enttsHaveWorldMatrix = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::WorldMatrixComponent);
	const bool transformKnowsEntts = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::TransformComponent);
	const bool worldMatrixComponentKnowsEntts = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::WorldMatrixComponent);

	ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
	ASSERT_TRUE(enttsHaveWorldMatrix, "some entity doesn't have the WorldMatrix component");
	ASSERT_TRUE(transformKnowsEntts, "the Transform component doesn't have a record about some entity");
	ASSERT_TRUE(worldMatrixComponentKnowsEntts, "the WorldMatrix component doesn't have a record about some entity");
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

	Utils::PrepareRandomDataForArray(enttsCount, transformData.positions);
	Utils::PrepareRandomDataForArray(enttsCount, transformData.directions);
	Utils::PrepareRandomDataForArray(enttsCount, transformData.scales);

	entityMgr.AddTransformComponent(
		enttsIDs,
		transformData.positions,
		transformData.directions,
		transformData.scales);

	// get data from the Transform component
	std::vector<XMFLOAT3> posArr;
	std::vector<XMFLOAT3> dirArr;
	std::vector<XMFLOAT3> scalesArr;
	std::vector<ptrdiff_t> dataIdxs;

	entityMgr.transformSystem_.GetTransformDataOfEntts(enttsIDs, dataIdxs, posArr, dirArr, scalesArr);

	//
	// check if transform data from the Transform component is equal to the expected values
	//
	const bool isPosDataEqual = Utils::ContainerCompare(posArr, transformData.positions, Utils::CheckFloat3Equal);
	const bool isDirDataEqual = Utils::ContainerCompare(dirArr, transformData.directions, Utils::CheckFloat3Equal);
	const bool isScaleDataEqual = Utils::ContainerCompare(scalesArr, transformData.scales, Utils::CheckFloat3Equal);

	ASSERT_TRUE(isPosDataEqual, "TEST COMPONENTS: positions data of entts isn't equal to the expected values");
	ASSERT_TRUE(isDirDataEqual, "TEST COMPONENTS: directions data of entts isn't equal to the expected values");
	ASSERT_TRUE(isScaleDataEqual, "TEST COMPONENTS: scales data of entts isn't equal to the expected values");
}

