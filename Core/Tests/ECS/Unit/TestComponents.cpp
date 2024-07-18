#include "TestComponents.h"
#include "Utils.h"
#include "HelperTypes.h"
#include "Common/Types.h"   // ECS typedefs

#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
using namespace Utils;


// ********************************************************************************
//                          PUBLIC TESTS FUNCTIONS
// ********************************************************************************

void TestComponents::Run()
{
	Log::Print("");
	Log::Print("----------------  TESTS: ECS Components -----------------");
	Log::Print("");

	try
	{
		// test each ECS component
		TestTransformComponent();
		TestNameComponent();
		TestMoveComponent();
		TestMeshComponent();

		TestRenderComponent();
		TestTexTransformComponent();
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		THROW_ERROR("can't pass the test for some ECS component");
	}
}

///////////////////////////////////////////////////////////

void TestComponents::TestTransformComponent()
{
	// UNIT TEST: check behaviour of the ECS when we use the Transform component;

	try
	{
		// here we check if everything works correct about adding 
		// the Transform and WorldMatrix components to the created entities

		EntityManager mgr;
		TransformData data;

		const u32 enttsCount = 50; 
		const Transform& transformComp = mgr.transform_;
		const WorldMatrix& worldMatComp = mgr.world_;


		const std::vector<EntityID> ids = mgr.CreateEntities(enttsCount);
		GetRandTransformData(enttsCount, data);
		mgr.AddTransformComponent(ids, data.positions, data.dirQuats, data.uniformScales);

	
		// check data from the EntityManager
		const bool enttsHaveTransform = CheckEnttsHaveComponent(mgr, ids, transformComp.type_);
		const bool enttsHaveWorldMatrix = CheckEnttsHaveComponent(mgr, ids, worldMatComp.type_);

		ASSERT_TRUE(enttsHaveTransform, "some entity doesn't have the Transform component");
		ASSERT_TRUE(enttsHaveWorldMatrix, "some entity doesn't have the WorldMatrix component");

		// check if stored into the Transform component data is correct
		CompareTransformData(transformComp, ids, data);

		// check if stored into the WorldMatrix component data is correct
		const bool areIDsValid = Utils::ContainerCompare(worldMatComp.ids_, ids);
		ASSERT_TRUE(areIDsValid, "TEST: IDs data from the WorldMatrix component isn't valid");

	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test with the Transform component");
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void TestComponents::TestNameComponent()
{
	// here we test functional of the EntityManager about the Name component;

	try
	{
		EntityManager mgr;
		std::vector<EntityName> names;
		std::vector<EntityID> ids;

		const u32 enttsCount = 50;
		const u32 nameLength = 10;
		const Name& component = mgr.names_;

		GetRandEnttsNames(enttsCount, nameLength, names);
		ids = mgr.CreateEntities(enttsCount);
		mgr.AddNameComponent(ids, names);


		// test if data is stored correctly in the EntityManager
		const bool enttsHaveNames = CheckEnttsHaveComponent(mgr, ids, component.type_);
		ASSERT_TRUE(enttsHaveNames, "some entity doesn't have the Name component");
		
		// test if data is stored correctly in the Name component
		CompareNameData(component, ids, names);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test for the Name component");
	}

	Log::Print(LOG_MACRO, "\t\t\tPASSED");
}

///////////////////////////////////////////////////////////

void TestComponents::TestMoveComponent()
{
	// UNIT TEST: check behaviour of the ECS when we use the Move component;

	try
	{
		std::vector<EntityID> ids;
		EntityManager mgr;
		MoveData move;
		const u32 enttsCount = 20;   // create this number of new empty entts
		const Movement& component = mgr.movement_;

		ids = mgr.CreateEntities(enttsCount);
		GetRandMoveData(enttsCount, move);
		mgr.AddMoveComponent(ids, move.translations, move.rotQuats, move.uniformScales);


		// test if data is stored correctly in the EntityManager
		const bool enttsHaveComponent = CheckEnttsHaveComponent(mgr, ids, component.type_);
		ASSERT_TRUE(enttsHaveComponent, "some entity doesn't have the Move component");
		
		// check if stored into the Movement component data is correct
		CompareMoveData(component, ids, move);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test for the Move component");
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void TestComponents::TestMeshComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the mesh component to entities

	try
	{
		std::vector<EntityID> enttsIDs;
		EntityManager mgr;

		const u32 enttsCount = 50;   // create this number of new empty entts
		const std::vector<MeshID> meshesIDs{ 1,2,3,4 };
		const MeshComponent& component = mgr.meshComponent_;

		enttsIDs = mgr.CreateEntities(enttsCount);
		mgr.AddMeshComponent(enttsIDs, meshesIDs);


		// test if data is stored correctly in the EntityManager
		const bool enttsHaveComponent  = CheckEnttsHaveComponent(mgr, enttsIDs, component.type_);
		ASSERT_TRUE(enttsHaveComponent, "some entity doesn't have the Mesh component");

		// test if data is stored correctly in the Name component
		CompareMeshData(mgr.meshSystem_, enttsIDs, meshesIDs);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test with the Mesh component");
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void TestComponents::TestRenderComponent()
{
	// UNIT TEST: check behaviour of the ECS when we add 
	//            the render component to entities

	try
	{
		EntityManager mgr;
		RenderedData data;
		std::vector<EntityID> ids;

		const u32 enttsCount = 50;
		const Rendered& component = mgr.renderComponent_;

		Utils::GetRandRenderedData(enttsCount, data);
		ids = mgr.CreateEntities(enttsCount);
		mgr.AddRenderingComponent(ids, data.shaderTypes, data.primTopologyTypes);


		// test if data is stored correctly in the EntityManager
		const bool enttsHaveComponent  = CheckEnttsHaveComponent(mgr, ids, component.type_);
		ASSERT_TRUE(enttsHaveComponent, "some entity doesn't have the Rendered component");
		
		// test if data is stored correctly in the Name component
		CompareRenderedData(component, ids, data);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test for the Rendered component");
	}

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void TestComponents::TestTexTransformComponent()
{
	// UNIT TEST: check behaviour of the ECS when we use
	//            the TextureTransform component (texture animation) to entities

	try
	{
		EntityManager mgr;
		std::vector<EntityID> ids;
		const u32 enttsCount = 10;

		// prepare a matrix for texture transformation 
		const XMMATRIX translation = XMMatrixTranslation(10, 10, 10);
		const XMMATRIX rotation = XMMatrixRotationZ(0.01f);
		const XMMATRIX scale = XMMatrixScaling(5, 5, 5);
		const XMMATRIX texTransform = scale * rotation * translation;
		const std::vector<XMMATRIX> texTransformArr(enttsCount, texTransform);
		const TextureTransform& component = mgr.texTransform_;
		

		ids = mgr.CreateEntities(enttsCount);
		mgr.AddTextureTransformComponent(ids, texTransformArr);


		// test if data is stored correctly in the EntityManager
		const bool enttsHaveComponent = CheckEnttsHaveComponent(mgr, ids, component.type_);
		ASSERT_TRUE(enttsHaveComponent, "some entity doesn't have the TextureTransform component");

		// test if data is stored correctly in the Name component
		CompareTexTransformations(component, ids, texTransformArr);
	}
	catch (EngineException& e)
	{
		Log::Error(e);
		THROW_ERROR("can't pass the test for the TextureTransform component");
	}

	Log::Print(LOG_MACRO, "\tPASSED");
}


