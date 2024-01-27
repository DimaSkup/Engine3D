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



// make and open a logger text file
void Log::m_init(void)
{
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


// print message about closing of the logger file
void Log::m_close(void)
{
	char time[9];
	char date[9];

	_strtime_s(time, 9);
	_strdate_s(date, 9);

	fprintf(m_file, "\n-------------------------------------------\n");
	fprintf(m_file, "%s : %s| the end of the log file\n", time, date);
}


// prints a usual message
void Log::Print(char* message, ...)
{
	va_list args;
	int len = 0;
	char* buffer = nullptr;

	va_start(args, message);

	len = _vscprintf(message, args) + 1;	// +1 together with '/0'
	try
	{
		buffer = new char[len];

		vsprintf_s(buffer, len, message, args);


		SetConsoleTextAttribute(Log::handle, 0x000A);
		Log::m_print("", buffer);
		SetConsoleTextAttribute(Log::handle, 0x0007);
	}
	catch (std::bad_alloc & e)
	{
		printf("Log::Print(): ERROR: %s", e.what());
		printf("Log::Print(): can't allocate memory for the buffer");
		va_end(args);

		return;
	}

	_DELETE_ARR(buffer);

	va_end(args);
}


// prints a debug message
void Log::Debug(char* message, ...)
{
#ifdef _DEBUG
	va_list args;
	int len = 0;
	char* buffer = nullptr;

	va_start(args, message);

	len = _vscprintf(message, args) + 1; // +1 together with '/0'

	try
	{
		buffer = new char[len];
	}
	catch (std::bad_alloc & e)
	{
		printf("Log::Debug(): ERROR: %s", e.what());
		printf("Log::Debug(): can't allocate memory for the buffer");
		va_end(args);
		return;
	}
	

	vsprintf_s(buffer, len, message, args);
	Log::m_print("DEBUG: ", buffer);
	
	_DELETE_ARR(buffer);

	va_end(args);
#endif
}


// prints an error message
void Log::Error(char* message, ...)
{
	va_list args;
	int len = 0;
	char* buffer = nullptr;
	SetConsoleTextAttribute(Log::handle, 0x0004);  // set console text color to red

	va_start(args, message);

	len = _vscprintf(message, args) + 1;	// +1 together with '/0'

	try
	{
		buffer = new char[len];
	}
	catch (std::bad_alloc & e)
	{
		printf("Log::Error(): ERROR: %s", e.what());
		printf("Log::Error(): can't allocate memory for the buffer");
		va_end(args);
		return;
	}


	vsprintf_s(buffer, len, message, args);

	Log::m_print("ERROR: ", buffer);
	SetConsoleTextAttribute(Log::handle, 0x0007);


	_DELETE_ARR(buffer);

	va_end(args);
}

void Log::Debug(char* funcNameAndLine, const std::string & message)
{
	std::string debugMsg{ funcNameAndLine + message };
	Log::m_print("DEBUG: ", debugMsg.c_str());

	return;
}


void Log::Error(char* funcNameAndLine, const std::string & message)
{
	std::string errorMsg{ funcNameAndLine + message };

	SetConsoleTextAttribute(Log::handle, 0x0004);  // set console text color to red
	Log::m_print("ERROR: ", errorMsg.c_str());     // print the error message
	SetConsoleTextAttribute(Log::handle, 0x0007);  // set console texture color back to white
}



// EXCEPTION ERROR PRINTING (takes a pointer to the exception)
void Log::Error(COMException* exception, bool showMessageBox)
{
	Log::printError(*exception, showMessageBox);
}


// EXCEPTION ERROR PRINTING (takes a reference to the exception)
void Log::Error(COMException & exception, bool showMessageBox)
{
	Log::printError(exception, showMessageBox);
}


// a common handler for exception errors printing
void Log::printError(COMException & exception, bool showMessageBox)
{
	std::wstring errorMsg = exception.getStr();

	if (showMessageBox)
		MessageBoxW(NULL, errorMsg.c_str(), L"Error", MB_ICONERROR);

	std::string consoleErrorMsg = StringHelper::ToString(errorMsg);
	Log::Error(THIS_FUNC, consoleErrorMsg.c_str());
}


// a helper for printing messages into the command prompt and into the logger text file
void Log::m_print(char* levtext, const char* text)
{
	clock_t cl = clock();
	char time[9];

	_strtime_s(time, 9);
	printf("%s::%d|\t%s%s\n", time, cl, levtext, text);

	if (m_file)
	{
		fprintf(m_file, "%s::%d|\t%s %s\n", time, cl, levtext, text);
	}
}