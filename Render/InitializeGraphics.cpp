////////////////////////////////////////////////////////////////////
// Filename:     InitializeGraphics.cpp
// Description:  there are functions for initialization of DirectX
//               and graphics parts of the engine;
// Created:      02.12.22
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "InitializeGraphics.h"






/////////////////////////////////////////////////////////////////////////////////////////
//
//                                PUBLIC FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////


// initialize the DirectX stuff
bool InitializeGraphics::InitializeDirectX(GraphicsClass* pGraphics, HWND hwnd, int windowWidth, int windowHeight, bool vsyncEnabled, bool fullScreen, float screenNear, float screenDepth)
{
	try 
	{
		bool result = false;

		// Create the D3DClass object
		pGraphics->pD3D_ = new D3DClass();
		COM_ERROR_IF_FALSE(pGraphics->pD3D_, "can't create the D3DClass object");


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
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
} // InitializeDirectX()


// initialize the main wrapper for all of the terrain processing
bool InitializeGraphics::InitializeTerrainZone(GraphicsClass* pGraphics, SETTINGS::settingsParams* settingsList)
{
	try
	{
		pGraphics->pZone_ = new ZoneClass();
		COM_ERROR_IF_FALSE(pGraphics->pZone_, "can't allocate the memory for a zone class instance");

		bool result = pGraphics->pZone_->Initialize(settingsList);
		COM_ERROR_IF_FALSE(result, "can't initialize the zone class instance");

		return true;
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
		std::vector<ShaderClass*> shadersPointers;

		// create and initialize a data container for the shaders
		pGraphics->pDataForShaders_ = new DataContainerForShadersClass(pGraphics->pZone_->GetCamera());
		COM_ERROR_IF_FALSE(pGraphics->pDataForShaders_, "can't create a container for the shaders data");

		// create a container for the shaders classes
		pGraphics->pShadersContainer_ = new ShadersContainer();
		COM_ERROR_IF_FALSE(pGraphics->pShadersContainer_, "can't create a container for the shaders");



		// add shaders to the shaders container 
		shadersPointers.push_back(new ColorShaderClass());
		shadersPointers.push_back(new TextureShaderClass());
		shadersPointers.push_back(new LightShaderClass());
		//shadersPointers.push_back(new MultiTextureShaderClass());
		//shadersPointers.push_back(new LightMapShaderClass());
		

		for (const auto & pShader : shadersPointers)
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



/*








	

		
	

		// create and initialize the AlphaMapShaderClass object
		pShader = new(std::nothrow) AlphaMapShaderClass();
		COM_ERROR_IF_FALSE(pShader, "can't create the AlphaMapShaderClass object");

		result = pShader->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't ininitialize the AlphaMapShader object");
		pGraphics->AddShader("AlphaMapShaderClass", pShader);



*/


		/*
		// create and initialize the BumpMapShaderClass object
		pShader = new(std::nothrow) BumpMapShaderClass();
		COM_ERROR_IF_FALSE(pShader, "can't create the BumpMapShaderClass object");

		result = pShader->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the BumpMapShader object");
		pGraphics->AddShader("BumpMapShaderClass", pShader);
		


		// create and initialize the CombinedShaderClass object
		pShader = new (std::nothrow) CombinedShaderClass();
		COM_ERROR_IF_FALSE(pShader, "can't create the CombinedShaderClass object");

		result = pShader->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the CombinedShader object");
		pGraphics->AddShader("CombinedShaderClass", pShader);
		
		*/


		// clean temporal pointers since we've already don't need it
		pDevice = nullptr;
		pDeviceContext = nullptr;
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
bool InitializeGraphics::InitializeScene(GraphicsClass* pGraphics, HWND hwnd, SETTINGS::settingsParams* settingsList)
{
	try
	{
		// initialize view matrices
		DirectX::XMMATRIX baseViewMatrix = pGraphics->pZone_->GetCamera()->GetViewMatrix(); // initialize a base view matrix with the camera for 2D user interface rendering
		pGraphics->viewMatrix_ = baseViewMatrix;   // at the beginning the baseViewMatrix and usual view matrices are the same

		if (!InitializeModels(pGraphics))           // initialize all the models on the scene
			return false;

		if (!InitializeLight(pGraphics))            // initialize all the light sources on the scene
			return false;

		if (!InitializeGUI(pGraphics, hwnd, baseViewMatrix)) // initialize the GUI of the game/engine (interface elements, text, etc.)
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

	// create the models list object
	pGraphics->pModelList_ = new ModelListClass();
	COM_ERROR_IF_FALSE(pGraphics->pModelList_, "can't create a ModelListClass object");

	// initialize internal default models
	result = this->InitializeInternalDefaultModels(pGraphics, pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize internal default models");

	// FRUSTUM: create a frustum object
	pGraphics->pFrustum_ = new FrustumClass();
	COM_ERROR_IF_FALSE(pGraphics->pFrustum_, "can't create the frustum class object");


	// reset the temporal pointers
	pDevice = nullptr;

	return true;
} // InitializeModels()


bool InitializeGraphics::InitializeInternalDefaultModels(GraphicsClass* pGraphics, ID3D11Device* pDevice)
{
	bool result = false;

	// get some pointer to the shaders so we will use it during initialization of the models
	ShaderClass* pColorShader   = pGraphics->GetShadersContainer()->GetShaderByName("ColorShaderClass");
	ShaderClass* pLightShader   = pGraphics->GetShadersContainer()->GetShaderByName("LightShaderClass");
	ShaderClass* pTextureShader = pGraphics->GetShadersContainer()->GetShaderByName("TextureShaderClass");
	SETTINGS::settingsParams* pSettings = SETTINGS::GetSettings();

	// first of all we need to initialize default models so we can use its data later for initialization of the other models
	result = this->InitializeDefaultModels(pDevice, pColorShader);
	COM_ERROR_IF_FALSE(result, "can't initialize the default models");

	// add some models to the scene
	result = this->CreateCube(pDevice, pTextureShader, pSettings->CUBES_NUMBER);
	COM_ERROR_IF_FALSE(result, "can't initialize the cube models");
	
	result = this->CreateSphere(pDevice, pLightShader, pSettings->SPHERES_NUMBER);
	COM_ERROR_IF_FALSE(result, "can't initialize the spheres models");

	result = this->CreateTerrain(pDevice, pColorShader);
	COM_ERROR_IF_FALSE(result, "can't initialize the terrain");

	// generate random data (positions, colours, etc.) for all the models
	result = pGraphics->pModelList_->GenerateDataForModels();
	COM_ERROR_IF_FALSE(result, "can't generate data for the models");


	// setup some particular cube model
	ModelListClass::Get()->GetModelByID("cube")->GetMediator()->SetRenderingShaderByName(pColorShader->GetShaderName());


	return true;
} /* InitializeInternalDefaultModels() */


// initialize all the light sources on the scene
bool InitializeGraphics::InitializeLight(GraphicsClass* pGraphics)
{
	Log::Print("---------------- INITIALIZATION: LIGHT SOURCES -----------------");
	Log::Debug(THIS_FUNC_EMPTY);
	bool result = false;

	DirectX::XMFLOAT4 ambientColorOn{ 0.1f, 0.1f, 0.1f, 1.0f };
	DirectX::XMFLOAT4 ambientColorOff{ 1.0f, 1.0f, 1.0f, 1.0f };

	// Create the LightClass object (contains all the light data)
	pGraphics->pLight_ = new LightClass();
	COM_ERROR_IF_FALSE(pGraphics->pLight_, "can't create the LightClass object");

	// set up the LightClass object
	pGraphics->pLight_->SetAmbientColor(ambientColorOn.x, ambientColorOn.y, ambientColorOn.z, ambientColorOn.w); // set the intensity of the ambient light to 15% white color
	pGraphics->pLight_->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	pGraphics->pLight_->SetDirection(1.0f, 0.0f, 1.0f);
	pGraphics->pLight_->SetSpecularColor(0.0f, 0.0f, 0.0f, 1.0f);
	pGraphics->pLight_->SetSpecularPower(32.0f);

	return true;
}


// initialize the GUI of the game/engine (interface elements, text, etc.)
bool InitializeGraphics::InitializeGUI(GraphicsClass* pGraphics, HWND hwnd,
										const DirectX::XMMATRIX& baseViewMatrix)
{

	Log::Print("---------------- INITIALIZATION: GUI -----------------------");
	Log::Debug(THIS_FUNC_EMPTY);
	bool result = false;

	// ----------------------------- DEBUG TEXT ------------------------------------- //
	pGraphics->pDebugText_ = new (std::nothrow) DebugTextClass();
	COM_ERROR_IF_FALSE(pGraphics->pDebugText_, "can't create a debug text class object");

	// initialize the debut text class object
	result = pGraphics->pDebugText_->Initialize(pGraphics->pD3D_->GetDevice(),
		pGraphics->pD3D_->GetDeviceContext(),
		hwnd, 
		SETTINGS::GetSettings()->WINDOW_WIDTH, 
		SETTINGS::GetSettings()->WINDOW_HEIGHT,
		baseViewMatrix);
	COM_ERROR_IF_FALSE(result, "can't initialize the debut text class object");

	return true;
} // InitializeGUI







/////////////////////////////////////////////////////////////////////////////////////////
//
//                               PRIVATE FUNCTIONS
//
/////////////////////////////////////////////////////////////////////////////////////////

// initialization of the default models which will be used for creation other basic models;
// for default models we use a color shader
bool InitializeGraphics::InitializeDefaultModels(ID3D11Device* pDevice, ShaderClass* pColorShader)
{
	// the default cube
	std::unique_ptr<CubeModelCreator> pCubeCreator = std::make_unique<CubeModelCreator>();
	pCubeCreator->CreateAndInitModel(pDevice, pColorShader);
	

	// the default sphere
	std::unique_ptr<SphereModelCreator> pSphereCreator = std::make_unique<SphereModelCreator>();
	pSphereCreator->CreateAndInitModel(pDevice, pColorShader);

	// the default plane
	std::unique_ptr<PlaneModelCreator> pPlaneCreator = std::make_unique<PlaneModelCreator>();
	pPlaneCreator->CreateAndInitModel(pDevice, pColorShader);

	// because we don't want to render the default models we remove it from the rendering list
	for (auto & elem : ModelListClass::Get()->GetDefaultModelsList())
	{
		std::string debugMsg{ "remove from rendering:  " + elem.first };
		Log::Print(THIS_FUNC, debugMsg.c_str());
		ModelListClass::Get()->GetModelsRenderingList().erase(elem.first);
	}
	

	return true;
}


bool InitializeGraphics::CreateCube(ID3D11Device* pDevice, ShaderClass* pShader, size_t cubesCount)
{
	assert(pDevice);
	assert(pShader);

	std::unique_ptr<CubeModelCreator> pCubeCreator = std::make_unique<CubeModelCreator>();
	ModelClass* pModel = nullptr;
	bool result = false;

	for (size_t i = 0; i < cubesCount; i++)
	{
		pModel = pCubeCreator->CreateAndInitModel(pDevice, pShader);
		result = pModel->AddTexture(pDevice, L"data/textures/stone01.dds");
		COM_ERROR_IF_FALSE(result, "can't add a texture to the cube");
	}

	pModel = nullptr;
	Log::Debug("-------------------------------------------");

	return true;
}

bool InitializeGraphics::CreateSphere(ID3D11Device* pDevice, ShaderClass* pShader, size_t spheresCount)
{
	assert(pDevice);
	assert(pShader);

	std::unique_ptr<SphereModelCreator> pSphereCreator = std::make_unique<SphereModelCreator>();
	ModelClass* pModel = nullptr;
	bool result = false;

	// initialize sphere models spheresNumber times
	for (size_t i = 0; i < spheresCount; i++)
	{
		pModel = pSphereCreator->CreateAndInitModel(pDevice, pShader);
		result = pModel->AddTexture(pDevice, L"data/textures/gigachad.dds");
		COM_ERROR_IF_FALSE(result, "can't add a texture to the sphere");
	}

	pModel = nullptr;
	Log::Debug("-------------------------------------------");

	return true;
}


bool InitializeGraphics::CreateTerrain(ID3D11Device* pDevice, ShaderClass* pShader)
{
	assert(pDevice);
	assert(pShader);

	float terrainWidth = 256.0f;
	float terrainHeight = 256.0f;

	std::unique_ptr<TerrainModelCreator> pTerrainCreator = std::make_unique<TerrainModelCreator>();
	ModelClass* pModel = nullptr;
	bool result = false;
	pModel = pTerrainCreator->CreateAndInitModel(pDevice, pShader);

	pModel->SetPosition(-(terrainWidth / 2), 0.0f, -(terrainHeight / 2));   // move the terrain to the location it should be rendered at
	pModel = nullptr;

	return true;
}


