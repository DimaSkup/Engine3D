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
	Settings* pEngineSettings = Settings::Get();

	// intialize the engine
	Engine engine;

	engine.Initialize(hInstance, 
		pEngineSettings,
		"MyWindowClass");

	// run the engine
	while (engine.ProcessMessages() == true)
	{
		engine.Update();
		engine.RenderFrame();
	}

	return 0;
}
