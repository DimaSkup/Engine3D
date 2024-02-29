///////////////////////////////////////////////////////////////////////////////
// Filename: engine.cpp
// Revising: 05.10.22
///////////////////////////////////////////////////////////////////////////////
#include "Engine.h"

#include <functional>

Engine::Engine()
{
}


Engine::~Engine()
{
	Log::Print("-------------------------------------------------");
	Log::Print("            START OF THE DESTROYMENT:            ");
	Log::Print("-------------------------------------------------");

	// unregister the window class, destroys the window,
	// reset the responsible members;

	if (this->hwnd_ != NULL)
	{
		windowContainer_.renderWindow_.UnregisterWindowClass(hInstance_);
		ChangeDisplaySettings(NULL, 0); // before destroying the window we need to set it to the windowed mode
		DestroyWindow(this->hwnd_);  // Remove the window
		this->hwnd_ = NULL;
		this->hInstance_ = NULL;

		Log::Debug(LOG_MACRO);
	}


	Log::Print(LOG_MACRO, "the engine is shut down successfully");
}


///////////////////////////////////////////////////////////////////////////////
//
//                            PUBLIC FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////




bool Engine::Initialize(HINSTANCE hInstance,
						Settings & engineSettings,
	                    const std::wstring & windowClass)
{
	// this function initializes all the main parts of the engine

	try
	{
		bool result = false;
		windowTitle_ = StringHelper::StringToWide(engineSettings.GetSettingStrByKey("WINDOW_TITLE"));
		const int windowWidth  = engineSettings.GetSettingIntByKey("WINDOW_WIDTH");   // get the window width/height
		const int windowHeight = engineSettings.GetSettingIntByKey("WINDOW_HEIGHT");
		
	
		// ------------------------ TIMERS (CPU, GAME TIMER) ---------------------------- //

		timer_.Reset();       // reset the engine/game timer
		cpu_.Initialize();     // initialize the cpu clock
		

		// ------------------------------     WINDOW      ------------------------------- //

		// initialize the window
		result = windowContainer_.renderWindow_.Initialize(hInstance, hwnd_, windowTitle_, windowClass, windowWidth, windowHeight);
		COM_ERROR_IF_FALSE(result, "can't initialize the window");

		// store a handle to the application instance
		this->hInstance_ = hInstance;  


		// ------------------------------ GRAPHICS SYSTEM ------------------------------- //

		// initialize the graphics system
		result = graphics_.Initialize(hwnd_, systemState_);
		COM_ERROR_IF_FALSE(result, "can't initialize the graphics system");


		// ------------------------------  SOUND SYSTEM --------------------------------- //

		// initialize the sound obj
		//result = sound_.Initialize(hwnd);
		//COM_ERROR_IF_FALSE(result, "can't initialize the sound system");


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

	return windowContainer_.renderWindow_.ProcessMessages(hInstance_, hwnd_);
}

///////////////////////////////////////////////////////////

void Engine::Update()
{
	// each frame this function updates the state of the engine;

	timer_.Tick();
	
	// to update the system stats each of timers classes we needs to call its 
	// own Update function for each frame of execution the application goes through
	cpu_.Update();

	// update the percentage of total cpu use that is occuring each second
	//systemState_.cpu = cpu_->GetCpuPercentage();

	// get the time which passed since the previous frame
	float deltaTime = timer_.GetDeltaTime();

	// later we will use the frame time speed
	// to calculate how fast the viewer should move and rotate;
	// also if we have less than 60 frames per second we set this value to 16.6f (1000 miliseconds / 60 = 16.6)
	if (deltaTime > 16.6f) deltaTime = 16.6f;

	// update the member delta time
	deltaTime_ = deltaTime;

	// handle events both from the mouse and keyboard
	this->HandleMouseMovement(deltaTime_);
	this->HandleKeyboardEvents();

	// this method is called every frame in order to count the frame
	CalculateFrameStats();

	return;
}

void Engine::CalculateFrameStats()
{
	// measure the number of frames being rendered per second (FPS);
	// this method would be called every frame in order to count the frame

	// code computes the avetage frames per second, and also the average time it takes
	// to render one frame. These stats are appended to the window caption bar

	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	// compute averages over one second period
	if ((timer_.GetGameTime() - timeElapsed) >= 1.0f)
	{
		const int fps = frameCount;   // fps = frameCount / 1
		const float msPerFrame = 1000.0f / (float)fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << windowTitle_ << L" " 
			 << L"FPS: " << fps << L" "
			 << L"Frame Time: " << msPerFrame << L" (ms)";
		SetWindowText(hwnd_, outs.str().c_str());

		// reset for next average
		frameCount = 0;
		timeElapsed += 1.0f;

		// store the fps value for later using (for example: render this value as text onto the screen)
		systemState_.fps = fps;
		systemState_.frameTime = msPerFrame;
	}
}


///////////////////////////////////////////////////////////

void Engine::RenderFrame()
{
	// this function executes rendering of each frame;

	try
	{
		Log::Print(LOG_MACRO, std::to_string(timer_.GetGameTime()));

		

		// we have to call keyboard handling here because in another case we will have 
		// a delay between pressing on some key and handling of this event; 
		// for instance: a delay between a W key pressing and start of the moving;
		graphics_.HandleKeyboardInput(keyboardEvent_, deltaTime_);
		
		graphics_.RenderFrame(systemState_, deltaTime_, timer_.GetGameTime());
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



void Engine::HandleMouseMovement(const float deltaTime)
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
					deltaTime);
				break;
			}
		} // switch
	} // while

	return;
}

///////////////////////////////////////////////////////////

void Engine::HandleKeyboardEvents()
{
	// handle main keyboard events 
	// (exit/quit, pause, minimization/maximization of the window, etc.)

	KeyboardClass & keyboard = windowContainer_.keyboard_;


	while (!keyboard.KeyBufferIsEmpty())
	{
		// store the keycode of the pressed key
		const unsigned char keyCode = keyboardEvent_.GetKeyCode();

		// if we pressed the ESC button we exit from the application
		if (keyCode == VK_ESCAPE)
		{
			windowContainer_.isExit_ = true;
			return;
		}

		// store what type of the keyboard event we have 
		keyboardEvent_ = keyboard.ReadKey();

		// if we are currently pressing some key
		//if (keyboardEvent_.IsPress())
		//{
			
		//}
		

	} // end while

	return;
}

///////////////////////////////////////////////////////////

HWND Engine::GetHWND() const
{
	// return a handle to the main window
	return this->hwnd_;
}

HINSTANCE Engine::GetInstance() const
{
	// return a handle to the application (engine/game) instance
	return this->hInstance_;
}