////////////////////////////////////////////////////////////////////
// Filename:    Settings.cpp
// Description: contains settings for the engine; uses a singleton pattern
//
// Revising:    27.11.22
////////////////////////////////////////////////////////////////////
#include "Settings.h"


// a pointer to the instance of the class
Settings* Settings::pInstance_ = nullptr;


// a private constructor which can be called only from the Get() function
Settings::Settings()
{
	LoadSettingsFromFile();
}


// get a pointer to the instance of the class
Settings* Settings::Get()
{
	if (pInstance_ == nullptr)
	{
		pInstance_ = new Settings;
	}

	return pInstance_;
}

void Settings::Shutdown()
{
	if (!settingsList_.empty())
	{
		settingsList_.clear();
	}

	pInstance_ = nullptr;
}


// load engine settings from the settings file
bool Settings::LoadSettingsFromFile()
{
	std::fstream fin;
	std::string key;
	std::string value;


	// try to open the settings file
	fin.open("data/settings.txt");

	if (fin.fail())
	{
		Log::Error(LOG_MACRO, "can't open the settings file");
		return false;
	}


	// read all the pairs [setting_key => setting_value] from the file
	while (!fin.eof())
	{
		fin >> key;
		fin >> value;

		// and record it into the settings list
		auto result = settingsList_.insert({ key, value });

		if (!result.second) // if we didn't record a pair
		{
			std::string errorMsg{ "can't insert a pair [key=>value] into the settings list: [key: " + key + " => value: " + value };
			COM_ERROR_IF_FALSE(false, errorMsg);
		}
	}
	

	return true;
}



// get an integer setting parameter by the input key
int Settings::GetSettingIntByKey(const char* key)
{
	// check if we have such a key
	auto iterator = CheckSettingKey(key);

	// convert const char* into integer
	std::istringstream iCharStream(iterator->second.c_str());
	int intVal = 0;

	if ((iCharStream >> intVal).fail())
	{
		std::string errorMsg{ "can't convert value from string into integer: " + iterator->second };
		COM_ERROR_IF_FALSE(false, errorMsg);
	}

	return intVal;
}


// get a float setting parameter by the input key
float Settings::GetSettingFloatByKey(const char* key)
{
	// check if we have such a key
	auto iterator = CheckSettingKey(key);
	

	// convert const char* into float
	std::istringstream iCharStream(iterator->second.c_str());
	float floatVal = 0.0f;

	if ((iCharStream >> floatVal).fail())
	{
		std::string errorMsg{ "can't convert value from string into float: " + iterator->second };
		COM_ERROR_IF_FALSE(false, errorMsg);
	}

	return floatVal;
}



// get a boolean setting parameter by the input key
bool Settings::GetSettingBoolByKey(const char* key)
{
	// check if we have such a key
	auto iterator = CheckSettingKey(key);

	// check if the input setting parameter is correct boolean value
	bool isCorrectBoolVal = (iterator->second == "true" || iterator->second == "false");
		
	if (!isCorrectBoolVal)
	{
		COM_ERROR_IF_FALSE(false, "can't convert value from string into bool: " + iterator->second);
	}

	return (iterator->second == "true");
}


// get a string setting parameter by the input key
std::string Settings::GetSettingStrByKey(const char* key)
{
	// check if we have such a key
	auto iterator = CheckSettingKey(key);

	// check if this value is correct
	COM_ERROR_IF_FALSE(!(iterator->second.empty()), "the setting value by key is empty");

	return iterator->second;
}


/*
	template<class T>
void Settings::GetSettingByKey(const std::string & key, T & dest)
{
	auto iterator = settingsList_.find(key);

	// check if we have such a key
	if (iterator == settingsList_.end())
	{
		std::string errorMsg{ "there is no such a key: " + key };
		Log::Error(LOG_MACRO, errorMsg.c_str());
	}
	

	if (typeid(dest) == typeid(float))
		dest = ::atof(iterator->second.c_str());
	else if (typeid(dest) == typeid(bool))
		dest = (iterator->second == "true");
	else if (typeid(dest) == typeid(int))
		dest = atoi(iterator->second.c_str());
	else if (typeid(dest) == typeid(std::string))
		dest = iterator->second;
	else
	{
		COM_ERROR_IF_FALSE(false, "wrong destination type");
	}
}

*/




void Settings::UpdateSettingByKey(const char* key, const std::string & src)
{
	// check if we have such a key
	Settings::CheckSettingKey(key);

	//Settings::settingsList_.erase(key);

	// update a setting value
	//Settings::settingsList_.insert({ std::string(key), src });
	settingsList_[key] = src;
}








// searches a value by the key in the map and returns an iterator to it;
std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::string, std::string>>>> 
Settings::CheckSettingKey(const char* key)
{
	auto iterator = settingsList_.find(key);

	// check if we have such a key
	if (iterator == settingsList_.end())
	{
		std::string strKey{ key };
		std::string errorMsg{ "there is no such a key: " + strKey };
		COM_ERROR_IF_FALSE(false, errorMsg);
	}

	return iterator;
}
