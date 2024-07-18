// **********************************************************************************
// Filename:      NameSystem.h
// Description:   Entity-Component-System (ECS) system for control entities names;
// 
// Created:       12.06.24
// **********************************************************************************
#pragma once

#include "../Common/Types.h"
#include "../Components/Name.h"



class NameSystem final
{
public:
	NameSystem(Name* pNameComponent);
	~NameSystem() {}

	void Serialize(std::ofstream& fout, u32& offset);
	void Deserialize(std::ifstream& fin, const u32 offset);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<EntityName>& enttsNames);
	
	void RenameRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<EntityName>& newEnttsNames);

	void RemoveRecords(const std::vector<EntityID>& enttsIDs);
	

	void PrintAllNames();
	EntityID GetIdByName(const EntityName& name);
	
private:
	bool CheckIfCanAddRecord(const EntityID& enttID, const EntityName& enttName);

private:
	Name* pNameComponent_ = nullptr;

};
