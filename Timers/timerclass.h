////////////////////////////////////////////////////////////////////
// Filename:     timerclass.h
// Description:  this class is a high precision timer that measures
//               the exact time between frames of execution. It's 
//               primary use if for synchronizing objects that 
//               require a standard time frame for movement.
// Revising:     01.08.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES 
//////////////////////////////////
#include <windows.h>

#include "../Engine/Log.h"

//////////////////////////////////
// Class name: TimerClass
//////////////////////////////////
class TimerClass 
{
public:
	TimerClass(void);
	TimerClass(const TimerClass& copy) {};
	~TimerClass(void) {};

	bool Initialize(void);
	void Frame(void);

	float GetTime(void);

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
};
