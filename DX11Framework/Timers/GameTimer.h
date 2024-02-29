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
class GameTimer
{
public:
	GameTimer();

	float GetGameTime() const;   // in seconds
	float GetDeltaTime() const;  // in seconds

	void Reset();  // call before message loop
	void Start();  // call when unpaused
	void Stop();   // call when paused
	void Tick();   // call every frame

private:
	bool isStopped_ = false;

	double secondsPerCount_ = 0.0f;
	double deltaTime_ = -1.0f;

	__int64 baseTime_ = 0;
	__int64 prevTime_ = 0;
	__int64 stopTime_ = 0;
	__int64 pausedTime_ = 0;
	__int64 currTime_ = 0;
};
