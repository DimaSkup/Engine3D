///////////////////////////////////////////////////////////////////////////////
// Filename:    ECS::Common/log.h
// Description: there is a ECS::Log system header
///////////////////////////////////////////////////////////////////////////////
#pragma once


#define LOG_MACRO    __FUNCTION__, __LINE__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string>
#include <cassert>

#include "LIB_Exception.h"


namespace ImgReader
{

class Log
{
public:
	Log();
	~Log();

	static Log* Get(); // to get a static pointer to this class instance

	static void Print(const char* message);                       // for specific using
	static void Debug(const char* message);                       // for specific using
	static void Print(const std::string& message);
	static void Debug(const std::string& message);
	static void Debug(const char* funcName, const int codeLine);  // for empty messages

	static void Print(const char* funcName, const int codeLine, const std::string & message);
	static void Debug(const char* funcName, const int codeLine, const std::string & message);
	static void Error(const char* funcName, const int codeLine, const std::string & message);
	static void Error(const std::string& msg) {};

	static void Print(const char* funcName, const int codeLine, const char* message); // print a usual message
	static void Debug(const char* funcName, const int codeLine, const char* message); // pring a debug message
	static void Error(const char* funcName, const int codeLine, const char* message); // print a message about some error

	static void Error(LIB_Exception* exception, bool showMessageBox = false);
	static void Error(LIB_Exception& exception, bool showMessageBox = false);

	static HANDLE handle;  // we need it for changing the text colour in the command prompt
	static FILE* m_file;   // a pointer to the ECS::Logger file handler

private:
	static void printError(LIB_Exception& exception, bool showMessageBox);  // a Common handler for error printing

	void m_init();  // make and open a ECS::Logger text file
	void m_close(); // print message about closing of the ECS::Logger file
	static void m_print(const char* levtext, const char* text);  // a helper for printing messages into the command prompt and into the ECS::Logger text file

private:
	static Log* m_instance;

}; // log


}; // namespace ECS