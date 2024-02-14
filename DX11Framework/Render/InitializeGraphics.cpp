////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     InitializeGraphics.cpp
// Description:  there are functions for initialization of DirectX
//               and graphics parts of the engine;
//
// Created:      02.12.22
////////////////////////////////////////////////////////////////////////////////////////////
#include "InitializeGraphics.h"


#include "../GameObjects/ModelInitializer.h"


// includes all of the shaders (are used for initialization of these shaders and 
// set them into the shaders_container)

#include "../ShaderClass/TerrainShaderClass.h"         // for rendering the terrain 
#include "../ShaderClass/SpecularLightShaderClass.h"   // for light effect with specular
#include "../ShaderClass/MultiTextureShaderClass.h"    // for multitexturing
#include "../ShaderClass/LightMapShaderClass.h"        // for light mapping
#include "../ShaderClass/AlphaMapShaderClass.h"        // for alpha mapping
#include "../ShaderClass/BumpMapShaderClass.h"         // for bump mapping
#include "../ShaderClass/SkyDomeShaderClass.h"         // for rendering the sky dome
#include "../ShaderClass/SkyPlaneShaderClass.h"        // for rendering the sky plane
#include "../ShaderClass/DepthShaderClass.h"           // for coloring objects according to its depth position

#include "../ShaderClass/SpriteShaderClass.h"          // for rendering 2D sprites
#include "../ShaderClass/ReflectionShaderClass.h"      // for rendering planar reflection



InitializeGraphics::InitializeGraphics(GraphicsClass* pGraphics)
{
	Log::Debug(LOG_MACRO);

	// check input params
	assert(pGraphics != nullptr);
	
	try
	{
		// as we will use these pointers too often during initialization we make
		// local copies of it
		pGraphics_ = pGraphics;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't allocate memory for members of the class");
	}
}




////////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


bool InitializeGraphics::InitializeDirectX(HWND hwnd,
	const UINT windowWidth,
	const UINT windowHeight,
	const float nearZ,            // near Z-coordinate of the screen/frustum
	const float farZ,             // far Z-coordinate of the screen/frustum (screen depth)
	const bool vSyncEnabled,
	const bool isFullScreenMode)
{
	// this function initializes the DirectX stuff 
	// (device, deviceContext, swapChain, rasterizerState, viewport, etc)

	try 
	{
		bool result = pGraphics_->d3d_.Initialize(hwnd,
			windowWidth,
			windowHeight,
			vSyncEnabled,
			isFullScreenMode,
			nearZ,
			farZ);
		COM_ERROR_IF_FALSE(result, "can't initialize the Direct3D");

		// setup the rasterizer state
		pGraphics_->d3d_.SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);
		pGraphics_->d3d_.SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't initialize DirectX");
		return false;
	}

	return true;
} // end InitializeDirectX

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeShaders(ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd)
{
	// this function initializes all the shader classes (color, texture, light, etc.)
	// and the HLSL shaders as well

	Log::Print("---------------------------------------------------------");
	Log::Print("                INITIALIZATION: SHADERS                  ");
	Log::Print("---------------------------------------------------------");

	try
	{
		bool result = false;

		result = pGraphics_->colorShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the color shader class");

		result = pGraphics_->textureShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the texture shader class");

		result = pGraphics_->lightShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the light shader class");

		result = pGraphics_->pointLightShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the point light shader class");
	


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
}  // end InitializeShaders

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeScene(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	HWND hwnd,
	ModelsStore & modelsStore,
	Settings & settings,
	const UINT windowWidth,
	const UINT windowHeight,
	const float nearZ,        // near Z-coordinate of the frustum
	const float farZ,         // far Z-coordinate of the frustum
	const float fovDegrees)   // field of view
{
	// this function initializes all the scene elements

	try
	{
		bool result = false;

		///////////////////////////////////////////////////
		//  SETUP CAMERAS AND VIEW MATRICES
		///////////////////////////////////////////////////

		// calculate the aspect ratio
		const float aspectRatio = (float)windowWidth / (float)windowHeight;


		CameraClass* pCamera = pGraphics_->GetCamera();

		// setup the EditorCamera object
		pCamera->SetPosition({ 0.0f, 0.0f, -3.0f });
		pCamera->SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);

		// setup the camera for rendering to textures
		pGraphics_->pCameraForRenderToTexture_->SetPosition({ 0.0f, 0.0f, -5.0f });
		pGraphics_->pCameraForRenderToTexture_->SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);

		// initialize view matrices
		pCamera->UpdateViewMatrix();
		pGraphics_->baseViewMatrix_ = pCamera->GetViewMatrix(); // initialize a base view matrix with the camera for 2D user interface rendering
		pGraphics_->viewMatrix_ = pGraphics_->baseViewMatrix_;                  


		///////////////////////////////////////////////////
		//  CREATE AND INIT RELATED TO TEXTURES STUFF
		///////////////////////////////////////////////////

		// initialize a textures manager
		pGraphics_->pTextureManager_ = std::make_unique<TextureManagerClass>(pDevice);

		// initialize the render to texture object
		result = pGraphics_->pRenderToTexture_->Initialize(pDevice, 256, 256, farZ, nearZ, 1);
		COM_ERROR_IF_FALSE(result, "can't initialize the render to texture object");



		///////////////////////////////////////////////////
		//  CREATE AND INIT SCENE ELEMENTS
		///////////////////////////////////////////////////

		// initialize all the models on the scene
		if (!InitializeModels(pDevice, pDeviceContext, modelsStore, settings, farZ))           
			return false;

		// initialize all the light sources on the scene
		if (!InitializeLight(settings))
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
} // end InitializeScene

/////////////////////////////////////////////////

void PrepareRandomPositionRotationForModelsToInit(
	const UINT numOfModels,
	std::vector<DirectX::XMVECTOR> & randPositions,
	std::vector<DirectX::XMVECTOR> & randRotations)
{
	// GENERATE random positions and rotations data for the models

	assert(randPositions.size() == numOfModels);
	assert(randRotations.size() == numOfModels);


	const int range = 100;
	for (UINT idx = 0; idx < numOfModels; ++idx)
	{
		const float randX_pos = static_cast<float>(rand() % range);
		const float randY_pos = static_cast<float>(rand() % range);
		const float randZ_pos = static_cast<float>(rand() % range);

		const float randX_rot = static_cast<float>((rand() % 100) * 0.01f);
		const float randY_rot = static_cast<float>((rand() % 100) * 0.01f);
		const float randZ_rot = static_cast<float>((rand() % 100) * 0.01f);

		randPositions[idx] = { randX_pos, randY_pos, randZ_pos };
		randRotations[idx] = { randX_rot, randY_rot, randZ_rot };
	}
}


bool InitializeGraphics::InitializeModels(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext,
	ModelsStore & modelsStore,
	Settings & settings,
	const float farZ)
{
	// initialize all the list of models on the scene

	Log::Print("---------------- INITIALIZATION: MODELS -----------------");

	Log::Debug(LOG_MACRO);

	try
	{
		// create some members of the graphics class
		std::unique_ptr<ModelInitializer> pModelInitializer = std::make_unique<ModelInitializer>();  
		pGraphics_->pFrustum_ = new FrustumClass();  

		///////////////////////////////

		// initialize the frustum object
		pGraphics_->pFrustum_->Initialize(farZ);


		///////////////////////////////

		// create a plane model
		ModelsCreator modelsCreator;
		//modelsCreator.CreatePlane(pDevice, models_, { 0,0,0 }, { 0,0,0 });

		// define how many cube we want to create
		const UINT numOfCubes = settings.GetSettingIntByKey("CUBES_NUMBER");

		// arrays for random positions/rotations values of cubes
		std::vector<DirectX::XMVECTOR> randPositions(numOfCubes);
		std::vector<DirectX::XMVECTOR> randRotations(numOfCubes);

		// generate random data for cubes
		PrepareRandomPositionRotationForModelsToInit(
			numOfCubes,
			randPositions,
			randRotations);

		// create a cube which will be a basic cube for creation of the other ones
		const UINT originCube_idx = modelsCreator.CreateCube(pDevice,
			modelsStore,
			randPositions[0],
			randRotations[0]);

		// set a texture for the created cube
		modelsStore.SetTextureByIndex(originCube_idx, "data/textures/box01d.dds", aiTextureType_DIFFUSE);

		const TextureClass & originCubeTexture = modelsStore.textures_[originCube_idx];


		// create a cube models numOfCubes times
		for (UINT counter = 1; counter < numOfCubes; ++counter)
		{
			modelsCreator.CreateCopyOfModelByIndex(
				originCube_idx,
				modelsStore,
				pDevice,
				pDeviceContext,
				randPositions[counter],
				randRotations[counter]);
		}

		// arrays for positions/rotations modification values for the cubes
		std::vector<DirectX::XMVECTOR> positionModificators(numOfCubes);
		std::vector<DirectX::XMVECTOR> rotationModificators(numOfCubes);

		// prepare rotation modification data for the cubes
		for (UINT idx = 1; idx < numOfCubes; ++idx)
		{
			positionModificators[idx] = XMVectorZero();
		}


		const int range = 100;
		// prepare rotation modification data for the cubes
		for (UINT idx = 1; idx < numOfCubes; ++idx)
		{
			//const DirectX::XMMATRIX movement = DirectX::XMMatrixTranslationFromVector(positions_[index]);

			const float randX_rot = static_cast<float>((rand() % range) * 0.0001f); // rand(0,100) / 10000.0f 
			const float randY_rot = static_cast<float>((rand() % range) * 0.0001f);
			const float randZ_rot = static_cast<float>((rand() % range) * 0.0001f);

			rotationModificators[idx] = { randX_rot, randY_rot, randZ_rot };
		}

		// apply the positions/rotations modificators
		modelsStore.positionsModificators_ = positionModificators;
		modelsStore.rotationModificators_ = rotationModificators;

		// clear the transient initialization data
		randPositions.clear();
		randRotations.clear();
		positionModificators.clear();
		rotationModificators.clear();

		///////////////////////////////
		
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
  
bool InitializeGraphics::InitializeTerrainZone(
	const float farZ,                            // screen depth
	const float cameraHeightOffset)              // the offset of the camera above the terrain
{

	// this function initializes the main wrapper for all of the terrain processing

	Log::Debug("\n\n\n");
	Log::Print("--------------- INITIALIZATION: TERRAIN ZONE  -----------------");

	try
	{

	

		// create and initialize the zone class object
		pGraphics_->pZone_ = new ZoneClass(pGraphics_->GetCamera());

		bool result = pGraphics_->pZone_->Initialize(farZ, cameraHeightOffset);
		COM_ERROR_IF_FALSE(result, "can't initialize the zone class instance");

		return true;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		return false;
	}
	catch (COMException & exception)
	{
		Log::Error(exception, true);
		Log::Error(LOG_MACRO, "can't initialize the terrain zone");
		return false;
	}


	return true;

} // InitializeTerrainZone

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeLight(Settings & settings)
{
	// this function initializes all the light sources on the scene

	Log::Print("---------------- INITIALIZATION: LIGHT SOURCES -----------------");
	Log::Debug(LOG_MACRO);


	const DirectX::XMFLOAT3 ambientColorOn{ 0.3f, 0.3f, 0.3f };
	const DirectX::XMFLOAT3 ambientColorOff{ 0, 0, 0 };

	const UINT numDiffuseLights = settings.GetSettingIntByKey("NUM_DIFFUSE_LIGHTS");
	const UINT numPointLights   = settings.GetSettingIntByKey("NUM_POINT_LIGHTS");

	LightStore & lightStore = pGraphics_->lightsStore_;

	// set up the DIFFUSE light
	lightStore.CreateDiffuseLight(ambientColorOn,
		{ 1, 1, 1 },                                            // diffuse color
		{ 0, 0, 0 },                                            // specular color
		{ 1, -0.5f, 1 },                                        // direction
		32.0f);                                                 // specular power



	// --------------------------------------------------------------------- //
	// set up the point light sources


	const int range = 100;

	// prepare random colors and positions for point light sources and initialize these lights
	for (size_t idx = 0; idx < numPointLights; ++idx)
	{
		const float red   = (float)(rand() % 255) * 0.01f;   // rand(0, 255) / 100.0f
		const float green = (float)(rand() % 255) * 0.01f;
		const float blue  = (float)(rand() % 255) * 0.01f;

		const float posX = (float)(rand() % range);
		const float posY = (float)(rand() % range);
		const float posZ = (float)(rand() % range);

		lightStore.CreatePointLight(
		{ posX, posY, posZ },
		{ red, green, blue });
	}


	return true;
}

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeGUI(D3DClass & d3d, 
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
		pGraphics_->userInterface_.Initialize(pDevice,
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







////////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////


#if 0
void GameObjectsListClass::GenerateRandomDataForRenderableGameObjects()
{
	// this function generates random color/position values 
	// for the game objects on the scene

	DirectX::XMFLOAT3 color{ 1, 1, 1 };   // white
	DirectX::XMFLOAT3 position{ 0, 0, 0 };
	const float posMultiplier = 50.0f;
	const float gameObjCoordsStride = 20.0f;


	// seed the random generator with the current time
	srand(static_cast<unsigned int>(time(NULL)));

	for (auto & elem : renderableGameObjectsList_)
	{
		// generate a random RGB colour for the game object
		color.x = static_cast<float>(rand()) / RAND_MAX;
		color.y = static_cast<float>(rand()) / RAND_MAX;
		color.z = static_cast<float>(rand()) / RAND_MAX;

		// generate a random position in from of the viewer for the game object
		position.x = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + gameObjCoordsStride;
		position.y = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + 5.0f;
		position.z = (static_cast<float>(rand()) / RAND_MAX) * posMultiplier + gameObjCoordsStride;

		elem.second->SetColor(color);
		elem.second->SetPosition(position);
	}


	return;

} // end GenerateDataForGameObjects
#endif