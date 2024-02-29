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
//#include "../Timers/GameTimer.h"

//class WindowContainer;

//////////////////////////////////
// Class name: RenderWindow
//////////////////////////////////
class RenderWindow
{
public:
	// initializes the private members and registers the window class
	bool Initialize(HINSTANCE hInstance, 
					HWND & mainWnd,
		            const std::wstring & windowTitle,
		            const std::wstring & windowClass,
		            const int width, 
		            const int height);
	bool ProcessMessages(HINSTANCE & hInstance, HWND & hwnd);
	float AspectRatio() const;

	inline POINT GetWindowDimensions() const { return{ (LONG)windowWidth_, (LONG)windowHeight_ }; };

	void UnregisterWindowClass(HINSTANCE & hInstance);
	//inline int GetWindowWidth() const { return windowCurrentWidth_; }
	//inline int GetWindowHeight() const { return windowCurrentHeight_; }

	void UpdateWindowDimensions(const UINT newWidth, const UINT newHeight);
	void UpdateClientDimensions(const UINT newWidth, const UINT newHeight);
	
	~RenderWindow();

private:
	void RegisterWindowClass(const HINSTANCE hInstance);            // registers the window class
	bool CreateWindowExtended(const HINSTANCE hInstance, HWND & hwnd);           // creates the window object

private:
	std::string windowTitle_{ "" };
	std::string windowClass_{ "" };
	std::wstring windowTitleWide_ { L"" }; // wide string representation of window title
	std::wstring windowClassWide_ { L"" }; // wide string representation of window class name

	// window's zone width/height
	UINT windowWidth_ = 0;
	UINT windowHeight_ = 0;

	// client's zone width/height
	UINT clientWidth_ = 0;
	UINT clientHeight_ = 0;
};
