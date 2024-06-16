#include "MeshSystem.h"
#include "../ECS_Common/LIB_Exception.h"
#include "../ECS_Common/Utils.h"
#include "../ECS_Common/log.h"

#include <stdexcept>
#include <fstream>
#include <sstream>


MeshSystem::MeshSystem(MeshComponent* pMeshComponent)
{
	ASSERT_NOT_NULLPTR(pMeshComponent, "ptr to the mesh component == nullptr");

	pMeshComponent_ = pMeshComponent;
}

///////////////////////////////////////////////////////////

void MeshSystem::Serialize(const std::string& dataFilepath)
{
	// serialize all the data from the Mesh component into the data file

	ASSERT_NOT_EMPTY(dataFilepath.empty(), "path to the data file is empty");

	std::ofstream fout(dataFilepath, std::ios::binary);
	if (fout.is_open())
	{
		std::stringstream ss;

		// prepare data for serialization
		for (const auto& it : pMeshComponent_->entityToMeshes_)
		{
			// entity_id + space + number_of_related_meshes + space
			ss << it.first << " " << it.second.size() << " ";

			// + related meshes IDs
			for (const MeshID& meshID : it.second)
				ss << meshID << " ";
		}

		// write serialized Mesh component data into the data file
		fout.write(ss.str().c_str(), ss.str().length());

		fout.close();
	}
	else
	{
		THROW_ERROR("can't open file for serialization: " + dataFilepath);
	}
}

///////////////////////////////////////////////////////////

void MeshSystem::Deserialize(const std::string& dataFilepath)
{
	// deserialize the data from the data file into the Mesh component

	ASSERT_NOT_EMPTY(dataFilepath.empty(), "path to the data file is empty");

	MeshComponent& component = *pMeshComponent_;
	std::ifstream fin;

	try
	{
		fin.open(dataFilepath, std::ios::binary);
		if (!fin.is_open())
		{
			THROW_ERROR("can't open file for deserialization: " + dataFilepath);
		}

		// read into the buffer all the content of the data file
		std::stringstream buffer;
		buffer << fin.rdbuf();
		fin.close();

		// clear the component of previous data
		component.entityToMeshes_.clear();
		component.meshToEntities_.clear();


		std::vector<EntityID> enttsIDs;
		std::vector<std::set<MeshID>> meshesIDsSets;

		enttsIDs.reserve(16);

		// while we didn't get to the end of the data file
		while (!buffer.eof())
		{
			UINT numOfRelatedMeshes = 0;
			EntityID enttID;
			std::set<MeshID> meshesIDs;

			// read in the entity ID and the number of related meshes
			buffer >> enttID >> numOfRelatedMeshes;

			// read in an ID of each related meshes
			while (numOfRelatedMeshes != 0)
			{
				MeshID meshID;
				buffer >> meshID;
				meshesIDs.insert(meshID);
				--numOfRelatedMeshes;
			}

			// store data into arrays so later we will use them to fill in the component
			enttsIDs.push_back(enttID);
			meshesIDsSets.push_back(meshesIDs);
		}

		// since we deserialized all the necessary data we clear up the buffer
		buffer.clear();

		// load in the Mesh component with deserialized data
		for (size_t idx = 0; idx < enttsIDs.size(); ++idx)
		{
			const EntityID& enttID = enttsIDs[idx];

			// entt_id => set_of_meshes_ids
			component.entityToMeshes_.emplace(enttID, meshesIDsSets[idx]);

			// mesh_id => set_of_entts_ids
			for (const MeshID& meshID : meshesIDsSets[idx])
				component.meshToEntities_[meshID].insert(enttID);
		}
	}
	catch (LIB_Exception& e)
	{
		fin.close();
		ECS::Log::Error(e, true);
		THROW_ERROR("something went wrong during deserialization");
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