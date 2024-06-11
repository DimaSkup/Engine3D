// *********************************************************************************
// Filename:     RenderSystem.h
// Description:  an ECS system for execution of entities rendering
// 
// Created:      21.05.24
// *********************************************************************************
#pragma once

#include <d3d11.h>
#include <unordered_map>
#include <unordered_set>

// ECS components
#include "../ECS_Components/Transform.h"
#include "../ECS_Components/WorldMatrix.h"
#include "../ECS_Components/Rendered.h"
#include "../ECS_Components/MeshComponent.h"


class RenderSystem final
{
public:
	RenderSystem(
		Rendered* pRenderComponent,
		Transform* pTransformComponent,
		WorldMatrix* pWorldMatrixComponent,
		MeshComponent* pMeshComponent);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<RENDERING_SHADERS>& shaderTypes,
		const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& primTopology);

	void RemoveRecords(const std::vector<EntityID>& enttsIDs);

	void GetRenderingDataOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<XMMATRIX>& outWorldMatrices,
		std::vector<RENDERING_SHADERS>& outShaderTypes);

	// for debug/unit-test purposes
	void GetEnttsIDsFromRenderedComponent(std::vector<EntityID>& outEnttsIDs);
	
private:
	void GetWorldMatricesOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<XMMATRIX>& outWorldMatrices);

	void GetMeshesRelatedToEntts(
		const std::vector<EntityID>& enttsToRender,
		std::unordered_set<MeshID>& outMeshesIDsToRender);

private:
	Rendered* pRenderComponent_ = nullptr;
	Transform* pTransformComponent_ = nullptr;
	WorldMatrix* pWorldMatComponent_ = nullptr;
	MeshComponent* pMeshComponent_ = nullptr;
};