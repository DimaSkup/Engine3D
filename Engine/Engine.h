#pragma once

///////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
///////////////////////////////////////////////////////////////////////////////



#include "macros.h"
//#include "inputclass.h"
#include "Mouse/MouseClass.h"
#include "graphicsclass.h"
#include "log.h"
#include "Window/window.h"

// timers
#include "fpsclass.h"
#include "cpuclass.h"
#include "timerclass.h"

#include "positionclass.h" // for the camera movement control

//////////////////////////////////////////////////////////
// Class name: SystemClass
//////////////////////////////////////////////////////////
class Engine
{
private:
	struct EngineDesc
	{
		SystemClassDesc()
		{
			width = 800;
			height = 600;
			fullScreen = false;
		}

		int width;
		int height;
		bool fullScreen;
	};

public:
	Engine();
	Engine(const Engine& copy);
	~Engine();

	bool Initialize(void);
	void Shutdown();
	void Run();

	void SetDescription(int width, int height, bool isFullscreenMode);
private:
	
	//void InitializeWindows(int&, int&);	// initialize the WinAPI
	//void ShutdownWindows();
	bool frame();

private:
	//KeyboardClass keyboard;
	MouseClass mouse;

	bool m_init;
	EngineDesc sys_desc;

	Log log;
	Window* m_window;
	InputManager* m_inputManager;
	InputClass* m_input;
	GraphicsClass* m_graphics;

	// timers
	FpsClass* m_Fps;
	CpuClass* m_pCpu;
	TimerClass* m_pTimer;

	PositionClass* m_pPosition;  // for the camera movement control
};

/////////////////////////////
// GLOBALS
/////////////////////////////
static Engine* ApplicationHandle = nullptr;