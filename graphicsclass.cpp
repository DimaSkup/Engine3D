////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
// Revising: 18.04.22
////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

GraphicsClass::GraphicsClass(void)
{
	m_D3D = nullptr;
	//m_Model = nullptr;
	m_Camera = nullptr;
	//m_LightShader = nullptr;
	//m_Light = nullptr;

	m_Bitmap = nullptr;
	m_TextureShader = nullptr;

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
	/*
	m_Model = new(std::nothrow) ModelClass();
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
	*/


	// ------------------------------ CAMERA -------------------------------------- //
	// Create the CameraClass object
	m_Camera = new(std::nothrow) CameraClass();
	if (!m_Camera)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the CameraClass object");
		return false;
	}

	// Initialize the CameraClass object
	m_Camera->SetPosition(0.0f, 0.0f, -7.0f);
	//m_Camera->SetRotation(0.0f, 1.0f, 0.0f);

/*
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
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f); // set the intensity of the ambient light to 15% white color
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); // cyan
	m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);
*/


	// ------------------------------ TEXTURE SHADER ------------------------------ //
	// create the texture shader object
	m_TextureShader = new(std::nothrow) TextureShaderClass();
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


	// ------------------------------ BITMAP -------------------------------------- //
	// create the bitmap object
	m_Bitmap = new(std::nothrow) BitmapClass();
	if (!m_Bitmap)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the BitmapClass object");
		return false;
	}

	// initialize the bitmap object
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight,
		                          L"cat.dds", 256, 256);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the BitmapClass object");
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
	_SHUTDOWN(m_Bitmap);
	_SHUTDOWN(m_TextureShader);
	_DELETE(m_Camera);
	//_SHUTDOWN(m_Model);
	_SHUTDOWN(m_D3D);
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	return;
}

// Executes some calculations and runs rendering of each frame
bool GraphicsClass::Frame()
{
	// value of a rotation angle
	static float rotation = 0.0f;

	//rotation += 0.001f; // update the rotation variable each frame
	rotation += (float)D3DX_PI * 0.0005f;    // update the rotation variable each frame

	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	

	if (!Render(rotation))
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
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix; 	// matrices variables
	bool result = false;

	// Clear all the buffers before frame rendering
	m_D3D->BeginScene(1.0f, .5f, 0.0f, 1.0f);

	// Generate the view matrix
	m_Camera->Render();

	// Initialize matrices
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	
	
	/*
	// rotate the worldMatrix arond it's world, etc
	D3DXMATRIX mScale, mSpin, mTranslate, mOrbit;

	D3DXMatrixScaling(&mScale, 0.3f, 0.3f, 0.3f);
	D3DXMatrixRotationZ(&mSpin, -rotation);
	D3DXMatrixTranslation(&mTranslate, -4.0f, 0.0f, 0.0f);
	D3DXMatrixRotationY(&mOrbit, -rotation * 2.0f);
	

	worldMatrix = mScale * mSpin * mTranslate * mOrbit;
	*/

	
    /*
	// rotate the world matrix
	D3DXMatrixRotationY(&worldMatrix, rotation);

	

	// Setup pipeline parts for rendering of the model
	m_Model->Render(m_D3D->GetDeviceContext());

	// render the model using HLSL shaders
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(),
		                           worldMatrix, viewMatrix, projectionMatrix,
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


	*/


	// ATTENTION: do 2D rendering only when all 3D rendering is finished
	// turn off the Z buffer to begin all 2D rendering
	m_D3D->TurnZBufferOff();

	// put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing
	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 100, 100);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the 2D model");
		return false;
	}

	// render the bitmap with the texture shader
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(),
		                             worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the 2D model using texture shader");
		return false;
	}

	// turn the Z buffer on now that all 2D rendering has completed
	m_D3D->TurnZBufferOn();

	// Show the rendered scene on the screen
	m_D3D->EndScene();

	return true;
}