////////////////////////////////////////////////////////////////////
// Filename: cpuclass.cpp
// Revising: 31.07.22
////////////////////////////////////////////////////////////////////
#include "cpuclass.h"
#include <pdhmsg.h>

CpuClass::CpuClass(void)
{
}

CpuClass::CpuClass(const CpuClass& copy)
{
}

CpuClass::~CpuClass(void)
{
	Log::Debug(LOG_MACRO);
}


// --------------------------------------------------------------------------------- //
//                                                                                   //
//                              PUBLIC FUNCTIONS                                     // 
//                                                                                   //
// --------------------------------------------------------------------------------- //


// will setup the handle for querying the cpu on its usage
void CpuClass::Initialize(void)
{
	return;
	Log::Get()->Debug(LOG_MACRO);

	PDH_STATUS status;  // performance data helper status
	HANDLE hPdhLibrary = NULL;
	LPWSTR pMessage = nullptr;
	DWORD wdErrorCode = PDH_PLA_ERROR_ALREADY_EXISTS;

	// initialize the flag indicating whether this object can read the system cpu usage or not
	m_canReadCpu = true;

	hPdhLibrary = LoadLibrary(L"pdh.dll");
	if (NULL == hPdhLibrary)
	{
		wprintf(L"LoadLibrary failed with %lu\n", GetLastError());
	}

	// create a query object to poll cpu usage
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)
	{
		Log::Get()->Error(LOG_MACRO, "can't create a query object to poll CPU usage");
		m_canReadCpu = false;
	}

	// Set query object to poll all cpus in the system
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 
		                   0, &m_counterHandle);
	if (status != ERROR_SUCCESS)
	{
		Log::Get()->Error(LOG_MACRO, "can't set query object to poll all CPUs in the system");


		if (!FormatMessage(FORMAT_MESSAGE_FROM_HMODULE |
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			hPdhLibrary,
			status,
			0,
			(LPWSTR)&pMessage,
			0,
			NULL))
		{
			Log::Error(LOG_MACRO, "Format message failed with " + std::to_string(GetLastError()));
		}

		Log::Error(LOG_MACRO, "Formatted message: " + StringHelper::ToString(pMessage));
		LocalFree(pMessage);
		m_canReadCpu = false;
	}
	else
	{
		Log::Error(LOG_MACRO, "can't set query object to poll all cpus in the system");
	}

	m_lastSampleTime = GetTickCount();


	return;
} // Initialize()

// releases the handle we used to query the cpu usage
void CpuClass::Shutdown(void)
{
	Log::Get()->Debug(LOG_MACRO);

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