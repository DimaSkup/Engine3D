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


		// Initialize the DirectX stuff (device, deviceContext, swapChain, rasterizerState, viewport, etc)
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
		bool result = false;

		// ------------------------------   COLOR SHADER  ----------------------------------- //

		// create and initialize the ColorShaderClass object
		pGraphics->pColorShader_ = new ColorShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pColorShader_, "can't create a ColorShaderClass object");

		result = pGraphics->pColorShader_->Initialize(pDevice, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the ColorShaderClass object");


		// ----------------------------   TEXTURE SHADER   ---------------------------------- //

		// create and initialize the TextureShaderClass ojbect
		pGraphics->pTextureShader_ = new TextureShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pTextureShader_, "can't create the texture shader object");

		result = pGraphics->pTextureShader_->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the texture shader object");


		// ------------------------------   LIGHT SHADER  ----------------------------------- //

		// Create and initialize the LightShaderClass object
		pGraphics->pLightShader_ = new LightShaderClass();
		COM_ERROR_IF_FALSE(pGraphics->pLightShader_, "can't create the LightShaderClass object");

		result = pGraphics->pLightShader_->Initialize(pDevice, pDeviceContext, hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the LightShaderClass object");

	}
	catch (COMException& exception) // if we have some error during initialization of shader we handle such an error here
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
		Log::Debug(THIS_FUNC_EMPTY);
		DirectX::XMMATRIX baseViewMatrix;

		if (!InitializeCamera(pGraphics, baseViewMatrix, settingsList)) // initialize all the cameras on the scene and the engine's camera as well
			return false;

		if (!InitializeModels(pGraphics))                 // initialize all the models on the scene
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

	bool result = false;
	int modelsNumber = 100;  // the number of models on the scene

	// ------------------------------ models list ------------------------------------ //

	// create the models list object
	pGraphics->pModelList_ = new ModelListClass();
	COM_ERROR_IF_FALSE(pGraphics->pModelList_, "can't create a ModelListClass object");


	// initialize the models list object
	result = pGraphics->pModelList_->Initialize(modelsNumber);
	COM_ERROR_IF_FALSE(result, "can't initialize the models list object");


	// initialize models objects
	result = InitializeModel(pGraphics, "data/models/sphere", L"data/textures/patrick_bateman_2.dds"); // for navigation to particular file we go from the project root directory
	COM_ERROR_IF_FALSE(result, "can't initialize a model");


	// FRUSTUM: create a frustum object
	pGraphics->pFrustum_ = new FrustumClass();
	COM_ERROR_IF_FALSE(pGraphics->pFrustum_, "can't create the frustum class object");




	// ----------------------- internal default models ------------------------------- //

	// make and initialize a new 2D square with a patrick bateman photo
	pGraphics->pCatSquare_ = new Square(0.0f, 0.0f, 0.0f);

	result = pGraphics->pCatSquare_->Initialize(pDevice, "cat square");
	COM_ERROR_IF_FALSE(result, "can't initialize the cat 2D square");

	pGraphics->pCatSquare_->AddTextures(pDevice, L"data/textures/patrick_bateman.dds");
	pGraphics->pCatSquare_->SetPosition(0.0f, 5.0f, 0.0f);


	// make and initialize a new YELLOW 2D square
	pGraphics->pYellowSquare_ = new Square(1.0f, 1.0f, 0.0f);

	result = pGraphics->pYellowSquare_->Initialize(pDevice, "yellow sqaure");
	COM_ERROR_IF_FALSE(result, "can't initialize the yellow 2D square");

	// setup this 2D yellow sqaure
	pGraphics->pYellowSquare_->SetPosition(-2.0f, 0.0f, 1.0f);




	// make and initialize a new RED triangle
	pGraphics->pTriangleRed_ = new Triangle(1.0f, 0.0f, 0.0f);

	result = pGraphics->pTriangleRed_->Initialize(pDevice, "red triangle");
	COM_ERROR_IF_FALSE(result, "can't initialize the red triangle");

	// setup this red triangle
	pGraphics->pTriangleRed_->SetPosition(0.0f, 0.0f, 0.0f);



	// make and initialize a new GREEN triangle
	pGraphics->pTriangleGreen_ = new Triangle(0.0f, 1.0f, 0.0f);

	result = pGraphics->pTriangleGreen_->Initialize(pDevice, "green triangle");
	COM_ERROR_IF_FALSE(result, "can't initialize the green triangle");

	// setup this green triangle
	pGraphics->pTriangleGreen_->SetPosition(0.0f, 0.0f, 0.0f);


	// reset the temporal pointers
	pDevice = nullptr;

	return true;
} // InitializeModels()


  // initializes a single model by its name and texture
bool InitializeModel(GraphicsClass* pGraphics, LPSTR modelName, WCHAR* textureName)
{
	bool result = false;

	// Create and initialize a ModelClass object
	pGraphics->pModel_ = new ModelClass();
	COM_ERROR_IF_FALSE(pGraphics->pModel_, "can't create the ModelClass object");

	result = pGraphics->pModel_->Initialize(pGraphics->pD3D_->GetDevice(), modelName, textureName);
	COM_ERROR_IF_FALSE(result, "can't initialize the ModelClass object");

	return true;
} // InitializeModel()


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
	pGraphics->pLight_->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f); // set the intensity of the ambient light to 15% white color
	pGraphics->pLight_->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	pGraphics->pLight_->SetDirection(1.0f, 0.0f, 1.0f);
	pGraphics->pLight_->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
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



