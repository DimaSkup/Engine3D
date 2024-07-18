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
#include "../Common/Types.h"

// ECS components
#include "../Components/Transform.h"
#include "../Components/WorldMatrix.h"
#include "../Components/Rendered.h"
#include "../Components/MeshComponent.h"


class RenderSystem final
{
public:
	RenderSystem(
		Rendered* pRenderComponent,
		Transform* pTransformComponent,
		WorldMatrix* pWorldMatrixComponent,
		MeshComponent* pMeshComponent);
	~RenderSystem() {}

	void Serialize(std::ofstream& fout, u32& offset);
	void Deserialize(std::ifstream& fin, const u32 offset);

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