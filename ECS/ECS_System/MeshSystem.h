// **********************************************************************************
// Filename:      MeshSystem.h
// Description:   Entity-Component-System (ECS) system for control 
//                mesh data of entities
// 
// Created:       21.05.24
// **********************************************************************************
#pragma once

#include "../ECS_Components/MeshComponent.h"
#include <vector>
#include <fstream>

class MeshSystem final
{
public:
	MeshSystem(MeshComponent* pMeshComponent);

	void Serialize(std::ofstream& fout, size_t& offset);
	void Deserialize(std::ifstream& fin, const size_t offset);

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