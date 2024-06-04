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

typedef unsigned int UINT;

class MeshSystem
{
public:
	MeshSystem(MeshComponent* pMeshComponent);

	void AddRecord(const EntityID& entityID);
	void RemoveRecord(const EntityID& entityID);

	void AddMeshForEntity(const EntityID& entityID, const MeshID& meshID);
	void RelateEntityToMesh(const MeshID& meshID, const EntityID& entityID);

	void AddMeshesToEntities(
		const std::vector<EntityID>& entityIDs,
		const std::vector<MeshID>& meshesIDs);

	// for debug/unit-test purposes
	std::set<EntityID> GetEntitiesIDsSet() const;

private:
	MeshComponent* pMeshComponent_ = nullptr;
};