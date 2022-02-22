///////////////////////////////////////////////////////////////////////////////
// Filename: log.cpp
// There is a log system source file
///////////////////////////////////////////////////////////////////////////////

#include "systemclass.h"
#include "log.h"


Log* m_instance = nullptr;

Log::Log(void)
{
	if (!m_instance)
	{
		m_instance = this;
		m_file = nullptr;
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
	fclose(m_file);
	m_instance = nullptr;
	
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

		fprintf(m_file, "%d : %d: the log file is created\n", time, date);
		fprintf(m_file, "-------------------------------------------\n");
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

	fprintf(m_file, "-------------------------------------------\n");
	fprintf(m_file, "%d : %d: the end of the log file is created\n", time, date);
}

void Log::Debug(char* message, ...)
{
	va_list args;
	int len = 0;
	char* buffer = nullptr;
	
	va_start(args, message);

	len = _vscprintf(message, args) + 1; // together with '/0'
	buffer = new(std::nothrow) char[len];

	if (buffer)
	{
		vsprintf_s(buffer, len, message, args);
		m_print(buffer);
	}

	delete buffer;
	buffer = nullptr;

	va_end(args);
}