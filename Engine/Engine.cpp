///////////////////////////////////////////////////////////////////////////////
// Filename: engine.cpp
// Revising: 05.10.22
///////////////////////////////////////////////////////////////////////////////
#include "Engine.h"

// initialize all the main parts of the engine
bool Engine::Initialize(HINSTANCE hInstance,
	                    std::string windowTitle,
	                    std::string windowClass,
	                    const int width, 
						const int height,
                        const bool fullScreen)
{
	try
	{
		bool result = false;

		timer_.Start();   // start the engine timer


		// ------------------------------     WINDOW      ------------------------------- //

		// initialize the window
		result = this->renderWindow_.Initialize(hInstance, windowTitle, windowClass, width, height);
		COM_ERROR_IF_FALSE(result, "can't initialize the window");


		// ------------------------------ GRAPHICS SYSTEM ------------------------------- //

		// initialize the graphics system
		result = this->graphics_.Initialize(this->renderWindow_.GetHWND());
		COM_ERROR_IF_FALSE(result, "can't initialize the graphics system");


		// ------------------------ TIMERS (FPS, CPU, TIMER) ---------------------------- //

		fps_.Initialize();     // initialize the fps system
		cpu_.Initialize();     // initialize the cpu clock


		// ------------------------------  SOUND SYSTEM --------------------------------- //

		// initialize the sound obj
		//result = this->sound_.Initialize(this->renderWindow_.GetHWND());
		//COM_ERROR_IF_FALSE(result, "can't initialize the sound obj");


		Log::Print(THIS_FUNC, "is initialized!");
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
}

// hangle messages from the window
bool Engine::ProcessMessages()
{
	if (IsExit())       // if we want to close the engine
		return false;

	return this->renderWindow_.ProcessMessages();
}


// each frame we update the state of the engine
void Engine::Update()
{
	
	// to update the system stats each of timers classes we needs to call its 
	// own Frame function for each frame of execution the application goes through
	fps_.Frame();
	cpu_.Frame();
	deltaTime_ = timer_.GetMilisecondsElapsed();
	timer_.Restart();

	systemState_.fps = fps_.GetFps();
	systemState_.cpu = cpu_.GetCpuPercentage();

	
	while  (!keyboard_.KeyBufferIsEmpty())
	{
		kbe_ = keyboard_.ReadKey();

		if (kbe_.GetKeyCode() == VK_ESCAPE)
			isExit_ = true;
	/*
		
		unsigned char keycode = kbe_.GetKeyCode();
		std::string outmsg{ "" };

		if (kbe_.IsPress())
		{
			outmsg += "Key press: ";
		}
		if (kbe_.IsRelease())
		{
			outmsg += "Key release: ";
		}

		outmsg += keycode;
		Log::Debug(THIS_FUNC, outmsg.c_str());
	*/
	}
	
	
	// actions on mouse events
	while (!mouse_.EventBufferIsEmpty())
	{
		
		me_ = mouse_.ReadEvent();

		if (me_.GetType() == MouseEvent::EventType::RAW_MOVE)
		{
			this->graphics_.GetEditorCamera().AdjustRotation((float)me_.GetPosY() * 0.01f, (float)me_.GetPosX() * 0.01f, 0);
		}
	}
}


// executes rendering of each frame
void Engine::RenderFrame()
{
	graphics_.RenderFrame(&systemState_, kbe_, me_, mouse_, deltaTime_);
}