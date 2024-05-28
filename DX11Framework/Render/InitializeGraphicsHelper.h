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

#include "InitGraphicsHelperDataTypes.h"

using namespace DirectX;






void CreateNanoSuit(
	ID3D11Device* pDevice,
	EntityManager& entityMgr,
	MeshStorage& meshStorage,
	ModelsCreator& modelsCreator)
{
	const std::string entityID = "nanosuit";
	const std::string filepath = "data/models/nanosuit/nanosuit.obj";

	// load all the meshes of the model from the data file
	const std::vector<MeshID> meshesIDs = modelsCreator.CreateFromFile(pDevice, filepath);

	// set shader for each mesh
	for (const MeshID& meshID : meshesIDs)
		meshStorage.SetRenderingShaderForMeshByID(meshID, Mesh::RENDERING_SHADERS::TEXTURE_SHADER);

	TransformData transformData;
	transformData.positions.push_back({ 10, -3, 0 });
	transformData.directions.push_back({ 0, DirectX::XM_PIDIV2, 0 });
	transformData.scales.push_back({ 1, 1, 1 });

	// create an entity and setup it for rendering
	entityMgr.CreateEntities({ entityID });
	entityMgr.AddTransformComponents({ entityID }, transformData.positions, transformData.directions, transformData.scales);
	entityMgr.AddMeshComponents({ entityID }, meshesIDs);
	entityMgr.AddRenderingComponents({ entityID });
}

///////////////////////////////////////////////////////////

void PrepareMaterialsForMeshes(std::map<MeshID, DataForMeshInit>& meshInitData)
{
	try
	{
		std::map<std::string, RGBA_COLOR> colorsForMaterials =
		{
			//{"pyramid_ambient", RGBA_COLOR(73.0f, 36.0f, 62.0f, 1.0f, true)},
			{"pyramid_ambient", RGBA_COLOR(1,1,1,1)},
			{"pyramid_diffuse", RGBA_COLOR(1,1,1,1)},
			{"pyramid_specular", RGBA_COLOR(0.2f, 0.2f, 0.2f, 10.0f)},

			{"cylinder_ambient", RGBA_COLOR(187.0f, 132.0f, 147.0f, 1.0f, true)},
			{"cylinder_diffuse", RGBA_COLOR(187.0f, 132.0f, 147.0f, 1.0f, true)},
			{"cylinder_specular", RGBA_COLOR(0.8f, 0.8f, 0.8f, 96.0f)},

			{"sphere_ambient", RGBA_COLOR(187.0f, 132.0f, 147.0f, 1.0f, true)},
			{"sphere_diffuse", RGBA_COLOR(187.0f, 132.0f, 147.0f, 1.0f, true)},
			{"sphere_specular", RGBA_COLOR(0.8f, 0.8f, 0.8f, 96.0f)},
		};

		const std::map<MeshID, std::vector<std::string>> meshesMaterials =
		{
			{"pyramid",  {"pyramid_ambient",  "pyramid_diffuse",  "pyramid_specular"}},
			{"cylinder", {"cylinder_ambient", "cylinder_diffuse", "cylinder_specular"}},
			{"sphere",   {"sphere_ambient",   "sphere_diffuse",   "sphere_specular"}},
		};


		// setup material for each mesh by ID
		for (auto& it : meshesMaterials)
		{
			const MeshID& meshID = it.first;
			const std::vector<std::string>& materialKeys = it.second;
			DataForMeshInit& data = meshInitData.at(meshID);

			data.material.ambient_ = DirectX::XMFLOAT4(colorsForMaterials.at(materialKeys[0]).rgba);
			data.material.diffuse_ = DirectX::XMFLOAT4(colorsForMaterials.at(materialKeys[1]).rgba);
			data.material.specular_ = DirectX::XMFLOAT4(colorsForMaterials.at(materialKeys[2]).rgba);
		}
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't setup material for some mesh");
	}
}

///////////////////////////////////////////////////////////

void PrepareTexturesForMeshes(std::map<std::string, DataForMeshInit>& meshInitData)
{
	// setup textures for meshes (later it will be used during creation of meshes)

	try
	{
		// 'mesh_id' => [texture_type' => 'path_to_texture_file']
		std::map<MeshID, std::map<aiTextureType, std::string>> texturesPaths;   

		texturesPaths.insert({ "pyramid", {} });
		texturesPaths.insert({ "cylinder", {} });
		texturesPaths.insert({ "sphere", {} });
		
		texturesPaths["pyramid"] = {
			{aiTextureType_DIFFUSE, "data/textures/brick01.dds"},
			{aiTextureType_LIGHTMAP, "data/textures/white_lightmap.dds"}
		};

		texturesPaths["cylinder"] = {
			{aiTextureType_DIFFUSE, "data/textures/gigachad.dds"},
			{aiTextureType_LIGHTMAP, "data/textures/white_lightmap.dds"}
		};

		texturesPaths["sphere"] = {
			{aiTextureType_DIFFUSE, "data/textures/cat.dds"},
			{aiTextureType_LIGHTMAP, "data/textures/white_lightmap.dds"}
		};
		
		// ------------------------------------------------ //

		// create pairs ['texture_type' => 'ptr_to_texture_obj'] for each mesh
		for (auto& it : meshInitData)
		{
			const std::string& meshID = it.first;
			DataForMeshInit& data = it.second;

			// get ptrs to textures
			TextureClass* pDiffuseMap = TextureManagerClass::Get()->GetTextureByKey(texturesPaths.at(meshID)[aiTextureType_DIFFUSE]);
			TextureClass* pLightMap = TextureManagerClass::Get()->GetTextureByKey(texturesPaths.at(meshID)[aiTextureType_LIGHTMAP]);

			data.texturesMap =
			{
				{aiTextureType_DIFFUSE, pDiffuseMap },
				{aiTextureType_LIGHTMAP, pLightMap }
			};
		}

	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't setup material for some mesh");
	}
}

///////////////////////////////////////////////////////////

void PrepareRenderingShadersForMeshes(std::map<std::string, DataForMeshInit>& meshInitData)
{
	// setup what king of shader will be used for rendering the mesh
	try
	{
		meshInitData.at("pyramid").renderingShaderType = Mesh::RENDERING_SHADERS::TEXTURE_SHADER;
		meshInitData.at("cylinder").renderingShaderType = Mesh::RENDERING_SHADERS::TEXTURE_SHADER;
		meshInitData.at("sphere").renderingShaderType = Mesh::RENDERING_SHADERS::TEXTURE_SHADER;
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		THROW_ERROR("can't setup material for some mesh");
	}
}

///////////////////////////////////////////////////////////

void PrepareDataForModelsHelper(std::map<std::string, DataForMeshInit>& meshesInitData)
{
	// prepare mesh data for all the meshes ('mesh_id' => 'init_data')
	 
	meshesInitData = 
	{
		{"pyramid", {}},
		{"cylinder", {}},
		{"sphere", {}},
	};
	
	PrepareMaterialsForMeshes(meshesInitData);
	PrepareTexturesForMeshes(meshesInitData);
	PrepareRenderingShadersForMeshes(meshesInitData);
}


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

#endif

// ************************************************************************************
// 
//                         CREATE CYLINDERS HELPERS
//                   main func is the CreateCylindersHelper()
// 
// ************************************************************************************


void CreateAndSetupCylinderMesh(
	ID3D11Device* pDevice,
	ModelsCreator& modelsCreator,
	MeshStorage& meshStorage,
	std::string& outCylinderMeshID,
	const DataForMeshInit& meshInitData,
	const ModelsCreator::CYLINDER_PARAMS& cylParams)
{
	// generate and setup a cylinder mesh;
	// NOTICE: all the meshes are stored inside the meshStorage;

	const std::string cylMeshID = modelsCreator.CreateCylinder(pDevice, cylParams);

	// apply mesh settings
	meshStorage.SetRenderingShaderForMeshByID(cylMeshID, meshInitData.renderingShaderType);
	meshStorage.SetTexturesForMeshByID(cylMeshID, meshInitData.texturesMap);
	meshStorage.SetMaterialForMeshByID(cylMeshID, meshInitData.material);

	// setup cylinder mesh ID param
	outCylinderMeshID = cylMeshID;
}

///////////////////////////////////////////////////////////

void PrepareTransformDataForCylinders(UINT& cylindersCount, TransformData& data)
{
	// prepare transform data for cylinders entities
	// 
	// ATTENTION: here we set value of cylinders count

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
	cylindersCount = (UINT)data.positions.size();
	ASSERT_TRUE(cylindersCount == (UINT)data.directions.size(), "positions arr size != directions arr size");
	ASSERT_TRUE(cylindersCount == (UINT)data.scales.size(), "positions arr size != scales arr size");
}

///////////////////////////////////////////////////////////

void CreateAndSetupCylindersEntities(
	EntityManager& entityMgr,
	const UINT cylindersCount,
	const std::string& cylinderMeshID,
	const TransformData& transformData,
	const MovementData& movementData)
{
	const std::string cylinderID = "cylinder_";
	std::vector<EntityID> entityIDs(cylindersCount);

	// generate unique ID for each entity
	for (UINT idx = 0; idx < entityIDs.size(); ++idx)
		entityIDs[idx] = { cylinderID + std::to_string(idx) };

	entityMgr.CreateEntities(entityIDs);
	entityMgr.AddTransformComponents(entityIDs, transformData.positions, transformData.directions, transformData.scales);
	//entityMgr.AddMovementComponents(entityIDs, movementData.translations, movementData.rotQuats, movementData.scaleChanges);
	entityMgr.AddMeshComponents(entityIDs, { cylinderMeshID });
	entityMgr.AddRenderingComponents(entityIDs);
}

///////////////////////////////////////////////////////////

void CreateCylindersHelper(
	ID3D11Device* pDevice,
	EntityManager& entityMgr,
	ModelsCreator& modelsCreator,
	MeshStorage& meshStorage,
	const DataForMeshInit& meshInitData,
	const ModelsCreator::CYLINDER_PARAMS & cylParams)
{
	// 1. create cylinder mesh
	// 2. prepare transform data (at this step we define how many entities we will have)
	// 3. prepare movement data (optional)
	// 4. create entities
	// 5. setup entities

	// define transformations (position, direction, scale) from local spaces to world space
	TransformData transformData;
	MovementData movementData;
	UINT cylindersCount = 0;
	std::string cylinderMeshID{ "" };

	CreateAndSetupCylinderMesh(pDevice, modelsCreator, meshStorage, cylinderMeshID, meshInitData, cylParams);
	PrepareTransformDataForCylinders(cylindersCount, transformData);
	//PrepareMovementDataForCylinders(cylindersCount, movementData);

	CreateAndSetupCylindersEntities(
		entityMgr,
		cylindersCount,
		cylinderMeshID,
		transformData,
		movementData);
}


#if 0
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


// ************************************************************************************
// 
//                          CREATE PYRAMIDS HELPERS
//                   main func is the CreatePyramidsHelper()
// 
// ************************************************************************************

void CreateAndSetupPyramidMesh(
	ID3D11Device* pDevice,
	ModelsCreator& modelsCreator,
	MeshStorage& meshStorage,
	std::string& outPyramidMeshID,
	const DataForMeshInit& meshInitData,
	const ModelsCreator::PYRAMID_PARAMS& pyrParams)
{
	// generate and setup a pyramid mesh;
	// NOTICE: all the meshes are stored inside the meshStorage;

	const std::string pyramidMeshID = modelsCreator.CreatePyramid(pDevice,
		pyrParams.height,
		pyrParams.baseWidth,
		pyrParams.baseDepth);

	// apply mesh settings
	meshStorage.SetRenderingShaderForMeshByID(pyramidMeshID, meshInitData.renderingShaderType);
	meshStorage.SetTexturesForMeshByID(pyramidMeshID, meshInitData.texturesMap);
	meshStorage.SetMaterialForMeshByID(pyramidMeshID, meshInitData.material);

	// setup output mesh ID parameter
	outPyramidMeshID = pyramidMeshID;
}

///////////////////////////////////////////////////////////

void PrepareTransformDataForPyramids(UINT& pyramidsCount, TransformData& data)
{
	// prepare transform data for spheres entities
	// 
	// ATTENTION: here we set value of spheres count

	// add a pyramid at the center
	data.positions.push_back({ 0,-2,0 });
	data.directions.push_back({ 0,0,0 });
	data.scales.push_back({ 1,1,1 });

	// in the end we have to check if data arrays have equal size
	pyramidsCount = (UINT)data.positions.size();
	ASSERT_TRUE(pyramidsCount == (UINT)data.directions.size(), "positions arr size != directions arr size");
	ASSERT_TRUE(pyramidsCount == (UINT)data.scales.size(), "positions arr size != scales arr size");

}

///////////////////////////////////////////////////////////

void CreateAndSetupPyramidsEntities(
	EntityManager& entityMgr,
	const UINT pyramidsCount,
	const std::string& pyramidMeshID,
	const TransformData& transformData,
	const MovementData& movementData)
{
	const std::string prefix = "pyramid_";
	std::vector<EntityID> entityIDs(pyramidsCount);

	// generate unique ID for each entity
	for (UINT idx = 0; idx < entityIDs.size(); ++idx)
		entityIDs[idx] = { prefix + std::to_string(idx) };

	entityMgr.CreateEntities(entityIDs);
	entityMgr.AddTransformComponents(entityIDs, transformData.positions, transformData.directions, transformData.scales);
	//entityMgr.AddMovementComponents(entityIDs, movementData.translations, movementData.rotQuats, movementData.scaleChanges);
	entityMgr.AddMeshComponents(entityIDs, { pyramidMeshID });
	entityMgr.AddRenderingComponents(entityIDs);
}

///////////////////////////////////////////////////////////

void CreatePyramidsHelper(
	ID3D11Device* pDevice,
	EntityManager& entityMgr,
	ModelsCreator& modelsCreator,
	MeshStorage& meshStorage,
	const DataForMeshInit& meshInitData,
	const ModelsCreator::PYRAMID_PARAMS& pyramidParams)
{
	// 1. create sphere mesh
	// 2. prepare transform data (at this step we define how many entities we will have)
	// 3. prepare movement data
	// 4. create entities
	// 5. setup entities

	// define transformations (position, direction, scale) from local spaces to world space
	TransformData transformData;
	MovementData movementData;
	UINT pyramidsCount = 0;
	std::string pyramidMeshID{ "" };

	CreateAndSetupPyramidMesh(pDevice, 
		modelsCreator,
		meshStorage, 
		pyramidMeshID, 
		meshInitData,
		pyramidParams);
	PrepareTransformDataForPyramids(pyramidsCount, transformData);
	//PrepareMovementDataForPyramids(pyramidsCount, movementData);

	CreateAndSetupPyramidsEntities(
		entityMgr,
		pyramidsCount,
		pyramidMeshID,
		transformData,
		movementData);
}


// ************************************************************************************
// 
//                          CREATE SPHERES HELPERS
//                   main func is the CreateSpheresHelper()
// 
// ************************************************************************************

void CreateAndSetupSphereMesh(
	ID3D11Device* pDevice,
	ModelsCreator& modelsCreator,
	MeshStorage& meshStorage,
	std::string& outSphereMeshID,
	const DataForMeshInit& meshInitData,
	const ModelsCreator::SPHERE_PARAMS& sphereParams)
{
	// generate and setup a sphere mesh;
	// NOTICE: all the meshes are stored inside the meshStorage;

	const std::string sphereMeshID = modelsCreator.CreateSphere(pDevice,
		sphereParams.radius,
		sphereParams.sliceCount,
		sphereParams.stackCount);

	// apply mesh settings
	meshStorage.SetRenderingShaderForMeshByID(sphereMeshID, meshInitData.renderingShaderType);
	meshStorage.SetTexturesForMeshByID(sphereMeshID, meshInitData.texturesMap);
	meshStorage.SetMaterialForMeshByID(sphereMeshID, meshInitData.material);

	// setup sphere mesh ID param
	outSphereMeshID = sphereMeshID;
}

///////////////////////////////////////////////////////////

void PrepareTransformDataForSpheres(UINT& spheresCount,	TransformData& data)
{
	// prepare transform data for spheres entities
	// 
	// ATTENTION: here we set value of spheres count
	 
	// create positions for 2 rows of 5 spheres by each row
	const UINT spheresPerRow = 5;
	UINT columnIdx = 0;
	for (UINT i = 0; i < spheresPerRow; ++i)
	{
		data.positions.push_back({ -5.0f, 3.5f, -10.0f + columnIdx * 5.0f});
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
	spheresCount = (UINT)data.positions.size();
	ASSERT_TRUE(spheresCount == (UINT)data.directions.size(), "positions arr size != directions arr size");
	ASSERT_TRUE(spheresCount == (UINT)data.scales.size(), "positions arr size != scales arr size");
}

///////////////////////////////////////////////////////////

void PrepareMovementDataForSpheres(const UINT spheresCount,	MovementData& data)
{
	// default movement values
	const float rotationSpeed = DirectX::XM_PI * 0.001f;
	const DirectX::XMFLOAT3 noTranslation{ 0,0,0 };
	const DirectX::XMFLOAT4 noRotationChange{ 0,0,0,0 };
	const DirectX::XMFLOAT3 noScaleChange{ 1,1,1 };

	// allocate memory for data and set it to default values
	std::vector<DirectX::XMVECTOR> rotationQuatVecArr(spheresCount, DirectX::XMVectorZero());
	data.translations.resize(spheresCount, noTranslation);
	data.rotQuats.resize(spheresCount, noRotationChange);
	data.scaleChanges.resize(spheresCount, noScaleChange);

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

void CreateAndSetupSpheresEntities(
	EntityManager& entityMgr,
	const UINT spheresCount,
	const std::string& sphereMeshID,
	const TransformData& transformData,
	const MovementData& movementData)
{
	const std::string prefix = "sphere_";
	std::vector<EntityID> entityIDs(spheresCount);

	// generate unique ID for each entity
	for (UINT idx = 0; idx < entityIDs.size(); ++idx)
		entityIDs[idx] = { prefix + std::to_string(idx) };

	entityMgr.CreateEntities(entityIDs);
	entityMgr.AddTransformComponents(entityIDs, transformData.positions, transformData.directions, transformData.scales);
	entityMgr.AddMovementComponents(entityIDs, movementData.translations, movementData.rotQuats, movementData.scaleChanges);
	entityMgr.AddMeshComponents(entityIDs, { sphereMeshID });
	entityMgr.AddRenderingComponents(entityIDs);
}

///////////////////////////////////////////////////////////

void CreateSpheresHelper(
	ID3D11Device* pDevice,
	EntityManager& entityMgr,
	ModelsCreator& modelsCreator,
	MeshStorage& meshStorage,
	const DataForMeshInit& meshInitData,
	const ModelsCreator::SPHERE_PARAMS& sphereParams)
{
	// 1. create sphere mesh
	// 2. prepare transform data (at this step we define how many spheres we will have)
	// 3. prepare movement data
	// 4. create entities
	// 5. setup entities

	// define transformations (position, direction, scale) from local spaces to world space
	TransformData transformData;
	MovementData movementData;
	UINT spheresCount = 0;
	std::string sphereMeshID{""};

	CreateAndSetupSphereMesh(pDevice, modelsCreator, meshStorage, sphereMeshID, meshInitData, sphereParams);
	PrepareTransformDataForSpheres(spheresCount, transformData);
	PrepareMovementDataForSpheres(spheresCount, movementData);

	CreateAndSetupSpheresEntities(
		entityMgr,
		spheresCount,
		sphereMeshID,
		transformData,
		movementData);
}



// ************************************************************************************


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
