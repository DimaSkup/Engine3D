////////////////////////////////////////////////////////////////////
// Filename:     cpuclass.h
// Description:  this class is used to determine the percentage 
//               of total cpu use that is occuring each second
// Revising:     31.07.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// LINKING
//////////////////////////////////
#pragma comment(lib, "pdh.lib")


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <pdh.h>

#include "../Engine/Log.h"


//////////////////////////////////
// Class name: CpuClass
//////////////////////////////////
class CpuClass
{
public:
	CpuClass(void);
	CpuClass(const CpuClass&);
	~CpuClass(void);

	void Initialize(void);
	void Shutdown(void);
	void Frame(void);
	int GetCpuPercentage(void);

private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	ULONG m_lastSampleTime;
	LONG m_cpuUsage;
};