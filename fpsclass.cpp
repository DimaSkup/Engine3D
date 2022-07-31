////////////////////////////////////////////////////////////////////
// Filename: fpsclass.cpp
////////////////////////////////////////////////////////////////////
#include "fpsclass.h"

FpsClass::FpsClass(void)
{
}

// we don't use the copy constructor and destructor in this class
FpsClass::FpsClass(const FpsClass& copy) {}
FpsClass::~FpsClass(void) {}



// --------------------------------------------------------------------------------- //
//                                                                                   //
//                              PUBLIC FUNCTIONS                                     // 
//                                                                                   //
// --------------------------------------------------------------------------------- //

// sets all the counter to zero and starts the timer
void FpsClass::Initialize(void)
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();

	return;
}

// this function must be called each frame so that it can increment the frame count by 1.
// If it finds that one second has elapsed then it will store the frame count 
// in the m_fps variable. It then resets the count and starts the timer again
void FpsClass::Frame(void)
{
	++m_count;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}


// returns the frame per second speed for the last second that just passed.
// This function should be constantly queried so the lates fps can be displayed to the screen.
int FpsClass::GetFps(void)
{
	return m_fps;
}