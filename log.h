#pragma once

///////////////////////////////////////////////////////////////////////////////
// Filename: log.h
// there is a log system header
///////////////////////////////////////////////////////////////////////////////

class Log
{
public:
	Log(void);
	~Log(void);

	static Log* Get() { return m_instance; }

	void Debug(char* message, ...);
	void Error(char* message, ...);

private:
	static Log* m_instance;

	void m_init();
	void m_close();
	void m_print(char* levtext, char* text);

	FILE* m_file;
};