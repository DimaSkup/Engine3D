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
#include "../GameObjects/ModelsStore.h"
#include "../GameObjects/TextureManagerClass.h"

using namespace DirectX;




///////////////////////////////////////////////////////////

void CreateSkullModel(ID3D11Device* pDevice, ModelsStore & modelsStore)
{
	// load skull's vertices and indices data from the file and
	// create a new model using this data

	std::ifstream fin("data/models/skull.txt");

	if (!fin)
	{
		MessageBoxA(0, "data/models/skull.txt not found", 0, 0);
		return;
	}

	UINT vCount = 0;
	UINT tCount = 0;
	std::string ignore;

	fin >> ignore >> vCount;
	fin >> ignore >> tCount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<VERTEX> vertices(vCount);
	for (UINT idx = 0; idx < vCount; ++idx)
	{
		fin >> vertices[idx].position.x >> vertices[idx].position.y >> vertices[idx].position.z;
		fin >> vertices[idx].normal.x >> vertices[idx].normal.y >> vertices[idx].normal.z;
	}

	fin >> ignore >> ignore >> ignore;

	const UINT skullIndexCount = 3 * tCount;
	std::vector<UINT> indices(skullIndexCount);

	for (UINT idx = 0; idx < tCount; ++idx)
	{
		fin >> indices[idx * 3 + 0] >> indices[idx * 3 + 1] >> indices[idx * 3 + 2];
	}

	fin.close();

	// create a new model using these vertex and index data arrays
	const UINT skullModel_idx = modelsStore.CreateNewModelWithData(pDevice, "skull",
		vertices,
		indices,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },
		{ 0, 15, 0 },
		DirectX::XMVectorZero(),
		DirectX::XMVectorZero(),
		DirectX::XMVectorZero());

	// set skull material (material varies per object)
	Material& mat = modelsStore.materials_[skullModel_idx];

	mat.ambient = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mat.diffuse = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	// set that we want to render cubes using some particular shader
	modelsStore.SetRenderingShaderForVertexBufferByModelIdx(skullModel_idx, ModelsStore::LIGHT_SHADER);
}

///////////////////////////////////////////////////////////

void GeneratePositionsRotations_ForCubes_LikeItIsATerrain_InMinecraft(
	const UINT numOfModels,
	_Inout_ std::vector<DirectX::XMVECTOR> & outPositions,
	_Inout_ std::vector<DirectX::XMVECTOR> & outRotations)
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
	geoGen.CreateGridMesh(
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
		DirectX::XMFLOAT3 & pos = meshData.vertices[i].position;

		// use a function for making hills for the terrain
		pos.y = floorf(0.3f * (0.3f * (pos.z*sinf(0.1f * pos.x)) + (pos.x * cosf(0.1f * pos.z))));

		// store the position and rotation for this cube
		outPositions[i] = XMLoadFloat3(&pos);
		outRotations[i] = XMVectorZero();
	}
}

///////////////////////////////////////////////////////////

void GenerateRandom_PositionsRotations_ForCubes(
	const UINT numOfModels,
	_Inout_ std::vector<DirectX::XMVECTOR> & outPositions,
	_Inout_ std::vector<DirectX::XMVECTOR> & outRotations)
{
	assert(numOfModels > 0);

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData meshData;

	// define the width and depth of the cubes terrain
	const float width = sqrtf((float)numOfModels);
	const float depth = width;

	// generate a basic grid which will be used for cubes placement
	geoGen.CreateGridMesh(
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
		DirectX::XMFLOAT3 & pos = meshData.vertices[i].position;

		// setup cubes pos by Y-axis
		pos.y = -1.0f;

		// store the position and rotation for this cube
		outPositions[i] = XMLoadFloat3(&pos);
		outRotations[i] = XMVectorZero();
	}
}

///////////////////////////////////////////////////////////

void CreateTerrain(ID3D11Device* pDevice,
	Settings & settings,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore,
	const ModelsStore::RENDERING_SHADERS terrainRenderingShader)
{
	//
	// CREATE TERRAIN
	//

	const bool isCreateTerrainFromFile = settings.GetSettingBoolByKey("IS_CREATE_TERRAIN_FROM_FILE");
	const bool isGenerateTerrainManually = settings.GetSettingBoolByKey("IS_GENERATE_TERRAIN_MANUALLY");

	// if we want to load terrain data from some data file
	if (isCreateTerrainFromFile)
	{
		// CREATE A TERRAIN GRID FROM FILE
		const std::string terrainSetupFilepath{ "data/terrain/setup_load_bmp_height_map.txt" };

		const UINT terrainGridIdx = modelsCreator.CreateTerrainFromFile(terrainSetupFilepath,
			pDevice,
			modelsStore);

		// get an index of the terrain grid vertex buffer and set a rendering shader for it
		const UINT terrainGridVertexBuffer = modelsStore.relatedToVertexBufferByIdx_[terrainGridIdx];
		modelsStore.useShaderForBufferRendering_[terrainGridVertexBuffer] = ModelsStore::LIGHT_SHADER;

		// set a texture for this terrain grid
		modelsStore.SetTextureByIndex(terrainGridIdx, "data/textures/dirt01d.dds", aiTextureType_DIFFUSE);

	}

	// if we want to generate terrain data manually (generate its meshes, set textures for it, etc.)
	else if (isGenerateTerrainManually)
	{
		// MANUALLY GENERATE A TERRAIN GRID

		// get params of terrain
		const UINT terrainWidth = settings.GetSettingIntByKey("TERRAIN_WIDTH");  // size by X-axis
		const UINT terrainDepth = settings.GetSettingIntByKey("TERRAIN_DEPTH");  // size by Z-axis

																				 // generate a terrain model based on the setup params and get its idx
		const UINT terrainGridIdx = modelsCreator.CreateGeneratedTerrain(pDevice,
			modelsStore,
			(float)terrainWidth,
			(float)terrainDepth,
			terrainWidth + 1,
			terrainDepth + 1);

		// setup the rendering shader for the terrain
		modelsStore.SetRenderingShaderForVertexBufferByIdx(modelsStore.GetRelatedVertexBufferByModelIdx(terrainGridIdx), terrainRenderingShader);

		// set terrain material (material varies per object)
		Material & mat = modelsStore.materials_[terrainGridIdx];

#if 0
		const float red = 1.0f;
		const float green = 175.0f / 255.0f;
		const float blue = 69.0f / 255.0f;
#endif
		const float red = 219.0f / 255.0f;
		const float green = 175.0f / 255.0f;
		const float blue = 160.0f / 255.0f;

		mat.ambient = DirectX::XMFLOAT4(red, green, blue, 1.0f);
		mat.diffuse = DirectX::XMFLOAT4(red, green, blue, 1.0f);
		mat.specular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	}

	else
	{
		COM_ERROR_IF_FALSE(false, "you have to choose the terrain creation type");
	}

}

///////////////////////////////////////////////////////////

void CreateCubes(ID3D11Device* pDevice,
	Settings & settings,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore,
	const ModelsStore::RENDERING_SHADERS renderingShaderType,
	const UINT numOfCubes)
{
	//
	// CREATE CUBES
	//

	// check if we want to create any cube if no we just return from the function
	if (numOfCubes == 0) return;

	// ----------------------------------------------------- //

	const DirectX::XMVECTOR defaultZeroVec{ 0, 0, 0, 1 };

	// create a cube which will be a BASIC CUBE for creation of the other ones;
	// (here we use default zero vector but if we have some input data for positions/rotations/etc.
	//  we'll apply it later to all the models including the origin cube)
	const UINT originCube_idx = modelsCreator.CreateCube(pDevice,
		modelsStore,
		"",               // manually create a cube
		{ -10, 2, 0 },    // position
		defaultZeroVec,   // rotation
		defaultZeroVec,   // position modificator
		defaultZeroVec);  // rotation modificator

	// set cube material (material varies per object)
	Material & mat = modelsStore.materials_[originCube_idx];

	mat.ambient = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat.diffuse = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	// modify cube's texture
	//modelsStore.texTransform_[originCube_idx] = DirectX::XMMatrixScaling(10, 10, 10);


	// ----------------------------------------------------- //

	// if we want to create some copies of the origin cube
	std::vector<UINT> copiedCubesIndices;    // will contain indices of the copies of the origin cube
	const UINT numOfCopies = numOfCubes - 1;

	if (numOfCopies > 0)
	{
		// create (numOfCubes-1) copies of the origin cube (-1 because we've already created one cube)
		copiedCubesIndices = modelsStore.CreateBunchCopiesOfModelByIndex(originCube_idx, numOfCopies);
	}

	// ----------------------------------------------------- //
#if 0
	// we will put here positions/rotations data of cubes
	std::vector<DirectX::XMVECTOR> cubesPositions;
	std::vector<DirectX::XMVECTOR> cubesRotations;


	// if we want to GENERATE positions and rotations data for the cubes to make it like a terrain in minecraft
	const bool isMinecraftMode = settings.GetSettingBoolByKey("IS_MINECRAFT_MODE");

	if (isMinecraftMode && numOfCubes > 1)
	{
		GeneratePositionsRotations_ForCubes_LikeItIsATerrain_InMinecraft(
			numOfCubes,
			cubesPositions,
			cubesRotations);
	}

	// generate absolutely random positions/rotations/etc. of this number of cubes
	else
	{
		GenerateRandom_PositionsRotations_ForCubes(
			numOfCubes,
			cubesPositions,
			cubesRotations);
	}

	// ------------------------------------------------------ //

	if (cubesPositions.size() || cubesRotations.size())
	{
		// apply the positions/rotations for cubes
		std::copy(cubesPositions.begin(), cubesPositions.end(), modelsStore.positions_.begin() + originCube_idx);
		std::copy(cubesRotations.begin(), cubesRotations.end(), modelsStore.rotations_.begin() + originCube_idx);

		// apply the positions/rotations modificators
		//std::copy(positionModificators.begin() + skipOriginCube, positionModificators.end(), modelsStore.positionModificators_.begin() + originCube_idx);
		//std::copy(rotationModificators.begin() + skipOriginCube, rotationModificators.end(), modelsStore.rotationModificators_.begin() + originCube_idx);

		// clear the transient initialization data
		cubesPositions.clear();
		cubesRotations.clear();

		//positionModificators.clear();
		//rotationModificators.clear();

		// apply positions/rotations/scales/etc. to the cubes
		copiedCubesIndices.push_back(originCube_idx);
		modelsStore.UpdateWorldMatricesForModelsByIdxs(copiedCubesIndices);
	}

	// ----------------------------------------------------- //
#endif
	// setup rendering shader for the vertex buffer of cube
	const UINT cubeVertexBufferIdx = modelsStore.GetRelatedVertexBufferByModelIdx(originCube_idx);
	modelsStore.SetRenderingShaderForVertexBufferByIdx(cubeVertexBufferIdx, renderingShaderType);



	return;
}

///////////////////////////////////////////////////////////

void CreateCylinders(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	ModelsCreator & modelsCreator,
	const ModelsCreator::CYLINDER_PARAMS & cylParams,
	const ModelsStore::RENDERING_SHADERS renderingShaderType,
	const UINT numOfCylinders)
{
	// we don't want to create any cylinder so just go out
	if (numOfCylinders == 0)
		return;

	// ----------------------------------------------------- //

	// PREPARE DATA FOR CYLINDERS
	assert(numOfCylinders == 10);

	// define transformations from local spaces to world space
	std::vector<XMVECTOR> cylPos(10);

	// we create 5 rows of 2 cylinders and spheres per row
	for (UINT i = 0; i < 5; ++i)
	{
		cylPos[i * 2 + 0] = { -5.0f, 1.5f, -10.0f + i*5.0f };
		cylPos[i * 2 + 1] = { +5.0f, 1.5f, -10.0f + i*5.0f };
	}

	// --------------------------------------------------- //

	// create a new BASIC cylinder model
	const UINT originCyl_Idx = modelsCreator.CreateCylinder(
		pDevice,
		modelsStore,
		cylParams);

	// set that we want to render cubes using some particular shader
	const UINT cylinderVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[originCyl_Idx];
	modelsStore.useShaderForBufferRendering_[cylinderVertexBufferIdx] = renderingShaderType;

	// set a default texture for the basic cylinder model
	modelsStore.SetTextureByIndex(originCyl_Idx, "data/textures/gigachad.dds", aiTextureType_DIFFUSE);

	// set cylinder material (material varies per object)
	Material & mat = modelsStore.materials_[originCyl_Idx];

#if 0
	const float red = 103.0f / 255.0f;
	const float green = 63.0f / 255.0f;
	const float blue = 105.0f / 255.0f;
#endif

	const float red = 73.0f / 255.0f;
	const float green = 36.0f / 255.0f;
	const float blue = 62.0f / 255.0f;

	mat.ambient = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.diffuse = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 10.0f);

	// --------------------------------------------------- //

	// if we want to create more than only one cylinder model;
	// notice: -1 because we've already create one cylinder (basic)
	const UINT numOfCopies = numOfCylinders - 1;
	std::vector<UINT> cylIndices = modelsStore.CreateBunchCopiesOfModelByIndex(originCyl_Idx, numOfCopies);

	// apply generated positions/rotations/scales/etc. to the cylinders
	std::copy(cylPos.begin(), cylPos.end(), modelsStore.positions_.begin() + originCyl_Idx);

	// since we set new positions for cylinders we have to update its world matrices
	cylIndices.push_back(originCyl_Idx);
	modelsStore.UpdateWorldMatricesForModelsByIdxs(cylIndices);
}

///////////////////////////////////////////////////////////

void CreateWaves(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	ModelsCreator & modelsCreator,
	const ModelsCreator::WAVES_PARAMS & wavesParams,
	const ModelsStore::RENDERING_SHADERS renderingShaderType)
{
	// create a WAVES model
	const UINT wavesIdx = modelsCreator.CreateWaves(
		pDevice,
		modelsStore,
		wavesParams,
		{ 0,-2,0,1 },               // init position
		{ 0, 0,0,0 },               // init rotation
		DirectX::XMVectorZero(),   // by default no position/rotation modification
		DirectX::XMVectorZero());  

	// setup rendering shader for the vertex buffer
	modelsStore.SetRenderingShaderForVertexBufferByModelIdx(wavesIdx, renderingShaderType);

	// set material (material varies per object)
	Material & mat = modelsStore.materials_[wavesIdx];

	// WAVES MATERIAL
	mat.ambient  = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat.diffuse  = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
}

///////////////////////////////////////////////////////////

void CreateSpheres(ID3D11Device* pDevice,
	ModelsStore & modelsStore,
	ModelsCreator & modelsCreator,
	const ModelsCreator::SPHERE_PARAMS & sphereParams,
	const ModelsStore::RENDERING_SHADERS renderingShaderType,
	const UINT numOfSpheres)
{
	// we don't want to create any cylinder so just go out
	if (numOfSpheres == 0)
		return;


	// -------------------------------------------------------------- // 

	// PREPARE DATA FOR SPHERES
	assert(numOfSpheres > 10);

	// define transformations from local spaces to world space
	std::vector<XMVECTOR> spheresPos(numOfSpheres);
	std::vector<XMVECTOR> spheresScales(numOfSpheres);


	// we create 5 rows of 2 cylinders and spheres per row
	for (UINT i = 0; i < 5; ++i)
	{
		spheresPos[i * 2 + 0] = { -5.0f, 3.5f, -10.0f + i*5.0f };
		spheresPos[i * 2 + 1] = { +5.0f, 3.5f, -10.0f + i*5.0f };

		spheresScales[i * 2 + 0] = { 1, 1, 1, 1 };  // default scale
		spheresScales[i * 2 + 1] = { 1, 1, 1, 1 };  // default scale
	}

	// set position and scale for the central sphere
	spheresPos.back() = { 0,11,0 };
	spheresScales.back() = { 3, 3, 3 };

	// -------------------------------------------------------------- // 

	// create a new BASIC sphere model
	const UINT originSphere_idx = modelsCreator.CreateSphere(pDevice,
		modelsStore,
		sphereParams.radius,
		sphereParams.sliceCount,
		sphereParams.stackCount,
		spheresPos[0],
		{ 0, 0, 0, 1 },
		DirectX::XMVectorZero(),   // by default we have no position modification
		DirectX::XMVectorZero());  // by default we have no rotation modification

								   // set default texture for the basic sphere model
	modelsStore.SetTextureByIndex(originSphere_idx, "data/textures/gigachad.dds", aiTextureType_DIFFUSE);

	// setup primitive topology for the vertex buffer of sphere
	const UINT sphereVertexBufferIdx = modelsStore.GetRelatedVertexBufferByModelIdx(originSphere_idx);
	modelsStore.SetPrimitiveTopologyForVertexBufferByIdx(sphereVertexBufferIdx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// setup rendering shader for the vertex buffer
	modelsStore.SetRenderingShaderForVertexBufferByIdx(sphereVertexBufferIdx, renderingShaderType);

	// set sphere material (material varies per object)
	Material & mat = modelsStore.materials_[originSphere_idx];

#if 0
	const float red = 215.0f / 255.0f;
	const float green = 75.0f / 255.0f;
	const float blue = 118.0f / 255.0f;
#endif

	const float red = 187.0f / 255.0f;
	const float green = 132.0f / 255.0f;
	const float blue = 147.0f / 255.0f;


	mat.ambient = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.diffuse = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);


	// -------------------------------------------------------------- // 

	// create copies of the origin sphere model (-1 because we've already create one (basic) sphere)
	// and get indices of all the copied models
	const std::vector<UINT> copiedModelsIndices(modelsStore.CreateBunchCopiesOfModelByIndex(originSphere_idx, numOfSpheres - 1));

	// apply generated positions/rotations/scales to the spheres
	std::copy(spheresPos.begin(), spheresPos.end(), modelsStore.positions_.begin() + originSphere_idx);
	std::copy(spheresScales.begin(), spheresScales.end(), modelsStore.scales_.begin() + originSphere_idx);

	modelsStore.UpdateWorldMatrixForModelByIdx(originSphere_idx);
	modelsStore.UpdateWorldMatricesForModelsByIdxs(copiedModelsIndices);


	modelsStore.SetModelAsModifiable(originSphere_idx + 10);
	modelsStore.SetRotationModificator(originSphere_idx + 10, XMQuaternionRotationMatrix(XMMatrixRotationY(DirectX::XM_PI * 0.01f)));
}

///////////////////////////////////////////////////////////

void CreateEditorGrid(ID3D11Device* pDevice,
	Settings & settings,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore)
{
	//
	// CREATE EDITOR GRID
	//
	GeometryGenerator geoGen;

	std::vector<DirectX::XMVECTOR> editorGridPositions;

	
	const float fullWidthOfEditorGrid = settings.GetSettingFloatByKey("EDITOR_GRID_DIMENSION"); // size of the editor grid by X and Z
	const float halfWidthOfEditorGrid = 0.5f * fullWidthOfEditorGrid;
	const float halfDepthOfEditorGrid = halfWidthOfEditorGrid;
	const float editorGridCellWidth = settings.GetSettingFloatByKey("EDITOR_GRID_CELL_DIMENSION"); // size of the editor's grid cell by X and Z
	const float editorGridCellDepth = editorGridCellWidth;
	const UINT editorGridCellVertexCountByX = static_cast<UINT>(editorGridCellWidth) + 1;
	const UINT editorGridCellVertexCountByZ = editorGridCellVertexCountByX;
	const UINT editorGridPositionsCountInLine = static_cast<UINT>(fullWidthOfEditorGrid / editorGridCellWidth);
	const UINT editorGridCellsCount = editorGridPositionsCountInLine * editorGridPositionsCountInLine;


	// compute positions for each cell of the editor grid
	for (UINT i = 0; i < editorGridPositionsCountInLine; ++i)
	{
		const float posZ = halfDepthOfEditorGrid - i*editorGridCellDepth;

		for (UINT j = 0; j < editorGridPositionsCountInLine; ++j)
		{
			const float posX = -halfWidthOfEditorGrid + j*editorGridCellWidth;

			editorGridPositions.push_back({ posX, 0.0f, posZ });
		}
	}

	// generate mesh data for the editor grid cell
	GeometryGenerator::MeshData editorGridMesh;

	geoGen.CreateGridMesh(
		editorGridCellWidth,
		editorGridCellDepth,
		editorGridCellVertexCountByX,
		editorGridCellVertexCountByZ,
		editorGridMesh);


	// create a cell model of the editor grid
	const UINT originEditorGridCellIdx = modelsStore.CreateNewModelWithData(pDevice,
		"editor_grid_cell",
		editorGridMesh.vertices,
		editorGridMesh.indices,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },
		editorGridPositions[0],  // position of the first cell
		{ 0, 0, 0, 0 },   // rotation 
		{ 0, 0, 0, 0 },   // position changes
		{ 0, 0, 0, 0 }    // rotation changes
	);  

	// set that we want to render the editor grid using topology linelist
	modelsStore.SetPrimitiveTopologyForVertexBufferByIdx(modelsStore.GetRelatedVertexBufferByModelIdx(originEditorGridCellIdx), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
	// create copies of the origin grid cell (-1 because we've already create one cell)
	const std::vector<UINT> idxsOfCopiedCells = modelsStore.CreateBunchCopiesOfModelByIndex(originEditorGridCellIdx, editorGridCellsCount - 1);

	// setup a position for each cell of the editor grid 
	
	std::copy(editorGridPositions.begin(), editorGridPositions.end(), modelsStore.positions_.begin() + originEditorGridCellIdx);
	//for (const UINT cell_idx : idxsOfCopiedCells)
	

	editorGridPositions.clear();
}

///////////////////////////////////////////////////////////

void CreatePyramids(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore,
	const ModelsCreator::PYRAMID_PARAMS & pyramidParams,
	const ModelsStore::RENDERING_SHADERS & pyramidRenderingShader)
{
	// CREATE PYRAMID
	const UINT pyramidIdx = modelsCreator.CreatePyramid(
		pDevice,
		modelsStore,
		pyramidParams.height,
		pyramidParams.baseWidth,
		pyramidParams.baseDepth,
		{ 0,0,0,1 },
		{ 0,0,0,0 },
		DirectX::XMVectorZero(),   // by default no position modification
		DirectX::XMVectorZero());  // by default no rotation modification

								   // setup material for the pyramid
	Material & mat = modelsStore.materials_[pyramidIdx];

#if 1
	const float red = 251.0f / 255.0f;
	const float green = 109.0f / 255.0f;
	const float blue = 72.0f / 255.0f;
#elif 0  // pink/purple
	const float red = 112.0f / 255.0f;
	const float green = 66.0f / 255.0f;
	const float blue = 100.0f / 255.0f;
#endif

	mat.ambient = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.diffuse = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	// setup the pyramid model
	modelsStore.SetTextureByIndex(pyramidIdx, "data/textures/brick01.dds", aiTextureType_DIFFUSE);
	modelsStore.SetRenderingShaderForVertexBufferByIdx(modelsStore.GetRelatedVertexBufferByModelIdx(pyramidIdx), pyramidRenderingShader);


	return;
}

///////////////////////////////////////////////////////////

void CreateAxis(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore)
{
#if 0
	// create a simple cube which will be a part of axis visual navigation
	modelsCreator.CreateCubeMesh(pDevice,
		modelsStore,
		{ 0,0,0 },
		{ 0,0,0 });
#endif

	// generate data for the axis
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData axisMeshData;
	geoGen.CreateAxisMesh(axisMeshData);

	// create an axis model

	const UINT axisModelIdx = modelsStore.CreateNewModelWithData(pDevice,
		"axis",
		axisMeshData.vertices,
		axisMeshData.indices,
		{ TextureManagerClass::Get()->GetTextureByKey("unloaded_texture") },
		{ 0, 0.0001f, 0, 1 }, // position (Y = 0.0001f because if we want to render axis and chunks bounding boxes at the same time there can be z-fighting)
		{ 0, 0, 0, 0 },       // rotation 
		{ 0, 0, 0, 0 },       // position changes
		{ 0, 0, 0, 0 });      // rotation changes);

							  // set that we want to render axis using topology linelist
	const UINT axisVertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[axisModelIdx];
	modelsStore.usePrimTopologyForBuffer_[axisVertexBufferIdx] = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
}

///////////////////////////////////////////////////////////

void ComputeChunksToModels(ModelsStore & modelsStore)
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
	ModelsStore & modelsStore,
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
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create bounding boxes for chunks");
	}

}

///////////////////////////////////////////////////////////

void CreateGeospheres(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	ModelsStore & modelsStore,
	const UINT numOfGeospheres,
	const std::vector<XMVECTOR> & inPositions = {})
{
	// if we don't want to render any geosphere just go out
	if (numOfGeospheres == 0)
		return;

	// create BASIC geosphere models
	const UINT origin_GeoSphereIdx = modelsCreator.CreateGeophere(pDevice, modelsStore, 3.0f, 10);

	// set that we want to render cubes using some particular shader
	const UINT vertexBufferIdx = modelsStore.relatedToVertexBufferByIdx_[origin_GeoSphereIdx];
	modelsStore.useShaderForBufferRendering_[vertexBufferIdx] = ModelsStore::LIGHT_SHADER;

	// set texture for geosphere
	modelsStore.SetTextureByIndex(origin_GeoSphereIdx, "data/textures/gigachad.dds", aiTextureType_DIFFUSE);

	// if we want to create more than only one geosphere (-1 because we've already create one (BASIC))
	modelsStore.CreateBunchCopiesOfModelByIndex(origin_GeoSphereIdx, numOfGeospheres - 1);

	// if we have some input positions for this exact number of geospheres we use it
	if (inPositions.size() == numOfGeospheres)
	{
		std::copy(inPositions.begin(), inPositions.end(), modelsStore.positions_.begin() + origin_GeoSphereIdx);
	}
}
