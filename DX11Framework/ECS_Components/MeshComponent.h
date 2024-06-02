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
	struct ComponentData : public BaseComponentData
	{
		// 'entity_id' => 'set of meshes ids'
		std::map<EntityID, std::set<MeshID>> entityToMeshes;

		// 'mesh_id' => 'set of entities ids'
		std::map<MeshID, std::set<EntityID>> meshToEntities;
	};

public:
	MeshComponent() : BaseComponent(__func__) {}

	virtual void AddRecord(const EntityID& entityID) override;
	virtual void RemoveRecord(const EntityID& entityID) override;

	void AddMeshForEntity(const EntityID& entityID, const std::string& meshID);
	void RelateEntityToMesh(const std::string& meshID, const EntityID& entityID);

	virtual std::set<EntityID> GetEntitiesIDsSet() const override
	{
		std::set<EntityID> entityIDs;

		for (const auto& it : data_.entityToMeshes)
			entityIDs.insert(it.first);

		return entityIDs;
	}

	inline const std::map<std::string, std::set<EntityID>>& GetMeshToEntitiesRecords() const
	{
		return data_.meshToEntities;
	}

public:
	ComponentData data_;
};
