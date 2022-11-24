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
	int fps_ = 0;  // framerate
	int cpu_ = 0;  // cpu performance
	DirectX::XMFLOAT3 editorPosition_; // the current position of the engine editor's camera
	DirectX::XMFLOAT2 editorRotation_; // the current rotation of the engine editor's camera
};
