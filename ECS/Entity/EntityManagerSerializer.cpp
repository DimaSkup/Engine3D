// ********************************************************************************
// Filename:     EntityManagerSerializer.cpp
// Description:  contains implementation of functional 
//               for the EntityManagerSerializer
// 
// Created:      26.06.24
// ********************************************************************************
#include "EntityManagerSerializer.h"

#include "../Common/Utils.h"
#include "../Common/LIB_Exception.h"
#include "../Common/Log.h"
#include "../Common/Assert.h"

#include <fstream>

namespace ECS
{

void EntityManagerSerializer::Serialize(
	EntityManager& entityMgr,
	const std::string& dataFilepath)
{
	std::ofstream fout(dataFilepath, std::ios::binary);
	Assert::True(fout.is_open(), "can't open a file for serialization: " + dataFilepath);

	SerializedDataHeader header;

	// write into the file a header and data from the EntityManager (IDs, component flags, etc.)
	WriteDataHeader(fout, header);
	SerializeDataOfEnttMgr(fout, entityMgr);

	// serialize data from the all components
	entityMgr.transformSystem_.Serialize(fout, header.GetDataBlockPos(TransformComponent));
	entityMgr.moveSystem_.Serialize(fout, header.GetDataBlockPos(MoveComponent));
	entityMgr.nameSystem_.Serialize(fout, header.GetDataBlockPos(NameComponent));
	entityMgr.meshSystem_.Serialize(fout, header.GetDataBlockPos(MeshComp));
	entityMgr.renderSystem_.Serialize(fout, header.GetDataBlockPos(RenderedComponent));

	// go at the beginning of the file and update the header
	fout.seekp(0, std::ios_base::beg);
	WriteDataHeader(fout, header);

	fout.close();

	Log::Debug("data from the ECS has been saved successfully into the file: " + dataFilepath);
}

///////////////////////////////////////////////////////////

void EntityManagerSerializer::WriteDataHeader(
	std::ofstream& fout,
	SerializedDataHeader& header)
{
	// write into a file by dataFilepath the data header for all the data
	// related to the Entity-Component-System; so later this header will be used
	// to get a position of the necessary block of data;

	header.recordsCount = 17;     // one block per component
	header.records.resize(header.recordsCount);

	for (u32 idx = 0; idx < header.recordsCount; ++idx)
		header.records[idx].dataBlockMarker = idx;

	// write the header into the file
	Utils::FileWrite(fout, &(header.recordsCount));
	Utils::FileWrite(fout, header.records);
}

///////////////////////////////////////////////////////////

void EntityManagerSerializer::SerializeDataOfEnttMgr(
	std::ofstream& fout,
	EntityManager& entityMgr)
{
	// serialize data of the entity manager: all the entities IDs 
	// and related components flags (not components data itself or something else)

	const u32 dataBlockMarker = EntityManager::ENTT_MGR_SERIALIZE_DATA_BLOCK_MARKER;
	const u32 dataCount = (u32)std::ssize(entityMgr.ids_);

	// write data into the data file
	Utils::FileWrite(fout, &dataBlockMarker);
	Utils::FileWrite(fout, &dataCount);

	Utils::FileWrite(fout, entityMgr.ids_);
	Utils::FileWrite(fout, entityMgr.componentFlags_);
}

}