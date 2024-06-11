#include "MeshSystem.h"
#include "../ECS_Common/LIB_Exception.h"

#include <stdexcept>


MeshSystem::MeshSystem(MeshComponent* pMeshComponent)
{
	ASSERT_NOT_NULLPTR(pMeshComponent, "ptr to the mesh component == nullptr");

	pMeshComponent_ = pMeshComponent;
}

///////////////////////////////////////////////////////////

void MeshSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<MeshID>& meshesIDs)   // add this batch of meshes to each input entity
{
	// add a record about each input entity into the Mesh component;
	// NOTICE: if there is already a record by some entity ID 
	//         we just add the input batch of meshes to it;

	ASSERT_NOT_EMPTY(enttsIDs.empty(), "entities IDs array is empty");

	const std::set<EntityID> enttsIDsSet{ enttsIDs.begin(), enttsIDs.end() };
	const std::set<MeshID> meshesIDsSet{ meshesIDs.begin(), meshesIDs.end() };
	
	// make relations 'entity_id' => 'set_of_meshes_ids'
	for (const EntityID& enttID : enttsIDs)
		pMeshComponent_->entityToMeshes_[enttID].insert(meshesIDsSet.begin(), meshesIDsSet.end());

	// make relations 'mesh_id' => 'set_of_entts_ids'
	for (const MeshID& meshID : meshesIDs)
		pMeshComponent_->meshToEntities_[meshID].insert(enttsIDsSet.begin(), enttsIDsSet.end());
}

///////////////////////////////////////////////////////////

void MeshSystem::RemoveRecord(const std::vector<EntityID>& enttsIDs)
{
	assert("TODO: IMPLEMENT IT!" && 0);
}

///////////////////////////////////////////////////////////

void MeshSystem::GetAllMeshesIDsArr(std::vector<MeshID>& outMeshesIDs)
{
	// get all the meshes IDs from the Mesh component;
	// out: array of meshes IDs
	outMeshesIDs.reserve(pMeshComponent_->meshToEntities_.size());

	for (const auto& it : pMeshComponent_->meshToEntities_)
		outMeshesIDs.emplace_back(it.first);
}

void MeshSystem::GetEnttsIDsFromMeshComponent(std::vector<EntityID>& outEnttsIDs)
{
	// get a set of all the entities which have the Mesh component
	// out: array of entities IDs

	outEnttsIDs.reserve(std::ssize(pMeshComponent_->entityToMeshes_));

	for (const auto& it : pMeshComponent_->entityToMeshes_)
		outEnttsIDs.push_back(it.first);
}
