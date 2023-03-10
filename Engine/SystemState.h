////////////////////////////////////////////////////////////////////
// Filename:     SystemState.h
// Description:  contains the information about the current state of 
//               the engine and its parts; we use this information
//               for debug output (onto the screen), etc.;
// Revising:     25.11.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Engine/Log.h"
#include <DirectXMath.h>

// engine's state (information about the system, camera, player, etc.)
class SystemState
{
public:

	//void SetFps(int fps) { fps_ = fps; }
	//void SetCpu(int cpu) { cpu_ = cpu; }

public:
	int mouseX = 0;                          // the mouse cursor X position in the window
	int mouseY = 0;                          // the mouse cursor Y position in the window
	int fps = 0;                             // framerate
	int cpu = 0;                             // cpu performance
	DirectX::XMFLOAT3 editorCameraPosition;  // the current position of the engine editor's camera
	DirectX::XMFLOAT3 editorCameraRotation;  // the current rotation of the engine editor's camera
	int renderCount = 0;                     // the number of currently rendered models on the scene
	
};
