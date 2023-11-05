////////////////////////////////////////////////////////////////////
// Filename:     UtilsForDLL.cpp
// Description:  contains different utils for work with
//               dynamic link libraries (DLL)
//
// Created:      01.08.23
////////////////////////////////////////////////////////////////////
#include "UtilsForDLL.h"


// using a DLL's name we try to load this library
UtilsForDLL::UtilsForDLL(const wchar_t* dllName)
{
	hinstDLL = LoadLibrary(dllName);

	if (hinstDLL == NULL)
	{
		std::string errorMsg{ "there is no DLL library: " + StringConverter::ToString(dllName) };
		Log::Error(THIS_FUNC, errorMsg.c_str());
		COM_ERROR_IF_FALSE(false, "can't load the DLL library");
	}
}

UtilsForDLL::~UtilsForDLL()
{
	BOOL fFreeDLLResult = FreeLibrary(hinstDLL);
}



////////////////////////////////////////////////////////////////////
//
//                         PUBLIC FUNCTIONS
//
////////////////////////////////////////////////////////////////////

// get an address of the DLL's process
DLLPROC UtilsForDLL::GetProcAddrFromDLL(const char* funcName)
{
	DLLPROC procAddr = (DLLPROC)GetProcAddress(hinstDLL, funcName);

	// if we didn't manage to get an address of the process
	if (procAddr == NULL)
	{
		std::string errorMsg{ "unable to call the DLL function: " + std::string(funcName) };
		Log::Error(THIS_FUNC, errorMsg.c_str());

		return NULL;
	}

	return procAddr;
}

