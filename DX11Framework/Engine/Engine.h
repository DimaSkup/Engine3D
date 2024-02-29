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

#include "../Render/graphicsclass.h"
#include "../Sound/SoundClass.h"

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
class Engine
{
public:
	Engine();
	~Engine();

	// initializes the private members for the Engine class
	bool Initialize(HINSTANCE hInstance,
		            Settings & engineSettings,
		            const std::wstring & windowClass);

	bool ProcessMessages();                // processes all the messages which we get from input devices
	void Update();                         // update the state of the engine/game for the current frame                   
	void CalculateFrameStats();            // measure the number of frames being rendered per second (FPS)
	void RenderFrame();                    // do all the rendering onto the screen

	void OnActivate()
	{
		Log::Error(LOG_MACRO, "WM_ACTIVATE listener is called");
		exit(-1);
	}

	inline bool IsPaused() const { return isPaused_; }

	// access functions return a copy of the main window handle or app instance handle;
	HWND GetHWND() const;
	HINSTANCE GetInstance() const;
	

private:  // restrict a copying of this class instance
	Engine(const Engine & obj);
	Engine & operator=(const Engine & obj);

private:
	void HandleMouseMovement(const float deltaTime);
	void HandleKeyboardEvents();

private:
	HWND hwnd_ = NULL;                     // main window handle
	HINSTANCE hInstance_ = NULL;           // application instance handle

	bool isPaused_ = false;                // defines if the engine/game is currently paused
	float deltaTime_ = 0.0f;

	std::wstring windowTitle_{ L"" };      // window title/caption

	WindowContainer  windowContainer_;
	GraphicsClass    graphics_;            // rendering system
	
	SystemState      systemState_;         // contains different info about the state of the engine
	CpuClass         cpu_;                 // cpu usage counter
	GameTimer        timer_;               // used to keep track of the "delta-time" and game time

	KeyboardEvent    keyboardEvent_;       // the current keyboard event
	MouseEvent       mouseEvent_;          // the current mouse event
	SoundClass       sound_;               // for playing sounds


};