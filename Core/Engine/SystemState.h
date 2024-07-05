////////////////////////////////////////////////////////////////////////////////////////////
// Filename:     SystemState.h
// Description:  contains the information about the current state of 
//               the engine and its parts; we use this information
//               for camera control, for the debug output (onto the screen), etc.
// Revising:     25.11.22
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Engine/Log.h"
#include <DirectXMath.h>


class SystemState
{
public:
	SystemState();
	~SystemState();

	static SystemState* Get();

	bool intersect = false;                  // the flag to define if we clicked on some model or not
	int mouseX = 0;                          // the mouse cursor X position in the window
	int mouseY = 0;                          // the mouse cursor Y position in the window
	int fps = 0;                             // framerate
	int cpu = 0;                             // cpu performance
				
	UINT visibleVerticesCount = 0;          // the number of rendered vertices for this frame
	UINT visibleObjectsCount = 0;            // the number of rendered models for this frame
	UINT cellsDrawn = 0;                     // the number of rendered terrain cells
	UINT cellsCulled = 0;                    // the number of culled terrain cells

	float frameTime = 0.0f;                  // the seconds per frame
	DirectX::XMFLOAT3 editorCameraPos;       // the current position of the engine editor's camera
	DirectX::XMFLOAT3 editorCameraDir;       // the current rotation of the engine editor's camera

private:
	static SystemState* pInstance_;          // a ptr to the instance of this class
};
