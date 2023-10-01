////////////////////////////////////////////////////////////////////
// Filename:     RenderWindow.cpp
// Description:  
// Revising:     30.09.22
////////////////////////////////////////////////////////////////////
#include "WindowContainer.h"



RenderWindow::~RenderWindow()
{
	// this function unregisters the window class, destroys the window,
	// reset the responsible members;

	if (this->hwnd_ != NULL)
	{
		UnregisterClass(this->windowClassWide_.c_str(), this->hInstance_); // Remove the application instance
		ChangeDisplaySettings(NULL, 0); // before destroying the window we need to set it to the windowed mode
		DestroyWindow(this->hwnd_);  // Remove the window
		this->hwnd_ = NULL;
		this->hInstance_ = NULL;

		Log::Debug(THIS_FUNC_EMPTY);
	}
} // end ~RenderWindow

///////////////////////////////////////////////////////////

bool RenderWindow::Initialize(HINSTANCE hInstance, 
	                          std::string windowTitle,
	                          std::string windowClass, 
	                          int width, int height)
{
	// this function setups the window params,
	// registers the window class and show us a new window;

	Log::Debug(THIS_FUNC_EMPTY);
	bool RegisterWindowClassResult = false;

	this->hInstance_ = hInstance;  // handle to application instance
	this->windowDimensions_.x = width;
	this->windowDimensions_.y = height;
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
	ShowCursor(FALSE);
	SetForegroundWindow(this->hwnd_);
	SetFocus(this->hwnd_);


	Log::Print(THIS_FUNC, "the window is created successfully");

	return true;
} // end Initialize()

///////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// this function handles the window messages
	return WindowContainer::Get()->WindowProc(hwnd, uMsg, wParam, lParam); // call our handler of window messages
}

///////////////////////////////////////////////////////////

void RenderWindow::RegisterWindowClass()
{
	// this function registers the window class

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
} // end RegisterWindowClass()

///////////////////////////////////////////////////////////

bool RenderWindow::CreateWindowExtended()
{
	// this function creates the window

	Log::Debug(THIS_FUNC_EMPTY);

	// calculate the centre of the screen
	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - this->windowDimensions_.x / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - this->windowDimensions_.y / 2;

	RECT winRect; // window rectangle
	winRect.left = centerScreenX;
	winRect.top = centerScreenY;
	winRect.right = winRect.left + this->windowDimensions_.x;
	winRect.bottom = winRect.top + this->windowDimensions_.y;
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

	this->hwnd_ = CreateWindowEx(WS_EX_APPWINDOW, // extended windows style
		this->windowClassWide_.c_str(),   // window class name
		this->windowTitleWide_.c_str(),   // window title
		WS_VISIBLE | WS_OVERLAPPEDWINDOW, // windows style
		winRect.left,              // window x position
		winRect.top,              // window y position
		winRect.right - winRect.left,  // window width
		winRect.bottom - winRect.top, // window height
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

} // end CreateWindowExtended()

///////////////////////////////////////////////////////////

bool RenderWindow::ProcessMessages(void)
{
	// this function dispatches the window messages to the WindowProc function;
	// or destroys the window if we closed it

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
} // end ProcessMessages()

///////////////////////////////////////////////////////////

HWND RenderWindow::GetHWND() const
{
	return this->hwnd_;
}

///////////////////////////////////////////////////////////

void RenderWindow::UpdateWindowDimensions(const unsigned int newWidth, const unsigned int newHeight)
{
	windowDimensions_.x = newWidth;
	windowDimensions_.y = newHeight;
	return;
}