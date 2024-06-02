// *********************************************************************************
// Filename:     RenderSystem.cpp
// Description:  implementation of the RenderSystem's functional
// 
// Created:      21.05.24
// *********************************************************************************
#include "RenderSystem.h"



// ************************************************************************************
//               Helper structs to store parts of the transient data
// ************************************************************************************

struct TransientDataForRendering
{
	// stores one frame transient data;
	// 
	// this is intermediate data used for rendering 
	// multiple entities which have the same mesh;

	std::vector<DirectX::XMMATRIX> matricesForRendering;
	std::map<aiTextureType, ID3D11ShaderResourceView* const*> texturesSRVs;

	void Clear()
	{
		matricesForRendering.clear();
		texturesSRVs.clear();
	}
};





// *********************************************************************************
//                            PUBLIC FUNCTIONS
// *********************************************************************************

void RenderSystem::Render(
	ID3D11DeviceContext* pDeviceContext,
	Rendered& renderComponent,
	Transform& transformComponent,
	MeshComponent& meshComponent,
	MeshStorage& meshStorage,
	ColorShaderClass& colorShader,
	TextureShaderClass& textureShader,
	LightShaderClass& lightShader,
	const std::vector<DirectionalLight>& dirLights,
	const std::vector<PointLight>& pointLights,
	const std::vector<SpotLight>& spotLights,
	const DirectX::XMFLOAT3& cameraPos,
	const DirectX::XMMATRIX& viewProj)
{

	TransientDataForRendering transientData;

	// update the lights params for this frame
	lightShader.SetLights(
		pDeviceContext,
		cameraPos,
		dirLights,
		pointLights,
		spotLights);

	// go through each mesh and render it
	for (const auto& meshToEntities : meshComponent.GetMeshToEntitiesRecords())
	{
		const std::set<EntityID> entitiesSet = meshToEntities.second;
		const Mesh::MeshDataForRendering meshData = meshStorage.GetMeshDataForRendering(meshToEntities.first);  // get all the necessary data of the mesh for rendering

		PrepareIAStageForRendering(pDeviceContext, meshData);
		GetWorldMatricesOfEntities(transformComponent, entitiesSet, transientData.matricesForRendering);

		// RENDER GEOMETRY
		switch (meshData.renderingShaderType)
		{
			case Mesh::RENDERING_SHADERS::COLOR_SHADER:
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
			case Mesh::RENDERING_SHADERS::TEXTURE_SHADER:
			{

				// if we want to render textured object we have to get its textures
				PrepareTexturesSRV_ToRender(
					meshStorage.textures_.at(meshData.dataIdx), 
					transientData.texturesSRVs);

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
			case Mesh::RENDERING_SHADERS::LIGHT_SHADER:
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

		// since we've rendered this mesh we have to clear transient data
		transientData.Clear();
	}
}




// *********************************************************************************
//                           PRIVATE FUNCTIONS
// *********************************************************************************

void RenderSystem::PrepareIAStageForRendering(
	ID3D11DeviceContext* pDeviceContext,
	const Mesh::MeshDataForRendering& meshData)
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
	pDeviceContext->IASetPrimitiveTopology(meshData.topologyType);


	pDeviceContext->IASetIndexBuffer(
		meshData.pIndexBuffer,             // pIndexBuffer
		DXGI_FORMAT::DXGI_FORMAT_R32_UINT, // format of the indices
		0);                                // offset, in bytes

	return;
}

///////////////////////////////////////////////////////////

void RenderSystem::PrepareTexturesSRV_ToRender(
	const std::map<aiTextureType, TextureClass*>& texturesMap,
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