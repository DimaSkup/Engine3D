// *********************************************************************************
// Filename:     MeshComponent.h
// Description:  implementation of the Mesh component's functional
// 
// Created:      22.05.24
// *********************************************************************************
#include "MeshComponent.h"




void MeshComponent::AddRecord(const EntityID& entityID)
{
	// create record: 'entity_id' => empty set
	const auto res = entityToMeshes_.insert({ entityID, {} });
	COM_ERROR_IF_FALSE(res.second, "can't create a record for entity: " + entityID);
}

///////////////////////////////////////////////////////////

void MeshComponent::RemoveRecord(const EntityID& entityID)
{
	try
	{
		std::set<std::string> meshesIDs = entityToMeshes_.at(entityID);

		// detach this entity from all the meshes
		for (const std::string& meshID : meshesIDs)
			meshToEntities_.at(meshID).erase(entityID);

		// remove a record about this entity (so it won't have this component)
		entityToMeshes_.erase(entityID);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR("something went out of range during removing of record: " + entityID);
	}
}

///////////////////////////////////////////////////////////

void MeshComponent::AddMeshForEntity(const EntityID& entityID, const std::string& meshID)
{
	try
	{
		// add a mesh to entity
		std::set<std::string>& entityMeshes = entityToMeshes_.at(entityID);

		if (!entityMeshes.contains(meshID))
		{
			const auto res = entityMeshes.insert(meshID);
			COM_ERROR_IF_FALSE(res.second, "can't add mesh (" + meshID + ") to entity (" + entityID + ")");
		}
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		COM_ERROR("can't add mesh for entity [entity_id, mesh_id]: " + entityID + ";  " + meshID);
	}
}

///////////////////////////////////////////////////////////

void MeshComponent::RelateEntityToMesh(const std::string& meshID, const EntityID& entityID)
{
	try
	{
		// get a set of related entities to this mesh
		std::set<EntityID>& relatedEntities = meshToEntities_.at(meshID);

		if (!relatedEntities.contains(entityID))
		{
			const auto res = relatedEntities.insert(entityID);
			COM_ERROR_IF_FALSE(res.second, "can't relate entity (" + entityID + ") to mesh (" + meshID + ")");
		}
	}
	catch (const std::out_of_range&)
	{
		// there is no such a mesh yet so add a record about it and relate an entity to it
		const auto res = meshToEntities_.insert({ meshID, {entityID} });
		COM_ERROR_IF_FALSE(res.second, "can't relate entity (" + entityID + ") to mesh (" + meshID + ")");
	}
}

///////////////////////////////////////////////////////////