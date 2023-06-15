#include "Settings.h"

Settings* Settings::pInstance_ = nullptr;
Settings::settingsParams* Settings::pParams_ = nullptr;

Settings::Settings()
{
	LoadSettingsFromFile();
}

Settings* Settings::GetInstance()
{
	if (pInstance_ == nullptr)
	{
		pInstance_ = new Settings;
	}

	return pInstance_;
}

bool Settings::LoadSettingsFromFile()
{
	

	/*
	std::fstream fin;

	fin.open("data/settings");

	if (fin.fail())
	{
		Log::Error(THIS_FUNC, "can't open the settings file");
		return false;
	}
	*/




	// window settings
	settingsList_.insert({ "WINDOW_TITLE", "Title" });
	settingsList_.insert({ "WINDOW_TOP_POS", "0" });
	settingsList_.insert({ "WINDOW_LEFT_POS", "0" });
	settingsList_.insert({ "WINDOW_WIDTH", "800" });
	settingsList_.insert({ "WINDOW_HEIGHT", "600" });
	settingsList_.insert({ "FULL_SCREEN", "false" });         // full screen mode flag
	settingsList_.insert({ "VSYNC_ENABLED", "false" });       // vsync is disabled by default    


	// camera settings
	settingsList_.insert({ "CAMERA_SPEED", "0.02f" });        // speed of the camera movement
	settingsList_.insert({ "CAMERA_SENSITIVITY", "0.01f" });  // aka mouse sensivity
	settingsList_.insert({ "FOV_DEGREES", "90.0f" });         // field of view
	settingsList_.insert({ "NEAR_Z", "0.1f" });               // near render plane 
	settingsList_.insert({ "FAR_Z", "100.0f" });              // far render plane
            

	// models settings
	settingsList_.insert({ "MODEL_FILE_TYPE", ".txt" });         // internal model data file will have this format
	settingsList_.insert({ "MODEL_DIR_PATH",  "data/models/" }); // the path to the directory with models

	// number of models on the screen
	settingsList_.insert({ "SPHERES_NUMBER", "10" });
	settingsList_.insert({ "CUBES_NUMBER", "10" });

	return true;
}

template<class T>
T Settings::GetSettingByKey(const std::string & settingKey, T & dest)
{
	auto iterator = settingsList_.find(settingKey);
	

	if (typeid(dest) == typeid(float))
		return ::atof(iterator->second.c_str());
	else if (typeid(dest) == typeid(bool))
		return (iterator->second == "true");
	else if (typeid(dest) == typeid(int))
		return static_cast<int>(iterator->second.c_str());
	else if (typeid(dest) == typeid(std::string))
		return iterator->second;
	else
	{
		COM_ERROR_IF_FALSE(false, "wrong destination type");
	}
}