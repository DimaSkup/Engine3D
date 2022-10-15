////////////////////////////////////////////////////////////////////
// Filename:     RenderWindow.cpp
// Description:  
// Revising:     30.09.22
////////////////////////////////////////////////////////////////////
#include "WindowContainer.h"



// initializes the private members, registers the window class and show us a new window;
bool RenderWindow::Initialize(HINSTANCE hInstance, 
	                          std::string windowTitle,
	                          std::string windowClass, 
	                          int width, int height)
{
	Log::Debug(THIS_FUNC_EMPTY);
	bool RegisterWindowClassResult = false;

	this->hInstance_ = hInstance;  // handle to application instance
	this->width_ = width; 
	this->height_ = height;
	this->windowTitle_ = windowTitle;
	this->windowTitleWide_ = StringConverter::StringToWide(this->windowTitle_); // wide string representation of window title
	this->windowClass_ = windowClass;
	this->windowClassWide_ =  StringConverter::StringToWide(this->windowClass_); // wide string representation of window class name

	this->RegisterWindowClass();  // registers the window class

	if (!this->CreateWindowExtended()) // creates the window
	{
		return false;    // if we can't create the window we return false
	}

	// bring the window up on the screen and set it as main focus
	ShowWindow(this->hwnd_, SW_SHOW);
	SetForegroundWindow(this->hwnd_);
	SetFocus(this->hwnd_);

	Log::Print(THIS_FUNC, "the window is created successfully");

	return true;
} // Initialize()


// unregisters the window class, destroys the window, reset the responsible members;
RenderWindow::~RenderWindow()
{
	Log::Debug(THIS_FUNC_EMPTY);

	if (this->hwnd_ != NULL)
	{
		UnregisterClass(this->windowClassWide_.c_str(), this->hInstance_); // Remove the application instance
		ChangeDisplaySettings(NULL, 0); // before destroying the window we need to set it to the windowed mode
		DestroyWindow(this->hwnd_);  // Remove the window
		this->hwnd_ = NULL;
		this->hInstance_ = NULL;
	}
}


// handles the window messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	// all other messages
	case WM_CLOSE:
	{
		DestroyWindow(hwnd);
		return 0;
	}
	default:
		return WindowContainer::Get()->WindowProc(hwnd, uMsg, wParam, lParam); // call our handler of window messages
	}
}

// registers the window class
void RenderWindow::RegisterWindowClass()
{
	Log::Debug(THIS_FUNC_EMPTY);

	WNDCLASSEX wc;  // our window class (this has to be filled before our window can be created)

	// setup the window's class description
	wc.cbSize = sizeof(WNDCLASSEX); // need to fill in the size of our struct for cbSize
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS; // flags [redraw on width/height; own device context; get messages about double clicks]
	wc.lpfnWndProc = WindowProc; // pointer to Window Proc function for handling messages from this window
	wc.cbClsExtra = NULL; //# of extra bytes to allocate following the window-class structure.
	wc.cbWndExtra = NULL; //# of extra bytes to allocate following the window instance.
	wc.hInstance = this->hInstance_; // handle to the instance that contains the Window Procedure
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);  // handle to the class icon. Must be a handle to an icon resource.
	wc.hIconSm = wc.hIcon; // handle to small icon for this class. 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // use an arrow cursor for the app
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // handle to the class background brush for the window's background colour
	wc.lpszMenuName = nullptr; // pointer to a null terminated character string for the menu.
	wc.lpszClassName = this->windowClassWide_.c_str(); // pointer to a null terminated string of our class name for the window class

	// register the class so that it is usable
	if (!RegisterClassEx(&wc))
	{
		Log::Error(THIS_FUNC, "can't register the window class");
		return;
	}
} // RegisterWindowClass()


// creates the window
bool RenderWindow::CreateWindowExtended()
{
	Log::Debug(THIS_FUNC_EMPTY);

	RECT winRect{ 0, 0, this->width_, this->height_ };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, NULL);

	this->hwnd_ = CreateWindowEx(WS_EX_APPWINDOW, // extended windows style
		this->windowClassWide_.c_str(),   // window class name
		this->windowTitleWide_.c_str(),   // window title
		WS_VISIBLE | WS_OVERLAPPEDWINDOW, // windows style
		0,              // window x position
		0,              // window y position
		winRect.right,  // window width
		winRect.bottom, // window height
		NULL,  // handle to parent of this window. Since this is the first window, it has no parent window
		NULL,  // handle to menu or child window identifier. Can be set to NULL and use menu in WindowClassEx if the class menu is to be used.
		this->hInstance_, // handle to the instance of module to be used with this window
		nullptr); // param to create window

	if (!this->hwnd_)
	{
		// ErrorLogger::Log(GetLastError(), "CreateWindowEx Failed for window: " + this->windowTitle_);
		Log::Error(THIS_FUNC, "can't create the window");
		return false;
	}

	return true;
} // CreateWindowObject()


// dispatches the window messages to the WindowProc function
bool RenderWindow::ProcessMessages(void)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));  // Initialize the message structure

	// Handle the windows messages
	while (PeekMessage(&msg, // where to store messages (if one exists)
		NULL,                // handle to window we are checking messages for
		0,                   // minimum filter msg value - we are not filtering for specific messages
		0,                   // maximum filter msg value
		PM_REMOVE))          // remove message after capturing it via PeekMessage
	{
		TranslateMessage(&msg); // translate message from virtual key messages into character messages so we can display such messages
		DispatchMessage(&msg);  // dispatch message to our Window Proc for this window
	}

	// check if the window was closed
	if (msg.message == WM_NULL) // WM_NULL but not WM_QUIT -- for the case if we want to have more than only one window
	{
		if (!IsWindow(this->hwnd_))
		{
			this->hwnd_ = NULL; // message processing look takes care of destroying this window
			UnregisterClass(this->windowClassWide_.c_str(), this->hInstance_);
			return false;
		}
	}

	return true;
} // ProcessMessages()

HWND RenderWindow::GetHWND()
{
	return hwnd_;
}