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
	// UNIT TEST: check behaviour of the ECS when we use the Transform component;

	try
	{
		const size_t newEnttsCount = 50;   // create this number of new empty entts
		EntityManager entityMgr;
		TransformData transform;

		entityMgr.CreateEntities(newEnttsCount);
		const std::vector<EntityID>& enttsIDs = entityMgr.GetAllEnttsIDs();

		PrepareRandomDataForTransformComponent(newEnttsCount, transform.positions, transform.directions, transform.scales);
		entityMgr.AddTransformComponent(enttsIDs, transform.positions, transform.directions, transform.scales);

		// TEST EVERYTHING IS OK
		const bool enttsHaveTransform = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::TransformComponent);
		const bool transformKnowsEntts = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::TransformComponent);

		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
		ASSERT_TRUE(transformKnowsEntts, "the Transform component doesn't have a record about some entity");
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e);
		THROW_ERROR("can't pass the test with the Transform component");
	}

	ECS::Log::Print(LOG_MACRO, "\t\tPASSED");
}



///////////////////////////////////////////////////////////

void ECS_Test_Components::TestMovementComponent()
{
	// UNIT TEST: check behaviour of the ECS when we use the Move component;

	try
	{
		const size_t newEnttsCount = 50;   // create this number of new empty entts
		EntityManager entityMgr;
		TransformData transform;
		MoveData move;

		entityMgr.CreateEntities(newEnttsCount);
		const std::vector<EntityID>& enttsIDs = entityMgr.GetAllEnttsIDs();

		PrepareRandomDataForTransformComponent(newEnttsCount, transform.positions, transform.directions, transform.scales);
		PrepareRandomDataForMoveComponent(newEnttsCount, move.translations, move.rotQuats, move.scaleChanges);

		entityMgr.AddTransformComponent(enttsIDs, transform.positions, transform.directions, transform.scales);
		entityMgr.AddMoveComponent(enttsIDs, move.translations, move.rotQuats, move.scaleChanges);

		// TEST EVERYTHING IS OK
		const bool enttsHaveTransform  = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::TransformComponent);
		const bool enttsHaveMove       = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::MoveComponent);
		const bool transformKnowsEntts = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::TransformComponent);
		const bool moveKnowsEntts      = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::MoveComponent);

		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Move component");
		ASSERT_TRUE(transformKnowsEntts, "the Transform component doesn't have a record about some entity");
		ASSERT_TRUE(moveKnowsEntts, "the Move component doen't have a record about some entity");
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e);
		THROW_ERROR("can't pass the test with the Move component");
	}

	ECS::Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestMeshComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the mesh component to entities

	try
	{
		const size_t newEnttsCount = 50;   // create this number of new empty entts
		const std::vector<MeshID> meshesIDs{ "sphere", "cube", "cylinder", "pyramid" };
		EntityManager entityMgr;
		TransformData transform;

		entityMgr.CreateEntities(newEnttsCount);
		const std::vector<EntityID>& enttsIDs = entityMgr.GetAllEnttsIDs();

		PrepareRandomDataForTransformComponent(newEnttsCount, transform.positions, transform.directions, transform.scales);

		entityMgr.AddTransformComponent(enttsIDs, transform.positions, transform.directions, transform.scales);
		entityMgr.AddMeshComponents(enttsIDs, meshesIDs);

		// TEST EVERYTHING IS OK
		const bool enttsHaveTransform      = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::TransformComponent);
		const bool enttsHaveMeshComponent  = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::MeshComp);
		const bool transformKnowsEntts     = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::TransformComponent);
		const bool meshComponentKnowsEntts = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::MeshComp);

		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
		ASSERT_TRUE(enttsHaveMeshComponent, "some entity doesn't have the Mesh component");
		ASSERT_TRUE(transformKnowsEntts, "the Transform component doesn't have a record about some entity");
		ASSERT_TRUE(meshComponentKnowsEntts, "the Mesh component doesn't have a record about some entity");
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e);
		THROW_ERROR("can't pass the test with the Mesh component");
	}

	ECS::Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestRenderComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the render component to entities

	try
	{
		const size_t newEnttsCount = 50;   // create this number of new empty entts
		const std::vector<MeshID> meshesIDs{ "sphere", "cube", "cylinder", "pyramid" };  // prepare data for the Mesh component
		EntityManager entityMgr;
		TransformData transform;

		entityMgr.CreateEntities(newEnttsCount);
		const std::vector<EntityID>& enttsIDs = entityMgr.GetAllEnttsIDs();

		// prepare data for components
		PrepareRandomDataForTransformComponent(enttsIDs.size(), transform.positions, transform.directions, transform.scales);
		const std::vector<RENDERING_SHADERS> shaderTypes(enttsIDs.size(), RENDERING_SHADERS::COLOR_SHADER);
		const std::vector<D3D11_PRIMITIVE_TOPOLOGY> primTopologyTypes(enttsIDs.size(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// add components
		entityMgr.AddTransformComponent(enttsIDs, transform.positions, transform.directions, transform.scales);
		entityMgr.AddMeshComponents(enttsIDs, meshesIDs);
		entityMgr.AddRenderingComponents(enttsIDs, shaderTypes, primTopologyTypes);

		// TEST EVERYTHING IS OK
		const bool enttsHaveTransform      = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::TransformComponent);
		const bool enttsHaveMeshComp       = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::MeshComp);
		const bool enttsHaveRendered       = entityMgr.CheckEnttsByIDsHaveComponent(enttsIDs, ComponentType::RenderedComponent);

		const bool transformKnowsEntts     = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::TransformComponent);
		const bool meshComponentKnowsEntts = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::MeshComp);
		const bool renderedKnowsEntts      = Utils::CheckComponentKnowsAboutEntities(entityMgr, enttsIDs, ComponentType::RenderedComponent);

		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
		ASSERT_TRUE(enttsHaveMeshComp, "some entity doesn't have the Mesh component");
		ASSERT_TRUE(enttsHaveRendered, "some entity doesn't have the Rendered component");

		ASSERT_TRUE(transformKnowsEntts, "the Transform component doesn't have a record about some entity");
		ASSERT_TRUE(meshComponentKnowsEntts, "the Mesh component doesn't have a record about some entity");
		ASSERT_TRUE(renderedKnowsEntts, "the Rendered component doesn't have a record about some entity");
	}
	catch (LIB_Exception& e)
	{
		ECS::Log::Error(e);
		THROW_ERROR("can't pass the test with the Rendered component");
	}

	ECS::Log::Print(LOG_MACRO, "\t\tPASSED");
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
	
	Utils::PrepareRandomDataForArray(elemCount, outPositions);
	Utils::PrepareRandomDataForArray(elemCount, outDirections);
	Utils::PrepareRandomDataForArray(elemCount, outScales);
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::PrepareRandomDataForMoveComponent(
	const size_t elemCount,
	std::vector<DirectX::XMFLOAT3>& outTranslations,
	std::vector<DirectX::XMFLOAT4>& outRotationQuats,
	std::vector<DirectX::XMFLOAT3>& outScaleChanges)
{
	Utils::PrepareRandomDataForArray(elemCount, outTranslations);
	Utils::PrepareRandomDataForArray(elemCount, outRotationQuats);
	Utils::PrepareRandomDataForArray(elemCount, outScaleChanges);
}

