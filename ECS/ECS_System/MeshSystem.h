// **********************************************************************************
// Filename:      MeshSystem.h
// Description:   Entity-Component-System (ECS) system for control 
//                mesh data of entities
// 
// Created:       21.05.24
// **********************************************************************************
#pragma once

#include "../ECS_Components/MeshComponent.h"

class MeshSystem final
{
public:
	MeshSystem(MeshComponent* pMeshComponent);

	void Serialize(const std::string& dataFilepath);
	void Deserialize(const std::string& dataFilepath);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<MeshID>& meshesIDs);   // add this batch of meshes to each input entity

	void RemoveRecord(const std::vector<EntityID>& enttsIDs);

	void GetAllMeshesIDsFromMeshComponent(std::vector<MeshID>& outMeshesIDs);
	void GetEnttsIDsFromMeshComponent(std::vector<EntityID>& outEnttsIDs);

	void GetMeshesIDsRelatedToEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<MeshID>& outMeshesIDs,
		std::vector<std::set<EntityID>>& outEnttsByMesh);

private:
	MeshComponent* pMeshComponent_ = nullptr;
};