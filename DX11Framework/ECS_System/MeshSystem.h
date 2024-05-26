// **********************************************************************************
// Filename:      MeshSystem.h
// Description:   Entity-Component-System (ECS) system for control 
//                mesh data of entities
// 
// Created:       21.05.24
// **********************************************************************************
#pragma once

#include "BaseSystem.h"
#include "../ECS_Components/MeshComponent.h"
#include "../GameObjects/MeshStorage.h"

typedef unsigned int UINT;

class MeshSystem : public BaseSystem
{
public:
	MeshSystem() : BaseSystem("MeshSystem") {}

	void AddMeshToEntities(
		const std::vector<EntityID>& entityIDs,
		const std::string& meshID,
		MeshComponent& meshComponent)
	{
		for (const EntityID& entityID : entityIDs)
		{
			// add a mesh ID into the set of meshes of this entity
			meshComponent.AddMeshForEntity(entityID, meshID);
			meshComponent.RelateEntityToMesh(meshID, entityID);
		}
		
	}
};