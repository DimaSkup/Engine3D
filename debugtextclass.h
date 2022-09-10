////////////////////////////////////////////////////////////////////
// Filename:      debugtestclass.h
// Description:   this class is responsible for the output of debug
//                data (fps, cpu, etc.) onto the screen
//
// Revising:      10.09.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "textclass.h"
#include "log.h"


//////////////////////////////////
// Class name: DebugTextClass
//////////////////////////////////
class DebugTextClass : public TextClass
{
public:

	bool SetMousePosition(DirectX::XMFLOAT2 pos);   // set the mouse position data for rendering it on the screen
	bool SetDisplayParams(int width, int height);

	// set the fps and cpu data for rendering it on the screen
	bool SetFps(int fps);
	bool SetCpu(int cpu);
	bool SetCameraOrientation(DirectX::XMFLOAT2 orientation);

private:
	// 
	std::map<std::string, SentenceType*> sentences;
};
