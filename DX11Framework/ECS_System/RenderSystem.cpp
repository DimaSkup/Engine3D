// *********************************************************************************
// Filename:     RenderSystem.cpp
// Description:  implementation of the RenderSystem's functional
// 
// Created:      21.05.24
// *********************************************************************************
#include "RenderSystem.h"
#include "../ECS_Entity/Utils.h"
#include "../Engine/log.h"

#include <unordered_set>


using XMMATRIX = DirectX::XMMATRIX;

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
	const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& primTopologyArr)
{
	std::unordered_map<EntityID, Rendered::RenderingParams>& records = pRenderComponent_->records_;

	for (size_t idx = 0; idx < shaderTypesArr.size(); ++idx)
		records.try_emplace(enttsIDs[idx], Rendered::RenderingParams(shaderTypesArr[idx], primTopologyArr[idx]));
}

///////////////////////////////////////////////////////////

void RenderSystem::RemoveRecords(const std::vector<EntityID>& enttsIDs)
{
#if 0
	std::vector<entitiesName>& enttsIDs = pRenderComponent_->entitiesIDs_;

	const ptrdiff_t enttIdx = GetIdxOfEntityByID(enttsIDs, entitiesName);

	pRenderComponent_->renderingShaderType_.push_back(shaderType);
	pRenderComponent_->usePrimTopology_.push_back(primTopology);
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

	GetWorldMatricesOfEntts(enttsIDs, outWorldMatrices);

	// get shader types of the visible entities
	outShaderTypes.reserve(std::ssize(enttsIDs));

	for (const EntityID& enttID : enttsIDs)
		outShaderTypes.push_back(pRenderComponent_->records_.at(enttID).renderingShaderType_);


#if 0
	
	// go through each mesh, get rendering data of related entities, 
	// and execute rendering
	for (const  : transientData.meshesIDsToRender)
	{
		const RENDERING_SHADERS shaderType = it.first;
		const std::vector<entitiesName>& enttsToRender = it.second;
		
		

		switch (shaderType)
		{
			case RENDERING_SHADERS::COLOR_SHADER:
			{
				// render the the current mesh
				colorShader.RenderGeometry(
					pDeviceContext,
					transientData.matricesForRendering,
					viewProj,
					meshData.indexCount,
					0.0f);

				break;
			}
			case RENDERING_SHADERS::TEXTURE_SHADER:
			{

				

				textureShader.PrepareShaderForRendering(
					pDeviceContext,
					cameraPos);

				// render the the current mesh
				textureShader.Render(
					pDeviceContext,
					transientData.matricesForRendering,
					viewProj,
					DirectX::XMMatrixIdentity(),      // textures transformation matrix
					transientData.texturesSRVs,
					meshData.indexCount);

				break;
			}
			case RENDERING_SHADERS::LIGHT_SHADER:
			{
				// if we want to render textured object we have to get its textures
				PrepareTexturesSRV_ToRender(
					meshStorage.textures_.at(meshData.dataIdx),
					transientData.texturesSRVs);

				// render the geometry from the current vertex buffer
				lightShader.RenderGeometry(
					pDeviceContext,
					meshData.material,
					viewProj,
					DirectX::XMMatrixIdentity(),      // textures transformation matrix
					transientData.matricesForRendering,
					transientData.texturesSRVs,
					meshData.indexCount);

				break;
			}
		}
	}

	// go through each mesh and render it
	for (const auto& meshToEntities : pMeshComponent_->meshToEntities_)
	{
		const std::set<entitiesName> entitiesSet = meshToEntities.second;
		

		PrepareIAStageForRendering(pDeviceContext, meshData, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		GetWorldMatricesOfEntities(entitiesSet, transientData.matricesForRendering);
		


		// RENDER GEOMETRY
		for (const auto& it : transientData.renderShadersTypesToEntts)
		{
		

			
		}

		// since we've rendered this mesh we have to clear transient data
		transientData.Clear();
	}
#endif
}

///////////////////////////////////////////////////////////

std::vector<EntityID> RenderSystem::GetEnttsIDsFromRenderedComponent() const
{
	std::vector<EntityID> enttsIDs;
	enttsIDs.reserve(std::ssize(pRenderComponent_->records_));

	for (const auto& it : pRenderComponent_->records_)
		enttsIDs.push_back(it.first);

	return enttsIDs;
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




