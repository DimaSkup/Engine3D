#include "window.h"

Window* Window::m_winInstance = nullptr;

Window::Window(void)
{
	if (!m_winInstance)
	{
		m_winInstance = this;
		m_hwnd = NULL;

		m_maximized = false;
		m_minimized = false;
		m_isexit = false;
		m_active = true;
		m_isresize = false;
		m_pInputManager = nullptr;
	}
	else
	{
		Log::Get()->Error("Window::Window(): there is already a Window instance");
	}

	Log::Get()->Debug("Window::Window()");
}

bool Window::Initialize(const DescWindow& desc)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	m_desc = desc;

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get the instance of this application
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name
	m_applicationName = L"Engine";

	// Setup the windows class with default settings
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wc.lpfnWndProc = StaticWndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = m_hinstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName;

	// Register the window class
	if (!RegisterClassEx(&wc))
	{
		Log::Get()->Error("Window::Init(): can't register the class");
		return false;
	}

	// Determine the resolution of the clients desctop screen
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	RECT winRect{ 0, 0, m_desc.width, m_desc.height };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, NULL);

	// Setup the screen settings depending upon whether it is running in full screen
	// or in windowed mode
	if (m_desc.fullScreen)
	{
		Log::Get()->Debug("Window::Initialize(): there is a full screen mode");

		// Initialize settings of the display device
		memset(&dmScreenSettings, 0, sizeof(DEVMODE));

		dmScreenSettings.dmSize = sizeof(DEVMODE);	// size of the DEVMODE structure (only public structure members)
		dmScreenSettings.dmPelsWidth = static_cast<unsigned long int>(screenWidth);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned long int>(screenHeight);
		dmScreenSettings.dmBitsPerPel = 32;			// specifies the color resolution, in bits per pixel, of the display device
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;	// specifies which fields will be initialized

		// Change the display settings
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set top left position of the window
		posX = posY = 0;
	}
	else	// if windowed mode
	{
		Log::Get()->Debug("Window::Initialize(): there is a windowed mode");

		screenWidth = m_desc.width;
		screenHeight = m_desc.height;

		// Place the window in the middle of the screen 
		posX = (GetSystemMetrics(SM_CXSCREEN) - m_desc.width) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - m_desc.height) / 2;
	}

	// Create the window with the screen settings and get the handle to it
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		m_applicationName,
		m_applicationName,
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,
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
		return false;;
	}

	// Bring the window up on the screen and set it as main focus
	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);
	//SetForegroundWindow(m_hwnd);
	//SetFocus(m_hwnd);
	
	ShowCursor(false);	// hide the mouse cursor

	Log::Get()->Debug(THIS_FUNC, "the window is initialized successfully");

	return true;
}

void Window::RunEvent(void)
{
	MSG msg;

	// Initialize the message structure
	ZeroMemory(&msg, sizeof(MSG));

	// Handle the windows messages
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Window::Shutdown(void)
{
	Log::Get()->Debug("Window::Shutdown()");

	// Show the mouse cursor
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode
	if (m_desc.fullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// Check if the window is being created
		case WM_CREATE:
		
			Log::Get()->Debug(THIS_FUNC, "the window is created");
			return 0;
		
		// Check if the window is being destroyed
		
		
		// Check if the window is being closed
		case WM_CLOSE:
		
			Log::Get()->Debug("WndProc(): window is closed");
		
			m_isexit = true;
			return 0;
		
		case WM_ACTIVATE:
		
			if (LOWORD(wParam) != WA_INACTIVE)
				m_active = true;
			else
				m_active = false;
			return 0;
		
		case WM_MOVE:
			//Log::Get()->Debug(THIS_FUNC, "the window IS MOVED");
			m_desc.posx = LOWORD(lParam);
			m_desc.posy = HIWORD(lParam);
			m_UpdateWindowState();
			return 0;
		
		case WM_SIZE:
		
			if (!m_desc.resizing)
				return 0;
		
			m_desc.width = LOWORD(lParam);
			m_desc.height = HIWORD(lParam);
			m_isresize = true;

			if (wParam == SIZE_MINIMIZED)
			{
				m_active = false;
				m_minimized = true;
				m_maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_active = true;
				m_minimized = false;
				m_maximized = true;
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (m_minimized)
				{
					m_active = true;
					m_minimized = false;
				}
				else if (m_maximized)
				{
					m_active = true;
					m_maximized = false;
				}
			}
			m_UpdateWindowState();
			return 0;
		
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP: case WM_LBUTTONDOWN:
		case WM_MBUTTONUP: case WM_MBUTTONDOWN:
		case WM_RBUTTONUP: case WM_RBUTTONDOWN:
		case WM_MOUSEWHEEL:
		case WM_KEYUP: case WM_KEYDOWN:
		
			if (m_pInputManager)
				m_pInputManager->Run(message, wParam, lParam);
			return 0;
		
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}


void Window::SetInputManager(InputManager* inputManager)
{
	m_pInputManager = inputManager;
	m_UpdateWindowState();
}


void Window::m_UpdateWindowState()
{
	RECT clientRect;
	
	clientRect.left = m_desc.posx;
	clientRect.top = m_desc.posy;
	clientRect.right = m_desc.width;
	clientRect.bottom = m_desc.height;
	if (m_pInputManager)
		m_pInputManager->SetWinRect(clientRect);
}

LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Window::Get()->WndProc(hwnd, message, wParam, lParam);
}