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

	_DELETE(pCamera_);
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
bool GraphicsClass::RenderFrame()
{
	bool result = false;
	int renderCount = 0;  // the count of models that have been rendered for the current frame

	// Clear all the buffers before frame rendering
	this->pD3D_->BeginScene();

	
	// Generate the view matrix based on the camera's position
	pCamera_->Render();

	// Initialize matrices
	pD3D_->GetWorldMatrix(worldMatrix_);
	pD3D_->GetProjectionMatrix(projectionMatrix_);
	pD3D_->GetOrthoMatrix(orthoMatrix_);

	// get the view matrix based on the camera's position
	pCamera_->GetViewMatrix(viewMatrix_);

	RenderScene();  // render all the stuff on the screen

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
	pTriangleRed_ = new ModelClass();   // make a red triangle
	if (!pTriangleRed_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the red triangle model object");
		return false;
	}

	pTriangleRed_ = new ModelClass();   // make a green triangle
	if (!pTriangleRed_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the green triangle model object");
		return false;
	}


	// make vertex data for the RED triangle 
	const int trVerticesCount = 3;
	VERTEX* trVertices = new VERTEX[trVerticesCount];
	
	trVertices[0] = VERTEX(-0.5f, -0.5f, 1.0f,     0, 0,  0, 0, 0,    1.0f, 0.0f, 0.0f);  // bottom left (position / texture / normal / colour)
	trVertices[1] = VERTEX( 0.0f,  0.5f, 1.0f,     0, 0,  0, 0, 0,    1.0f, 0.0f, 0.0f);  // top middle
	trVertices[2] = VERTEX( 0.5f, -0.5f, 1.0f,     0, 0,  0, 0, 0,    1.0f, 0.0f, 0.0f);  // bottom right

	

	if (!pTriangleRed_->Initialize(pD3D_->GetDevice(), trVertices, trVerticesCount, "red triangle"))
	{
		Log::Error(THIS_FUNC, "can't initialize the red triangle");
	}

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
	// Create the CameraClass object
	pCamera_ = new CameraClass();
	if (!pCamera_)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the CameraClass object");
		return false;
	}

	// set up the CameraClass object
	pCamera_->SetPosition({ 0.0f, 0.0f, -7.0f });
	pCamera_->Render();                      // generate the view matrix
	pCamera_->GetViewMatrix(viewMatrix_); // initialize a base view matrix with the camera for 2D user interface rendering
											 //m_Camera->SetRotation(0.0f, 1.0f, 0.0f);
	pCamera_->GetViewMatrix(baseViewMatrix);

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
bool GraphicsClass::RenderScene()
{
	int renderCount = 0;  // the number of models which was rendered onto the screen

	if (!RenderModels(renderCount))
		return false;

	if (!RenderGUI())
		return false;

	return true;
}


// prepares and renders all the models on the scene
bool GraphicsClass::RenderModels(int& renderCount)
{
	bool result = false;
	int modelCount = 0;      // the number of models that will be rendered
	float posX = 0.0f;       // x-axis position of a model
	float posY = 0.0f;       // y-axis position of a model
	float posZ = 0.0f;       // z-axis position of a model
	float radius = 0.0f;     // a radius of a sphere model
	DirectX::XMVECTOR color{ 0.0f, 0.0f, 0.0f, 1.0f };  // default colour of a model
	bool renderModel = false; // a flag which defines if we render a model or not
	static float angle = 0.0f;


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
			pModelList_->GetData(static_cast<int>(index), posX, posY, posZ, color);

			// set the radius of the sphere to 1.0 since this is already known
			radius = 1.0f;

			// check if the sphere model is in the view frustum
			renderModel = pFrustum_->CheckSphere(posX, posY, posZ, radius);

			// if it can be seen then render it, if not skip this model and check the next sphere
			if (renderModel)
			{
				// move the model to the location it should be rendered at
				worldMatrix_ = DirectX::XMMatrixTranslation(posX, posY, posZ);


				// put the model vertex and index buffers on the graphics pipeline 
				// to prepare them for drawing
				pModel_->Render(pD3D_->GetDeviceContext());

				// render the model using the light shader
				result = pLightShader_->Render(pD3D_->GetDeviceContext(),
					pModel_->GetIndexCount(),
					worldMatrix_, viewMatrix_, projectionMatrix_,
					pModel_->GetTexture(),
					//m_Light->GetDiffuseColor(),
					{
						DirectX::XMVectorGetX(color),
						DirectX::XMVectorGetY(color),
						DirectX::XMVectorGetZ(color),
						1.0f
					},

					pLight_->GetDirection(),
					pLight_->GetAmbientColor(),

					pCamera_->GetPosition(),
					pLight_->GetSpecularColor(),
					pLight_->GetSpecularPower());

				if (!result)
				{
					Log::Debug(THIS_FUNC, "can't render the model using the colour shader");
					return false;
				}

				// reset the world matrix to the original state
				pD3D_->GetWorldMatrix(worldMatrix_);



				
				angle += 0.1f;
				//Log::Print("%f", angle);
				DirectX::XMMATRIX rotateX = DirectX::XMMatrixRotationX(angle);
				DirectX::XMMATRIX rotateY = DirectX::XMMatrixRotationY(angle);
				DirectX::XMMATRIX worldMatrixForTriangle = worldMatrix_ * rotateX * rotateY;

				pTriangleRed_->Render(pD3D_->GetDeviceContext());
				//result = pColorShader_->Render(pD3D_->GetDeviceContext(), pTriangleRed_->GetIndexCount(), worldMatrixForTriangle, viewMatrix_, projectionMatrix_);
				if (!result)
				{
					//Log::Error(THIS_FUNC, "can't render the red triangle using the colour shader");
				}

				// render the model using the light shader
				result = pLightShader_->Render(pD3D_->GetDeviceContext(),
					pTriangleRed_->GetIndexCount(),
					worldMatrixForTriangle, viewMatrix_, projectionMatrix_,
					pModel_->GetTexture(),
					//m_Light->GetDiffuseColor(),
					{
						DirectX::XMVectorGetX(color),
						DirectX::XMVectorGetY(color),
						DirectX::XMVectorGetZ(color),
						1.0f
					},

					pLight_->GetDirection(),
					pLight_->GetAmbientColor(),

					pCamera_->GetPosition(),
					pLight_->GetSpecularColor(),
					pLight_->GetSpecularPower());



				// since this model was rendered then increase the count for this frame
				renderCount++;
			} // if
		} // for

	}
	



	/*
	result = pColorShader_->Render(pD3D_->GetDeviceContext(), pModel_->GetIndexCount(),
	worldMatrix_, viewMatrix_, projectionMatrix_);
	*/

	

	return true;
}

// renders the engine/game GUI
bool GraphicsClass::RenderGUI()
{
	bool result = false;

	if (!RenderGUIDebugText())
	{
		Log::Error(THIS_FUNC, "can't render debut text");
		return false;
	}

	return true;
}


// render the debug data onto the screen in the upper-left corner
bool GraphicsClass::RenderGUIDebugText()
{
	bool result = false;
	DirectX::XMFLOAT2 mousePos { 0.0f, 0.0f };  // pInput->GetMousePos()
	DirectX::XMFLOAT3 cameraPos { 0.0f, 0.0f, -7.0f };
	DirectX::XMFLOAT2 cameraOrientation{ 0.0f, 0.0f };
	int fps = 60;
	int cpu = 0;
	int renderModelsCount = 0;

	// set up the debug text data
	result = pDebugText_->SetDebugParams(mousePos, screenWidth_, screenHeight_, fps, cpu,
		cameraPos, cameraOrientation, renderModelsCount);

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