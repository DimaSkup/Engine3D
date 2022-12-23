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
	bool result = false;

	timer_.Start();   // start the engine timer


	// ------------------------------     WINDOW      ----------------------------------- //

	// initialize the window
	result = this->renderWindow_.Initialize(hInstance, windowTitle, windowClass, width, height);
	if (!result)  // if we can't initialize the window
	{
		return false;
	}


	// ------------------------------ GRAPHICS SYSTEM ----------------------------------- //

	// initialize the graphics system
	result = this->graphics_.Initialize(this->renderWindow_.GetHWND());
	if (!result) // if we can't initialize the graphics system
	{
		return false;
	}


	// ------------------------ TIMERS (FPS, CPU, TIMER) -------------------------------- //

	fps_.Initialize();     // initialize the fps system
	cpu_.Initialize();     // initialize the cpu clock

						   
						   
	/*
	// initialize the engine's timer
	if (!timer_.Initialize())
	{
		Log::Error(THIS_FUNC, "can't initialize the engine's timer");
		return false;
	}
	*/
	

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
	//timer_.Frame();

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
			this->graphics_.editorCamera_.AdjustRotation((float)me_.GetPosY() * 0.01f, (float)me_.GetPosX() * 0.01f, 0);
		}
		
	}
	

	
}

void Engine::RenderFrame()
{
	this->graphics_.RenderFrame(&systemState_, kbe_, me_, mouse_, deltaTime_);
}

/*
bool Engine::Initialize(void)
{

	// ------------------------- WINDOW AND INPUT MANAGER -------------------------- //

	m_inputManager = new(std::nothrow) InputManager();
	if (!m_inputManager)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the InputManager");
		return false;
	}

	
	m_inputManager->Initialize();
	m_window->SetInputManager(m_inputManager);
	

	return true;
}


// The Run function is where our application will loop and do all the application
// processing until we decide to quit
void Engine::Run(void)
{
	
	Log::Get()->Debug(THIS_FUNC_EMPTY);
	m_inputManager->AddInputListener(m_input);

	if (m_init)
	{
		while (frame());
	}

	return;
} // Run()


// The Frame function is where all the processing for our application is done
bool Engine::frame(void)
{
	bool result = false;

	// handle messages from the window 
	// (as well as input from devices because it is based on WinAPI)
	m_window->RunEvent();


	if (!m_window->IsActive())
		return true;

	// if there is an exit we stop the engine processing
	if (m_window->IsExit())
	{
		return false;
	}

	if (m_window->IsResize())
	{
	}

	// Check if the user pressed escape and wants to exit the application
	if (m_input->IsEscapePressed())
	{
		Log::Get()->Debug(THIS_FUNC, "the ESC is pressed");
		return false;
	}
	

	



	// Do the frame processing for the graphics object
	if (!m_graphics->Frame(m_pPosition))
	{
		Log::Get()->Error(THIS_FUNC, "there is something went wrong during the frame processing");
		return false;
	}

	// finally render the graphics to the screen
	result = m_graphics->Render(m_input,
		                        m_pFps->GetFps(),
		                        m_pCpu->GetCpuPercentage(),
		                        m_pTimer->GetTime());
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't render the graphics to the screen");
		return false;
	}

	return true;
} // frame()
*/