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
		// create and initialize a data container for the shaders
		pGraphics->pDataForShaders_ = new DataContainerForShadersClass(&pGraphics->editorCamera_);


		ShaderClass* pShader = nullptr;  // a pointer to different shader objects
		Log::Debug(THIS_FUNC_EMPTY);

		bool result = false;

		// create and initialize the ColorShaderClass object
		pShader = new ColorShaderClass();
		COM_ERROR_IF_FALSE(pShader, "can't create a ColorShaderClass object");

		result = pShader->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the ColorShaderClass object");
		pGraphics->AddShader("ColorShaderClass", pShader);



		// create and initialize the TextureShaderClass ojbect
		pShader = new TextureShaderClass();
		COM_ERROR_IF_FALSE(pShader, "can't create the texture shader object");

		result = pShader->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the texture shader object");
		pGraphics->AddShader("TextureShaderClass", pShader);



		// Create and initialize the LightShaderClass object
		pShader = new LightShaderClass();
		COM_ERROR_IF_FALSE(pShader, "can't create the LightShaderClass object");

		result = pShader->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the LightShaderClass object");
		pGraphics->AddShader("LightShaderClass", pShader);


		// create and initialize the MultitextureShaderClass object
		pShader = new(std::nothrow) MultiTextureShaderClass();
		COM_ERROR_IF_FALSE(pShader, "can't create the MultiTextureShaderClass object");

		result = pShader->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the MultiTextureShader object");
		pGraphics->AddShader("MultiTextureShaderClass", pShader);

		
		// create and initialize the LightMapShaderClass object
		pShader = new(std::nothrow) LightMapShaderClass();
		COM_ERROR_IF_FALSE(pShader, "can't create the LightMapShaderClass object");

		result = pShader->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the LightMapShader object");
		pGraphics->AddShader("LightMapShaderClass", pShader);


		// create and initialize the AlphaMapShaderClass object
		pShader = new(std::nothrow) AlphaMapShaderClass();
		COM_ERROR_IF_FALSE(pShader, "can't create the AlphaMapShaderClass object");

		result = pShader->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't ininitialize the AlphaMapShader object");
		pGraphics->AddShader("AlphaMapShaderClass", pShader);


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
		DirectX::XMMATRIX baseViewMatrix;

		if (!InitializeCamera(pGraphics, baseViewMatrix, settingsList)) // initialize all the cameras on the scene and the engine's camera as well
			return false;

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
	ModelClass* pModel = nullptr;  // a temporal pointer to the model for easier using
	bool result = false;

	
	ShaderClass* pColorShader   = pGraphics->GetShaderByName("ColorShaderClass");
	ShaderClass* pLightShader   = pGraphics->GetShaderByName("LightShaderClass");
	ShaderClass* pTextureShader = pGraphics->GetShaderByName("TextureShaderClass");


	// first of all we need to initialize default models so we can use its data later for initialization of the other models
	result = this->InitializeDefaultModels(pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize the default models");

	// add some models to the scene
	result = this->CreateCube(pDevice, pLightShader, InitializeGraphics::CUBES_NUMBER_);
	COM_ERROR_IF_FALSE(result, "can't initialize the cube model");
	//this->CreateSphere(pDevice, pLightShader, InitializeGraphics::SPHERES_NUMBER_);
	//this->CreateTerrain(pDevice, pTextureShader);


	// generate random data for all the models
	result = pGraphics->pModelList_->GenerateDataForModels();

	// reset temporal pointers
	pModel = nullptr;

	return true;
} /* InitializeInternalDefaultModels() */



// set up the engine camera properties
bool InitializeGraphics::InitializeCamera(GraphicsClass* pGraphics, DirectX::XMMATRIX& baseViewMatrix, SETTINGS::settingsParams* settingsList)
{
	Log::Print("---------------- INITIALIZATION: THE CAMERA -----------------");
	Log::Debug(THIS_FUNC_EMPTY);

	float windowWidth = static_cast<float>(settingsList->WINDOW_WIDTH);
	float windowHeight = static_cast<float>(settingsList->WINDOW_HEIGHT);
	float aspectRatio = windowWidth / windowHeight;


	// set up the EditorCamera object
	pGraphics->editorCamera_.SetPosition({ 0.0f, 0.0f, -3.0f });
	pGraphics->editorCamera_.SetProjectionValues(settingsList->FOV_DEGREES, aspectRatio, settingsList->NEAR_Z, settingsList->FAR_Z);
	pGraphics->viewMatrix_ = pGraphics->editorCamera_.GetViewMatrix(); // initialize a base view matrix with the camera for 2D user interface rendering
	baseViewMatrix = pGraphics->editorCamera_.GetViewMatrix();

	return true;
}


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

	Log::Print("---------------- INITIALIZATION: THE GUI -----------------------");
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


bool InitializeGraphics::InitializeDefaultModels(ID3D11Device* pDevice)
{
	// the default cube
	std::unique_ptr<CubeModelCreator> pCubeCreator = std::make_unique<CubeModelCreator>();
	pCubeCreator->CreateAndInitModel(pDevice);

	// the default sphere
	//std::unique_ptr<SphereModelCreator> pSphereCreator = std::make_unique<SphereModelCreator>();
	//pSphereCreator->CreateAndInitModel(pDevice);

	// the default plane
	//std::unique_ptr<PlaneModelCreator> pPlaneCreator = std::make_unique<PlaneModelCreator>();
	//pPlaneCreator->CreateAndInitModel(pDevice);

	return true;
}


bool InitializeGraphics::CreateCube(ID3D11Device* pDevice, ShaderClass* pShader, size_t cubesCount)
{
	std::unique_ptr<CubeModelCreator> pCubeCreator = std::make_unique<CubeModelCreator>();
	ModelClass* pModel = nullptr;
	bool result = false;

	for (size_t i = 0; i < cubesCount; i++)
	{
		pModel = pCubeCreator->CreateAndInitModel(pDevice, pShader);
		result = pModel->AddTexture(pDevice, L"data/textures/stone01.dds");
		COM_ERROR_IF_FALSE(result, "can't add a texture to the cube");
	}

	//delete pModelCreator; // delete the cube creator object
	pModel = nullptr;
	Log::Debug("-------------------------------------------");

	return true;
}

bool InitializeGraphics::CreateSphere(ID3D11Device* pDevice, ShaderClass* pShader, size_t spheresCount)
{
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

	//delete pModelCreator; // delete the sphrere creator object
	pModel = nullptr;
	Log::Debug("-------------------------------------------");

	return true;
}


bool InitializeGraphics::CreateTerrain(ID3D11Device* pDevice, ShaderClass* pShader)
{
	//pModelCreator = new TerrainCreator();
	std::unique_ptr<TerrainModelCreator> pTerrainCreator = std::make_unique<TerrainModelCreator>();
	ModelClass* pModel = nullptr;
	bool result = false;
	pModel = pTerrainCreator->CreateAndInitModel(pDevice, pShader);

	// add textures to the terrain
	result = pModel->AddTexture(pDevice, L"data/textures/dirt01.dds");
	COM_ERROR_IF_FALSE(result, "can't add a texture to the terrain");

	// setup the terrain
	pModel->SetRotation(DirectX::XMConvertToRadians(180), DirectX::XMConvertToRadians(-90));
	pModel->SetPosition(0.0f, -3.0f, 20.0f);   // move the terrain to the location it should be rendered at
	pModel->SetScale(20.0f, 20.0f, 20.0f);

	//delete pModelCreator; // delete the terrain creator object
	pModel = nullptr;
	Log::Debug("-------------------------------------------");

	return true;
}


