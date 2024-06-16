// *********************************************************************************
// Filename:       ECS_Test_Systems.cpp
// Description:    implementation of tests for each system of the ECS;
// 
// Created:        13.06.24
// *********************************************************************************
#include "ECS_Test_Systems.h"
#include "ECS_Entity/EntityManager.h"
#include "UnitTestUtils.h"
#include "HelperTypes.h"

#include "../Engine/log.h"
#include "../Engine/EngineException.h"



void ECS_Test_Systems::TestSerializationDeserialization()
{
	// here we test ECS systems for correct serialization and
	// deserialization of data from the ECS components

	Log::Print("-------------  TESTS: ECS SYSTEMS  ---------------");
	Log::Print("");

	try
	{
		TestTransformSystemToSerialAndDeserial();
		TestMoveSystemToSerialAndDeserial();
		TestMeshSystemToSerialAndDeserial();
		TestRenderedSystemToSerialAndDeserial();
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "TEST SYSTEMS: serialization/deserialization of some system works incorectly");
		exit(-1);
	}
	
}



// *********************************************************************************
//                            PRIVATE HELPERS
// *********************************************************************************
void ECS_Test_Systems::TestTransformSystemToSerialAndDeserial()
{
	// test the serialization and deserialization of data 
	// from the Transform component

	const size_t enttsCount = 10;
	EntityManager entityMgr;
	std::vector<EntityID> enttsIDs;
	TransformData transform;

	// create entities and add the Trasnform component to them
	enttsIDs = entityMgr.CreateEntities(enttsCount);

	Utils::PrepareRandomDataForArray(enttsCount, transform.positions);
	Utils::PrepareRandomDataForArray(enttsCount, transform.directions);
	Utils::PrepareRandomDataForArray(enttsCount, transform.scales);
	entityMgr.AddTransformComponent(enttsIDs, transform.positions, transform.directions, transform.scales);

	// serialize and deserialize transform data
	const std::string dataFilepath = "test_serialization.bin";
	entityMgr.transformSystem_.Serialize(dataFilepath);
	entityMgr.transformSystem_.Deserialize(dataFilepath);


	//
	// check if deserialized data is correct
	//
	const std::vector<XMFLOAT3>& origPos = transform.positions;
	const std::vector<XMFLOAT3>& origDir = transform.directions;
	const std::vector<XMFLOAT3>& origScales = transform.scales;

	const std::vector<XMFLOAT3>& deserialPos = entityMgr.transform_.positions_;
	const std::vector<XMFLOAT3>& deserialDir = entityMgr.transform_.directions_;
	const std::vector<XMFLOAT3>& deserialScales = entityMgr.transform_.scales_;

	const bool isPosCorrect = Utils::ContainerCompare(deserialPos, origPos, Utils::CheckFloat3Equal);
	const bool isDirCorrect = Utils::ContainerCompare(deserialDir, origDir, Utils::CheckFloat3Equal);
	const bool isScaleCorrect = Utils::ContainerCompare(deserialScales, origScales, Utils::CheckFloat3Equal);

	ASSERT_TRUE(isPosCorrect, "TEST SYSTEMS:deserialized positions data isn't correct");
	ASSERT_TRUE(isDirCorrect, "TEST SYSTEMS:deserialized directions data isn't correct");
	ASSERT_TRUE(isScaleCorrect, "TEST SYSTEMS:deserialized scales data isn't correct");

	Log::Print(LOG_MACRO, "\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Systems::TestMoveSystemToSerialAndDeserial()
{
	// test the serialization and deserialization of data 
	// from the Move component
}

///////////////////////////////////////////////////////////

void ECS_Test_Systems::TestMeshSystemToSerialAndDeserial()
{
	// test the serialization and deserialization of data 
	// from the Mesh component

	const size_t enttsCount = 10;
	EntityManager entityMgr;
	std::vector<MeshID> meshesIDs{ 1,2,3,4,5,6,7,8,9,10 };
	std::vector<EntityID> enttsIDs;

	enttsIDs = entityMgr.CreateEntities(enttsCount);
	entityMgr.AddMeshComponent(enttsIDs, meshesIDs);

	// store current data of the Mesh component so later
	// we will use it for comparison with deserialized data
	std::map<EntityID, std::set<MeshID>> origEntityToMeshes = entityMgr.meshComponent_.entityToMeshes_;
	std::map<MeshID, std::set<EntityID>> origMeshToEntts = entityMgr.meshComponent_.meshToEntities_;

	// serialize and deserialize Mesh component data
	const std::string dataFilepath = "test_serialization.bin";
	entityMgr.meshSystem_.Serialize(dataFilepath);
	entityMgr.meshSystem_.Deserialize(dataFilepath);


	//
	// check if deserialized data is correct
	//
	std::map<EntityID, std::set<MeshID>>& deserialEnttToMeshes = entityMgr.meshComponent_.entityToMeshes_;
	std::map<MeshID, std::set<EntityID>>& deserialMeshToEntts = entityMgr.meshComponent_.meshToEntities_;

	const bool isCompletelyEqual1 = Utils::ContainerCompare(origEntityToMeshes, deserialEnttToMeshes);
	const bool isCompletelyEqual2 = Utils::ContainerCompare(origMeshToEntts, deserialMeshToEntts);

	ASSERT_TRUE(isCompletelyEqual1, "TEST SYSTEMS:counts of the origin records and the deserialized records aren't equal");
	ASSERT_TRUE(isCompletelyEqual2, "TEST SYSTEMS:counts of the origin records and the deserialized records aren't equal");

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Systems::TestRenderedSystemToSerialAndDeserial()
{
	// test the serialization and deserialization of data 
	// from the Rendered component

	const size_t enttsCount = 10;
	const std::vector<RENDERING_SHADERS> shaderTypes(enttsCount, RENDERING_SHADERS::TEXTURE_SHADER);
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY> primTopologyTypes(enttsCount, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	EntityManager entityMgr;
	std::vector<EntityID> enttsIDs;

	enttsIDs = entityMgr.CreateEntities(enttsCount);
	entityMgr.AddRenderingComponent(enttsIDs, shaderTypes, primTopologyTypes);

	// serialize and deserialize Rendered component data
	const std::string dataFilepath = "test_serialization.bin";
	entityMgr.renderSystem_.Serialize(dataFilepath);
	entityMgr.renderSystem_.Deserialize(dataFilepath);

	//
	// check if deserialized data is correct
	//
	const std::vector<EntityID>& deserialIDs = entityMgr.renderComponent_.ids_;
	const std::vector<RENDERING_SHADERS>& deserialShaderTypes = entityMgr.renderComponent_.shaderTypes_;
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& deserialTopologies = entityMgr.renderComponent_.primTopologies_;

	const bool isIdsDataCorrect = Utils::ContainerCompare(deserialIDs, enttsIDs);
	const bool isShaderTypesDataCorrect = Utils::ContainerCompare(deserialShaderTypes, shaderTypes);
	const bool isTopologiesDataCorrect = Utils::ContainerCompare(deserialTopologies, primTopologyTypes);

	ASSERT_TRUE(isIdsDataCorrect, "TEST SYSTEMS: deserialized IDs data isn't correct");
	ASSERT_TRUE(isShaderTypesDataCorrect, "TEST SYSTEMS: deserialized shader types data isn't correct");
	ASSERT_TRUE(isTopologiesDataCorrect, "TEST SYSTEMS: deserialized primitive topologies data isn't correct");

	Log::Print(LOG_MACRO, "\tPASSED");
}