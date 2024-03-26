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
	struct CYLINDER_PARAMS
	{
		float height;
		float bottomRadius;
		float topRadius;
		UINT sliceCount;
		UINT stackCount;
	};

	struct SPHERE_PARAMS
	{
		float radius;
		UINT sliceCount;
		UINT stackCount;
	};

	struct GEOSPHERE_PARAMS
	{
		float radius;
		UINT numSubdivisions;
	};

	struct PYRAMID_PARAMS
	{
		float height;     
		float baseWidth;  // size of pyramid base by X
		float baseDepth;  // size of pyramid base by Z
	};

public:
	void LoadParamsForDefaultModels(
		Settings & settings,
		ModelsCreator::CYLINDER_PARAMS & cylParams,
		ModelsCreator::SPHERE_PARAMS & sphereParams,
		ModelsCreator::GEOSPHERE_PARAMS & geosphereParams,
		ModelsCreator::PYRAMID_PARAMS & pyramidParams);

	const UINT CreatePlane(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification,  // position modification
		const DirectX::XMVECTOR & inRotModification); // rotation modification


	const UINT CreateCube(ID3D11Device* pDevice, 
		ModelsStore & modelsStore,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification,  // position modification
		const DirectX::XMVECTOR & inRotModification); // rotation modification

	const UINT CreatePyramid(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const float height,                           // height of the pyramid
		const float baseWidth,                        // width (length by X) of one of the base side
		const float baseDepth,                        // depth (length by Z) of one of the base side
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification,  // position modification
		const DirectX::XMVECTOR & inRotModification); // rotation modification

	const UINT CreateSphere(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const float radius,
		const UINT sliceCount,
		const UINT stackCount,
		const DirectX::XMVECTOR & inPosition,
		const DirectX::XMVECTOR & inDirection,
		const DirectX::XMVECTOR & inPosModification,  // position modification
		const DirectX::XMVECTOR & inRotModification); // rotation modification

	const UINT CreateGeophere(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const float radius,
		const UINT numSubdivisions);

	const UINT CreateCylinder(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const ModelsCreator::CYLINDER_PARAMS & cylParams,
		const DirectX::XMVECTOR & inPosition = DirectX::XMVectorZero(),
		const DirectX::XMVECTOR & inDirection = DirectX::XMVectorZero(),
		const DirectX::XMVECTOR & inPosModification = DirectX::XMVectorZero(),   // position modification 
		const DirectX::XMVECTOR & inRotModification = DirectX::XMVectorZero());  // rotation modification

	const UINT CreateChunkBoundingBox(
		const UINT chunkDimension,
		ModelsStore & modelsStore,
		ID3D11Device* pDevice);

	const UINT CreateGrid(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const float gridWidth,
		const float gridDepth,
		const DirectX::XMVECTOR & inPosition = DirectX::XMVectorZero(),          // initial position
		const DirectX::XMVECTOR & inDirection = DirectX::XMVectorZero(),
		const DirectX::XMVECTOR & inPosModification = DirectX::XMVectorZero(),   // position modification 
		const DirectX::XMVECTOR & inRotModification = DirectX::XMVectorZero());

	const UINT CreateGeneratedTerrain(ID3D11Device* pDevice,
		ModelsStore & modelsStore,
		const float terrainWidth,
		const float terrainDepth,
		const UINT cellsCountAlongWidth,
		const UINT cellsCountAlongDepth);

	const UINT CreateTerrainFromFile(const std::string & terrainSetupFile,
		ID3D11Device* pDevice,
		ModelsStore & modelsStore);

	const UINT CreateOneCopyOfModelByIndex(const UINT index,
		ModelsStore & modelsStore,
		ID3D11Device* pDevice);


private:
	void GenerateHeightsForGrid(GeometryGenerator::MeshData & grid);

	void PaintGridWithRainbow(GeometryGenerator::MeshData & grid,
		const UINT verticesCountByX,
		const UINT verticesCountByZ);

	void PaintGridAccordingToHeights(GeometryGenerator::MeshData & grid);
};