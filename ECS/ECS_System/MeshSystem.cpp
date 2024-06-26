#include "MeshSystem.h"
#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Utils.h"
#include "../ECS_Common/log.h"

#include <stdexcept>


MeshSystem::MeshSystem(MeshComponent* pMeshComponent)
{
	ASSERT_NOT_NULLPTR(pMeshComponent, "ptr to the mesh component == nullptr");

	pMeshComponent_ = pMeshComponent;
}

///////////////////////////////////////////////////////////

void MeshSystem::Serialize(std::ofstream& fout, size_t& offset)
{
	// serialize all the data from the Mesh component into the data file

	// store offset of this data block so we will use it later for deserialization
	offset = static_cast<size_t>(fout.tellp());

	const MeshComponent& component = *pMeshComponent_;
	const size_t dataBlockMarker = static_cast<size_t>(ComponentType::MeshComp);
	const size_t dataCount = component.entityToMeshes_.size();

	// write into the file the data block marker and the data count value
	Utils::FileWrite(fout, &dataBlockMarker);
	Utils::FileWrite(fout, &dataCount);

	// if we have any data in the mesh component we serialize it
	for (const auto& it : pMeshComponent_->entityToMeshes_)
	{
		const EntityID enttID = it.first;
		const std::vector<MeshID> relatedMeshesIDs = { it.second.begin(), it.second.end() };
		const size_t relatedMeshesCount = relatedMeshesIDs.size();

		Utils::FileWrite(fout, &enttID);
		Utils::FileWrite(fout, &relatedMeshesCount);
		Utils::FileWrite(fout, relatedMeshesIDs);
	}
}

///////////////////////////////////////////////////////////

void MeshSystem::Deserialize(std::ifstream& fin, const size_t offset)
{
	// deserialize the data from the data file into the Mesh component
	
	// read data starting from this offset
	fin.seekg(offset, std::ios_base::beg);

	// check if we read the proper data block
	size_t dataBlockMarker = 0;
	Utils::FileRead(fin, &dataBlockMarker);

	const bool isProperDataBlock = (dataBlockMarker == static_cast<size_t>(ComponentType::MeshComp));
	ASSERT_TRUE(isProperDataBlock, "read wrong data block during deserialization of the Mesh component data from a file");

	// ------------------------------------------

	MeshComponent& component = *pMeshComponent_;

	// clear the component of previous data
	component.entityToMeshes_.clear();
	component.meshToEntities_.clear();

	// read in how much data will we have
	size_t dataCount = 0;
	Utils::FileRead(fin, &dataCount);

	// read in each entity ID and its related meshes IDs
	for (size_t idx = 0; idx < dataCount; ++idx)
	{
		EntityID enttID = 0;
		size_t relatedMeshesCount = 0;

		Utils::FileRead(fin, &enttID);
		Utils::FileRead(fin, &relatedMeshesCount);
			
		std::vector<MeshID> meshesIDs(relatedMeshesCount);
		Utils::FileRead(fin, meshesIDs);

		// store data into component: make pair ['entity_id' => 'set_of_related_meshes_ids']
		component.entityToMeshes_.insert({ enttID, {meshesIDs.begin(), meshesIDs.end()} });

		// store data into component: make pair ['mesh_id' => 'set_of_related_entities_ids']
		for (const MeshID meshID : meshesIDs)
			component.meshToEntities_[meshID].insert(enttID);
	}
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

	std::set<EntityID> enttsIDsSet{ enttsIDs.begin(), enttsIDs.end() };
	std::set<MeshID> meshesIDsSet{ meshesIDs.begin(), meshesIDs.end() };

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

void MeshSystem::GetAllMeshesIDsFromMeshComponent(std::vector<MeshID>& outMeshesIDs)
{
	// get all the meshes IDs from the Mesh component;
	// out: array of meshes IDs
	outMeshesIDs.reserve(pMeshComponent_->meshToEntities_.size());

	for (const auto& it : pMeshComponent_->meshToEntities_)
		outMeshesIDs.emplace_back(it.first);
}

///////////////////////////////////////////////////////////

void MeshSystem::GetEnttsIDsFromMeshComponent(std::vector<EntityID>& outEnttsIDs)
{
	// get only unique IDs of all the entities which the Mesh component has;
	// out: array of entities IDs

	outEnttsIDs.reserve(std::ssize(pMeshComponent_->entityToMeshes_));

	for (const auto& it : pMeshComponent_->entityToMeshes_)
		outEnttsIDs.push_back(it.first);
}

///////////////////////////////////////////////////////////

void MeshSystem::GetMeshesIDsRelatedToEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<MeshID>& outMeshesIDs,               // meshes by these IDs will be rendered for this frame
	std::vector<std::set<EntityID>>& outEnttsByMesh) // entities which are related to the mesh (from meshesIDs parameter)
{
	// in:     array of entts IDs
	// 
	// out: 1) array of meshes which are related to the input entities
	//      2) array of entities sets which are related to the output meshes

	outMeshesIDs.reserve(std::ssize(enttsIDs));

	for (const auto& it : pMeshComponent_->meshToEntities_)
	{
		outMeshesIDs.push_back(it.first);      // meshID
		outEnttsByMesh.push_back(it.second);   // related entts to this meshID
	}
}