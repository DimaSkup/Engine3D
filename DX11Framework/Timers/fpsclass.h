////////////////////////////////////////////////////////////////////
// Filename:     fpsclass.h
// Description:  the FpsClass is simply a counter with a timer 
//               associated with it. It counts how many frames 
//               occur in a one second period and constantly 
//               updates that count.
// Created:      31.07.22
// Revising:     23.12.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// LINKING
//////////////////////////////////
#pragma comment(lib, "winmm.lib")

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <windows.h>
#include <mmsystem.h>

#include "../Engine/Log.h"

//////////////////////////////////
// Class name: FpsClass
//////////////////////////////////
class FpsClass
{
public:
	FpsClass();
	~FpsClass();

	void Initialize(void);
	void Update(int & gameCycles);
	int  GetFps(void) const;

private:
	int fps_ = 0;       // frame counter during the current second (1000 ms)
	ULONG startTime_;   // time when the current frame started
};
