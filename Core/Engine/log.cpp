///////////////////////////////////////////////////////////////////////////////
// Filename: Log.cpp
// There is a Log system source file
///////////////////////////////////////////////////////////////////////////////
#include "Log.h"
#include <source_location>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <sstream>


using namespace std;

Log* Log::pInstance_ = nullptr;
HANDLE Log::handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
FILE* Log::pFile_ = nullptr;


///////////////////////////////////////////////////////////

Log::Log()
{
	if (!pInstance_) // we can have only one instance of Logger
	{
		if (!InitHelper())
		{
			SetConsoleTextAttribute(Log::handle_, ConsoleColor::RED);
			printf("Log::Log(): can't initialize the logger");
			SetConsoleTextAttribute(Log::handle_, ConsoleColor::WHITE);
		}

		pInstance_ = this;
		
		printf("Log::Log(): the Log system is created successfully\n");
	}
	else
	{
		printf("Log::Log(): there is already one instance of the ECS::Log\n");
	}
}

///////////////////////////////////////////////////////////

Log::~Log()
{
	if (!pFile_) return;

	CloseHelper();
	fflush(pFile_);
	fclose(pFile_);

	printf("Log::~Log(): the Log system is destroyed\n");
}


// ************************************************************************************
// 
//                             LOG PRINT METHODS
// 
// ************************************************************************************

void Log::Print(const std::string& message, ConsoleColor attr)
{
	// prints a usual message and setup it wit passed particular console text attribute

	SetConsoleTextAttribute(Log::handle_, attr);
	PrintHelper(" ", message.c_str());
	SetConsoleTextAttribute(Log::handle_, ConsoleColor::WHITE);  // reset
}

///////////////////////////////////////////////////////////

void Log::Print()
{
	// print empty string
	PrintHelper("", "");
}

///////////////////////////////////////////////////////////

void Log::Print(const std::string& msg, const std::source_location& location)
{
	// prints a usual message and the source location params as well

	stringstream ss;
	ss << location.function_name() << "() (line:" << location.line() << "): " << msg;

	SetConsoleTextAttribute(Log::handle_, ConsoleColor::GREEN);
	PrintHelper("", ss.str().c_str());
	SetConsoleTextAttribute(Log::handle_, ConsoleColor::WHITE);
}


// ************************************************************************************
//  
//                             LOG DEBUG METHODS
// 
// ************************************************************************************

void Log::Debug(const std::source_location& location)
{
	stringstream ss;
	ss << location.function_name() << "() (line:" << location.line() << ")";

	PrintHelper("DEBUG: ", ss.str().c_str());
}

///////////////////////////////////////////////////////////

void Log::Debug(const std::string& msg, const std::source_location& location)
{
	// prints a debug message
	stringstream ss;
	ss << location.function_name() << "() (line:" << location.line() << "): " << msg;

	Log::PrintHelper("DEBUG: ", ss.str().c_str());
}


// ************************************************************************************
// 
//                              LOG ERROR METHODS
// 
// ************************************************************************************


void Log::Error(EngineException* pException, bool showMsgBox)
{
	// LIB_Exception ERROR PRINTING (takes a pointer to the LIB_Exception)
	PrintExceptionErrHelper(*pException, showMsgBox);
}

///////////////////////////////////////////////////////////

void Log::Error(EngineException& e, bool showMsgBox)
{
	// LIB_Exception ERROR PRINTING (takes a reference to the LIB_Exception)
	PrintExceptionErrHelper(e, showMsgBox);
}

///////////////////////////////////////////////////////////

void Log::Error(const std::string& msg, const std::source_location& location)
{
	stringstream ss;
	ss << location.function_name() << "() (line:" << location.line() << "): " << msg;

	SetConsoleTextAttribute(Log::handle_, ConsoleColor::RED);  
	PrintHelper("ERROR: ", ss.str().c_str());
	SetConsoleTextAttribute(Log::handle_, ConsoleColor::WHITE);
}


// ************************************************************************************
// 
//                         PRIVATE METHODS (HELPERS)
// 
// ************************************************************************************


void Log::PrintExceptionErrHelper(EngineException& e, bool showMsgBox)
{
	// a common handler for LIB_Exception errors printing

	const std::wstring errorMsg = e.getStr();

	if (showMsgBox) MessageBoxW(NULL, errorMsg.c_str(), L"Error", MB_ICONERROR);

	SetConsoleTextAttribute(Log::handle_, ConsoleColor::RED);
	PrintHelper("ERROR: ", StringHelper::ToString(errorMsg).c_str());
	SetConsoleTextAttribute(Log::handle_, ConsoleColor::WHITE);
}

///////////////////////////////////////////////////////////

bool Log::InitHelper()
{
	//
	// this function creates and opens a Logger text file
	//

	if (fopen_s(&pFile_, "ECS_Log.txt", "w") == 0)
	{
		printf("Log::m_init(): the Log file is created successfully\n");

		char time[9];
		char date[9];

		_strtime_s(time, 9);
		_strdate_s(date, 9);

		fprintf(pFile_, "%s : %s| the Log file is created\n", time, date);
		fprintf(pFile_, "-------------------------------------------\n\n");
		return true;
	}
	else
	{
		printf("Log::m_init(): can't create the Log file\n");
		return false;
	}
}

///////////////////////////////////////////////////////////

void Log::CloseHelper()
{
	// print message about closing of the Logger file

	char time[9];
	char date[9];

	_strtime_s(time, 9);
	_strdate_s(date, 9);

	fprintf(pFile_, "\n-------------------------------------------\n");
	fprintf(pFile_, "%s : %s| the end of the Log file\n", time, date);
}

///////////////////////////////////////////////////////////

void Log::PrintHelper(const char* levtext, const char* text)
{
	// a helper for printing messages into the command prompt
	// and into the Logger text file
	char time[9];
	_strtime_s(time, 9);

	printf("%s::%d|\t%s%s\n", time, clock(), levtext, text);

	if (pFile_)
		fprintf(pFile_, "%s::%d|\t%s %s\n", time, clock(), levtext, text);
}
