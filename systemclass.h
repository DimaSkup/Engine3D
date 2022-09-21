#pragma once

///////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
///////////////////////////////////////////////////////////////////////////////



#include "macros.h"
#include "inputclass.h"
#include "graphicsclass.h"
#include "log.h"
#include "window.h"

// timers
#include "fpsclass.h"
#include "cpuclass.h"
#include "timerclass.h"

#include "positionclass.h" // for the camera movement control

//////////////////////////////////////////////////////////
// Struct name: SystemClassDesc
//////////////////////////////////////////////////////////
struct SystemClassDesc
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

//////////////////////////////////////////////////////////
// Class name: SystemClass
//////////////////////////////////////////////////////////
class SystemClass
{
public:
	SystemClass(const SystemClassDesc& desc);
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize(void);
	void Shutdown();
	void Run();
private:
	
	//void InitializeWindows(int&, int&);	// initialize the WinAPI
	//void ShutdownWindows();
	bool frame();

private:

	bool m_init;
	SystemClassDesc m_sysDesc;

	Log log;
	Window* m_window;
	InputManager* m_inputManager;
	InputClass* m_input;
	GraphicsClass* m_graphics;

	// timers
	FpsClass* m_pFps;
	CpuClass* m_pCpu;
	TimerClass* m_pTimer;

	PositionClass* m_pPosition;  // for the camera movement control
};

/////////////////////////////
// GLOBALS
/////////////////////////////
static SystemClass* ApplicationHandle = nullptr;