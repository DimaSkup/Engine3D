////////////////////////////////////////////////////////////////////
// Filename:     SystemState.h
// Description:  contains the information about the current state of 
//               the engine and its parts; we use this information
//               for the debug output (onto the screen), etc.;
// Revising:     25.11.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Engine/Log.h"
#include <DirectXMath.h>

// engine's state (information about the system, camera, player, etc.)
struct SystemState
{
	int mouseX = 0;                          // the mouse cursor X position in the window
	int mouseY = 0;                          // the mouse cursor Y position in the window
	int fps = 0;                             // framerate
	int cpu = 0;                             // cpu performance
	DirectX::XMFLOAT3 editorCameraPosition;  // the current position of the engine editor's camera
	DirectX::XMFLOAT3 editorCameraRotation;  // the current rotation of the engine editor's camera
	bool intersect = false;                  // the flag to define if we clicked on some model or not
											 
	int renderCount = 0;                     // the number of currently rendered models on the scene
	int cellsDrawn = 0;                      // the number of rendered terrain cells
	int cellsCulled = 0;                     // the number of culled terrain cells
};
