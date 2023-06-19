///////////////////////////////////////////////////////////////////////////////
// Filename: engine.cpp
// Revising: 05.10.22
///////////////////////////////////////////////////////////////////////////////
#include "Engine.h"


Engine::Engine()
{
	try
	{
		pGraphics_ = new GraphicsClass();
		pFps_ = new FpsClass();
		pCpu_ = new CpuClass();
		pTimer_ = new Timer();
		pSystemState_ = new SystemState();
		pSound_ = new SoundClass();
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(THIS_FUNC, e.what());
	}
}


Engine::~Engine()
{
	Log::Print("-------------------------------------------------");
	Log::Print("            START OF THE DESTROYMENT:            ");
	Log::Print("-------------------------------------------------");

	Log::Debug(THIS_FUNC_EMPTY);

	_DELETE(pGraphics_);
	_DELETE(pFps_);
	_DELETE(pCpu_);
	_DELETE(pTimer_);
	_DELETE(pSystemState_);
	_DELETE(pSound_);

	Log::Print(THIS_FUNC, "the engine is shut down successfully");
}


///////////////////////////////////////////////////////////////////////////////
//
//                            PUBLIC FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////



// initialize all the main parts of the engine
bool Engine::Initialize(HINSTANCE hInstance,
						Settings* pEngineSettings,
	                    std::string windowClass)
{
	try
	{
		bool result = false;
		std::string windowTitle{ pEngineSettings->GetSettingStrByKey("WINDOW_TITLE") };
		int windowWidth = pEngineSettings->GetSettingIntByKey("WINDOW_WIDTH");   // get the window width/height
		int windowHeight = pEngineSettings->GetSettingIntByKey("WINDOW_HEIGHT");
	

		pTimer_->Start();   // start the engine timer


		// ------------------------------     WINDOW      ------------------------------- //

		// initialize the window
		result = this->renderWindow_.Initialize(hInstance, windowTitle, windowClass, windowWidth, windowHeight);
		COM_ERROR_IF_FALSE(result, "can't initialize the window");


		// ------------------------------ GRAPHICS SYSTEM ------------------------------- //

		// initialize the graphics system
		result = this->pGraphics_->Initialize(this->renderWindow_.GetHWND());
		COM_ERROR_IF_FALSE(result, "can't initialize the graphics system");


		// ------------------------ TIMERS (FPS, CPU, TIMER) ---------------------------- //

		pFps_->Initialize();     // initialize the fps system
		pCpu_->Initialize();     // initialize the cpu clock


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
	pFps_->Frame();
	pCpu_->Frame();
	deltaTime_ = pTimer_->GetMilisecondsElapsed();
	pTimer_->Restart();

	pSystemState_->fps = pFps_->GetFps();
	pSystemState_->cpu = pCpu_->GetCpuPercentage();

	

	// handle keyboard events
	while  (!keyboard_.KeyBufferIsEmpty())
	{
		keyboardEvent_ = keyboard_.ReadKey();

		// if we pressed the ESC button we exit from the application
		if (keyboardEvent_.GetKeyCode() == VK_ESCAPE)
		{
			isExit_ = true;
			return;
		}

		if (keyboardEvent_.IsPress() && keyboardEvent_.GetKeyCode() == VK_F2)
		{
			pGraphics_->ChangeModelFillMode();
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
		mouseEvent_ = mouse_.ReadEvent();

		switch (mouseEvent_.GetType())
		{
			case MouseEvent::EventType::RAW_MOVE:
			{
				// each time when we execute raw mouse move we update the camera's rotation
				this->pGraphics_->HandleMovementInput(mouseEvent_, deltaTime_);
				break;
			}
			case MouseEvent::EventType::Move:
			{
				// update mouse position data because we need to print mouse position on the screen
				pSystemState_->mouseX = mouseEvent_.GetPosX();
				pSystemState_->mouseY = mouseEvent_.GetPosY();
				break;
			}
		}
	}
}


// executes rendering of each frame
void Engine::RenderFrame()
{
	this->pGraphics_->HandleMovementInput(keyboardEvent_, deltaTime_);
	this->pGraphics_->RenderFrame(pSystemState_);
}