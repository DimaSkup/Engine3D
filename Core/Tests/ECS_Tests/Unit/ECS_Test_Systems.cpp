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
	Log::Print("---------------  TESTS: SYSTEMS  -----------------");
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
		Log::Error(LOG_MACRO, "serialization/deserialization of some system works incorectly");
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
	const std::vector<XMFLOAT3>& tPos = entityMgr.transform_.positions_;
	const std::vector<XMFLOAT3>& tDir = entityMgr.transform_.directions_;
	const std::vector<XMFLOAT3>& tScale = entityMgr.transform_.scales_;

	// check if arrays of origin translation data and arrays of deserialized data are equal
	ASSERT_TRUE(transform.positions.size() == tPos.size(), "count of the origin positions and the deserialized positions aren't equal");
	ASSERT_TRUE(transform.directions.size() == tDir.size(), "count of the origin directions and the deserialized directions aren't equal");
	ASSERT_TRUE(transform.scales.size() == tScale.size(), "count of the origin scales and the deserialized scales aren't equal");

	// check if each value of deserialized data is correct
	for (size_t idx = 0; idx < tPos.size(); ++idx)
	{
		const bool isPosCorrect = Utils::CheckFloat3Equal(tPos[idx], transform.positions[idx]);
		const bool isDirCorrect = Utils::CheckFloat3Equal(tDir[idx], transform.directions[idx]);
		const bool isScaleCorrect = Utils::CheckFloat3Equal(tScale[idx], transform.scales[idx]);

		ASSERT_TRUE(isPosCorrect, "deserialized positions data isn't correct by idx: " + std::to_string(idx));
		ASSERT_TRUE(isDirCorrect, "deserialized directions data isn't correct");
		ASSERT_TRUE(isScaleCorrect, "deserialized scales data isn't correct");
	}

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

	// get the Mesh component current data so later
	// we will use it for comparison with deserialized data
	std::map<EntityID, std::set<MeshID>> origEntityToMeshes = entityMgr.meshComponent_.entityToMeshes_;
	std::map<MeshID, std::set<EntityID>> origMeshToEntts = entityMgr.meshComponent_.meshToEntities_;

	// serialize and deserialize Mesh component data
	const std::string dataFilepath = "test_serialization.bin";
	entityMgr.meshSystem_.Serialize(dataFilepath);
	entityMgr.meshSystem_.Deserialize(dataFilepath);

	// get deserialized data from the MeshComponent
	std::map<EntityID, std::set<MeshID>>& deserialEnttToMeshes = entityMgr.meshComponent_.entityToMeshes_;
	std::map<MeshID, std::set<EntityID>>& deserialMeshToEntts = entityMgr.meshComponent_.meshToEntities_;

	// check if we have the same amount of deserialized data as the origin one
	const bool isCompletelyEqual1 = Utils::MapCompare(origEntityToMeshes, deserialEnttToMeshes);
	const bool isCompletelyEqual2 = Utils::MapCompare(origMeshToEntts, deserialMeshToEntts);

	ASSERT_TRUE(isCompletelyEqual1, "counts of the origin records and the deserialized records aren't equal");
	ASSERT_TRUE(isCompletelyEqual2, "counts of the origin records and the deserialized records aren't equal");

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Systems::TestRenderedSystemToSerialAndDeserial()
{
	// test the serialization and deserialization of data 
	// from the Rendered component
	const size_t enttsCount = 10;
	EntityManager entityMgr;
	std::vector<RENDERING_SHADERS> shaderTypes(enttsCount, RENDERING_SHADERS::TEXTURE_SHADER);
	std::vector<EntityID> enttsIDs;
}