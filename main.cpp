///////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
///////////////////////////////////////////////////////////////////////////////

//#include "engine.h"
#include "Window/RenderWindow.h"
#include "LogSystem/Log.h"

int main()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	Log logger;

	RenderWindow rw;
	rw.Initialize(hInstance, "title", "MyWindowClass", 800, 600);
	while (rw.ProcessMessages() == true)
	{

	}
	/*
	Engine* engine;
	bool result;

	engine = new(std::nothrow) Engine();
	if (!engine)
	{
		Log::Get()->Error("main(): can't allocate memory for the SystemClass object");
		return 0;
	}

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
