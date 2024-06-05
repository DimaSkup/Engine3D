#include "ECS_Test_Components.h"

#include "UnitTestUtils.h"

using namespace DirectX;

struct TransformData
{
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> directions;
	std::vector<XMFLOAT3> scales;
};

struct MoveData
{
	std::vector<XMFLOAT3> translations;
	std::vector<XMFLOAT4> rotQuats;
	std::vector<XMFLOAT3> scaleChanges;

};


void ECS_Test_Components::TestTransformComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the transform component to entities

	try
	{
		EntityManager entityMgr;
		UnitTestUtils utils;
		std::vector<EntityID> entitiesIDs;                  
		TransformData transform;

		utils.EntitiesCreationHelper(entityMgr, entitiesIDs);
		PrepareRandomDataForTransformComponent(entitiesIDs.size(), transform.positions, transform.directions, transform.scales);

		entityMgr.AddTransformComponent(entitiesIDs, transform.positions, transform.directions, transform.scales);

		// TEST EVERYTHING IS OK
		const bool enttsHaveTransform = utils.CheckEntitiesHaveComponent(entityMgr, entitiesIDs, ComponentType::TransformComponent);
		const bool transformKnowsEntts = utils.CheckComponentKnowsAboutEntities(entityMgr, entitiesIDs, ComponentType::TransformComponent);

		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
		ASSERT_TRUE(transformKnowsEntts, "the Transform component doesn't have a record about some entity");
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test with the Transform component");
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}



///////////////////////////////////////////////////////////

void ECS_Test_Components::TestMovementComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the movement component to entities

	try
	{
		EntityManager entityMgr;
		UnitTestUtils utils;

		std::vector<EntityID> entitiesIDs;                   // all entities IDs
		TransformData transform;
		MoveData move;

		utils.EntitiesCreationHelper(entityMgr, entitiesIDs);

		PrepareRandomDataForTransformComponent(entitiesIDs.size(), transform.positions, transform.directions, transform.scales);
		PrepareRandomDataForMoveComponent(entitiesIDs.size(), move.translations, move.rotQuats, move.scaleChanges);

		entityMgr.AddTransformComponent(entitiesIDs, transform.positions, transform.directions, transform.scales);
		entityMgr.AddMoveComponent(entitiesIDs, move.translations, move.rotQuats, move.scaleChanges);

		// TEST EVERYTHING IS OK
		const bool enttsHaveTransform  = utils.CheckEntitiesHaveComponent(entityMgr, entitiesIDs, ComponentType::TransformComponent);
		const bool enttsHaveMove       = utils.CheckEntitiesHaveComponent(entityMgr, entitiesIDs, ComponentType::MoveComponent);
		const bool transformKnowsEntts = utils.CheckComponentKnowsAboutEntities(entityMgr, entitiesIDs, ComponentType::TransformComponent);
		const bool moveKnowsEntts      = utils.CheckComponentKnowsAboutEntities(entityMgr, entitiesIDs, ComponentType::MoveComponent);

		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Move component");
		ASSERT_TRUE(transformKnowsEntts, "the Transform component doesn't have a record about some entity");
		ASSERT_TRUE(moveKnowsEntts, "the Move component doen't have a record about some entity");
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
		EntityManager entityMgr;
		UnitTestUtils utils;

		const std::vector<MeshID> meshesIDs{"sphere", "cube", "cylinder", "pyramid"};
		std::vector<EntityID> entitiesIDs;                   // all entities IDs
		TransformData transform;

		utils.EntitiesCreationHelper(entityMgr, entitiesIDs);
		PrepareRandomDataForTransformComponent(entitiesIDs.size(), transform.positions, transform.directions, transform.scales);

		entityMgr.AddTransformComponent(entitiesIDs, transform.positions, transform.directions, transform.scales);
		entityMgr.AddMeshComponents(entitiesIDs, meshesIDs);

		// TEST EVERYTHING IS OK
		const bool enttsHaveTransform      = utils.CheckEntitiesHaveComponent(entityMgr, entitiesIDs, ComponentType::TransformComponent);
		const bool enttsHaveMeshComponent  = utils.CheckEntitiesHaveComponent(entityMgr, entitiesIDs, ComponentType::MeshComp);
		const bool transformKnowsEntts     = utils.CheckComponentKnowsAboutEntities(entityMgr, entitiesIDs, ComponentType::TransformComponent);
		const bool meshComponentKnowsEntts = utils.CheckComponentKnowsAboutEntities(entityMgr, entitiesIDs, ComponentType::MeshComp);

		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
		ASSERT_TRUE(enttsHaveMeshComponent, "some entity doesn't have the Mesh component");
		ASSERT_TRUE(transformKnowsEntts, "the Transform component doesn't have a record about some entity");
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
		EntityManager entityMgr;
		UnitTestUtils utils;

		const std::vector<MeshID> meshesIDs{ "sphere", "cube", "cylinder", "pyramid" };
		std::vector<EntityID> entitiesIDs;
		TransformData transform;

		utils.EntitiesCreationHelper(entityMgr, entitiesIDs);
		PrepareRandomDataForTransformComponent(entitiesIDs.size(), transform.positions, transform.directions, transform.scales);

		entityMgr.AddTransformComponent(entitiesIDs, transform.positions, transform.directions, transform.scales);
		entityMgr.AddMeshComponents(entitiesIDs, meshesIDs);
		entityMgr.AddRenderingComponents(entitiesIDs);

		// TEST EVERYTHING IS OK
		const bool enttsHaveTransform      = utils.CheckEntitiesHaveComponent(entityMgr, entitiesIDs, ComponentType::TransformComponent);
		const bool enttsHaveMeshComp       = utils.CheckEntitiesHaveComponent(entityMgr, entitiesIDs, ComponentType::MeshComp);
		const bool enttsHaveRendered       = utils.CheckEntitiesHaveComponent(entityMgr, entitiesIDs, ComponentType::RenderedComponent);

		const bool transformKnowsEntts     = utils.CheckComponentKnowsAboutEntities(entityMgr, entitiesIDs, ComponentType::TransformComponent);
		const bool meshComponentKnowsEntts = utils.CheckComponentKnowsAboutEntities(entityMgr, entitiesIDs, ComponentType::MeshComp);
		const bool renderedKnowsEntts      = utils.CheckComponentKnowsAboutEntities(entityMgr, entitiesIDs, ComponentType::RenderedComponent);

		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
		ASSERT_TRUE(enttsHaveMeshComp, "some entity doesn't have the Mesh component");
		ASSERT_TRUE(enttsHaveRendered, "some entity doesn't have the Rendered component");

		ASSERT_TRUE(transformKnowsEntts, "the Transform component doesn't have a record about some entity");
		ASSERT_TRUE(meshComponentKnowsEntts, "the Mesh component doesn't have a record about some entity");
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

void ECS_Test_Components::PrepareRandomDataForTransformComponent(
	const size_t elemCount,
	std::vector<DirectX::XMFLOAT3>& outPositions,
	std::vector<DirectX::XMFLOAT3>& outDirections,
	std::vector<DirectX::XMFLOAT3>& outScales)
{
	UnitTestUtils utils;
	utils.PrepareRandomDataForArray(elemCount, outPositions);
	utils.PrepareRandomDataForArray(elemCount, outDirections);
	utils.PrepareRandomDataForArray(elemCount, outScales);
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::PrepareRandomDataForMoveComponent(
	const size_t elemCount,
	std::vector<DirectX::XMFLOAT3>& outTranslations,
	std::vector<DirectX::XMFLOAT4>& outRotationQuats,
	std::vector<DirectX::XMFLOAT3>& outScaleChanges)
{
	UnitTestUtils utils;
	utils.PrepareRandomDataForArray(elemCount, outTranslations);
	utils.PrepareRandomDataForArray(elemCount, outRotationQuats);
	utils.PrepareRandomDataForArray(elemCount, outScaleChanges);
}

