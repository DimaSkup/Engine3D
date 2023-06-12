#include "Settings.h"

Settings* Settings::pInstance_ = nullptr;
Settings::settingsParams* Settings::pParams_ = nullptr;

Settings::Settings()
{
	

	pParams_ = new Settings::settingsParams();
	LoadSettingsFromFile();
}

Settings* Settings::GetInstance()
{
	if (pInstance_ == nullptr)
	{
		pInstance_ = new Settings;
	}
	else
		return pInstance_;
}

bool Settings::LoadSettingsFromFile()
{
	std::fstream fin;

	fin.open("data/settings");

	if (fin.fail())
	{
		Log::Error(THIS_FUNC, "can't open the settings file");
		return false;
	}

	return true;
}