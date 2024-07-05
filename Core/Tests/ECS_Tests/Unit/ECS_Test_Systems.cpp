// *********************************************************************************
// Filename:       ECS_Test_Systems.cpp
// Description:    implementation of tests for each system of the ECS;
// 
// Created:        13.06.24
// *********************************************************************************
#include "ECS_Test_Systems.h"
#include "UnitTestUtils.h"

#include <fstream>

ECS_Test_Systems::~ECS_Test_Systems()
{
	Utils::RemoveFile(dataFilepathToSerializedData_);
}

///////////////////////////////////////////////////////////

void ECS_Test_Systems::TestSerializationDeserialization()
{
	// here we test ECS systems for correct serialization and
	// deserialization of data from the ECS components

	Log::Print("-------------  TESTS: ECS SYSTEMS  ---------------");
	Log::Print("");

	try
	{
		TestTransformSystemToSerialAndDeserial();
		TestNameSystemToSerialAndDeserial();
		TestMoveSystemToSerialAndDeserial();
		TestMeshSystemToSerialAndDeserial();
		TestRenderedSystemToSerialAndDeserial();
	}
	catch (EngineException& e)
	{
		Log::Error(e, false);
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
	std::vector<EntityID> enttsIDs;
	EntityManager entityMgr;
	TransformData transform;

	// create entities and add the Trasnform component to them
	enttsIDs = entityMgr.CreateEntities(enttsCount);

	Utils::PrepareRandomTransformData(enttsCount, transform);
	entityMgr.AddTransformComponent(enttsIDs, transform.positions, transform.directions, transform.scales);

	// serialize and deserialize transform data
	size_t offset = 0;
	std::ofstream fout(dataFilepathToSerializedData_, std::ios::binary);
	entityMgr.transformSystem_.Serialize(fout, offset);
	fout.close();

	std::ifstream fin(dataFilepathToSerializedData_, std::ios::binary);
	entityMgr.transformSystem_.Deserialize(fin, offset);
	fin.close();
	Utils::RemoveFile(dataFilepathToSerializedData_);

	//
	// check if deserialized data is correct
	//

	// NOTE: because the Transform component stores normalized direction vectors
	//       we have to normalize the origin direction vectors
	for (XMFLOAT3& dir : transform.directions)
	{
		XMStoreFloat3(&dir, DirectX::XMVector3Normalize(XMLoadFloat3(&dir)));
	}

	const std::vector<XMFLOAT3>& origPos = transform.positions;
	const std::vector<XMFLOAT3>& origDir = transform.directions;
	const std::vector<XMFLOAT3>& origScales = transform.scales;

	const std::vector<EntityID>& deserialIDs = entityMgr.transform_.ids_;
	const std::vector<XMFLOAT3>& deserialPos = entityMgr.transform_.positions_;
	const std::vector<XMFLOAT3>& deserialDir = entityMgr.transform_.dirQuats_;
	const std::vector<XMFLOAT3>& deserialScales = entityMgr.transform_.scales_;

	// compare deserialized data to the origin data
	const bool isIDsDataCorrect = Utils::ContainerCompare(deserialIDs, enttsIDs);
	const bool isPosCorrect = Utils::ContainerCompare(deserialPos, origPos);
	const bool isDirCorrect = Utils::ContainerCompare(deserialDir, origDir);
	const bool isScaleCorrect = Utils::ContainerCompare(deserialScales, origScales);

	ASSERT_TRUE(isIDsDataCorrect, "TEST SYSTEMS: deserialized IDs data isn't correct");
	ASSERT_TRUE(isPosCorrect, "TEST SYSTEMS:deserialized positions data isn't correct");
	ASSERT_TRUE(isDirCorrect, "TEST SYSTEMS:deserialized directions data isn't correct");
	ASSERT_TRUE(isScaleCorrect, "TEST SYSTEMS:deserialized scales data isn't correct");

	Log::Print(LOG_MACRO, "\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Systems::TestNameSystemToSerialAndDeserial()
{
	// test the serialization and deserialization of data
	// from the Name component using the NameSystem

	const UINT enttsCount = 50;
	const UINT nameLength = 10;
	EntityManager entityMgr;
	std::vector<EntityName> enttsNames;
	std::vector<EntityID> enttsIDs;
	
	// generate random names for entities
	Utils::GenerateEnttsNames(enttsCount, nameLength, enttsNames);

	enttsIDs = entityMgr.CreateEntities(enttsCount);
	entityMgr.AddNameComponent(enttsIDs, enttsNames);

	// serialize and deserialize names data
	size_t offset = 0;
	std::ofstream fout(dataFilepathToSerializedData_, std::ios::binary);
	entityMgr.nameSystem_.Serialize(fout, offset);
	fout.close();

	std::ifstream fin(dataFilepathToSerializedData_, std::ios::binary);
	entityMgr.nameSystem_.Deserialize(fin, offset);
	fin.close();
	Utils::RemoveFile(dataFilepathToSerializedData_);


	//
	// check if deserialized data is correct
	//
	const Name& nameComponent = entityMgr.names_;
	const std::vector<EntityID>& deserialIDs = nameComponent.ids_;
	const std::vector<EntityName>& deserialNames = nameComponent.names_;

	const bool isIDsDataCorrect = Utils::ContainerCompare(deserialIDs, enttsIDs);
	const bool isNamesDataCorrect = Utils::ContainerCompare(deserialNames, enttsNames);

	ASSERT_TRUE(isIDsDataCorrect, "deserialized IDs data isn't correct");
	ASSERT_TRUE(isNamesDataCorrect, "deserialized names data isn't correct");

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Systems::TestMoveSystemToSerialAndDeserial()
{
	// test the serialization and deserialization of data 
	// from the Move component using the MoveSystem

	const size_t enttsCount = 10;
	EntityManager entityMgr;
	std::vector<EntityID> enttsIDs;
	MoveData move;

	// create entities and add the Movement component to them
	enttsIDs = entityMgr.CreateEntities(enttsCount);
	Utils::PrepareRandomMovementData(enttsCount, move);
	entityMgr.AddMoveComponent(enttsIDs, move.translations, move.rotQuats, move.scaleChanges);

	// serialize and deserialize movement data
	entityMgr.moveSystem_.Serialize(dataFilepathToSerializedData_);
	entityMgr.moveSystem_.Deserialize(dataFilepathToSerializedData_);
	Utils::RemoveFile(dataFilepathToSerializedData_);

	//
	// check if deserialized data is correct
	//
	const std::vector<EntityID>& deserialIDs = entityMgr.movement_.ids_;
	const std::vector<XMFLOAT3>& deserialTranslations = entityMgr.movement_.translations_;
	const std::vector<XMFLOAT4>& deserialRotQuats = entityMgr.movement_.rotationQuats_;
	const std::vector<XMFLOAT3>& deserialScaleChanges = entityMgr.movement_.scaleChanges_;

	// compare deserialized data to the origin data
	const bool isIDsDataCorrect          = Utils::ContainerCompare(deserialIDs, enttsIDs);
	const bool isTranslationsDataCorrect = Utils::ContainerCompare(deserialTranslations, move.translations);
	const bool isRotQuatsDataCorrect     = Utils::ContainerCompare(deserialRotQuats, move.rotQuats);
	const bool isScaleChangesDataCorrect = Utils::ContainerCompare(deserialScaleChanges, move.scaleChanges);

	ASSERT_TRUE(isIDsDataCorrect, "TEST SYSTEMS: deserialized IDs data isn't correct");
	ASSERT_TRUE(isTranslationsDataCorrect, "TEST SYSTEMS: deserialized translations data isn't correct");
	ASSERT_TRUE(isRotQuatsDataCorrect, "TEST SYSTEMS: deserialized rotation quaternions data isn't correct");
	ASSERT_TRUE(isScaleChangesDataCorrect, "TEST SYSTEMS: deserialized scale changes data isn't correct");

	Log::Print(LOG_MACRO, "\t\tPASSED");
}

///////////////////////////////////////////////////////////

void ECS_Test_Systems::TestMeshSystemToSerialAndDeserial()
{
	// test the serialization and deserialization of data 
	// from the Mesh component

	const size_t enttsCount = 10;
	const std::vector<MeshID> meshesIDs{ 1,2,3,4,5,6,7,8,9,10 };
	std::vector<EntityID> enttsIDs;
	EntityManager entityMgr;

	enttsIDs = entityMgr.CreateEntities(enttsCount);
	entityMgr.AddMeshComponent(enttsIDs, meshesIDs);

	// store current data of the Mesh component so later
	// we will use it for comparison with deserialized data
	std::map<EntityID, std::set<MeshID>> origEntityToMeshes = entityMgr.meshComponent_.entityToMeshes_;
	std::map<MeshID, std::set<EntityID>> origMeshToEntts = entityMgr.meshComponent_.meshToEntities_;

	// serialize and deserialize Mesh component data
	size_t offset = 0;
	std::ofstream fout(dataFilepathToSerializedData_, std::ios::binary);
	entityMgr.meshSystem_.Serialize(fout, offset);
	fout.close();

	std::ifstream fin(dataFilepathToSerializedData_, std::ios::binary);
	entityMgr.meshSystem_.Deserialize(fin, offset);
	fin.close();
	Utils::RemoveFile(dataFilepathToSerializedData_);


	//
	// check if deserialized data is correct
	//
	std::map<EntityID, std::set<MeshID>>& deserialEnttToMeshes = entityMgr.meshComponent_.entityToMeshes_;
	std::map<MeshID, std::set<EntityID>>& deserialMeshToEntts = entityMgr.meshComponent_.meshToEntities_;

	// compare deserialized data to the origin data
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
	RenderedData rendered;
	EntityManager entityMgr;
	std::vector<EntityID> enttsIDs;

	enttsIDs = entityMgr.CreateEntities(enttsCount);
	Utils::PrepareRandomRenderedData(enttsCount, rendered);
	entityMgr.AddRenderingComponent(enttsIDs, rendered.shaderTypes, rendered.primTopologyTypes);

	// serialize and deserialize Rendered component data
	size_t offset = 0;
	std::ofstream fout(dataFilepathToSerializedData_, std::ios::binary);
	entityMgr.renderSystem_.Serialize(fout, offset);
	fout.close();

	std::ifstream fin(dataFilepathToSerializedData_, std::ios::binary);
	entityMgr.renderSystem_.Deserialize(fin, offset);
	fin.close();
	Utils::RemoveFile(dataFilepathToSerializedData_);


	//
	// check if deserialized data is correct
	//
	const std::vector<EntityID>& deserialIDs = entityMgr.renderComponent_.ids_;
	const std::vector<ECS::RENDERING_SHADERS>& deserialShaderTypes = entityMgr.renderComponent_.shaderTypes_;
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& deserialTopologies = entityMgr.renderComponent_.primTopologies_;

	// compare deserialized data to the origin data
	const bool isIDsDataCorrect         = Utils::ContainerCompare(deserialIDs, enttsIDs);
	const bool isShaderTypesDataCorrect = Utils::ContainerCompare(deserialShaderTypes, rendered.shaderTypes);
	const bool isTopologiesDataCorrect  = Utils::ContainerCompare(deserialTopologies, rendered.primTopologyTypes);

	ASSERT_TRUE(isIDsDataCorrect, "TEST SYSTEMS: deserialized IDs data isn't correct");
	ASSERT_TRUE(isShaderTypesDataCorrect, "TEST SYSTEMS: deserialized shader types data isn't correct");
	ASSERT_TRUE(isTopologiesDataCorrect, "TEST SYSTEMS: deserialized primitive topologies data isn't correct");

	Log::Print(LOG_MACRO, "\tPASSED");
}