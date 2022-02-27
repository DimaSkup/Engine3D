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
	_DELETE(m_input);

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

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object
	ApplicationHandle = this;

	// Get the instance of this application
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name
	m_applicationName = L"Engine";

	// Setup the windows class with default settings
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;

	// Register the window class
	if (!RegisterClassEx(&wc))
	{
		Log::Get()->Error("Window::Init(): can't register the class");
		return;
	}

	// Determine the resolution of the clients desctop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending upon whether it is running in full screen
	// or in windowed mode
	if (FULL_SCREEN)
	{
		// If full screen set maximum size of the users desctop and 32bit
		memset(&dmScreenSettings, 0, sizeof(DEVMODE));
		dmScreenSettings.dmSize = sizeof(DEVMODE);
		dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(screenWidth);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(screenHeight);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen 
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW,
							m_applicationName,
							m_applicationName,
							WS_OVERLAPPEDWINDOW | WS_VISIBLE,
							posX,
							posY,
							screenWidth,
							screenHeight,
							NULL,
							NULL,
							m_hinstance,
							NULL);

	if (!m_hwnd)
	{
		Log::Get()->Error("Window::Init(): can't create the window");
		return;
	}

	// Bring the window up on the screen and set it as main focus
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Show the mouse cursor
	ShowCursor(true);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		// Check if a key has been pressed on the keyboard
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state
			m_input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key
			m_input->KeyUp((unsigned int)wparam);
			return 0;
		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		// Check if the window is being created
		case WM_CREATE:
		{
			return 0;
		}
		// Check if the window is being destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		// Check if the window is being closed
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, message, wparam, lparam);
		}
	}

	return DefWindowProc(hwnd, message, wparam, lparam);
}