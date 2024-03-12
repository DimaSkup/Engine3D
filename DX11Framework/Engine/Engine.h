///////////////////////////////////////////////////////////////////////////////
// Filename:      engine.h
// Description:   this class is responsible for initialization and running
//                all the main parts of the engine.
// Revising:      05.10.22
///////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "macros.h"  // to use some macroses for work with memory, and some debug macroses
#include "Log.h"
#include "SystemState.h"
#include "EventListener.h"

#include "../Render/graphicsclass.h"
#include "../Sound/SoundClass.h"

// input
#include "../Input/inputmanager.h"
#include "../Keyboard/KeyboardClass.h"
#include "../Mouse/MouseClass.h"
#include "../Input/inputcodes.h"

// window module
#include "../Window/window.h"
#include "../Window/WindowContainer.h"

// timers
//#include "../Timers/timer.h"
#include "../Timers/cpuclass.h"
#include "../Timers/GameTimer.h"

// camera
#include "../Camera/EditorCamera.h"



//////////////////////////////////////////////////////////
// Class name: SystemClass
//////////////////////////////////////////////////////////
class Engine : public EventListener
{
public:
	Engine();
	~Engine();

	// initializes the private members for the Engine class
	bool Initialize(HINSTANCE hInstance,
					HWND hwnd,
		            Settings & engineSettings,
					const std::wstring & windowTitle);

	//bool ProcessMessages();                // processes all the messages which we get from input devices
	void Update();                         // update the state of the engine/game for the current frame                   
	void CalculateFrameStats();            // measure the number of frames being rendered per second (FPS)
	void RenderFrame();                    // do all the rendering onto the screen

	inline bool IsPaused() const { return isPaused_; }
	inline bool IsExit() const { return isExit_; }

	// access functions return a copy of the main window handle or app instance handle;
	HWND GetHWND() const;
	HINSTANCE GetInstance() const;
	
	virtual void EventActivate(const APP_STATE state) override;
	virtual void EventWindowMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void EventWindowResize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void EventWindowSizing(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void EventKeyboard(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void EventMouse(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;






private:  // restrict a copying of this class instance
	Engine(const Engine & obj);
	Engine & operator=(const Engine & obj);

private:
	HWND hwnd_ = NULL;                     // main window handle
	HINSTANCE hInstance_ = NULL;           // application instance handle

	bool isPaused_ = false;                // defines if the engine/game is currently paused
	bool isExit_ = false;                  // are we going to exit?
	bool isMinimized_ = false;             // is the window minimized?
	bool isMaximized_ = true;              // is the window maximized?
	bool isResizing_ = false;              // are we resizing the window?
	float deltaTime_ = 0.0f;

	std::wstring windowTitle_{ L"" };      // window title/caption

	
	InputManager  inputManager_;
	KeyboardClass keyboard_;               // represents a keyboard device
	MouseClass    mouse_;                  // represents a mouse device
	GraphicsClass    graphics_;            // rendering system
	
	SystemState      systemState_;         // contains different info about the state of the engine
	CpuClass         cpu_;                 // cpu usage counter
	GameTimer        timer_;               // used to keep track of the "delta-time" and game time

	KeyboardEvent    keyboardEvent_;       // the current keyboard event
	MouseEvent       mouseEvent_;          // the current mouse event
	SoundClass       sound_;               // for playing sounds

										   // window's zone width/height
	UINT windowWidth_ = 0;
	UINT windowHeight_ = 0;

	// client's zone width/height
	UINT clientWidth_ = 0;
	UINT clientHeight_ = 0;
};