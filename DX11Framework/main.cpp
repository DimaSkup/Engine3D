///////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
///////////////////////////////////////////////////////////////////////////////

#include "Engine/Engine.h"
#include "Engine/Settings.h"

int main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND mainWnd;
	Log logger;          // this instance is necessary to create a logger text file
	Settings engineSettings;
	Engine engine;
	EventHandler eventHandler;  
	WindowContainer  windowContainer;

	////////////////////////////////////////////////

	eventHandler.AddEventListener(&engine);


	const std::wstring windowTitle = StringHelper::StringToWide(engineSettings.GetSettingStrByKey("WINDOW_TITLE"));
	const int windowWidth = engineSettings.GetSettingIntByKey("WINDOW_WIDTH");   // get the window width/height
	const int windowHeight = engineSettings.GetSettingIntByKey("WINDOW_HEIGHT");

	// set an event handler for the window container
	windowContainer.SetEventHandler(&eventHandler);

	// initialize the main window
	bool result = windowContainer.renderWindow_.Initialize(hInstance, 
		mainWnd,
		windowTitle,
		L"MyWindowClass",
		windowWidth, 
		windowHeight);
	COM_ERROR_IF_FALSE(result, "can't initialize the window");


	////////////////////////////////////////////////

	// explicitly initialize Windows Runtime and COM
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		printf("ERROR: can't explicitly initialize Windows Runtime and COM");
		return -1;
	}

	////////////////////////////////////////////////


	// intialize the engine
	result = engine.Initialize(hInstance, 
		mainWnd,
		engineSettings,
		windowTitle);
	if (!result)
	{
		Log::Error(LOG_MACRO, "can't initialize the engine");
		return -1;
	}

	// run the engine
	while (windowContainer.renderWindow_.ProcessMessages(hInstance, mainWnd) == true)
	{
		if (!engine.IsPaused())
		{
			engine.Update();
			engine.RenderFrame();
		}
		else
		{
			Sleep(100);
		}

		if (engine.IsExit())
			break;
	}

	windowContainer.renderWindow_.UnregisterWindowClass(hInstance);

	return 0;
}
