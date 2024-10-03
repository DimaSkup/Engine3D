// ************************************************************************************
// Filename:     InitializeGraphics.cpp
// Description:  there are functions for initialization of DirectX
//               and graphics parts of the engine;
//
// Created:      02.12.22
// ************************************************************************************
#include "InitializeGraphics.h"
#include "InitGraphicsHelperDataTypes.h"

#include "Common/LIB_Exception.h"    // ECS exception

#include "../Common/Assert.h"
#include "../Common/MathHelper.h"


using namespace DirectX;
using namespace Mesh;

InitializeGraphics::InitializeGraphics()
{
	Log::Debug();
}


// ************************************************************************************
//
//                                PUBLIC FUNCTIONS
//
// ************************************************************************************

bool InitializeGraphics::InitializeDirectX(
	D3DClass & d3d,
	HWND hwnd,
	Settings& settings)
{
	// THIS FUNC initializes the DirectX stuff 
	// (device, deviceContext, swapChain, rasterizerState, viewport, etc)

	try 
	{
		const bool vsyncEnabled = settings.GetBool("VSYNC_ENABLED");
		const bool isFullScreenMode = settings.GetBool("FULL_SCREEN");
		const bool enable4xMSAA = settings.GetBool("ENABLE_4X_MSAA");
		const UINT windowWidth = settings.GetInt("WINDOW_WIDTH");
		const UINT windowHeight = settings.GetInt("WINDOW_HEIGHT");

		const float screenNear = settings.GetFloat("NEAR_Z");
		const float screenDepth = settings.GetFloat("FAR_Z");       // how far we can see

		bool result = d3d.Initialize(
			hwnd,
			windowWidth,
			windowHeight,
			vsyncEnabled,
			isFullScreenMode,
			enable4xMSAA,
			screenNear,
			screenDepth);
		Assert::True(result, "can't initialize the Direct3D");

		// setup the rasterizer state to default params
		d3d.SetRasterState(RenderStates::STATES::CULL_MODE_BACK);
		d3d.SetRasterState(RenderStates::STATES::FILL_MODE_SOLID);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error("can't initialize DirectX");
		return false;
	}

	return true;
}



/////////////////////////////////////////////////

bool InitializeGraphics::InitializeScene(
	D3DClass & d3d,
	ECS::EntityManager& entityMgr,
	MeshStorage& meshStorage,
	Settings& settings,
	RenderToTextureClass& renderToTexture,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
	
{
	// THIS FUNC initializes some main elements of the scene:
	// models, light sources, textures

	try
	{
		bool result = false;

		// near/far Z-coordinate of the frustum/camera
		const float nearZ = settings.GetFloat("NEAR_Z");
		const float farZ = settings.GetFloat("FAR_Z");

		// --------------------------------------------
		// create and init related to textures stuff

		result = renderToTexture.Initialize(pDevice, 256, 256, farZ, nearZ, 1);
		Assert::True(result, "can't initialize the render to texture object");

		// --------------------------------------------
		// create and init scene elements

		result = InitializeModels(
			pDevice, 
			pDeviceContext, 
			entityMgr,
			meshStorage,
			settings, 
			farZ);
		Assert::True(result, "can't initialize models");

		result = InitializeLightSources(entityMgr, settings);
		Assert::True(result, "can't initialize light sources");

		Log::Print("is initialized");
	}

	catch (EngineException& e)
	{
		Log::Error(e, true);
		Log::Error("can't initialize the scene");

		return false;
	}


	return true;
} 

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeCameras(
	CameraClass & editorCamera,
	CameraClass & cameraForRenderToTexture,
	DirectX::XMMATRIX & baseViewMatrix,      // is used for 2D rendering
	Settings& settings)
{
	try
	{
		const CameraClass::CameraInitParams camParams(
			settings.GetInt("WINDOW_WIDTH"),
			settings.GetInt("WINDOW_HEIGHT"),
			settings.GetFloat("NEAR_Z"),
			settings.GetFloat("FAR_Z"),
			settings.GetFloat("FOV_IN_RAD"),  // field of view in radians
			settings.GetFloat("CAMERA_SPEED"),
			settings.GetFloat("CAMERA_SENSITIVITY"));

		editorCamera.Initialize(camParams);              // initialize the editor's camera object
		cameraForRenderToTexture.Initialize(camParams);  // initialize the camera which is used for rendering into textures

		editorCamera.SetPosition({ 0, 0, -3, 1 });
		cameraForRenderToTexture.SetPosition({ 0, 0, -5, 1 });
	
		// initialize view matrices for the editor camera
		editorCamera.UpdateViewMatrix();
		baseViewMatrix = editorCamera.GetViewMatrix(); // initialize a base view matrix with the camera for 2D user interface rendering
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error("can't initialize the cameras objects");
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////

void LoadBasicMeshesParams(
	Settings& settings,
	WavesMeshParams& wavesGeomParams,
	CylinderMeshParams& cylGeomParams,
	SphereMeshParams& sphereGeomParams,
	GeosphereMeshParams& geosphereGeomParams,
	PyramidMeshParams& pyramidGeomParams)
{
	// load params for waves
	wavesGeomParams.numRows = settings.GetInt("WAVES_NUM_ROWS");
	wavesGeomParams.numColumns = settings.GetInt("WAVES_NUM_COLUMNS");
	wavesGeomParams.spatialStep = settings.GetFloat("WAVES_SPATIAL_STEP");
	wavesGeomParams.timeStep = settings.GetFloat("WAVES_TIME_STEP");
	wavesGeomParams.speed = settings.GetFloat("WAVES_SPEED");
	wavesGeomParams.damping = settings.GetFloat("WAVES_DAMPING");

	// load params for cylinders
	cylGeomParams.height = settings.GetFloat("CYLINDER_HEIGHT");
	cylGeomParams.bottomRadius = settings.GetFloat("CYLINDER_BOTTOM_CAP_RADIUS");
	cylGeomParams.topRadius = settings.GetFloat("CYLINDER_TOP_CAP_RADIUS");
	cylGeomParams.sliceCount = settings.GetInt("CYLINDER_SLICE_COUNT");
	cylGeomParams.stackCount = settings.GetInt("CYLINDER_STACK_COUNT");

	// load params for spheres
	sphereGeomParams.radius = settings.GetFloat("SPHERE_RADIUS");
	sphereGeomParams.sliceCount = settings.GetInt("SPHERE_SLICE_COUNT");
	sphereGeomParams.stackCount = settings.GetInt("SPHERE_STACK_COUNT");

	// load params for geospheres
	geosphereGeomParams.radius = settings.GetFloat("GEOSPHERE_RADIUS");
	geosphereGeomParams.numSubdivisions = settings.GetInt("GEOSPHERE_NUM_SUBDIVISITIONS");

	// load params for pyramids
	pyramidGeomParams.height = settings.GetFloat("PYRAMID_HEIGHT");
	pyramidGeomParams.baseWidth = settings.GetFloat("PYRAMID_BASE_WIDTH");
	pyramidGeomParams.baseDepth = settings.GetFloat("PYRAMID_BASE_DEPTH");
}

///////////////////////////////////////////////////////////

void CreateSpheres(
	ID3D11Device* pDevice,
	ECS::EntityManager& entityMgr)
{
	//
	// create and setup spheres entities
	//

	Log::Debug();

	const u32 spheresCount = 10;
	const std::vector<EntityID> enttsIDs = entityMgr.CreateEntities(spheresCount);

	// ---------------------------------------------------------
	// setup transform data for entities

	TransformData transform;

	transform.positions.reserve(spheresCount);
	transform.dirQuats.resize(spheresCount, {0,0,0,1});   // no rotation
	transform.uniformScales.resize(spheresCount, 1.0f);

	// make two rows of the spheres
	for (u32 idx = 0; idx < spheresCount/2; ++idx)
	{
		transform.positions.emplace_back(-5.0f, 5.0f, 10.0f * idx);
		transform.positions.emplace_back(+5.0f, 5.0f, 10.0f * idx);
	}
		
	entityMgr.AddTransformComponent(
		enttsIDs,
		transform.positions,
		transform.dirQuats,
		transform.uniformScales);


	// ---------------------------------------------------------
	// setup movement for the spheres

	MovementData movement;

	movement.translations.resize(spheresCount, { 0,0,0 });
	movement.rotQuats.resize(spheresCount, { 0,0,0,1 });  // XMQuaternionRotationRollPitchYaw(0, 0.001f, 0));
	movement.uniformScales.resize(spheresCount, 1.0f);
	movement.uniformScales[0] = 1.0f;

	entityMgr.AddMoveComponent(
		enttsIDs,
		movement.translations,
		movement.rotQuats,
		movement.uniformScales);

	transform.Clear();
	movement.Clear();	

	// ---------------------------------------------------------
	// set names for the spheres

	std::vector<EntityName> enttsNames;

	for (const EntityID& id : enttsIDs)
		enttsNames.emplace_back("sphere_" + std::to_string(id));

	entityMgr.AddNameComponent(enttsIDs, enttsNames);


	// ---------------------------------------------------------
	// setup meshes of the entities

	TextureManager* pTexMgr = TextureManager::Get();
	MeshStorage* pMeshStorage = MeshStorage::Get();
	ModelsCreator modelCreator;

	const MeshID sphereMeshID = modelCreator.CreateSphere(pDevice);
	const TexPath gigachadTexPath = "data/textures/gigachad.dds";

	pTexMgr->LoadFromFile(gigachadTexPath);

	pMeshStorage->SetTextureForMeshByID(
		sphereMeshID, 
		aiTextureType_DIFFUSE, 
		pTexMgr->GetIDByName(gigachadTexPath));

	entityMgr.AddMeshComponent(enttsIDs, { sphereMeshID });


	// ---------------------------------------------------------
	// setup rendering params of the entities
	entityMgr.AddRenderingComponent(enttsIDs);


	// ------------------------------------------
	// set BOUNDING for the entts

	DirectX::BoundingBox aabb;
	pMeshStorage->GetBoundingDataByID(sphereMeshID, aabb);

	entityMgr.AddBoundingComponent(
		enttsIDs,
		std::vector<DirectX::BoundingBox>(spheresCount, aabb),
		std::vector<ECS::BoundingType>(spheresCount, ECS::BoundingType::SPHERE));
}

///////////////////////////////////////////////////////////

void CreateCylinders(ID3D11Device* pDevice, ECS::EntityManager& enttMgr)
{
	//
	// create and setup cylinders entities
	//

	Log::Debug();

	const UINT cylindersCount = 10;
	const std::vector<EntityID> enttsIDs = enttMgr.CreateEntities(cylindersCount);

	// ---------------------------------------------------------
	// setup transform data for entities

	TransformData transform;

	transform.positions.reserve(cylindersCount);
	transform.dirQuats.resize(cylindersCount, { 0,0,0,1 });  // no rotation
	transform.uniformScales.resize(cylindersCount, 1.0f);

	// make two rows of the cylinders
	for (size_t idx = 0; idx < cylindersCount / 2; ++idx)
	{
		transform.positions.emplace_back(-5.0f, 2.0f, 10.0f * idx);
		transform.positions.emplace_back(+5.0f, 2.0f, 10.0f * idx);
	}

	enttMgr.AddTransformComponent(
		enttsIDs,
		transform.positions,
		transform.dirQuats,
		transform.uniformScales);

	// ---------------------------------------------------------
	// add names for the entities

	std::vector<std::string> names;

	for (const EntityID id : enttsIDs)
		names.push_back("cylinder_" + std::to_string(id));

	enttMgr.AddNameComponent(enttsIDs, names);


	// ---------------------------------------------------------
	// setup meshes of the entities

	TextureManager* pTexMgr = TextureManager::Get();
	ModelsCreator modelCreator;
	MeshStorage* pMeshStorage = MeshStorage::Get();

	const MeshID cylinderMeshID = modelCreator.CreateCylinder(pDevice);
	const TexPath brickTexPath = "data/textures/brick01.dds";

	pTexMgr->LoadFromFile(brickTexPath);

	pMeshStorage->SetTextureForMeshByID(
		cylinderMeshID, 
		aiTextureType_DIFFUSE, 
		pTexMgr->GetIDByName(brickTexPath));

	enttMgr.AddMeshComponent(enttsIDs, { cylinderMeshID });


	// ---------------------------------------------------------
	// setup rendering params of the entities

	enttMgr.AddRenderingComponent(enttsIDs);

	// ------------------------------------------
	// set BOUNDING for the entts

	DirectX::BoundingBox aabb;
	pMeshStorage->GetBoundingDataByID(cylinderMeshID, aabb);

	enttMgr.AddBoundingComponent(
		enttsIDs,
		std::vector<DirectX::BoundingBox>(cylindersCount, aabb),
		std::vector<ECS::BoundingType>(cylindersCount, ECS::BoundingType::AABB));
}

///////////////////////////////////////////////////////////

void CreateCubes(ID3D11Device* pDevice, ECS::EntityManager& enttMgr)
{
	//
	// create and setup cubes entities
	//

	Log::Debug();

	const std::vector<EntityName> names =
	{
		"cat",
		"fireflame",
		"wireFence",
		"woodCrate01",
		"woodCrate02",
		"box01",
	};

	const size cubesCount = std::ssize(names);
	const std::vector<EntityID> enttsIDs = enttMgr.CreateEntities((u32)cubesCount);
	std::map<EntityName, EntityID> enttsNameToID;

	for (u32 idx = 0; const EntityID id : enttsIDs)
		enttsNameToID.insert({ names[idx++], id });

	// ---------------------------------------------------------
	// setup textures for the cubes

	const std::string texDir = "data/textures/";

	const std::vector<std::string> texKeys =
	{
		"cat",
		"fireAtlas",
		"lightmap",
		"wireFence",
		"woodCrate01",
		"woodCrate02",
		"box01",
	};

	const std::vector<TexPath> texPaths =
	{
		texDir + "cat.dds",
		texDir + "fire_atlas.dds",
		texDir + "lightmap.dds",
		texDir + "WireFence.dds",
		texDir + "WoodCrate01.dds",
		texDir + "WoodCrate02.dds",
		texDir + "box01d.dds",
	};

	TextureManager* pTexMgr = TextureManager::Get();
	std::vector<TexID> texIDs;
	
	texIDs.reserve(std::ssize(texPaths));
	
	// load textures from files and get its ids
	for (const TexPath& path : texPaths)
	{
		const TexID id = pTexMgr->LoadFromFile(path);
		texIDs.push_back(id);
	}

	// ---------------------------------------------

	std::map<std::string, TexID> keysToTexIDs;
	std::map<std::string, TexPath> keysToTexPaths;

	for (u32 idx = 0; const TexID& id : texIDs)
		keysToTexIDs.insert({ texKeys[idx++], id });

	for (u32 idx = 0; const TexPath& path : texPaths)
		keysToTexPaths.insert({ texKeys[idx++], path });

	// ---------------------------------------------

	// fill in textures arrays (IDs/paths) with default values of the unloaded texture
	const TexID unloadedTexID = TextureManager::TEX_ID_UNLOADED;
	const u32 texTypesCount = TextureClass::TEXTURE_TYPE_COUNT;

	std::vector<TexID> fireflameTexIDs(texTypesCount, unloadedTexID);
	std::vector<TexID> wireFenceTexIDs(texTypesCount, unloadedTexID);
	std::vector<TexID> woodCrate01TexIDs(texTypesCount, unloadedTexID);
	std::vector<TexID> woodCrate02TexIDs(texTypesCount, unloadedTexID);
	
	std::vector<TexPath> fireflameTexPaths(texTypesCount, "unloaded");
	std::vector<TexPath> wireFenceTexPaths(texTypesCount, "unloaded");
	std::vector<TexPath> woodCrate01TexPaths(texTypesCount, "unloaded");
	std::vector<TexPath> woodCrate02TexPaths(texTypesCount, "unloaded");
	
	// setup textures for some cubes
	fireflameTexPaths[aiTextureType_DIFFUSE] = keysToTexPaths.at("fireAtlas");
	wireFenceTexPaths[aiTextureType_DIFFUSE] = keysToTexPaths.at("wireFence");	
	woodCrate01TexPaths[aiTextureType_DIFFUSE] = keysToTexPaths.at("woodCrate01");
	woodCrate02TexPaths[aiTextureType_DIFFUSE] = keysToTexPaths.at("woodCrate02");

	fireflameTexIDs[aiTextureType_DIFFUSE] = keysToTexIDs.at("fireAtlas");
	wireFenceTexIDs[aiTextureType_DIFFUSE] = keysToTexIDs.at("wireFence");
	woodCrate01TexIDs[aiTextureType_DIFFUSE] = keysToTexIDs.at("woodCrate01");
	woodCrate02TexIDs[aiTextureType_DIFFUSE] = keysToTexIDs.at("woodCrate02");

	// ---------------------------------------------------------
	// create and setup a cube mesh for entities

	ModelsCreator modelCreator;
	MeshStorage* pMeshStorage = MeshStorage::Get();
	const MeshID cubeMeshID = modelCreator.CreateCube(pDevice);

	pMeshStorage->SetTextureForMeshByID(
		cubeMeshID, 
		aiTextureType_DIFFUSE, 
		pTexMgr->GetIDByName("data/textures/cat.dds"));


	// ---------------------------------------------------------
	// prepare transformations

	const std::vector<XMVECTOR> dirQuats(cubesCount, { 0,0,0,1 }); // no rotation
	const std::vector<float> uniformScales(cubesCount, 1.0f);
	std::vector<XMFLOAT3> positions;

	positions.reserve(cubesCount);

	for (size idx = 0; idx < cubesCount; ++idx)
		positions.emplace_back(0.0f, 1.0f, 2.0f * idx);

	positions[2] = { -7, -0.3f, 0 };


	// ---------------------------------------------------------
	// prepare textures transformations

	ECS::AtlasAnimParams atlasTexAnimParams;
	ECS::RotationAroundCoordParams rotAroundCoordsParams;

	atlasTexAnimParams.Push(15, 8, 4);
	rotAroundCoordsParams.Push(0.5f, 0.5f, 0.1f);

	// ---------------------------------------------------------
	// setup the cubes entities

	enttMgr.AddTransformComponent(enttsIDs, positions, dirQuats, uniformScales);
	enttMgr.AddNameComponent(enttsIDs, names);
	enttMgr.AddMeshComponent(enttsIDs, std::vector(cubesCount, cubeMeshID));

	
	enttMgr.AddTexturedComponent(enttsNameToID.at("fireflame"), fireflameTexIDs, fireflameTexPaths);
	enttMgr.AddTexturedComponent(enttsNameToID.at("wireFence"), wireFenceTexIDs, wireFenceTexPaths);
	enttMgr.AddTexturedComponent(enttsNameToID.at("woodCrate01"), woodCrate01TexIDs, woodCrate01TexPaths);
	enttMgr.AddTexturedComponent(enttsNameToID.at("woodCrate02"), woodCrate02TexIDs, woodCrate02TexPaths);


	enttMgr.AddTextureTransformComponent(
		ECS::TexTransformType::ATLAS_ANIMATION, 
		enttsNameToID.at("fireflame"),
		atlasTexAnimParams);

	enttMgr.AddTextureTransformComponent(
		ECS::TexTransformType::ROTATION_AROUND_TEX_COORD, 
		enttsNameToID.at("cat"), 
		rotAroundCoordsParams);

	enttMgr.AddRenderingComponent(enttsIDs);

	// ---------------------------------------------------------
	// setup blending params of the entities

	using enum ECS::RENDER_STATES;

	enttMgr.AddRenderStatesComponent(
		{ enttsNameToID.at("wireFence") },
		std::set<ECS::RENDER_STATES>{ ALPHA_CLIPPING, CULL_MODE_NONE });


	// ------------------------------------------
	// set BOUNDING for the entts

	DirectX::BoundingBox aabb;
	pMeshStorage->GetBoundingDataByID(cubeMeshID, aabb);

	enttMgr.AddBoundingComponent(
		enttsIDs,
		std::vector<DirectX::BoundingBox>(cubesCount, aabb),
		std::vector<ECS::BoundingType>(cubesCount, ECS::BoundingType::AABB));
}

///////////////////////////////////////////////////////////

void CreateTerrain(ID3D11Device* pDevice, ECS::EntityManager& entityMgr)
{
	//
	// create and setup terrain elements
	//

	Log::Debug();
	
	// create and setup a terrain grid mesh
	ModelsCreator modelCreator;
	MeshStorage* pMeshStorage = MeshStorage::Get();
	TextureManager* pTexMgr = TextureManager::Get();

	const UINT gridWidth = 500;
	const UINT gridDepth = 500;

	const MeshID terrainMeshID = modelCreator.CreateGeneratedTerrain(
		pDevice,
		gridWidth,
		gridDepth,
		gridWidth + 1,
		gridDepth + 1);

	// load and set a texture for the terrain mesh
	const TexPath dirt01diffTexPath = "data/textures/dirt01d.dds";
	TexID terrainTexID = pTexMgr->LoadFromFile(dirt01diffTexPath);
	pMeshStorage->SetTextureForMeshByID(terrainMeshID, aiTextureType_DIFFUSE,terrainTexID);

	// setup a transformation for the terrain's texture
	ECS::StaticTexTransParams terrainTexTransform;
	terrainTexTransform.Push(DirectX::XMMatrixScaling(50, 50, 0));

	// setup bounding data for the terrain
	DirectX::BoundingBox aabb;
	pMeshStorage->GetBoundingDataByID(terrainMeshID, aabb);
		
	// ------------------------------------------

	// create and setup a terrain entity
	EntityID terrainEnttID = entityMgr.CreateEntity();

	entityMgr.AddTransformComponent(terrainEnttID, { 0, 0, 0 });
	entityMgr.AddNameComponent(terrainEnttID, "terrain");
	entityMgr.AddMeshComponent(terrainEnttID, terrainMeshID);

	entityMgr.AddTextureTransformComponent(ECS::TexTransformType::STATIC, { terrainEnttID }, { terrainTexTransform });
	entityMgr.AddRenderingComponent({ terrainEnttID });
	entityMgr.AddBoundingComponent(terrainEnttID, aabb, ECS::BoundingType::AABB);
}

///////////////////////////////////////////////////////////

void CreateWater(ID3D11Device* pDevice, ECS::EntityManager& mgr)
{
	//
	// create and setup water model 
	//

	Log::Debug();

	try
	{

	// create and setup a water mesh
	ModelsCreator modelCreator;
	MeshStorage* pMeshStorage = MeshStorage::Get();
	TextureManager* pTexMgr = TextureManager::Get();

	const float waterWidth = 500;
	const float waterDepth = 500;

	const MeshID waterMeshID = modelCreator.CreatePlane(
		pDevice,
		waterWidth,
		waterDepth);

	// specify a material for the water
	const Mesh::Material mat(
		XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f),
		XMFLOAT4(0.137f, 0.42f, 0.556f, 0.5f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f),
		XMFLOAT4(.5f, .5f, .5f, 1));

	pMeshStorage->SetMaterialForMeshByID(waterMeshID, mat);

	const TexPath waterDiffTexPath = "data/textures/water2.dds";
	pTexMgr->LoadFromFile(waterDiffTexPath);

	pMeshStorage->SetTextureForMeshByID(
		waterMeshID,
		aiTextureType_DIFFUSE,
		pTexMgr->GetIDByName(waterDiffTexPath));

	// create and setup a water entity
	EntityID waterEnttID = mgr.CreateEntity();

	ECS::StaticTexTransParams waterTexTransform;

	waterTexTransform.Push(DirectX::XMMatrixScaling(50, 50, 0), DirectX::XMMatrixTranslation(0.1f, 0.1f, 0.0f));

	// ---------------------------------------------------------
	// setup blending params of the entities

	using enum ECS::RENDER_STATES;

	mgr.AddTransformComponent(waterEnttID, { 0, 0, 0 }, DirectX::XMQuaternionRotationRollPitchYaw(XM_PIDIV2, 0.0f, 0.0f));
	mgr.AddNameComponent(waterEnttID, "water");
	mgr.AddMeshComponent(waterEnttID, { waterMeshID });
	mgr.AddTextureTransformComponent(ECS::TexTransformType::STATIC, { waterEnttID }, { waterTexTransform });
	mgr.AddRenderingComponent({ waterEnttID });

	mgr.AddRenderStatesComponent({ waterEnttID }, std::set<ECS::RENDER_STATES>{ TRANSPARENCY });


	// set BOUNDING for the entt
	DirectX::BoundingBox aabb;
	pMeshStorage->GetBoundingDataByID(waterMeshID, aabb);
	mgr.AddBoundingComponent(waterEnttID, aabb, ECS::BoundingType::AABB);

	}
	catch (EngineException& e)
	{
		Log::Error(e);
		Log::Error("can't create a water entity");
	}
	catch (ECS::LIB_Exception& e)
	{
		Log::Error(e.GetStr());
		Log::Error("can't create a water entity");
	}
}

///////////////////////////////////////////////////////////

void CreateSkull(ID3D11Device* pDevice, ECS::EntityManager& mgr)
{
	// create and setup a skull mesh
	ModelsCreator modelCreator;
	MeshStorage* pMeshStorage = MeshStorage::Get();
	TextureManager* pTexMgr = TextureManager::Get();

	const MeshID skullMeshID = modelCreator.CreateSkull(pDevice);

	// specify a material for the skull
	Mesh::Material mat;
	mat.ambient_  = XMFLOAT4(1, 1, 1, 1);
	mat.diffuse_  = XMFLOAT4(1, 1, 1, 1);
	mat.specular_ = XMFLOAT4(0.8f, 0.8f, 0.8f, 128.0f);

	pMeshStorage->SetMaterialForMeshByID(skullMeshID, mat);


	// create and setup a water entity
	EntityID skullEnttID = mgr.CreateEntity();

	mgr.AddTransformComponent(skullEnttID, { 0, 4, 20 });
	mgr.AddNameComponent(skullEnttID, "skull");
	mgr.AddMeshComponent(skullEnttID, { skullMeshID });
	mgr.AddRenderingComponent({ skullEnttID });

}

///////////////////////////////////////////////////////////

void CreatePlanes(ID3D11Device* pDevice, ECS::EntityManager& mgr)
{
	Log::Debug();

	try 
	{

	const UINT planesCount = 2;
	const std::vector<EntityID> enttsIDs = mgr.CreateEntities(planesCount);

	// ---------------------------------------------------------
	// setup transform data for entities

	TransformData transform;

	transform.positions.resize(planesCount, { 0,0,0 });
	transform.dirQuats.resize(planesCount, { 0,0,0,1 });  // no rotation
	transform.uniformScales.resize(planesCount, 1.0f);

	// make two rows of the cylinders
	transform.positions[0] = { 0, 1.5f, 6 };
	transform.positions[1] = { 2, 1.5f, 6 };
	//transform.positions[2] = { 4, 1.5f, 6 };
	//transform.positions[3] = { 4, 1.5f, 6 };

	mgr.AddTransformComponent(
		enttsIDs,
		transform.positions,
		transform.dirQuats,
		transform.uniformScales);

	// ---------------------------------------------------------
	// add names for the entities

	std::vector<std::string> names;

	for (const EntityID id : enttsIDs)
		names.push_back("plane_" + std::to_string(id));

	mgr.AddNameComponent(enttsIDs, names);


	// ---------------------------------------------------------
	// setup meshes of the entities

	TextureManager* pTexMgr = TextureManager::Get();
	ModelsCreator modelCreator;
	MeshStorage* pMeshStorage = MeshStorage::Get();

	std::vector<MeshID> planesMeshesIDs;

	for (const EntityID id : enttsIDs)
	{
		MeshID meshID = modelCreator.CreatePlane(pDevice);
		planesMeshesIDs.push_back(meshID);
	}
		
	const std::vector<TexPath> texPaths =
	{
		"data/textures/brick01.dds",
		"data/textures/sprite01.tga",
	};

	// create textures
	for (const TexPath& path : texPaths)
		pTexMgr->LoadFromFile(path);

	for (u32 idx = 0; const TexPath& path : texPaths)
	{
		pMeshStorage->SetTextureForMeshByID(
			planesMeshesIDs[idx++],
			aiTextureType_DIFFUSE,
			pTexMgr->GetIDByName(path));
	}
	

	for (u32 idx = 0; idx < (u32)std::ssize(enttsIDs); ++idx)
		mgr.AddMeshComponent(enttsIDs[idx], {planesMeshesIDs[idx]});


	// ---------------------------------------------------------
	// setup rendering params of the entities

	mgr.AddRenderingComponent(enttsIDs);

	// ---------------------------------------------------------
	// setup render states of the entities
#if 1
	std::vector<std::set<ECS::RENDER_STATES>> renderStates =
	{
		{ECS::RENDER_STATES::ADDING},
		{ECS::RENDER_STATES::SUBTRACTING},
	};

	mgr.AddRenderStatesComponent(enttsIDs, renderStates);
#endif


	}
	catch (EngineException& e)
	{
		Log::Error(e);
		Log::Error("can't create plane entities");
	}
	catch (ECS::LIB_Exception& e)
	{
		Log::Error(e.GetStr());
		Log::Error("can't create plane entities");
	}
}

///////////////////////////////////////////////////////////

void CreateTrees(ID3D11Device* pDevice, ECS::EntityManager& mgr)
{
	// create and setup trees entities

	Log::Debug();

	

	const u32 treesCount = 30;
	const std::vector<EntityID> treesEnttIDs = mgr.CreateEntities(treesCount);

	//const std::string pathToModel = "data/models/tree2/source/HeroTree.fbx";
	//const std::string pathToModel = "data/models/trees/60-tree/Tree.blend";

	ModelsCreator modelCreator;
	MeshStorage* pMeshStorage = MeshStorage::Get();

	const std::string pathToModel = "data/models/trees/FBX format/conifer_macedonian_pine1.fbx";
	const std::vector<MeshID> treeMeshesIds = modelCreator.ImportFromFile(pDevice, pathToModel);


	std::vector<DirectX::XMFLOAT3> positions;

	positions.reserve(treesCount);

	// generate positions for the trees
	for (u32 idx = 0; idx < treesCount; ++idx)
	{
		float x = 0;
		float y = -1.0f;
		float z = 0;
		
		// to prevent tree be placed under the water
		while (y < 0.0f)
		{
			x = MathHelper::RandF(-100, 100);
			z = MathHelper::RandF(-100, 100);
			y = 0.1f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
		}

		positions.emplace_back(x, y, z);
	}

	
	const DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYawFromVector({ DirectX::XM_PIDIV2, 0, 0 });
	const float enttsUniformScale = 0.01f;

	mgr.AddTransformComponent(
		treesEnttIDs, 
		positions, 
		std::vector<DirectX::XMVECTOR>(treesCount, quat), 
		std::vector<float>(treesCount, enttsUniformScale));

	mgr.AddNameComponent(treesEnttIDs[0], "tree");
	mgr.AddMeshComponent(treesEnttIDs, treeMeshesIds);
	mgr.AddRenderingComponent(treesEnttIDs);

	

	// ------------------------------------------
	// set BOUNDING for the tree entts
	
	//std::vector<DirectX::BoundingBox> meshesBoundingData;
	//pMeshStorage->GetBoundingDataByIDs(treeMeshesIds, meshesBoundingData);


	DirectX::BoundingBox aabb;
	pMeshStorage->GetCommonBoundingBoxByIDs(treeMeshesIds, aabb);

	mgr.AddBoundingComponent(
		treesEnttIDs, 
		std::vector<DirectX::BoundingBox>(treesCount, aabb),
		std::vector<ECS::BoundingType>(treesCount, ECS::BoundingType::AABB));  

	// ---------------------------------------------------------
	// setup render states of the entities

	using enum ECS::RENDER_STATES;

	std::vector<std::set<ECS::RENDER_STATES>> renderStates(treesCount, { ALPHA_CLIPPING, CULL_MODE_NONE });

	mgr.AddRenderStatesComponent(treesEnttIDs, renderStates);
}

///////////////////////////////////////////////////////////

void CreateNanoSuit(ID3D11Device* pDevice, ECS::EntityManager& entityMgr)
{
	// create and setup a nanosuit entity

	Log::Debug();

	ModelsCreator modelCreator;
	const EntityID nanosuitEnttID = entityMgr.CreateEntity();

	const std::vector<MeshID> nanosuitMeshesIDs = modelCreator.ImportFromFile(pDevice, "data/models/nanosuit/nanosuit.obj");

	entityMgr.AddTransformComponent(nanosuitEnttID, { 10, 2, 8 }, {0,0,0,1}, {0.5f});
	entityMgr.AddNameComponent(nanosuitEnttID, "nanosuit");
	entityMgr.AddMeshComponent(nanosuitEnttID, nanosuitMeshesIDs);
	entityMgr.AddRenderingComponent({ nanosuitEnttID });

	DirectX::BoundingBox aabb;
	MeshStorage::Get()->GetCommonBoundingBoxByIDs(nanosuitMeshesIDs, aabb);

	entityMgr.AddBoundingComponent(nanosuitEnttID, aabb, ECS::BoundingType::AABB);
}

///////////////////////////////////////////////////////////

void CreateHouse(ID3D11Device* pDevice, ECS::EntityManager& entityMgr)
{
	// create and setup a nanosuit entity

	Log::Debug();

	ModelsCreator modelCreator;
	const EntityID enttID = entityMgr.CreateEntity();

	const std::string pathToModel = "data/models/stalker/stalker-house/source/SmallHouse.fbx";
	const std::vector<MeshID> meshID = modelCreator.ImportFromFile(pDevice, pathToModel);

	const DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYawFromVector({ DirectX::XM_PIDIV2, 0,0 });

	entityMgr.AddTransformComponent(enttID, { -10,3,-10 }, quat);
	entityMgr.AddNameComponent(enttID, "house");
	entityMgr.AddMeshComponent(enttID, meshID);
	entityMgr.AddRenderingComponent({ enttID });

	// set AABB
	DirectX::BoundingBox aabb;
	MeshStorage::Get()->GetCommonBoundingBoxByIDs(meshID, aabb);
	entityMgr.AddBoundingComponent(enttID, aabb, ECS::BoundingType::AABB);
}

///////////////////////////////////////////////////////////

void CreateHouse2(ID3D11Device* pDevice, ECS::EntityManager& entityMgr)
{
	// create and setup a nanosuit entity

	Log::Debug();

	ModelsCreator modelCreator;
	const EntityID enttID = entityMgr.CreateEntity();

	const std::string pathToModel = "data/models/stalker/abandoned-house-20/source/LittleHouse.fbx";
	const std::vector<MeshID> meshID = modelCreator.ImportFromFile(pDevice, pathToModel);

	const DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYawFromVector({ DirectX::XM_PIDIV2, 0,0 });

	entityMgr.AddTransformComponent(enttID, { 20,3,-10 }, quat, {0.01f});
	entityMgr.AddNameComponent(enttID, "blockpost");
	entityMgr.AddMeshComponent(enttID, meshID);
	entityMgr.AddRenderingComponent({ enttID });

	// set AABB
	DirectX::BoundingBox aabb;
	MeshStorage::Get()->GetCommonBoundingBoxByIDs(meshID, aabb);
	entityMgr.AddBoundingComponent(enttID, aabb, ECS::BoundingType::AABB);

}

///////////////////////////////////////////////////////////

bool InitializeGraphics::InitializeModels(
	ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	ECS::EntityManager& entityMgr,
	MeshStorage& meshStorage,
	Settings & settings,
	const float farZ)
{
	// initialize all the models on the scene

	Log::Print("---------------- INITIALIZATION: MODELS -----------------");

	Log::Debug();

	try
	{
		

		CreateTerrain(pDevice, entityMgr);
		CreateWater(pDevice, entityMgr);
		CreateSkull(pDevice, entityMgr);
		CreatePlanes(pDevice, entityMgr);

		CreateNanoSuit(pDevice, entityMgr);
		CreateHouse(pDevice, entityMgr);
		CreateHouse2(pDevice, entityMgr);

		CreateSpheres(pDevice, entityMgr);
		CreateCubes(pDevice, entityMgr);
		CreateCylinders(pDevice, entityMgr);
		CreateTrees(pDevice, entityMgr);
		
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(e.what());
		Log::Error("went out of range");
		return false;
	}
	catch (const std::bad_alloc & e)
	{
		Log::Error(e.what());
		Log::Error("can't allocate memory for some element");
		return false;
	}
	catch (EngineException & e)
	{
		Log::Error(e);
		Log::Error("can't initialize models");
		return false;
	}

	return true;
}

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeSprites(const UINT screenWidth,
	const UINT screenHeight)
{
	Log::Debug();

	const UINT crosshairWidth = 25;
	const UINT crosshairHeight = crosshairWidth;
	const char* animatedSpriteSetupFilename{ "data/models/sprite_data_01.txt" };
	const char* crosshairSpriteSetupFilename{ "data/models/sprite_crosshair.txt" };

	////////////////////////////////////////////////

#if 0

	// initialize an animated sprite
	pGameObj = pRenderableGameObjCreator_->Create2DSprite(animatedSpriteSetupFilename,
		"animated_sprite",
		{ 0, 500 },
		screenWidth, screenHeight);


	////////////////////////////////////////////////
	// compute a crosshair's center location
	POINT renderCrossAt{ screenWidth / 2 - crosshairWidth, screenHeight / 2 - crosshairHeight };

	// initialize a crosshair
	pGameObj = pRenderableGameObjCreator_->Create2DSprite(crosshairSpriteSetupFilename,
		"sprite_crosshair",
		renderCrossAt,
		screenWidth, screenHeight);
#endif

	return true;

}

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeLightSources(
	ECS::EntityManager& mgr,
	Settings & settings)
{
	// this function initializes all the light sources on the scene

	Log::Print("---------------- INITIALIZATION: LIGHT SOURCES -----------------");
	Log::Debug();

	const u32 numDirLights = settings.GetInt("NUM_DIRECTIONAL_LIGHTS");
	const u32 numPointLights = settings.GetInt("NUM_POINT_LIGHTS");
	const u32 numSpotLights = 1;

	// -----------------------------------------------------------------------------
	//                 DIRECTIONAL LIGHTS: SETUP AND CREATE
	// -----------------------------------------------------------------------------

	ECS::DirLightsInitParams dirLightsParams;

	dirLightsParams.ambients = 
	{
		{0.3f, 0.3f, 0.3f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};

	dirLightsParams.diffuses = 
	{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.2f, 0.2f, 0.2f, 1.0f},
		{0.2f, 0.2f, 0.2f, 1.0f}
	};

	dirLightsParams.speculars =
	{
		{0.5f, 0.5f, 0.5f, 1.0f},
		{0.25f, 0.25f, 0.25f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};

	dirLightsParams.directions =
	{
		{0.57735f, -0.9f, 0.57735f},
		{-0.57735f, -0.57735f, 0.57735f},
		{0.0f, -0.707f, -0.707f}
	};

	// create directional light entities and add a light component to them
	std::vector<EntityID> dirLightsIds = mgr.CreateEntities(numDirLights);
	mgr.AddLightComponent(dirLightsIds,	dirLightsParams);
	

	// -----------------------------------------------------------------------------
	//                    POINT LIGHTS: SETUP AND CREATE
	// -----------------------------------------------------------------------------

	ECS::PointLightsInitParams pointLightsParams;

	pointLightsParams.ambients.resize(numPointLights, { 0,0,0,1 });
	pointLightsParams.diffuses.resize(numPointLights, { 0.7f, 0.7f, 0.7f, 1.0f });
	pointLightsParams.speculars.resize(numPointLights, { 0.7f, 0.7f, 0.7f, 1.0f });
	pointLightsParams.attenuations.resize(numPointLights, { 0, 0.1f, 0 });
	pointLightsParams.ranges.resize(numPointLights, 40.0f);
	pointLightsParams.positions.reserve(numPointLights);

	// generate positions for the point light sources
	for (size_t idx = 0; idx < numPointLights; ++idx)
		pointLightsParams.positions.emplace_back(MathHelper::RandF(0, 30), 4.0f, MathHelper::RandF(0, 30));

	// create point light entities and add a light component to them
	std::vector<EntityID> pointLightsIds = mgr.CreateEntities(numPointLights);
	mgr.AddLightComponent(pointLightsIds, pointLightsParams);


	// -----------------------------------------------------------------------------
	//                   SPOT LIGHTS: SETUP AND CREATE
	// -----------------------------------------------------------------------------

	ECS::SpotLightsInitParams spotLightsParams;

	spotLightsParams.ambients.resize(numSpotLights, { 0.1f, 0.1f, 0.0f, 1.0f });
	spotLightsParams.diffuses.resize(numSpotLights, { 0.4f, 0.4f, 0.4f, 1.0f });
	spotLightsParams.speculars.resize(numSpotLights, { 0, 0, 0, 1 });

	spotLightsParams.positions.resize(numSpotLights, { 0, 0, 0 });
	spotLightsParams.directions.resize(numSpotLights, { 0, 0, 0 });
	spotLightsParams.attenuations.resize(numSpotLights, { 1.0f, 0.01f, 0.0f });

	spotLightsParams.ranges.resize(numSpotLights, 100);
	spotLightsParams.spotExponents.resize(numSpotLights, 96);

	// create spot light entities and add a light component to them
	std::vector<EntityID> spotLightsIds = mgr.CreateEntities(numSpotLights);

	mgr.AddLightComponent(spotLightsIds, spotLightsParams);
	mgr.AddNameComponent(spotLightsIds.front(), "flashlight");


	return true;
}

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeGUI(
	D3DClass & d3d, 
	UserInterfaceClass & UI,
	Settings & settings)
{
	// this function initializes the GUI of the game/engine (interface elements, text, etc.);

	Log::Print("---------------- INITIALIZATION: GUI -----------------------");
	Log::Debug();

	try
	{
		const UINT wndWidth  = settings.GetInt("WINDOW_WIDTH");
		const UINT wndHeight = settings.GetInt("WINDOW_HEIGHT");
		const std::string fontDataFilePath    = settings.GetString("FONT_DATA_FILE_PATH");
		const std::string fontTextureFilePath = settings.GetString("FONT_TEXTURE_FILE_PATH");

		std::string videoCardName{ "" };
		int videoCardMemory = 0;

		ID3D11Device* pDevice = nullptr;
		ID3D11DeviceContext* pDeviceContext = nullptr;


		d3d.GetDeviceAndDeviceContext(pDevice, pDeviceContext);
		d3d.GetVideoCardInfo(videoCardName, videoCardMemory);

		// initialize the user interface
		UI.Initialize(
			pDevice,
			pDeviceContext,
			fontDataFilePath,
			fontTextureFilePath,
			wndWidth,
			wndHeight,
			videoCardMemory,
			videoCardName);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(e.what());
		throw EngineException("can't allocate memory for GUI elements");
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		return false;
	}

	return true;
}
