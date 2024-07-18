// *********************************************************************************
// Filename:       TestSystems.cpp
// Description:    implementation of tests for each system of the ECS;
// 
// Created:        13.06.24
// *********************************************************************************
#include "TestSystems.h"
#include "Utils.h"
#include "../Common/MathHelper.h"


void TestSystems::Run()
{
	Log::Print("-------------  TESTS: ECS SYSTEMS  ---------------");
	Log::Print("");

	try
	{
		TestSerialDeserial();
		TestMoveSysUpdating();
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "TEST SYSTEMS: some test doesn't pass");
		exit(-1);
	}
}

///////////////////////////////////////////////////////////

void TestSystems::TestSerialDeserial()
{
	// here we test ECS systems for correct SEPARATE
	// serialization and deserialization of data from the ECS components

	try
	{
		TestTransformSysSerialDeserial();
		TestNameSysSerialDeserial();
		TestMoveSysSerialDeserial();
		TestMeshSysSerialDeserial();
		TestRenderedSysSerialDeserial();
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
		THROW_ERROR("TEST SYSTEMS: serialization/deserialization of some system works incorectly");
	}
}


// *********************************************************************************
//                            UPDATING TEST HELPERS
// *********************************************************************************

void TestSystems::TestMoveSysUpdating()
{
	// test updating functional of the ECS MoveSystem

	const u32 enttsCount = 1;
	EntityManager mgr;
	TransformData transform;
	MoveData move;
	std::vector<EntityID> ids;

	ids = mgr.CreateEntities(enttsCount);

	transform.positions.emplace_back(0.0f, 0.0f, 0.0f);
	transform.dirQuats.push_back({ 0.0f, 0.0f, 0.0f, 0.0f });
	transform.uniformScales.emplace_back(1.0f);

	move.translations.emplace_back(10.0f, 0.0f, 0.0f);
	move.rotQuats.push_back({ 0.0f, 0.0f, 0.0f, 0.0f });
	move.uniformScales.emplace_back(1.0f);
	//Utils::GetRandTransformData(enttsCount, transform);
	//Utils::GetRandMoveData(enttsCount, move);

	mgr.AddTransformComponent(ids, transform.positions, transform.dirQuats, transform.uniformScales);
	mgr.AddMoveComponent(ids, move.translations, move.rotQuats, move.uniformScales);

	mgr.Update(100.0f, 1.0f);

	//Transform& transComp = mgr.transform_;
	//const XMFLOAT4 expectedTransAndUniScale = { 10.0f, 0.0f, 0.0f, 1.0f };
	//bool isTranslatedProperly = (transComp.posAndUniformScale_ == expectedTransAndUniScale);
	//ASSERT_TRUE(isTranslatedProperly, "wrong translation");

	Log::Print(LOG_MACRO, "\tPASSED");

}

// *********************************************************************************
//                    SERIALIZATION / DESERIALIZATION HELPERS
// *********************************************************************************

void TestSystems::TestTransformSysSerialDeserial()
{
	// test the serialization and deserialization of data 
	// from the Transform component

	const std::string filepath = "test_serialization.bin";
	const u32 enttsCount = 10;
	std::vector<EntityID> ids;
	EntityManager origMgr;
	EntityManager deserMgr;
	TransformData data;

	// create entities and add the Trasnform component to them
	ids = origMgr.CreateEntities(enttsCount);

	Utils::GetRandTransformData(enttsCount, data);
	origMgr.AddTransformComponent(ids, data.positions, data.dirQuats, data.uniformScales);

	// serialize and deserialize transform data
	// and then check if deserialized data is correct
	Utils::SysSerialDeserialHelper(filepath, TransformComponent, origMgr, deserMgr);
	Utils::CompareTransformData(deserMgr.transform_, ids, data);

	Log::Print(LOG_MACRO, "\tPASSED");
}

///////////////////////////////////////////////////////////

void TestSystems::TestNameSysSerialDeserial()
{
	// test the serialization and deserialization of data
	// from the Name component using the NameSystem

	const std::string filepath = "test_serialization.bin";
	const u32 enttsCount = 50;
	const u32 nameLength = 10;
	EntityManager origMgr;
	EntityManager deserMgr;
	std::vector<EntityID> ids;
	std::vector<EntityName> names;
	
	Utils::GetRandEnttsNames(enttsCount, nameLength, names);

	ids = origMgr.CreateEntities(enttsCount);
	origMgr.AddNameComponent(ids, names);

	// serialize and deserialize names data
	// and then check if deserialized data is correct
	Utils::SysSerialDeserialHelper(filepath, NameComponent, origMgr, deserMgr);
	Utils::CompareNameData(deserMgr.names_, ids, names);

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void TestSystems::TestMoveSysSerialDeserial()
{
	// test the serialization and deserialization of data 
	// from the Move component using the MoveSystem

	const std::string filepath = "test_serialization.bin";
	const u32 enttsCount = 10;
	EntityManager origMgr;
	EntityManager deserMgr;
	std::vector<EntityID> ids;
	MoveData data;

	// create entities and add the Movement component to them
	ids = origMgr.CreateEntities(enttsCount);
	Utils::GetRandMoveData(enttsCount, data);
	origMgr.AddMoveComponent(ids, data.translations, data.rotQuats, data.uniformScales);

	// serialize and deserialize movement data
	// and then check if deserialized data is correct
	Utils::SysSerialDeserialHelper(filepath, MoveComponent, origMgr, deserMgr);
	Utils::CompareMoveData(deserMgr.movement_, ids, data);

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void TestSystems::TestMeshSysSerialDeserial()
{
	// test the serialization and deserialization of data 
	// from the Mesh component

	const std::string filepath = "test_serialization.bin";
	const std::vector<MeshID> meshesIDs{ 1,2,3,4,5,6,7,8,9,10 };
	const u32 enttsCount = 10;
	std::vector<EntityID> enttsIDs;
	EntityManager origMgr;
	EntityManager deserMgr;

	enttsIDs = origMgr.CreateEntities(enttsCount);
	origMgr.AddMeshComponent(enttsIDs, meshesIDs);

	// store current data of the Mesh component before serialization 
	// so later we will use it for comparison with deserialized data
	MeshComponent& component = origMgr.meshComponent_;
	auto origEntityToMeshes = component.entityToMeshes_;
	auto origMeshToEntts = component.meshToEntities_;

	// serialize and deserialize Mesh component data
	// and then check if deserialized data is correct
	Utils::SysSerialDeserialHelper(filepath, MeshComp, origMgr, deserMgr);
	Utils::CompareMeshData(deserMgr.meshSystem_, enttsIDs, meshesIDs);

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void TestSystems::TestRenderedSysSerialDeserial()
{
	// test the serialization and deserialization of data 
	// from the Rendered component

	const std::string filepath = "test_serialization.bin";
	const u32 enttsCount = 10;
	RenderedData data;
	EntityManager origMgr;
	EntityManager deserMgr;
	std::vector<EntityID> ids;

	Utils::GetRandRenderedData(enttsCount, data);
	ids = origMgr.CreateEntities(enttsCount);
	origMgr.AddRenderingComponent(ids, data.shaderTypes, data.primTopologyTypes);

	// serialize and deserialize Rendered component data
	// and then check if deserialized data is correct
	Utils::SysSerialDeserialHelper(filepath, RenderedComponent, origMgr, deserMgr);
	Utils::CompareRenderedData(deserMgr.renderComponent_, ids, data);

	
	Log::Print(LOG_MACRO, "\tPASSED");
}