#pragma once

///////////////////////////////////////////////////////////////////////////////
// Filename: log.h
// there is a log system header
///////////////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string>

#include "macros.h"

class Log
{
public:
	Log(void);
	~Log(void);

	static Log* Get() { return m_instance; }

	void Print(char* message, ...);
	void Debug(char* message, ...);
	void Error(char* message, ...);

private:
	static Log* m_instance;

	void m_init();
	void m_close();
	void m_print(char* levtext, char* text);

	HANDLE handle;
	FILE* m_file;
};