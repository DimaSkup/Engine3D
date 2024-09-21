// *********************************************************************************
// Filename:     MeshComponent.h
// Description:  an ECS component which adds a mesh (or meshes) to entity
// 
// Created:      16.05.24
// *********************************************************************************
#pragma once

#include "../Common/Types.h"
#include <map>
#include <set>

namespace ECS
{

struct MeshComponent
{
	ComponentType type_ = ComponentType::MeshComp;

	std::map<EntityID, std::set<MeshID>> entityToMeshes_;  // each entity can have multiple meshes
	std::map<MeshID, std::set<EntityID>> meshToEntities_;  // each mesh can be related to multiple entities
};

}