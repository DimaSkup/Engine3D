#include "NameSystem.h"

#include "../Common/Assert.h"
#include "../Common/Utils.h"
#include "../Common/log.h"

#include "SaveLoad/NameSysSerDeser.h"

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
	const Name& component = *pNameComponent_;

	NameSysSerDeser::Serialize(
		fout, 
		offset,
		static_cast<u32>(component.type_),
		component.ids_,
		component.names_);
}

///////////////////////////////////////////////////////////

void NameSystem::Deserialize(std::ifstream& fin, const u32 offset)
{
	Name& component = *pNameComponent_;

	NameSysSerDeser::Deserialize(
		fin,
		offset,
		component.ids_,
		component.names_);
}

///////////////////////////////////////////////////////////

void NameSystem::AddRecords(
	const std::vector<EntityID>& ids,
	const std::vector<EntityName>& names)
{
	// add name for each entity from the input arr

	CheckInputData(ids, names);

	for (ptrdiff_t idx = 0; idx < std::ssize(ids); ++idx)
	{
		Name& component = *pNameComponent_;
		const ptrdiff_t insertAt = Utils::GetPosForID(component.ids_, ids[idx]);

		Utils::InsertAtPos(component.ids_, insertAt, ids[idx]);
		Utils::InsertAtPos(component.names_, insertAt, names[idx]);
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
	const bool exist = Utils::ArrHasVal(comp.names_, name);
	const ptrdiff_t idx = Utils::FindIdxOfVal(comp.names_, name);

	// if there is such a name in the arr we return a responsible entity ID;
	// or in another case we return invalid ID
	return (exist) ? comp.ids_[idx] : INVALID_ENTITY_ID;
}

///////////////////////////////////////////////////////////

const EntityName& NameSystem::GetNameById(const EntityID& id) 
{
	const Name& comp = *pNameComponent_;
	const bool exist = Utils::BinarySearch(comp.ids_, id);
	const ptrdiff_t idx = Utils::FindIdxOfVal(comp.ids_, id);

	// if there is such an ID in the arr we return a responsible entity name;
	// or in another case we return invalid value
	return (exist) ? comp.names_[idx] : INVALID_ENTITY_NAME;
}

///////////////////////////////////////////////////////////

void NameSystem::CheckInputData(
	const std::vector<EntityID>& ids,
	const std::vector<EntityName>& names)
{
	// here we check if input data is correct to store it into the Name component

	const Name& component = *pNameComponent_;
	bool idsValid = true;
	bool namesValid = true;
	bool namesUnique = true;

	// check ids are valid (entts doesn't have the Name component yet)
	idsValid = !Utils::CheckValuesExistInSortedArr(component.ids_, ids);

	// check names are valid
	for (const EntityName& name : names)
		namesValid &= (!name.empty());

	// check names are unique
	namesUnique = !Utils::CheckValuesExistInArr(component.names_, names);

	Assert::True(idsValid, "there is already an entt with the Name component");
	Assert::True(namesValid, "some input name is empty");
	Assert::True(namesUnique, "some input name isn't unique");
}

}