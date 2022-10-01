#pragma once

///////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
///////////////////////////////////////////////////////////////////////////////



#include "macros.h"
//#include "inputclass.h"
#include "../Mouse/MouseClass.h"
#include "../Render/graphicsclass.h"
#include "../Engine/Log.h"
#include "../Window/window.h"

// timers
#include "../Timers/fpsclass.h"
#include "../Timers/cpuclass.h"
#include "../Timers/timerclass.h"

#include "../Camera/positionclass.h" // for the camera movement control

//////////////////////////////////////////////////////////
// Class name: SystemClass
//////////////////////////////////////////////////////////
class Engine
{
private:
	struct EngineDesc
	{
		EngineDesc()
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
	//Window* m_window;
	//InputManager* m_inputManager;
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