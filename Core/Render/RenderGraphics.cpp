////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "RenderGraphics.h"

#include "../Common/MathHelper.h"
#include "../Common/Utils.h"
#include "../GameObjects/TextureManager.h"

#include <DirectXCollision.h>



using namespace DirectX;
using namespace ECS;

RenderGraphics::RenderGraphics()
{
	Log::Debug(LOG_MACRO);
}


RenderGraphics::~RenderGraphics()
{
	Log::Debug(LOG_MACRO);
}




////////////////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////



void RenderGraphics::Initialize(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const Settings & settings)      
{
	Log::Debug(LOG_MACRO);
}

///////////////////////////////////////////////////////////

void RenderGraphics::Render(
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,

	EntityManager& entityMgr,
	MeshStorage& meshStorage,
	Shaders::ShadersContainer & shadersContainer,
	SystemState & systemState,

	D3DClass & d3d,
	LightStorage & lightsStore,
	UserInterfaceClass & UI,

	const DirectX::XMMATRIX & WVO,           // world * basic_view * ortho
	const DirectX::XMMATRIX & viewProj,      // view * projection
	const DirectX::XMFLOAT3 & cameraPos,
	const DirectX::XMFLOAT3 & cameraDir,     // the direction where the camera is looking at
	const float deltaTime,                   // time passed since the previous frame
	const float totalGameTime)               // time passed since the start of the application
{
	try
	{
		// RENDER 3D STUFF
		RenderModels(
			pDevice,
			pDeviceContext,

			entityMgr,
			meshStorage,
			shadersContainer.colorShader_,
			shadersContainer.textureShader_,
			shadersContainer.lightShader_,
			systemState,
			lightsStore,

			viewProj,
			cameraPos,
			cameraDir,
			deltaTime,
			totalGameTime);

		// RENDER 2D STUFF
		d3d.TurnZBufferOff();        // turn off the Z-buffer and enable alpha blending to begin 2D rendering
		d3d.TurnOnAlphaBlending();

		UI.Render(pDeviceContext, entityMgr, WVO);

		d3d.TurnOffAlphaBlending();  // turn off alpha blending now that the text has been rendered
		d3d.TurnZBufferOn();         // turn the Z buffer back on now that the 2D rendering has completed


	}

	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't render the scene onto the screen");
	}
}

///////////////////////////////////////////////////////////

void RenderGraphics::RenderModels(
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,

	EntityManager& entityMgr,
	MeshStorage& meshStorage,
	ColorShaderClass & colorShader,
	TextureShaderClass & textureShader,
	LightShaderClass & lightShader,
	SystemState & systemState,
	LightStorage & lightsStore,

	const DirectX::XMMATRIX & viewProj,     // view * projection
	const DirectX::XMFLOAT3 & cameraPos,
	const DirectX::XMFLOAT3 & cameraDir,    // the direction where the camera is looking at
	const float deltaTime,
	const float totalGameTime)
{    
	//
	// this function prepares and renders all the visible models onto the screen
	//

#define USE_LIGHT_SHADER true

	try
	{
		TextureManager* pTexMgr = TextureManager::Get();
		MeshStorage* pMeshStorage = MeshStorage::Get();

		std::vector<EntityID> visibleEntts;// = entityMgr.GetAllEnttsIDs();
		Mesh::DataForRendering meshesData;   // for rendering

		std::vector<DirectX::XMMATRIX> worldMatrices;
		std::vector<ECS::RENDERING_SHADERS> shaderTypes;
		std::vector<MeshID> meshesIDsToRender;
		std::vector<std::set<EntityID>> enttsSortedByMeshes;

		// TEMPORARY (RENDER ALL THE ENTITIES):
		// currently we don't have any frustum culling so just
		// render all the entitites which have the Rendered component
		entityMgr.renderSystem_.GetEnttsIDsFromRenderedComponent(visibleEntts);


		// prepare entts data for rendering
		entityMgr.GetRenderingDataOfEntts(
			visibleEntts,
			worldMatrices,        // of each visible entt
			shaderTypes,          // of each visible entt
			meshesIDsToRender,
			enttsSortedByMeshes);

		// prepare meshes data for rendering
		pMeshStorage->GetMeshesDataForRendering(meshesIDsToRender, meshesData);




#if USE_LIGHT_SHADER

		const std::vector<DirectionalLight>& dirLights = lightsStore.GetDirLightsData();
		const std::vector<PointLight>& pointLights = lightsStore.GetPointLightsData();
		const std::vector<SpotLight>& spotLights = lightsStore.GetSpotLightsData();

		lightShader.Prepare(
			pDeviceContext,
			viewProj,
			cameraPos,
			dirLights,
			pointLights,
			spotLights,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

#else

		textureShader.Prepare(
			pDeviceContext,
			viewProj,
			cameraPos,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

#endif


	

		// ------------------------------------------------------
		// go through each mesh and render it

		for (size_t idx = 0; idx < meshesIDsToRender.size(); ++idx)
		{
			// entities which are related to the current mesh
			const std::vector<EntityID>& relatedEntts = { enttsSortedByMeshes[idx].begin(), enttsSortedByMeshes[idx].end() };
			MeshName meshName = meshesData.names_[idx];


			std::vector<DirectX::XMMATRIX> worldMatricesToRender;
			std::vector<DirectX::XMMATRIX> texTransforms;

			// get world matrices of entts related to this mesh
			GetEnttsWorldMatricesForRendering(
				visibleEntts,
				relatedEntts,
				worldMatrices,
				worldMatricesToRender);


			entityMgr.texTransformSystem_.GetTexTransformsForEntts(
				relatedEntts, 
				texTransforms);

			// get own textures of entities (if it has the Textured component)
			std::vector<const TexIDsArr*> enttsTexIDsArrays;
			std::vector<EntityID> enttsNoTexComp;           // use related mesh textures set
			std::vector<EntityID> enttsWithTexComp;         // use own textures set
			std::vector<TextureClass*> texObjPtrs;
			std::vector<ID3D11ShaderResourceView*> entitiesTexSRVs;

			entityMgr.texturesSystem_.GetTexIDsByEnttsIDs(
				relatedEntts,
				enttsNoTexComp,
				enttsWithTexComp,
				enttsTexIDsArrays);


			// get arr of SRV of the current mesh
			const u32 uniqueTexSets = (u32)enttsWithTexComp.size() + 1;  // one mesh + number of entities with own textures
			
			std::vector<TexID> texIDs;   // mesh textures IDs + entts textures IDs
			texIDs.reserve(uniqueTexSets * TextureClass::TEXTURE_TYPE_COUNT);

			// concatenate arrays of mesh textures IDs and entts textures IDs
			Utils::AppendArray(texIDs, meshesData.texIDs_[idx]);

			for (const TexIDsArr* ptrToArr : enttsTexIDsArrays)
				Utils::AppendArray(texIDs, *ptrToArr);

			// get SRV (shader resource view) of each texture
			SRVsArr texSRVs;
			pTexMgr->GetSRVsByTexIDs(texIDs, texSRVs);

			// instances with default textures from the mesh
			std::vector<UINT> instancesCounts;
			instancesCounts.push_back((UINT)std::ssize(enttsNoTexComp));

			// instances each with its own textures set
			std::vector<UINT>instancesPerTexSet(enttsWithTexComp.size(), 1);
			Utils::AppendArray(instancesCounts, instancesPerTexSet);
		

#if USE_LIGHT_SHADER

			lightShader.UpdateInstancedBuffer(
				pDeviceContext,
				worldMatricesToRender,
				texTransforms,
				std::vector<Material>(relatedEntts.size(), meshesData.materials_[idx]));
			

			lightShader.Render(
				pDeviceContext,
				meshesData.pVBs_[idx],
				meshesData.pIBs_[idx],
				texSRVs,
				instancesCounts,
				meshesData.indexCount_[idx]);

#else
			textureShader.UpdateInstancedBuffer(
				pDeviceContext,
				worldMatricesToRender,
				texTransforms);

			// render the the current mesh
			textureShader.Render(
				pDeviceContext,
				*meshData.ppVertexBuffer,
				meshData.pIndexBuffer,
				texSRVs,
				meshData.indexCount,
				(UINT)std::ssize(relatedEntts));
#endif

			
		}
	}
	catch (EngineException& e)
	{
		// if we got here that means we didn't manage to render geometry with some shader
		// so at least try to render it with the color shader
		Log::Error(e, false);
	}
}

///////////////////////////////////////////////////////////

void RenderGraphics::PrepareIAStageForRendering(
	ID3D11DeviceContext* pDeviceContext,
	const Mesh::DataForRendering& meshData,
	const D3D11_PRIMITIVE_TOPOLOGY topologyType)
{
	

	return;
}

///////////////////////////////////////////////////////////

void RenderGraphics::GetEnttsWorldMatricesForRendering(
	const std::vector<EntityID>& visibleEntts,
	const std::vector<EntityID>& enttsIDsToGetMatrices,
	const std::vector<DirectX::XMMATRIX>& inWorldMatrices,   // world matrices of all the currently visible entts
	std::vector<DirectX::XMMATRIX>& outWorldMatrices)
{
	std::vector<ptrdiff_t> dataIdxs;

	outWorldMatrices.reserve(std::ssize(enttsIDsToGetMatrices));
	dataIdxs.reserve(std::size(outWorldMatrices));

	// get data idxs of entts
	for (const EntityID& enttID : enttsIDsToGetMatrices)
		dataIdxs.push_back(std::distance(visibleEntts.begin(), std::upper_bound(visibleEntts.begin(), visibleEntts.end(), enttID)) - 1);

	// get world matrices
	for (const ptrdiff_t idx : dataIdxs)
		outWorldMatrices.emplace_back(inWorldMatrices[idx]);
}

///////////////////////////////////////////////////////////

void RenderGraphics::PrepareTexturesSRV_ToRender(
	const std::vector<TextureClass*>& textures,
	std::vector<ID3D11ShaderResourceView* const*>& outTexturesSRVs)
{
	// get a bunch of pointers to SRVs (shader resource views) by input textures array

	outTexturesSRVs.reserve(textures.size());

	for (const TextureClass* pTexture : textures)
	{
		if (pTexture)
			outTexturesSRVs.push_back(pTexture->GetTextureResourceViewAddress());
		else
			outTexturesSRVs.push_back(nullptr);
	}
}