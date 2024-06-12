//////////////////////////////////////////////////////////////////////////////////////////
// Filename:     InitializeGraphicsHelper.h
// Description:  contains diffurent helpers which are used during initialization of graphics;
//               these functions are used in the InitializeGraphics.cpp file
//
// Created:      02.04.24
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once

typedef unsigned int UINT;

#include <vector>
#include <DirectXMath.h>
#include <d3d11.h>

#include "../GameObjects/GeometryGenerator.h"
#include "../GameObjects/ModelsCreator.h"
#include "../GameObjects/TextureManagerClass.h"
#include "../Common/MathHelper.h"

#include "InitGraphicsHelperDataTypes.h"
#include "../GameObjects/MeshHelperTypes.h"

using namespace DirectX;


void CreateNanoSuit(
	ID3D11Device* pDevice,
	EntityManager& entityMgr,
	MeshStorage& meshStorage)
{
	const std::string entitiesName = "nanosuit";
	const std::string filepath = "data/models/nanosuit/nanosuit.obj";
	/*
	// load all the meshes of the model from the data file
	const std::vector<MeshID> meshesIDs = modelsCreator.ImportFromFile(pDevice, filepath);

	// set shader for each mesh
	for (const MeshID& meshID : meshesIDs)
		meshStorage.SetRenderingShaderForMeshByID(meshID, Mesh::RENDERING_SHADERS::LIGHT_SHADER);

	TransformData transformData;
	transformData.positions.push_back({ 10, -3, 0 });
	transformData.directions.push_back({ 0, DirectX::XM_PIDIV2, 0 });
	transformData.scales.push_back({ 1, 1, 1 });

	// create an entity and setup it for rendering
	entityMgr.CreateEntities({ entitiesName });
	entityMgr.AddTransformComponent({ entitiesName }, transformData.positions, transformData.directions, transformData.scales);
	entityMgr.AddMeshComponents({ entitiesName }, meshesIDs);
	entityMgr.AddRenderingComponent({ entitiesName });
	*/
}


// ********************************************************************************
//                    ENTITIES: PREPARE TRANSFORM DATA
// ********************************************************************************

void PrepareTransformDataForCylinders(std::map<std::string, TransformData>& transformData)
{
	// prepare transform data for cylinders entities

	transformData.insert_or_assign("cylinder", TransformData());
	TransformData& data = transformData["cylinder"];

	// create positions for 2 rows of 5 cylinders by each row
	const UINT cylPerRow = 5;
	UINT columnIdx = 0;
	for (UINT i = 0; i < cylPerRow; ++i)
	{
		data.positions.push_back({ -5.0f, 0.0f, -10.0f + columnIdx * 5.0f });
		data.positions.push_back({ +5.0f, 0.0f, -10.0f + columnIdx * 5.0f });
		++columnIdx;
	}

	// set directions and scales to default
	data.directions.resize(data.positions.size(), { 0,0,0 });
	data.scales.resize(data.positions.size(), { 1,1,1 });

	// in the end we have to check if data arrays have equal size
	const UINT cylindersCount = (UINT)data.positions.size();
	ASSERT_TRUE(cylindersCount == (UINT)data.directions.size(), "positions arr size != directions arr size");
	ASSERT_TRUE(cylindersCount == (UINT)data.scales.size(), "positions arr size != scales arr size");
}

///////////////////////////////////////////////////////////

void PrepareTransformDataForSpheres(std::map<std::string, TransformData>& transformData)
{
	// prepare transform data for spheres entities

	transformData.insert_or_assign("sphere", TransformData());
	TransformData& data = transformData["sphere"];

	// create positions for 2 rows of 5 spheres by each row
	const UINT spheresPerRow = 5;
	UINT columnIdx = 0;
	for (UINT i = 0; i < spheresPerRow; ++i)
	{
		data.positions.push_back({ -5.0f, 3.5f, -10.0f + columnIdx * 5.0f });
		data.positions.push_back({ +5.0f, 3.5f, -10.0f + columnIdx * 5.0f });
		++columnIdx;
	}

	// set directions and scales to default
	data.directions.resize(data.positions.size(), { 0,0,0 });
	data.scales.resize(data.positions.size(), { 1,1,1 });

	// add a central sphere
	data.positions.push_back({ 0,10.0f,0 });
	data.directions.push_back({ 0,0,0 });
	data.scales.push_back({ 3,3,3 });

	// in the end we have to check if data arrays have equal size
	const UINT spheresCount = (UINT)data.positions.size();
	ASSERT_TRUE(spheresCount == (UINT)data.directions.size(), "positions arr size != directions arr size");
	ASSERT_TRUE(spheresCount == (UINT)data.scales.size(), "positions arr size != scales arr size");
}

///////////////////////////////////////////////////////////

void PrepareTransformDataForPyramids(std::map<std::string, TransformData>& transformData)
{
	// prepare transform data for pyramids entities

	transformData.insert_or_assign("pyramid", TransformData());
	TransformData& data = transformData["pyramid"];

	// add a pyramid at the center
	data.positions.push_back({ 0,-2,0 });
	data.directions.push_back({ 0,0,0 });
	data.scales.push_back({ 1,1,1 });

	// in the end we have to check if data arrays have equal size
	const UINT pyramidsCount = (UINT)data.positions.size();
	ASSERT_TRUE(pyramidsCount == (UINT)data.directions.size(), "positions arr size != directions arr size");
	ASSERT_TRUE(pyramidsCount == (UINT)data.scales.size(), "positions arr size != scales arr size");
}

///////////////////////////////////////////////////////////

void PrepareTransformDataForEntities(std::map<std::string, TransformData>& transformData)
{
	PrepareTransformDataForCylinders(transformData);
	PrepareTransformDataForSpheres(transformData);
	PrepareTransformDataForPyramids(transformData);
}


// ********************************************************************************
//                    ENTITIES: PREPARE MOVEMENT DATA
// ********************************************************************************

void PrepareMovementDataForCylinders(
	const size_t entitiesCount,             // cylinders count
	std::map<std::string, MovementData>& modelsMovementData)
{
	modelsMovementData.insert_or_assign("cylinder", MovementData());
	MovementData& data = modelsMovementData["cylinder"];

	const DirectX::XMFLOAT3 noTranslation{ 0,0,0 };
	const DirectX::XMFLOAT4 noRotationChange{ 0,0,0,0 };
	const DirectX::XMFLOAT3 noScaleChange{ 1,1,1 };

	// allocate memory for data and set it to default values
	data.translations.resize(entitiesCount, noTranslation);
	data.rotQuats.resize(entitiesCount, noRotationChange);
	data.scaleChanges.resize(entitiesCount, noScaleChange);
}

///////////////////////////////////////////////////////////

void PrepareMovementDataForSpheres(
	const size_t entitiesCount,             // spheres count
	std::map<std::string, MovementData>& movementData)
{
	movementData.insert_or_assign("sphere", MovementData());
	MovementData& data = movementData["sphere"];

	// default movement values
	const float rotationSpeed = DirectX::XM_PI * 0.001f;
	const DirectX::XMFLOAT3 noTranslation{ 0,0,0 };
	const DirectX::XMFLOAT4 noRotationChange{ 0,0,0,0 };
	const DirectX::XMFLOAT3 noScaleChange{ 1,1,1 };

	// allocate memory for data and set it to default values
	std::vector<DirectX::XMVECTOR> rotationQuatVecArr(entitiesCount, DirectX::XMVectorZero());
	data.translations.resize(entitiesCount, noTranslation);
	data.rotQuats.resize(entitiesCount, noRotationChange);
	data.scaleChanges.resize(entitiesCount, noScaleChange);

	// the central sphere moves down and is getting smaller
	//data.translations.back() = { 0, -1.0f, 0 };
	//data.scaleChanges.back() = { .9999f, .9999f, .9999f };

	// create rotation quaternions (we do rotation around itself but in random direction)
	for (DirectX::XMVECTOR& rotQuat : rotationQuatVecArr)
	{
		const DirectX::XMVECTOR randAxis{ MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF() };
		rotQuat = DirectX::XMQuaternionRotationAxis(randAxis, rotationSpeed);
	}

	// convert rotations quaternions from XMVECTOR into XMFLOAT4
	UINT data_idx = 0;
	for (const XMVECTOR& quat : rotationQuatVecArr)
		DirectX::XMStoreFloat4(&data.rotQuats[data_idx++], quat);

	// in the end we have to check if data arrays have equal size
	const size_t size = data.translations.size();
	ASSERT_TRUE(size == data.rotQuats.size(), "translations arr size != rotation quats arr size");
	ASSERT_TRUE(size == data.scaleChanges.size(), "translations arr size != scales arr size");
}

///////////////////////////////////////////////////////////

void PrepareMovementDataForPyramids(
	const size_t entitiesCount,             // pyramids count
	std::map<std::string, MovementData>& modelsMovementData)
{
}

///////////////////////////////////////////////////////////

void PrepareMovementDataForEntities(
	std::map<std::string, TransformData>& transformData,
	std::map<std::string, MovementData>& movementData)
{
	//PrepareMovementDataForCylinders(modelsTransformData.at("cylinder"));
	PrepareMovementDataForSpheres(transformData.at("spheres").positions.size(), movementData);
	//PrepareMovementDataForPyramids(modelsTransformData.at("pyramid"));
}


// ********************************************************************************
//                             MAIN HELPERS
// ********************************************************************************






#if 0

///////////////////////////////////////////////////////////

void GeneratePositionsRotations_ForCubes_LikeItIsATerrain_InMinecraft(
	const UINT numOfModels,
	_Inout_ std::vector<DirectX::XMVECTOR>& outPositions,
	_Inout_ std::vector<DirectX::XMVECTOR>& outRotations)
{
	// GENERATE positions and rotations data for the cubes 
	// to make it like a terrain in minecraft

	assert(numOfModels > 0);

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData;

	// define the width and depth of the cubes terrain
	const float width = sqrtf((float)numOfModels);
	const float depth = width;

	// generate a basic grid which will be used for cubes placement
	geoGen.GenerateFlatGridMesh(
		width,
		depth,
		(UINT)width + 1,  // +1 because we want to place each new cube right after the previous, without gaps
		(UINT)depth + 1,
		meshData);

	// allocate memory for position/rotation data for each cube
	outPositions.resize(numOfModels);
	outRotations.resize(numOfModels);

	// generate height for each vertex and set color for it according to its height;
	// and fill in the input array of positions

	for (size_t i = 0; i < numOfModels; ++i)
	{
		DirectX::XMFLOAT3& pos = meshData.vertices[i].position;

		// use a function for making hills for the terrain
		pos.y = floorf(0.3f * (0.3f * (pos.z * sinf(0.1f * pos.x)) + (pos.x * cosf(0.1f * pos.z))));

		// store the position and rotation for this cube
		outPositions[i] = XMLoadFloat3(&pos);
		outRotations[i] = XMVectorZero();
	}
}

///////////////////////////////////////////////////////////

void GenerateRandom_PositionsRotations_ForCubes(
	const UINT numOfModels,
	_Inout_ std::vector<DirectX::XMVECTOR>& outPositions,
	_Inout_ std::vector<DirectX::XMVECTOR>& outRotations)
{
	assert(numOfModels > 0);

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData;

	// define the width and depth of the cubes terrain
	const float width = sqrtf((float)numOfModels);
	const float depth = width;

	// generate a basic grid which will be used for cubes placement
	geoGen.GenerateFlatGridMesh(
		width,
		depth,
		(UINT)width + 1,  // +1 because we want to place each new cube right after the previous, without gaps
		(UINT)depth + 1,
		meshData);

	// allocate memory for position/rotation data for each cube
	outPositions.resize(numOfModels);
	outRotations.resize(numOfModels);

	for (size_t i = 0; i < numOfModels; ++i)
	{
		DirectX::XMFLOAT3& pos = meshData.vertices[i].position;

		// setup cubes pos by Y-axis
		pos.y = -1.0f;

		// store the position and rotation for this cube
		outPositions[i] = XMLoadFloat3(&pos);
		outRotations[i] = XMVectorZero();
	}
}

///////////////////////////////////////////////////////////

void CreateTerrainFromFile(
	ID3D11Device* pDevice,
	EntityStore& modelsStore,
	ModelsCreator& modelsCreator,
	const std::string& terrainSetupFile)
{
	// CREATE A TERRAIN GRID FROM FILE

	const UINT terrainGridIdx = modelsCreator.CreateTerrainFromFile(
		terrainSetupFile,
		pDevice,
		modelsStore);

	// get an index of the terrain grid vertex buffer and set a rendering shader for it
	const UINT terrainGrid_vb_idx = modelsStore.relationsModelsToVB_[terrainGridIdx];
	modelsStore.SetRenderingShaderForVertexBufferByIdx(terrainGrid_vb_idx, EntityStore::LIGHT_SHADER);

	// set textures for this terrain grid
	modelsStore.SetTexturesForVB_ByIdx(
		terrainGrid_vb_idx,                      // index of the vertex buffer
		{
			{aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey("data/textures/dirt01d.dds")},
		});
}

//////////////////////////////////////////////////////////

void CreateGeneratedTerrain(
	ID3D11Device* pDevice,
	EntityStore& modelsStore,
	ModelsCreator& modelsCreator,
	Settings& settings,
	const EntityStore::RENDERING_SHADERS& renderingShaderType)
{
	// MANUALLY GENERATE A TERRAIN GRID

	// terrain size by X-axis and Z-axis
	const UINT terrainWidth = settings.GetSettingIntByKey("TERRAIN_WIDTH");
	const UINT terrainDepth = settings.GetSettingIntByKey("TERRAIN_DEPTH");

	// generate a terrain model based on the setup params and get its index
	const UINT terrainGrid_idx = modelsCreator.CreateGeneratedTerrain(pDevice,
		modelsStore,
		(float)terrainWidth,
		(float)terrainDepth,
		terrainWidth + 1,
		terrainDepth + 1);

	// get index of the terrain vertex buffer
	const UINT vb_idx = modelsStore.GetRelatedVertexBufferByModelIdx(terrainGrid_idx);

	// setup the rendering shader for the terrain
	modelsStore.SetRenderingShaderForVertexBufferByIdx(vb_idx, renderingShaderType);


	// set textures for this terrain grid
	modelsStore.SetTexturesForVB_ByIdx(
		vb_idx,                      // index of the vertex buffer
		{
			{aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey("data/textures/grass2.dds")},
		});

	// set terrain material (material varies per object)
	Material& mat = modelsStore.materials_[terrainGrid_idx];

	/*
		const float red = 1.0f;
		const float green = 175.0f / 255.0f;
		const float blue = 69.0f / 255.0f;
	*/
	const float red = 219.0f / 255.0f;
	const float green = 175.0f / 255.0f;
	const float blue = 160.0f / 255.0f;

	mat.ambient = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.diffuse = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
}

///////////////////////////////////////////////////////////

void CreateWaves(ID3D11Device* pDevice,
	EntityStore & modelsStore,
	ModelsCreator & modelsCreator,
	const ModelsCreator::WavesMeshParams & wavesParams,
	const EntityStore::RENDERING_SHADERS renderingShaderType)
{

	// --------------------------------------------------- //
	//                  CREATE WAVES
	// --------------------------------------------------- //

	const UINT wavesIdx = modelsCreator.CreateWaves(
		pDevice,
		modelsStore,
		wavesParams,
		{ 0,-2,0,1 },               // init position
		{ 0, 0,0,0 },               // init rotation
		DirectX::XMVectorZero(),    // by default no position/rotation modification
		DirectX::XMVectorZero());  


	// --------------------------------------------------- //
	//                  SETUP WAVES
	// --------------------------------------------------- //

	// define paths to textures
	const std::string diffuseMapPath{ "data/textures/water2.dds" };

	// get an index of the waves' vertex buffer
	const UINT waves_vb_idx = modelsStore.GetRelatedVertexBufferByModelIdx(wavesIdx);

	// setup rendering shader for the vertex buffer
	modelsStore.SetRenderingShaderForVertexBufferByIdx(waves_vb_idx, renderingShaderType);

	// set textures for the model
	modelsStore.SetTexturesForVB_ByIdx(
		waves_vb_idx,                      // index of the vertex buffer
		{
			{aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey(diffuseMapPath)},
		});

	// set WAVES MATERIAL (material varies per object)
	Material & mat = modelsStore.materials_[wavesIdx];

	mat.ambient  = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat.diffuse  = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
}

///////////////////////////////////////////////////////////

void ComputePositionsForEditorGrid(
	Settings & settings,
	_Out_ float & cellWidth,
	_Out_ float & cellDepth,
	_Out_ UINT & cellsVertexCountByX,
	_Out_ UINT & cellsVertexCountByZ,
	_Out_ UINT & cellsCount,
	_Out_ std::vector<DirectX::XMVECTOR> & outGridPositions)
{
	const float fullWidth = settings.GetSettingFloatByKey("EDITOR_GRID_DIMENSION");      // size of the editor grid by X and Z
	cellWidth = settings.GetSettingFloatByKey("EDITOR_GRID_CELL_DIMENSION"); // size of the editor's grid cell by X and Z
	cellDepth = cellWidth;

	cellsVertexCountByX = static_cast<UINT>(cellWidth) + 1;
	cellsVertexCountByZ = cellsVertexCountByX;

	const float halfWidth = 0.5f * fullWidth;
	const float halfDepth = halfWidth;
	
	const UINT posCountInLine = static_cast<UINT>(fullWidth / cellWidth);
	cellsCount = posCountInLine * posCountInLine;

	// compute positions for each cell of the editor grid
	for (UINT i = 0; i < posCountInLine; ++i)
	{
		const float posZ = halfDepth - i * cellDepth;

		for (UINT j = 0; j < posCountInLine; ++j)
		{
			const float posX = -halfWidth + j * cellWidth;

			// store a 3D position of the cell
			outGridPositions.push_back({ posX, 0.0f, posZ });
		}
	}
}

void CreateEditorGrid(ID3D11Device* pDevice,
	Settings & settings,
	ModelsCreator & modelsCreator,
	EntityStore & modelsStore)
{
	//
	// CREATE EDITOR GRID
	//
	GeometryGenerator geoGen;

	std::vector<DirectX::XMVECTOR> gridPositions;
	float cellWidth = 0;
	float cellDepth = 0;
	UINT cellsVertexCountByX = 0;
	UINT cellsVertexCountByZ = 0;
	UINT cellsCount = 0;

	ComputePositionsForEditorGrid(settings,
		cellWidth, 
		cellDepth, 
		cellsVertexCountByX, 
		cellsVertexCountByZ,
		cellsCount,
		gridPositions);

	// -------------------------------------------------- //

	GeometryGenerator::MeshData editorGridMesh;

	// generate mesh data for the editor's grid cell
	geoGen.GenerateFlatGridMesh(
		cellWidth,
		cellDepth,
		cellsVertexCountByX,
		cellsVertexCountByZ,
		editorGridMesh);

	// make a map of textures for this model
	std::map<aiTextureType, TextureClass*> texturesMap
	{
		{ aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") }
	};

	// create a cell model of the editor grid
	const UINT originEditorGridCellIdx = modelsStore.CreateNewModelWithRawData(pDevice,
		"editor_grid_cell",
		editorGridMesh.vertices,
		editorGridMesh.indices,
		texturesMap);

	// set that we want to render the editor grid using topology linelist
	modelsStore.SetPrimitiveTopologyForVertexBufferByIdx(modelsStore.GetRelatedVertexBufferByModelIdx(originEditorGridCellIdx), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
	// create copies of the origin grid cell (-1 because we've already create one cell)
	const std::vector<UINT> idxsOfCopiedCells = modelsStore.CreateBunchCopiesOfModelByIndex(originEditorGridCellIdx, cellsCount - 1);

	// setup a position for each cell of the editor grid 
	std::copy(gridPositions.begin(), gridPositions.end(), modelsStore.positions_.begin() + originEditorGridCellIdx);

	gridPositions.clear();
}

///////////////////////////////////////////////////////////

void CreateAxis(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	EntityStore & modelsStore)
{

	// generate data for the axis
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData axisMeshData;
	geoGen.GenerateAxisMesh(axisMeshData);

	// make a map of textures for this model
	std::map<aiTextureType, TextureClass*> texturesMap
	{
		{ aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") }
	};

	// create an axis model
	const uint32_t axisModelIdx = modelsStore.CreateNewModelWithRawData(pDevice,
		"axis",
		axisMeshData.vertices,
		axisMeshData.indices,
		texturesMap);

	modelsStore.SetPositionsForModelsByIdxs(
		{ axisModelIdx }, 
		{ DirectX::XMVectorZero() });

	// get an index of the axis' vertex buffer
	const UINT axis_vb_idx = modelsStore.GetRelatedVertexBufferByModelIdx(axisModelIdx);

	// set that we want to render the axis using topology linelist
	modelsStore.SetPrimitiveTopologyForVertexBufferByIdx(axis_vb_idx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

///////////////////////////////////////////////////////////

void ComputeChunksToModels(EntityStore & modelsStore)
{
	// prepare data for computation of relations between chunks and models
	//std::vector<uint32_t> modelsIDs = modelsStore.IDs_;
	std::vector<XMVECTOR> minChunksDimensionsArr = modelsStore.minChunksDimensions_;
	std::vector<XMVECTOR> maxChunksDimensionsArr = modelsStore.maxChunksDimensions_;
	std::vector<std::vector<uint32_t>> relationsChunksToModelsArr;

	// compute relations between chunks and models
	modelsStore.ComputeRelationsModelsToChunks(modelsStore.chunksCount_,
		modelsStore.numOfModels_,
		minChunksDimensionsArr,
		maxChunksDimensionsArr,
		relationsChunksToModelsArr);

	// store the relations into the models storage
	std::move(relationsChunksToModelsArr.begin(), relationsChunksToModelsArr.end(), modelsStore.relationsChunksToModels_.begin());

	// clear transiend data
	//modelsIDs.clear();
	minChunksDimensionsArr.clear();
	maxChunksDimensionsArr.clear();
	relationsChunksToModelsArr.clear();
}

///////////////////////////////////////////////////////////

void CreateChunkBoundingBoxes(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	EntityStore & modelsStore,
	const UINT chunkDimension)
{
	try
	{
		// get positions for these chunks bounding boxes
		const std::vector<DirectX::XMVECTOR> & positions = modelsStore.GetChunksCenterPositions();

		// how many bounding boxes we want to create
		const UINT chunksBoundingBoxesCount = (UINT)positions.size();

		assert(chunksBoundingBoxesCount > 0);

		// create chunk BASIC bounding box
		const UINT chunkBoundingBoxIdx = modelsCreator.CreateChunkBoundingBox(chunkDimension, modelsStore, pDevice);

		// set that we want to render cubes using some particular shader
		const UINT chunkBoxVertexBufferIdx = modelsStore.GetRelatedVertexBufferByModelIdx(chunkBoundingBoxIdx);
		modelsStore.SetPrimitiveTopologyForVertexBufferByIdx(chunkBoxVertexBufferIdx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		// create copies of BASIC bounding box (-1 because we've already create a one box (BASIC))
		modelsStore.CreateBunchCopiesOfModelByIndex(chunkBoundingBoxIdx, chunksBoundingBoxesCount - 1);

		// set positions for bounding boxes
		std::copy(positions.begin(), positions.end(), modelsStore.positions_.begin() + chunkBoundingBoxIdx);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		ASSERT_TRUE(false, "can't create bounding boxes for chunks");
	}

}

///////////////////////////////////////////////////////////
#endif

#if 0
void CreateGeospheres(ID3D11Device* pDevice,
	ModelsCreator& modelsCreator,
	EntityStore& modelsStore,
	const UINT numOfGeospheres)
{
	// if we don't want to render any geosphere just go out
	if (numOfGeospheres == 0)
		return;

	// --------------------------------------------------- //
	//              SETUP BASIC GEOSPHERE
	// --------------------------------------------------- //

	const UINT origin_GeoSphereIdx = modelsCreator.CreateGeophere(
		pDevice,
		modelsStore,
		3.0f,         // radius (this line) and subdivisitions (next line)
		10);


	// --------------------------------------------------- //
	//                  SETUP GEOSPHERE
	// --------------------------------------------------- //

	// get an index of the geosphere's vertex buffer
	const UINT vb_idx = modelsStore.GetRelatedVertexBufferByModelIdx(origin_GeoSphereIdx);

	// define paths to textures
	const std::string diffuseMapPath{ "data/textures/gigachad.dds" };
	const std::string lightMapPath{ "data/textures/lightmap_white.dds" };

	// set textures for the model
	modelsStore.SetTexturesForVB_ByIdx(
		vb_idx,                      // index of the vertex buffer
		{
			{aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey(diffuseMapPath)},
			{aiTextureType_LIGHTMAP, TextureManagerClass::Get()->GetTextureByKey(lightMapPath)},
		});

	modelsStore.SetRenderingShaderForVertexBufferByIdx(vb_idx, EntityStore::LIGHT_SHADER);


	// --------------------------------------------------- //
	//            CREATE COPIES (IF NECESSARY)
	// --------------------------------------------------- //
#
	// generate random positions for geospheres
	std::vector<DirectX::XMVECTOR> positions(numOfGeospheres);

	for (DirectX::XMVECTOR& pos : positions)
	{
		// set random x,y, and z coords for position
		pos = { MathHelper::RandF(1, 30), MathHelper::RandF(1, 30), MathHelper::RandF(1, 30) };
	}

	// if we want to create more than only one geosphere (-1 because we've already create one (BASIC))
	std::vector<UINT> indicesOfGeospheres = modelsStore.CreateBunchCopiesOfModelByIndex(origin_GeoSphereIdx, numOfGeospheres - 1);
	indicesOfGeospheres.push_back(origin_GeoSphereIdx);

	modelsStore.SetPositionsForModelsByIdxs(indicesOfGeospheres, positions);

}

#endif
