////////////////////////////////////////////////////////////////////
// Filename:    Settings.h
// Description: contains settings for the engine; uses a singleton pattern
//
// Revising:    27.11.22
////////////////////////////////////////////////////////////////////
#pragma once

#include "log.h"
#include <fstream>
#include <string>
#include <map>
#include <sstream>

class Settings
{
public:

	static Settings* Get();

	void Shutdown();   // release memory from the settings data

	// get a setting value in a particular type
	int GetSettingIntByKey(const char* key);
	float GetSettingFloatByKey(const char* key);
	bool GetSettingBoolByKey(const char* key);
	std::string GetSettingStrByKey(const char* key);

	// for string source type we use this function for updating some setting by a key
	void UpdateSettingByKey(const char* key, const std::string & src);

	// for simple source types (int, float, bool, etc.) we use this function for
	// updating some setting by a key
	template<typename T>
	void UpdateSettingByKey(const char* key, T src);

private:
	Settings();

	Settings(Settings & other) = delete;        // should not be cloneable
	void operator=(const Settings &) = delete;  // should not be assignable

	bool LoadSettingsFromFile();

	// searches a value by the key in the map and returns an iterator to it;
	std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::string, std::string>>>> CheckSettingKey(const char* key);

private:
	static Settings* pInstance_;
	std::map <std::string, std::string> settingsList_;   // contains pairs [setting_key => setting_value]
};


// record setting data (src) by a particular key
template<typename T>
void Settings::UpdateSettingByKey(const char* key, T src)
{
	// check if we have such a key
	CheckSettingKey(key);

	// check if the src type is allowed
	if ((typeid(src) == typeid(float)) ||
		(typeid(src) == typeid(bool)) ||
		(typeid(src) == typeid(int)))
	{
		Settings::settingsList_[key] = std::to_string(src);
	}
	// we have wrong type
	else
	{
		std::string typeName{ typeid(T).name() };
		std::string errorMsg{ "wrong source type: " + typeName };
		Log::Error(LOG_MACRO, errorMsg.c_str());
	}

	return;
}

