// *********************************************************************************
// Filename:     MeshComponent.h
// Description:  an ECS component which adds a mesh to entity
// 
// Created:      16.05.24
// *********************************************************************************
#pragma once

#include "../ECS_Entity/ECS_Types.h"
#include "BaseComponent.h"
#include "../GameObjects/MeshStorage.h"

#include <stdexcept>
#include <map>

class MeshComponent : public BaseComponent
{
public:
	MeshComponent() : BaseComponent("Mesh") {}

	virtual void AddRecord(const EntityID& entityID) override;
	virtual void RemoveRecord(const EntityID& entityID) override;

	void AddMeshForEntity(const EntityID& entityID, const std::string& meshID);
	void RelateEntityToMesh(const std::string& meshID, const EntityID& entityID);


public:
	// 'entity_id' => 'set of meshes ids'
	std::map<EntityID, std::set<std::string>> entityToMeshes_;

	// 'mesh_id' => 'set of entities ids'
	std::map<std::string, std::set<EntityID>> meshToEntities_;
};
