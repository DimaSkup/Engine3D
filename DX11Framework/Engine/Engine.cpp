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
		//pSystemState_ = new SystemState();
		pSystemState_ = std::make_shared<SystemState>();

		pSound_ = new SoundClass();
		pWindowContainer_ = new WindowContainer();
	}
	catch (std::bad_alloc & e)
	{
		Log::Error(LOG_MACRO, e.what());
	}
}


Engine::~Engine()
{
	Log::Print("-------------------------------------------------");
	Log::Print("            START OF THE DESTROYMENT:            ");
	Log::Print("-------------------------------------------------");

	Log::Debug(LOG_MACRO);

	
	_DELETE(pFps_);
	_DELETE(pCpu_);
	_DELETE(pTimer_);
	_DELETE(pGraphics_);
	_DELETE(pSound_);
	_DELETE(pWindowContainer_);

	Log::Print(LOG_MACRO, "the engine is shut down successfully");
}


///////////////////////////////////////////////////////////////////////////////
//
//                            PUBLIC FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////




bool Engine::Initialize(HINSTANCE hInstance,
						Settings* pEngineSettings,
	                    std::string windowClass)
{
	// this function initializes all the main parts of the engine

	try
	{
		bool result = false;
		std::string windowTitle{ pEngineSettings->GetSettingStrByKey("WINDOW_TITLE") };
		int windowWidth = pEngineSettings->GetSettingIntByKey("WINDOW_WIDTH");   // get the window width/height
		int windowHeight = pEngineSettings->GetSettingIntByKey("WINDOW_HEIGHT");
	

		pTimer_->Start();   // start the engine timer


		// ------------------------------     WINDOW      ------------------------------- //

		// initialize the window
		result = pWindowContainer_->renderWindow_.Initialize(hInstance, windowTitle, windowClass, windowWidth, windowHeight);
		COM_ERROR_IF_FALSE(result, "can't initialize the window");


		// ------------------------------ GRAPHICS SYSTEM ------------------------------- //

		// initialize the graphics system
		result = this->pGraphics_->Initialize(pWindowContainer_->renderWindow_.GetHWND(), pSystemState_);
		COM_ERROR_IF_FALSE(result, "can't initialize the graphics system");


		// ------------------------ TIMERS (FPS, CPU, TIMER) ---------------------------- //

		pFps_->Initialize();     // initialize the fps system
		pCpu_->Initialize();     // initialize the cpu clock


		// ------------------------------  SOUND SYSTEM --------------------------------- //

		// initialize the sound obj
		//result = this->sound_.Initialize(this->renderWindow_.GetHWND());
		//COM_ERROR_IF_FALSE(result, "can't initialize the sound obj");


		Log::Print(LOG_MACRO, "is initialized!");
	}
	catch (COMException& exception)
	{
		Log::Error(exception);
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool Engine::ProcessMessages()
{
	// this function handles messages from the window;

	// if we want to close the window and the engine as well
	if (pWindowContainer_->IsExit())       
		return false;

	return pWindowContainer_->renderWindow_.ProcessMessages();
}

///////////////////////////////////////////////////////////

void Engine::Update()
{
	// each frame this function updates the state of the engine;
	
	// to update the system stats each of timers classes we needs to call its 
	// own Frame function for each frame of execution the application goes through
	pFps_->Frame();
	pCpu_->Frame();
	deltaTime_ = pTimer_->GetMilisecondsElapsed();
	pTimer_->Restart();

	pSystemState_->fps = pFps_->GetFps();
	pSystemState_->cpu = pCpu_->GetCpuPercentage();

	
	// handle events both from the mouse and keyboard
	this->HandleMouseEvents();
	this->HandleKeyboardEvents();

	return;
}

///////////////////////////////////////////////////////////

void Engine::RenderFrame()
{
	// this function executes rendering of each frame;

	try
	{
		HWND hwnd = pWindowContainer_->renderWindow_.GetHWND();

		// we have to call keyboard handling here because in another case we will have 
		// a delay between pressing on some key and handling of this event; 
		// for instance: a delay between a W key pressing and start of the moving;
		this->pGraphics_->HandleKeyboardInput(keyboardEvent_,
			hwnd,
			deltaTime_);

		this->pGraphics_->RenderFrame(hwnd,
			deltaTime_);
	}
	catch (COMException & e)
	{
		Log::Error(e, true);
		Log::Error(LOG_MACRO, "can't render a frame");
		throw std::exception("can't render a frame");
	}

	return;
}




///////////////////////////////////////////////////////////////////////////////
//
//                            PRIVATE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////



void Engine::HandleMouseEvents()
{
	// handle mouse events
	while (!pWindowContainer_->pMouse_->EventBufferIsEmpty())
	{
		mouseEvent_ = pWindowContainer_->pMouse_->ReadEvent();

		switch (mouseEvent_.GetType())
		{
			case MouseEvent::EventType::Move:
			{
				// update mouse position data because we need to print mouse position on the screen
				pSystemState_->mouseX = mouseEvent_.GetPosX();
				pSystemState_->mouseY = mouseEvent_.GetPosY();
				break;
			}
			default:
			{
				// each time when we execute raw mouse move we update the camera's rotation
				this->pGraphics_->HandleMouseInput(mouseEvent_,
					pWindowContainer_->renderWindow_.GetWindowDimensions(),
					deltaTime_);
				break;
			}
		} // switch
	} // while

	return;
}

///////////////////////////////////////////////////////////

void Engine::HandleKeyboardEvents()
{
	// handle keyboard events
	while (!pWindowContainer_->pKeyboard_->KeyBufferIsEmpty())
	{
		keyboardEvent_ = pWindowContainer_->pKeyboard_->ReadKey();

		// if we pressed the ESC button we exit from the application
		if (keyboardEvent_.GetKeyCode() == VK_ESCAPE)
		{
			pWindowContainer_->isExit_ = true;
			return;
		}

		// if F2 we change the rendering fill mode
		if (keyboardEvent_.IsPress() && keyboardEvent_.GetKeyCode() == VK_F2)
		{
			pGraphics_->ChangeModelFillMode();
			Log::Debug(LOG_MACRO, "F2 key is pressed");
			return;
		}

	} // end while

	return;
}