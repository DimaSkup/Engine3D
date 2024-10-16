// *********************************************************************************
// Filename:     RenderSystem.cpp
// Description:  implementation of the ECS RenderSystem's functional
// 
// Created:      21.05.24
// *********************************************************************************
#include "RenderSystem.h"

#include "../Common/Utils.h"
#include "../Common/UtilsFilesystem.h"
#include "../Common/log.h"
#include "../Common/Assert.h"

#include <unordered_set>
#include <stdexcept>
#include <fstream>
#include <sstream>

namespace ECS
{

RenderSystem::RenderSystem(
	Rendered* pRenderComponent,
	Transform* pTransformComponent,
	WorldMatrix* pWorldMatrixComponent,
	MeshComponent* pMeshComponent)
{
	Assert::NotNullptr(pRenderComponent, "ptr to the Rendered component == nullptr");
	Assert::NotNullptr(pTransformComponent, "ptr to the Transform component == nullptr");
	Assert::NotNullptr(pWorldMatrixComponent, "ptr to the WorldMatrix component == nullptr");
	Assert::NotNullptr(pMeshComponent, "ptr to the Mesh component == nullptr");

	pRenderComponent_ = pRenderComponent;
	pTransformComponent_ = pTransformComponent;
	pWorldMatComponent_ = pWorldMatrixComponent,
	pMeshComponent_ = pMeshComponent;
}


// *********************************************************************************
//                            PUBLIC FUNCTIONS
// *********************************************************************************

void RenderSystem::Serialize(std::ofstream& fout, u32& offset)
{
	// serialize all the data from the Rendered component into the data file

	// store offset of this data block so we will use it later for deserialization
	offset = static_cast<u32>(fout.tellp());

	Rendered& component = *pRenderComponent_;
	const u32 dataBlockMarker = static_cast<u32>(pRenderComponent_->type_);
	const u32 dataCount = (u32)std::ssize(component.ids_);

	// write serialized data into the file
	Utils::FileWrite(fout, &dataBlockMarker);
	Utils::FileWrite(fout, &dataCount);

	Utils::FileWrite(fout, component.ids_);
	Utils::FileWrite(fout, component.shaderTypes_);
	Utils::FileWrite(fout, component.primTopologies_);
}

/////////////////////////////////////////////////

void RenderSystem::Deserialize(std::ifstream& fin, const u32 offset)
{
	// deserialize the data from the data file into the Rendered component
	
	// read data starting from this offset
	fin.seekg(offset, std::ios_base::beg);

	// check if we read the proper data block
	u32 dataBlockMarker = 0;
	Utils::FileRead(fin, &dataBlockMarker);

	const bool isProperDataBlock = (dataBlockMarker == static_cast<u32>(ComponentType::RenderedComponent));
	Assert::True(isProperDataBlock, "read wrong data block during deserialization of the Rendered component data from a file");

	// ------------------------------------------

	// read in how much data will we have
	u32 dataCount = 0;
	Utils::FileRead(fin, &dataCount);

	std::vector<EntityID>& ids = pRenderComponent_->ids_;
	std::vector<RENDERING_SHADERS>& shaderTypes = pRenderComponent_->shaderTypes_;
	std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologies = pRenderComponent_->primTopologies_;

	// prepare enough amount of memory for data
	ids.resize(dataCount);
	shaderTypes.resize(dataCount);
	topologies.resize(dataCount);

	Utils::FileRead(fin, ids);
	Utils::FileRead(fin, shaderTypes);
	Utils::FileRead(fin, topologies);
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

	for (u32 idx = 0; idx < enttsIDs.size(); ++idx)
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

void RenderSystem::ClearVisibleEntts()
{
	// clear an arr of entities that were visible in the previous frame;
	// so we will be able to use it again for the current frame;
	pRenderComponent_->visibleEnttsIDs_.clear();
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
		idxs.push_back(Utils::GetIdxInSortedArr(pRenderComponent_->ids_, enttID));

	// get shader type of each input entity
	for (const ptrdiff_t idx : idxs)
		outShaderTypes.push_back(pRenderComponent_->shaderTypes_[idx]);
}

}