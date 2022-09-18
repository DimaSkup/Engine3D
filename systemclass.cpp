///////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

SystemClass::SystemClass(const SystemClassDesc& desc)
{
	m_input = nullptr;
	m_graphics = nullptr;
	if (!ApplicationHandle)
		ApplicationHandle = this;
	m_sysDesc = desc;

	// initialize timers pointers
	m_pFps = nullptr;
	m_pCpu = nullptr;
	m_pTimer = nullptr;

	Log::Get()->Debug(THIS_FUNC_EMPTY);
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass(void)
{
}

bool SystemClass::Initialize(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);
	bool result;


	// ------------------------- WINDOW AND INPUT MANAGER -------------------------- //
	m_window = new(std::nothrow) Window();
	if (!m_window)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the Window");
		return false;
	}

	m_inputManager = new(std::nothrow) InputManager();
	if (!m_inputManager)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for the InputManager");
		return false;
	}

	
	m_inputManager->Initialize();

	// set the default window parameters
	DescWindow winDesc;
	winDesc.width = m_sysDesc.width;
	winDesc.height = m_sysDesc.height;
	winDesc.fullScreen = m_sysDesc.fullScreen;
	result = m_window->Initialize(winDesc);
	
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the window");
		return false;
	}
	m_window->SetInputManager(m_inputManager);
	
	

	

	// -------------------------- INPUT AND GRAPHICS  -------------------------------- //

	m_input = new InputClass;	// Create the input object. This object will be used to handle reading the keyboard input from the user
	m_graphics = new GraphicsClass;	// Create the graphics object. This object will handle rendering all the graphics for this app

	if (!m_input || !m_graphics)
	{
		Log::Get()->Error(THIS_FUNC, "can't allocate the memory for InputClass or GraphicsClass");
		return false;
	}
	
	result = m_graphics->Initialize(m_sysDesc.width, m_sysDesc.height, m_window->GetHWND(), m_sysDesc.fullScreen);
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the graphics class");
		return false;
	}
	
	
	// --------------------- TIMERS (FPS, CPU, TIMER) -------------------------------- //
	// create the fps object
	m_pFps = new(std::nothrow) FpsClass();
	if (!m_pFps)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the fps object");
		return false;
	}

	// initialize the fps object
	m_pFps->Initialize();


	
	// create the cpu object
	m_pCpu = new(std::nothrow) CpuClass();
	if (!m_pCpu)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the cpu object");
		return false;
	}
	

	// initialize the cpu object
	m_pCpu->Initialize();


	// create the timer object
	m_pTimer = new(std::nothrow) TimerClass();
	if (!m_pTimer)
	{
		Log::Get()->Error(THIS_FUNC, "can't create the timer object");
		return false;
	}

	// initialize the timer object
	result = m_pTimer->Initialize();
	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the timer object");
	}


	Log::Get()->Debug(THIS_FUNC, "the end");
	
	return true;
}

// The Shutdown function does the clean up
void SystemClass::Shutdown()
{
	_DELETE(m_pTimer);  // release the timer object
	_SHUTDOWN(m_pCpu);  // release the cpu object
	_DELETE(m_pFps);    // release the fps object
	_SHUTDOWN(m_inputManager); // release the input manager object
	_SHUTDOWN(m_window);       // release the window object
	_SHUTDOWN(m_graphics);     // release the graphics object
	_DELETE(m_input);          // release the input object
	ApplicationHandle = nullptr;

	return;
}


// The Run function is where our application will loop and do all the application
// processing until we decide to quit
void SystemClass::Run(void)
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
bool SystemClass::frame(void)
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
	

	// to update the system stats each of timers classes needs to call its 
	// own Frame function for each frame of execution the application goes through
	m_pTimer->Frame();
	m_pFps->Frame();
	m_pCpu->Frame();

	// Do the frame processing for the graphics object
	if (!m_graphics->Frame(0.0f))
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