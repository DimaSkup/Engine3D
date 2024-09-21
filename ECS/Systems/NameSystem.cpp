#include "NameSystem.h"

#include "../Common/Assert.h"
#include "../Common/Utils.h"
#include "../Common/log.h"

#include <fstream>

using namespace Utils;

namespace ECS
{


NameSystem::NameSystem(Name* pNameComponent)
{
	Assert::NotNullptr(pNameComponent, "ptr to the Name component == nullptr");
	pNameComponent_ = pNameComponent;
}

///////////////////////////////////////////////////////////

void NameSystem::Serialize(std::ofstream& fout, u32& offset)
{
	// serialize all the data from the Name component into the data file

	// store offset of this data block so we will use it later for deserialization
	offset = static_cast<u32>(fout.tellp());

	const std::vector<EntityID>& ids = pNameComponent_->ids_;
	const std::vector<EntityName>& names = pNameComponent_->names_;
	const u32 dataBlockMarker = static_cast<u32>(pNameComponent_->type_);
	const u32 dataCount = (u32)std::ssize(ids);

	// write the data block marker, data count, and the IDs values
	FileWrite(fout, &dataBlockMarker);
	FileWrite(fout, &dataCount);    
	FileWrite(fout, ids);

	for (const EntityName& name : names)
	{
		// go through each name and:
		// 1. write how many name's characters will we write into the file so later we will be able to read proper string;
		// 2. write name

		u32 strSize = (u32)name.size();
		FileWrite(fout, &strSize);
		FileWrite(fout, name.data(), (u32)name.size());
	}
}

///////////////////////////////////////////////////////////

void NameSystem::Deserialize(std::ifstream& fin, const u32 offset)
{
	// deserialize all the data from the data file into the Name component

	// read Name data starting from this offset
	fin.seekg(offset, std::ios_base::beg);

	// check if we read the proper data block
	u32 dataBlockMarker = 0;
	FileRead(fin, &dataBlockMarker);

	const bool isProperDataBlock = (dataBlockMarker == static_cast<int>(ComponentType::NameComponent));
	Assert::True(isProperDataBlock, "read wrong data during deserialization of the Name component data");

	// ------------------------------------------

	std::vector<EntityID>& ids = pNameComponent_->ids_;
	std::vector<EntityName>& names = pNameComponent_->names_;

	// get how many data elements we will have
	u32 dataCount = 0;
	FileRead(fin, &dataCount);

	// prepare enough amount of memory for data
	ids.resize(dataCount);
	names.resize(dataCount);

	// read in entities ids
	FileRead(fin, ids);

	// read in entities names
	for (u32 idx = 0, strSize = 0; idx < dataCount; ++idx)
	{
		FileRead(fin, &strSize);                     // read in chars count
		names[idx].resize(strSize);                         // prepare memory for a string
		FileRead(fin, names[idx].data(), strSize);   // read in a string
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
		Assert::NotEmpty(enttName.empty(), "entity name is empty for entity ID: " + std::to_string(enttID));

		if (CheckIfCanAddRecord(enttID, enttName))
		{
			const ptrdiff_t insertAt = GetPosForID(component.ids_, enttID);

			InsertAtPos(component.ids_, insertAt, enttID);
			InsertAtPos(component.names_, insertAt, enttName);
		}
	}
}

///////////////////////////////////////////////////////////

void NameSystem::PrintAllNames()
{
	const std::vector<EntityID>& ids = pNameComponent_->ids_;
	const std::vector<EntityName>& names = pNameComponent_->names_;

	for (u32 idx = 0; idx < names.size(); ++idx)
	{
		std::string msg{ "id:name = [ " + std::to_string(ids[idx]) + " : " + names[idx] + ']' };

		ECS::Log::Print(msg);
	}
}

///////////////////////////////////////////////////////////

EntityID NameSystem::GetIdByName(const EntityName& name)
{
	const Name& comp = *pNameComponent_;
	const ptrdiff_t idx = FindIdxOfVal(comp.names_, name);

	// if there is such a name in the arr we return a responsible entity ID;
	// or in another case we return invalid ID
	return (idx != std::ssize(comp.names_)) ? comp.ids_[idx] : INVALID_ENTITY_ID;                     
}

///////////////////////////////////////////////////////////

const EntityName& NameSystem::GetNameById(const EntityID& id) 
{
	const Name& comp = *pNameComponent_;
	const ptrdiff_t idx = FindIdxOfVal(comp.ids_, id);

	// if there is such an ID in the arr we return a responsible entity name;
	// or in another case we return invalid value
	return (idx != std::ssize(comp.ids_)) ? comp.names_[idx] : INVALID_ENTITY_NAME;
}

///////////////////////////////////////////////////////////

bool NameSystem::CheckIfCanAddRecord(const EntityID& enttID, const EntityName& enttName)
{
	// 1. check if there is no such an ID
	// 2. check if input name is unique
	// 
	// return: true - if there is no such ID and name is unique

	return (!BinarySearch(pNameComponent_->ids_, enttID) &&
		!ArrHasVal<EntityName>(pNameComponent_->names_, enttName));
}

}