////////////////////////////////////////////////////////////////////
// Filename:    Settings.h
// Description: contains the settings for engine, camera, player, etc.
// Revising:    27.11.22
////////////////////////////////////////////////////////////////////
#pragma once

#include "log.h"
#include <fstream>
#include <string>

class Settings
{
public:
	struct settingsParams
	{
		// window settings
		std::string WINDOW_TITLE = "Title";
		int WINDOW_TOP_POS = 0;
		int WINDOW_LEFT_POS = 0;
		int  WINDOW_WIDTH = 800;
		int  WINDOW_HEIGHT = 600;             
		bool FULL_SCREEN = false;             // full screen mode flag
		bool VSYNC_ENABLED = false;           // vsync is disabled by default
		
		// camera settings
		float CAMERA_SPEED = 0.02f;           // speed of the camera movement
		float CAMERA_SENSITIVITY = 0.01f;     // aka mouse sensivity
		float FOV_DEGREES = 90.0f;            // field of view
		float NEAR_Z = 0.1f;                  // near render plane
		float FAR_Z = 100.0f;                // far render plane

		// models settings
		const std::string MODEL_FILE_TYPE{ ".txt" };         // internal model data file will have this format
		const std::string MODEL_DIR_PATH{ "data/models/" };  // the path to the directory with models

		// number of models on the screen
		size_t SPHERES_NUMBER = 10;
		size_t CUBES_NUMBER = 10;
	};

protected:
	Settings();

	Settings(Settings & other) = delete;        // should not be cloneable
	void operator=(const Settings &) = delete;  // should not be assignable

public:
	static settingsParams* GetSettings() { return pParams_; }
	static Settings* GetInstance();

	bool LoadSettingsFromFile();

private:
	static Settings* pInstance_;
	static settingsParams* pParams_;
};

Settings* ENGINE_SETTINGS = Settings::GetInstance();   // make a global variable for settings to use it everywhere
