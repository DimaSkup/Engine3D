///////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
///////////////////////////////////////////////////////////////////////////////

#include "Engine/Engine.h"
#include "Engine/Log.h"
#include "Engine/Settings.h"




int main()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	Log logger;   // this instance is necessary to create a logger text file
	SETTINGS SETTINGS;

	// intialize the engine
	Engine engine;
	engine.Initialize(hInstance, 
		SETTINGS::GetSettings()->WINDOW_TITLE,
		"MyWindowClass",
		SETTINGS::GetSettings()->WINDOW_WIDTH,
		SETTINGS::GetSettings()->WINDOW_HEIGHT,
		SETTINGS::GetSettings()->FULL_SCREEN);

	// run the engine
	while (engine.ProcessMessages() == true)
	{
		engine.Update();
		engine.RenderFrame();
	}

	return 0;
}
