#include "MeshSystem.h"
#include <stdexcept>




MeshSystem::MeshSystem(MeshComponent* pMeshComponent)
{
	ASSERT_NOT_NULLPTR(pMeshComponent, "ptr to the mesh component == nullptr");

	pMeshComponent_ = pMeshComponent;
}

///////////////////////////////////////////////////////////

void MeshSystem::AddRecord(const EntityID& entityID)
{
	ASSERT_NOT_EMPTY(entityID.empty(), "entity ID is empty");

	// create record: 'entity_id' => empty set
	const auto res = pMeshComponent_->entityToMeshes_.insert({ entityID, {} });
	ASSERT_TRUE(res.second, "can't create a record for entity: " + entityID);
}

///////////////////////////////////////////////////////////

void MeshSystem::RemoveRecord(const EntityID& entityID)
{
	try
	{
		std::set<std::string> meshesIDs = pMeshComponent_->entityToMeshes_.at(entityID);

		// detach this entity from all the meshes
		for (const std::string& meshID : meshesIDs)
			pMeshComponent_->meshToEntities_.at(meshID).erase(entityID);

		// remove a record about this entity (so it won't have this component)
		pMeshComponent_->entityToMeshes_.erase(entityID);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("something went out of range during removing of record: " + entityID);
	}
}

///////////////////////////////////////////////////////////

void MeshSystem::AddMeshForEntity(const EntityID& entityID, const MeshID& meshID)
{
	ASSERT_NOT_EMPTY(meshID.empty(), "mesh ID is empty");
	ASSERT_NOT_EMPTY(entityID.empty(), "entity ID is empty");

	// add a mesh to entity;
	// to be exact we create a relation between entity ID and mesh ID (string => string)
	try
	{
		// check if this entity doesn't have such a mesh yet
		if (!pMeshComponent_->entityToMeshes_.at(entityID).contains(meshID))
		{
			const auto res = pMeshComponent_->entityToMeshes_[entityID].insert(meshID);
			ASSERT_TRUE(res.second, "can't add mesh (" + meshID + ") to entity (" + entityID + ")");
		}
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't add mesh for entity [entity_id, mesh_id]: " + entityID + ";  " + meshID);
	}
}

///////////////////////////////////////////////////////////

void MeshSystem::RelateEntityToMesh(
	const MeshID& meshID, 
	const EntityID& entityID)
{
	ASSERT_NOT_EMPTY(meshID.empty(), "mesh ID is empty");
	ASSERT_NOT_EMPTY(entityID.empty(), "entity ID is empty");

	try
	{
		std::set<EntityID>& relatedEntities = pMeshComponent_->meshToEntities_.at(meshID);

		// if the entity isn't related to this mesh
		if (!relatedEntities.contains(entityID))
		{
			const auto res = relatedEntities.insert(entityID);
			ASSERT_TRUE(res.second, "can't relate entity (" + entityID + ") to mesh (" + meshID + ")");
		}
	}
	catch (const std::out_of_range&)
	{
		// there is no such a mesh yet so add a record about it and relate an entity to it
		const auto res = pMeshComponent_->meshToEntities_.insert({ meshID, {entityID} });
		ASSERT_TRUE(res.second, "can't relate entity (" + entityID + ") to mesh (" + meshID + ")");
	}
}

///////////////////////////////////////////////////////////

void MeshSystem::AddMeshesToEntities(
	const std::vector<EntityID>& entityIDs,
	const std::vector<MeshID>& meshesIDs)
{
	assert(entityIDs.size() && "the array of entities IDs is empty");
	assert(meshesIDs.size() && "the array of meshes IDs is empty");

	for (const EntityID& entityID : entityIDs)
	{
		for (const MeshID& meshID : meshesIDs)
		{
			// add a mesh ID into the set of meshes of this entity
			AddMeshForEntity(entityID, meshID);
			RelateEntityToMesh(meshID, entityID);
		}
	}
}

///////////////////////////////////////////////////////////

std::set<EntityID> MeshSystem::GetEntitiesIDsSet() const
{
	// return a set of all the entities which have the mesh component

	std::set<EntityID> entitiesIDs;

	for (const auto& it : pMeshComponent_->entityToMeshes_)
		entitiesIDs.insert(it.first);

	return entitiesIDs;
}
