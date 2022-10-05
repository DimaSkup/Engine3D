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
#include "macros.h"
#include "Log.h"

//#include "inputclass.h"
#include "../Mouse/MouseClass.h"
#include "../Render/graphicsclass.h"

#include "../Window/window.h"
#include "../Window/WindowContainer.h"

// timers
#include "../Timers/fpsclass.h"
#include "../Timers/cpuclass.h"
#include "../Timers/timerclass.h"

#include "../Camera/positionclass.h" // for the camera movement control


//////////////////////////////////////////////////////////
// Class name: SystemClass
//////////////////////////////////////////////////////////
class Engine : private WindowContainer
{
public:
	//Engine();
	//Engine(const Engine& copy);
	//~Engine();

	// initializes the private members for the Engine class
	bool Initialize(HINSTANCE hInstance, std::string windowTitle, std::string windowClass, int width, int height);
	bool ProcessMessages();
	void Update(); // processes all the messages which we get from input devices
	//void Shutdown();
	//void Run();
private:
	//bool frame();

private:
	WindowContainer windowContainer;   // this is necessary to make possible calling of the static window messages handler
	


	/*
	bool m_init;

	

	InputClass* m_input;
	GraphicsClass* m_graphics;

	// timers
	FpsClass* m_Fps;
	CpuClass* m_pCpu;
	TimerClass* m_pTimer;

	PositionClass* m_pPosition;  // for the camera movement control
	*/
};

/////////////////////////////
// GLOBALS
/////////////////////////////
static Engine* ApplicationHandle = nullptr;