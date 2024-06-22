#include "NameSystem.h"

#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Utils.h"
#include "../ECS_Common/log.h"

#include <fstream>
#include <sstream>


NameSystem::NameSystem(Name* pNameComponent)
{
	ASSERT_NOT_NULLPTR(pNameComponent, "ptr to the Name component == nullptr");
	pNameComponent_ = pNameComponent;
}

///////////////////////////////////////////////////////////

void NameSystem::Serialize(const std::string& dataFilepath)
{
	// serialize all the data from the Name component into the data file

	ASSERT_NOT_EMPTY(dataFilepath.empty(), "path to the data file is empty");

	std::ofstream fout(dataFilepath, std::ios::binary);
	ASSERT_TRUE(fout.is_open(), "can't open file for serialization: " + dataFilepath);

	Name& nameComp = *pNameComponent_;

	// make string about how much data we have for each data array 
	// so later we will use this number for deserialization
	const ptrdiff_t dataCount = std::ssize(nameComp.ids_);
	const std::string dataCountStr = "name_data_count: " + std::to_string(dataCount);

	std::stringstream buffer;
	buffer.write(dataCountStr.c_str(), dataCountStr.size());

	// if we have any data for serialization we write data into the buffer 
	if (dataCount > 0)
	{
		buffer.write((const char*)nameComp.ids_.data(), dataCount * sizeof(EntityID));

		for (const EntityName& name : nameComp.names_)
			buffer << name << ' ';

		// write into the data file content of the buffer
		fout << buffer.rdbuf();
	}

	fout.close();
}

///////////////////////////////////////////////////////////

void NameSystem::Deserialize(const std::string& dataFilepath)
{
	// deserialize all the data from the data file into the Name component
	ASSERT_NOT_EMPTY(dataFilepath.empty(), "path to the data file is empty");

	std::ifstream fin(dataFilepath, std::ios::binary);
	ASSERT_TRUE(fin.is_open(), "can't open a file for deserialization: " + dataFilepath);

	// read into buffer all the file content
	std::stringstream buffer;
	buffer << fin.rdbuf();
	fin.close();
	
	// read in how many data we have for the Name component
	UINT dataCount = 0;
	std::string ignore;

	buffer >> ignore >> dataCount;

	// check if we read the proper data block
	ASSERT_TRUE(ignore == "name_data_count:", "read wrong data during deserialization of the Name component data from a file: " + dataFilepath);

	// clear the Name component from the previous data
	Name& nameComp = *pNameComponent_;

	std::vector<EntityID>& ids = pNameComponent_->ids_;
	std::vector<EntityName>& names = pNameComponent_->names_;

	ids.clear();
	names.clear();

	// if we have any data
	if (dataCount > 0)
	{
		ids.resize(dataCount);
		names.resize(dataCount);

		buffer.read((char*)ids.data(), dataCount * sizeof(EntityID));

		for (UINT idx = 0; idx < dataCount; ++idx)
			buffer >> names[idx];
	}
}

///////////////////////////////////////////////////////////

void NameSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<EntityName>& enttsNames)
{
	// add name for each entity from the input arr

	Name& component = *pNameComponent_;

	for (ptrdiff_t idx = 0; idx < std::ssize(enttsIDs); ++idx)
	{
		const EntityID& enttID = enttsIDs[idx];
		const EntityName& enttName = enttsNames[idx];
		ASSERT_NOT_EMPTY(enttName.empty(), "entity name is empty for entity ID: " + std::to_string(enttID));

		if (CheckIfCanAddRecord(enttID, enttName))
		{
			const ptrdiff_t insertAt = Utils::GetPosForID(component.ids_, enttID);

			Utils::InsertAtPos<EntityID>(component.ids_, insertAt, enttID);
			Utils::InsertAtPos<EntityName>(component.names_, insertAt, enttName);
		}
	}
}

///////////////////////////////////////////////////////////

void NameSystem::PrintAllNames()
{
	const std::vector<EntityID>& ids = pNameComponent_->ids_;
	const std::vector<EntityName>& names = pNameComponent_->names_;

	for (size_t idx = 0; idx < names.size(); ++idx)
	{
		std::string msg{ "id:name = [ " + std::to_string(ids[idx]) + " : " + names[idx] + ']' };

		ECS::Log::Print(msg);
	}
}

///////////////////////////////////////////////////////////

bool NameSystem::CheckIfCanAddRecord(const EntityID& enttID, const EntityName& enttName)
{
	// 1. check if there is already such an ID
	// 2. check if name is unique
	// 
	// return: true - if there is no such ID and name is unique

	return (!std::binary_search(pNameComponent_->ids_.begin(), pNameComponent_->ids_.end(), enttID) &&
		!Utils::ArrHasVal<EntityName>(pNameComponent_->names_, enttName));
}