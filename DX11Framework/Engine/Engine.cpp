///////////////////////////////////////////////////////////////////////////////
// Filename: engine.cpp
// Revising: 05.10.22
///////////////////////////////////////////////////////////////////////////////
#include "Engine.h"


Engine::Engine()
{
}


Engine::~Engine()
{
	Log::Print("-------------------------------------------------");
	Log::Print("            START OF THE DESTROYMENT:            ");
	Log::Print("-------------------------------------------------");


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
		const std::string windowTitle{ pEngineSettings->GetSettingStrByKey("WINDOW_TITLE") };
		const int windowWidth = pEngineSettings->GetSettingIntByKey("WINDOW_WIDTH");   // get the window width/height
		const int windowHeight = pEngineSettings->GetSettingIntByKey("WINDOW_HEIGHT");
		
	

		timer_.Start();   // start the engine timer


		// ------------------------------     WINDOW      ------------------------------- //

		// initialize the window
		result = windowContainer_.renderWindow_.Initialize(hInstance, windowTitle, windowClass, windowWidth, windowHeight);
		COM_ERROR_IF_FALSE(result, "can't initialize the window");

		HWND hwnd = windowContainer_.renderWindow_.GetHWND();

		// ------------------------------ GRAPHICS SYSTEM ------------------------------- //

		// initialize the graphics system
		result = graphics_.Initialize(hwnd, systemState_);
		COM_ERROR_IF_FALSE(result, "can't initialize the graphics system");


		// ------------------------ TIMERS (FPS, CPU, TIMER) ---------------------------- //

		fps_.Initialize();     // initialize the fps system
		cpu_.Initialize();     // initialize the cpu clock


		// ------------------------------  SOUND SYSTEM --------------------------------- //

		// initialize the sound obj
		result = sound_.Initialize(hwnd);
		COM_ERROR_IF_FALSE(result, "can't initialize the sound system");


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
	if (windowContainer_.IsExit())       
		return false;

	return windowContainer_.renderWindow_.ProcessMessages();
}

///////////////////////////////////////////////////////////

void Engine::Update()
{
	// each frame this function updates the state of the engine;
	
	// to update the system stats each of timers classes we needs to call its 
	// own Update function for each frame of execution the application goes through
	fps_.Update(gameCycles_);
	cpu_.Update();

	deltaTime_ = timer_.GetMilisecondsElapsed();

	// later we will use the frame time speed
	// to calculate how fast the viewer should move and rotate;
	// also if we have less than 60 frames per second we set this value to 16.6f (1000 miliseconds / 60 = 16.6)
	if (deltaTime_ > 16.6f) deltaTime_ = 16.6f;

	timer_.Restart();

	// update the count of frames per second
	systemState_.fps = fps_.GetFps();

	// update the percentage of total cpu use that is occuring each second
	//systemState_.cpu = cpu_->GetCpuPercentage();

	const std::string newCaption{ "FPS: " + std::to_string(systemState_.fps) };

	// update the caption of the window
	SetWindowTextA(windowContainer_.renderWindow_.GetHWND(), newCaption.c_str());

	
	
	// handle events both from the mouse and keyboard
	this->HandleMouseMovement();
	this->HandleKeyboardEvents();

	return;
}

///////////////////////////////////////////////////////////

void Engine::RenderFrame()
{
	// this function executes rendering of each frame;

	try
	{
		// we have to call keyboard handling here because in another case we will have 
		// a delay between pressing on some key and handling of this event; 
		// for instance: a delay between a W key pressing and start of the moving;
		graphics_.HandleKeyboardInput(keyboardEvent_, deltaTime_);

		graphics_.RenderFrame(windowContainer_.renderWindow_.GetHWND(), 
			systemState_, 
			deltaTime_,
			gameCycles_);
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



void Engine::HandleMouseMovement()
{

	MouseClass & mouse = windowContainer_.mouse_;

	// handle mouse events
	while (!mouse.EventBufferIsEmpty())
	{
		mouseEvent_ = mouse.ReadEvent();
		const MouseEvent::EventType eventType = mouseEvent_.GetType();

		switch (eventType)
		{
			case MouseEvent::EventType::Move:
			{
				MousePoint mPoint = mouseEvent_.GetPos();

				// update mouse position data because we need to print mouse position on the screen
				systemState_.mouseX = mPoint.x;
				systemState_.mouseY = mPoint.y;
				break;
			}
			default:
			{
				// each time when we execute raw mouse move we update the camera's rotation
				graphics_.HandleMouseInput(mouseEvent_, 
					eventType,
					windowContainer_.renderWindow_.GetWindowDimensions(),
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
	KeyboardClass & keyboard = windowContainer_.keyboard_;

	// handle keyboard events
	while (!keyboard.KeyBufferIsEmpty())
	{
		// store what type of the keyboard event we have 
		keyboardEvent_ = keyboard.ReadKey();

		// store the keycode of the pressed key
		const unsigned char keyCode = keyboardEvent_.GetKeyCode();

		// if we pressed the ESC button we exit from the application
		if (keyCode == VK_ESCAPE)
		{
			windowContainer_.isExit_ = true;
			return;
		}

		// if F2 we change the rendering fill mode
		if (keyboardEvent_.IsPress() && keyCode == VK_F2)
		{
			graphics_.ChangeModelFillMode();
			Log::Debug(LOG_MACRO, "F2 key is pressed");
			return;
		}

	} // end while

	return;
}