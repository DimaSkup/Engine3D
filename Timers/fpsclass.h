////////////////////////////////////////////////////////////////////
// Filename:     fpsclass.h
// Description:  the FpsClass is simply a counter with a timer 
//               associated with it. It counts how many frames 
//               occur in a one second period and constantly 
//               updates that count.
// Revising:     31.07.22
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

#include "../LogSystem/Log.h"

//////////////////////////////////
// Class name: FpsClass
//////////////////////////////////
class FpsClass
{
public:
	FpsClass(void);
	FpsClass(const FpsClass& copy);
	~FpsClass(void);

	void Initialize(void);
	void Frame(void);
	int GetFps(void);

private:
	int m_fps, m_count;
	ULONG m_startTime;
};
