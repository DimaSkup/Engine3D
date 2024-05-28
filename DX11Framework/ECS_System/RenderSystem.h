// *********************************************************************************
// Filename:     RenderSystem.h
// Description:  an ECS system for execution of entities rendering
// 
// Created:      21.05.24
// *********************************************************************************
#pragma once

#include "BaseSystem.h"

#include <d3d11.h>

#include "../ECS_Components/Transform.h"
#include "../ECS_Components/Rendered.h"
#include "../ECS_Components/MeshComponent.h"

#include "../GameObjects/MeshStorage.h"
#include "../GameObjects/VertexBuffer.h"
#include "../GameObjects/IndexBuffer.h"

#include "../EffectsAndShaders/colorshaderclass.h"
#include "../EffectsAndShaders/textureshaderclass.h"
#include "../EffectsAndShaders/LightShaderClass.h"

typedef unsigned int UINT;

class RenderSystem : public BaseSystem
{
public:
	RenderSystem() : BaseSystem("RenderSystem") {}

	void AddForRendering(
		const EntityID& entityID,
		Rendered& renderedComponent)
	{
		// add an entity for rendering
		renderedComponent.AddRecord(entityID);
	}

	void Render(
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
		const DirectX::XMMATRIX& viewProj);
	
private:
	void PrepareIAStageForRendering(
		ID3D11DeviceContext* pDeviceContext,
		const Mesh::MeshDataForRendering& meshData);

	void GetWorldMatricesOfEntities(
		const Transform& transformComponent,
		const std::set<EntityID>& entityIDs,
		std::vector<DirectX::XMMATRIX>& outWorldMatrices)
	{
		// go through each entity and get its world matrix
		for (const EntityID& entityID : entityIDs)
			outWorldMatrices.push_back(transformComponent.entityToData_.at(entityID).world_);
	}

	void PrepareTexturesSRV_ToRender(
		const std::map<aiTextureType, TextureClass*>& texturesMap,
		std::map<aiTextureType, ID3D11ShaderResourceView* const*>& texturesSRVs);
};