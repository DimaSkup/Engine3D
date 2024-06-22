// **********************************************************************************
// Filename:      NameSystem.h
// Description:   Entity-Component-System (ECS) system for control entities names;
// 
// Created:       12.06.24
// **********************************************************************************
#pragma once

#include "../ECS_Common/ECS_Types.h"
#include "../ECS_Components/Name.h"



class NameSystem final
{
public:
	NameSystem(Name* pNameComponent);

	void Serialize(const std::string& dataFilepath);
	void Deserialize(const std::string& dataFilepath);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<EntityName>& enttsNames);
	
	void RenameRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<EntityName>& newEnttsNames);

	void RemoveRecords(const std::vector<EntityID>& enttsIDs);
	

	void PrintAllNames();
	
private:
	bool CheckIfCanAddRecord(const EntityID& enttID, const EntityName& enttName);

private:
	Name* pNameComponent_ = nullptr;

};
