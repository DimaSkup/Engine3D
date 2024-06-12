#include "NameSystem.h"

#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Utils.h"
#include "../ECS_Common/log.h"


NameSystem::NameSystem(Name* pNameComponent)
{
	ASSERT_NOT_NULLPTR(pNameComponent, "ptr to the Name component == nullptr");
	pNameComponent_ = pNameComponent;
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