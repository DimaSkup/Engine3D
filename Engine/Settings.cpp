#include "Settings.h"

Settings* Settings::pInstance_ = nullptr;

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
		Log::Error(THIS_FUNC, errorMsg.c_str());
		return NULL;
	}

	return intVal;
}


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
		Log::Error(THIS_FUNC, errorMsg.c_str());
		return NULL;
	}

	return floatVal;
}


bool Settings::GetSettingBoolByKey(const char* key)
{
	// check if we have such a key
	auto iterator = CheckSettingKey(key);

	// convert const char* into bool
	std::istringstream iCharStream(iterator->second.c_str());
	bool boolVal = false;

	if ((iCharStream >> boolVal).fail())
	{
		std::string errorMsg{ "can't convert value from string into bool: " + iterator->second };
		Log::Error(THIS_FUNC, errorMsg.c_str());
		return NULL;
	}

	return boolVal;
}


std::string Settings::GetSettingStrByKey(const char* key)
{
	// check if we have such a key
	auto iterator = CheckSettingKey(key);

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
		Log::Error(THIS_FUNC, errorMsg.c_str());
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
	CheckSettingKey(key);

	// update a setting value
	settingsList_[key] = src;
}

/*

template<class T>
void Settings::UpdateSettingByKey(const std::string & key, T src)
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
*/




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
