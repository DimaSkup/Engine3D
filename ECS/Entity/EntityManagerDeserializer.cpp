// ********************************************************************************
// Filename:     EntityManagerDeserializer.cpp
// Description:  contains implementation of functional 
//               for the EntityManagerDeserializer
// 
// Created:      26.06.24
// ********************************************************************************
#include "EntityManagerDeserializer.h"

#include "../Common/Utils.h"
#include "../Common/Assert.h"
#include "../Common/Log.h"

#include <fstream>

namespace ECS
{

void EntityManagerDeserializer::Deserialize(
	EntityManager& entityMgr,
	const std::string& dataFilepath)
{
	std::ifstream fin(dataFilepath, std::ios::binary);
	Assert::True(fin.is_open(), "can't open a file for deserialization: " + dataFilepath);

	SerializedDataHeader header;

	ReadDataHeader(fin, header);

	// print out header records ['data_block_marker' => 'data_block_pos']
	for (const SerializedDataHeaderRecord& record : header.records)
	{
		Log::Print(std::to_string(record.dataBlockMarker) + " => " + std::to_string(record.dataBlockPos));
	}


	// deserialize EntityManager data: entities IDs, component flags, etc.
	DeserializeDataOfEnttMgr(fin, entityMgr);

	// deserialize components data
	entityMgr.transformSystem_.Deserialize(fin, header.GetDataBlockPos(TransformComponent));
	entityMgr.nameSystem_.Deserialize(fin, header.GetDataBlockPos(NameComponent));
	entityMgr.meshSystem_.Deserialize(fin, header.GetDataBlockPos(MeshComp));
	entityMgr.moveSystem_.Deserialize(fin, header.GetDataBlockPos(MoveComponent));
	entityMgr.renderSystem_.Deserialize(fin, header.GetDataBlockPos(RenderedComponent));

	fin.close();
}

///////////////////////////////////////////////////////////

void EntityManagerDeserializer::ReadDataHeader(
	std::ifstream& fin,
	SerializedDataHeader& header)
{
	// read in the data header from the data file;
	// so later we'll use this header to navigate through the file to
	// deserialize EntityManager and Components data;

	Utils::FileRead(fin, &(header.recordsCount));

	header.records.resize(header.recordsCount);

	for (SerializedDataHeaderRecord& record : header.records)
	{
		Utils::FileRead(fin, &(record.dataBlockMarker));
		Utils::FileRead(fin, &(record.dataBlockPos));
	}
}

///////////////////////////////////////////////////////////

void EntityManagerDeserializer::DeserializeDataOfEnttMgr(
	std::ifstream& fin,
	EntityManager& entityMgr)
{
	// deserialize data for the entity manager: all the entities IDs 
	// and related components flags (not components data itself or something else)

	u32 dataBlockMarker = 0;
	Utils::FileRead(fin, &dataBlockMarker);

	const bool isProperDataBlock = (dataBlockMarker == EntityManager::ENTT_MGR_SERIALIZE_DATA_BLOCK_MARKER);
	Assert::True(isProperDataBlock, "ECS deserialization: read wrong block of data (there must be data for the EntityManager)");

	// --------------------------------

	// read in how much data will we have for the entity manager
	u32 dataCount = 0;
	Utils::FileRead(fin, &dataCount);

	// prepare enough amount of memory for data
	entityMgr.ids_.resize(dataCount);
	entityMgr.componentHashes_.resize(dataCount);

	Utils::FileRead(fin, entityMgr.ids_);
	Utils::FileRead(fin, entityMgr.componentHashes_);
}

///////////////////////////////////////////////////////////

}