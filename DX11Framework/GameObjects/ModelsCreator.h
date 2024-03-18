////////////////////////////////////////////////////////////////////////////////////////////
// Filename:        ModelsCreator.h
// Description:     a functional for models creation
//                  (plane, cube, imported models, etc.)
//
// Created:         12.02.24
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>     
#include <windows.h>  // for using UINT type
#include "ModelsStore.h"
#include "GeometryGenerator.h"

class ModelsCreator
{
public:
	const UINT CreatePlane(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification = DirectX::XMVectorZero(),  // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification = DirectX::XMVectorZero()); // rotation modification; if we don't set this param the model won't rotate


	const UINT CreateCube(ID3D11Device* pDevice, 
		ModelsStore & modelsStore,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification = DirectX::XMVectorZero(),  // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification = DirectX::XMVectorZero()); // rotation modification; if we don't set this param the model won't rotate

	const UINT CreateSphere(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification = DirectX::XMVectorZero(),  // position modification; if we don't set this param the model won't move
		const DirectX::XMVECTOR & inRotModification = DirectX::XMVectorZero()); // rotation modification; if we don't set this param the model won't rotate

	const UINT CreateCylinder(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification,  // position modification; 
		const DirectX::XMVECTOR & inRotModification,  // rotation modification; 
		const float bottomRadius,
		const float topRadius,
		const float height,
		const UINT sliceCount,
		const UINT stackCount);


	const UINT CreateGeneratedTerrain(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const float terrainWidth,
		const float terrainDepth,
		const UINT cellsCountAlongWidth,
		const UINT cellsCountAlongDepth);

	const UINT CreateTerrainFromFile(const std::string & terrainSetupFile,
		ID3D11Device* pDevice,
		ModelsStore & modelsStore);

	const UINT CreateCopyOfModelByIndex(const UINT index,
		ModelsStore & modelsStore,
		ID3D11Device* pDevice);


private:
	void GenerateHeightsForGrid(GeometryGenerator::MeshData & grid);

	void PaintGridWithRainbow(GeometryGenerator::MeshData & grid,
		const UINT verticesCountByX,
		const UINT verticesCountByZ);

	void PaintGridAccordingToHeights(GeometryGenerator::MeshData & grid);
};