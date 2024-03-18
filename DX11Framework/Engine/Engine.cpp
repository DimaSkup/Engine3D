///////////////////////////////////////////////////////////////////////////////
// Filename: engine.cpp
// Revising: 05.10.22
///////////////////////////////////////////////////////////////////////////////
#include "Engine.h"

#include <functional>
#include <Psapi.h>
#include <winuser.h>

Engine::Engine()
{
	timer_.Reset();       // reset the engine/game timer
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
						HWND mainWnd,
						Settings & engineSettings,
						const std::wstring & windowTitle)
{
	// this function initializes all the main parts of the engine

	try
	{
		bool result = false;

	
		// ------------------------ TIMERS (CPU, GAME TIMER) ---------------------------- //

		
		cpu_.Initialize();     // initialize the cpu clock
		

		// ------------------------------     WINDOW      ------------------------------- //

		// store a handle to the application instance
		this->hInstance_ = hInstance;  

		this->hwnd_ = mainWnd;
		this->windowTitle_ = windowTitle;


		// ------------------------------ GRAPHICS SYSTEM ------------------------------- //

		// initialize the graphics system
		result = graphics_.Initialize(hwnd_, systemState_);
		COM_ERROR_IF_FALSE(result, "can't initialize the graphics system");


		// ------------------------------  SOUND SYSTEM --------------------------------- //

		// initialize the sound obj
		//result = sound_.Initialize(hwnd);
		//COM_ERROR_IF_FALSE(result, "can't initialize the sound system");


		// ------------------------------  INPUT SYSTEM --------------------------------- //

		// setup keyboard input params
		keyboard_.EnableAutoRepeatKeys();
		keyboard_.EnableAutoRepeatChars();

		// execute tick so we will be able to receive the initialization time
		timer_.Tick();

		// set the duration time of the engine initialization process
		graphics_.GetUserInterface().SetStringByKey(
			graphics_.GetD3DClass().GetDevice(),
			"init_duration_time",
			{ "Init time: " + std::to_string(timer_.GetGameTime()) + "s" },    // duration of the initialization time
			{ 10, 300 });                                                // draw at this screen position
		

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

		// reset for next average
		frameCount = 0;
		timeElapsed += 1.0f;

		// store the fps value for later using (for example: render this value as text onto the screen)
		systemState_.fps = fps;
		systemState_.frameTime = msPerFrame;



		PROCESS_MEMORY_COUNTERS pmc;
		DWORD processID;
		GetWindowThreadProcessId(hwnd_, &processID);

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
		COM_ERROR_IF_FALSE(hProcess != NULL, "can't get a process handle of the window");
		
		GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));

		std::wostringstream outs;
		outs.precision(6);
		outs << windowTitle_ << L" "
			 << L"FPS: " << fps << L" "
			 << L"Frame Time: " << msPerFrame << L" (ms); "
			 << L"RAM Usage: " << pmc.WorkingSetSize / 1024 / 1024 << L" (mb)";
		SetWindowText(hwnd_, outs.str().c_str());		
	}
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




///////////////////////////////////////////////////////////////////////////////////////////////
//                              PUBLIC EVENT HANDLERS API
///////////////////////////////////////////////////////////////////////////////////////////////

void Engine::EventActivate(const APP_STATE state)
{
	// define that the app is curretly running or paused

	if (state == APP_STATE::ACTIVATED)
	{
		isPaused_ = false;
		timer_.Start();
	}
	else if (state == APP_STATE::DEACTIVATED)
	{
		isPaused_ = true;
		timer_.Stop();
	}

	return;
}

///////////////////////////////////////////////////////////

void Engine::EventWindowMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return;
}

///////////////////////////////////////////////////////////

void Engine::EventWindowResize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Log::Debug(LOG_MACRO, "THE WINDOW IS RESIZED");

	// new values of window width/height
	const int width = LOWORD(lParam);
	const int height = HIWORD(lParam);

	// get the window and client dimensions
	RECT winRect;
	RECT clientRect;
	GetWindowRect(hwnd, &winRect);
	GetClientRect(hwnd, &clientRect);



	// update the window rectangle
	winRect.left = winRect.left;
	winRect.top = winRect.top;
	winRect.right = winRect.left + width;
	winRect.bottom = winRect.top + height;
	AdjustWindowRect(&winRect, GetWindowLong(hwnd, GWL_STYLE), FALSE);


	// set new dimenstions for the window
	SetWindowPos(hwnd, 0,
		winRect.left,
		winRect.top,
		winRect.right - winRect.left,
		winRect.bottom - winRect.top,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

	// update this window with new dimensions
	UpdateWindow(hwnd);

	// update the currernt window and client dimensions for the renderWindow object
	//windowContainer_.renderWindow_.UpdateWindowDimensions(width, height);
	//windowContainer_.renderWindow_.UpdateClientDimensions(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);


	return;
}

///////////////////////////////////////////////////////////

void Engine::EventWindowSizing(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Log::Debug("WINDOW SIZING EVENT BUT WE DO NOTHING");

	return;
}

///////////////////////////////////////////////////////////

void Engine::EventKeyboard(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// THIS FUNCTION is a handler for all the keyboard events;
	// 
	// in case if we got some keyboard event 
	// from the window the call chain for this function: 
	// 1. WindowContainer::WindowProc() ->
	// 2. EventHandler::HandleEvent() ->
	// 3. this function

	this->inputManager_.HandleKeyboardMessage(keyboard_, uMsg, wParam, lParam);

	KeyboardClass & keyboard = keyboard_;


	while (!keyboard.KeyBufferIsEmpty())
	{
		// store the keycode of the pressed key
		const unsigned char keyCode = keyboardEvent_.GetKeyCode();

		// if we pressed the ESC button we exit from the application
		if (keyCode == VK_ESCAPE)
		{
			Log::Debug(LOG_MACRO, "Esc is pressed");
			isExit_ = true;
			return;
		}

		// store what type of the keyboard event we have 
		keyboardEvent_ = keyboard.ReadKey();

		// if we are currently pressing some key
		//if (keyboardEvent_.IsPress())
		//{

		//}


	} // end while
}

///////////////////////////////////////////////////////////

void Engine::EventMouse(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// THIS FUNCTION is a handler for all the mouse events;
	// 
	// in case if we got some mouse event 
	// from the window the call chain for this function: 
	// 1. WindowContainer::WindowProc() ->
	// 2. EventHandler::HandleEvent() ->
	// 3. this function

	this->inputManager_.HandleMouseMessage(mouse_, uMsg, wParam, lParam);


	MouseClass & mouse = mouse_;

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
					{ (LONG)windowWidth_, (LONG)windowHeight_ },//windowContainer_.renderWindow_.GetWindowDimensions(),
					deltaTime_);
				break;
			}
		} // switch
	} // while
}
