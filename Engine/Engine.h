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
#include "../Sound/SoundClass.h"

// window module
#include "../Window/window.h"
#include "../Window/WindowContainer.h"

// timers
#include "../Timers/timer.h"
#include "../Timers/fpsclass.h"
#include "../Timers/cpuclass.h"
#include "../Timers/timerclass.h"

// camera
#include "../Camera/EditorCamera.h"



//////////////////////////////////////////////////////////
// Class name: SystemClass
//////////////////////////////////////////////////////////
class Engine : public WindowContainer
{
public:
	Engine();
	~Engine();

	// initializes the private members for the Engine class
	bool Initialize(HINSTANCE hInstance,
					Settings* pEngineSettings,
					std::string windowClass);
	bool ProcessMessages();
	void Update();                               // processes all the messages which we get from input devices
	void RenderFrame();

private:  // restrict a copying of this class instance
	Engine(const Engine & obj);
	Engine & operator=(const Engine & obj);

private:
	GraphicsClass* pGraphics_ = nullptr;         // rendering system
	FpsClass*      pFps_ = nullptr;
	CpuClass*      pCpu_ = nullptr;
	Timer*         pTimer_ = nullptr;
	SystemState*   pSystemState_ = nullptr;

	KeyboardEvent keyboardEvent_;    // the current keyboard event
	MouseEvent    mouseEvent_;       // the current mouse event
	SoundClass*    pSound_ = nullptr;            // for playing sounds

	float deltaTime_ = 0.0f;                     // the time passed since the last frame
};

/////////////////////////////
// GLOBALS
/////////////////////////////
static Engine* ApplicationHandle = nullptr;