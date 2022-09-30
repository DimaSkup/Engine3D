////////////////////////////////////////////////////////////////////
// Filemane: timerclass.cpp
// Revising: 01.08.22
////////////////////////////////////////////////////////////////////
#include "timerclass.h"

TimerClass::TimerClass(void)
{

}

// we don't use the copy constructor and destructor in this class
TimerClass::TimerClass(const TimerClass& copy) {}
TimerClass::~TimerClass(void) {}



// --------------------------------------------------------------------------------- //
//                                                                                   //
//                              PUBLIC FUNCTIONS                                     // 
//                                                                                   //
// --------------------------------------------------------------------------------- //

bool TimerClass::Initialize(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	// Check to see if this system supports high performance timers
	void* ptrFrequency = &m_frequency;
	QueryPerformanceFrequency(static_cast<LARGE_INTEGER*>(ptrFrequency));
	if (m_frequency == 0)
	{
		Log::Get()->Error(THIS_FUNC, "there is no high performance timers in the system");
		return false;
	}

	// find out how many times the frequency counter ticks every millisecond
	m_ticksPerMs = static_cast<float>(m_frequency / 1000);

	void* ptrStartTime = &m_startTime;
	QueryPerformanceFrequency(static_cast<LARGE_INTEGER*>(ptrStartTime));

	ptrFrequency = nullptr;
	ptrStartTime = nullptr;

	return true;
} // Initialize()


// This function is called for every single loop of execution by the main program.
// This way we can calculate the difference of time between loops and determine the time
// it took to execute this frame.
void TimerClass::Frame(void)
{
	INT64 currentTime;
	float timeDifference;
	void* ptrCurrentTime = &currentTime;

	QueryPerformanceCounter(static_cast<LARGE_INTEGER*>(ptrCurrentTime));

	timeDifference = static_cast<float>(currentTime - m_startTime);
	m_frameTime = timeDifference / m_ticksPerMs;
	m_startTime = currentTime;

	return;
}

// returns the most recent frame time that was calculated
float TimerClass::GetTime(void)
{
	return m_frameTime;
}