////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "RenderGraphics.h"
#include "../Common/MathHelper.h"

#include "../GameObjects/TextureManagerClass.h"



using namespace DirectX;

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

	try
	{
		// compute data for the fire animation
		fireTexAnimData_.Initialize(
			15,            // verticall cells count
			8,             // horizontal cells count
			4.0f);         // duration of animation
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't create an instance of the RenderGraphics class");
	}
}

///////////////////////////////////////////////////////////

void RenderGraphics::Update(
	ID3D11DeviceContext* pDeviceContext,
	EntityManager& entityMgr,
	Shaders::ShadersContainer & shadersContainer,
	LightStore & lightsStore,
	SystemState & sysState,
	UserInterfaceClass& UI,
	const DirectX::XMFLOAT3 & cameraPos,
	const DirectX::XMFLOAT3 & cameraDir,
	const float deltaTime,
	const float totalGameTime)
{
	//
	// Update the scene for this frame
	//


	// every 60 frames we update the UI
	//if (gameCycles % isUpdateGUI == 0)
	//{
		// update user interface for this frame (for the editor window)
	UI.Update(pDeviceContext, sysState);

	entityMgr.Update(deltaTime);

	// set to zero as we haven't rendered models for this frame yet
	sysState.renderedModelsCount = 0;
	sysState.renderedVerticesCount = 0;

	////////////////////////////////////////////////
	//  UPDATE THE LIGHT SOURCES 
	////////////////////////////////////////////////

	// circle light over the land surface
	DirectX::XMFLOAT3 & pointLightPos = lightsStore.pointLightsStore_.pointLightsArr_[0].position;
	pointLightPos.x = 10.0f*cosf(0.2f*totalGameTime);
	pointLightPos.z = 10.0f*sinf(0.2f*totalGameTime);
	pointLightPos.y = 10.0f;

	// the spotlight takes on the camera position and is aimed in the same direction 
	// the camera is looking. In this way, it looks like we are holding a flashlight
	SpotLight & spotLight = lightsStore.spotLightsStore_.spotLightsArr_[0];
	spotLight.position = cameraPos;
	spotLight.direction = cameraDir;

	// update the lights params for this frame
	shadersContainer.lightShader_.SetLights(
		pDeviceContext,
		cameraPos,
		lightsStore.dirLightsStore_.dirLightsArr_,
		lightsStore.pointLightsStore_.pointLightsArr_,
		lightsStore.spotLightsStore_.spotLightsArr_);

	

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

#if 0
	//
	// Animate fire
	//
	const uint32_t fire_idx = modelsStore.GetIndexByTextID("plane");

	fireTexAnimData_.Update(deltaTime, modelsStore.texTransform_[fire_idx]);

	//
	// Animate fireball texture
	//
	const UINT cube_idx = modelsStore.GetIndexByTextID("cube");
	const XMMATRIX cubeTexTrans = 
		DirectX::XMMatrixTranslation(-0.5f, -0.5f, 0.0f) *
		DirectX::XMMatrixRotationZ(totalGameTime) *
		DirectX::XMMatrixTranslation(0.5f, 0.5f, 0.0f);
	modelsStore.texTransform_[cube_idx] = cubeTexTrans;
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
	LightStore & lightsStore,
	UserInterfaceClass & UI,
	FrustumClass & editorFrustum,

	const DirectX::XMMATRIX & WVO,           // world * basic_view * ortho
	const DirectX::XMMATRIX & viewProj,      // view * projection
	const DirectX::XMFLOAT3 & cameraPos,
	const DirectX::XMFLOAT3 & cameraDir,     // the direction where the camera is looking at
	const float deltaTime,                   // time passed since the previous frame
	const float totalGameTime,               // time passed since the start of the application
	const float cameraDepth)                 // how far the camera can see
{
	try
	{
		// RENDER 3D STUFF
		RenderModels(
			pDevice,
			pDeviceContext,
			entityMgr,
			meshStorage,
			editorFrustum,
			shadersContainer.colorShader_,
			shadersContainer.textureShader_,
			shadersContainer.lightShader_,
			systemState,
			lightsStore,
			viewProj,
			cameraPos,
			cameraDir,
			deltaTime,
			totalGameTime,
			cameraDepth);

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
	FrustumClass & editorFrustum,
	ColorShaderClass & colorShader,
	TextureShaderClass & textureShader,
	LightShaderClass & lightShader,
	SystemState & systemState,
	LightStore & lightsStore,
	const DirectX::XMMATRIX & viewProj,     // view * projection
	const DirectX::XMFLOAT3 & cameraPos,
	const DirectX::XMFLOAT3 & cameraDir,    // the direction where the camera is looking at
	const float deltaTime,
	const float totalGameTime,
	const float cameraDepth)
{    
	//
	// this function prepares and renders all the visible models onto the screen
	//

	try
	{
		std::vector<EntityID> visibleEntts;// = entityMgr.GetAllEnttsIDs();
		
		std::vector<DirectX::XMMATRIX> worldMatrices;
		std::vector<RENDERING_SHADERS> shaderTypes;
		std::vector<MeshID> meshesIDsToRender;
		std::vector<std::set<EntityID>> enttsSortedByMeshes;

		// TEMPORARY: currently we don't have any frustum culling so just
		// render all the entitites which have the Rendered component
		entityMgr.renderSystem_.GetEnttsIDsFromRenderedComponent(visibleEntts);


		// ------------------------------------------------------
		// prepare entts data for rendering

		entityMgr.GetRenderingDataOfEntts(
			visibleEntts, 
			worldMatrices,        // world matrix of each visible entt
			shaderTypes,          // of each visible entt
			meshesIDsToRender,    
			enttsSortedByMeshes);


		// ------------------------------------------------------
		// prepare meshes data for rendering

		std::map<aiTextureType, ID3D11ShaderResourceView* const*> texturesSRVs;
		std::vector<Mesh::DataForRendering> meshesDataForRender;

		MeshStorage::Get()->GetMeshesDataForRendering(meshesIDsToRender, meshesDataForRender);

		textureShader.PrepareShaderForRendering(
			pDeviceContext,
			cameraPos);


		// ------------------------------------------------------
		// go through each mesh and rendering it

		for (size_t idx = 0; idx < meshesIDsToRender.size(); ++idx)
		{
			PrepareTexturesSRV_ToRender(meshesDataForRender[idx].textures, texturesSRVs);
			PrepareIAStageForRendering(pDeviceContext, meshesDataForRender[idx], D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// get world matrices of entts related to this mesh
			std::vector<DirectX::XMMATRIX> worldMatricesToRender;
			worldMatricesToRender.reserve(std::ssize(enttsSortedByMeshes[idx]));

			for (const EntityID& enttID : enttsSortedByMeshes[idx])
			{
				const ptrdiff_t dataIdx = std::distance(visibleEntts.begin(), std::upper_bound(visibleEntts.begin(), visibleEntts.end(), enttID)) - 1;
				worldMatricesToRender.emplace_back(worldMatrices[dataIdx]);
			}

			// render the the current mesh
			textureShader.Render(
				pDeviceContext,
				worldMatricesToRender,
				viewProj,
				DirectX::XMMatrixIdentity(),      // textures transformation matrix
				texturesSRVs,
				meshesDataForRender[idx].indexCount);
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
	// prepare input assembler (IA) stage before the rendering process

	const UINT offset = 0;

	pDeviceContext->IASetVertexBuffers(
		0,                                 // start slot
		1,                                 // num buffers
		meshData.ppVertexBuffer,           // ppVertexBuffers
		meshData.pStride,                  // pStrides
		&offset);

	// set what primitive topology we want to use to render this vertex buffer
	pDeviceContext->IASetPrimitiveTopology(topologyType);

	pDeviceContext->IASetIndexBuffer(
		meshData.pIndexBuffer,             // pIndexBuffer
		DXGI_FORMAT::DXGI_FORMAT_R32_UINT, // format of the indices
		0);                                // offset, in bytes

	return;
}



///////////////////////////////////////////////////////////

void RenderGraphics::PrepareTexturesSRV_ToRender(
	const std::unordered_map<aiTextureType, TextureClass*>& texturesMap,
	std::map<aiTextureType, ID3D11ShaderResourceView* const*>& texturesSRVs)
{
	// get a bunch of pointers to shader resource views by input textures map

	for (const auto& texture : texturesMap)
	{
		ID3D11ShaderResourceView* const* ppSRV = texture.second->GetTextureResourceViewAddress();

		// insert pair ['texture_type' => 'texture_SRV']
		texturesSRVs.insert_or_assign(texture.first, ppSRV);
	}
}