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

	// ------------------------------ DIRECT3D -------------------------------------- //
	// Create the Direct3D object
	m_D3D = new(std::nothrow) D3DClass();
	if (!m_D3D)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the D3DClass object");
		return false;
	}

	// Initialize the Direct3D (device, deviceContext, swapChain, rasterizerState, viewport, etc)
	result = m_D3D->Initialize(screenWidth, screenHeight, 
		                       VSYNC_ENABLED, hwnd, fullscreen,
		                       SCREEN_NEAR, SCREEN_DEPTH);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the Direct3D");
		return false;
	}

	// ------------------------------ MODEL -------------------------------------- //
	// Create the ModelClass object
	m_Model = new(std::nothrow) ModelClass();
	if (!m_Model)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the ModelClass object");
		return false;
	}

	// Initialize the ModelClass object (vertex and index buffer, etc)
	result = m_Model->Initialize(m_D3D->GetDevice(), L"angel.dds");
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the ModelClass object");
		return false;
	}

	// ------------------------------ CAMERA -------------------------------------- //
	// Create the CameraClass object
	m_Camera = new(std::nothrow) CameraClass();
	if (!m_Camera)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the CameraClass object");
		return false;
	}

	// Initialize the CameraClass object
	m_Camera->SetPosition(0.0f, 2.0f, -5.0f);
	//m_Camera->SetRotation(0.0f, 1.0f, 0.0f);

	// ------------------------------ LIGHT SHADER -------------------------------------- //
	// Create the LightShaderClass object
	m_LightShader = new(std::nothrow) LightShaderClass();
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
	m_Light = new(std::nothrow) LightClass();
	if (!m_Light)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the LightClass object");
		return false;
	}

	// Initialize the LightClass object
	m_Light->SetDiffuseColor(0.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);


	Log::Get()->Debug(THIS_FUNC, "GraphicsClass is successfully initialized");
	return true;
}

// Shutdowns all the graphics rendering parts, releases the memory
void GraphicsClass::Shutdown()
{
	_DELETE(m_Light);
	_SHUTDOWN(m_LightShader);
	_DELETE(m_Camera);
	_SHUTDOWN(m_Model);
	_SHUTDOWN(m_D3D);
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
}

// Executes some calculations and runs rendering of each frame
bool GraphicsClass::Frame()
{
	// value of a rotation angle
	static float rotation = 0.0f;

	if (rotation == 360.0f)
		rotation == 0.0f;

	rotation += 0.01f;

	if (!Render(0.0f))
	{
		Log::Get()->Error(THIS_FUNC, "something went wrong during frame rendering");
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------------- //
// 
//                             PRIVATE METHODS 
//
// ----------------------------------------------------------------------------------- //

// Executes rendering of each frame
bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix; 	// matrices variables
	bool result = false;

	// Clear all the buffers before frame rendering
	m_D3D->BeginScene(1.0f, .5f, 0.0f, 1.0f);

	// Generate the view matrix
	m_Camera->Render();

	// Initialize matrices
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);

	// rotate the world matrix
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Setup pipeline parts for rendering of the model
	m_Model->Render(m_D3D->GetDeviceContext());

	// render the model using HLSL shaders
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(),
		                           worldMatrix, viewMatrix, projectionMatrix,
		                           m_Model->GetTexture(), 
		                           m_Light->GetDiffuseColor(), m_Light->GetDirection());
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the model using HLSL shaders");
		return false;
	}

	// Show the rendered scene on the screen
	m_D3D->EndScene();

	return true;
}