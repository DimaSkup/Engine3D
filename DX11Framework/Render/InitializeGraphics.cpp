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

#include "../ShaderClass/textureshaderclass.h"         // for texturing models
#include "../ShaderClass/LightShaderClass.h"           // for light effect on models
#include "../ShaderClass/TerrainShaderClass.h"         // for rendering the terrain 
#include "../ShaderClass/SpecularLightShaderClass.h"   // for light effect with specular
#include "../ShaderClass/MultiTextureShaderClass.h"    // for multitexturing
#include "../ShaderClass/LightMapShaderClass.h"        // for light mapping
#include "../ShaderClass/AlphaMapShaderClass.h"        // for alpha mapping
#include "../ShaderClass/BumpMapShaderClass.h"         // for bump mapping
#include "../ShaderClass/CombinedShaderClass.h"        // for different shader effects at once (multitexturing, lighting, alpha mapping, etc.)
#include "../ShaderClass/SkyDomeShaderClass.h"         // for rendering the sky dome
#include "../ShaderClass/SkyPlaneShaderClass.h"        // for rendering the sky plane
#include "../ShaderClass/DepthShaderClass.h"           // for coloring objects according to its depth position
#include "../ShaderClass/PointLightShaderClass.h"      // for point lighting
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


bool InitializeGraphics::InitializeDirectX(HWND hwnd)
{
	// this function initializes the DirectX stuff

	assert(pGraphics_ != nullptr);

	try 
	{
		// get some engine settings
		int windowWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");
		int windowHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");
		bool vsyncEnabled = pEngineSettings_->GetSettingBoolByKey("VSYNC_ENABLED");
		bool fullScreen = pEngineSettings_->GetSettingBoolByKey("FULL_SCREEN");
		float screenNear = pEngineSettings_->GetSettingFloatByKey("NEAR_Z");
		float screenDepth = pEngineSettings_->GetSettingFloatByKey("FAR_Z");


		// Initialize the DirectX stuff (device, deviceContext, swapChain, 
		// rasterizerState, viewport, etc)
		bool result = pGraphics_->d3d_.Initialize(hwnd,
			windowWidth,
			windowHeight,
			vsyncEnabled,
			fullScreen,
			screenNear,
			screenDepth);
		COM_ERROR_IF_FALSE(result, "can't initialize the Direct3D");

		// setup the rasterizer state
		pGraphics_->d3d_.SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);
		pGraphics_->d3d_.SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		return false;
	}
	catch (COMException& exception)
	{
		Log::Error(exception, true);
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

	assert(pGraphics_ != nullptr);

	Log::Print("---------------------------------------------------------");
	Log::Print("                INITIALIZATION: SHADERS                  ");
	Log::Print("---------------------------------------------------------");

	try
	{
		bool result = false;

		result = pGraphics_->colorShader_.Initialize(pDevice, pDeviceContext);
		COM_ERROR_IF_FALSE(result, "can't initialize the color shader class");
#if 0
		std::vector<ShaderClass*> pointersToShaders;


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
			pGraphics_->pShadersContainer_->SetShaderByName(pShader->GetShaderName(), pShader);
		}

		// go through each shader and initialize it
		for (auto & elem : pGraphics_->pShadersContainer_->GetShadersList())
		{
			result = elem.second->Initialize(pDevice, pDeviceContext, hwnd);
			COM_ERROR_IF_FALSE(result, "can't initialize the " + elem.second->GetShaderName() + " object");

			// after the initialization we add this shader into the main model_to_shader mediator
			pGraphics_->pModelsToShaderMediator_->AddShader(elem.second);
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
	HWND hwnd)
{
	// this function initializes all the scene elements

	try
	{
		bool result = false;

		// get some settings values which is necessary for the camera's initialization
		const float windowWidth = pEngineSettings_->GetSettingFloatByKey("WINDOW_WIDTH");
		const float windowHeight = pEngineSettings_->GetSettingFloatByKey("WINDOW_HEIGHT");
		const float fovDegrees = pEngineSettings_->GetSettingFloatByKey("FOV_DEGREES");
		const float nearZ = pEngineSettings_->GetSettingFloatByKey("NEAR_Z");
		const float farZ = pEngineSettings_->GetSettingFloatByKey("FAR_Z");


		///////////////////////////////////////////////////
		//  SETUP CAMERAS AND VIEW MATRICES
		///////////////////////////////////////////////////

		// calculate the aspect ratio
		const float aspectRatio = windowWidth / windowHeight;

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
		if (!InitializeModels(pDevice, pDeviceContext))           
			return false;

		// initialize all the light sources on the scene
		if (!InitializeLight())           
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

bool InitializeGraphics::InitializeModels(ID3D11Device* pDevice, 
	ID3D11DeviceContext* pDeviceContext)
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
		const float farZ = pEngineSettings_->GetSettingFloatByKey("FAR_Z");
		pGraphics_->pFrustum_->Initialize(farZ);

		// initialize internal default models
		bool result = this->InitializeInternalDefaultModels();
		COM_ERROR_IF_FALSE(result, "can't initialize internal default models");

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

bool InitializeGraphics::InitializeSprites()
{
	Log::Debug(LOG_MACRO);

	const UINT screenWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");
	const UINT screenHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");
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

bool InitializeGraphics::InitializeInternalDefaultModels()
{
	Log::Debug("-------------------------------------------");
	Log::Debug(LOG_MACRO);

	// get how many times we have to create a model of a particular type
	const int spheresCount = pEngineSettings_->GetSettingIntByKey("SPHERES_NUMBER");
	const int cubesCount = pEngineSettings_->GetSettingIntByKey("CUBES_NUMBER");
	const int planesCount = pEngineSettings_->GetSettingIntByKey("PLANES_NUMBER");
	const int treesCount = pEngineSettings_->GetSettingIntByKey("TREES_NUMBER");

	////////////////////////////////////////////////


	// try to create and initialize models objects
	try
	{
		// first of all we need to initialize the default game object so we can 
		// use its data later for initialization of the other game object
		//this->InitializeDefaultModels();


		////////// add other game object to the scene (cubes, spheres, etc.) //////////
			
		// create a cube model cubesCount times
		for (size_t it = 0; it < cubesCount; it++)
		{
			//pCubeMesh->SetTextureByIndex(0, "data/textures/WireFence.dds", aiTextureType::aiTextureType_DIFFUSE);
			//pCubeMesh->SetTextureByIndex(1, "data/textures/stone02n.dds", aiTextureType::aiTextureType_NORMALS);
		}


		// create a sphere model spheresCount times
		for (size_t it = 0; it < spheresCount; it++)
		{
			//
		}

		// create a plane planesCount times
		for (size_t it = 0; it < planesCount; it++)
		{
			//
		}


		// after creation of different game objects 
		// we generate random data (positions, colours, etc.) for all
		// usual game objects (cubes, spheres, etc.)
		//pGraphics_->pGameObjectsList_->GenerateRandomDataForRenderableGameObjects();
#if 0



		/////////////////////////////////////////
		//  SETUP PARTICULAR GAME OBJECTS
		/////////////////////////////////////////

		RenderableGameObject* pCube10 = pGraphics_->GetGameObjectsList()->GetRenderableGameObjByID("cube(10)");
		pCube10->SetPosition(10, 2.2f, 15);

		RenderableGameObject* pPlane25 = pGraphics_->GetGameObjectsList()->GetRenderableGameObjByID("plane(25)");
		pPlane25->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/brick01.dds", aiTextureType::aiTextureType_DIFFUSE);
		pPlane25->SetPosition(-10, 0, 0);


		RenderableGameObject* pPlane26 = pGraphics_->GetGameObjectsList()->GetRenderableGameObjByID("plane(26)");
		pPlane26->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/water1.dds", aiTextureType::aiTextureType_DIFFUSE);
		pPlane26->SetPosition(-10, 0, -1);

		////////

		RenderableGameObject* pPlane27 = pGraphics_->GetGameObjectsList()->GetRenderableGameObjByID("plane(27)");
		pPlane27->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/brick01.dds", aiTextureType::aiTextureType_DIFFUSE);
		pPlane27->SetPosition(-12, 0, -1);


		////////

		RenderableGameObject* pPlane28 = pGraphics_->GetGameObjectsList()->GetRenderableGameObjByID("plane(28)");
		//pWaterPlane->SetID("water_plane");
		pPlane28->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/water2.dds", aiTextureType::aiTextureType_DIFFUSE);
		pPlane28->SetPosition(8, 2, 20);
		pPlane28->SetRotationInDeg(0, 0, 90);

		
		/////////////////////////////////////////
		//  CREATE A NANOSUIT
		/////////////////////////////////////////

		RenderableGameObject* pNanoSuit = this->CreateGameObjectFromFile("data/models/nanosuit/nanosuit.obj", "nanosuit");
		pNanoSuit->GetModel()->SetRenderShaderName("LightShaderClass");
		pNanoSuit->SetPosition(10, 2, 20);
		pNanoSuit->SetScale(0.1f, 0.1f, 0.1f);


		/////////////////////////////////////////
		//  CREATE A STALKER-HOUSE
		/////////////////////////////////////////

		RenderableGameObject* pStalkerHouse = this->CreateGameObjectFromFile("data/models/stalker-house/source/SmallHouse.fbx", "stalker_house");
		pStalkerHouse->SetPosition(10, 2, 30);
		pStalkerHouse->SetRotationInDeg(0, 0, 90);

		/////////////////////////////////////////
		//  CREATE AN ABOUNDED HOUSE
		/////////////////////////////////////////

		RenderableGameObject* abandonedHouse = this->CreateGameObjectFromFile("data/models/abandoned-military-house/source/BigBuilding.fbx", "abandoned_house");
		abandonedHouse->SetPosition(10, 2, 50);
		abandonedHouse->SetRotationInDeg(0, 0, 90);
		abandonedHouse->SetScale(0.01f, 0.01f, 0.01f);

		/////////////////////////////////////////////////////////
		//  CREATE AN ORANGE WITH EMBEDDED COMPRESSED TEXTURE
		/////////////////////////////////////////////////////////
		RenderableGameObject* pOrange = this->CreateGameObjectFromFile("data/models/orange_embeddedtexture.fbx", "orange");
		pOrange->SetPosition(10, 2, 60);

		///////////////////////////////////////////////////////////////////////
		//  CREATE A GAME OBJECT WITH EMBEDDED COMPRESSED INDEXED TEXTURE
		///////////////////////////////////////////////////////////////////////
		RenderableGameObject* pPerson = this->CreateGameObjectFromFile("data/models/person_embeddedindexed.blend", "person");
		pPerson->SetPosition(10, 2, 70);
		pPerson->SetScale(0.02f, 0.02f, 0.02f);
		
		/////////////////////////////////////////////////////////
		//  CREATE A MODEL OF AKS-74
		/////////////////////////////////////////////////////////

		// generate path to the aks-74 model
		const std::string pathToModelsDir{ pEngineSettings_->GetSettingStrByKey("MODEL_DIR_PATH") };
		const std::string localPathToModel{ "aks-74_game_ready/scene.gltf" };
		
		const std::string fullFilePath{ pathToModelsDir + localPathToModel };

		RenderableGameObject* pGameObj_aks_74 = this->CreateGameObjectFromFile(fullFilePath, "aks_74");

		// setup the model (set rendering shader and add a texture)
		pGameObj_aks_74->GetModel()->SetRenderShaderName("BumpMapShaderClass");
		//pGameObj_aks_74->GetModel()->SetRenderShaderName("TextureShaderClass");
		pGameObj_aks_74->SetPosition(0, 0, 0);
		pGameObj_aks_74->SetRotationInDeg(0, 0, 90);
		pGameObj_aks_74->SetScale(3, 3, 3);



		//GameObject* pPlane30 = pGraphics_->GetGameObjectsList()->GetGameObjectByID("plane(30)");
		RenderableGameObject* pCube1 = pGraphics_->GetGameObjectsList()->GetRenderableGameObjByID("cube(1)");
		pCube1->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/models/aks-74_game_ready/textures/M_brown_mag_baseColor.png", aiTextureType::aiTextureType_DIFFUSE);
		pCube1->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(1, "data/models/aks-74_game_ready/textures/M_brown_mag_normal.png", aiTextureType::aiTextureType_NORMALS);
		pCube1->GetModel()->SetRenderShaderName("BumpMapShaderClass");
		pCube1->SetPosition(0, 2, 0);
		pCube1->SetRotationInDeg(0, 0, 0);
		//pPlane30->GetData()->SetScale(5, 5, 0);

			
		// create one triangle
		this->CreateTriangle();

		// create one 3D line
		VERTEX startPoint;
		VERTEX endPoint;

		startPoint.position = { 0, 0, 0 };
		endPoint.position = { 10, 10, 10 };
		//this->CreateLine3D(startPoint.position, endPoint.position);

		for (size_t it = 0; it < cubesCount; it++)
		{
		// create a cube cubesCount times

		}

	

			

		for (it = 0; it < treesCount; it++)   // create a tree treesCount times
		{
			this->CreateTree(pDevice);
		}

		
#endif

		
			

		Log::Debug("-------------------------------------------");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		return false;
	}
	
	
	
	

	Log::Debug(LOG_MACRO, "all the models are initialized");
	Log::Debug("-------------------------------------------");

	return true;
} // end InitializeInternalDefaultModels

/////////////////////////////////////////////////
  
bool InitializeGraphics::InitializeTerrainZone()
{

	// this function initializes the main wrapper for all of the terrain processing

	Log::Debug("\n\n\n");
	Log::Print("--------------- INITIALIZATION: TERRAIN ZONE  -----------------");

	try
	{

	

		// create and initialize the zone class object
		pGraphics_->pZone_ = new ZoneClass(pGraphics_->GetCamera());

		bool result = pGraphics_->pZone_->Initialize(pEngineSettings_);
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

bool InitializeGraphics::InitializeLight()
{
	// this function initializes all the light sources on the scene

	Log::Print("---------------- INITIALIZATION: LIGHT SOURCES -----------------");
	Log::Debug(LOG_MACRO);


	bool result = false;

	const DirectX::XMFLOAT3 ambientColorOn{ 0.3f, 0.3f, 0.3f };
	const DirectX::XMFLOAT3 ambientColorOff{ 0, 0, 0 };
	const DirectX::XMFLOAT3 redColor{ 1, 0, 0 };
	const DirectX::XMFLOAT3 greenColor{ 0, 1, 0 };
	const DirectX::XMFLOAT3 blueColor{ 0, 0, 1 };
	const DirectX::XMFLOAT3 whiteColor{ 1, 1, 1 };
	const DirectX::XMFLOAT3 purpleColor{ 1, 0, 1 };
	const DirectX::XMFLOAT3 cyanColor{ 0, 1, 1 };

	const UINT numDiffuseLights = pEngineSettings_->GetSettingIntByKey("NUM_DIFFUSE_LIGHTS");
	const UINT numPointLights = pEngineSettings_->GetSettingIntByKey("NUM_POINT_LIGHTS");

	// allocate memory for light sources and put pointers to them into the relative arrays
	try
	{
		pGraphics_->arrDiffuseLights_.resize(numDiffuseLights);
		pGraphics_->arrPointLights_.resize(numPointLights);

		// allocate memory for diffuse light sources (for example: sun)
		for (auto & pDiffuseLightSrc : pGraphics_->arrDiffuseLights_)
		{
			pDiffuseLightSrc = new LightClass();
		}

		// allocate memory for point light sources (for example: light bulb, candle)
		for (auto & pPointLightSrc : pGraphics_->arrPointLights_)
		{
			pPointLightSrc = new LightClass();
		}
	}
	catch (std::bad_alloc & e)
	{
		// release memory from the diffuse light sources
		for (auto & pDiffuseLightSrc : pGraphics_->arrDiffuseLights_)
		{
			_DELETE(pDiffuseLightSrc);
		}

		// release memory from the point light sources
		for (auto & pPointLightSrc : pGraphics_->arrPointLights_)
		{
			_DELETE(pPointLightSrc);
		}

		pGraphics_->arrDiffuseLights_.clear();
		pGraphics_->arrPointLights_.clear();

		Log::Error(LOG_MACRO, e.what());
		Log::Error(LOG_MACRO, "can't allocate memory for the light sources");
		return false;
	}
	
	// set up the DIFFUSE light
	pGraphics_->arrDiffuseLights_[0]->SetAmbientColor(ambientColorOn); // set the intensity of the ambient light to 15% white color
	pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(1.0f, 1.0f, 1.0f);
	pGraphics_->arrDiffuseLights_[0]->SetDirection(1.0f, -0.5f, 1.0f);
	pGraphics_->arrDiffuseLights_[0]->SetSpecularColor(0.0f, 0.0f, 0.0f);
	pGraphics_->arrDiffuseLights_[0]->SetSpecularPower(32.0f);

	// set up the point light sources
	pGraphics_->arrPointLights_[0]->SetDiffuseColor(redColor);
	pGraphics_->arrPointLights_[0]->SetPosition(20.0f, 2.8f, 20.0f);

	pGraphics_->arrPointLights_[1]->SetDiffuseColor(greenColor);
	pGraphics_->arrPointLights_[1]->SetPosition(25.0f, 3.0f, 20.0f);

	pGraphics_->arrPointLights_[2]->SetDiffuseColor(blueColor);
	pGraphics_->arrPointLights_[2]->SetPosition(20.0f, 3.0f, 25.0f);

	pGraphics_->arrPointLights_[3]->SetDiffuseColor(whiteColor);
	pGraphics_->arrPointLights_[3]->SetPosition(100.0f, 3.0f, 100.0f);

	pGraphics_->arrPointLights_[4]->SetDiffuseColor(purpleColor);
	pGraphics_->arrPointLights_[4]->SetPosition(40, 3.0f, 20);

	pGraphics_->arrPointLights_[5]->SetDiffuseColor(cyanColor);
	pGraphics_->arrPointLights_[5]->SetPosition(50, 3.0f, 20);


	return true;
}

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeGUI(D3DClass & d3d, 
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext)
{
	// this function initializes the GUI of the game/engine (interface elements, text, etc.);

	Log::Print("---------------- INITIALIZATION: GUI -----------------------");
	Log::Debug(LOG_MACRO);

	try
	{
		bool result = false;
		const int windowWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");  
		const int windowHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");
		const std::string fontDataFilePath = pEngineSettings_->GetSettingStrByKey("FONT_DATA_FILE_PATH");
		const std::string fontTextureFilePath = pEngineSettings_->GetSettingStrByKey("FONT_TEXTURE_FILE_PATH");

		std::string videoCardName{ "" };
		int videoCardMemory = 0;

		// get an information about the video card
		d3d.GetVideoCardInfo(videoCardName, videoCardMemory);

		// create the UI object
		pGraphics_->pUserInterface_ = new UserInterfaceClass();

		

		// initialize the user interface
		pGraphics_->pUserInterface_->Initialize(pDevice,
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