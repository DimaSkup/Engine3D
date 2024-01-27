///////////////////////////////////////////////////////////////////////////////
// Filename:    Log.h
// Description: there is a log system header
///////////////////////////////////////////////////////////////////////////////
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string>
#include <cassert>

#include "../Engine/macros.h"
#include "../Engine/COMException.h"



class Log
{
public:
	Log(void);
	~Log(void);

	static Log* Get(); // to get a static pointer to this class instance

	static void Print(char* message, ...); // print a usual message
	static void Debug(char* message, ...); // pring a debug message
	static void Error(char* message, ...); // print a message about some error

	static void Error(char* funcNameAndLine, const std::string & message);
	static void Debug(char* funcNameAndLine, const std::string & message);

	static void Error(COMException* exception, bool showMessageBox = false);
	static void Error(COMException& exception, bool showMessageBox = false);

	static HANDLE handle;  // we need it for changing the text colour in the command prompt
	static FILE* m_file;   // a pointer to the logger file handler

private:
	static void printError(COMException& exception, bool showMessageBox);  // a common handler for error printing

	void m_init();  // make and open a logger text file
	void m_close(); // print message about closing of the logger file
	static void m_print(char* levtext, const char* text);  // a helper for printing messages into the command prompt and into the logger text file

private:
	static Log* m_instance;
};