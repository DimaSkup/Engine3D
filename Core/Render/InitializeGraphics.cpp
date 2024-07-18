// ************************************************************************************
// Filename:     InitializeGraphics.cpp
// Description:  there are functions for initialization of DirectX
//               and graphics parts of the engine;
//
// Created:      02.12.22
// ************************************************************************************
#include "InitializeGraphics.h"

#include "InitializeGraphicsHelper.h"
#include "InitGraphicsHelperDataTypes.h"


using namespace DirectX;
using namespace Mesh;


InitializeGraphics::InitializeGraphics()
{
	Log::Debug(LOG_MACRO);
}


// ************************************************************************************
//
//                                PUBLIC FUNCTIONS
//
// ************************************************************************************

bool InitializeGraphics::InitializeDirectX(
	D3DClass & d3d,
	HWND hwnd,
	const UINT windowWidth,
	const UINT windowHeight,
	const float nearZ,            // near Z-coordinate of the screen/frustum
	const float farZ,             // far Z-coordinate of the screen/frustum (screen depth)
	const bool vSyncEnabled,
	const bool isFullScreenMode,
	const bool enable4xMSAA)
{
	// THIS FUNC initializes the DirectX stuff 
	// (device, deviceContext, swapChain, rasterizerState, viewport, etc)

	try 
	{
		bool result = d3d.Initialize(hwnd,
			windowWidth,
			windowHeight,
			vSyncEnabled,
			isFullScreenMode,
			enable4xMSAA,
			nearZ,
			farZ);
		ASSERT_TRUE(result, "can't initialize the Direct3D");

		// setup the rasterizer state to default params
		d3d.SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);
		d3d.SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize DirectX");
		return false;
	}

	return true;
}

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	Shaders::ShadersContainer & shadersContainer)
{
	// THIS FUNC initializes all the shader classes (color, texture, light, etc.)
	// and the HLSL shaders as well

	Log::Print("---------------------------------------------------------");
	Log::Print("                INITIALIZATION: SHADERS                  ");
	Log::Print("---------------------------------------------------------");

	try
	{
		bool result = false;

		result = shadersContainer.colorShader_.Initialize(pDevice, pDeviceContext);
		ASSERT_TRUE(result, "can't initialize the color shader class");

		result = shadersContainer.textureShader_.Initialize(pDevice, pDeviceContext);
		ASSERT_TRUE(result, "can't initialize the texture shader class");

		result = shadersContainer.lightShader_.Initialize(pDevice, pDeviceContext);
		ASSERT_TRUE(result, "can't initialize the light shader class");
#if 0


		// NOTE: when you add a new shader class you have to include a header of this class
		//       into the ShadersContainer.h
		//
		// make shaders objects (later all the pointers will be stored in the shaders container)
		// so we don't need clear this vector with pointers
		pointersToShaders.push_back(new TextureShaderClass());
		pointersToShaders.push_back(new SpecularLightShaderClass());
		pointersToShaders.push_back(new LightShaderClass());
		pointersToShaders.push_back(new MultiTextureShaderClass());
		pointersToShaders.push_back(new AlphaMapShaderClass());
		pointersToShaders.push_back(new TerrainShaderClass());
		pointersToShaders.push_back(new SkyDomeShaderClass());
		pointersToShaders.push_back(new DepthShaderClass());
		pointersToShaders.push_back(new BumpMapShaderClass());
		pointersToShaders.push_back(new SkyPlaneShaderClass());
		pointersToShaders.push_back(new LightMapShaderClass());
		//pointersToShaders.push_back(new FontShaderClass());
		pointersToShaders.push_back(new PointLightShaderClass());
		pointersToShaders.push_back(new SpriteShaderClass());
		pointersToShaders.push_back(new ReflectionShaderClass());
		

		// add pairs [shader_name => shader_ptr] into the shaders container
		for (const auto & pShader : pointersToShaders)
		{
			graphics_->pShadersContainer_->SetShaderByName(pShader->GetShaderName(), pShader);
		}

		// go through each shader and initialize it
		for (auto & elem : graphics_->pShadersContainer_->GetShadersList())
		{
			result = elem.second->Initialize(pDevice, pDeviceContext, hwnd);
			ASSERT_TRUE(result, "can't initialize the " + elem.second->GetShaderName() + " object");

			// after the initialization we add this shader into the main model_to_shader mediator
			graphics_->pModelsToShaderMediator_->AddShader(elem.second);
		}

#endif

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		return false;
	}
	catch (EngineException & exception) // if we have some error during initialization of shaders we handle such an error here
	{
		Log::Error(exception, true);
		return false;
	}

	
	return true;
} 

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeScene(
	D3DClass & d3d,
	EntityManager & entityMgr,
	MeshStorage& meshStorage,
	LightStorage & lightStore,
	Settings & settings,
	RenderToTextureClass & renderToTexture,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	const float nearZ,               // near Z-coordinate of the frustum/camera
	const float farZ)                // far Z-coordinate of the frustum/camera
	
{
	// THIS FUNC initializes some main elements of the scene:
	// models, light sources, textures

	try
	{
		bool result = false;

		///////////////////////////////////////////////////
		//  CREATE AND INIT RELATED TO TEXTURES STUFF
		///////////////////////////////////////////////////

		// initialize the render to texture object
		result = renderToTexture.Initialize(pDevice, 256, 256, farZ, nearZ, 1);
		ASSERT_TRUE(result, "can't initialize the render to texture object");

		///////////////////////////////////////////////////
		//  CREATE AND INIT SCENE ELEMENTS
		///////////////////////////////////////////////////

		// initialize all the models on the scene
		result = InitializeModels(
			pDevice, 
			pDeviceContext, 
			entityMgr,
			meshStorage,
			settings, 
			farZ);
		ASSERT_TRUE(result, "can't initialize models");

		// initialize all the light sources on the scene
		result = InitializeLight(settings, lightStore);
		ASSERT_TRUE(result, "can't initialize light sources");

		Log::Print(LOG_MACRO, "is initialized");
	}

	catch (EngineException& exception)
	{
		Log::Error(exception, false);
		Log::Error(LOG_MACRO, "can't initialize the scene");

		return false;
	}


	return true;
} 

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeCameras(
	CameraClass & editorCamera,
	CameraClass & cameraForRenderToTexture,
	DirectX::XMMATRIX & baseViewMatrix,      // is used for 2D rendering
	const UINT windowWidth,
	const UINT windowHeight,
	const float nearZ,                       // near Z-coordinate of the frustum
	const float farZ,                        // far Z-coordinate of the frustum
	const float fovDegrees,                  // field of view
	const float cameraSpeed,                 // camera movement speed
	const float cameraSensitivity)           // camera rotation speed
{
	try
	{
		// calculate the aspect ratio
		const float aspectRatio = (float)windowWidth / (float)windowHeight;

		editorCamera.Initialize(cameraSpeed, cameraSensitivity);              // initialize the editor's camera object
		cameraForRenderToTexture.Initialize(cameraSpeed, cameraSensitivity);  // initialize the camera which is used for rendering into textures


		// setup the editor camera
		editorCamera.SetPosition({ 0.0f, 0.0f, -3.0f });
		editorCamera.SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);

		// initialize view matrices for the editor camera
		editorCamera.UpdateViewMatrix();
		baseViewMatrix = editorCamera.GetViewMatrix(); // initialize a base view matrix with the camera for 2D user interface rendering

		// setup the camera for rendering to textures
		cameraForRenderToTexture.SetPosition({ 0.0f, 0.0f, -5.0f });
		cameraForRenderToTexture.SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);
	}
	catch (EngineException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the cameras objects");
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
	wavesGeomParams.numRows = settings.GetSettingIntByKey("WAVES_NUM_ROWS");
	wavesGeomParams.numColumns = settings.GetSettingIntByKey("WAVES_NUM_COLUMNS");
	wavesGeomParams.spatialStep = settings.GetSettingFloatByKey("WAVES_SPATIAL_STEP");
	wavesGeomParams.timeStep = settings.GetSettingFloatByKey("WAVES_TIME_STEP");
	wavesGeomParams.speed = settings.GetSettingFloatByKey("WAVES_SPEED");
	wavesGeomParams.damping = settings.GetSettingFloatByKey("WAVES_DAMPING");

	// load params for cylinders
	cylGeomParams.height = settings.GetSettingFloatByKey("CYLINDER_HEIGHT");
	cylGeomParams.bottomRadius = settings.GetSettingFloatByKey("CYLINDER_BOTTOM_CAP_RADIUS");
	cylGeomParams.topRadius = settings.GetSettingFloatByKey("CYLINDER_TOP_CAP_RADIUS");
	cylGeomParams.sliceCount = settings.GetSettingIntByKey("CYLINDER_SLICE_COUNT");
	cylGeomParams.stackCount = settings.GetSettingIntByKey("CYLINDER_STACK_COUNT");

	// load params for spheres
	sphereGeomParams.radius = settings.GetSettingFloatByKey("SPHERE_RADIUS");
	sphereGeomParams.sliceCount = settings.GetSettingIntByKey("SPHERE_SLICE_COUNT");
	sphereGeomParams.stackCount = settings.GetSettingIntByKey("SPHERE_STACK_COUNT");

	// load params for geospheres
	geosphereGeomParams.radius = settings.GetSettingFloatByKey("GEOSPHERE_RADIUS");
	geosphereGeomParams.numSubdivisions = settings.GetSettingIntByKey("GEOSPHERE_NUM_SUBDIVISITIONS");

	// load params for pyramids
	pyramidGeomParams.height = settings.GetSettingFloatByKey("PYRAMID_HEIGHT");
	pyramidGeomParams.baseWidth = settings.GetSettingFloatByKey("PYRAMID_BASE_WIDTH");
	pyramidGeomParams.baseDepth = settings.GetSettingFloatByKey("PYRAMID_BASE_DEPTH");
}

///////////////////////////////////////////////////////////

void CreateSpheres(
	ID3D11Device* pDevice,
	EntityManager& entityMgr)
{
	//
	// create and setup spheres entities
	//

	Log::Debug(LOG_MACRO);

	const u32 spheresCount = 10;
	const std::vector<EntityID> enttsIDs = entityMgr.CreateEntities(spheresCount);

	// ---------------------------------------------------------
	// setup transform data for entities

	TransformData transform;

	transform.positions.reserve(spheresCount);
	transform.dirQuats.resize(spheresCount, DirectX::XMVectorZero());
	transform.uniformScales.resize(spheresCount, 1.0f);

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

#if 1
	MovementData movement;

	movement.translations.resize(spheresCount, { 0,0,0 });
	movement.rotQuats.resize(spheresCount, XMQuaternionRotationRollPitchYaw(0, 0.001f, 0));
	movement.uniformScales.resize(spheresCount, 1.0f);

	entityMgr.AddMoveComponent(
		enttsIDs,
		movement.translations,
		movement.rotQuats,
		movement.uniformScales);

	transform.Clear();
	movement.Clear();
#endif	

	// ---------------------------------------------------------
	// set names for the spheres

	std::vector<EntityName> enttsNames;

	for (size_t idx = 0; idx < spheresCount; ++idx)
		enttsNames.emplace_back("sphere_" + std::to_string(idx));

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
}

///////////////////////////////////////////////////////////

void CreateCylinders(ID3D11Device* pDevice, EntityManager& entityMgr)
{
	//
	// create and setup cylinders entities
	//

	Log::Debug(LOG_MACRO);

	const UINT cylindersCount = 10;
	const std::vector<EntityID> enttsIDs = entityMgr.CreateEntities(cylindersCount);

	// ---------------------------------------------------------
	// setup transform data for entities

	TransformData transform;

	transform.positions.reserve(cylindersCount);
	transform.dirQuats.resize(cylindersCount, DirectX::XMVectorZero());
	transform.uniformScales.resize(cylindersCount, 1.0f);

	for (size_t idx = 0; idx < cylindersCount / 2; ++idx)
	{
		transform.positions.emplace_back(-5.0f, 2.0f, 10.0f * idx);
		transform.positions.emplace_back(+5.0f, 2.0f, 10.0f * idx);
	}

	entityMgr.AddTransformComponent(
		enttsIDs,
		transform.positions,
		transform.dirQuats,
		transform.uniformScales);

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

	entityMgr.AddMeshComponent(enttsIDs, { cylinderMeshID });


	// ---------------------------------------------------------
	// setup rendering params of the entities

	entityMgr.AddRenderingComponent(enttsIDs);
}

///////////////////////////////////////////////////////////

void CreateCubes(ID3D11Device* pDevice, EntityManager& entityMgr)
{
	//
	// create and setup cubes entities
	//

	Log::Debug(LOG_MACRO);

	ModelsCreator modelCreator;
	TextureManager* pTexMgr = TextureManager::Get();
	MeshStorage* pMeshStorage = MeshStorage::Get();

	const size_t cubesCount = 2;
	const std::vector<EntityID> enttsIDs = entityMgr.CreateEntities(cubesCount);
	const EntityID catEnttID = enttsIDs.front();
	const EntityID fireflameEnttID = enttsIDs.back();

	// ---------------------------------------------------------
	// setup textures for the cubes


	
	const TexPath texPathCat = "data/textures/cat.dds";
	const TexPath texPathFireAtlas = "data/textures/fire_atlas.dds";
	const TexPath texPathLightMap = "data/textures/lightmap.dds";    // default lightmap

	pTexMgr->LoadFromFile(texPathCat);
	pTexMgr->LoadFromFile(texPathFireAtlas);
	pTexMgr->LoadFromFile(texPathLightMap);

	// get IDs of textures
	std::vector<TexID> tempTexIDs;
	pTexMgr->GetIDsByNames({ "unloaded", texPathFireAtlas, texPathLightMap }, tempTexIDs);
	const TexID unloadedTexID = tempTexIDs[0];

	// fill in textures arrays (IDs/paths) with default values of the unloaded texture
	std::vector<TexID> fireflameTexIDs(TextureClass::TEXTURE_TYPE_COUNT, unloadedTexID);
	std::vector<TexPath> fireflameTexPaths(TextureClass::TEXTURE_TYPE_COUNT, "unloaded");

	// setup some texture types
	fireflameTexPaths[aiTextureType_DIFFUSE] = texPathFireAtlas;
	fireflameTexPaths[aiTextureType_LIGHTMAP] = texPathLightMap;

	fireflameTexIDs[aiTextureType_DIFFUSE] = tempTexIDs[1];
	fireflameTexIDs[aiTextureType_LIGHTMAP] = tempTexIDs[2];

	// ---------------------------------------------------------
	// create and setup a cube mesh for entities

	const MeshID cubeMeshID = modelCreator.CreateCube(pDevice);

	pMeshStorage->SetTextureForMeshByID(
		cubeMeshID, 
		aiTextureType_DIFFUSE, 
		pTexMgr->GetIDByName("data/textures/cat.dds"));


	// ---------------------------------------------------------
	// prepare transformations

	const std::vector<XMVECTOR> dirQuats(cubesCount, DirectX::XMVectorZero());
	const std::vector<float> uniformScales(cubesCount, 1.0f);
	std::vector<XMFLOAT3> positions;

	positions.reserve(cubesCount);

	for (size_t idx = 0; idx < cubesCount; ++idx)
		positions.emplace_back(0.0f, 1.0f, 2.0f * idx);


	// ---------------------------------------------------------
	// setup the cubes entities

	entityMgr.AddTransformComponent(enttsIDs, positions, dirQuats, uniformScales);
	entityMgr.AddNameComponent(enttsIDs, { "cat", "fireflame"});
	entityMgr.AddMeshComponent(enttsIDs, std::vector(cubesCount, cubeMeshID));
	entityMgr.AddTexturedComponent(fireflameEnttID, fireflameTexIDs, fireflameTexPaths);
	entityMgr.AddTextureTransformComponent(fireflameEnttID, 15, 8, 4);
	entityMgr.AddTextureTransformComponentRotationAroundTexCoord(catEnttID, 0.5f, 0.5f, 0.1f);
	entityMgr.AddRenderingComponent(enttsIDs);

}

///////////////////////////////////////////////////////////


void CreateTerrain(ID3D11Device* pDevice, EntityManager& entityMgr)
{
	//
	// create and setup terrain elements
	//

	Log::Debug(LOG_MACRO);
	
	// create and setup a terrain grid mesh
	ModelsCreator modelCreator;
	MeshStorage* pMeshStorage = MeshStorage::Get();
	TextureManager* pTexMgr = TextureManager::Get();

	const UINT gridWidth = 100;
	const UINT gridHeight = 100;
	const MeshID terrainMeshID = modelCreator.CreateGrid(pDevice, gridWidth, gridHeight);
	const TexPath dirt01diffTexPath = "data/textures/dirt01d.dds";
	
	pTexMgr->LoadFromFile(dirt01diffTexPath);

	pMeshStorage->SetTextureForMeshByID(
		terrainMeshID, 
		aiTextureType_DIFFUSE,
		pTexMgr->GetIDByName(dirt01diffTexPath));
		//pTexMgr->LoadTextureFromFile("data/textures/fire_atlas.dds", aiTextureType_DIFFUSE));
		
	// create and setup a terrain entity
	EntityID terrainEnttID = entityMgr.CreateEntity();

	entityMgr.AddTransformComponent(terrainEnttID, { 0, 0, 0 });
	entityMgr.AddNameComponent(terrainEnttID, "terrain");
	entityMgr.AddMeshComponent(terrainEnttID, { terrainMeshID });
	entityMgr.AddTextureTransformComponent({ terrainEnttID }, { DirectX::XMMatrixScaling(5, 5, 0) });
	entityMgr.AddRenderingComponent({ terrainEnttID });
}

///////////////////////////////////////////////////////////

void CreateNanoSuit(ID3D11Device* pDevice, EntityManager& entityMgr)
{
	// create and setup a nanosuit entity

	Log::Debug(LOG_MACRO);

	ModelsCreator modelCreator;
	const EntityID nanosuitEnttID = entityMgr.CreateEntity();

	const std::vector<MeshID> nanosuitMeshesIDs = modelCreator.ImportFromFile(pDevice, "data/models/nanosuit/nanosuit.obj");

	entityMgr.AddTransformComponent(nanosuitEnttID, { 10, 0, 0 });
	entityMgr.AddMeshComponent(nanosuitEnttID, nanosuitMeshesIDs);
	entityMgr.AddRenderingComponent({ nanosuitEnttID });
}

///////////////////////////////////////////////////////////

bool InitializeGraphics::InitializeModels(
	ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	EntityManager& entityMgr,
	MeshStorage& meshStorage,
	Settings & settings,
	const float farZ)
{
	// initialize all the models on the scene

	Log::Print("---------------- INITIALIZATION: MODELS -----------------");

	Log::Debug(LOG_MACRO);

	try
	{
		CreateNanoSuit(pDevice, entityMgr);
		CreateSpheres(pDevice, entityMgr);
		CreateCylinders(pDevice, entityMgr);
		CreateCubes(pDevice, entityMgr);
		CreateSpheres(pDevice, entityMgr);
		CreateTerrain(pDevice, entityMgr);
	}
	catch (const std::out_of_range& e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "went out of range");
		return false;
	}
	catch (const std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't allocate memory for some element");
		return false;
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't initialize models");
		return false;
	}

	return true;
}

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeSprites(const UINT screenWidth,
	const UINT screenHeight)
{
	Log::Debug(LOG_MACRO);

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

} // end InitializeSprites

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeLight(
	Settings & settings,
	LightStorage & lightStore)
{
	// this function initializes all the light sources on the scene

	Log::Print("---------------- INITIALIZATION: LIGHT SOURCES -----------------");
	Log::Debug(LOG_MACRO);

	const UINT numOfDirLights = settings.GetSettingIntByKey("NUM_DIRECTIONAL_LIGHTS");
	const UINT numPointLights = settings.GetSettingIntByKey("NUM_POINT_LIGHTS");

	// -----------------------------------------------------------------------------
	//             SETUP INITIAL DATA FOR DIRECTIONAL LIGHT SOURCES
	// -----------------------------------------------------------------------------

	const DirectX::XMFLOAT4 ambients[3]
	{
		{0.3f, 0.3f, 0.3f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};

	const DirectX::XMFLOAT4 diffuses[3]
	{
		{0.5f, 0.5f, 0.5f, 1.0f},
		{0.2f, 0.2f, 0.2f, 1.0f},
		{0.2f, 0.2f, 0.2f, 1.0f}
	};

	const DirectX::XMFLOAT4 speculars[3]
	{
		{0.5f, 0.5f, 0.5f, 1.0f},
		{0.25f, 0.25f, 0.25f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};

	const DirectX::XMFLOAT3 directions[3]
	{
		{0.57735f, -0.57735f, 0.57735f},
		{-0.57735f, -0.57735f, 0.57735f},
		{0.0f, -0.707f, -0.707f}
	};

	// -----------------------------------------------------------------------------
	//                    CREATE DIRECTIONAL LIGHT SOURCES
	// -----------------------------------------------------------------------------

	for (UINT idx = 0; idx < numOfDirLights; ++idx)
	{
		lightStore.CreateNewDirectionalLight(
			ambients[idx],
			diffuses[idx],
			speculars[idx],
			directions[idx]);
	}
	

	// -----------------------------------------------------------------------------
	//                        CREATE POINT LIGHT SOURCES
	// -----------------------------------------------------------------------------

	// create POINT lights with random colours
	for (size_t idx = 0; idx < numPointLights; ++idx)
	{
		const DirectX::XMFLOAT4 ambient(0.3f, 0.3f, 0.3f, 1.0f);
		const DirectX::XMFLOAT4 diffuse(0.7f, 0.7f, 0.7f, 1.0f);
		const DirectX::XMFLOAT4 specular(0.7f, 0.7f, 0.7f, 1.0f);
		const DirectX::XMFLOAT3 attenutation(0.0f, 0.0f, 0.01f);
		const float range = 50.0f;

		// point light--position is change every frame to animate in Update function
		lightStore.CreateNewPointLight(
			ambient,
			diffuse,
			specular,
			{ 0, 0, 0 },  // point light--position is changed every frame to animate in Update function
			range,
			attenutation);
	}


	// -----------------------------------------------------------------------------
	//                   SETUP AND CREATE SPOTLIGHT SOURCES
	// -----------------------------------------------------------------------------

	const DirectX::XMFLOAT4 spot_ambient(0.1f, 0.1f, 0.0f, 1.0f);   // gives no ambienty
	const DirectX::XMFLOAT4 spot_diffuse(1.0f, 1.0f, 0.0f, 1.0f);   // yellow 
	const DirectX::XMFLOAT4 spot_specular(1.0f, 1.0f, 0.0f, 1.0f);  // gives white specular
	const DirectX::XMFLOAT3 spot_attenuation(0.0f, 0.1f, 0.0f);     
	const float spot_spot = 10.0f;
	const float spot_range = 300.0f;

	lightStore.CreateNewSpotLight(
		spot_ambient,
		spot_diffuse,
		spot_specular,
		{ 0,0,0 },        // spot light--position and direction changed every frame to animate in Update function
		spot_range,
		{ 0,0,0 },
		spot_spot,
		spot_attenuation);

	return true;
}

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeGUI(D3DClass & d3d, 
	UserInterfaceClass & UI,
	Settings & settings,
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const UINT windowWidth,
	const UINT windowHeight)
{
	// this function initializes the GUI of the game/engine (interface elements, text, etc.);

	Log::Print("---------------- INITIALIZATION: GUI -----------------------");
	Log::Debug(LOG_MACRO);

	try
	{
		const std::string fontDataFilePath    = settings.GetSettingStrByKey("FONT_DATA_FILE_PATH");
		const std::string fontTextureFilePath = settings.GetSettingStrByKey("FONT_TEXTURE_FILE_PATH");

		std::string videoCardName{ "" };
		int videoCardMemory = 0;

		// get an information about the video card
		d3d.GetVideoCardInfo(videoCardName, videoCardMemory);

		// initialize the user interface
		UI.Initialize(pDevice,
			pDeviceContext,
			fontDataFilePath,
			fontTextureFilePath,
			windowWidth,
			windowHeight,
			videoCardMemory,
			videoCardName);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		ASSERT_TRUE(false, "can't allocate memory for GUI elements");
	}
	catch (EngineException & e)
	{
		Log::Error(e, false);
		return false;
	}

	return true;

}
