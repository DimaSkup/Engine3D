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

void Log::m_close(void)
{
	char time[9];
	char date[9];

	_strtime_s(time, 9);
	_strdate_s(date, 9);

	fprintf(m_file, "\n-------------------------------------------\n");
	fprintf(m_file, "%s : %s| the end of the log file\n", time, date);
}

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
		printf("Log::Print(): can't allocate memory for the buffer");
		va_end(args);

		return;
	}

	_DELETE(buffer);

	va_end(args);
}


void Log::Debug(char* message, ...)
{
#ifdef _DEBUG
	va_list args;
	int len = 0;
	char* buffer = nullptr;

	va_start(args, message);

	len = _vscprintf(message, args) + 1; // +1 together with '/0'
	buffer = new(std::nothrow) char[len];
	if (buffer)
	{
		vsprintf_s(buffer, len, message, args);
		Log::m_print("DEBUG: ", buffer);
	}
	_DELETE(buffer);

	va_end(args);
#endif
}

void Log::Error(char* message, ...)
{
	va_list args;
	int len = 0;
	char* buffer = nullptr;

	va_start(args, message);

	len = _vscprintf(message, args) + 1;	// +1 together with '/0'
	buffer = new(std::nothrow) char[len];

	if (buffer)
	{
		vsprintf_s(buffer, len, message, args);


		SetConsoleTextAttribute(Log::handle, 0x0004);
		Log::m_print("ERROR: ", buffer);
		SetConsoleTextAttribute(Log::handle, 0x0007);
	}

	_DELETE(buffer);

	va_end(args);
}


// ERROR PRINTING (takes a pointer to the exception)
void Log::Error(COMException* exception, bool showMessageBox)
{
	Log::printError(*exception, showMessageBox);
}


// ERROR PRINTING (takes a reference to the exception)
void Log::Error(COMException & exception, bool showMessageBox)
{
	Log::printError(exception, showMessageBox);
}


// a common handler for error printing
void Log::printError(COMException & exception, bool showMessageBox)
{
	std::wstring errorMsg = exception.getStr();

	if (showMessageBox)
		MessageBoxW(NULL, errorMsg.c_str(), L"Error", MB_ICONERROR);

	std::string consoleErrorMsg = StringConverter::ToString(errorMsg);
	Log::Error(THIS_FUNC, consoleErrorMsg.c_str());
}



void Log::m_print(char* levtext, char* text)
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