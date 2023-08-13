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
		bool result = false;

		// Create the D3DClass object
		pGraphics->pD3D_ = new D3DClass();
		COM_ERROR_IF_FALSE(pGraphics->pD3D_, "can't create the D3DClass object");

		// get some engine settings
		int windowWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");
		int windowHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");
		bool vsyncEnabled = pEngineSettings_->GetSettingBoolByKey("VSYNC_ENABLED");
		bool fullScreen = pEngineSettings_->GetSettingBoolByKey("FULL_SCREEN");
		float screenNear = pEngineSettings_->GetSettingFloatByKey("NEAR_Z");
		float screenDepth = pEngineSettings_->GetSettingFloatByKey("FAR_Z");


		// Initialize the DirectX stuff (device, deviceContext, swapChain, 
		// rasterizerState, viewport, etc)
		result = pGraphics->pD3D_->Initialize(hwnd,
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
		Log::Error(exception);
		return false;
	}

	return true;
} // InitializeDirectX()


// initialize the main wrapper for all of the terrain processing
bool InitializeGraphics::InitializeTerrainZone(GraphicsClass* pGraphics)
{
	try
	{
		pGraphics->pZone_ = new ZoneClass(pEngineSettings_);
		COM_ERROR_IF_FALSE(pGraphics->pZone_, "can't allocate the memory for a zone class instance");

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
		Log::Error(exception);
		return false;
	}
	
}


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

		// create and initialize a data container for the shaders
		pGraphics->pDataForShaders_ = new DataContainerForShadersClass(pGraphics->pZone_->GetCamera());

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

		// clean temporal pointers since we've already don't need it
		pDevice = nullptr;
		pDeviceContext = nullptr;
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

		// initialize view matrices
		pGraphics->baseViewMatrix_ = pGraphics->pZone_->GetCamera()->GetViewMatrix(); // initialize a base view matrix with the camera for 2D user interface rendering
		pGraphics->viewMatrix_ = pGraphics->baseViewMatrix_;   // at the beginning the baseViewMatrix and usual view matrices are the same

		// initialize textures
		result = pGraphics->pTextureManager_->Initialize(pGraphics->pD3D_->GetDevice());
		COM_ERROR_IF_FALSE(result, "can't initialize textures manager");


		if (!InitializeModels(pGraphics))           // initialize all the models on the scene
			return false;

		if (!InitializeLight(pGraphics))            // initialize all the light sources on the scene
			return false;

		if (!InitializeGUI(pGraphics, hwnd, pGraphics->baseViewMatrix_)) // initialize the GUI of the game/engine (interface elements, text, etc.)
			return false;

		Log::Print(THIS_FUNC, "is initialized");
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
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

	// make temporal pointers for easier using of it
	ID3D11Device* pDevice = pGraphics->pD3D_->GetDevice();
	bool result = false;
	float farZ = pEngineSettings_->GetSettingFloatByKey("FAR_Z");


	try
	{
		// initialize the models' initializer
		pGraphics->pModelInitializer_ = new ModelInitializer();

		// initialize the frustum object
		pGraphics->pFrustum_->Initialize(farZ);

		// create the scene models list object
		pGraphics->pModelList_ = new ModelListClass();
		COM_ERROR_IF_FALSE(pGraphics->pModelList_, "can't create a ModelListClass object");

		// setup a pointer to models list inside the zone class obj
		pGraphics->pZone_->SetModelsList(pGraphics->pModelList_);

		// initialize internal default models
		result = this->InitializeInternalDefaultModels(pGraphics, pDevice);
		COM_ERROR_IF_FALSE(result, "can't initialize internal default models");

		// FRUSTUM: create a frustum object
		pGraphics->pFrustum_ = new FrustumClass();
		COM_ERROR_IF_FALSE(pGraphics->pFrustum_, "can't create the frustum class object");
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
		return false;
	}

	// reset the temporal pointers
	pDevice = nullptr;

	return true;
} // InitializeModels()


bool InitializeGraphics::InitializeSprites()
{
	Log::Debug(THIS_FUNC_EMPTY);

	ShadersContainer* pShadersContainer = pGraphics_->GetShadersContainer();
	ShaderClass* pTextureShader = pShadersContainer->GetShaderByName("TextureShaderClass");

	SpriteClass* pSprite = new SpriteClass(pGraphics_->pModelInitializer_);
	pSprite->Initialize(pGraphics_->pD3D_->GetDevice());
	pSprite->GetTextureArray()->AddTexture(L"data/textures/patrick_bateman.dds");

	new ModelToShaderMediator(pSprite, pTextureShader, DataContainerForShadersClass::Get());

	//ModelListClass::Get()->AddModel(pSprite, pSprite->GetModelDataObj()->GetID());
	ModelListClass::Get()->AddSprite(pSprite, pSprite->GetModelDataObj()->GetID());

	return true;
}


bool InitializeGraphics::InitializeInternalDefaultModels(GraphicsClass* pGraphics, ID3D11Device* pDevice)
{
	Log::Debug("-------------------------------------------");
	Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;
	size_t it = 0;              // loop iterator
	Model* pModel = nullptr;    // a temporal pointer to a model object
	ShadersContainer* pShadersContainer = pGraphics->GetShadersContainer();

	int spheresCount = pEngineSettings_->GetSettingIntByKey("SPHERES_NUMBER");
	int cubesCount = pEngineSettings_->GetSettingIntByKey("CUBES_NUMBER");
	int planesCount = pEngineSettings_->GetSettingIntByKey("PLANES_NUMBER");
	int treesCount = pEngineSettings_->GetSettingIntByKey("TREES_NUMBER");


	// get some pointer to the shaders so we will use it during initialization of the models
	ShaderClass* pColorShader = pShadersContainer->GetShaderByName("ColorShaderClass");
	ShaderClass* pLightShader = pShadersContainer->GetShaderByName("LightShaderClass");
	ShaderClass* pSpecularLightShader = pShadersContainer->GetShaderByName("SpecularLightShaderClass");
	ShaderClass* pTextureShader = pShadersContainer->GetShaderByName("TextureShaderClass");
	ShaderClass* pTerrainShader = pShadersContainer->GetShaderByName("TerrainShaderClass");
	ShaderClass* pSkyDomeShader = pShadersContainer->GetShaderByName("SkyDomeShaderClass");
	ShaderClass* pSkyPlaneShader = pShadersContainer->GetShaderByName("SkyPlaneShaderClass");
	ShaderClass* pDepthShader = pShadersContainer->GetShaderByName("DepthShaderClass");


	// try to create and initialize models objects
	try
	{
		// first of all we need to initialize default models so we can use its data later for initialization of the other models
		this->InitializeDefaultModels(pDevice, pColorShader);

		// add other models to the scene (cubes, spheres, etc.)

		for (it = 0; it < cubesCount; it++)    // create a cube cubesCount times
		{
			this->CreateCube(pDevice, pLightShader);
		}

		for (it = 0; it < spheresCount; it++)  // create a sphere spheresCount times
		{
			this->CreateSphere(pDevice, pLightShader);
		}

		for (it = 0; it < planesCount; it++)   // create a plane planesCount times
		{
			this->CreatePlane(pDevice, pTextureShader);
		}

		for (it = 0; it < treesCount; it++)   // create a tree treesCount times
		{
			this->CreateTree(pDevice, pLightShader);
		}

		

		Log::Debug("-------------------------------------------");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		return false;
	}


	result = this->CreateTerrain(pDevice, pDepthShader);
	COM_ERROR_IF_FALSE(result, "can't initialize the terrain");

	result = this->CreateSkyDome(pGraphics, pDevice, pSkyDomeShader);
	COM_ERROR_IF_FALSE(result, "can't initialize the sky dome");

	result = this->CreateSkyPlane(pDevice, pSkyPlaneShader);
	COM_ERROR_IF_FALSE(result, "can't initialize the sky plane");

	
	// generate random data (positions, colours, etc.) for all the models
	result = pGraphics->pModelList_->GenerateDataForModels();
	COM_ERROR_IF_FALSE(result, "can't generate data for the models");

	Log::Debug(THIS_FUNC, "all the models are initialized");
	Log::Debug("-------------------------------------------");

	return true;
} /* InitializeInternalDefaultModels() */


// initialize all the light sources on the scene
bool InitializeGraphics::InitializeLight(GraphicsClass* pGraphics)
{
	Log::Print("---------------- INITIALIZATION: LIGHT SOURCES -----------------");
	Log::Debug(THIS_FUNC_EMPTY);
	bool result = false;

	DirectX::XMFLOAT4 ambientColorOn{ 0.3f, 0.3f, 0.3f, 1.0f };
	DirectX::XMFLOAT4 ambientColorOff{ 1.0f, 1.0f, 1.0f, 1.0f };

	// Create the LightClass object (contains all the light data)
	pGraphics->pLight_ = new LightClass();
	COM_ERROR_IF_FALSE(pGraphics->pLight_, "can't create the LightClass object");

	// set up the LightClass object
	pGraphics->pLight_->SetAmbientColor(ambientColorOn.x, ambientColorOn.y, ambientColorOn.z, ambientColorOn.w); // set the intensity of the ambient light to 15% white color
	pGraphics->pLight_->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	pGraphics->pLight_->SetDirection(1.0f, -0.5f, 1.0f);
	pGraphics->pLight_->SetSpecularColor(0.0f, 0.0f, 0.0f, 1.0f);
	pGraphics->pLight_->SetSpecularPower(32.0f);

	return true;
}

// initialize the GUI of the game/engine (interface elements, text, etc.)
bool InitializeGraphics::InitializeGUI(GraphicsClass* pGraphics, HWND hwnd, const DirectX::XMMATRIX & baseViewMatrix)
{

	Log::Print("---------------- INITIALIZATION: GUI -----------------------");
	Log::Debug(THIS_FUNC_EMPTY);
	bool result = false;
	int windowWidth = pEngineSettings_->GetSettingIntByKey("WINDOW_WIDTH");   // get the window width/height
	int windowHeight = pEngineSettings_->GetSettingIntByKey("WINDOW_HEIGHT");

	// initialize the user interface
	result = pGraphics->pUserInterface_->Initialize(pGraphics->pD3D_, windowWidth, windowHeight, baseViewMatrix);
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
void InitializeGraphics::InitializeDefaultModels(ID3D11Device* pDevice, ShaderClass* pColorShader)
{
	assert(pColorShader != nullptr);

	bool isRendered = false;   // these models won't be rendered
	bool isDefault = true;     // these models are default

	// try to create and initialize internal default models
	try
	{
		// the default cube
		std::unique_ptr<CubeModelCreator> pCubeCreator = std::make_unique<CubeModelCreator>();
		pCubeCreator->CreateAndInitModel(pDevice, pColorShader, pGraphics_->pModelInitializer_, isRendered, isDefault);

		// the default sphere
		std::unique_ptr<SphereModelCreator> pSphereCreator = std::make_unique<SphereModelCreator>();
		pSphereCreator->CreateAndInitModel(pDevice, pColorShader, pGraphics_->pModelInitializer_, isRendered, isDefault);

		// the default plane
		std::unique_ptr<PlaneModelCreator> pPlaneCreator = std::make_unique<PlaneModelCreator>();
		pPlaneCreator->CreateAndInitModel(pDevice, pColorShader, pGraphics_->pModelInitializer_, isRendered, isDefault);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't initialize the default models");
	}

	Log::Debug("-------------------------------------------");
	return;
}

Model* InitializeGraphics::CreateCube(ID3D11Device* pDevice, ShaderClass* pShader)
{
	assert(pShader != nullptr);

	std::unique_ptr<CubeModelCreator> pCubeCreator = std::make_unique<CubeModelCreator>();
	Model* pModel = nullptr;
	bool isRendered = true;     // this model will be rendered
	bool isDefault = false;     // this model isn't default
	

	// try to create and initialize a cube model
	try
	{
		pModel = pCubeCreator->CreateAndInitModel(pDevice, pShader, pGraphics_->pModelInitializer_, isRendered, isDefault);

		// setup the cube
		pModel->GetTextureArray()->AddTexture(L"data/textures/stone01.dds");  // add texture
																			  // generate a random position in from of the viewer for the model
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the cube: " + pModel->GetModelDataObj()->GetID());  // try to get an ID of the failed model
	}

	return pModel;   // return a pointer to the created model
}

Model* InitializeGraphics::CreateSphere(ID3D11Device* pDevice, ShaderClass* pShader)
{
	assert(pShader != nullptr);

	std::unique_ptr<SphereModelCreator> pSphereCreator = std::make_unique<SphereModelCreator>();
	Model* pModel = nullptr;
	bool isRendered = true;     // this model will be rendered
	bool isDefault = false;     // this model isn't default

	try
	{
		pModel = pSphereCreator->CreateAndInitModel(pDevice, pShader, pGraphics_->pModelInitializer_, isRendered, isDefault);

		pModel->GetTextureArray()->AddTexture(L"data/textures/gigachad.dds");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the sphere: " + pModel->GetModelDataObj()->GetID());  // try to get an ID of the failed model
	}

	return pModel;   // return a pointer to the created model
}

Model* InitializeGraphics::CreatePlane(ID3D11Device* pDevice, ShaderClass* pShader)
{
	assert(pShader != nullptr);

	Model* pModel = nullptr;
	bool isRendered = true;     // this model will be rendered
	bool isDefault = false;     // this model isn't default

	try 
	{
		std::unique_ptr<PlaneModelCreator> pPlaneCreator = std::make_unique<PlaneModelCreator>();
		pModel = pPlaneCreator->CreateAndInitModel(pDevice, pShader, pGraphics_->pModelInitializer_, isRendered, isDefault);

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

Model* InitializeGraphics::CreateTree(ID3D11Device* pDevice, ShaderClass* pShader)
{
	assert(pShader != nullptr);

	Model* pModel = nullptr;
	bool isRendered = true;     // this model will be rendered
	bool isDefault = false;     // this model isn't default

	try
	{
		std::unique_ptr<TreeModelCreator> pTreeCreator = std::make_unique<TreeModelCreator>();
		pModel = pTreeCreator->CreateAndInitModel(pDevice, pShader, pGraphics_->pModelInitializer_, isRendered, isDefault);

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

bool InitializeGraphics::CreateTerrain(ID3D11Device* pDevice, ShaderClass* pTerrainShader)
{
	assert(pTerrainShader != nullptr);

	
	bool isRendered = true;     // this model will be rendered
	bool isDefault = false;     // this model isn't default
	Model* pTerrainModel = nullptr;

	// try to create and initialize a terrain
	try
	{
		
		std::unique_ptr<TerrainModelCreator> pTerrainCreator = std::make_unique<TerrainModelCreator>();
		pTerrainModel = pTerrainCreator->CreateAndInitModel(pDevice, pTerrainShader, pGraphics_->pModelInitializer_, isRendered, isDefault);

		// get a pointer to the terrain to setup its position, etc.
		TerrainClass* pTerrain = static_cast<TerrainClass*>(pTerrainModel);

		// setup terrain 
		pTerrain->GetModelDataObj()->SetPosition(-pTerrain->GetWidth(), -10.0f, -pTerrain->GetHeight());   // move the terrain to the location it should be rendered at

		pTerrain = nullptr;
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the terrain");
	}
	

	pTerrainModel = nullptr;

	return true;
}

bool InitializeGraphics::CreateSkyDome(GraphicsClass* pGraphics, ID3D11Device* pDevice, ShaderClass* pSkyDomeShader)
{
	assert(pSkyDomeShader != nullptr);

	Model* pModel = nullptr;
	SkyDomeClass* pSkyDome = nullptr;
	bool isRendered = true;     // this model will be rendered
	bool isDefault = true;      // this model is default

	try
	{
		// create and initialize a sky dome model
		std::unique_ptr<SkyDomeModelCreator> pSkyDomeCreator = std::make_unique<SkyDomeModelCreator>();
		pModel = pSkyDomeCreator->CreateAndInitModel(pDevice, pSkyDomeShader, pGraphics_->pModelInitializer_, isRendered, isDefault);
		pSkyDome = static_cast<SkyDomeClass*>(pModel);

		// setup the sky dome
		pGraphics->pDataForShaders_->SetSkyDomeApexColor(pSkyDome->GetApexColor());
		pGraphics->pDataForShaders_->SetSkyDomeCenterColor(pSkyDome->GetCenterColor());

		pSkyDome->GetTextureArray()->AddTexture(L"data/textures/doom_sky01d.dds");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the sky dome");
	}

	return true;
}

bool InitializeGraphics::CreateSkyPlane(ID3D11Device* pDevice, ShaderClass* pSkyPlaneShader)
{
	assert(pSkyPlaneShader != nullptr);

	bool result = false;
	Model* pModel = nullptr;
	SkyPlaneClass* pSkyPlane = nullptr;
	bool isRendered = true;     // this model will be rendered
	bool isDefault = true;      // this model is default
	WCHAR* cloudTexture1{ L"data/textures/cloud001.dds" };
	WCHAR* cloudTexture2{ L"data/textures/cloud002.dds" };

	try
	{
		// create and initialize a sky plane model
		std::unique_ptr<SkyPlaneCreator> pSkyPlaneCreator = std::make_unique<SkyPlaneCreator>();
		pModel = pSkyPlaneCreator->CreateAndInitModel(pDevice, pSkyPlaneShader, pGraphics_->pModelInitializer_, isRendered, isDefault);
		pSkyPlane = static_cast<SkyPlaneClass*>(pModel);

		// pay attention that we pass a pointer to void into the shaders data container so later we will be able to access to these params directly
		pGraphics_->pDataForShaders_->SetDataByKey("SkyPlaneTranslation", (void*)pSkyPlane->GetPtrToTranslationData());
		pGraphics_->pDataForShaders_->SetDataByKey("SkyPlaneBrigtness", (void*)pSkyPlane->GetPtrToBrightness());

		// after initialization we have to add cloud textures to the sky plane model
		result = pSkyPlane->LoadCloudTextures(pDevice, cloudTexture1, cloudTexture2);
		COM_ERROR_IF_FALSE(result, "can't load cloud textures for the sky plane model");
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		COM_ERROR_IF_FALSE(false, "can't create the sky plane model");
	}

	return true;
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

				pModel->GetMediator()->SetRenderingShaderByName(pShader->GetShaderName());
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

				pModel->GetMediator()->SetRenderingShaderByName(pShader->GetShaderName());
				pModel->GetTextureArray()->SetTexture(L"data/textures/stone01.dds", 0);  
				pModel->GetTextureArray()->SetTexture(L"data/textures/bump01.dds", 1);
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}
		else if (shaderName == "MultiTextureShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetMediator()->SetRenderingShaderByName(pShader->GetShaderName());
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

				pModel->GetMediator()->SetRenderingShaderByName(pShader->GetShaderName());
				pModel->GetTextureArray()->SetTexture(L"data/textures/gigachad.dds", 0);
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}
		else if (shaderName == "LightMapShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetMediator()->SetRenderingShaderByName(pShader->GetShaderName());
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

				pModel->GetMediator()->SetRenderingShaderByName(pShader->GetShaderName());
				pModel->GetTextureArray()->SetTexture(L"data/textures/stone01.dds", 0);  
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}
		else if (shaderName == "DepthShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetMediator()->SetRenderingShaderByName(pShader->GetShaderName());
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}
		else if (shaderName == "TextureShaderClass")
		{
			for (Model* & pModel : ptrsToModels)
			{
				heightOfModel += 3.0f;

				pModel->GetMediator()->SetRenderingShaderByName(pShader->GetShaderName());
				pModel->GetTextureArray()->SetTexture(L"data/textures/patrick_bateman.dds", 0);
				pModel->GetModelDataObj()->SetPosition(0.0f, heightOfModel, modelZStride);
			}
		}

		modelIndex++;        // increase the model's index so later we will setup another model's
		modelZStride += 3;   // the next model's Z-position will be changed by 3.0f
	}  // loop through shaders


	return true;
}