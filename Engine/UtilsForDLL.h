////////////////////////////////////////////////////////////////////
// Filename:     UtilsForDLL.h
// Description:  contains different utils for work with
//               dynamic link libraries (DLL)
//
// Created:      01.08.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <windows.h>

#include "../Engine/log.h"
#include "../Engine/StringConverter.h"


//////////////////////////////////
// TYPEDEFS
//////////////////////////////////
typedef VOID(*DLLPROC) (const char*);

//////////////////////////////////
// Class name: UtilsForDLL
//////////////////////////////////
class UtilsForDLL
{
public:
	UtilsForDLL(const wchar_t* dllName);
	~UtilsForDLL();

	DLLPROC GetProcAddrFromDLL(const char* funcName);

private:
	HINSTANCE hinstDLL = NULL;
	DLLPROC procAddr = nullptr;
};
