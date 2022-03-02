///////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
///////////////////////////////////////////////////////////////////////////////

#include "systemclass.h"
#include "log.h"

int main()
{
	
	SystemClass* System;
	SystemClassDesc sysDesc;
	bool result;


	// Create the system object
	sysDesc.fullScreen = false;
	System = new(std::nothrow) SystemClass(sysDesc);
	Log::Get()->Debug("here");
	if (!System)
	{
		Log::Get()->Error("main(): can't allocate memory for the SystemClass object");
		return 0;
	}

	

	// Initialize and run the system object
	result = System->Initialize();
	if (result)
	{
		Log::Get()->Debug("main(): the SystemClass object is successfully initialized");
		System->Run();
	}

	// Shutdown and release the system object
	_SHUTDOWN(System);
	
	return 0;
}
