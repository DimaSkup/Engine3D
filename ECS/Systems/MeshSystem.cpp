#include "MeshSystem.h"

#include "../Common/Assert.h"
#include "../Common/UtilsFilesystem.h"
#include "../Common/Utils.h"

#include "SaveLoad/MeshSysSerDeser.h"

#include <stdexcept>
#include <numeric>      // to use std::accumulate()

#include <ranges>
#include <set>

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
	MeshSysSerDeser::Serialize(
		fout,
		offset,
		static_cast<u32>(ComponentType::MeshComp),  // data block marker
		pMeshComponent_->enttToMeshes_);
}

///////////////////////////////////////////////////////////

void MeshSystem::Deserialize(std::ifstream& fin, const u32 offset)
{
	MeshSysSerDeser::Deserialize(
		fin,
		offset,
		pMeshComponent_->enttToMeshes_,
		pMeshComponent_->meshToEntts_);
}

///////////////////////////////////////////////////////////

void MeshSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs,
	const std::vector<MeshID>& meshesIDs)   // add this batch of meshes to each input entity
{
	// add a record about each input entity into the Mesh component;
	// NOTICE: if there is already a record by some entity ID 
	//         we just append the input batch of meshes to it;

	// CheckInputData();

	MeshComponent& comp = *pMeshComponent_;

	// make relations 'entity_id' => 'set_of_meshes_ids'
	for (const EntityID& enttID : enttsIDs)
		Utils::AppendArray(comp.enttToMeshes_[enttID], meshesIDs);

	// make relations 'mesh_id' => 'set_of_entts_ids'
	for (const MeshID& meshID : meshesIDs)
		Utils::AppendArray(comp.meshToEntts_[meshID], enttsIDs);
}

///////////////////////////////////////////////////////////

void MeshSystem::RemoveRecord(const std::vector<EntityID>& enttsIDs)
{
	Assert::True(false, "TODO: IMPLEMENT IT!");
}

///////////////////////////////////////////////////////////

void MeshSystem::GetAllMeshesIDsFromMeshComponent(std::vector<MeshID>& outMeshesIDs)
{
	// get all the meshes IDs from the Mesh component;
	// out: array of meshes IDs

	const MeshComponent& comp = *pMeshComponent_;
	outMeshesIDs.resize(comp.meshToEntts_.size());

	for (int idx = 0; const auto& it : comp.meshToEntts_)
		outMeshesIDs[idx++] = it.first;
}

///////////////////////////////////////////////////////////

void MeshSystem::GetEnttsIDsFromMeshComponent(std::vector<EntityID>& outEnttsIDs)
{
	// get only unique IDs of all the entities which the Mesh component has;
	// out: array of entities IDs

	const MeshComponent& comp = *pMeshComponent_;
	outEnttsIDs.resize(std::ssize(comp.enttToMeshes_));

	for (int idx = 0; const auto& it : comp.enttToMeshes_)
		outEnttsIDs[idx++] = it.first;
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
	std::map<MeshID, std::vector<EntityID>> meshToEntts;

	// get all the IDs of related meshes
	for (const EntityID enttID : enttsIDs)
	{
		for (const MeshID meshID : component.enttToMeshes_.find(enttID)->second)
			meshToEntts[meshID].push_back(enttID);
	}

	// get array of meshes IDs
	const auto keys = std::views::keys(meshToEntts);
	outMeshesIDs = { keys.begin(), keys.end() };

	// ---------------------------------------------

	outEnttsSortByMeshes.reserve(std::ssize(enttsIDs));
	outNumInstancesPerMesh.resize(std::ssize(meshToEntts));

	// get entts IDs sorted by meshes
	for (const auto& it : meshToEntts)
		Utils::AppendArray(outEnttsSortByMeshes, it.second);

	// get the number of entts per each mesh
	for (u32 idx = 0; const auto& it : meshToEntts)
		outNumInstancesPerMesh[idx++] = std::ssize(it.second);
}

///////////////////////////////////////////////////////////

} // namespace ECS