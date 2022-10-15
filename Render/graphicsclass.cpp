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
	DirectX::XMMATRIX baseViewMatrix;

	// --------------------------------------------------------------------------- //
	//              INITIALIZE ALL THE PARTS OF GRAPHICS SYSTEM                    //
	// --------------------------------------------------------------------------- //
	
	if (!InitializeDirectX(hwnd, screenWidth, screenHeight))
	{
		Log::Error(THIS_FUNC, "can't initialize DirectX stuff");
		return false;
	}

	if (!this->InitializeCamera(baseViewMatrix))
	{
		Log::Error(THIS_FUNC, "can't initialize the camera");
		return false;
	}

/*

// ------------------------------ TEXTURE SHADER ------------------------------ //
// create the texture shader object
m_TextureShader = new TextureShaderClass();
if (!m_TextureShader)
{
	Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the TextureShaderClass object");
	return false;
}

// initialize the texture shader object
result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
if (!result)
{
	Log::Get()->Error(THIS_FUNC, "can't initialize the TextureShaderClass object");
	return false;
}



// --------------------------- MODEL LIST ------------------------------------------- //
// create the model list object
m_pModelList = new ModelListClass();
if (!m_pModelList)
{
	Log::Get()->Error(THIS_FUNC, "can't create a ModelListClass object");
	return false;
}

// initialize the model list object 
int modelsNumber = 25;

result = m_pModelList->Initialize(modelsNumber);
if (!result)
{
	Log::Get()->Error(THIS_FUNC, "can't initialize the model list object");
	return false;
}


// ------------------------------ FRUSTUM ------------------------------------------- //
// create the frustum object
m_pFrustum = new FrustumClass();
if (!m_pFrustum)
{
	Log::Get()->Error(THIS_FUNC, "can't create the frustum class object");
	return false;
}




// --------------------------------------------------------------------------- //
//                                  3D                                         //
// --------------------------------------------------------------------------- //
result = Initialize3D(m_D3D, hwnd);
if (!result)
{
	Log::Get()->Error(THIS_FUNC, "there is an error during initialization of the 3D-module");
	return false;
}


// --------------------------------------------------------------------------- //
//                                  2D                                         //
// --------------------------------------------------------------------------- //
result = Initialize2D(hwnd, baseViewMatrix);
if (!result)
{
	Log::Get()->Error(THIS_FUNC, "there is an error during initialization of the 2D-module");
	return false;
}


*/


	Log::Print(THIS_FUNC, " is successfully initialized");
	return true;
}

// Shutdowns all the graphics rendering parts, releases the memory
void GraphicsClass::Shutdown()
{
	_SHUTDOWN(pModelList_);
	_DELETE(pFrustum_);
	_SHUTDOWN(pDebugText_);

	_SHUTDOWN(pTextureShader_);
	//_SHUTDOWN(pCharacter2D_);
	_SHUTDOWN(pBitmap_);

	_DELETE(pLight_);
	_SHUTDOWN(pLightShader_);
	//_DELETE(m_pMoveLook);

	_DELETE(pCamera_);
	_SHUTDOWN(pModel_);
	_SHUTDOWN(pD3D_);

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
} // Shutdown()


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
	pCamera_->Render();
	pCamera_->GetViewMatrix(baseViewMatrix); // initialize a base view matrix with the camera for 2D user interface rendering
	//m_Camera->SetRotation(0.0f, 1.0f, 0.0f);

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



  // Executes rendering of each frame
bool GraphicsClass::Render(InputClass* pInput,
	int fps,
	int cpu,
	float frameTime)
{

	bool result = false;
	int renderCount = 0;     // the count of models that have been rendered for the current frame

							 // Clear all the buffers before frame rendering
	m_D3D->BeginScene(0.2f, 0.4f, 0.6f, 1.0f);

	// Generate the view matrix based on the camera's position
	m_Camera->Render();

	// Initialize matrices
	m_D3D->GetWorldMatrix(m_worldMatrix);
	m_D3D->GetProjectionMatrix(m_projectionMatrix);
	m_D3D->GetOrthoMatrix(m_orthoMatrix);

	// get the view matrix based on the camera's position
	m_Camera->GetViewMatrix(m_viewMatrix);

	// render all the stuff on the screen
	result = Render3D(renderCount);
	result = Render2D(pInput, fps, cpu, renderCount);

	// Show the rendered scene on the screen
	m_D3D->EndScene();

	return true;
} // Render()






// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //

// the method for initialization all the 3D stuff (directed lighting, 3D models, etc)
bool GraphicsClass::Initialize3D(D3DClass* m_D3D, HWND hwnd)
{
	bool result = false;

	Log::Get()->Debug(THIS_FUNC_EMPTY);

	// ------------------------------ MODEL -------------------------------------- //
	// Create the ModelClass object

	m_Model = new ModelClass();
	if (!m_Model)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the ModelClass object");
		return false;
	}

	// Initialize the ModelClass object (vertex and index buffer, etc)
	result = m_Model->Initialize(m_D3D->GetDevice(), "sphere.txt", L"cat.dds");
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the ModelClass object");
		return false;
	}

	// ------------------------------ LIGHT SHADER -------------------------------------- //
	// Create the LightShaderClass object
	m_LightShader = new LightShaderClass();
	if (!m_LightShader)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the LightShaderClass object");
		return false;
	}

	// Initialize the LightShaderClass object
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the LightShaderClass object");
		return false;
	}

	// ----------------------------------- LIGHT ---------------------------------------- //
	// Create the LightClass object
	m_Light = new LightClass();
	if (!m_Light)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the LightClass object");
		return false;
	}

	// Initialize the LightClass object
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f); // set the intensity of the ambient light to 15% white color
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); // cyan
	m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	return true;
} // Initialize3D()


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

	// ----------------------------- DEBUG TEXT ------------------------------------- //
	m_pDebugText = new (std::nothrow) DebugTextClass;
	if (!m_pDebugText)
	{
		Log::Get()->Error(THIS_FUNC, "can't create a debug text class object");
		return false;
	}

	result = m_pDebugText->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd,
		m_screenWidth, m_screenHeight,
		baseViewMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the debug text class object");
		return false;
	}



	return true;
} // Initialize2D()





  // --------------------------------------------------------------------------- //
  //                         3D RENDERING METHOD                                 //
  // --------------------------------------------------------------------------- //

  // prepares and renders all the 3D-stuff onto the screen
bool GraphicsClass::Render3D(int& renderCount)
{
	bool result = false;
	int modelCount = 0;      // the number of models that will be rendered
	float posX = 0.0f;       // x-axis position of a model
	float posY = 0.0f;       // y-axis position of a model
	float posZ = 0.0f;       // z-axis position of a model
	float radius = 0.0f;     // a radius of a sphere model
	DirectX::XMVECTOR color{ 0.0f, 0.0f, 0.0f, 1.0f };  // colour of a model
	bool renderModel = false; // a flag which defines if we render a model or not

							  // construct the frustum
	m_pFrustum->ConstructFrustum(SCREEN_DEPTH, m_projectionMatrix, m_viewMatrix);

	// get the number of models that will be rendered 
	modelCount = m_pModelList->GetModelCount();

	// go through all the models and render only if they can be ssen by the camera view
	for (size_t index = 0; index < modelCount; index++)
	{
		// get the position and colour of the sphere model at this index
		m_pModelList->GetData(static_cast<int>(index), posX, posY, posZ, color);

		// set the radius of the sphere to 1.0 since this is already known
		radius = 1.0f;

		// check if the sphere model is in the view frustum
		renderModel = m_pFrustum->CheckSphere(posX, posY, posZ, radius);

		// if it can be seen then render it, if not skip this model and check the next sphere
		if (renderModel)
		{
			// move the model to the location if should be rendered at
			m_worldMatrix = DirectX::XMMatrixTranslation(posX, posY, posZ);

			// put the model vertex and index buffers on the graphics pipeline 
			// to prepare them for drawing
			m_Model->Render(m_D3D->GetDeviceContext());

			// render the model using the light shader
			result = m_LightShader->Render(m_D3D->GetDeviceContext(),
				m_Model->GetIndexCount(),
				m_worldMatrix, m_viewMatrix, m_projectionMatrix,
				m_Model->GetTexture(),
				//m_Light->GetDiffuseColor(),
				{
					DirectX::XMVectorGetX(color),
					DirectX::XMVectorGetY(color),
					DirectX::XMVectorGetZ(color),
					1.0f
				},

				m_Light->GetDirection(),
				m_Light->GetAmbientColor(),

				m_Camera->GetPosition(),
				m_Light->GetSpecularColor(),
				m_Light->GetSpecularPower());

			if (!result)
			{
				Log::Get()->Error(THIS_FUNC, "can't render the model using HLSL shaders");
				return false;
			}

			// reset to the original world matrix
			m_D3D->GetWorldMatrix(m_worldMatrix);

			// since this model was rendered then increase the count for this frame
			renderCount++;
		} // if
	} // for

	return true;
}  // Render3D()


   // --------------------------------------------------------------------------- //
   //                         2D RENDERING METHOD                                 //
   // --------------------------------------------------------------------------- //

   // prepares and renders all the 2D-stuff onto the screen
bool GraphicsClass::Render2D(InputClass* pInput,
	int fps, int cpu, int renderCount)
{
	bool result = false;

	// ATTENTION: do 2D rendering only when all 3D rendering is finished
	// turn off the Z buffer to begin all 2D rendering
	m_D3D->TurnZBufferOff();

	// turn on the alpha blending before rendering the text
	m_D3D->TurnOnAlphaBlending();

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
	



	// ----------------------------- PRINT DEBUG DATA ----------------------------------- //

	// set the location of the mouse
	result = m_pDebugText->SetMousePosition(pInput->GetMousePos());
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the mouse position for output");
	}

	// set the frames per second
	result = m_pDebugText->SetFps(fps);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the fps parameter for output");
		return false;
	}

	result = m_pDebugText->SetDisplayParams(m_screenWidth, m_screenHeight);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the display params for output");
	}

	// set the string with camera position data
	result = m_pDebugText->SetCameraPosition({ m_Camera->GetPosition() });
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the camera position for output");
	}

	// set the string with camera orientation data (pass the pitch and yaw of the camera)
	result = m_pDebugText->SetCameraOrientation(m_Camera->GetRotation());
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the camera orientation for output");
	}

	// set the number of rendered models
	result = m_pDebugText->SetRenderCount(renderCount);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the number of rendered models for output");
	}

	// set the cpu usage
	result = m_pDebugText->SetCpu(cpu);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the cpu for output");
		return false;
	}

	// render all the debug text onto the screen
	result = m_pDebugText->Render(m_D3D->GetDeviceContext(), m_worldMatrix, m_orthoMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the debug info onto the screen");
		return false;
	}




	// turn off alpha blending after rendering the text
	m_D3D->TurnOffAlphaBlending();

	// turn the Z buffer on now that all 2D rendering has completed
	m_D3D->TurnZBufferOn();


	return true;
}  // Render2D()






*/