// ************************************************************************************
// Filename:     InitializeGraphics.cpp
// Description:  there are functions for initialization of DirectX
//               and graphics parts of the engine;
//
// Created:      02.12.22
// ************************************************************************************
#include "InitializeGraphics.h"

#include "../GameObjects/ModelInitializer.h"
#include "../GameObjects/TerrainInitializer.h"
#include "../GameObjects/GeometryGenerator.h"

#include "../ImageReaders/ImageReader.h"               // for reading images data
#include "../Common/MathHelper.h"

#include "InitializeGraphicsHelper.h"


using namespace DirectX;



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
		COM_ERROR_IF_FALSE(result, "can't initialize the Direct3D");

		// setup the rasterizer state to default params
		d3d.SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);
		d3d.SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
	}
	catch (COMException & e)
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
		COM_ERROR_IF_FALSE(result, "can't initialize the color shader class");

		result = shadersContainer.textureShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the texture shader class");

		result = shadersContainer.lightShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the light shader class");
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
			COM_ERROR_IF_FALSE(result, "can't initialize the " + elem.second->GetShaderName() + " object");

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
	catch (COMException & exception) // if we have some error during initialization of shaders we handle such an error here
	{
		Log::Error(exception, true);
		return false;
	}

	
	return true;
} 

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeScene(
	D3DClass & d3d,
	ModelsStore & modelsStore,
	LightStore & lightStore,
	Settings & settings,
	FrustumClass & editorFrustum,
	TextureManagerClass & textureManager,
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

		// initialize the textures manager
		textureManager.Initialize(pDevice);

		// initialize the render to texture object
		result = renderToTexture.Initialize(pDevice, 256, 256, farZ, nearZ, 1);
		COM_ERROR_IF_FALSE(result, "can't initialize the render to texture object");

		///////////////////////////////////////////////////
		//  CREATE AND INIT SCENE ELEMENTS
		///////////////////////////////////////////////////

		// initialize all the models on the scene
		if (!InitializeModels(pDevice, pDeviceContext, modelsStore, settings, farZ))           
			return false;

		// initialize all the light sources on the scene
		if (!InitializeLight(settings, lightStore))
			return false;

		Log::Print(LOG_MACRO, "is initialized");
	}

	catch (COMException& exception)
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
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize the cameras objects");
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////

bool InitializeGraphics::InitializeModels(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	ModelsStore & modelsStore,
	Settings & settings,
	const float farZ)
{
	// // initialize all the models on the scene

	Log::Print("---------------- INITIALIZATION: MODELS -----------------");

	Log::Debug(LOG_MACRO);

	try
	{
		ModelsCreator modelsCreator;
		GeometryGenerator geoGen;

		// create structure objects which will contain params of some geometry objects
		ModelsCreator::WAVES_PARAMS wavesParams;
		ModelsCreator::CYLINDER_PARAMS cylParams;
		ModelsCreator::SPHERE_PARAMS sphereParams;
		ModelsCreator::GEOSPHERE_PARAMS geosphereParams;
		ModelsCreator::PYRAMID_PARAMS pyramidParams;

		// --------------------------------------------------- //

		// first of all we have to initialize the models store
		modelsStore.Initialize(settings);

		// --------------------------------------------------- //

		// define how many models we want to create
		const UINT numOfCubes = settings.GetSettingIntByKey("CUBES_NUMBER");
		const UINT numOfCylinders = settings.GetSettingIntByKey("CYLINDERS_NUMBER");
		const UINT numOfSpheres = settings.GetSettingIntByKey("SPHERES_NUMBER");
		const UINT numOfGeospheres = settings.GetSettingIntByKey("GEOSPHERES_NUMBER");
		const UINT chunkDimension = settings.GetSettingIntByKey("CHUNK_DIMENSION");
		const UINT isCreateChunkBoundingBoxes = settings.GetSettingBoolByKey("CREATE_CHUNK_BOUNDING_BOXES");

		// define shader types for each model type
		ModelsStore::RENDERING_SHADERS spheresRenderingShader = ModelsStore::RENDERING_SHADERS::LIGHT_SHADER;
		ModelsStore::RENDERING_SHADERS cylindersRenderingShader = ModelsStore::RENDERING_SHADERS::LIGHT_SHADER;
		ModelsStore::RENDERING_SHADERS cubesRenderingShader = ModelsStore::RENDERING_SHADERS::LIGHT_SHADER;
		ModelsStore::RENDERING_SHADERS pyramidRenderingShader = ModelsStore::RENDERING_SHADERS::LIGHT_SHADER;
		ModelsStore::RENDERING_SHADERS terrainRenderingShader = ModelsStore::RENDERING_SHADERS::LIGHT_SHADER;
		ModelsStore::RENDERING_SHADERS gridRenderingShader = ModelsStore::RENDERING_SHADERS::LIGHT_SHADER;
		ModelsStore::RENDERING_SHADERS wavesRenderingShader = ModelsStore::RENDERING_SHADERS::LIGHT_SHADER;

		// load params for initialization of different model types
		modelsCreator.LoadParamsForDefaultModels(settings, wavesParams, cylParams, sphereParams, geosphereParams, pyramidParams);

#if 0
		// LAND MATERIAL
		mat.ambient = DirectX::XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		mat.diffuse = DirectX::XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		mat.specular = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

		
#endif


		// --------------------------------------------------- //

		// CREATE CUBES
		CreateCubes(pDevice, 
			settings,
			modelsCreator, 
			modelsStore,
			cubesRenderingShader,
			numOfCubes);

		// CREATE SPHERES
		CreateSpheres(pDevice, 
			modelsStore, 
			modelsCreator,
			sphereParams,
			spheresRenderingShader, 
			numOfSpheres);

		// CREATE PYRAMID
		CreatePyramids(pDevice, modelsCreator, modelsStore, pyramidParams, pyramidRenderingShader);

#if 0
		// CREATE PLAIN GRID
		const UINT gridIdx = modelsCreator.CreateGridMesh(pDevice, modelsStore, 20, 20);

		// setup the grid
		modelsStore.SetTextureByIndex(gridIdx, "data/textures/dirt01.dds", aiTextureType_DIFFUSE);
		modelsStore.SetRenderingShaderForVertexBufferByIdx(modelsStore.GetRelatedVertexBufferByModelIdx(gridIdx), gridRenderingShader);
#endif

		// CREATE TERRAIN GRID
		//CreateTerrain(pDevice, settings, modelsCreator, modelsStore, terrainRenderingShader);

		// CREATE CYLINDERS
		CreateCylinders(
			pDevice, 
			modelsStore, 
			modelsCreator, 
			cylParams, 
			cylindersRenderingShader,
			numOfCylinders);

		// CREATE WAVES
		//CreateWaves(pDevice, modelsStore, modelsCreator, wavesParams, wavesRenderingShader);

		// CREATE CHUNK BOUNDING BOX
		if (isCreateChunkBoundingBoxes)
			CreateChunkBoundingBoxes(pDevice, modelsCreator, modelsStore, chunkDimension);

		// CREATE GEOSPHERES
		CreateGeospheres(pDevice, modelsCreator, modelsStore, numOfGeospheres, {});

		// CREATE PLANES
		//modelsCreator.CreatePlane(pDevice, models_, { 0,0,0 }, { 0,0,0 });

		// CREATE AXIS
		CreateAxis(pDevice, modelsCreator, modelsStore);

		// CREATE EDITOR GRID
		//CreateEditorGrid(pDevice, settings, modelsCreator, modelsStore);

		// COMPUTE CHUNKS TO MODELS RELATIONS
		ComputeChunksToModels(modelsStore);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't allocate memory for some element");
		return false;
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		Log::Error(LOG_MACRO, "can't initialize models");
		return false;
	}

	return true;
} // end InitializeModels

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

		// point light--position is change every frame to animate in UpdateScene function
		lightStore.CreateNewPointLight(
			ambient,
			diffuse,
			specular,
			{ 0, 0, 0 },  // point light--position is changed every frame to animate in UpdateScene function
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
		{ 0,0,0 },        // spot light--position and direction changed every frame to animate in UpdateScene function
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
		COM_ERROR_IF_FALSE(false, "can't allocate memory for GUI elements");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		return false;
	}

	return true;

} // InitializeGUI
