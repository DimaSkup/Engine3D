//////////////////////////////////
// Filename: graphicsclass.cpp
//////////////////////////////////
#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;
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































	ifstream fin;
	char input;
	int i;
	int m_vertexCount;


	// Open the model file
	fin.open("../Engine/data/cube.txt");

	// If it could not open the file then exit
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count
	fin >> m_vertexCount;
	LPCWSTR* kek = { m_vertexCount };

	MessageBox(hwnd, m_vertexCount, L"Error", MB_OK);
	return false;

	/*
	// Set the number of indices to be the same as the vertex count
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// Read up to the beginning of the data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}*/

	// Close the model filen
	fin.close();













	MessageBox(hwnd, L"kek", L"Error", MB_OK);
	return false;










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
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void GraphicsClass::Shutdown()
{
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

bool GraphicsClass::Frame()
{
	bool result;

	static float rotation = 0.0f;

	// Update the rotation variable each frame
	rotation += (float)D3DX_PI * 0.01f;


	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene
	result = Render(rotation);

	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	// Clear the buffers to begin the scene
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);


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
									m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen
	m_D3D->EndScene();

	return true;
}