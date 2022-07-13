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

	// initialize the WinAPI
	//InitializeWindows(screenWidth, screenHeight);

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
	
	

	



	m_input = new InputClass;	// Create the input object. This object will be used to handle reading the keyboard input from the user
	m_graphics = new GraphicsClass;	// Create the graphics object. This object will handle rendering all the graphics for this app

	if (!m_input || !m_graphics)
	{
		Log::Get()->Error("SystemClass::Initialize(): can't allocate the memory for InputClass or GraphicsClass");
		return false;
	}
	
	result = m_graphics->Initialize(m_sysDesc.width, m_sysDesc.height, m_window->GetHWND(), m_sysDesc.fullScreen);

	if (!result)
	{
		Log::Get()->Error(THIS_FUNC, "can't initialize the graphics class");
		return false;
	}
	
	

	Log::Get()->Debug(THIS_FUNC, "the end");
	
	return true;
}

// The Shutdown function does the clean up
void SystemClass::Shutdown()
{
	_SHUTDOWN(m_inputManager);
	_SHUTDOWN(m_window);
	_SHUTDOWN(m_graphics);
	_DELETE(m_input);
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
}


// The Frame function is where all the processing for our application is done
bool SystemClass::frame(void)
{

	// handle messages from the window
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
	if (!m_graphics->Frame(m_input->GetActiveKeyCode()))
	{
		Log::Get()->Error(THIS_FUNC, "there is something went wrong during the frame processing");
		return false;
	}

	return true;
}