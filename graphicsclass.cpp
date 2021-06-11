//////////////////////////////////
// Filename: graphicsclass.cpp
//////////////////////////////////
#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = nullptr;
	m_Camera = nullptr;
	//m_Model = nullptr;
	//m_LightShader = nullptr;
	//m_Light = nullptr;
	//m_TextureShader = nullptr;
	//m_Bitmap = nullptr;
	m_Text = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	D3DXMATRIX baseViewMatrix;

	// Create the Direct3D object
	m_D3D = new D3DClass;
	
	if (!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd,
								FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);

		return false;
	}



	// Create the camera object
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);


	// Create the text object
	m_Text = new TextClass;

	if (!m_Text)
	{
		return false;
	}

	// Intialize the text object
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(),
								hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object", L"Error", MB_OK);
		return false;
	}

	return true;

	/*
	// Create the model object
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// Initialize the model object
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/arekisanda.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object", L"Error", MB_OK);

		return false;
	}

	// Create the light object 
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);
	


	// Create the texture shader object
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
		return false;
	}



	// Create the bitmap object
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
	{
		return false;
	}

	// Initialize the bitmap object
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), 
									screenWidth, screenHeight, 
									L"../Engine/data/arekisanda.dds",
									400, 400);

	
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object", L"Error", MB_OK);
		return false;
	}

	*/

	return true;
}

void GraphicsClass::Shutdown()
{
/*

// Release the light object
if (m_Light)
{
delete m_Light;
m_Light = nullptr;
}

// Release the light shader object
if (m_LightShader)
{
m_LightShader->Shutdown();
delete m_LightShader;
m_LightShader = nullptr;
}

// Release the model object
if (m_Model)
{
m_Model->Shutdown();
delete m_Model;
m_Model = nullptr;
}



	// Release the bitmap object
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = nullptr;
	}

	// Release the texture shader object
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
	}
*/

	// Release the text object
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = nullptr;
	}

	// Release the camera object
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = nullptr;
	}

	return;
}

bool GraphicsClass::Frame(int mouseX, int mouseY)
{
	bool result;
	/*
	static float rotation = 0.0f;

	// Update the rotation variable each frame
	rotation += (float)D3DX_PI * 0.005f;


	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene
	result = Render();

	if (!result)
	{
		return false;
	}
	*/

	// Set the location of the mouse 
	result = m_Text->SetMousePosition(mouseX, mouseY, 
										m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Set the position of the camera
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;
	bool result;

	// Clear the buffers to begin the scene
	// (here we set the color of the background)
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);



/*
	// Rotate the world matrix by the rotation value so that the triangle will spin
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Put the model vertex and index buffers
	// on the graphics pipeline to prepare them for drawing
	m_Model->Render(m_D3D->GetDeviceContext());

	//Render the model using the light shader
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), 
									m_Model->GetIndexCount(),
									worldMatrix,
									viewMatrix,
									projectionMatrix,
									m_Model->GetTexture(),
									m_Light->GetDirection(),
									m_Light->GetAmbientColor(),
									m_Light->GetDiffuseColor(),
									m_Camera->GetPosition(),
									m_Light->GetSpecularColor(),
									m_Light->GetSpecularPower());
	if (!result)
	{
		return false;
	}

*/

	// Turn off the Z buffer to begin all 2D rendering
	m_D3D->TurnZBufferOff();

	// Turn on the alpha blending before rendering the text
	m_D3D->TurnOnAlphaBlending();

	// Render the text strings
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

/*
		// Put the bitmap vertex and index buffers on the graphics pipeline 
	// to prepare them for drawing
	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 200, 200);
	if (!result)
	{
		return false;
	}

	// Render the bitmap with the texture shader
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), 
									 m_Bitmap->GetIndexCount(), 
									 worldMatrix, 
									 viewMatrix,
									 orthoMatrix,
									 m_Bitmap->GetTexture());
	if (!result)
	{
		return false;
	}
*/
	// Turn off alpha blending after rendering the text
	m_D3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed
	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen
	m_D3D->EndScene();

	return true;
}