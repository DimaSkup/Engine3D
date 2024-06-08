// *********************************************************************************
// Filename:     MeshComponent.h
// Description:  an ECS component which adds a mesh to entity
// 
// Created:      16.05.24
// *********************************************************************************
#pragma once

#include "../ECS_Entity/ECS_Types.h"
#include <unordered_map>
#include <map>
#include <set>


class MeshComponent
{
public:
	MeshComponent() {}

public:
	ComponentType type_ = ComponentType::MeshComp;

	// 'entity_id' => 'set of meshes ids'
	std::map<EntityID, std::set<MeshID>> entityToMeshes_;

	// 'mesh_id' => 'set of entities ids'
	std::map<MeshID, std::set<EntityID>> meshToEntities_;
};