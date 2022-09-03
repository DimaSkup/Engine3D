///////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
///////////////////////////////////////////////////////////////////////////////

#include "systemclass.h"

int main()
{
	
	SystemClass* System;
	SystemClassDesc sysDesc;
	bool result;


	// Create the system object
	sysDesc.fullScreen = true;
	//sysDesc.width = 800;
	//sysDesc.height = 600;
	sysDesc.width = 1600;
	sysDesc.height = 900;
	System = new(std::nothrow) SystemClass(sysDesc);
	if (!System)
	{
		Log::Get()->Error("main(): can't allocate memory for the SystemClass object");
		return 0;
	}

	

	// Initialize and run the system object
	result = System->Initialize();
	if (result)
	{
		Log::Get()->Debug(THIS_FUNC, "the SystemClass object is successfully initialized");
		System->Run();
	}

	// Shutdown and release the system object
	_SHUTDOWN(System);
	
	return 0;
}
