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
#include <vector>
#include "../ECS_Common/ECS_Types.h"

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

	void Serialize(std::ofstream& fout, size_t& offset);
	void Deserialize(std::ifstream& fin, const size_t offset);

	void AddRecords(
		const std::vector<EntityID>& enttsIDs,
		const std::vector<ECS::RENDERING_SHADERS>& shaderTypes,
		const std::vector<D3D11_PRIMITIVE_TOPOLOGY>& primTopology);

	void RemoveRecords(const std::vector<EntityID>& enttsIDs);

	void GetRenderingDataOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<ECS::RENDERING_SHADERS>& outShaderTypes);

	// for debug/unit-test purposes
	void GetEnttsIDsFromRenderedComponent(std::vector<EntityID>& outEnttsIDs);

private:
	void GetShaderTypesOfEntts(
		const std::vector<EntityID>& enttsIDs,
		std::vector<ECS::RENDERING_SHADERS>& outShaderTypes);

private:
	Rendered* pRenderComponent_ = nullptr;
	Transform* pTransformComponent_ = nullptr;
	WorldMatrix* pWorldMatComponent_ = nullptr;
	MeshComponent* pMeshComponent_ = nullptr;
};