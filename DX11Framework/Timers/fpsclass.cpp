////////////////////////////////////////////////////////////////////
// Filename:     fpsclass.cpp
// Created:      31.07.22
// Revising:     23.12.22
////////////////////////////////////////////////////////////////////
#include "fpsclass.h"



FpsClass::FpsClass()
{
}

FpsClass::~FpsClass()
{
}



// --------------------------------------------------------------------------------- //
//                                                                                   //
//                              PUBLIC FUNCTIONS                                     // 
//                                                                                   //
// --------------------------------------------------------------------------------- //

// sets all the counter to zero and starts the timer
void FpsClass::Initialize(void)
{
	Log::Get()->Debug(LOG_MACRO);

	fps_ = 0;
	startTime_ = timeGetTime();

	return;
}

// this function must be called each frame so that it can increment the frame count by 1.
// If it finds that one second has elapsed then it will store the frame count 
// in the m_fps variable. It then resets the count and starts the timer again
void FpsClass::Update(int & gameCycles_)
{
	gameCycles_++;

	if (timeGetTime() >= (startTime_ + 1000))
	{
		fps_ = gameCycles_;
		gameCycles_ = 0;
		startTime_ = timeGetTime();   // start time of the new frame
	}
}


// returns the frame per second speed for the last second that just passed.
// This function should be constantly queried so the lates fps can be displayed to the screen.
int FpsClass::GetFps() const 
{
	return fps_;
}