////////////////////////////////////////////////////////////////////
// Filename:     RenderWindow.cpp
// Description:  
// Revising:     30.09.22
////////////////////////////////////////////////////////////////////
#include "WindowContainer.h"



RenderWindow::~RenderWindow()
{
}

///////////////////////////////////////////////////////////

bool RenderWindow::Initialize(HINSTANCE hInstance,
							  HWND & mainWnd,
	                          const std::wstring & windowTitle,
	                          const std::wstring & windowClass, 
	                          const int width, 
	                          const int height)
{
	// this function setups the window params,
	// registers the window class and show us a new window;

	Log::Debug();
	bool RegisterWindowClassResult = false;

	this->windowWidth_ = width;
	this->windowHeight_ = height;
	this->windowTitle_ = StringHelper::ToString(windowTitle);
	this->windowTitleWide_ = windowTitle; // wide string representation of window title
	this->windowClass_ = StringHelper::ToString(windowClass);
	this->windowClassWide_ = windowClass; // wide string representation of window class name

	this->RegisterWindowClass(hInstance);  // registers the window class

	if (!this->CreateWindowExtended(hInstance, mainWnd)) // creates the window and setups a handle to the main window
	{
		return false;    // if we can't create the window we return false
	}

	// bring the window up on the screen and set it as main focus
	ShowWindow(mainWnd, SW_SHOW);
	ShowCursor(TRUE);
	SetForegroundWindow(mainWnd);
	SetFocus(mainWnd);


	Log::Print("the window is created successfully");

	return true;
}

///////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// this function handles the window messages
	return WindowContainer::Get()->WindowProc(hwnd, uMsg, wParam, lParam); // call our handler of window messages
}

///////////////////////////////////////////////////////////

void RenderWindow::RegisterWindowClass(const HINSTANCE hInstance)
{
	// this function registers the window class

	Log::Debug();

	// our window class (this has to be filled before our window can be created)
	WNDCLASSEX wc;  

	// setup the window's class description
	wc.cbSize = sizeof(WNDCLASSEX);                             // need to fill in the size of our struct for cbSize
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS; // flags [redraw on width/height; own device context; get messages about double clicks]
	wc.lpfnWndProc = WindowProc;                                // pointer to Window Proc function for handling messages from this window
	wc.cbClsExtra = NULL;                                       //# of extra bytes to allocate following the window-class structure.
	wc.cbWndExtra = NULL;                                       //# of extra bytes to allocate following the window instance.
	wc.hInstance = hInstance;                                   // handle to the instance that contains the Window Procedure
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);                     // handle to the class icon. Must be a handle to an icon resource.
	wc.hIconSm = wc.hIcon;                                      // handle to small icon for this class. 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);                   // use an arrow cursor for the app
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);     // handle to the class background brush for the window's background colour
	wc.lpszMenuName = nullptr;                                  // pointer to a null terminated character string for the menu.
	wc.lpszClassName = this->windowClassWide_.c_str();          // pointer to a null terminated string of our class name for the window class

	// register the class so that it is usable
	if (!RegisterClassEx(&wc))
	{
		Log::Error("can't register the window class");
		return;
	}
}

///////////////////////////////////////////////////////////

bool RenderWindow::CreateWindowExtended(const HINSTANCE hInstance, HWND & hwnd)
{
	// this function creates the window

	Log::Debug();

	// get the window dimensions
	const UINT width = windowWidth_;
	const UINT height = windowHeight_;

	// calculate the centre of the screen
	const int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - (width / 2);
	const int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - (height / 2);

	RECT winRect; // window rectangle
	winRect.left = centerScreenX;
	winRect.top = centerScreenY;
	winRect.right = winRect.left + width;
	winRect.bottom = winRect.top + height;
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, // extended windows style
		this->windowClassWide_.c_str(),    // window class name
		this->windowTitleWide_.c_str(),    // window title
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,  // windows style
		winRect.left,                      // window left position
		winRect.top,                       // window top position
		winRect.right - winRect.left,      // window width
		winRect.bottom - winRect.top,      // window height
		NULL,                              // handle to parent of this window. Since this is the first window, it has no parent window
		NULL,                              // handle to menu or child window identifier. Can be set to NULL and use menu in WindowClassEx if the class menu is to be used.
		hInstance,                         // handle to the instance of module to be used with this window
		nullptr);                          // param to create window

	if (!hwnd)
	{
		// ErrorLogger::Log(GetLastError(), "CreateWindowEx Failed for window: " + this->windowTitle_);
		Log::Error("can't create the window");
		return false;
	}

	return true;

}

///////////////////////////////////////////////////////////

bool RenderWindow::ProcessMessages(HINSTANCE & hInstance, HWND & hwnd)
{
	// this function dispatches the window messages to the WindowProc function;
	// or destroys the window if we closed it

	MSG msg;
	//ZeroMemory(&msg, sizeof(MSG));  // Initialize the message structure

	// Handle the windows messages
	while (PeekMessage(&msg,    // where to store messages (if one exists)
		NULL,                   // handle to window we are checking messages for
		0,                      // minimum filter msg value - we are not filtering for specific messages
		0,                      // maximum filter msg value
		PM_REMOVE))             // remove message after capturing it via PeekMessage
	{
		TranslateMessage(&msg); // translate message from virtual key messages into character messages so we can display such messages
		DispatchMessage(&msg);  // dispatch message to our Window Proc for this window
	}

	// check if the window was closed
	if ((msg.message == WM_NULL) || (msg.message = WM_QUIT))
	{
		if (!IsWindow(hwnd))
		{
			hwnd = NULL; // message processing look takes care of destroying this window
			UnregisterClass(this->windowClassWide_.c_str(), hInstance);
			return false;
		}
	}

	// if we want to close the window and the engine as well
	//if (windowContainer_.IsExit())
	//	return false;

	return true;
} 

///////////////////////////////////////////////////////////

void RenderWindow::UpdateWindowDimensions(const UINT newWidth, const UINT newHeight)
{
	// update the dimensions of the window zone
	windowWidth_ = newWidth;
	windowHeight_ = newHeight;
	return;
}

void RenderWindow::UpdateClientDimensions(const UINT newWidth, const UINT newHeight)
{
	// update the dimensions of the client zone
	clientWidth_ = newWidth;
	clientHeight_ = newHeight;
}

///////////////////////////////////////////////////////////

void RenderWindow::UnregisterWindowClass(HINSTANCE & hInstance)
{
	// remove this window class from the instance of application
	UnregisterClass(windowClassWide_.c_str(), hInstance);
}

///////////////////////////////////////////////////////////

float RenderWindow::AspectRatio() const
{
	return static_cast<float>(clientWidth_) / clientHeight_;
}