///////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"
#include "log.h"

SystemClass::SystemClass(void)
{
	m_input = nullptr;
	m_graphics = nullptr;
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass(void)
{
}

bool SystemClass::Initialize(void)
{
	int screenWidth = 0, screenHeight = 0;
	bool result;

	// initialize the WinAPI
	InitializeWindows(screenWidth, screenHeight);

	m_input = new InputClass;	// Create the input object. This object will be used to handle reading the keyboard input from the user
	m_graphics = new GraphicsClass;	// Create the graphics object. This object will handle rendering all the graphics for this app

	if (!m_input || !m_graphics)
	{
		Log::Get()->Error("SystemClass::Initialize(): can't allocate the memory for InputClass or GraphicsClass");
		return false;
	}

	// Initialization
	m_input->Initialize();
	result = m_graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		Log::Get()->Error("SystemClass::Initialize(): can't initialize the graphics class");
		return false;
	}
	
	return true;
}

// The Shutdown function does the clean up
void SystemClass::Shutdown()
{
	_SHUTDOWN(m_graphics);
	_SHUTDOWN(m_input);

	ShutdownWindows();	// Shutdown the window

	return;
}


// The Run function is where our application will loop and do all the application
// processing until we decide to quit
void SystemClass::Run(void)
{
	MSG msg;
	bool done, result;

	// Initialize the message structure
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user
	done = false;
	while (!done)
	{
		// Handle the windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing
			result = Frame();
			if (!result)
			{
				Log::Get()->Error("SystemClass::Run(): there is something went wrong during the frame processing");
				done = true;
			}
		}
	}

	return;
}


// The Frame function is where all the processing for our application is done
bool SystemClass::Frame(void)
{
	bool result;

	// Check if the user pressed escape and wants to exit the application
	if (m_input->IsKeyDown(VK_ESCAPE))
	{
		Log::Get()->Debug("SystemClass::Frame(): the ESC is pressed");
		return false;
	}

	// Do the frame processing for the graphics object
	result = m_graphics->Frame();
	if (!result)
	{
		Log::Get()->Error("SystemClass::Frame(): there is something went wrong during the frame processing");
		return false;
	}

	return false;
}