////////////////////////////////////////////////////////////////////
// Filename:     RenderWindow.cpp
// Description:  
// Revising:     30.09.22
////////////////////////////////////////////////////////////////////
#include "RenderWindow.h"

// initializes the private members and registers the window class
bool RenderWindow::Initialize(HINSTANCE hInstance, std::string windowTitle,
	                          std::string windowClass, int width, int height)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);
	bool RegisterWindowClassResult = false;
	bool

	this->hInstance_ = hInstance;  // handle to application instance
	this->width_ = width; 
	this->height_ = height;
	this->windowTitle_ = windowTitle;
	this->windowTitleWide_ = StringConverter::StringToWide(this->windowTitle_); // wide string representation of window title
	this->windowClass_ = windowClass;
	this->windowClassWide_ = StringConverter::StringToWide(this->windowClass_); // wide string representation of window class name

	this->RegisterWindowClass();  // registers the window class
	if (!this->CreateWindowExtended()) // creates the window
	{
		return false;    // if we can't create the window we return false
	}

	return true;
} // Initialize()


// registers the window class
void RenderWindow::RegisterWindowClass()
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	WNDCLASSEX wc;  // our window class (this has to be filled before our window can be created)
	DEVMODE dmScreenSettings;

	// setup the window's class description
	wc.cbSize = sizeof(WNDCLASSEX); // need to fill in the size of our struct for cbSize
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS; // flags [redraw on width/height; own device context; get messages about double clicks]
	wc.lpfnWndProc = DefWindowProc; // pointer to Window Proc function for handling messages from this window
	wc.cbClsExtra = NULL; //# of extra bytes to allocate following the window-class structure.
	wc.cbWndExtra = NULL; //# of extra bytes to allocate following the window instance.
	wc.hInstance = this->hInstance_; // handle to the instance that contains the Window Procedure
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);  // handle to the class icon. Must be a handle to an icon resource.
	wc.hIconSm = wc.hIcon; // handle to small icon for this class. 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // use an arrow cursor for the app
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // handle to the class background brush for the window's background colour
	wc.lpszMenuName = nullptr; // pointer to a null terminated character string for the menu.
	wc.lpszClassName = this->windowClassWide_.c_str(); // pointer to a null terminated string of our class name for the window class

	// register the class so that it is usable
	if (!RegisterClassEx(&wc))
	{
		Log::Get()->Error(THIS_FUNC, "can't register the window class");
		return;
	}
} // RegisterWindowClass()

// creates the window
bool RenderWindow::CreateWindowExtended()
{
	RECT winRect{ 0, 0, this->width_, this->height_ };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, NULL);

	this->hwnd_ = CreateWindowEx(WS_EX_APPWINDOW, // extended windows style
		this->windowClassWide_.c_str(),   // window class name
		this->windowTitleWide_.c_str(),   // window title
		WS_VISIBLE | WS_OVERLAPPEDWINDOW, // windows style
		winRect.left,   // window x position
		winRect.top,    // window y position
		winRect.right,  // window width
		winRect.bottom, // window height
		NULL,  // handle to parent of this window. Since this is the first window, it has no parent window
		NULL,  // handle to menu or child window identifier. Can be set to NULL and use menu in WindowClassEx if the class menu is to be used.
		this->hInstance_, // handle to the instance of module to be used with this window
		nullptr); // param to create window

	if (!this->hwnd_)
	{
		// ErrorLogger::Log(GetLastError(), "CreateWindowEx Failed for window: " + this->windowTitle_);
		Log::Get()->Error(THIS_FUNC, "can't create the window");
		return false;
	}

	return true;
} // CreateWindowObject()