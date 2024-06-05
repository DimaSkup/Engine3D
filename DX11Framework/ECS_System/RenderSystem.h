// *********************************************************************************
// Filename:     RenderSystem.h
// Description:  an ECS system for execution of entities rendering
// 
// Created:      21.05.24
// *********************************************************************************
#pragma once

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

class RenderSystem
{
public:
	RenderSystem(
		Rendered* pRenderComponent,
		Transform* pTransformComponent,
		MeshComponent* pMeshComponent)
	{
		ASSERT_NOT_NULLPTR(pRenderComponent, "ptr to the Rendered component == nullptr");
		ASSERT_NOT_NULLPTR(pTransformComponent, "ptr to the Transform component == nullptr");
		ASSERT_NOT_NULLPTR(pMeshComponent, "ptr to the Mesh component == nullptr");

		pRenderComponent_ = pRenderComponent;
		pTransformComponent_ = pTransformComponent;
		pMeshComponent_ = pMeshComponent;
	}

	void AddRecord(const EntityID& entityID);
	void RemoveRecord(const EntityID& entityID);

	void Render(
		ID3D11DeviceContext* pDeviceContext,
		MeshStorage& meshStorage,
		ColorShaderClass& colorShader,
		TextureShaderClass& textureShader,
		LightShaderClass& lightShader,
		const std::vector<DirectionalLight>& dirLights,
		const std::vector<PointLight>& pointLights,
		const std::vector<SpotLight>& spotLights,
		const DirectX::XMFLOAT3& cameraPos,
		const DirectX::XMMATRIX& viewProj);

	// for debug/unit-test purposes
	std::set<EntityID> GetEntitiesIDsSet() const;
	
private:
	void PrepareIAStageForRendering(
		ID3D11DeviceContext* pDeviceContext,
		const Mesh::MeshDataForRendering& meshData);

	void GetWorldMatricesOfEntities(
		const std::set<EntityID>& entityIDs,
		std::vector<DirectX::XMMATRIX>& outWorldMatrices);

	void PrepareTexturesSRV_ToRender(
		const std::map<aiTextureType, TextureClass*>& texturesMap,
		std::map<aiTextureType, ID3D11ShaderResourceView* const*>& texturesSRVs);

private:
	Rendered* pRenderComponent_ = nullptr;
	Transform* pTransformComponent_ = nullptr;
	MeshComponent* pMeshComponent_ = nullptr;
};