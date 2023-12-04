////////////////////////////////////////////////////////////////////
// Filename:     RenderWindow.h
// Description:  this class is responsible for creation, initialization,
//               and showing of the window; 
//               also it dispatches some messages about window events to 
//               special handler which is in the WindowContainer class;
// Revising:     30.09.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Engine/Log.h"
#include "../Engine/StringHelper.h"

//class WindowContainer;

//////////////////////////////////
// Class name: RenderWindow
//////////////////////////////////
class RenderWindow
{
public:
	// initializes the private members and registers the window class
	bool Initialize(HINSTANCE hInstance,         
		            std::string windowTitle, 
		            std::string windowClass,
		            int width, int height);
	bool ProcessMessages();
	HWND GetHWND() const;

	inline const POINT & GetWindowDimensions() const { return windowDimensions_; };
	//inline int GetWindowWidth() const { return windowCurrentWidth_; }
	//inline int GetWindowHeight() const { return windowCurrentHeight_; }

	void UpdateWindowDimensions(const unsigned int newWidth, const unsigned int newHeight);
	
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
	POINT windowDimensions_{ 0, 0 }; // window's width/height
};
