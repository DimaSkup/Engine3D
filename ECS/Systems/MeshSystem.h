// **********************************************************************************
// Filename:      MeshSystem.h
// Description:   Entity-Component-System (ECS) system for control 
//                mesh data of entities
// 
// Created:       21.05.24
// **********************************************************************************
#pragma once

#include "../Components/MeshComponent.h"
#include <vector>
#include <fstream>

class MeshSystem final
{
public:
	MeshSystem(MeshComponent* pMeshComponent);
	~MeshSystem() {}

	void Serialize(std::ofstream& fout, u32& offset);
	void Deserialize(std::ifstream& fin, const u32 offset);

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