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

class Settings
{
public:
	template<class T>
	static std::string GetSettingByKey(const std::string & settingKey, T & dest);
	static Settings* GetInstance();

protected:
	Settings();

	Settings(Settings & other) = delete;        // should not be cloneable
	void operator=(const Settings &) = delete;  // should not be assignable

private:
	bool LoadSettingsFromFile();

private:
	static Settings* pInstance_;
	static std::map <std::string, std::string> settingsList_;   // contains pairs [setting_key => setting_value]
	//static settingsParams* pParams_;
};