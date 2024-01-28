///////////////////////////////////////////////////////////////////////////////
// Filename: log.cpp
// There is a log system source file
///////////////////////////////////////////////////////////////////////////////
#include "Log.h"


Log* Log::m_instance = nullptr;
HANDLE Log::handle = GetStdHandle(STD_OUTPUT_HANDLE);
FILE* Log::m_file = nullptr;

Log::Log(void)
{
	if (!m_instance) // we can have only one instance of logger
	{
		m_instance = this;
		m_init();
		
		printf("Log::Log(): the log system is created successfully\n");
	}
	else
	{
		printf("Log::Log(): there is already exists one instance of the log system\n");
	}
}

Log::~Log(void)
{
	if (!m_file)
		return;

	m_close();
	fflush(m_file);
	fclose(m_file);

	printf("Log::~Log(): the log system is destroyed\n");
}


// returns a pointer to the instance of the Log class
Log* Log::Get() { return m_instance; }


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void Log::Print(const char* message)
{
	// prints a usual message

	SetConsoleTextAttribute(Log::handle, 0x000A);   // set green
	Log::m_print("", message);
	SetConsoleTextAttribute(Log::handle, 0x0007);   // set white
}

void Log::Debug(const char* message)
{
	// prints a debug message

#if _DEBUG
	Log::m_print("", message);
#endif
}

void Log::Debug(const char* funcName, const int codeLine)
{
	// prints an empty debug message
#if _DEBUG
	const std::string msgForDebug{ (std::string)funcName + "() (" + std::to_string(codeLine) + ")"};
	Log::m_print("DEBUG: ", msgForDebug.c_str());
#endif

	return;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void Log::Print(const char* funcName, const int codeLine, const std::string & message)
{
	// prints a usual message

	const std::string msgForPrint{ (std::string)funcName + "() (" + std::to_string(codeLine) + "): " + message };

	SetConsoleTextAttribute(Log::handle, 0x000A);   // set green
	Log::m_print("", msgForPrint.c_str());
	SetConsoleTextAttribute(Log::handle, 0x0007);   // set white

	return;
}

void Log::Debug(const char* funcName, const int codeLine, const std::string & message)
{
	// prints a debug message
#if _DEBUG
	const std::string msgForDebug{ (std::string)funcName + "() (" + std::to_string(codeLine) + "): " + message };
	Log::m_print("DEBUG: ", msgForDebug.c_str());
#endif

	return;
}

void Log::Error(const char* funcName, const int codeLine, const std::string & message)
{
	std::string errorMsg{ (std::string)funcName + "() (" + std::to_string(codeLine) + "): " + message };

	SetConsoleTextAttribute(Log::handle, 0x0004);  // set console text color to red
	Log::m_print("ERROR: ", errorMsg.c_str());     // print the error message
	SetConsoleTextAttribute(Log::handle, 0x0007);  // set console texture color back to white

	return;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void Log::Print(const char* funcName, const int codeLine, const char* message)
{
	// prints a usual message

	const std::string msgForPrint{ (std::string)funcName + "() (" + std::to_string(codeLine) + "): " + message };

	SetConsoleTextAttribute(Log::handle, 0x000A);   // set green
	Log::m_print("", msgForPrint.c_str());
	SetConsoleTextAttribute(Log::handle, 0x0007);   // set white

	return;
}

void Log::Debug(const char* funcName, const int codeLine, const char* message)
{
	// prints a debug message

#ifdef _DEBUG
	const std::string msgForDebug{ (std::string)funcName + "() (" + std::to_string(codeLine) + "): " + message };
	Log::m_print("DEBUG: ", msgForDebug.c_str());
#endif
}

void Log::Error(const char* funcName, const int codeLine, const char* message)
{
	// prints an error message

	std::string errorMsg{ (std::string)funcName + "() (" + std::to_string(codeLine) + "): " + message };

	SetConsoleTextAttribute(Log::handle, 0x0004);  // set console text color to red
	Log::m_print("ERROR: ", errorMsg.c_str());     // print the error message
	SetConsoleTextAttribute(Log::handle, 0x0007);  // set console texture color back to white

	return;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void Log::Error(COMException* exception, bool showMessageBox)
{
	// EXCEPTION ERROR PRINTING (takes a pointer to the exception)
	Log::printError(*exception, showMessageBox);
}

void Log::Error(COMException & exception, bool showMessageBox)
{
	// EXCEPTION ERROR PRINTING (takes a reference to the exception)
	Log::printError(exception, showMessageBox);
}




///////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 PRIVATE FUNCTIONS
//
///////////////////////////////////////////////////////////////////////////////////////////////

void Log::printError(COMException & exception, bool showMessageBox)
{
	// a common handler for exception errors printing

	const std::wstring errorMsg = exception.getStr();

	if (showMessageBox)
		MessageBoxW(NULL, errorMsg.c_str(), L"Error", MB_ICONERROR);

	Log::Error(LOG_MACRO, StringHelper::ToString(errorMsg));

	return;
}

///////////////////////////////////////////////////////////

void Log::m_init(void)
{
	//
	// this function creates and opens a logger text file
	//

	if (fopen_s(&m_file, "log.txt", "w") == 0)
	{
		printf("Log::m_init(): the log file is created successfully\n");

		char time[9];
		char date[9];

		_strtime_s(time, 9);
		_strdate_s(date, 9);

		fprintf(m_file, "%s : %s| the log file is created\n", time, date);
		fprintf(m_file, "-------------------------------------------\n\n");
	}
	else
	{
		printf("Log::m_init(): can't create the log file\n");
	}
}

///////////////////////////////////////////////////////////

void Log::m_close(void)
{
	// print message about closing of the logger file

	char time[9];
	char date[9];

	_strtime_s(time, 9);
	_strdate_s(date, 9);

	fprintf(m_file, "\n-------------------------------------------\n");
	fprintf(m_file, "%s : %s| the end of the log file\n", time, date);
}



// a helper for printing messages into the command prompt and into the logger text file
void Log::m_print(const char* levtext, const char* text)
{
	const clock_t cl = clock();

	char time[9];
	_strtime_s(time, 9);

	printf("%s::%d|\t%s%s\n", time, cl, levtext, text);

	if (m_file)
	{
		fprintf(m_file, "%s::%d|\t%s %s\n", time, cl, levtext, text);
	}
}