///////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
///////////////////////////////////////////////////////////////////////////////

#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance, 
					PSTR pScmdLine,
					int iCmdShow)
{
	SystemClass* System;
	bool result;

	// Create the system object
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// Initialize and run the system object
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// Shutdown and release the system object
	_SHUTDOWN(System);

	return 0;
}