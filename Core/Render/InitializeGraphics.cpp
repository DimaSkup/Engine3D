// ************************************************************************************
// Filename:     InitializeGraphics.cpp
// Description:  there are functions for initialization of DirectX
//               and graphics parts of the engine;
//
// Created:      02.12.22
// ************************************************************************************
#include "InitializeGraphics.h"

//#include "../GameObjects/ModelLoader.h"
//#include "../GameObjects/TerrainInitializer.h"
//#include "../GameObjects/GeometryGenerator.h"

#include "../ImageReaders/ImageReader.h"               // for reading images data
#include "../Common/MathHelper.h"

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
	LightStore & lightStore,
	Settings & settings,
	FrustumClass & editorFrustum,
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

		// initialize the editor frustum object
		editorFrustum.Initialize(farZ);

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
		if (!InitializeModels(
			pDevice, 
			pDeviceContext, 
			entityMgr,
			meshStorage,
			settings, 
			farZ))           
			return false;

		// initialize all the light sources on the scene
		if (!InitializeLight(settings, lightStore))
			return false;

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
	const UINT spheresCount = 10;

	// create and setup entities
	const std::vector<EntityID> createdEnttsIDs = entityMgr.CreateEntities(spheresCount);

	TransformData transform;
	MovementData movement;

	// ---------------------------------------------------------
	// setup transform data for spheres

	transform.positions.reserve(spheresCount);
	transform.directions.resize(spheresCount, { 0,0,0 });
	transform.scales.resize(spheresCount, { 1,1,1 });

	for (size_t idx = 0; idx < spheresCount/2; ++idx)
	{
		transform.positions.push_back(XMFLOAT3(-5, 5, 10.0f * idx));
		transform.positions.push_back(XMFLOAT3(5, 5, 10.0f * idx));
	}
		
	entityMgr.AddTransformComponent(
		createdEnttsIDs,
		transform.positions,
		transform.directions,
		transform.scales);

	// ---------------------------------------------------------
	// setup movement for the spheres

	// precompute rotation quats for each sphere and store it as XMFLOAT4
	XMFLOAT4 rotQuat;
	DirectX::XMStoreFloat4(&rotQuat, XMQuaternionRotationRollPitchYaw(0, 0.001f, 0));

	// movement data for the Move component
	movement.translations.resize(spheresCount, { 0,0,0 });
	movement.rotQuats.resize(spheresCount, rotQuat);
	movement.scaleChanges.resize(spheresCount, { 1,1,1 });


	entityMgr.AddMoveComponent(
		createdEnttsIDs,
		movement.translations,
		movement.rotQuats,
		movement.scaleChanges);

	transform.Clear();
	movement.Clear();
	
	// ---------------------------------------------------------
	// set names for the spheres
	std::vector<EntityName> enttsNames;

	for (size_t idx = 0; idx < spheresCount; ++idx)
		enttsNames.emplace_back("sphere_" + std::to_string(idx));

	entityMgr.AddNameComponent(createdEnttsIDs, enttsNames);


	// ---------------------------------------------------------
	// setup meshes of the spheres entities

	// create a sphere mesh and setup its textures
	ModelsCreator modelCreator;
	const MeshID sphereMeshID = modelCreator.Create(pDevice, Mesh::MeshType::Sphere);
	TextureClass* pCatTextureDiffuse = TextureManagerClass::Get()->LoadTextureFromFile("data/textures/cat.dds");
	MeshStorage::Get()->SetTextureForMeshByID(sphereMeshID, aiTextureType_DIFFUSE, pCatTextureDiffuse);

	entityMgr.AddMeshComponent(createdEnttsIDs, { sphereMeshID });

	// ---------------------------------------------------------
	// setup rendering params of the entities

	entityMgr.AddRenderingComponent(
		createdEnttsIDs,
		std::vector(createdEnttsIDs.size(), ECS::RENDERING_SHADERS::LIGHT_SHADER),
		std::vector(createdEnttsIDs.size(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
}

///////////////////////////////////////////////////////////

void CreateNanoSuit(ID3D11Device* pDevice, EntityManager& entityMgr)
{
	// create and setup a nanosuit entity

	ModelsCreator modelCreator;
	const EntityID nanosuitEnttID = entityMgr.CreateEntity();

	const std::vector<MeshID> nanosuitMeshesIDs = modelCreator.ImportFromFile(pDevice, "data/models/nanosuit/nanosuit.obj");

	entityMgr.AddTransformComponent(nanosuitEnttID, { 10, 0, 0 });
	entityMgr.AddMeshComponent(nanosuitEnttID, nanosuitMeshesIDs);
	
	entityMgr.AddRenderingComponent(nanosuitEnttID, ECS::RENDERING_SHADERS::TEXTURE_SHADER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

		// ---------------------------------------------------

		CreateNanoSuit(pDevice, entityMgr);
		CreateSpheres(pDevice, entityMgr);
	
	
	
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
	LightStore & lightStore)
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
