///////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
///////////////////////////////////////////////////////////////////////////////

//#include "engine.h"
#include "Engine/Engine.h"
#include "Engine/Log.h"

int main()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	Log logger;   // this instance is necessary to create a logger text file

	Engine engine;
	engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600);
	while (engine.ProcessMessages() == true)
	{
		engine.Update();
	}
	/*
	Engine* engine;
	bool result;

	engine = new(std::nothrow) Engine();
	

	// set up the engine configuration
	engine->SetDescription(800, 600, false);


	// initialize and run the engine
	result = engine->Initialize();
	if (result)
	{
		Log::Get()->Debug(THIS_FUNC, "the engine object is successfully initialized");
		engine->Run();
	}

	// shutdown and release the engine object
	_SHUTDOWN(engine);
	*/
	
	return 0;
}
