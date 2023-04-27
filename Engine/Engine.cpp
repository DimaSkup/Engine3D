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

	

	// handle keyboard events
	while  (!keyboard_.KeyBufferIsEmpty())
	{
		kbe_ = keyboard_.ReadKey();

		// if we pressed the ESC button we exit from the application
		if (kbe_.GetKeyCode() == VK_ESCAPE)
		{
			isExit_ = true;
			return;
		}

		if (kbe_.IsPress() && kbe_.GetKeyCode() == VK_F2)
		{
			graphics_.ChangeModelFillMode();
			Log::Debug(THIS_FUNC, "F2 key is pressed");
			return;
		}


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
	
	
	// handle mouse events
	while (!mouse_.EventBufferIsEmpty())
	{
		me_ = mouse_.ReadEvent();

		switch (me_.GetType())
		{
			case MouseEvent::EventType::RAW_MOVE:
			{
				// each time when we execute raw mouse move we update the camera's rotation
				this->graphics_.HandleMovementInput(me_, deltaTime_);
				break;
			}
			case MouseEvent::EventType::Move:
			{
				// update mouse position data because we need to print mouse position on the screen
				systemState_.mouseX = me_.GetPosX();
				systemState_.mouseY = me_.GetPosY();
				break;
			}
		}
	}
}


// executes rendering of each frame
void Engine::RenderFrame()
{
	this->graphics_.HandleMovementInput(kbe_, deltaTime_);
	graphics_.RenderFrame(&systemState_);
}