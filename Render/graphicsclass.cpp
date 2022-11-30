////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 14.10.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

GraphicsClass::GraphicsClass(void)
{

}

// We don't use the copy constructor and destructor in this class
GraphicsClass::GraphicsClass(const GraphicsClass& another) {}
GraphicsClass::~GraphicsClass(void) {}



// ----------------------------------------------------------------------------------- //
//
//                             PUBLIC METHODS
//
// ----------------------------------------------------------------------------------- //

// Initializes all the main parts of graphics rendering module
bool GraphicsClass::Initialize(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;
	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	// --------------------------------------------------------------------------- //
	//              INITIALIZE ALL THE PARTS OF GRAPHICS SYSTEM                    //
	// --------------------------------------------------------------------------- //
	
	if (!InitializeDirectX(hwnd, screenWidth, screenHeight))
	{
		Log::Error(THIS_FUNC, "can't initialize DirectX stuff");
		return false;
	}

	if (!this->InitializeShaders(hwnd))
	{
		Log::Error(THIS_FUNC, "can't initialize the shaders system");
		return false;
	}

	if (!this->InitializeScene(hwnd))
	{
		Log::Error(THIS_FUNC, "can't initialize the scene");
		return false;
	}



	Log::Print(THIS_FUNC, " is successfully initialized");
	return true;
}

// Shutdowns all the graphics rendering parts, releases the memory
void GraphicsClass::Shutdown()
{
	_SHUTDOWN(pModelList_);
	_DELETE(pFrustum_);
	_SHUTDOWN(pDebugText_);

	
	//_SHUTDOWN(pCharacter2D_);
	_SHUTDOWN(pBitmap_);

	_DELETE(pLight_);

	// shaders
	_SHUTDOWN(pColorShader_);
	_SHUTDOWN(pTextureShader_);
	_SHUTDOWN(pLightShader_);

	_SHUTDOWN(pModel_);
	_SHUTDOWN(pD3D_);

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
} // Shutdown()


/*
	bool GraphicsClass::Render(InputClass* pInput,
	int fps,
	int cpu,
	float frameTime)
*/

// Executes rendering of each frame
bool GraphicsClass::RenderFrame(SystemState* systemState, 
								KeyboardEvent& kbe, 
								MouseEvent& me,
								TimerClass& timer)
{
	bool result = false;
	int renderCount = 0;  // the count of models that have been rendered for the current frame

	// Clear all the buffers before frame rendering
	this->pD3D_->BeginScene();

	
	// Generate the view matrix based on the camera's position
	editorCamera_.Render();

	// Initialize matrices
	pD3D_->GetWorldMatrix(worldMatrix_);
	pD3D_->GetProjectionMatrix(projectionMatrix_);
	pD3D_->GetOrthoMatrix(orthoMatrix_);

	// get the view matrix based on the camera's position
	editorCamera_.GetViewMatrix(viewMatrix_);

	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	editorCamera_.SetFrameTime(timer.GetTime());

	// after the frame time update the position class movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	
	editorCamera_.HandleMovement(kbe, me);
	

	editorCamera_.GetPosition(systemState->editorCameraPosition_);
	systemState->editorCameraRotation_ = editorCamera_.GetRotation();

	RenderScene(systemState);  // render all the stuff on the screen

	// Show the rendered scene on the screen
	this->pD3D_->EndScene();

	return true;
}




// memory allocation and releasing
void* GraphicsClass::operator new(size_t i)
{
	void* ptr = _aligned_malloc(i, 16);

	if (!ptr)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate memory for this object");
		return nullptr;
	}

	return ptr;
}


void GraphicsClass::operator delete(void* ptr)
{
	_aligned_free(ptr);
}


/*
// Executes some calculations and runs rendering of each frame
bool GraphicsClass::Frame(PositionClass* pPosition)
{

	// set the position of the camera
	m_Camera->SetPosition(pPosition->GetPosition());

	// set the rotation of the camera
	m_Camera->SetRotation(pPosition->GetRotation());

	return true;
} // Frame()
*/




// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //


bool GraphicsClass::InitializeDirectX(HWND hwnd, int width, int height)
{
	bool result = false;

	// Create the D3DClass object
	pD3D_ = new D3DClass();
	if (!pD3D_)
	{
		Log::Error(THIS_FUNC, "can't create the D3DClass object");
		return false;
	}

	// Initialize the DirectX stuff (device, deviceContext, swapChain, rasterizerState, viewport, etc)
	result = pD3D_->Initialize(hwnd, width, height,
		this->vsyncEnabled_,
		this->fullScreen_,
		this->screenNear_,
		this->screenDepth_);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the Direct3D");
		return false;
	}

	return true;
} // InitializeDirectX()


// initialize all the shaders (color, texture, light, etc.)
bool GraphicsClass::InitializeShaders(HWND hwnd)
{
	bool result = false;

	// ------------------------------   COLOR SHADER  ----------------------------------- //

	// create the ColorShaderClass object
	pColorShader_ = new ColorShaderClass();
	if (!pColorShader_)
	{
		Log::Error(THIS_FUNC, "can't create a ColorShaderClass object");
		return false;
	}

	// initialize the ColorShaderClass object
	result = pColorShader_->Initialize(pD3D_->GetDevice(), hwnd);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the ColorShaderClass object");
		return false;
	}

	// ----------------------------   TEXTURE SHADER   ---------------------------------- //

	// create the TextureShaderClass ojbect
	pTextureShader_ = new TextureShaderClass();
	if (!pTextureShader_)
	{
		Log::Error(THIS_FUNC, "can't create the texture shader object");
		return false;
	}

	// initialize the texture shader object
	result = pTextureShader_->Initialize(pD3D_->GetDevice(), hwnd);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the texture shader object");
		return false;
	}

	// ------------------------------   LIGHT SHADER  ----------------------------------- //

	// Create the LightShaderClass object
	pLightShader_ = new LightShaderClass();
	if (!pLightShader_)
	{
		Log::Error(THIS_FUNC, "can't create the LightShaderClass object");
		return false;
	}

	// Initialize the LightShaderClass object
	result = pLightShader_->Initialize(pD3D_->GetDevice(), hwnd);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the LightShaderClass object");
		return false;
	}

	return true;
}


// initializes all the stuff on the scene
bool GraphicsClass::InitializeScene(HWND hwnd)
{
	Log::Debug(THIS_FUNC_EMPTY);
	DirectX::XMMATRIX baseViewMatrix;

	if (!this->InitializeCamera(baseViewMatrix)) // initialize all the cameras on the scene and the engine's camera as well
		return false;

	if (!InitializeModels(hwnd))                 // initialize all the models on the scene
		return false;

	if (!this->InitializeLight(hwnd))            // initialize all the light sources on the scene
		return false;

	if (!this->InitializeGUI(hwnd, baseViewMatrix)) // initialize the GUI of the game/engine (interface elements, text, etc.)
		return false;


	return true;
}


// initialize all the list of models on the scene
bool GraphicsClass::InitializeModels(HWND hwnd)
{
	Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;
	int modelsNumber = 25;  // the number of models on the scene

	// ------------------------------ models list ------------------------------------ //
	
	// create the models list object
	pModelList_ = new ModelListClass();
	if (!pModelList_)
	{
		Log::Error(THIS_FUNC, "can't create a ModelListClass object");
		return false;
	}

	// initialize the models list object
	result = pModelList_->Initialize(modelsNumber);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the models list object");
		return false;
	}



	// -------------------------------- frustum -------------------------------------- //

	// create a frustum object
	pFrustum_ = new FrustumClass();
	if (!pFrustum_)
	{
		Log::Error(THIS_FUNC, "can't create the frustum class object");
		return false;
	}



	// ----------------------- initialize models objects ----------------------------- //

	if (!this->InitializeModel("data/models/sphere", L"data/textures/cat.dds")) // for navigation to particular file we go from the project root directory
	{
		Log::Error(THIS_FUNC, "can't initialize a model");
		return false;
	}

	

	// ----------------------- internal default models ------------------------------- //

	

	// make a RED triangle
	pTriangleRed_ = new Triangle();

	result = pTriangleRed_->Initialize(pD3D_->GetDevice(), "red triangle", { 1.0f, 0.0f, 0.0f, 1.0f });
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the red triangle");
		return false;
	}

	// setup this red triangle
	pTriangleRed_->SetPosition(0.0f, 0.0f, 0.0f);




	// make a GREEN triangle
	pTriangleGreen_ = new Triangle();

	result = pTriangleGreen_->Initialize(pD3D_->GetDevice(), "green triangle", { 0.0f, 1.0f, 0.0f, 1.0f });
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the green triangle");
		return false;
	}

	// setup this green triangle
	pTriangleGreen_->SetPosition(0.0f, 0.0f, 0.0f);
	

	return true;
} // InitializeModels()


// initializes a single model by its name and texture
bool GraphicsClass::InitializeModel(LPSTR modelName, 
	                                WCHAR* textureName)
{
	bool result = false;

	// Create the ModelClass object
	pModel_ = new ModelClass();
	if (!pModel_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the ModelClass object");
		return false;
	}

	// Initialize the ModelClass object (make a vertex and index buffer, etc)
	result = pModel_->Initialize(pD3D_->GetDevice(), modelName, textureName);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the ModelClass object");
		return false;
	}

	return true;
} // InitializeModel()


bool GraphicsClass::InitializeCamera(DirectX::XMMATRIX& baseViewMatrix)
{
	// set up the EditorCamera object
	editorCamera_.SetPosition({ 0.0f, 0.0f, -7.0f });
	editorCamera_.Render();                      // generate the view matrix
	editorCamera_.GetViewMatrix(viewMatrix_); // initialize a base view matrix with the camera for 2D user interface rendering
											 //m_Camera->SetRotation(0.0f, 1.0f, 0.0f);
	editorCamera_.GetViewMatrix(baseViewMatrix);

	return true;
}


// initialize all the light sources on the scene
bool GraphicsClass::InitializeLight(HWND hwnd)
{
	bool result = false;

	// Create the LightClass object (contains all the light data)
	pLight_ = new LightClass();
	if (!pLight_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the LightClass object");
		return false;
	}

	// Initialize the LightClass object
	pLight_->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f); // set the intensity of the ambient light to 15% white color
	pLight_->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); 
	pLight_->SetDirection(1.0f, 0.0f, 1.0f);
	pLight_->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	pLight_->SetSpecularPower(32.0f);

	return true;
}


// initialize the GUI of the game/engine (interface elements, text, etc.)
bool GraphicsClass::InitializeGUI(HWND hwnd, const DirectX::XMMATRIX& baseViewMatrix)
{
	Log::Debug(THIS_FUNC_EMPTY);

	bool result = false;


	// ----------------------------- DEBUG TEXT ------------------------------------- //
	pDebugText_ = new (std::nothrow) DebugTextClass();
	if (!pDebugText_)
	{
		Log::Error(THIS_FUNC, "can't create a debug text class object");
		return false;
	}

	// initialize the debut text class object
	result = pDebugText_->Initialize(pD3D_->GetDevice(), pD3D_->GetDeviceContext(),
		                             hwnd, screenWidth_, screenHeight_, baseViewMatrix);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't initialize the debut text class object");
		return false;
	}


	return true;
}












// renders all the stuff on the engine screen
bool GraphicsClass::RenderScene(SystemState* systemState)
{
	int renderCount = 0;  // the number of models which was rendered onto the screen

	if (!RenderModels(renderCount))
		return false;

	if (!RenderGUI(systemState))
		return false;

	return true;
}


// prepares and renders all the models on the scene
bool GraphicsClass::RenderModels(int& renderCount)
{
	DirectX::XMFLOAT3 modelPosition;   // contains a position for particular model
	DirectX::XMVECTOR modelColor;           // contains a colour of a model
	//DirectX::XMMATRIX modelWorld;      // write here some model's world matrix

	bool result = false;
	int modelCount = 0;                // the number of models that will be rendered
	bool renderModel = false;          // a flag which defines if we render a model or not
	float radius = 0.0f;               // a default radius of the model

	// timer							 
	static float t = 0.0f;
	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;

	t = (dwTimeCur - dwTimeStart) / 1000.0f;


	// construct the frustum
	pFrustum_->ConstructFrustum(screenDepth_, projectionMatrix_, viewMatrix_);

	// get the number of models that will be rendered
	modelCount = pModelList_->GetModelCount();

	if (true)
	{
		// go through all the models and render only if they can be seen by the camera view
		for (size_t index = 0; index < modelCount; index++)
		{
			// get the position and colour of the sphere model at this index
			pModelList_->GetData(static_cast<int>(index), modelPosition, modelColor);

			// set the radius of the sphere to 1.0 since this is already known
			radius = 1.0f;

			// check if the sphere model is in the view frustum
			renderModel = pFrustum_->CheckSphere(modelPosition.x, modelPosition.y, modelPosition.z, radius);

			// if it can be seen then render it, if not skip this model and check the next sphere
			if (renderModel)
			{
				// put the model vertex and index buffers on the graphics pipeline 
				// to prepare them for drawing
				pModel_->Render(pD3D_->GetDeviceContext());
				pModel_->SetPosition(modelPosition.x, modelPosition.y, modelPosition.z);   // move the model to the location it should be rendered at
				//pModel_->SetScale(2.0f, 1.0f, 1.0f);
				pModel_->SetRotation(t, 0.0f);
				
				// render the model using the light shader
				result = pLightShader_->Render(pD3D_->GetDeviceContext(),
					pModel_->GetIndexCount(),
					*(pModel_->GetWorldMatrix()), viewMatrix_, projectionMatrix_,
					pModel_->GetTexture(),
					//m_Light->GetDiffuseColor(),
					{
						DirectX::XMVectorGetX(modelColor),
						DirectX::XMVectorGetY(modelColor),
						DirectX::XMVectorGetZ(modelColor),
						1.0f
					},

					pLight_->GetDirection(),
					pLight_->GetAmbientColor(),

					editorCamera_.GetPosition(),
					pLight_->GetSpecularColor(),
					pLight_->GetSpecularPower());

				if (!result)
				{
					Log::Debug(THIS_FUNC, "can't render the model using the colour shader");
					return false;
				}
				

				// reset the world matrix to the original state
				pD3D_->GetWorldMatrix(worldMatrix_);

				// render the red triangle
				if (true)
				{
					pTriangleRed_->Render(pD3D_->GetDeviceContext());
					//pTriangleRed_->SetPosition(0.0f, 0.0f, 0.0f);


					result = pColorShader_->Render(pD3D_->GetDeviceContext(), pTriangleRed_->GetIndexCount(), *(pTriangleRed_->GetWorldMatrix()), viewMatrix_, projectionMatrix_);
					if (!result)
					{
						Log::Error(THIS_FUNC, "can't render the red triangle using the colour shader");
						return false;
					}
				}

				// render the green triangle
				if (true)
				{
					pTriangleGreen_->Render(pD3D_->GetDeviceContext());
					//pTriangleGreen_->SetPosition(0.0f, 0.0f, 0.0f);
					pTriangleGreen_->SetScale(0.3f, 0.3f, 0.3f);
				

					result = pColorShader_->Render(pD3D_->GetDeviceContext(), pTriangleGreen_->GetIndexCount(), *(pTriangleGreen_->GetWorldMatrix()), viewMatrix_, projectionMatrix_);
					if (!result)
					{
						Log::Error(THIS_FUNC, "can't render the green triangle using the colour shader");
						return false;
					}
				}

				// since this model was rendered then increase the count for this frame
				renderCount++;
			} // if
		} // for

	}
	

	return true;
} // RenderModels()

// renders the engine/game GUI
bool GraphicsClass::RenderGUI(SystemState* systemState)
{
	bool result = false;

	if (!RenderGUIDebugText(systemState))
	{
		Log::Error(THIS_FUNC, "can't render debut text");
		return false;
	}

	return true;
}


// render the debug data onto the screen in the upper-left corner
bool GraphicsClass::RenderGUIDebugText(SystemState* systemState)
{
	bool result = false;
	DirectX::XMFLOAT2 mousePos { 0.0f, 0.0f };  // pInput->GetMousePos()
	//DirectX::XMFLOAT3 cameraPos { 0.0f, 0.0f, -7.0f };
	DirectX::XMFLOAT2 cameraOrientation{ 0.0f, 0.0f };
	int cpu = 0;
	int renderModelsCount = 0;
	

	// set up the debug text data
	result = pDebugText_->SetDebugParams(mousePos, screenWidth_, screenHeight_, systemState->fps_, systemState->cpu_,
		systemState->editorCameraPosition_, 
		systemState->editorCameraRotation_,
		renderModelsCount);

	// ATTENTION: do 2D rendering only when all 3D rendering is finished
	// turn off the Z buffer to begin all 2D rendering
	pD3D_->TurnZBufferOff();

	// turn on the alpha blending before rendering the text
	pD3D_->TurnOnAlphaBlending();


	// ------------------- do rendering of the GUI here --------------------------------- //

	// render the debug text onto the screen
	result = pDebugText_->Render(pD3D_->GetDeviceContext(), worldMatrix_, orthoMatrix_);
	if (!result)
	{
		Log::Error(THIS_FUNC, "can't render the debug info onto the screen");
		return false;
	}

	// turn off alpha blending after rendering the text
	pD3D_->TurnOffAlphaBlending();

	// turn the Z buffer on now that all 2D rendering has completed
	pD3D_->TurnZBufferOn();

	
	return true;
} // RenderGUIDebugText()


/*
  // the method for initialization all the 2D stuff (text, 2D-background, 2D-characters, etc.)
bool GraphicsClass::Initialize2D(HWND hwnd, DirectX::XMMATRIX baseViewMatrix)
{
	bool result = false;

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	// ------------------------------ BACKGROUND --------------------------------- //
	// Create the bitmap with the background
	m_Bitmap = new BitmapClass();
	if (!m_Bitmap)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the BitmapClass object");
		return false;
	}

	// Initialize the bitmap object
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), m_screenWidth, m_screenHeight,
		L"grass.dds", m_screenWidth, m_screenHeight);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the bitmapClass object");
		return false;
	}

	// ------------------------------ CHARACTER 2D -------------------------------- //
	// Create the Character2D object
	m_Character2D = new(std::nothrow) Character2D();
	if (!m_Character2D)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the Character2D object");
		return false;
	}

	// Initialize the Character2D object
	result = m_Character2D->Initialize(m_D3D->GetDevice(), m_screenWidth, m_screenHeight,
		L"pot.dds", 100, 100);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the Character2D object");
		return false;
	}

	m_Character2D->SetCharacterPos(100, 100);

} // Initialize2D()




   // --------------------------------------------------------------------------- //
   //                         2D RENDERING METHOD                                 //
   // --------------------------------------------------------------------------- //

   // prepares and renders all the 2D-stuff onto the screen
bool GraphicsClass::Render2D(InputClass* pInput,
	int fps, int cpu, int renderCount)
{
	bool result = false;



	/*
	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 0, 0, 0.0f, 0.0f, 1.0f, 1.0f, 3);
	if (!result)
	{
	Log::Get()->Error(THIS_FUNC, "can't render the BitmapClass object");
	return false;
	}

	// render the bitmap with the texture shader
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(),
	m_worldMatrix, m_viewMatrix, m_orthoMatrix, m_Bitmap->GetTexture());
	if (!result)
	{
	Log::Get()->Error(THIS_FUNC, "can't render the bitmap using texture shader");
	return false;
	}


	// put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing
	result = m_Character2D->Render(m_D3D->GetDeviceContext());
	if (!result)
	{
	Log::Get()->Error(THIS_FUNC, "can't render the 2D model");
	return false;
	}

	// render the character2D with the texture shader
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Character2D->GetIndexCount(),
	m_worldMatrix, m_viewMatrix, m_orthoMatrix,
	m_Character2D->GetTexture());
	if (!result)
	{
	Log::Get()->Error(THIS_FUNC, "can't render the 2D model using texture shader");
	return false;
	}
}  // Render2D()






*/