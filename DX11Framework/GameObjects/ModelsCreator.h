////////////////////////////////////////////////////////////////////////////////////////////
// Filename:        ModelsCreator.h
// Description:     a functional for mesh/models creation
//                  (plane, cube, sphere, imported models, etc.)
//
// Created:         12.02.24
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>     
#include "MeshHelperTypes.h"
#include "MeshStorage.h"
#include "GeometryGenerator.h"
#include "TextureManagerClass.h"

typedef unsigned int UINT;


class ModelsCreator
{
public:
	//
	// TYPEDEFS
	//
	struct WAVES_PARAMS
	{
		UINT numRows;
		UINT numColumns;
		float spatialStep;
		float timeStep;
		float speed;
		float damping;
	};

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
	ModelsCreator();

	void Initialize(MeshStorage* pMeshStorage);

	const std::vector<MeshID> CreateFromFile(ID3D11Device* pDevice, const std::string filepath);

	const std::string CreatePlane(ID3D11Device* pDevice);
	const std::string CreateCube(ID3D11Device* pDevice);
	const std::string CreateSkullModel(ID3D11Device* pDevice);

	const std::string CreatePyramid(
		ID3D11Device* pDevice,
		const float height,                           // height of the pyramid
		const float baseWidth,                        // width (length by X) of one of the base side
		const float baseDepth);                        // depth (length by Z) of one of the base side

	const std::string CreateCylinder(
		ID3D11Device* pDevice,
		const ModelsCreator::CYLINDER_PARAMS& cylParams);

	

	const std::string CreateWaves(
		ID3D11Device* pDevice,
		const ModelsCreator::WAVES_PARAMS& params);

	const std::string CreateSphere(
		ID3D11Device* pDevice,
		const float radius,
		const UINT sliceCount,
		const UINT stackCount);

	const std::string CreateGeophere(
		ID3D11Device* pDevice,
		const float radius,
		const UINT numSubdivisions);


	const std::string CreateChunkBoundingBox(
		ID3D11Device* pDevice,
		const UINT chunkDimension);

	const std::string CreateGrid(ID3D11Device* pDevice,
		const float gridWidth,
		const float gridDepth);

	const std::string CreateGeneratedTerrain(
		ID3D11Device* pDevice,
		const float terrainWidth,
		const float terrainDepth,
		const UINT cellsCountAlongWidth,
		const UINT cellsCountAlongDepth);

	const std::string CreateTerrainFromFile(
		ID3D11Device* pDevice,
		const std::string & terrainSetupFile);

	const std::string CreateOneCopyOfModelByIndex(
		ID3D11Device* pDevice,
		const UINT index);


private:
	void GenerateHeightsForGrid(Mesh::MeshData & grid);

	void PaintGridWithRainbow(Mesh::MeshData & grid,
		const UINT verticesCountByX,
		const UINT verticesCountByZ);

	void PaintGridAccordingToHeights(Mesh::MeshData & grid);


private:
	GeometryGenerator geoGen;                  // for manual generation of vertices/indices data
	MeshStorage* pMeshStorage_ = nullptr;

	// make a default map of textures for models which will be created
	std::map<aiTextureType, TextureClass*> defaultTexturesMap_;
};