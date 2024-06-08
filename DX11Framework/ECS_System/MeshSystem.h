// **********************************************************************************
// Filename:      MeshSystem.h
// Description:   Entity-Component-System (ECS) system for control 
//                mesh data of entities
// 
// Created:       21.05.24
// **********************************************************************************
#pragma once

#include "../ECS_Components/MeshComponent.h"
#include "../GameObjects/MeshStorage.h"



class MeshSystem
{
public:
	MeshSystem(MeshComponent* pMeshComponent);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<MeshID>& meshesIDs);   // add this batch of meshes to each input entity

	void RemoveRecord(const std::vector<EntityID>& enttsIDs);

	void GetAllMeshesIDsArr(std::vector<MeshID>& outMeshesIDs);

	// for debug/unit-test purposes
	std::set<EntityID> GetEntitiesIDsSet() const;

private:
	MeshComponent* pMeshComponent_ = nullptr;
};