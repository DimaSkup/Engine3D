////////////////////////////////////////////////////////////////////
// Filename:     InitializeGraphics.cpp
// Description:  there are functions for initialization of DirectX
//               and graphics parts of the engine;
// Created:      02.12.22
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "InitializeGraphics.h"



// initialize the DirectX stuff
bool InitializeDirectX(GraphicsClass* pGraphics, HWND hwnd, int windowWidth, int windowHeight, bool vsyncEnabled, bool fullScreen, float screenNear, float screenDepth)
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
bool InitializeShaders(GraphicsClass* pGraphics, HWND hwnd)
{
	// make temporal pointer for easier using
	ID3D11Device* pDevice = pGraphics->pD3D_->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pGraphics->pD3D_->GetDeviceContext();

	try
	{
		Log::Print(THIS_FUNC_EMPTY);

		bool result = false;

		// create and initialize the ColorShaderClass object
		pGraphics->pColorShader_ = new ColorShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pColorShader_, "can't create a ColorShaderClass object");

		result = pGraphics->pColorShader_->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the ColorShaderClass object");



		// create and initialize the TextureShaderClass ojbect
		pGraphics->pTextureShader_ = new TextureShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pTextureShader_, "can't create the texture shader object");

		result = pGraphics->pTextureShader_->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the texture shader object");



		// Create and initialize the LightShaderClass object
		pGraphics->pLightShader_ = new LightShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pLightShader_, "can't create the LightShaderClass object");

		result = pGraphics->pLightShader_->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the LightShaderClass object");



		// create and initialize the MultitextureShaderClass object
		pGraphics->pMultiTextureShader_ = new(std::nothrow) MultiTextureShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pMultiTextureShader_, "can't create the MultiTextureShaderClass object");

		result = pGraphics->pMultiTextureShader_->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the MultiTextureShader object");


		
		// create and initialize the LightMapShaderClass object
		pGraphics->pLightMapShader_ = new(std::nothrow) LightMapShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pLightMapShader_, "can't create the LightMapShaderClass object");

		result = pGraphics->pLightMapShader_->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the LightMapShader object");


		// create and initialize the AlphaMapShaderClass object
		pGraphics->pAlphaMapShader_ = new(std::nothrow) AlphaMapShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pAlphaMapShader_, "can't create the AlphaMapShaderClass object");

		result = pGraphics->pAlphaMapShader_->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't ininitialize the AlphaMapShader object");


		// create and initialize the BumpMapShaderClass object
		pGraphics->pBumpMapShader_ = new(std::nothrow) BumpMapShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pBumpMapShader_, "can't create the BumpMapShaderClass object");

		result = pGraphics->pBumpMapShader_->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the BumpMapShader object");


		// create and initialize the CombinedShaderClass object
		pGraphics->pCombinedShader_ = new (std::nothrow) CombinedShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pCombinedShader_, "can't create the CombinedShaderClass object");

		result = pGraphics->pCombinedShader_->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the CombinedShader object");

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
bool InitializeScene(GraphicsClass* pGraphics, HWND hwnd, SETTINGS::settingsParams* settingsList)
{
	try
	{
		Log::Print(THIS_FUNC_EMPTY);
		DirectX::XMMATRIX baseViewMatrix;

		if (!InitializeCamera(pGraphics, baseViewMatrix, settingsList)) // initialize all the cameras on the scene and the engine's camera as well
			return false;

		if (!InitializeModels(pGraphics))           // initialize all the models on the scene
			return false;

		if (!InitializeLight(pGraphics))            // initialize all the light sources on the scene
			return false;

		if (!InitializeGUI(pGraphics, hwnd, baseViewMatrix)) // initialize the GUI of the game/engine (interface elements, text, etc.)
			return false;
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}


	return true;
}


// initialize all the list of models on the scene
bool InitializeModels(GraphicsClass* pGraphics)
{

	Log::Debug(THIS_FUNC_EMPTY);

	// make temporal pointers for easier using of it
	ID3D11Device* pDevice = pGraphics->pD3D_->GetDevice();

	std::string modelFilename{ "" };
	std::string modelId{ "" };
	bool result = false;

	// number of models
	int spheresNumber = 5;
	int cubesNumber = 30;


	// ------------------------------ models list ------------------------------------ //

	// create the models list object
	pGraphics->pModelList_ = new ModelListClass();
	COM_ERROR_IF_FALSE(pGraphics->pModelList_, "can't create a ModelListClass object");


	// initialize sphere objects spheresNumber times
	modelFilename = "sphere_high";

	for (size_t i = 0; i < spheresNumber; i++)
	{
		modelId = modelFilename + "_id: " + std::to_string(i);
		result = InitializeModel(pGraphics, modelFilename, modelId, L"data/textures/patrick_bateman_2.dds", L"data/textures/gigachad.dds");
		COM_ERROR_IF_FALSE(result, "can't initialize a sphere");
	}
	

	// initialize CUBE objects cubesNumber times
	modelFilename = "cube_2";

	for (size_t i = 0; i < cubesNumber; i++)
	{
		modelId = modelFilename + "_id: " + std::to_string(i);
		result = InitializeModel(pGraphics, modelFilename, modelId, L"data/textures/stone01.dds", L"data/textures/bump01.dds");
		COM_ERROR_IF_FALSE(result, "can't initialize a 3D cube");
	}


	// initialize internal default models
	result = InitializeInternalDefaultModels(pGraphics, pDevice);
	COM_ERROR_IF_FALSE(result, "can't initialize internal default models");


	// generate random data for models 
	result = pGraphics->pModelList_->GenerateDataForModels();
	COM_ERROR_IF_FALSE(result, "can't initialize the models list object");

	
	// FRUSTUM: create a frustum object
	pGraphics->pFrustum_ = new FrustumClass();
	COM_ERROR_IF_FALSE(pGraphics->pFrustum_, "can't create the frustum class object");


	// reset the temporal pointers
	pDevice = nullptr;

	return true;
} // InitializeModels()


  // initializes a single model by its name and texture
bool InitializeModel(GraphicsClass* pGraphics,
					 const string& modelName,
					 const string& modelId,
					 WCHAR* textureName1, 
					 WCHAR* textureName2)
{
	bool result = false;
	ModelClass* pModel = nullptr;    // a pointer to the model for easier using
	size_t modelIndex = 0;           // an index of the last added model to the models list


	// add a new model to the models list
	modelIndex = pGraphics->pModelList_->AddModel(new ModelClass(), modelId);
	pModel = pGraphics->pModelList_->GetModels()[modelIndex];

	// initialize the model
	pModel->SetModel(modelName);
	result = pModel->Initialize(pGraphics->pD3D_->GetDevice(), modelId);

	// check the result
	COM_ERROR_IF_FALSE(result, { "can't initialize the ModelClass object: " + modelId });

	// add textures to this new model
	//pModel->AddTexture(pGraphics->pD3D_->GetDevice(), textureName1);
	//pModel->AddTexture(pGraphics->pD3D_->GetDevice(), textureName2);

	pModel->AddTexture(pGraphics->pD3D_->GetDevice(), L"data/textures/stone01.dds");
	pModel->AddTexture(pGraphics->pD3D_->GetDevice(), L"data/textures/dirt01.dds");
	pModel->AddTexture(pGraphics->pD3D_->GetDevice(), L"data/textures/alpha01.dds");


	pModel = nullptr;

	return true;
} // InitializeModel()


bool InitializeInternalDefaultModels(GraphicsClass* pGraphics, ID3D11Device* pDevice)
{
	bool result = false;

	ModelClass* pModel = nullptr;  // a pointer to the model for easier using
	size_t modelIndex = 0;         // an index of the last added model to the models list
	std::string modelID{ "" };     // an identifier for the models
	
	
	// ----------------------- a DEFAULT 2D SQUARE ----------------------- //
	modelID = "square_id: 1";

	// add a model to the models list
	modelIndex = pGraphics->pModelList_->AddModel(new Square(), modelID);
	pModel = pGraphics->pModelList_->GetModels()[modelIndex];              

	// initialize the model object
	result = pModel->Initialize(pDevice, modelID);
	COM_ERROR_IF_FALSE(result, "can't initialize the 2D square");

	// add textures to this new model
	pModel->AddTexture(pDevice, L"data/textures/stone01.dds");
	pModel->AddTexture(pDevice, L"data/textures/dirt01.dds");
	pModel->AddTexture(pDevice, L"data/textures/alpha01.dds");



	// --------------------------- a TERRAIN ----------------------------- //
	modelID = "terrain";
	// add a model to the models list
	modelIndex = pGraphics->pModelList_->AddModel(new Square(), modelID);
	pModel = pGraphics->pModelList_->GetModels()[modelIndex];

	// initialize the model object
	result = pModel->Initialize(pDevice, modelID);
	COM_ERROR_IF_FALSE(result, "can't initialize the terrain");

	// add textures to this new model
	pModel->AddTexture(pDevice, L"data/textures/dirt01.dds");


	pModel = nullptr;

	return true;
}


// set up the engine camera properties
bool InitializeCamera(GraphicsClass* pGraphics, DirectX::XMMATRIX& baseViewMatrix, SETTINGS::settingsParams* settingsList)
{
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
bool InitializeLight(GraphicsClass* pGraphics)
{
	bool result = false;

	// Create the LightClass object (contains all the light data)
	pGraphics->pLight_ = new LightClass();
	COM_ERROR_IF_FALSE(pGraphics->pLight_, "can't create the LightClass object");

	// set up the LightClass object
	pGraphics->pLight_->SetAmbientColor(0.1f, 0.1f, 0.1f, 1.0f); // set the intensity of the ambient light to 15% white color
	pGraphics->pLight_->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	pGraphics->pLight_->SetDirection(1.0f, 0.0f, 1.0f);
	pGraphics->pLight_->SetSpecularColor(0.0f, 0.0f, 0.0f, 1.0f);
	pGraphics->pLight_->SetSpecularPower(32.0f);

	return true;
}


// initialize the GUI of the game/engine (interface elements, text, etc.)
bool InitializeGUI(GraphicsClass* pGraphics, HWND hwnd,
										const DirectX::XMMATRIX& baseViewMatrix)
{
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



