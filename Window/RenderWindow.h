////////////////////////////////////////////////////////////////////
// Filename:     RenderWindow.h
// Description:  
// Revising:     30.09.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Engine/Log.h"


//////////////////////////////////
// Class name: RenderWindow
//////////////////////////////////
class RenderWindow
{
public:
	bool Initialize(HINSTANCE hInstance,         // initializes the private members and registers the window class
		            std::string window_title, 
		            std::string window_class,
		            int width, int height);
	bool ProcessMessages();
	~RenderWindow();

private:
	void RegisterWindowClass();     // registers the window class
	bool CreateWindowExtended();    // creates the window object

private:
	HWND hwnd_ = NULL;              // handle to this window
	HINSTANCE hInstance_ = NULL;    // handle to application instance
	std::string windowTitle_ = "";
	std::wstring windowTitleWide_ = L""; // wide string representation of window title
	std::string windowClass_ = "";
	std::wstring windowClassWide_ = L""; // wide string representation of window class name
	int width_ = 0;
	int height_ = 0;
};
