////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "RenderGraphics.h"

#include "../Common/MathHelper.h"
#include "../GameObjects/TextureManagerClass.h"

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

void RenderGraphics::Update()
{
	//
	// Update the scene for this frame
	//

	


	

	

	////////////////////////////////////////////////
	//  UPDATE THE WAVES MODEL
	////////////////////////////////////////////////

#if 0
	//
	// Every quarter second, generate a random wave.
	//

	static float t_base = 0.0f;
	if ((totalGameTime - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % 190;
		DWORD j = 5 + rand() % 190;

		float r = MathHelper::RandF(1.0f, 2.0f);

		modelsStore.waves_.Disturb(i, j, r);
	}

#endif
	//const bool isWaveUpdated = modelsStore.waves_.Update(deltaTime);

	// if geometry of the wave was updated we have to update 
	// the related vertex buffer with new vertices
	//if (isWaveUpdated)
	if (true)
	{
#if 0
		// build new vertices for the current wave state
		const UINT vertexCount = modelsStore.waves_.GetVertexCount();
		const std::vector<XMFLOAT3> verticesPos(modelsStore.waves_.GetPositions());
		const std::vector<XMFLOAT3> verticesNorm(modelsStore.waves_.GetNormals());
		std::vector<VERTEX> verticesArr(vertexCount);
		const float wavesWidth_inv = 1.0f / modelsStore.waves_.GetWidth();
		const float wavedDepth_inv = 1.0f / modelsStore.waves_.GetDepth();


		// setup new vertices for the wave
		for (UINT idx = 0; idx < vertexCount; ++idx)
		{
			verticesArr[idx].position = verticesPos[idx];
			verticesArr[idx].normal = verticesNorm[idx];

			// derive tex-coords in [0,1] from position.
			verticesArr[idx].texture.x = 0.5f + verticesPos[idx].x * wavesWidth_inv;
			verticesArr[idx].texture.y = 0.5f - verticesPos[idx].z * wavedDepth_inv;
		}
				//
		// Update the wave vertex buffer with the new solution.
		//
		const uint32_t wavesIdx = modelsStore.GetIdxByTextID("waves");
		const UINT vertexBuffer_idx = modelsStore.GetRelatedVertexBufferByModelIdx(wavesIdx);
		modelsStore.vertexBuffers_[vertexBuffer_idx].UpdateDynamic(pDeviceContext, verticesArr);

#endif	


	
	}

#if 0
	// 
	// Animate water texture coordinates
	//

	const uint32_t wavesIdx = modelsStore.GetIndexByTextID("waves");

	// translate texture over time
	DirectX::XMFLOAT2 texOffset{
		modelsStore.texTransform_[wavesIdx].r[3].m128_f32[0],   // offset by X
		modelsStore.texTransform_[wavesIdx].r[3].m128_f32[1]    // offset by Y
	};

	texOffset.x += 0.1f * deltaTime;       // offset texture by X 
	texOffset.y += 0.05f * deltaTime;      // offset texture by Y

	const DirectX::XMMATRIX wavesOffset = DirectX::XMMatrixTranslation(texOffset.x, texOffset.y, 0.0f);

	// tile water texture
	const DirectX::XMMATRIX wavesScale = DirectX::XMMatrixScaling(5, 5, 5);

	// combine scale and translation
	modelsStore.texTransform_[wavesIdx] = DirectX::XMMatrixMultiply(wavesScale, wavesOffset);

#endif 

	return;
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

	try
	{
		TextureManagerClass* pTexMgr = TextureManagerClass::Get();
		MeshStorage* pMeshStorage = MeshStorage::Get();

		std::vector<EntityID> visibleEntts;// = entityMgr.GetAllEnttsIDs();
		std::vector<Mesh::DataForRendering> meshesDataForRender;
		
		std::vector<DirectX::XMMATRIX> worldMatrices;
		std::vector<ECS::RENDERING_SHADERS> shaderTypes;
		std::vector<MeshID> meshesIDsToRender;
		std::vector<std::set<EntityID>> enttsSortedByMeshes;

		// TEMPORARY: currently we don't have any frustum culling so just
		// render all the entitites which have the Rendered component
		entityMgr.renderSystem_.GetEnttsIDsFromRenderedComponent(visibleEntts);



		// ------------------------------------------------------
		// prepare entts data for rendering

		entityMgr.GetRenderingDataOfEntts(
			visibleEntts, 
			worldMatrices,        // of each visible entt
			shaderTypes,          // of each visible entt
			meshesIDsToRender,    
			enttsSortedByMeshes);


		// ------------------------------------------------------
		// prepare meshes data for rendering

		
	

		pMeshStorage->GetMeshesDataForRendering(meshesIDsToRender, meshesDataForRender);

	

		// ------------------------------------------------------
		// go through each mesh and rendering it

		for (size_t idx = 0; idx < meshesIDsToRender.size(); ++idx)
		{
			// entities that have the current mesh
			const std::vector<EntityID>& relatedEntts = { enttsSortedByMeshes[idx].begin(), enttsSortedByMeshes[idx].end() };
			const Mesh::DataForRendering& meshData = meshesDataForRender[idx];

			if (meshData.name == "cylinder")
			{
				colorShader.Render(
					pDeviceContext,
					*meshData.ppVertexBuffer,
					meshData.pIndexBuffer,
					viewProj,
					meshData.indexCount,
					totalGameTime);
			}
			else
			{
				continue;
			}


			// TEMPORARY: FOR DEBUG
			RENDERING_SHADERS renderingShaderType = RENDERING_SHADERS::COLOR_SHADER;
			MeshName meshName = meshData.name;

			std::vector<XMMATRIX> texTransforms;                               // textures transformations
			std::vector<ID3D11ShaderResourceView* const*> texturesSRVs;
			std::vector<DirectX::XMMATRIX> worldMatricesToRender;


			entityMgr.texTransformSystem_.GetTexTransformsForEntts(relatedEntts, texTransforms);
			//PrepareTexturesSRV_ToRender(meshData.textures, texturesSRVs);
			//PrepareIAStageForRendering(pDeviceContext, meshData, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// get world matrices of entts related to this mesh
			GetEnttsWorldMatricesForRendering(
				visibleEntts,
				relatedEntts, 
				worldMatrices,            
				worldMatricesToRender);
			
			const EntityID fireflameEntt = entityMgr.nameSystem_.GetIdByName("fireflame");
			ASSERT_TRUE(fireflameEntt != INVALID_ENTITY_ID, "wrong entity ID");

			const TexturesSet& fireflameTexSet = entityMgr.texturesSystem_.GetTexturesSetForEntt(fireflameEntt);

			std::vector<TextureClass*> texturesPtrsSet;
			texturesPtrsSet.reserve(Textured::TEXTURES_TYPES_COUNT);

			for (const TextureID& texID : fireflameTexSet)
			{
				TextureClass* pTexture = (!texID.empty()) ? pTexMgr->GetTextureByKey(texID) : nullptr;
				texturesPtrsSet.push_back(pTexture);
			}

			PrepareTexturesSRV_ToRender(texturesPtrsSet, texturesSRVs);
			try
			{
				switch (renderingShaderType)
				{
					case RENDERING_SHADERS::COLOR_SHADER:
					{
						UINT offset = 0;
						pDeviceContext->IASetVertexBuffers(0, 1, meshData.ppVertexBuffer, meshData.pStride, &offset);

						// set what primitive topology we want to use to render this vertex buffer
						pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

						pDeviceContext->IASetIndexBuffer(
							meshData.pIndexBuffer,                           // pIndexBuffer
							DXGI_FORMAT::DXGI_FORMAT_R32_UINT, // format of the indices
							0);                                // offset, in bytes

				
						colorShader.Render(
							pDeviceContext,
							*meshData.ppVertexBuffer,
							meshData.pIndexBuffer,
							viewProj,
							meshesDataForRender[idx].indexCount,
							totalGameTime);

						break;
					}
					case RENDERING_SHADERS::TEXTURE_SHADER:
					{
						textureShader.PrepareShaderForRendering(
							pDeviceContext,
							cameraPos,
							*meshData.ppVertexBuffer,
							meshData.pIndexBuffer,
							D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

						// render the the current mesh
						textureShader.Render(
							pDeviceContext,
							worldMatricesToRender,
							viewProj,
							texTransforms,      // textures transformation matrices
							texturesSRVs,
							meshData.indexCount);
						break;
					}
					case RENDERING_SHADERS::LIGHT_SHADER:
					{
						lightShader.PrepareForRendering(pDeviceContext);

						lightShader.RenderGeometry(
							pDeviceContext,
							meshData.material,
							viewProj,
							texTransforms,      // textures transformation matrices
							worldMatricesToRender,
							texturesSRVs,
							meshData.indexCount);

						break;
					}
				}
			}
			catch (EngineException& e)
			{
				// if we got here that means we didn't manage to render geometry with some shader
				// so at least try to render it with the color shader
				e;

				colorShader.Render(
					pDeviceContext,
					*meshData.ppVertexBuffer,
					meshData.pIndexBuffer,
					viewProj,
					meshesDataForRender[idx].indexCount,
					totalGameTime);

			}
		}
		
		

#if 0
		// go through each mesh, get rendering data of related entities, 
		// and execute rendering
		for (const : transientData.meshesIDsToRender)
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
#endif
	}
	catch (EngineException& e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't render models onto the scene");
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