/////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
/////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include "includes.h"
#include "log.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = nullptr;
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
bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, bool fullScreen)
{
	Log::Get()->Debug("GraphicsClass::Initialize()");

	// Create the Direct3D object
	m_D3D = new(std::nothrow) D3DClass();
	if (!m_D3D)
	{
		Log::Get()->Error("GraphicsClass::Initialize(): can't create the Direct3D object");
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
		Log::Get()->Error("GraphicsClass::Initialize(): can't initialize the Direct3D object");
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown(void)
{
	Log::Get()->Debug("GraphicsClass::Shutdown()");

	_SHUTDOWN(m_D3D);

	return;
}

bool GraphicsClass::Frame(void)
{
	if (!Render())
	{
		Log::Get()->Error("GraphicsClass::Frame(): there is something went wrong during the frame rendering");
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
	// Do some stuff before the render process
	m_D3D->BeginScene(0.2f, 0.4f, 0.6f, 1.0f);



	// Show the rendered scene onto the screen
	m_D3D->EndScene();

	return true;
}