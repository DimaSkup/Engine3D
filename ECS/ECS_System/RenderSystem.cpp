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

void RenderSystem::AddRecords(
	const std::vector<EntityID>& enttsIDs, 
	const std::vector<RENDERING_SHADERS>& shaderTypesArr,
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& topologyTypes)
{
	for (size_t idx = 0; idx < shaderTypesArr.size(); ++idx)
		pRenderComponent_->records_.try_emplace(enttsIDs[idx], RenderingParams(shaderTypesArr[idx], topologyTypes[idx]));
}

///////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////

void RenderSystem::GetRenderingDataOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<XMMATRIX>& outWorldMatrices,
	std::vector<RENDERING_SHADERS>& outShaderTypes)
{
	// get necessary data for rendering of each curretly visible entity;
	// 
	// in:     array of entities IDs;
	// 
	// out: 1) shader type for each entity
	//      2) world matrix of each entity 

	GetShaderTypesOfEntts(enttsIDs, outShaderTypes);
	GetWorldMatricesOfEntts(enttsIDs, outWorldMatrices);
}

///////////////////////////////////////////////////////////

void RenderSystem::GetEnttsIDsFromRenderedComponent(std::vector<EntityID>& outEnttsIDs)
{
	// get a bunch of entities IDs which the Rendered component has
	// out: array of entities IDs

	outEnttsIDs.reserve(std::ssize(pRenderComponent_->records_));

	for (const auto& it : pRenderComponent_->records_)
		outEnttsIDs.push_back(it.first);
}


// *********************************************************************************
// 
//                           PRIVATE FUNCTIONS
// 
// *********************************************************************************


void RenderSystem::GetMeshesRelatedToEntts(
	const std::vector<EntityID>& enttsToRender,
	std::unordered_set<MeshID>& outMeshesIDsToRender)
{
	// get unique meshes IDs which are related to the input entities
	for (const EntityID& enttID : enttsToRender)
	{
		const std::set<MeshID>& meshesIDs = pMeshComponent_->entityToMeshes_[enttID];
		outMeshesIDsToRender.insert(meshesIDs.begin(), meshesIDs.end());
	}
}

///////////////////////////////////////////////////////////

void RenderSystem::GetWorldMatricesOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<DirectX::XMMATRIX>& outWorldMatrices)
{
	// get world matrices of entities by its IDs from the WorldMatrix component
	// in:  array of entities IDs
	// out: array of world matrices
	outWorldMatrices.reserve(std::ssize(enttsIDs));

	for (const EntityID& enttID : enttsIDs)
		outWorldMatrices.push_back(pWorldMatComponent_->worlds_[enttID]);
}

///////////////////////////////////////////////////////////

void RenderSystem::GetShaderTypesOfEntts(
	const std::vector<EntityID>& enttsIDs,
	std::vector<RENDERING_SHADERS>& outShaderTypes)
{
	// get shader types of each input entity by its ID
	// out: array of rendering shader types

	outShaderTypes.reserve(std::ssize(enttsIDs));

	// get a rendering shader type for each entity by its ID
	for (const EntityID& enttID : enttsIDs)
	{
		try
		{
			outShaderTypes.push_back(pRenderComponent_->records_.at(enttID).renderingShaderType_);
		}
		catch (const std::out_of_range& e)
		{
			ECS::Log::Error(LOG_MACRO, e.what());
			THROW_ERROR("can't find a record with entity ID (# " + std::to_string(enttID) + ") inside the Rendered component");
		}
	}
}





