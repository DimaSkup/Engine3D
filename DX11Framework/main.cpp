///////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
///////////////////////////////////////////////////////////////////////////////

#include "Engine/Engine.h"
//#include "Engine/Log.h"
#include "Engine/Settings.h"

int main()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	Log logger;          // this instance is necessary to create a logger text file
	Settings engineSettings;
	Engine engine;

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
	const bool result = engine.Initialize(hInstance, 
		engineSettings,
		L"MyWindowClass");
	if (!result)
	{
		Log::Error(LOG_MACRO, "can't initialize the engine");
		return -1;
	}

	// run the engine
	while (engine.ProcessMessages() == true)
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
	}

	return 0;
}
