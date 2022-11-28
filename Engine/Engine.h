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

//#include "inputclass.h"
#include "../Mouse/MouseClass.h"
#include "../Render/graphicsclass.h"

#include "../Window/window.h"
#include "../Window/WindowContainer.h"

// timers
#include "../Timers/fpsclass.h"
#include "../Timers/cpuclass.h"
#include "../Timers/timerclass.h"

#include "../Camera/EditorCamera.h"

//////////////////////////////////////////////////////////
// Class name: SystemClass
//////////////////////////////////////////////////////////
class Engine : public WindowContainer
{
public:
	//Engine();
	//Engine(const Engine& copy);
	//~Engine();

	// initializes the private members for the Engine class
	bool Initialize(HINSTANCE hInstance,
					std::string windowTitle,
					std::string windowClass,
					const int width, 
					const int height,
					const bool fullScreen);
	bool ProcessMessages();
	void Update(); // processes all the messages which we get from input devices
	void RenderFrame();
	//void Shutdown();
	//void Run();

private:
	GraphicsClass graphics_;  // rendering system
	FpsClass      fps_;
	CpuClass      cpu_;
	TimerClass    timer_;
	SystemState   systemState_;

	KeyboardEvent kbe_;  // the current keyboard event
	MouseEvent    me_;   // the current mouse event
};

/////////////////////////////
// GLOBALS
/////////////////////////////
static Engine* ApplicationHandle = nullptr;