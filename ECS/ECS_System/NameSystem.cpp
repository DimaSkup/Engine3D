#include "NameSystem.h"

#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Utils.h"
#include "../ECS_Common/log.h"

#include <fstream>



NameSystem::NameSystem(Name* pNameComponent)
{
	ASSERT_NOT_NULLPTR(pNameComponent, "ptr to the Name component == nullptr");
	pNameComponent_ = pNameComponent;
}

///////////////////////////////////////////////////////////

void NameSystem::Serialize(std::ofstream& fout, size_t& offset)
{
	// serialize all the data from the Name component into the data file

	// store offset of this data block so we will use it later for deserialization
	offset = static_cast<size_t>(fout.tellp());

	const std::vector<EntityID>& ids = pNameComponent_->ids_;
	const std::vector<EntityName>& names = pNameComponent_->names_;
	const size_t dataBlockMarker = static_cast<size_t>(pNameComponent_->type_);
	const size_t dataCount = ids.size();

	// write the data block marker, data count, and the IDs values
	Utils::FileWrite(fout, &dataBlockMarker);
	Utils::FileWrite(fout, &dataCount);    
	Utils::FileWrite(fout, ids);

	for (const EntityName& name : names)
	{
		// go through each name and:
		// 1. write how many name's characters will we write into the file so later we will be able to read proper string;
		// 2. write name

		size_t strSize = name.size();
		Utils::FileWrite(fout, &strSize);
		Utils::FileWrite(fout, name.data(), name.size());
	}
}

///////////////////////////////////////////////////////////

void NameSystem::Deserialize(std::ifstream& fin, const size_t offset)
{
	// deserialize all the data from the data file into the Name component

	// read Name data starting from this offset
	fin.seekg(offset, std::ios_base::beg);

	// check if we read the proper data block
	size_t dataBlockMarker = 0;
	Utils::FileRead(fin, &dataBlockMarker);

	const bool isProperDataBlock = (dataBlockMarker == static_cast<int>(ComponentType::NameComponent));
	ASSERT_TRUE(isProperDataBlock, "read wrong data during deserialization of the Name component data");

	// ------------------------------------------

	std::vector<EntityID>& ids = pNameComponent_->ids_;
	std::vector<EntityName>& names = pNameComponent_->names_;

	// get how many data elements we will have
	size_t dataCount = 0;
	Utils::FileRead(fin, &dataCount);

	// prepare enough amount of memory for data
	ids.resize(dataCount);
	names.resize(dataCount);

	// read in entities ids
	Utils::FileRead(fin, ids);

	// read in entities names
	for (size_t idx = 0, strSize = 0; idx < dataCount; ++idx)
	{
		Utils::FileRead(fin, &strSize);                     // read in chars count
		names[idx].resize(strSize);                         // prepare memory for a string
		Utils::FileRead(fin, names[idx].data(), strSize);   // read in a string
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