////////////////////////////////////////////////////////////////////
// Filename:     InitializeGraphics.cpp
// Description:  there are functions for initialization of DirectX
//               and graphics parts of the engine;
// Created:      02.12.22
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "InitializeGraphics.h"


InitializeGraphics::InitializeGraphics(GraphicsClass* pGraphics)
{
	assert(pGraphics != nullptr);

	pGraphics_ = pGraphics;

	Log::Debug(THIS_FUNC_EMPTY);
}




/////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


// initialize the DirectX stuff
bool InitializeGraphics::InitializeDirectX(GraphicsClass* pGraphics, HWND hwnd)
{
	try 
	{
		// Create the D3DClass object
		pGraphics->pD3D_ = new D3DClass();

		// get some engine settings
		int windowWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");
		int windowHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");
		bool vsyncEnabled = pEngineSettings_->GetSettingBoolByKey("VSYNC_ENABLED");
		bool fullScreen = pEngineSettings_->GetSettingBoolByKey("FULL_SCREEN");
		float screenNear = pEngineSettings_->GetSettingFloatByKey("NEAR_Z");
		float screenDepth = pEngineSettings_->GetSettingFloatByKey("FAR_Z");


		// Initialize the DirectX stuff (device, deviceContext, swapChain, 
		// rasterizerState, viewport, etc)
		bool result = pGraphics->pD3D_->Initialize(hwnd,
			windowWidth,
			windowHeight,
			vsyncEnabled,
			fullScreen,
			screenNear,
			screenDepth);
		COM_ERROR_IF_FALSE(result, "can't initialize the Direct3D");

		// setup the rasterizer state
		pGraphics->pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::CULL_MODE_BACK);
		pGraphics->pD3D_->SetRenderState(D3DClass::RASTER_PARAMS::FILL_MODE_SOLID);
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		return false;
	}
	catch (COMException& exception)
	{
		Log::Error(exception, true);
		Log::Error(THIS_FUNC, "can't initialize DirectX");
		return false;
	}

	return true;
} // InitializeDirectX()


// initialize all the shaders (color, texture, light, etc.)
bool InitializeGraphics::InitializeShaders(GraphicsClass* pGraphics, HWND hwnd)
{
	Log::Debug("\n\n\n");
	Log::Print("--------------- INITIALIZATION: SHADERS -----------------");

	// make temporal pointer for easier using
	ID3D11Device* pDevice = pGraphics->pD3D_->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pGraphics->pD3D_->GetDeviceContext();

	try
	{
		bool result = false;
		std::vector<ShaderClass*> pointersToShaders;

		// create a container for the shaders classes
		pGraphics->pShadersContainer_ = new ShadersContainer();

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
		
		// add pairs [shader_name => shader_ptr] into the shaders container
		for (const auto & pShader : pointersToShaders)
		{
			pGraphics->pShadersContainer_->SetShaderByName(pShader->GetShaderName(), pShader);
		}

		// go through each shader and initialize it
		for (auto & elem : pGraphics->pShadersContainer_->GetShadersList())
		{
			result = elem.second->Initialize(pDevice, pDeviceContext, hwnd);

			if (!result)
			{
				std::string errorMsg{ "can't initialize the " + elem.second->GetShaderName() + " object" };
				COM_ERROR_IF_FALSE(false, errorMsg.c_str());
			}
		}

	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		return false;
	}
	catch (COMException& exception) // if we have some error during initialization of shaders we handle such an error here
	{
		// clean temporal pointers
		pDevice = nullptr;
		pDeviceContext = nullptr;

		Log::Error(exception);
		return false;
	}

	
	return true;
}  // InitializeShaders()


   


// initializes all the stuff on the scene
bool InitializeGraphics::InitializeScene(GraphicsClass* pGraphics, HWND hwnd)
{
	try
	{
		bool result = false;

		// get some settings values which is necessary for the camera's initialization
		float windowWidth = pEngineSettings_->GetSettingFloatByKey("WINDOW_WIDTH");
		float windowHeight = pEngineSettings_->GetSettingFloatByKey("WINDOW_HEIGHT");
		float fovDegrees = pEngineSettings_->GetSettingFloatByKey("FOV_DEGREES");
		float nearZ = pEngineSettings_->GetSettingFloatByKey("NEAR_Z");
		float farZ = pEngineSettings_->GetSettingFloatByKey("FAR_Z");

		// calculate the aspect ratio
		float aspectRatio = windowWidth / windowHeight;

		// setup the EditorCamera object
		pGraphics->GetCamera()->SetPosition({ 0.0f, 0.0f, -3.0f });
		pGraphics->GetCamera()->SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);

		// initialize view matrices
		pGraphics->baseViewMatrix_ = pGraphics->GetCamera()->GetViewMatrix(); // initialize a base view matrix with the camera for 2D user interface rendering
		pGraphics->viewMatrix_ = pGraphics->baseViewMatrix_;   // at the beginning the baseViewMatrix and usual view matrices are the same

		// initialize textures
		result = pGraphics->pTextureManager_->Initialize(pGraphics->pD3D_->GetDevice(), pGraphics_->pD3D_->GetDeviceContext());
		COM_ERROR_IF_FALSE(result, "can't initialize textures manager");


		if (!InitializeModels(pGraphics))           // initialize all the models on the scene
			return false;

		if (!InitializeLight(pGraphics))            // initialize all the light sources on the scene
			return false;

		Log::Print(THIS_FUNC, "is initialized");
	}
	catch (COMException& exception)
	{
		Log::Error(exception, true);
		Log::Error(THIS_FUNC, "can't initialize the scene");

		return false;
	}


	return true;
}


// initialize all the list of models on the scene
bool InitializeGraphics::InitializeModels(GraphicsClass* pGraphics)
{
	Log::Debug("\n\n\n");
	Log::Print("---------------- INITIALIZATION: MODELS -----------------");

	Log::Debug(THIS_FUNC_EMPTY);


	try
	{
		pGraphics->pModelInitializer_ = new ModelInitializer();  // create a models' initializer
		pGraphics->pFrustum_ = new FrustumClass();               // create a frustum object
		pGraphics->pModelList_ = new ModelListClass();           // create a models list 

		// initialize the frustum object
		float farZ = pEngineSettings_->GetSettingFloatByKey("FAR_Z");
		pGraphics->pFrustum_->Initialize(farZ);

		// initialize internal default models
		bool result = this->InitializeInternalDefaultModels(pGraphics, pGraphics->pD3D_->GetDevice());
		COM_ERROR_IF_FALSE(result, "can't initialize internal default models");
		
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, "can't allocate memory for some element");
		return false;
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(THIS_FUNC, "can't initialize models");
		return false;
	}

	return true;
} // InitializeModels()


bool InitializeGraphics::InitializeSprites()
{
	Log::Debug(THIS_FUNC_EMPTY);

	int screenWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");
	int screenHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");
	int renderX = 50;
	int renderY = 50;

	ShadersContainer* pShadersContainer = pGraphics_->GetShadersContainer();
	ShaderClass* pTextureShader = pShadersContainer->GetShaderByName("TextureShaderClass");

	SpriteClass* pSprite = new SpriteClass(pGraphics_->pModelInitializer_);
	pSprite->Initialize(pGraphics_->pD3D_->GetDevice(), 
		screenWidth, screenHeight, 
		renderX, renderY,
		"data/models/sprite_data_01.txt");

	// update rendering position 
	//renderX = (screenWidth / 2) - (pSprite->GetSpriteWidth() / 2);   // at the middle of the screen
	//renderY = screenHeight - pSprite->GetSpriteHeight();
	//pSprite->SetRenderLocation(renderX, renderY);

	//new ModelToShaderMediator(pSprite, pTextureShader, DataContainerForShadersClass::Get());

	//ModelListClass::Get()->AddModel(pSprite, pSprite->GetModelDataObj()->GetID());
	ModelListClass::Get()->AddSprite(pSprite, pSprite->GetModelDataObj()->GetID());

	return true;
}


bool InitializeGraphics::InitializeInternalDefaultModels(GraphicsClass* pGraphics, ID3D11Device* pDevice)
{
	Log::Debug("-------------------------------------------");
	Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;
	bool isCreatePrimitiveModels = true;  // defines if we need to create some primitive models (cubes, spheres, etc.)
	size_t it = 0;              // loop iterator
	Model* pModel = nullptr;    // a temporal pointer to a model object
	ShadersContainer* pShadersContainer = pGraphics->GetShadersContainer();


	int spheresCount = pEngineSettings_->GetSettingIntByKey("SPHERES_NUMBER");
	int cubesCount = pEngineSettings_->GetSettingIntByKey("CUBES_NUMBER");
	int planesCount = pEngineSettings_->GetSettingIntByKey("PLANES_NUMBER");
	int treesCount = pEngineSettings_->GetSettingIntByKey("TREES_NUMBER");

	if (isCreatePrimitiveModels)
	{
		// try to create and initialize models objects
		try
		{
			// first of all we need to initialize default models so we can use its data later for initialization of the other models
			this->InitializeDefaultModels(pDevice);

			// add other models to the scene (cubes, spheres, etc.)

		
			for (it = 0; it < cubesCount; it++)    // create a cube cubesCount times
			{
				this->CreateCube(pDevice);
			}
			
			for (it = 0; it < planesCount; it++)   // create a plane planesCount times
			{
				this->CreatePlane(pDevice);
			}

		
		
			for (it = 0; it < spheresCount; it++)  // create a sphere spheresCount times
			{
				this->CreateSphere(pDevice);
			}

			/*
			for (it = 0; it < treesCount; it++)   // create a tree treesCount times
			{
				this->CreateTree(pDevice);
			}

			
			*/
			// generate random data (positions, colours, etc.) for all the models
			result = pGraphics->pModelList_->GenerateDataForModels();
			COM_ERROR_IF_FALSE(result, "can't generate data for the models");

			// setup some particular models in a particular way
			//if (!SetupModels(pGraphics_->pShadersContainer_))
			//	return false;

			Log::Debug("-------------------------------------------");
		}
		catch (COMException & e)
		{
			Log::Error(e, true);
			return false;
		}
	}
	
	
	

	Log::Debug(THIS_FUNC, "all the models are initialized");
	Log::Debug("-------------------------------------------");

	return true;
} /* InitializeInternalDefaultModels() */


  // initialize the main wrapper for all of the terrain processing
bool InitializeGraphics::InitializeTerrainZone(GraphicsClass* pGraphics)
{
	Log::Debug("\n\n\n");
	Log::Print("--------------- INITIALIZATION: TERRAIN ZONE  -----------------");

	try
	{
		ID3D11Device* pDevice = pGraphics->GetD3DClass()->GetDevice();

		// create models which are parts of the zone so we can use it later withing the ZoneClass
		this->CreateTerrain(pDevice);
		this->CreateSkyDome(pDevice);
		this->CreateSkyPlane(pDevice);
	
		// create and initialize the zone class object
		pGraphics->pZone_ = new ZoneClass(pGraphics->pEngineSettings_,
			pGraphics->GetCamera(),
			pGraphics->pModelList_,
			pGraphics->GetShadersContainer());

		bool result = pGraphics->pZone_->Initialize();
		COM_ERROR_IF_FALSE(result, "can't initialize the zone class instance");

		return true;
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
		return false;
	}
	catch (COMException & exception)
	{
		Log::Error(exception, true);
		Log::Error(THIS_FUNC, "can't initialize the terrain zone");
		return false;
	}
}


// initialize all the light sources on the scene
bool InitializeGraphics::InitializeLight(GraphicsClass* pGraphics)
{
	Log::Print("---------------- INITIALIZATION: LIGHT SOURCES -----------------");
	Log::Debug(THIS_FUNC_EMPTY);


	bool result = false;
	DirectX::XMFLOAT4 ambientColorOn{ 0.1f, 0.1f, 0.1f, 1.0f };
	DirectX::XMFLOAT4 ambientColorOff{ 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 redColor{ 1.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 greenColor{ 0.0f, 1.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 blueColor{ 0.0f, 0.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 whiteColor{ 1.0f, 1.0f, 1.0, 1.0f };

	UINT numDiffuseLights = pEngineSettings_->GetSettingIntByKey("NUM_DIFFUSE_LIGHTS");
	UINT numPointLights = pEngineSettings_->GetSettingIntByKey("NUM_POINT_LIGHTS");

	// allocate memory for light sources and put pointers to them into the relative arrays
	try
	{
		pGraphics->arrDiffuseLights_.resize(numDiffuseLights);
		pGraphics->arrPointLights_.resize(numPointLights);

		for (auto & pDiffuseLightSrc : pGraphics->arrDiffuseLights_)
		{
			pDiffuseLightSrc = new LightClass();
		}

		for (auto & pPointLightSrc : pGraphics->arrPointLights_)
		{
			pPointLightSrc = new LightClass();
		}
	}
	catch (std::bad_alloc & e)
	{
		for (auto & pDiffuseLightSrc : pGraphics->arrDiffuseLights_)
		{
			_DELETE(pDiffuseLightSrc);
		}

		for (auto & pPointLightSrc : pGraphics->arrPointLights_)
		{
			_DELETE(pPointLightSrc);
		}

		pGraphics->arrDiffuseLights_.clear();
		pGraphics->arrPointLights_.clear();

		Log::Error(THIS_FUNC, e.what());
		Log::Error(THIS_FUNC, "can't allocate memory for the light sources");
		return false;
	}
	
	// set up the DIFFUSE light
	pGraphics->arrDiffuseLights_[0]->SetAmbientColor(ambientColorOn); // set the intensity of the ambient light to 15% white color
	pGraphics->arrDiffuseLights_[0]->SetDiffuseColor(1.0f, (1.0f / 255.0f) * 140.f, 0.1f, 1.0f);
	pGraphics->arrDiffuseLights_[0]->SetDirection(1.0f, -0.5f, 1.0f);
	pGraphics->arrDiffuseLights_[0]->SetSpecularColor(0.0f, 0.0f, 0.0f, 1.0f);
	pGraphics->arrDiffuseLights_[0]->SetSpecularPower(32.0f);

	// set up the point light sources
	pGraphics->arrPointLights_[0]->SetDiffuseColor(redColor);
	pGraphics->arrPointLights_[0]->SetPosition(20.0f, 2.8f, 20.0f);

	pGraphics->arrPointLights_[1]->SetDiffuseColor(greenColor);
	pGraphics->arrPointLights_[1]->SetPosition(25.0f, 3.0f, 20.0f);

	pGraphics->arrPointLights_[2]->SetDiffuseColor(blueColor);
	pGraphics->arrPointLights_[2]->SetPosition(20.0f, 3.0f, 25.0f);

	pGraphics->arrPointLights_[3]->SetDiffuseColor(whiteColor);
	pGraphics->arrPointLights_[3]->SetPosition(100.0f, 3.0f, 100.0f);

	return true;
}

// initialize the GUI of the game/engine (interface elements, text, etc.)
bool InitializeGraphics::InitializeGUI(GraphicsClass* pGraphics, 
	HWND hwnd, 
	const DirectX::XMMATRIX & baseViewMatrix)
{

	Log::Print("---------------- INITIALIZATION: GUI -----------------------");
	Log::Debug(THIS_FUNC_EMPTY);
	bool result = false;
	int windowWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");   // get the window width/height
	int windowHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");

	ShaderClass* pShader = pGraphics->pShadersContainer_->GetShaderByName("FontShaderClass");
	FontShaderClass* pFontShader = static_cast<FontShaderClass*>(pShader);

	// initialize the user interface
	result = pGraphics->pUserInterface_->Initialize(pGraphics->pD3D_, 
		windowWidth,
		windowHeight, 
		baseViewMatrix,
		pFontShader);
	COM_ERROR_IF_FALSE(result, "can't initialize the user interface (GUI)");

	return true;
} // InitializeGUI







/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////

// initialization of the default models which will be used for creation other basic models;
// for default models we use a color shader
void InitializeGraphics::InitializeDefaultModels(ID3D11Device* pDevice)
{
	// try to create and initialize internal default models
	try
	{
		bool isRendered = false;   // these models won't be rendered
		bool isDefault = true;     // these models are default

		// the default cube
		pCubeCreator_->CreateAndInitModel(pDevice, pGraphics_->pModelInitializer_, isRendered, isDefault);

		// the default sphere
		pSphereCreator_->CreateAndInitModel(pDevice, pGraphics_->pModelInitializer_, isRendered, isDefault);

		// the default plane
		pPlaneCreator_->CreateAndInitModel(pDevice, pGraphics_->pModelInitializer_, isRendered, isDefault);

		// the default tree
		pTreeCreator_->CreateAndInitModel(pDevice, pGraphics_->pModelInitializer_, isRendered, isDefault);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't initialize the default models");
	}

	Log::Debug("-------------------------------------------");
	return;
}

Model* InitializeGraphics::CreateCube(ID3D11Device* pDevice)
{
	Model* pModel = nullptr;

	// try to create and initialize a cube model
	try
	{
		bool isRendered = true;     // this model will be rendered
		bool isDefault = false;     // this model isn't default
		
		pModel = pCubeCreator_->CreateAndInitModel(pDevice,
			pGraphics_->pModelInitializer_, 
			isRendered, 
			isDefault);

		// setup the cube
		pModel->GetTextureArray()->AddTexture(L"data/textures/stone01.dds");  // add texture															  
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the cube: " + pModel->GetModelDataObj()->GetID());  // try to get an ID of the failed model
	}

	return pModel;   // return a pointer to the created model
}

Model* InitializeGraphics::CreateSphere(ID3D11Device* pDevice)
{
	Model* pModel = nullptr;

	try
	{
		bool isRendered = true;     // this model will be rendered
		bool isDefault = false;     // this model isn't default

		pModel = pSphereCreator_->CreateAndInitModel(pDevice,
			pGraphics_->pModelInitializer_, 
			isRendered, 
			isDefault);

		pModel->GetTextureArray()->AddTexture(L"data/textures/gigachad.dds");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the sphere: " + pModel->GetModelDataObj()->GetID());  // try to get an ID of the failed model
	}

	return pModel;   // return a pointer to the created model
}

Model* InitializeGraphics::CreatePlane(ID3D11Device* pDevice)
{
	Model* pModel = nullptr;
	

	try 
	{
		bool isRendered = true;     // this model will be rendered
		bool isDefault = false;     // this model isn't default
	
		pModel = pPlaneCreator_->CreateAndInitModel(pDevice,
			pGraphics_->pModelInitializer_, 
			isRendered, 
			isDefault);

		// setup the model
		pModel->GetTextureArray()->AddTexture(L"data/textures/stone01.dds");  // add texture
		pModel->GetModelDataObj()->SetRotationInDegrees(90.0f, 0.0f, 0.0f);
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't create the plane: " + pModel->GetModelDataObj()->GetID());   //  try to get an ID of the failed model
	}


	return pModel;   // return a pointer to the created model
}

Model* InitializeGraphics::CreateTree(ID3D11Device* pDevice)
{
	Model* pModel = nullptr;
	
	try
	{
		bool isRendered = true;     // this model will be rendered
		bool isDefault = false;     // this model isn't default

		pModel = pTreeCreator_->CreateAndInitModel(pDevice,
			pGraphics_->pModelInitializer_, 
			isRendered, 
			isDefault);

		// setup the model
		pModel->GetTextureArray()->AddTexture(L"data/textures/grass.dds");  // add texture
	}
	catch (COMException & e)
	{
		Log::Error(e, false);
		COM_ERROR_IF_FALSE(false, "can't create the tree: " + pModel->GetModelDataObj()->GetID());   //  try to get an ID of the failed model
	}

	return pModel;   // return a pointer to the created model
}

TerrainClass* InitializeGraphics::CreateTerrain(ID3D11Device* pDevice)
{
	TerrainClass* pTerrain = nullptr;

	// try to create and initialize a terrain
	try
	{
		bool isRendered = true;     // this model will be rendered
		bool isDefault = false;     // this model isn't default
		
		std::unique_ptr<TerrainModelCreator> pTerrainCreator = std::make_unique<TerrainModelCreator>();
		Model* pTerrainModel = pTerrainCreator->CreateAndInitModel(pDevice, pGraphics_->pModelInitializer_, isRendered, isDefault);

		// get a pointer to the terrain to setup its position, etc.
		pTerrain = static_cast<TerrainClass*>(pTerrainModel);

		// setup terrain 
		pTerrain->GetModelDataObj()->SetPosition(-pTerrain->GetWidth(), -10.0f, -pTerrain->GetHeight());   // move the terrain to the location it should be rendered at
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the terrain");
	}

	return pTerrain;
}

SkyDomeClass* InitializeGraphics::CreateSkyDome(ID3D11Device* pDevice)
{
	Model* pModel = nullptr;

	try
	{
		bool isRendered = true;     // this model will be rendered
		bool isDefault = true;      // this model is default

		// create and initialize a sky dome model
		std::unique_ptr<SkyDomeModelCreator> pSkyDomeCreator = std::make_unique<SkyDomeModelCreator>();

		pModel =  pSkyDomeCreator->CreateAndInitModel(pDevice, 
			pGraphics_->pModelInitializer_,
			isRendered, 
			isDefault);

		
		pModel->GetTextureArray()->AddTexture(L"data/textures/doom_sky01d.dds");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the sky dome");
	}

	return static_cast<SkyDomeClass*>(pModel);
}

SkyPlaneClass* InitializeGraphics::CreateSkyPlane(ID3D11Device* pDevice)
{
	SkyPlaneClass* pSkyPlane = nullptr;

	try
	{
		bool isRendered = true;     // this model will be rendered
		bool isDefault = true;      // this model is default
		WCHAR* cloudTexture1{ L"data/textures/cloud001.dds" };
		WCHAR* cloudTexture2{ L"data/textures/cloud002.dds" };

		// create and initialize a sky plane model
		std::unique_ptr<SkyPlaneCreator> pSkyPlaneCreator = std::make_unique<SkyPlaneCreator>();

		Model* pModel = pSkyPlaneCreator->CreateAndInitModel(pDevice,
			pGraphics_->pModelInitializer_, 
			isRendered, 
			isDefault);

		pSkyPlane = static_cast<SkyPlaneClass*>(pModel);

		// after initialization we have to add cloud textures to the sky plane model
		bool result = pSkyPlane->LoadCloudTextures(pDevice, cloudTexture1, cloudTexture2);
		COM_ERROR_IF_FALSE(result, "can't load cloud textures for the sky plane model");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the sky plane model");
	}

	return pSkyPlane;
}


// setup some models to demonstrate a work of particular shaders
bool InitializeGraphics::SetupModels(const ShadersContainer* pShadersContainer)
{
	assert(pShadersContainer != nullptr);

	Log::Debug(THIS_FUNC_EMPTY);

	std::vector<Model*> ptrsToModels = { nullptr, nullptr, nullptr };  // array of pointers to models
	std::vector<std::string> modelsID = { "", "", "" };                // array of models' names
	std::string shaderName{ "" };
	ShaderClass* pShader = nullptr;
	UINT modelIndex = 1;
	float modelZStride = 0.0f;    // a stride by Z-axis

	// setup trees
	for (int treeIndex = 1; treeIndex < 20; treeIndex++)
	{
		std::string treeID{ "tree(" + std::to_string(treeIndex) + ')' };

		Model* pTree = pGraphics_->pModelList_->GetModelByID(treeID);

		float posX = 0.0f, posZ = 0.0f;

		posX = (static_cast<float>(rand()) / 100.0f);
		posZ = (static_cast<float>(rand()) / 100.0f);
		pTree->GetModelDataObj()->SetPosition(posX, 0.0f, posZ);
	}

	// setup sprites
	//Model* pSprite = pGraphics_->pModelList_->GetModelByID("sprite");
	//pSprite->GetModelDataObj()->SetPosition(2.0f, 2.0f, 2.0f);
	
	

	for (const auto & elem : pShadersContainer->GetShadersList())
	{
		float heightOfModel = 0.0f;   // a stride by Y-axis
		shaderName = elem.first;
		pShader = elem.second;

		// if we came across such a shader we skip it
		if (shaderName == "TerrainShaderClass" ||
			shaderName == "SkyDomeShaderClass" ||
			shaderName == "SkyPlaneShaderClass")
			continue;

		// define which models will we use at the current iteration through shaders
		modelsID[0] = { "cube("   + std::to_string(modelIndex) + ')' };
		modelsID[1] = { "sphere(" + std::to_string(modelIndex) + ')' };
		modelsID[2] = { "plane("  + std::to_string(modelIndex) + ')' };

		// loop through the array of models' ids and get pointers to these models
		for (size_t i = 0; i < modelsID.size(); i++)
		{
			ptrsToModels[i] = pGraphics_->pModelList_->GetModelByID(modelsID[i]);
		}
		

		// for particular shader we have particular model's setup
		if (shaderName == "AlphaMapShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetTextureArray()->SetTexture(L"data/textures/dirt01.dds", 0); 
				pModel->GetTextureArray()->SetTexture(L"data/textures/stone01.dds", 1);
				pModel->GetTextureArray()->SetTexture(L"data/textures/alpha01.dds", 2);
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}
		else if (shaderName == "BumpMapShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetTextureArray()->SetTexture(L"data/textures/stone01.dds", 0);  
				pModel->GetTextureArray()->SetTexture(L"data/textures/bump01.dds", 1);
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
				pModel->GetModelDataObj()->SetScale(10.0f, 10.0f, 10.0f);
			}
		}
		else if (shaderName == "MultiTextureShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetTextureArray()->SetTexture(L"data/textures/stone01.dds", 0); 
				pModel->GetTextureArray()->SetTexture(L"data/textures/dirt01.dds", 1);
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}
		else if (shaderName == "LightShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetTextureArray()->SetTexture(L"data/textures/gigachad.dds", 0);
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}
		else if (shaderName == "LightMapShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetTextureArray()->SetTexture(L"data/textures/stone01.dds", 0);  
				pModel->GetTextureArray()->SetTexture(L"data/textures/light01.dds", 1);  
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}
		else if (shaderName == "SpecularLightShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetTextureArray()->SetTexture(L"data/textures/stone01.dds", 0);  
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}
		else if (shaderName == "DepthShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}
		else if (shaderName == "TextureShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetTextureArray()->SetTexture(L"data/textures/patrick-0.dds", 0);
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}

		modelIndex++;        // increase the model's index so later we will setup another model's
		modelZStride += 3;   // the next model's Z-position will be changed by 3.0f
	}  // loop through shaders


	return true;
}