// *********************************************************************************
// Filename:     RenderSystem.cpp
// Description:  implementation of the ECS RenderSystem's functional
// 
// Created:      21.05.24
// *********************************************************************************
#include "RenderSystem.h"

#include "../ECS_Common/Utils.h"
#include "../ECS_Common/log.h"
#include "../ECS_Common/LIB_Exception.h"

#include <unordered_set>
#include <stdexcept>
#include <fstream>
#include <sstream>

using namespace ECS;


RenderSystem::RenderSystem(
	Rendered* pRenderComponent,
	Transform* pTransformComponent,
	WorldMatrix* pWorldMatrixComponent,
	MeshComponent* pMeshComponent)
{
	ASSERT_NOT_NULLPTR(pRenderComponent, "ptr to the Rendered component == nullptr");
	ASSERT_NOT_NULLPTR(pTransformComponent, "ptr to the Transform component == nullptr");
	ASSERT_NOT_NULLPTR(pWorldMatrixComponent, "ptr to the WorldMatrix component == nullptr");
	ASSERT_NOT_NULLPTR(pMeshComponent, "ptr to the Mesh component == nullptr");

	pRenderComponent_ = pRenderComponent;
	pTransformComponent_ = pTransformComponent;
	pWorldMatComponent_ = pWorldMatrixComponent,
	pMeshComponent_ = pMeshComponent;
}


// *********************************************************************************
//                            PUBLIC FUNCTIONS
// *********************************************************************************

void RenderSystem::Serialize(const std::string& dataFilepath)
{
	// serialize all the data from the Rendered component into the data file

	ASSERT_NOT_EMPTY(dataFilepath.empty(), "path to the data file is empty");

	std::ofstream fout(dataFilepath, std::ios::binary);
	if (fout.is_open())
	{
		const std::vector<EntityID>& ids = pRenderComponent_->ids_;
		const std::vector<RENDERING_SHADERS>& shaderTypes = pRenderComponent_->shaderTypes_;
		const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologies = pRenderComponent_->primTopologies_;

		// if we have any data we execute serialization
		if (ids.size() > 0)
		{
			const std::string dataCountStr = { "rendered_component_data_count: " + std::to_string(ids.size()) };

			// write data into file
			fout.write(dataCountStr.c_str(), dataCountStr.size() * sizeof(char));
			fout.write((const char*)(ids.data()), ids.size() * sizeof(EntityID));
			fout.write((const char*)(shaderTypes.data()), shaderTypes.size() * sizeof(RENDERING_SHADERS));
			fout.write((const char*)(topologies.data()), topologies.size() * sizeof(D3D11_PRIMITIVE_TOPOLOGY));
		}

		fout.close();
	}
	else
	{
		THROW_ERROR("can't open file for serialization: " + dataFilepath);
	}
}

/////////////////////////////////////////////////

void RenderSystem::Deserialize(const std::string& dataFilepath)
{
	// deserialize the data from the data file into the Rendered component

	ASSERT_NOT_EMPTY(dataFilepath.empty(), "path to the data file is empty");
	
	std::ifstream fin(dataFilepath, std::ios::binary);

	try
	{
		if (fin.is_open())
		{
			Rendered& component = *pRenderComponent_;
			std::vector<EntityID>& ids = component.ids_;
			std::vector<RENDERING_SHADERS>& shaderTypes = component.shaderTypes_;
			std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologies = component.primTopologies_;

			// read into the buffer all the content of the data file
			std::stringstream buffer;
			buffer << fin.rdbuf();
			fin.close();

			// define how much component data we have to read in
			std::string ignore;
			UINT dataCount = 0;
			buffer >> ignore >> dataCount;

			ASSERT_TRUE(ignore == "rendered_component_data_count:", "RENDER COMPONENT DESERIALIZATION: read in from the file wrong block of data");
			ignore.clear();
		
			// clear the component of previous data
			ids.clear();
			shaderTypes.clear();
			topologies.clear();

			// if we have any data to read in
			if (dataCount > 0)
			{
				ids.resize(dataCount);
				shaderTypes.resize(dataCount);
				topologies.resize(dataCount);

				// read in data for the component
				buffer.read((char*)ids.data(), dataCount * sizeof(EntityID));
				buffer.read((char*)shaderTypes.data(), dataCount * sizeof(RENDERING_SHADERS));
				buffer.read((char*)topologies.data(), dataCount * sizeof(D3D11_PRIMITIVE_TOPOLOGY));
			}
		}
		else
		{
			THROW_ERROR("can't open file for deserialization: " + dataFilepath);
		}
	}
	catch (LIB_Exception& e)
	{
		fin.close();
		Log::Error(e, true);
		THROW_ERROR("can't deserialize data for the Rendered component");
	}
}

/////////////////////////////////////////////////

void RenderSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs, 
	const std::vector<RENDERING_SHADERS>& shaderTypes,
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologyTypes)
{
	// set that input entities by IDs will be rendered onto the screen;
	// also setup rendering params for each input entity;

	Rendered& component = *pRenderComponent_;

	for (size_t idx = 0; idx < enttsIDs.size(); ++idx)
	{
		// check if there is no record with such entity ID
		if (!std::binary_search(component.ids_.begin(), component.ids_.end(), enttsIDs[idx]))
		{
			// execute sorted insertion into the data arrays
			const ptrdiff_t insertAtPos = Utils::GetPosForID(component.ids_, enttsIDs[idx]);

			Utils::InsertAtPos<EntityID>(component.ids_, insertAtPos, enttsIDs[idx]);
			Utils::InsertAtPos<RENDERING_SHADERS>(component.shaderTypes_, insertAtPos, shaderTypes[idx]);
			Utils::InsertAtPos<D3D11_PRIMITIVE_TOPOLOGY>(component.primTopologies_, insertAtPos, topologyTypes[idx]);
		}
	}	
}

/////////////////////////////////////////////////

void RenderSystem::RemoveRecords(const std::vector<EntityID>& enttsIDs)
{
#if 0
	std::vector<entitiesName>& enttsIDs = pRenderComponent_->entitiesIDs_;

	const ptrdiff_t enttIdx = GetIdxOfEntityByID(enttsIDs, entitiesName);

	pRenderComponent_->renderingShaderType_.push_back(shaderType);
	pRenderComponent_->usePrimTopoECS::Logy_.push_back(primTopoECS::Logy);
	pRenderComponent_->entitiesForRendering_.erase(entitiesName);
#endif
}

/////////////////////////////////////////////////

void RenderSystem::GetRenderingDataOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<RENDERING_SHADERS>& outShaderTypes)
{
	// get necessary data for rendering of each curretly visible entity;
	// 
	// in:     array of entities IDs;
	// out:    shader type for each entity 

	GetShaderTypesOfEntts(enttsIDs, outShaderTypes);
}

/////////////////////////////////////////////////

void RenderSystem::GetEnttsIDsFromRenderedComponent(std::vector<EntityID>& outEnttsIDs)
{
	// get a bunch of entities IDs which the Rendered component has
	// out: array of entities IDs

	outEnttsIDs = pRenderComponent_->ids_;
}



// *********************************************************************************
//                           PRIVATE FUNCTIONS
// *********************************************************************************

void RenderSystem::GetShaderTypesOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<RENDERING_SHADERS>& outShaderTypes)
{
	// get shader types of each input entity by its ID;
	// 
	// in:  SORTED array of entities IDs
	// out: array of rendering shader types

	
	std::vector<ptrdiff_t> idxs; 

	idxs.reserve(std::ssize(enttsIDs));
	outShaderTypes.reserve(std::ssize(enttsIDs));

	// get index into array of each input entity by ID
	for (const EntityID& enttID : enttsIDs)
		idxs.push_back(Utils::GetIdxOfID(pRenderComponent_->ids_, enttID));

	// get shader type of each input entity
	for (const ptrdiff_t idx : idxs)
		outShaderTypes.push_back(pRenderComponent_->shaderTypes_[idx]);
}





