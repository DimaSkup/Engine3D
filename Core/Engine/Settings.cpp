////////////////////////////////////////////////////////////////////
// Filename:    Settings.cpp
// Description: contains settings for the engine; uses a singleton pattern
//
// Revising:    27.11.22
////////////////////////////////////////////////////////////////////
#include "Settings.h"
#include "../Common/Assert.h"

#include <fstream>
#include <sstream>



Settings::Settings()
{
	LoadSettingsFromFile();
}

Settings::~Settings()
{
	settingsList_.clear();
}

///////////////////////////////////////////////////////////

// load engine settings from the settings file
void Settings::LoadSettingsFromFile()
{
	// try to open the settings file
	std::fstream fin("data/settings.txt");
	Assert::True(fin.is_open(), "can't open the settings file");

	std::string key;
	std::string value;

	// read all the pairs [setting_key => setting_value] from the file
	while (!fin.eof())
	{
		fin >> key;
		fin >> value;

		// try to insert a pair [key => value]; if we didn't manage to do it we throw an exception
		if (!settingsList_.insert({ key, value }).second) 
		{
			throw EngineException("can't insert a pair [key=>value] into "
				                  "the settings list: "
				                  "[key: " + key + " => value: " + value);
		}
	}
}

///////////////////////////////////////////////////////////

const int Settings::GetInt(const char* key)
{
	// get an integer setting parameter by the input key

	try
	{
		std::string& val = settingsList_.at(key);
		std::istringstream iCharStream(val);
		int intVal = 0;

		if ((iCharStream >> intVal).fail())
		{
			throw EngineException("can't convert value from string into integer; by key: " + std::string(key));
		}

		return intVal;
	}
	catch (std::out_of_range& e)
	{
		Log::Error(e.what());
		throw EngineException("can't find an integer by key: " + std::string(key));
	}
}

///////////////////////////////////////////////////////////

const float Settings::GetFloat(const char* key)
{
	// get a float setting parameter by the input key

	try
	{
		const std::string& val = settingsList_.at(key);
		std::istringstream iCharStream(val);
		float floatVal = 0;

		// try to convert from string to float
		if ((iCharStream >> floatVal).fail())
		{
			throw EngineException("can't convert value from string into float; by key: " + std::string(key));
		}

		return floatVal;
	}
	catch (std::out_of_range& e)
	{
		Log::Error(e.what());
		throw EngineException("can't find a float by key: " + std::string(key));
	}
}

///////////////////////////////////////////////////////////

const bool Settings::GetBool(const char* key)
{
	// get a boolean setting parameter by the input key

	try
	{
		const std::string& val = settingsList_.at(key);

		if (val == "true")
		{
			return true;
		}
		else if (val == "false")
		{
			return false;
		}
		else
		{
			throw EngineException("can't convert value from string into bool by key: " + std::string(key));
		}
	}
	catch (std::out_of_range& e)
	{
		Log::Error(e.what());
		throw EngineException("can't find a boolean by key: " + std::string(key));
	}
}

///////////////////////////////////////////////////////////

const std::string& Settings::GetString(const char* key)
{
	// get a string setting parameter by the input key

	try
	{
		const std::string& str = settingsList_.at(key);
		Assert::NotEmpty(str.empty(), "the setting value by key (" + std::string(key) + ") is empty");

		return str;
	}
	catch (std::out_of_range& e)
	{
		Log::Error(e.what());
		throw EngineException("can't find a string by key: " + std::string(key));
	}
}

///////////////////////////////////////////////////////////

void Settings::UpdateSettingByKey(const char* key, const std::string& val)
{
	try 
	{
		settingsList_.at(key) = val;
	}
	catch (std::out_of_range& e)
	{
		Log::Error(e.what());
		throw EngineException("can't update a setting by key: " + std::string(key));
	}
}
