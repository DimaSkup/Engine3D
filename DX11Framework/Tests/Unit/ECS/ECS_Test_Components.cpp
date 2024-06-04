#include "ECS_Test_Components.h"

#include "UnitTestUtils.h"

using namespace DirectX;

void ECS_Test_Components::TestAddTransformComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the transform component to entities

	try
	{
		EntityManager entityMgr;
		UnitTestUtils utils;
		std::vector<EntityID> entityIDs;                   // all entities IDs
		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> directions;
		std::vector<XMFLOAT3> scales;


		utils.EntitiesCreationHelper(entityMgr, entityIDs);

		// prepare random data for component
		utils.PrepareRandomDataForArray(entityIDs.size(), positions);
		utils.PrepareRandomDataForArray(entityIDs.size(), directions);
		utils.PrepareRandomDataForArray(entityIDs.size(), scales);

		// add the component and check if everything is OK
		entityMgr.AddTransformComponents(entityIDs, positions, directions, scales);
		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, ComponentType::TransformComponent);
		

		//entityMgr.GetComponent<Transform>().GetDataOfEntity
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		return;
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestAddMovementComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the movement component to entities

	try
	{
		EntityManager entityMgr;
		UnitTestUtils utils;

		std::vector<EntityID> entityIDs;                   // all entities IDs
		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> directions;
		std::vector<XMFLOAT3> scales;

		utils.EntitiesCreationHelper(entityMgr, entityIDs);

		// prepare random data for Transform component
		utils.PrepareRandomDataForArray(entityIDs.size(), positions);
		utils.PrepareRandomDataForArray(entityIDs.size(), directions);
		utils.PrepareRandomDataForArray(entityIDs.size(), scales);

		utils.AddTransformComponentHelper(entityMgr, entityIDs, positions, directions, scales);
		utils.AddMovementComponentHelper(entityMgr, entityIDs);

		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, ComponentType::TransformComponent);
		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, ComponentType::MovementComponent);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		return;
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestAddMeshComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the mesh component to entities

	try
	{
		EntityManager entityMgr;
		UnitTestUtils utils;

		const std::vector<MeshID> meshesIDs{"meshID"};
		std::vector<EntityID> entityIDs;                   // all entities IDs
		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> directions;
		std::vector<XMFLOAT3> scales;

		utils.EntitiesCreationHelper(entityMgr, entityIDs);

		// prepare random data for Transform component
		utils.PrepareRandomDataForArray(entityIDs.size(), positions);
		utils.PrepareRandomDataForArray(entityIDs.size(), directions);
		utils.PrepareRandomDataForArray(entityIDs.size(), scales);
		utils.AddTransformComponentHelper(entityMgr, entityIDs, positions, directions, scales);
		entityMgr.AddMeshComponents(entityIDs, meshesIDs);

		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, ComponentType::TransformComponent);
		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, ComponentType::MeshComp);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		return;
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Components::TestAddRenderComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the render component to entities

	try
	{
		EntityManager entityMgr;
		UnitTestUtils utils;

		const std::vector<MeshID> meshesIDs{ "meshID" };
		std::vector<EntityID> entityIDs;                   // all entities IDs
		std::vector<XMFLOAT3> positions;
		std::vector<XMFLOAT3> directions;
		std::vector<XMFLOAT3> scales;

		utils.EntitiesCreationHelper(entityMgr, entityIDs);

		// prepare random data for Transform component
		utils.PrepareRandomDataForArray(entityIDs.size(), positions);
		utils.PrepareRandomDataForArray(entityIDs.size(), directions);
		utils.PrepareRandomDataForArray(entityIDs.size(), scales);

		utils.AddTransformComponentHelper(entityMgr, entityIDs, positions, directions, scales);
		entityMgr.AddMeshComponents(entityIDs, meshesIDs);
		entityMgr.AddRenderingComponents(entityIDs);

		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, ComponentType::TransformComponent);
		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, ComponentType::MeshComp);
		utils.CheckEntitiesHaveComponent(entityMgr, entityIDs, ComponentType::RenderedComponent);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		return;
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}