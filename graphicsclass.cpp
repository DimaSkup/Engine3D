////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 18.04.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

GraphicsClass::GraphicsClass(void)
{
	m_D3D = nullptr;
	m_Model = nullptr;
	m_Camera = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;

	m_Bitmap = nullptr;
	m_Character2D = nullptr;
	m_TextureShader = nullptr;
	m_pText = nullptr;

	FULL_SCREEN = false;
}

GraphicsClass::GraphicsClass(const GraphicsClass& another)
{
}

GraphicsClass::~GraphicsClass(void)
{
}

// ----------------------------------------------------------------------------------- //
// 
//                             PUBLIC METHODS 
//
// ----------------------------------------------------------------------------------- //
// Initializes all the main parts of graphics rendering module
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, bool fullscreen)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	bool result = false;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	DirectX::XMMATRIX baseViewMatrix;

	// --------------------------------------------------------------------------- //
	//                                 COMMON                                      //
	// --------------------------------------------------------------------------- //

	// ------------------------------ DIRECT3D ----------------------------------- //
	// Create the Direct3D object
	m_D3D = new D3DClass();
	if (!m_D3D)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the D3DClass object");
		return false;
	}

	// Initialize the Direct3D (device, deviceContext, swapChain, 
	// rasterizerState, viewport, etc)
	result = m_D3D->Initialize(screenWidth, screenHeight, 
		                       VSYNC_ENABLED, hwnd, fullscreen,
		                       SCREEN_NEAR, SCREEN_DEPTH);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the Direct3D");
		return false;
	}


	// ------------------------------ CAMERA -------------------------------------- //
	// Create the CameraClass object
	m_Camera = new CameraClass();
	if (!m_Camera)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the CameraClass object");
		return false;
	}

	// Initialize the CameraClass object
	m_Camera->SetPosition(0.0f, 0.0f, -7.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix); // initialize a base view matrix with the camera for 2D user interface rendering
											 //m_Camera->SetRotation(0.0f, 1.0f, 0.0f);



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


	// --------------------------------------------------------------------------- //
	//                                  3D                                         //
	// --------------------------------------------------------------------------- //
	//result = Initialize3D(m_D3D, hwnd);
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
	



	Log::Get()->Debug(THIS_FUNC, "GraphicsClass is successfully initialized");
	return true;
}

// Shutdowns all the graphics rendering parts, releases the memory
void GraphicsClass::Shutdown()
{
	//_DELETE(m_Light);
	//_SHUTDOWN(m_LightShader);
	_SHUTDOWN(m_pText);
	_SHUTDOWN(m_Bitmap);
	_SHUTDOWN(m_Character2D);
	_SHUTDOWN(m_TextureShader);
	_DELETE(m_Camera);
	//_SHUTDOWN(m_Model);
	_SHUTDOWN(m_D3D);
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
}

// Executes some calculations and runs rendering of each frame
bool GraphicsClass::Frame(InputClass* pInput, int fps, int cpu, float frameTime)
{
	// value of a rotation angle
	static float rotation = 0.0f;
	bool result = false;

	//rotation += 0.001f; // update the rotation variable each frame
	rotation += (float)D3DX_PI * 0.0005f;    // update the rotation variable each frame

	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// try to render this frame
	result = Render(rotation,
		            pInput->GetActiveKeyCode(),
		            pInput->GetMousePos(), 
		            fps, cpu, frameTime);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "something went wrong during frame rendering");
		return false;
	}

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

// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //

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
	result = m_Model->Initialize(m_D3D->GetDevice(), "sphere_high.txt", L"cat.dds");
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

	// ------------------------------ LIGHT -------------------------------------- //
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
}


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




	// -------------------------------- TEXT ------------------------------------ //
	m_pText = new TextClass;
	if (!m_pText)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for TextClass object");
		return false;
	}

	result = m_pText->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd,
		m_screenWidth, m_screenHeight, "text.txt", baseViewMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the text object");
		return false;
	}

	// set some engine params for rendering on the screen
	//Log::Get()->Print("WIDTH = %d", m_screenWidth);
	m_pText->SetDisplayParams(m_screenWidth, m_screenHeight);

	return true;
}





// Executes rendering of each frame
bool GraphicsClass::Render(float rotation, 
	                       int activeKeyCode, 
	                       POINT mousePos,
	                       int fps,
	                       int cpu,
	                       float frameTime)
{
	bool result = false;

	// Clear all the buffers before frame rendering
	m_D3D->BeginScene(0.2f, 0.4f, 0.6f, 1.0f);

	// Generate the view matrix
	m_Camera->Render();

	// Initialize matrices
	m_D3D->GetWorldMatrix(m_worldMatrix);
	m_D3D->GetProjectionMatrix(m_projectionMatrix);
	m_Camera->GetViewMatrix(m_viewMatrix);
	m_D3D->GetOrthoMatrix(m_orthoMatrix);

	static int posX_2D = 100;
	static int posY_2D = 100;


	//result = Render3D(rotation);
	result = Render2D(rotation, mousePos, fps, cpu);


	// Show the rendered scene on the screen
	m_D3D->EndScene();

	return true;
}


// --------------------------------------------------------------------------- //
//                                  3D                                         //
// --------------------------------------------------------------------------- //
bool GraphicsClass::Render3D(float rotation)
{
	bool result = false;
	DirectX::XMMATRIX translationMatrix;
	


	/*
	DirectX::XMMATRIX mScale, mSpin, mTranslate, mOrbit;

	// rotate the worldMatrix
	mScale = DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f);
	mSpin = DirectX::XMMatrixRotationZ(-rotation);
	mTranslate = DirectX::XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	mOrbit = DirectX::XMMatrixRotationY(-rotation * 2.0f);

	// calculate the world matrix
	m_worldMatrix = mScale * mSpin * mTranslate * mOrbit;

	//m_worldMatrix = DirectX::XMMatrixRotationZ(rotation);
	*/


	// rotate the view matrix
	translationMatrix = DirectX::XMMatrixRotationY(rotation);
	m_viewMatrix = translationMatrix * m_viewMatrix;

	
	// Setup pipeline parts for rendering of the model
	m_Model->Render(m_D3D->GetDeviceContext());

	// render the model using HLSL shaders
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(),
									m_worldMatrix, m_viewMatrix, m_projectionMatrix,
									m_Model->GetTexture(),
									m_Light->GetDiffuseColor(),
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

	return true;
}


// --------------------------------------------------------------------------- //
//                                  2D                                         //
// --------------------------------------------------------------------------- //
bool GraphicsClass::Render2D(float rotation, POINT mousePos, int fps, int cpu)
{
	bool result = false;

	// ATTENTION: do 2D rendering only when all 3D rendering is finished
	// turn off the Z buffer to begin all 2D rendering
	m_D3D->TurnZBufferOff();

	// turn on the alpha blending before rendering the text
	m_D3D->TurnOnAlphaBlending();


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

	// set the location of the mouse
	result = m_pText->SetMousePosition(mousePos);


	// ---------------------- PRINT FPS, CPU AND TIMER DATA ---------------------------- //



	// set the frames per second
	result = m_pText->SetFps(fps);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the fps for the text object");
		return false;
	}
	
	// set the cpu usage
	result = m_pText->SetCpu(cpu);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't set the cpu for the text object");
		return false;
	}

	// --------------- render the TEXT strings on the screen ------------------------- //
	result = m_pText->Render(m_D3D->GetDeviceContext(), m_worldMatrix, m_orthoMatrix);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the text strings");
		return false;
	}


	// turn off alpha blending after rendering the text
	m_D3D->TurnOffAlphaBlending();

	// turn the Z buffer on now that all 2D rendering has completed
	m_D3D->TurnZBufferOn();


	return true;
}