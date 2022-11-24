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

	// ------------------------------     WINDOW      ----------------------------------- //

	// initialize the window
	result = this->renderWindow_.Initialize(hInstance, windowTitle, windowClass, width, height);
	if (!result)  // if we can't initialize the window
	{
		return false;
	}


	// ------------------------------ GRAPHICS SYSTEM ----------------------------------- //

	// initialize the graphics system
	result = this->graphics_.Initialize(this->renderWindow_.GetHWND(), width, height, fullScreen);
	if (!result) // if we can't initialize the graphics system
	{
		return false;
	}


	// ------------------------ TIMERS (FPS, CPU, TIMER) -------------------------------- //

	fps_.Initialize();     // initialize the fps system
	cpu_.Initialize();     // initialize the cpu clock
						   
	// initialize the engine's timer
	if (!timer_.Initialize())
	{
		Log::Error(THIS_FUNC, "can't initialize the engine's timer");
		return false;
	}
	

	return true;
}

// hangle messages from the window
bool Engine::ProcessMessages()
{
	return this->renderWindow_.ProcessMessages();
}

// each frame we update the state of the engine
void Engine::Update()
{
	
	// to update the system stats each of timers classes we needs to call its 
	// own Frame function for each frame of execution the application goes through
	fps_.Frame();
	cpu_.Frame();
	timer_.Frame();

	systemState_.fps_ = fps_.GetFps();
	//systemState_.cpu_ = cpu_.GetCpuPercentage();

	// during each frame the position class object is updated with the 
	// frame time for calculation the updated position
	//editorCamera_.SetFrameTime(timer_.GetTime());

	// after the frame time update the position class movement functions can be updated
	// with the current state of the input devices. The movement function will update
	// the position of the camera to the location for this frame
	if (!keyboard_.KeyBufferIsEmpty() || !mouse_.EventBufferIsEmpty())
	{
		//editorCamera_.HandleMovement(keyboard_, mouse_);
	}

	// the new rotation of the camera is retrieved and sent to the Graphics::Frame function
	// to update the rendering of the camera position

	//editorCamera_.GetRotation(systemState_.editorRotation_);

	while (!keyboard_.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard_.ReadChar();
		std::string outmsg{ "Char: " };
		outmsg += ch;
		Log::Debug(THIS_FUNC, outmsg.c_str());
	}

	while (!keyboard_.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard_.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
		std::string outmsg{ "" };

		if (kbe.IsPress())
		{
			outmsg += "Key press: ";
		}
		if (kbe.IsRelease())
		{
			outmsg += "Key release: ";
		}

		outmsg += keycode;
		Log::Debug(THIS_FUNC, outmsg.c_str());
	}

	// actions on mouse events
	while (!mouse_.EventBufferIsEmpty())
	{
		
		MouseEvent me = mouse_.ReadEvent();

		if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
		{
			/*
			std::string outmsg{ "X: " };
			outmsg += std::to_string(me.GetPosX());
			outmsg += ", Y: ";
			outmsg += std::to_string(me.GetPosY());
			outmsg += "\n";
			Log::Debug(THIS_FUNC, outmsg.c_str());
			*/
		}
		
	}
}

void Engine::RenderFrame()
{
	this->graphics_.RenderFrame(&systemState_);
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