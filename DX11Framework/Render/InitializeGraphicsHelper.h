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
#include "../ECS_Entity/EntityManager.h"
#include "../GameObjects/TextureManagerClass.h"
#include "../Common/MathHelper.h"

using namespace DirectX;


#if 0
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

void CreateTerrainFromFile(
	ID3D11Device* pDevice,
	EntityStore & modelsStore,
	ModelsCreator & modelsCreator,
	const std::string & terrainSetupFile)
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
	Settings & settings,
	const EntityStore::RENDERING_SHADERS & renderingShaderType)
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

///////////////////////////////////////////////////////////

void CreateCubes(ID3D11Device* pDevice,
	Settings & settings,
	ModelsCreator & modelsCreator,
	EntityStore & modelsStore,
	const EntityStore::RENDERING_SHADERS renderingShaderType,
	const UINT numOfCubes)
{
	//
	// CREATE CUBES
	//

	// check if we want to create any cube if no we just return from the function
	if (numOfCubes == 0) return;


	// --------------------------------------------------- //
	//                 CREATE BASIC CUBE
	// --------------------------------------------------- //

	const UINT originCube_idx = modelsCreator.CreateCube(pDevice,
		modelsStore,
		"",                        // manually create a cube (generate its vertices/indices/etc.)
		{ -10, 2, 0 },             // position
		DirectX::XMVectorZero(),   // rotation
		DirectX::XMVectorZero(),   // position (this line) and rotation (next line) modificators
		DirectX::XMVectorZero());  


	// --------------------------------------------------- //
	//                   SETUP CUBE(S)
	// --------------------------------------------------- //

	// define paths to textures
	const std::string diffuseMapPath{ "data/textures/flare.dds" };
	const std::string lightMapPath{ "data/textures/flarealpha_a.dds" };

	// get an index of the cube's vertex buffer
	const UINT cube_vb_idx = modelsStore.GetRelatedVertexBufferByModelIdx(originCube_idx);

	// setup rendering shader for the vertex buffer of cube
	modelsStore.SetRenderingShaderForVertexBufferByIdx(cube_vb_idx, renderingShaderType);

	// set textures for the model
	modelsStore.SetTexturesForVB_ByIdx(
		cube_vb_idx,                      // index of the vertex buffer
		{
			{aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey(diffuseMapPath)},
			{aiTextureType_LIGHTMAP, TextureManagerClass::Get()->GetTextureByKey(lightMapPath)},
		});

	// setup material
	Material& mat = modelsStore.materials_[originCube_idx];

	mat.ambient  = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat.diffuse  = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);


	// --------------------------------------------------- //
	//            CREATE COPIES (IF NECESSARY)
	// --------------------------------------------------- //
#if 0

	// if we want to create some copies of the origin cube
	std::vector<UINT> copiedCubesIndices;    // will contain indices of the copies of the origin cube
	const UINT numOfCopies = numOfCubes - 1;

	if (numOfCopies > 0)
	{
		// create (numOfCubes-1) copies of the origin cube (-1 because we've already created one cube)
		copiedCubesIndices = modelsStore.CreateBunchCopiesOfModelByIndex(originCube_idx, numOfCopies);
	}

	// ----------------------------------------------------- //
#endif


	return;
}

///////////////////////////////////////////////////////////

void CreateCylinders(ID3D11Device* pDevice,
	EntityStore & modelsStore,
	ModelsCreator & modelsCreator,
	const ModelsCreator::CYLINDER_PARAMS & cylParams,
	const EntityStore::RENDERING_SHADERS renderingShaderType,
	const UINT numOfCylinders)
{
	// we don't want to create any cylinder so just go out
	if (numOfCylinders == 0)
		return;

	assert(numOfCylinders == 10);


	// ----------------------------------------------------- //
	//             PREPARE DATA FOR CYLINDERS
	// ----------------------------------------------------- //

	// define transformations from local spaces to world space
	std::vector<XMVECTOR> cylPos(numOfCylinders);
	static float cylHeightPos = 0.0f;

	for (UINT i = 0; i < cylPos.size()/2; ++i)
	{
		cylPos[i * 2 + 0] = { -5.0f, cylHeightPos, -10.0f + i*5.0f };
		cylPos[i * 2 + 1] = { +5.0f, cylHeightPos, -10.0f + i*5.0f };
	}

	cylHeightPos += 5.0f;

	// --------------------------------------------------- //
	//        create a new BASIC cylinder model
	// --------------------------------------------------- //

	const UINT originCyl_Idx = modelsCreator.CreateCylinder(
		pDevice,
		modelsStore,
		cylParams);



	// --------------------------------------------------- //
	//                SETUP CYLINDERS
	// --------------------------------------------------- //

	// set that we want to render cubes using some particular shader
	const UINT cylinder_vb_idx = modelsStore.relationsModelsToVB_[originCyl_Idx];
	modelsStore.SetRenderingShaderForVertexBufferByIdx(cylinder_vb_idx, EntityStore::RENDERING_SHADERS::LIGHT_SHADER);

	// define paths to cylinder's textures
	const std::string diffuseMapPath{ "data/textures/gigachad.dds" };

	// set textures for the cylinder
	modelsStore.SetTexturesForVB_ByIdx(
		cylinder_vb_idx,                      // index of the vertex buffer
		{
			{aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey(diffuseMapPath)},
		});


	// set cylinder's material (material varies per object)
	Material& mat = modelsStore.materials_[originCyl_Idx];

#if 0
	const float inv_255 = 1.0f / 255.0f;
	const float red = 73.0f * inv_255;
	const float green = 36.0f * inv_255;
	const float blue = 62.0f * inv_255;
#elif 1
	const float red = 1.0f;
	const float green = 1.0f;
	const float blue = 1.0f;
#endif


	mat.ambient = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.diffuse = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 10.0f);


	// --------------------------------------------------- //
	//            CREATE COPIES (IF NECESSARY)
	// --------------------------------------------------- //
#if 1
	// if we want to create more than only one cylinder model;
	// notice: -1 because we've already create one cylinder (basic)
	const UINT numOfCopies = numOfCylinders - 1;
	std::vector<UINT> cylIndices = modelsStore.CreateBunchCopiesOfModelByIndex(originCyl_Idx, numOfCopies);
	cylIndices.push_back(originCyl_Idx);

	// apply generated positions/rotations/scales/etc. to the cylinders
	modelsStore.SetPositionsForModelsByIdxs(cylIndices, cylPos);
#endif
}

///////////////////////////////////////////////////////////

void CreateWaves(ID3D11Device* pDevice,
	EntityStore & modelsStore,
	ModelsCreator & modelsCreator,
	const ModelsCreator::WAVES_PARAMS & wavesParams,
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


#endif

///////////////////////////////////////////////////////////

void CreateSpheresHelper(
	ID3D11Device* pDevice,
	EntityManager& entityMgr,
	ModelsCreator& modelsCreator,
	MeshStorage& meshStorage,
	const ModelsCreator::SPHERE_PARAMS& sphereParams,
	const UINT numOfSpheres)
{
	// we don't want to create any cylinder so just go out
	if (numOfSpheres == 0)
		return;

	assert(numOfSpheres > 10);


	const UINT spheresCount = 11;

	// --------------------------------------------------- //
	//         create and setup a sphere mesh 
	// --------------------------------------------------- //

	const std::string sphereMeshID = modelsCreator.CreateSphere(pDevice,
		sphereParams.radius,
		sphereParams.sliceCount,
		sphereParams.stackCount);

	meshStorage.SetRenderingShaderForMeshByID(sphereMeshID, MeshStorage::RENDERING_SHADERS::TEXTURE_SHADER);

	// define paths to textures
	const std::string diffuseMapPath{ "data/textures/gigachad.dds" };
	const std::string lightMapPath{ "data/textures/white_lightmap.dds" };

	// set textures for the mesh
	meshStorage.SetTexturesForMeshByID(
		sphereMeshID,
		{
			{aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey(diffuseMapPath)},
			{aiTextureType_LIGHTMAP, TextureManagerClass::Get()->GetTextureByKey(lightMapPath)},
		});

	// setup mesh material
	Material material;

	const float inv_255 = 1.0f / 255.0f;
	const float red = 187.0f * inv_255;
	const float green = 132.0f * inv_255;
	const float blue = 147.0f * inv_255;

	material.ambient = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	material.diffuse = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	material.specular = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

	meshStorage.SetMaterialForMeshByID(sphereMeshID, material);

	// --------------------------------------------------- //
	//         prepare transform data for spheres
	// --------------------------------------------------- //

	// define transformations from local spaces to world space
	std::vector<XMFLOAT3> spheresPos(numOfSpheres);
	std::vector<XMFLOAT3> spheresRot(numOfSpheres, { 0,0,0 });
	std::vector<XMFLOAT3> spheresScales(numOfSpheres, { 1,1,1 });

	// we create 5 rows of 2 cylinders and spheres per row
	for (UINT i = 0; i < 5; ++i)
	{
		spheresPos[i * 2 + 0] = { -5.0f, 3.5f, -10.0f + i * 5.0f };
		spheresPos[i * 2 + 1] = { +5.0f, 3.5f, -10.0f + i * 5.0f };
	}

	// set position and scale for the central sphere
	spheresPos.back() = { 0,11,0 };
	spheresScales.back() = { 3, 3, 3 };

	// --------------------------------------------------- //
	//         prepare movement data for spheres
	// --------------------------------------------------- //

	const DirectX::XMFLOAT3 noTranslation{ 0,0,0 };
	const DirectX::XMFLOAT4 noRotationChange{ 0,0,0,0 };
	const DirectX::XMFLOAT3 noScaleChange{ 1,1,1 };

	const std::vector<DirectX::XMFLOAT3> translationsArr(spheresCount, noTranslation);
	const std::vector<DirectX::XMFLOAT3> scaleFactors(spheresCount, noScaleChange);
	std::vector<DirectX::XMFLOAT4> rotationQuatsArr(spheresCount, noRotationChange);
	std::vector<DirectX::XMVECTOR> rotationQuatVecArr(spheresCount);

	// create rotation quaternions
	const float rotationSpeed = DirectX::XM_PI * 0.001f;

	// create rotation quaternions
	for (DirectX::XMVECTOR& rotQuat : rotationQuatVecArr)
		rotQuat = DirectX::XMQuaternionRotationAxis({ MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF() }, rotationSpeed);

	// convert rotations quaternions into XMFLOAT4
	for (size_t idx = 0; idx < rotationQuatVecArr.size(); ++idx)
		DirectX::XMStoreFloat4(&rotationQuatsArr[idx], rotationQuatVecArr[idx]);


	// --------------------------------------------------- //
	//        create and setup a sphere entities
	// --------------------------------------------------- //

	const std::string sphereID = "sphere_";
	std::vector<EntityID> entityIDs(spheresCount);

	// generate unique ID for each sphere entity
	for (UINT idx = 0; idx < entityIDs.size(); ++idx)
		entityIDs[idx] = { sphereID + std::to_string(idx) };

	entityMgr.CreateEntities(entityIDs);
	entityMgr.AddTransformComponents(entityIDs, spheresPos, spheresRot, spheresScales);
	entityMgr.AddMovementComponents(entityIDs, translationsArr, rotationQuatsArr, scaleFactors);
	entityMgr.AddMeshComponents(entityIDs, sphereMeshID);
	entityMgr.AddRenderingComponents(entityIDs);

#if 0
	

	// --------------------------------------------------- //
	//            CREATE COPIES (IF NECESSARY)
	// --------------------------------------------------- //

	modelsStore.SetPosRotScaleForModelsByIdxs(
		{ originSphere_idx },
		{ {0, 15, 0,1} },
		{ spheresRot[0] },
		{ spheresScales[0] });
	modelsStore.SetAsModifiableModelsByTextID(modelsStore.GetTextIdByIdx(originSphere_idx));
	//modelsStore.SetPositionModificator(originSphere_idx, { 0.01f, 0, 0 });
	modelsStore.SetRotationModificator(originSphere_idx, DirectX::XMQuaternionRotationAxis({ 1, 0, 0 }, DirectX::XM_PI*0.001f));

	// create copies of the origin sphere model (-1 because we've already create one (basic) sphere)
// and get indices of all the copied models
	std::vector<UINT> copiedModelsIndices(modelsStore.CreateBunchCopiesOfModelByIndex(originSphere_idx, numOfSpheres - 1));
	copiedModelsIndices.push_back(originSphere_idx);

	// apply generated positions/rotations/scales to the spheres
	modelsStore.SetPosRotScaleForModelsByIdxs(
		copiedModelsIndices,
		spheresPos,
		spheresRot,
		spheresScales);


#endif

	
}



#if 0
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
	geoGen.CreateGridMesh(
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

void CreatePyramids(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	EntityStore & modelsStore,
	const ModelsCreator::PYRAMID_PARAMS & pyramidParams,
	const EntityStore::RENDERING_SHADERS & pyramidRenderingShader)
{

	// --------------------------------------------------- //
	//                CREATE PYRAMID
	// --------------------------------------------------- //

	const UINT pyramidIdx = modelsCreator.CreatePyramid(
		pDevice,
		modelsStore,
		pyramidParams.height,
		pyramidParams.baseWidth,
		pyramidParams.baseDepth,
		{ 0, -1, 0, 1 },             // position
		DirectX::XMVectorZero(),     // rotation
		DirectX::XMVectorZero(),     // position and rotation modificators 
		DirectX::XMVectorZero());  


	// --------------------------------------------------- //
	//                  SETUP PYRAMID
	// --------------------------------------------------- //

	// setup material for the pyramid
	Material & mat = modelsStore.materials_[pyramidIdx];

#if 1  // orange
	const float red = 251.0f / 255.0f;
	const float green = 109.0f / 255.0f;
	const float blue = 72.0f / 255.0f;

#elif 0  // pink/purple
	const float red = 112.0f / 255.0f;
	const float green = 66.0f / 255.0f;
	const float blue = 100.0f / 255.0f;
#endif

	mat.ambient  = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.diffuse  = DirectX::XMFLOAT4(red, green, blue, 1.0f);
	mat.specular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	// get an index of the pyramid's vertex buffer
	const UINT pyramid_vb_idx = modelsStore.GetRelatedVertexBufferByModelIdx(pyramidIdx);

	// define paths to textures
	const std::string diffuseMapPath{ "data/textures/brick01.dds" };
	const std::string lightMapPath{ "data/textures/white_lightmap.dds" };

	// set textures for the model
	modelsStore.SetTexturesForVB_ByIdx(
		pyramid_vb_idx,                      // index of the vertex buffer
		{
			{aiTextureType_DIFFUSE, TextureManagerClass::Get()->GetTextureByKey(diffuseMapPath)},
			{aiTextureType_LIGHTMAP, TextureManagerClass::Get()->GetTextureByKey(lightMapPath)},
		});

	modelsStore.SetRenderingShaderForVertexBufferByIdx(pyramid_vb_idx, pyramidRenderingShader);


	return;
}

///////////////////////////////////////////////////////////

void CreateAxis(ID3D11Device* pDevice,
	ModelsCreator & modelsCreator,
	EntityStore & modelsStore)
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
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create bounding boxes for chunks");
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
	const std::string lightMapPath{ "data/textures/white_lightmap.dds" };

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
