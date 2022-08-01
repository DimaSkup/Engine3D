////////////////////////////////////////////////////////////////////
// Filename: cpuclass.cpp
// Revising: 31.07.22
////////////////////////////////////////////////////////////////////
#include "cpuclass.h"

CpuClass::CpuClass(void)
{

}

// we don't use the copy constructor and destructor in this class
CpuClass::CpuClass(const CpuClass& copy) {}
CpuClass::~CpuClass(void) {}


// --------------------------------------------------------------------------------- //
//                                                                                   //
//                              PUBLIC FUNCTIONS                                     // 
//                                                                                   //
// --------------------------------------------------------------------------------- //


// will setup the handle for querying the cpu on its usage
void CpuClass::Initialize(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	PDH_STATUS status;

	// initialize the flag indicating whether this object can read the system cpu usage or not
	m_canReadCpu = true;

	// create a query object to poll cpu usage
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)
	{
		Log::Get()->Error(THIS_FUNC, "can't create a query object to poll CPU usage");
		m_canReadCpu = false;
	}

	// Set query object to poll all cpus in the system
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 
		                   0, &m_counterHandle);
	if (status != ERROR_SUCCESS)
	{
		Log::Get()->Error(THIS_FUNC, "can't set query object to poll all CPUs in the system");
		m_canReadCpu = false;
	}

	m_lastSampleTime = GetTickCount();
	m_cpuUsage = 0;

	return;
} // Initialize()

// releases the handle we used to query the cpu usage
void CpuClass::Shutdown(void)
{
	Log::Get()->Debug(THIS_FUNC_EMPTY);

	if (m_canReadCpu)
	{
		PdhCloseQuery(m_queryHandle);
	}

	return;
}


// this function is called each frame;
// the function each second asks the cpu for its usage and save that value in m_cpuUsage
void CpuClass::Frame(void)
{
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu)
	{
		if ((m_lastSampleTime + 1000) < GetTickCount()) // if one second is passed
		{
			m_lastSampleTime = GetTickCount();
			PdhCollectQueryData(m_queryHandle);
			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);
			m_cpuUsage = value.longValue;
		}
	}

	return;
}


// returns the value of the current cpu usage to any calling function or 
// just returns zero value if it couldn't read the cpu for whatever reason
int CpuClass::GetCpuPercentage(void)
{
	if (m_canReadCpu)
	{
		return static_cast<int>(m_cpuUsage);
	}
	else
	{
		return 0;
	}
}