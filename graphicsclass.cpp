///////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include "includes.h"
#include "log.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = nullptr;
	FULL_SCREEN = false;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, bool fullScreen)
{
	bool result;
	FULL_SCREEN = fullScreen;

	// Create the Direct3D object
	m_D3D = new(std::nothrow) D3DClass;
	if (!m_D3D)
	{
		Log::Get()->Error("GraphicsClass::Initialize(): can't allocate the memory for the Direct3D object");
		return false;
	}

	// Initialize the Direct3D object
	result = m_D3D->Initialize(screenWidth, 
							   screenHeight, 
							   VSYNC_ENABLED, 
							   hwnd, 
							   FULL_SCREEN,
							   SCREEN_DEPTH,
							   SCREEN_NEAR);
	if (!result)
	{
		Log::Get()->Error("GraphicsClass::Initialize(): can't initialize the Direct3D object");
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	_SHUTDOWN(m_D3D);

	return;
}

bool GraphicsClass::Frame()
{
	bool result;

	// Render the graphics scene
	result = Render();
	if (!result)
	{
		Log::Get()->Error("GraphicsClass::Frame(): something went wrong during the rendering of the graphics scene");
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	// Clear the buffers to begin the scene
	m_D3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// Present the rendered scene to the screen
	m_D3D->EndScene();

	return true;
}