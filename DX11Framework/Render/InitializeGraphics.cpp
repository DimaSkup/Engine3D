////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     InitializeGraphics.cpp
// Description:  there are functions for initialization of DirectX
//               and graphics parts of the engine;
//
// Created:      02.12.22
////////////////////////////////////////////////////////////////////////////////////////////
#include "InitializeGraphics.h"


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

		this->pSphereCreator_        = std::make_unique<GameObjectCreator<Sphere>>(pGraphics_->pGameObjectsList_);
		this->pCubeCreator_          = std::make_unique<GameObjectCreator<Cube>>(pGraphics_->pGameObjectsList_);
		this->pPlaneCreator_         = std::make_unique<GameObjectCreator<Plane>>(pGraphics_->pGameObjectsList_);
		this->pCustomGameObjCreator_ = std::make_unique<GameObjectCreator<CustomModel>>(pGraphics_->pGameObjectsList_);
		this->p2DSpriteCreator_      = std::make_unique<GameObjectCreator<SpriteClass>>(pGraphics_->pGameObjectsList_);
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
		// Create the D3DClass object
		pGraphics_->pD3D_ = new D3DClass();

		// get some engine settings
		int windowWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");
		int windowHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");
		bool vsyncEnabled = pEngineSettings_->GetSettingBoolByKey("VSYNC_ENABLED");
		bool fullScreen = pEngineSettings_->GetSettingBoolByKey("FULL_SCREEN");
		float screenNear = pEngineSettings_->GetSettingFloatByKey("NEAR_Z");
		float screenDepth = pEngineSettings_->GetSettingFloatByKey("FAR_Z");


		// Initialize the DirectX stuff (device, deviceContext, swapChain, 
		// rasterizerState, viewport, etc)
		bool result = pGraphics_->pD3D_->Initialize(hwnd,
			windowWidth,
			windowHeight,
			vsyncEnabled,
			fullScreen,
			screenNear,
			screenDepth);
		COM_ERROR_IF_FALSE(result, "can't initialize the Direct3D");

		// setup the rasterizer state
		pGraphics_->pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);
		pGraphics_->pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
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

	// if DirectX was initialized correctly we make local copies of pointers to
	// ID3D11Device and ID3D11DeviceContext
	pDevice_ = pGraphics_->pD3D_->GetDevice();
	pDeviceContext_ = pGraphics_->pD3D_->GetDeviceContext();

	return true;
} // end InitializeDirectX

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeShaders(HWND hwnd)
{
	// this function initializes all the shader classes (color, texture, light, etc.)
	// and the HLSL shaders as well

	assert(pGraphics_ != nullptr);

	Log::Print("---------------------------------------------------------");
	Log::Print("                INITIALIZATION: SHADERS                  ");
	Log::Print("---------------------------------------------------------");

	// make temporal pointer for easier using
	ID3D11Device* pDevice = pGraphics_->pD3D_->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pGraphics_->pD3D_->GetDeviceContext();

	try
	{
		bool result = false;
		std::vector<ShaderClass*> pointersToShaders;


		// NOTE: when you add a new shader class you have to include a header of this class
		//       into the ShadersContainer.h
		//
		// make shaders objects (later all the pointers will be stored in the shaders container)
		// so we don't need clear this vector with pointers
		pointersToShaders.push_back(new ColorShaderClass());
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
		pointersToShaders.push_back(new FontShaderClass());
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

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
		return false;
	}
	catch (COMException & exception) // if we have some error during initialization of shaders we handle such an error here
	{
		// clean temporal pointers
		pDevice = nullptr;
		pDeviceContext = nullptr;

		Log::Error(exception, true);
		return false;
	}

	
	return true;
}  // end InitializeShaders

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeScene(HWND hwnd)
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
		float aspectRatio = windowWidth / windowHeight;

		// setup the EditorCamera object
		pGraphics_->GetCamera()->SetPosition({ 0.0f, 0.0f, -3.0f });
		pGraphics_->GetCamera()->SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);

		// setup the camera for rendering to textures
		pGraphics_->pCameraForRenderToTexture_->SetPosition(0.0f, 0.0f, -5.0f);
		pGraphics_->pCameraForRenderToTexture_->SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);

		// initialize view matrices
		pGraphics_->baseViewMatrix_ = pGraphics_->GetCamera()->GetViewMatrix(); // initialize a base view matrix with the camera for 2D user interface rendering
		pGraphics_->viewMatrix_ = pGraphics_->baseViewMatrix_;                  


		///////////////////////////////////////////////////
		//  CREATE AND INIT RELATED TO TEXTURES STUFF
		///////////////////////////////////////////////////

		// initialize a textures manager
		pGraphics_->pTextureManager_ = std::make_unique<TextureManagerClass>(pDevice_);

		// initialize the render to texture object
		result = pGraphics_->pRenderToTexture_->Initialize(this->pDevice_, 256, 256, farZ, nearZ, 1);
		COM_ERROR_IF_FALSE(result, "can't initialize the render to texture object");



		///////////////////////////////////////////////////
		//  CREATE AND INIT SCENE ELEMENTS
		///////////////////////////////////////////////////

		// initialize all the models on the scene
		if (!InitializeModels())           
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

bool InitializeGraphics::InitializeModels()
{
	// initialize all the list of models on the scene

	Log::Print("---------------- INITIALIZATION: MODELS -----------------");

	Log::Debug(LOG_MACRO);

	// temporal pointers to the device and device context
	ID3D11Device* pDevice = pGraphics_->pD3D_->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pGraphics_->pD3D_->GetDeviceContext();

	try
	{
		// create some members of the graphics class
		pGraphics_->pModelInitializer_ = new ModelInitializer(pDevice, pDeviceContext);  
		pGraphics_->pFrustum_ = new FrustumClass();               
		

		///////////////////////////////

		// initialize the frustum object
		float farZ = pEngineSettings_->GetSettingFloatByKey("FAR_Z");
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

	int screenWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");
	int screenHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");
	int crosshairWidth = 25;
	int crosshairHeight = crosshairWidth;
	const char* animatedSpriteSetupFilename{ "data/models/sprite_data_01.txt" };
	const char* crosshairSpriteSetupFilename{ "data/models/sprite_crosshair.txt" };

	GameObject* pGameObj = nullptr;

	////////////////////////////////////////////////



	// initialize an animated sprite
	pGameObj = this->Create2DSprite(animatedSpriteSetupFilename,
		"animated_sprite",
		{ 0, 500 });


	////////////////////////////////////////////////
	// compute a crosshair's center location
	POINT renderCrossAt{ screenWidth / 2 - crosshairWidth, screenHeight / 2 - crosshairHeight };

	// initialize a crosshair
	pGameObj = this->Create2DSprite(crosshairSpriteSetupFilename,
		"sprite_crosshair",
		renderCrossAt);


	return true;

} // end InitializeSprites

/////////////////////////////////////////////////

bool InitializeGraphics::InitializeInternalDefaultModels()
{
	Log::Debug("-------------------------------------------");
	Log::Debug(LOG_MACRO);

	// a temporal pointer to a renderable game object
	RenderableGameObject* pRenderGameObj = nullptr;

	// get how many times we have to create a model of a particular type
	int spheresCount = pEngineSettings_->GetSettingIntByKey("SPHERES_NUMBER");
	int cubesCount = pEngineSettings_->GetSettingIntByKey("CUBES_NUMBER");
	int planesCount = pEngineSettings_->GetSettingIntByKey("PLANES_NUMBER");
	int treesCount = pEngineSettings_->GetSettingIntByKey("TREES_NUMBER");

	////////////////////////////////////////////////


	// try to create and initialize models objects
	try
	{
		// first of all we need to initialize the default game object so we can 
		// use its data later for initialization of the other game object
		this->InitializeDefaultModels();


		////////// add other game object to the scene (cubes, spheres, etc.) //////////
			
		// create a cube model cubesCount times
		for (size_t it = 0; it < cubesCount; it++)
		{
			pRenderGameObj = this->CreateCube();

			// set that this cube must be rendered by the TextureShaderClass and add a texture to this model
			pRenderGameObj->GetModel()->SetRenderShaderName("LightShaderClass");

			Mesh* pCubeMesh = pRenderGameObj->GetModel()->GetMeshByIndex(0);
			pCubeMesh->SetTextureByIndex(0, "data/textures/WireFence.dds", aiTextureType::aiTextureType_DIFFUSE);
			//pCubeMesh->SetTextureByIndex(1, "data/textures/stone02n.dds", aiTextureType::aiTextureType_NORMALS);
		}
			

		// create a sphere model spheresCount times
		for (size_t it = 0; it < spheresCount; it++)
		{
			pRenderGameObj = this->CreateSphere();
			pRenderGameObj->GetModel()->SetRenderShaderName("TextureShaderClass");
			pRenderGameObj->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/gigachad.dds", aiTextureType::aiTextureType_DIFFUSE);
		}

		// create a plane planesCount times
		for (size_t it = 0; it < planesCount; it++)
		{
			pRenderGameObj = this->CreatePlane();
			pRenderGameObj->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, "data/textures/blue01.tga", aiTextureType::aiTextureType_DIFFUSE);
		}


		// after creation of different game objects 
		// we generate random data (positions, colours, etc.) for all
		// usual game objects (cubes, spheres, etc.)
		pGraphics_->pGameObjectsList_->GenerateRandomDataForGameObjects();


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

#if 0
			
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

	// for debug purposes we have to control if we create terrain/sky_dome/etc. or not
	bool isCreateTerrain = pEngineSettings_->GetSettingBoolByKey("IS_CREATE_TERRAIN");
	bool isCreateSkyDome = pEngineSettings_->GetSettingBoolByKey("IS_CREATE_SKY_DOME");
	bool isCreateSkyPlane = pEngineSettings_->GetSettingBoolByKey("IS_CREATE_SKY_PLANE");

	try
	{

		// create models which are parts of the zone so we can use it later withing the ZoneClass
		if (isCreateTerrain)  this->CreateTerrain();

		if (isCreateSkyDome)
		{
			RenderableGameObject* pSkyDomeGameObj = this->CreateSkyDome();
			bool isUseTexture = false;

			if (isUseTexture)
			{
				// setup the sky dome after initialization
				pSkyDomeGameObj->GetModel()->SetRenderShaderName("TextureShaderClass");
				const std::string skyDomeTexturePath{ "data/textures/doom_sky01d.dds" };

				// add a default texture to the sky dome
				pSkyDomeGameObj->GetModel()->GetMeshByIndex(0)->SetTextureByIndex(0, skyDomeTexturePath, aiTextureType_DIFFUSE);
			}
			
		}

		if (isCreateSkyPlane) this->CreateSkyPlane();

		
	

		// create and initialize the zone class object
		pGraphics_->pZone_ = new ZoneClass(pEngineSettings_,
			pDeviceContext_,
			pGraphics_->GetCamera(),
			pGraphics_->pGameObjectsList_,
			pGraphics_->pModelsToShaderMediator_->GetDataContainerForShaders());

		bool result = pGraphics_->pZone_->Initialize();
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
	pGraphics_->arrDiffuseLights_[0]->SetAmbientColor(ambientColorOff); // set the intensity of the ambient light to 15% white color
	pGraphics_->arrDiffuseLights_[0]->SetDiffuseColor(1.0f, (1.0f / 255.0f) * 140.f, 0.1f);
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

bool InitializeGraphics::InitializeGUI(HWND hwnd, 
	const DirectX::XMMATRIX & baseViewMatrix)
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


		ShaderClass* pShader = pGraphics_->pShadersContainer_->GetShaderByName("FontShaderClass");
		FontShaderClass* pFontShader = static_cast<FontShaderClass*>(pShader);

		// create the UI object
		pGraphics_->pUserInterface_ = new UserInterfaceClass(pDevice_, pDeviceContext_);

		// initialize the user interface
		result = pGraphics_->pUserInterface_->Initialize(pGraphics_->pD3D_,
			fontDataFilePath,
			fontTextureFilePath,
			windowWidth,
			windowHeight,
			baseViewMatrix,
			pFontShader);
		COM_ERROR_IF_FALSE(result, "can't initialize the user interface (GUI)");

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


void InitializeGraphics::InitializeDefaultModels()
{
	// initialization of the default models which will be used for 
	// creation other basic models; 
	// for default models we use a color shader

	Log::Debug(LOG_MACRO);

	bool result = false;
	GameObject* pGameObj = nullptr;

	// try to create and initialize internal default models
	try
	{
		// the default cube model
		result = pCubeCreator_->CreateDefaultRenderableGameObject(pDevice_,
			pDeviceContext_, 
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			"ColorShaderClass");
		COM_ERROR_IF_FALSE(result, "can't initialize a default cube model");

		// the default sphere
		Log::Debug(LOG_MACRO, "creation of a default sphere model");
		result = pSphereCreator_->CreateDefaultRenderableGameObject(pDevice_,
			pDeviceContext_, 
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			"ColorShaderClass");
		COM_ERROR_IF_FALSE(result, "can't initialize a default sphere model");


		/*
		
		// the default triangle
		Log::Debug(LOG_MACRO, "creation of a default triangle model");
		pTriangleCreator_->CreateAndInitDefaultModel(pDevice_, pDeviceContext_, pGraphics_->pModelInitializer_);

		// the default tree
		Log::Debug(LOG_MACRO, "creation of a default tree model");
		pTreeCreator_->CreateAndInitDefaultModel(pDevice_, pDeviceContext_, pGraphics_->pModelInitializer_);

		*/
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't initialize the default models");
	}

	Log::Debug("-------------------------------------------");
	return;

} // end InitializeDefaultModels

/////////////////////////////////////////////////

RenderableGameObject* InitializeGraphics::CreateLine3D(const DirectX::XMFLOAT3 & startPos,
	const DirectX::XMFLOAT3 & endPos)
{
	// create and initialize a line3D model

	RenderableGameObject* pGameObj = nullptr;

#if 0
	

	try
	{
		/////////////////////////  CREATE AND INIT A GAME OBJECT  /////////////////////////

		// create an instance of Line3D and get a pointer to the models list object
		Model* pModel = new Line3D(pGraphics_->pModelInitializer_,
			this->pDevice_,
			this->pDeviceContext_);

		Line3D* pLine = static_cast<Line3D*>(pModel);
		pLine->SetStartPoint(startPos);
		pLine->SetEndPoint(endPos);

		// initialize a model of the line
		bool result = pModel->Initialize("no_path");
		COM_ERROR_IF_FALSE(result, "can't initialize a Line3D object");

		// create a new game object and add a model into it
		pGameObj = new RenderableGameObject(pLine);


		///////////////////////// SETUP THE GAME OBJECT  /////////////////////////

		// setup the game object properties
		pGameObj->SetColor(1, 1, 1, 1);

		pGraphics_->pGameObjectsList_->AddGameObject(pGameObj);
		pGraphics_->pGameObjectsList_->SetGameObjectForRenderingByID(pGameObj->GetID());

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pGraphics_->pModelsToShaderMediator_);
		pModel->SetRenderShaderName("ColorShaderClass");

		// print a message about successful creation
		std::string msg{ pGameObj->GetID() + " is created" };
		Log::Debug(THIS_FUNC, msg.c_str());
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the instance of Line3D");
	}
	catch (COMException & e)
	{
		std::string errorMsg{ "can't create the cube" };

		// try to get an ID of the failed game object
		if (pGameObj != nullptr)
		{
			errorMsg += ": " + pGameObj->GetID();
		}

		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, errorMsg);
	}
#endif
	return pGameObj;   // return a pointer to the created game object (line3D)

} // end CreateLine3D

/////////////////////////////////////////////////

RenderableGameObject* InitializeGraphics::CreateCube(RenderableGameObject* pOriginCube)
{
	// this function creates and initializes a new cube game object;
	// there is 2 cases:
	//   1. we want to create a new empty cube so we will create a copy of the default cube
	//   2. we want to create a copy of some another cube


	RenderableGameObject* pGameObj = nullptr;
	bool isCopyOfCube = false;         // in case of error we will use this flag to get an ID of the origin game object
	
	try
	{
		// check input model's type (it must have a cube type)
		if (pOriginCube != nullptr)
		{
			// check if this game object has correct model
			COM_ERROR_IF_NULLPTR(pOriginCube->GetModel(), "ptr to model == nullptr");

			bool result = (pOriginCube->GetModel()->GetModelType() == "cube");
			COM_ERROR_IF_FALSE(result, "the input model is not a cube model");

			isCopyOfCube = true;
		}

		// if we didn't pass any cube model into the function 
		// we create a copy of the default cube
		if (pOriginCube == nullptr)
		{
			pOriginCube = pGraphics_->pGameObjectsList_->GetDefaultGameObjectByID("cube");
		}

		///////////////////////////////////////////////////

		// create a new cube game object
		pGameObj = dynamic_cast<RenderableGameObject*>(pCubeCreator_->CreateCopyOfGameObject(pOriginCube));

		///////////////////////////////////////////////////

		// print message about success
		std::string debugMsg{ "cube '" + pGameObj->GetID() + "' is created" };
		Log::Debug(LOG_MACRO, debugMsg.c_str());

	}
	catch (COMException & e)
	{
		Log::Error(e, false);

		// if we wanted to create a copy of game object print a message about the origin game object
		if (isCopyOfCube)
		{
			Log::Error(LOG_MACRO, "can't create a copy of cube: " + pOriginCube->GetID());
		}

		// try to get an ID of the failed game object
		COM_ERROR_IF_FALSE(false, "can't create the cube: " + pGameObj->GetID());  
	}

	return pGameObj;   // return a pointer to the created game object

} // end CreateCube

  /////////////////////////////////////////////////

RenderableGameObject* InitializeGraphics::CreateSphere(RenderableGameObject* pOriginSphere)
{
	// this function creates and initializes a new sphere game object;
	// there is 2 cases:
	//   1. we want to create a new empty sphere so we will create a copy of the default sphere
	//   2. we want to create a copy of some another sphere



	RenderableGameObject* pGameObj = nullptr;
	bool isCopyOfSphere = false;         // in case of error we will use this flag to get an ID of the origin game object

	try
	{
		// check input model's type (it must have a sphere type)
		if (pOriginSphere != nullptr)
		{
			// check if this game object has correct model
			COM_ERROR_IF_NULLPTR(pOriginSphere->GetModel(), "ptr to model == nullptr");

			bool result = (pOriginSphere->GetModel()->GetModelType() == "sphere");
			COM_ERROR_IF_FALSE(result, "the input model is not a sphere model");

			isCopyOfSphere = true;
		}

		// if we didn't pass any sphere model into the function 
		// we create a copy of the default sphere
		if (pOriginSphere == nullptr)
		{
			pOriginSphere = pGraphics_->pGameObjectsList_->GetDefaultGameObjectByID("sphere");
		}

		///////////////////////////////////////////////////

		// create a new sphere game object
		pGameObj = dynamic_cast<RenderableGameObject*>(pSphereCreator_->CreateCopyOfGameObject(pOriginSphere));

		///////////////////////////////////////////////////

		// print message about success
		std::string debugMsg{ "sphere '" + pGameObj->GetID() + "' is created" };
		Log::Debug(LOG_MACRO, debugMsg.c_str());

	}
	catch (COMException & e)
	{
		Log::Error(e, true);

		// if we wanted to create a copy of game object print a message about the origin game object
		if (isCopyOfSphere)
		{
			Log::Error(LOG_MACRO, "can't create a copy of sphere: " + pOriginSphere->GetID());
		}

		// try to get an ID of the failed model
		COM_ERROR_IF_FALSE(false, "can't create the sphere: " + pGameObj->GetID());  
	}

	return pGameObj;   // return a pointer to the created model
} // end CreateSphere

#if 0
GameObject* InitializeGraphics::CreateTriangle()
{
	Log::Debug(THIS_FUNC_EMPTY);

	GameObject* pGameObj = nullptr;

	try
	{
		/////////////////////////  CREATE AND INIT A GAME OBJECT  /////////////////////////

		// create and init a model for a game object
		Model* pModel = pTriangleCreator_->CreateAndInitModel(pDevice_,
			pDeviceContext_,
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			"no_path",
			"ColorShaderClass");

		// create a new game object and add a model into it
		pGameObj = new GameObject();
		pGameObj->SetModel(pModel);


		///////////////////////// SETUP THE GAME OBJECT  /////////////////////////

		// make a relation between the model and some shader which will be used for
		// rendering this model (by default the rendering shader is a color shader)
		pModel->SetModelToShaderMediator(pGraphics_->pModelsToShaderMediator_);
		pModel->SetRenderShaderName("ColorShaderClass");

		// print a message about successful creation
		std::string msg{ pGameObj->GetID() + " is created" };
		Log::Debug(THIS_FUNC, msg.c_str());
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		COM_ERROR_IF_FALSE(false, "can't allocate memory for the instance of triangle");
	}
	catch (COMException & e)
	{
		std::string errorMsg{ "can't create the triangle" };

		// try to get an ID of the failed game object
		if (pGameObj != nullptr)
		{
			errorMsg += ": " + pGameObj->GetID();
		}

		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, errorMsg);
	}

	return pModel;   // return a pointer to the created model

} // end CreateTriangle

/////////////////////////////////////////////////
#endif

RenderableGameObject* InitializeGraphics::CreatePlane()
{
	Log::Debug(LOG_MACRO);

	RenderableGameObject* pGameObj = nullptr;

	try 
	{
		// get name of the shader class which will be used for rendering of this plane
		const std::string renderingShaderName = pEngineSettings_->GetSettingStrByKey("RENDER_SHADER_NAME_FOR_PLANE_GAME_OBJ");

		pGameObj = pPlaneCreator_->CreateNewRenderableGameObject(pDevice_,
			pDeviceContext_,
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			"no_path",            // the plane class creates data by itself (vertices/indices) so we don't need a path to the data file here
			renderingShaderName,
			GameObject::RENDERABLE_GAME_OBJ);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);

		//  try to get an ID of the failed game object
		const std::string gameObjID = (pGameObj != nullptr) ? pGameObj->GetID() : "";
		COM_ERROR_IF_FALSE(false, "can't create the plane: " + gameObjID);   
	}


	return pGameObj;   // return a pointer to the created game object (plane)

} // end CreatePlane

/////////////////////////////////////////////////
#if 0
GameObject* InitializeGraphics::CreateTree()
{
	Log::Debug(THIS_FUNC_EMPTY);

	Model* pModel = nullptr;
	
	try
	{
		// as this model type (TreeModel) is default we have to get a path to the 
		// default models directory to get a data file
		std::string defaultModelsDirPath{ pEngineSettings_->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };

		// generate a path to the data file
		std::string filePath{ defaultModelsDirPath + modelType_ };

		pModel = pTreeCreator_->CreateAndInitModel(pDevice_,
			pDeviceContext_,
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			filePath,
			"ColorShaderClass");
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't create the tree: " + pModel->GetModelDataObj()->GetID());   //  try to get an ID of the failed model
	}

	return pModel;   // return a pointer to the created model

} // end CreateTree

/////////////////////////////////////////////////
#endif

RenderableGameObject* InitializeGraphics::Create2DSprite(const std::string & setupFilename,
	const std::string & spriteID,
	const POINT & renderAtPos)
{
	Log::Debug(LOG_MACRO);

	RenderableGameObject* pGameObj = nullptr;

	const UINT screenWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");
	const UINT screenHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");
	const std::string renderingShaderName = pEngineSettings_->GetSettingStrByKey("RENDER_SHADER_NAME_FOR_SPRITE_GAME_OBJ");

	// try to create and initialize a 2D sprite
	try
	{
		pGameObj = p2DSpriteCreator_->CreateNewRenderableGameObject(pDevice_,
			pDeviceContext_,
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			"no_path",                             // the 2D sprite class creates data by itself (vertices/indices) so we don't need a path to the data file here
			renderingShaderName,
			GameObject::SPRITE_GAME_OBJ,
			spriteID);

		// setupping of the sprite
		SpriteClass* pSprite = static_cast<SpriteClass*>(pGameObj->GetModel());
		pSprite->SetupSprite(renderAtPos, screenWidth, screenHeight, setupFilename);

	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't create a 2D sprite");
		COM_ERROR_IF_FALSE(false, "can't create a 2D sprite");
	}

	return pGameObj;

} // end Create2DSprite

/////////////////////////////////////////////////

RenderableGameObject* InitializeGraphics::CreateGameObjectFromFile(const std::string & filePath,
	const std::string & gameObjID)   // expected ID for this game object
{
	// this function IMPORTS some model from the outer model data file (by modelFilename)
	// and initializes a new internal model using this data

	Log::Debug(LOG_MACRO);

	// check input params
	assert(filePath.empty() != true);

	RenderableGameObject* pGameObj = nullptr;

	try
	{
		// create a model for the game object
		pGameObj = pCustomGameObjCreator_->CreateNewRenderableGameObject(pDevice_,
			pDeviceContext_,
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			filePath,                                 // a path to the data file
			"TextureShaderClass",
			GameObject::RENDERABLE_GAME_OBJ,
			gameObjID);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't import and create a new game obj with custom model");
		COM_ERROR_IF_FALSE(false, "can't import and create a new game obj with custom model");
	}

	return pGameObj;

} // CreateGameObjectFromFile

/////////////////////////////////////////////////


RenderableGameObject* InitializeGraphics::CreateTerrain()
{
	// this function creates and initializes a new terrain game object

	RenderableGameObject* pTerrainGameObj = nullptr;
	bool isZoneElement = true;

	try
	{
		// we create a terrain only once, or at least not too often
		// so we have a terrain creator here; not as a member of the InitializeGraphics class;
		std::unique_ptr<GameObjectCreator<TerrainClass>> pTerrainCreator = std::make_unique<GameObjectCreator<TerrainClass>>(pGraphics_->pGameObjectsList_);

		// create a new terrain game object
		pTerrainGameObj = pTerrainCreator->CreateNewRenderableGameObject(pDevice_,
			pDeviceContext_,
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			"no_path",
			"TerrainShaderClass",
			GameObject::ZONE_ELEMENT_GAME_OBJ);
#if 0
		// get a pointer to the terrain to setup its position, etc.
		TerrainClass* pTerrain = static_cast<TerrainClass*>(pTerrainGameObj->GetModel());

		// setup the terrain game object
		float terrainX_Pos = -pTerrain->GetWidth();
		float terrainY_Pos = -10.0f;                  // height in the world
		float terrainZ_Pos = -pTerrain->GetHeight();
#endif

		// move the terrain to the location it should be rendered at
		//pTerrainGameObj->GetData()->SetPosition(terrainX_Pos, terrainY_Pos, terrainZ_Pos);
		pTerrainGameObj->SetPosition(0, 0, 0);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the terrain");
	}

	return pTerrainGameObj;

} // end CreateTerrain

/////////////////////////////////////////////////

RenderableGameObject* InitializeGraphics::CreateSkyDome()
{
	RenderableGameObject* pSkyDomeGameObj = nullptr;
	bool isZoneElement = true;

	try
	{
		std::unique_ptr<GameObjectCreator<SkyDomeClass>> pSkyDomeCreator = std::make_unique<GameObjectCreator<SkyDomeClass>>(pGraphics_->pGameObjectsList_);

		// as this model type (sky dome) is default we have to get a path to the 
		// default models directory to get a data file
		std::string defaultModelsDirPath{ pEngineSettings_->GetSettingStrByKey("DEFAULT_MODELS_DIR_PATH") };
		std::string defaultExt{ pEngineSettings_->GetSettingStrByKey("DEFAULT_MODELS_EXT") };
		std::string filePath{ defaultModelsDirPath + "sky_dome" + defaultExt };

		// create and initialize a sky dome model
		pSkyDomeGameObj = pSkyDomeCreator->CreateNewRenderableGameObject(pDevice_, pDeviceContext_,
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			filePath,
			"SkyDomeShaderClass",
			GameObject::ZONE_ELEMENT_GAME_OBJ);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the sky dome");
	}

	return pSkyDomeGameObj;

} // end CreateSkyDome

/////////////////////////////////////////////////

RenderableGameObject* InitializeGraphics::CreateSkyPlane()
{
	RenderableGameObject* pSkyPlaneGameObj = nullptr;
	bool isZoneElement = true;
	const std::string cloudTexture1{ "data/textures/cloud001.dds" };
	const std::string cloudTexture2{ "data/textures/cloud002.dds" };

	try
	{
		// create and initialize a sky plane model
		std::unique_ptr<GameObjectCreator<SkyPlaneClass>> pSkyPlaneCreator = std::make_unique<GameObjectCreator<SkyPlaneClass>>(pGraphics_->pGameObjectsList_);

		pSkyPlaneGameObj = pSkyPlaneCreator->CreateNewRenderableGameObject(pDevice_,
			pDeviceContext_,
			pGraphics_->pModelInitializer_,
			pGraphics_->pModelsToShaderMediator_,
			"no_path",
			"SkyPlaneShaderClass",
			GameObject::ZONE_ELEMENT_GAME_OBJ);

		SkyPlaneClass* pSkyPlaneModel = static_cast<SkyPlaneClass*>(pSkyPlaneGameObj->GetModel());

		// after initialization we have to add cloud textures to the sky plane model
		bool result = pSkyPlaneModel->LoadCloudTextures(pDevice_, cloudTexture1, cloudTexture2);
		COM_ERROR_IF_FALSE(result, "can't load cloud textures for the sky plane model");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the sky plane model");
	}

	return pSkyPlaneGameObj;

} // end CreateSkyPlane

/////////////////////////////////////////////////