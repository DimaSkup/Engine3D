#include "MeshSystem.h"

#include "../Common/Assert.h"
#include "../Common/Utils.h"
#include "../Common/log.h"

#include <stdexcept>
#include <numeric>      // to use std::accumulate()

#include <ranges>

namespace ECS
{

MeshSystem::MeshSystem(MeshComponent* pMeshComponent)
{
	Assert::NotNullptr(pMeshComponent, "ptr to the mesh component == nullptr");

	pMeshComponent_ = pMeshComponent;
}

///////////////////////////////////////////////////////////

void MeshSystem::Serialize(std::ofstream& fout, u32& offset)
{
	// serialize all the data from the Mesh component into the data file

	// store offset of this data block so we will use it later for deserialization
	offset = static_cast<u32>(fout.tellp());

	const MeshComponent& component = *pMeshComponent_;
	const u32 dataBlockMarker = static_cast<u32>(ComponentType::MeshComp);
	const u32 dataCount = static_cast<u32>(std::ssize(component.entityToMeshes_));

	// write into the file the data block marker and the data count value
	Utils::FileWrite(fout, &dataBlockMarker);
	Utils::FileWrite(fout, &dataCount);

	// if we have any data in the mesh component we serialize it
	for (const auto& it : pMeshComponent_->entityToMeshes_)
	{
		const EntityID enttID = it.first;
		const std::vector<MeshID> relatedMeshesIDs = { it.second.begin(), it.second.end() };
		const u32 relatedMeshesCount = static_cast<u32>(std::ssize(relatedMeshesIDs));

		Utils::FileWrite(fout, &enttID);
		Utils::FileWrite(fout, &relatedMeshesCount);
		Utils::FileWrite(fout, relatedMeshesIDs);
	}
}

///////////////////////////////////////////////////////////

void MeshSystem::Deserialize(std::ifstream& fin, const u32 offset)
{
	// deserialize the data from the data file into the Mesh component
	
	// read data starting from this offset
	fin.seekg(offset, std::ios_base::beg);

	// check if we read the proper data block
	u32 dataBlockMarker = 0;
	Utils::FileRead(fin, &dataBlockMarker);

	const bool isProperDataBlock = (dataBlockMarker == static_cast<u32>(ComponentType::MeshComp));
	Assert::True(isProperDataBlock, "read wrong data block during deserialization of the Mesh component data from a file");

	// ------------------------------------------

	MeshComponent& component = *pMeshComponent_;

	// clear the component of previous data
	component.entityToMeshes_.clear();
	component.meshToEntities_.clear();

	// read in how much data will we have
	u32 dataCount = 0;
	Utils::FileRead(fin, &dataCount);

	// read in each entity ID and its related meshes IDs
	for (u32 idx = 0; idx < dataCount; ++idx)
	{
		EntityID enttID = 0;
		u32 relatedMeshesCount = 0;

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

	Assert::NotEmpty(enttsIDs.empty(), "entities IDs array is empty");

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
	const std::vector<EntityID>& enttsIDs,           // by these entts we will get meshes
	std::vector<MeshID>& outMeshesIDs,               // meshes by these IDs will be rendered for this frame
	std::vector<EntityID>& outEnttsSortByMeshes,
	std::vector<size>& outNumInstancesPerMesh)
{
	// in:     array of entts IDs
	// 
	// out: 1) arr of meshes which are related to the input entities
	//      2) arr of entts sorted by its meshes
	//      3) arr of entts number per mesh

	
	const MeshComponent& component = *pMeshComponent_;

#if 0
	bool enttsValid = true;

	// go through each entt and define if it is valid (has any meshes)
	for (const EntityID enttID : enttsIDs)
		enttsValid &= component.entityToMeshes_.contains(enttID);

	Assert::True(enttsValid, "there is some entt which doesn't have any mesh");
#endif

	// -----------------------------------

	std::map<MeshID, std::set<EntityID>> meshToEntts;

	// get all the IDs of related meshes
	for (const EntityID enttID : enttsIDs)
	{
		for (const MeshID meshID : component.entityToMeshes_.find(enttID)->second)
			meshToEntts[meshID].insert(enttID);
	}

	// get array of meshes IDs
	const auto keys = std::views::keys(meshToEntts);
	outMeshesIDs = { keys.begin(), keys.end() };

	// ---------------------------------------------

	outEnttsSortByMeshes.reserve(std::ssize(enttsIDs));
	outNumInstancesPerMesh.resize(std::ssize(meshToEntts));

	// get entts IDs sorted by meshes
	for (const auto& it : meshToEntts)
		Utils::AppendArray(outEnttsSortByMeshes, { it.second.begin(), it.second.end() });

	// get the number of entts per each mesh
	for (u32 idx = 0; const auto& it : meshToEntts)
		outNumInstancesPerMesh[idx++] = std::ssize(it.second);
}

///////////////////////////////////////////////////////////

} // namespace ECS