////////////////////////////////////////////////////////////////////
// Filename:     InitializeGraphics.cpp
// Description:  the implementation of the InitializeGraphics class
// Created:      02.12.22
// Revising:     02.12.22
////////////////////////////////////////////////////////////////////
#include "InitializeGraphics.h"




bool InitializeDirectX(GraphicsClass* pGraphics, HWND hwnd, float screenNear, float screenDepth)
{
	bool result = false;

	// Create the D3DClass object
	pGraphics->pD3D_ = new D3DClass();
	if (!pGraphics->pD3D_)
	{
		Log::Error(THIS_FUNC, "can't create the D3DClass object");
		return false;
	}

	// Initialize the DirectX stuff (device, deviceContext, swapChain, rasterizerState, viewport, etc)
	result = pGraphics->pD3D_->Initialize(hwnd,
		SETTINGS::GetSettings()->SCREEN_WIDTH,
		SETTINGS::GetSettings()->SCREEN_HEIGHT,
		SETTINGS::GetSettings()->VSYNC_ENABLED,
		SETTINGS::GetSettings()->FULL_SCREEN,
		screenNear,
		screenDepth);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the Direct3D");
		return false;
	}

	return true;
} // InitializeDirectX()


// initialize all the shaders (color, texture, light, etc.)
bool InitializeShaders(GraphicsClass* pGraphics, HWND hwnd)
{
	bool result = false;

	// ------------------------------   COLOR SHADER  ----------------------------------- //
	
	
	// create the ColorShaderClass object
	pGraphics->pColorShader_ = new ColorShaderClass();
	if (!pGraphics->pColorShader_)
	{
		Log::Error(THIS_FUNC, "can't create a ColorShaderClass object");
		return false;
	}

	// initialize the ColorShaderClass object
	result = pGraphics->pColorShader_->Initialize(pGraphics->pD3D_->GetDevice(), hwnd);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the ColorShaderClass object");
		return false;
	}

	// ----------------------------   TEXTURE SHADER   ---------------------------------- //

	// create the TextureShaderClass ojbect
	pGraphics->pTextureShader_ = new TextureShaderClass();
	if (!pGraphics->pTextureShader_)
	{
		Log::Error(THIS_FUNC, "can't create the texture shader object");
		return false;
	}

	// initialize the texture shader object
	result = pGraphics->pTextureShader_->Initialize(pGraphics->pD3D_->GetDevice(), hwnd);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the texture shader object");
		return false;
	}

	// ------------------------------   LIGHT SHADER  ----------------------------------- //

	// Create the LightShaderClass object
	pGraphics->pLightShader_ = new LightShaderClass();
	if (!pGraphics->pLightShader_)
	{
		Log::Error(THIS_FUNC, "can't create the LightShaderClass object");
		return false;
	}

	// Initialize the LightShaderClass object
	result = pGraphics->pLightShader_->Initialize(pGraphics->pD3D_->GetDevice(), hwnd);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the LightShaderClass object");
		return false;
	}

	return true;
}


// initializes all the stuff on the scene
bool InitializeScene(GraphicsClass* pGraphics, HWND hwnd)
{
	Log::Debug(THIS_FUNC_EMPTY);
	DirectX::XMMATRIX baseViewMatrix;

	if (!InitializeCamera(pGraphics, baseViewMatrix)) // initialize all the cameras on the scene and the engine's camera as well
		return false;

	if (!InitializeModels(pGraphics))                 // initialize all the models on the scene
		return false;

	if (!InitializeLight(pGraphics))            // initialize all the light sources on the scene
		return false;

	if (!InitializeGUI(pGraphics, hwnd, baseViewMatrix)) // initialize the GUI of the game/engine (interface elements, text, etc.)
		return false;


	return true;
}


// initialize all the list of models on the scene
bool InitializeModels(GraphicsClass* pGraphics)
{
	Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;
	int modelsNumber = 1;  // the number of models on the scene

	// ------------------------------ models list ------------------------------------ //

	// create the models list object
	pGraphics->pModelList_ = new ModelListClass();
	if (!pGraphics->pModelList_)
	{
		Log::Error(THIS_FUNC, "can't create a ModelListClass object");
		return false;
	}

	// initialize the models list object
	result = pGraphics->pModelList_->Initialize(modelsNumber);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the models list object");
		return false;
	}



	// -------------------------------- frustum -------------------------------------- //

	// create a frustum object
	pGraphics->pFrustum_ = new FrustumClass();
	if (!pGraphics->pFrustum_)
	{
		Log::Error(THIS_FUNC, "can't create the frustum class object");
		return false;
	}



	// ----------------------- initialize models objects ----------------------------- //

	if (!InitializeModel(pGraphics, "data/models/fat_cat_blender", L"data/textures/cat.dds")) // for navigation to particular file we go from the project root directory
	{
		Log::Error(THIS_FUNC, "can't initialize a model");
		return false;
	}



	// ----------------------- internal default models ------------------------------- //

	// make a YELLOW 2D square
	pGraphics->pYellowSquare_ = new Square(0.0f, 1.0f, 1.0f);

	result = pGraphics->pYellowSquare_->Initialize(pGraphics->pD3D_->GetDevice(), "yellow sqaure");
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the yellow 2D square");
		return false;
	}

	// setup this 2D yellow sqaure
	pGraphics->pYellowSquare_->SetPosition(-2.0f, 0.0f, 1.0f);



	// make a RED triangle
	pGraphics->pTriangleRed_ = new Triangle(1.0f, 0.0f, 0.0f);

	result = pGraphics->pTriangleRed_->Initialize(pGraphics->pD3D_->GetDevice(), "red triangle");
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the red triangle");
		return false;
	}

	// setup this red triangle
	pGraphics->pTriangleRed_->SetPosition(0.0f, 0.0f, 0.0f);




	// make a GREEN triangle
	pGraphics->pTriangleGreen_ = new Triangle(0.0f, 1.0f, 0.0f);

	result = pGraphics->pTriangleGreen_->Initialize(pGraphics->pD3D_->GetDevice(), "green triangle");
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the green triangle");
		return false;
	}

	// setup this green triangle
	pGraphics->pTriangleGreen_->SetPosition(0.0f, 0.0f, 0.0f);


	return true;
} // InitializeModels()


  // initializes a single model by its name and texture
bool InitializeModel(GraphicsClass* pGraphics, LPSTR modelName, WCHAR* textureName)
{
	bool result = false;

	// Create the ModelClass object
	pGraphics->pModel_ = new ModelClass();
	if (!pGraphics->pModel_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the ModelClass object");
		return false;
	}

	// Initialize the ModelClass object (make a vertex and index buffer, etc)
	result = pGraphics->pModel_->Initialize(pGraphics->pD3D_->GetDevice(), modelName, textureName);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the ModelClass object");
		return false;
	}

	return true;
} // InitializeModel()


bool InitializeCamera(GraphicsClass* pGraphics, DirectX::XMMATRIX& baseViewMatrix)
{
	// set up the EditorCamera object
	pGraphics->editorCamera_.SetPosition({ 0.0f, 0.0f, -3.0f });
	pGraphics->editorCamera_.Render();                      // generate the view matrix
	pGraphics->editorCamera_.GetViewMatrix(pGraphics->viewMatrix_); // initialize a base view matrix with the camera for 2D user interface rendering
											  //m_Camera->SetRotation(0.0f, 1.0f, 0.0f);
	pGraphics->editorCamera_.GetViewMatrix(baseViewMatrix);

	return true;
}


// initialize all the light sources on the scene
bool InitializeLight(GraphicsClass* pGraphics)
{
	bool result = false;

	// Create the LightClass object (contains all the light data)
	pGraphics->pLight_ = new LightClass();
	if (!pGraphics->pLight_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the LightClass object");
		return false;
	}

	// Initialize the LightClass object
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
	if (!pGraphics->pDebugText_)
	{
		Log::Error(THIS_FUNC, "can't create a debug text class object");
		return false;
	}

	// initialize the debut text class object
	result = pGraphics->pDebugText_->Initialize(pGraphics->pD3D_->GetDevice(),
		pGraphics->pD3D_->GetDeviceContext(),
		hwnd, 
		SETTINGS::GetSettings()->SCREEN_WIDTH, 
		SETTINGS::GetSettings()->SCREEN_HEIGHT,
		baseViewMatrix);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the debut text class object");
		return false;
	}


	return true;
}



