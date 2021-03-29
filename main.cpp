//////////////////////////////////
// Filename: main.cpp
//////////////////////////////////

//////////////////////////////////
// INCLUDE
//////////////////////////////////
#include <iostream>
#include <fstream>

//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
typedef struct
{
	float x, y, z;
} VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
} FaceType;

//////////////////////////////////
// FUNCTION PROTOTYPES
//////////////////////////////////


#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass *System;
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
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}