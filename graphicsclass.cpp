/////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
/////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include "includes.h"
#include "log.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	//m_ColorShader = nullptr;
	m_TextureShader = nullptr;

	FULL_SCREEN = false;
}

GraphicsClass::GraphicsClass(const GraphicsClass& another)
{
}

GraphicsClass::~GraphicsClass()
{
}


// -----------------------------------------
//
//            PUBLIC FUNCTIONS
//
// -----------------------------------------

// Here we initialize all the main parts of the engine
// like the Direct3D, camera, models, shaders
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, bool fullScreen)
{
	Log::Get()->Debug(__FUNCTION__, __LINE__);

	// Create the Direct3D object
	m_D3D = new(std::nothrow) D3DClass();
	if (!m_D3D)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the Direct3D object");
		return false;
	}

	FULL_SCREEN = fullScreen;
	
	// Initialize the Direct3D object
	if (!m_D3D->Initialize(screenWidth,
							screenHeight,
							VSYNC_ENABLED,
							hwnd,
							FULL_SCREEN,
							SCREEN_DEPTH,
							SCREEN_NEAR))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the Direct3D object");
		return false;
	}
	Log::Get()->Debug(THIS_FUNC, "Direct3D object is initialized successfully");
	
	// Create the camera object
	m_Camera = new(std::nothrow) CameraClass();
	if (!m_Camera)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the camera object");
		return false;
	}

	// Set the initial position of the camera
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	Log::Get()->Debug(THIS_FUNC, "Camera object is initialized successfully");



	// Create the model object
	m_Model = new(std::nothrow) ModelClass();
	if (!m_Model)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the model object");
		return false;
	}
	
	// Initialize the model object
	if (!m_Model->Initialize(m_D3D->GetDevice(), L"angel.dds"))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the model object");
		return false;
	}
	Log::Get()->Debug(THIS_FUNC, "Model object is initialized successfully");



	// Create the texture shader object
	m_TextureShader = new(std::nothrow) TextureShaderClass();
	if (!m_TextureShader)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the texture shader object");
		return false;
	}

	// Initialize the texture shader object
	if (!m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd))
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the texture shader object");
		return false;
	}
	

	return true;
}


void GraphicsClass::Shutdown(void)
{
	Log::Get()->Debug(THIS_FUNC);

	//_SHUTDOWN(m_ColorShader);
	_SHUTDOWN(m_TextureShader);
	_SHUTDOWN(m_Model);
	_DELETE(m_Camera);
	_SHUTDOWN(m_D3D);

	return;
}

bool GraphicsClass::Frame(void)
{
	if (!Render())
	{
		Log::Get()->Error(THIS_FUNC, "there is something went wrong during the frame rendering");
		return false;
	}

	return true;
}


// -----------------------------------------
//
//            PRIVATE FUNCTIONS
//
// -----------------------------------------
bool GraphicsClass::Render(void)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	// Clear the buffers to begin the scene
	m_D3D->BeginScene(0.2f, 0.4f, 0.6f, 1.0f);



	// Generate the view matrix based on the camera's position
	m_Camera->Render();

	// Get the world, view and projection matrices from the camera and d3d objects
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare 
	// them for drawing
	m_Model->Render(m_D3D->GetDeviceContext());

	// Render the model using the texture shader
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(),
		                             m_Model->GetIndexCount(),
		                             worldMatrix,
		                             viewMatrix,
		                             projectionMatrix,
		                             m_Model->GetTexture());
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the model using the texture shader");
		return false;
	}

	// Show the rendered scene to the screen
	m_D3D->EndScene();

	return true;
}