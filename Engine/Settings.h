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
	static Settings* GetInstance();

	// get a setting value in a particular type
	static int GetSettingIntByKey(const char* key);
	static float GetSettingFloatByKey(const char* key);
	static bool GetSettingBoolByKey(const char* key);
	static std::string GetSettingStrByKey(const char* key);


	// get some setting by a key and write it's value into the desc variable
	//template<class T>
	//static void GetSettingByKey(const char* key, T & dest);

	// for string source type we use this function for updating some setting by a key
	static void UpdateSettingByKey(const char* key, const std::string & src);

	// for simple source types (int, float, bool, etc.) we use this function for
	// updating some setting by a key
	template<class T>
	static void UpdateSettingByKey(const std::string & key, T src)
	{
		// check if we have such a key
		CheckSettingKey(key);

		// check if the src type is allowed
		if ((typeid(src) == typeid(float)) ||
			(typeid(src) == typeid(bool)) ||
			(typeid(src) == typeid(int)))
		{
			settingsList_[key] = std::to_string(src);
		}
		else
		{
			std::string typeName{ typeid(T).name() };
			std::string errorMsg{ "wrong source type: " + typeName };
			Log::Error(THIS_FUNC, errorMsg.c_str());
		}

		return;
	}

protected:
	Settings();

	Settings(Settings & other) = delete;        // should not be cloneable
	void operator=(const Settings &) = delete;  // should not be assignable

private:
	bool LoadSettingsFromFile();

	// searches a value by the key in the map and returns an iterator to it;
	static std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<const std::string, std::string>>>> CheckSettingKey(const char* key);        

private:
	static Settings* pInstance_;
	static std::map <std::string, std::string> settingsList_;   // contains pairs [setting_key => setting_value]

	//static settingsParams* pParams_;
};

