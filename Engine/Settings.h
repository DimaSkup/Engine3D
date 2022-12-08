////////////////////////////////////////////////////////////////////
// Filename:    Settings.h
// Description: contains the settings for engine, camera, player, etc.
// Revising:    27.11.22
////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

class SETTINGS
{
private:
	struct settingsParams
	{
		std::string WINDOW_TITLE = "Title";
		int  SCREEN_WIDTH = 800;
		int  SCREEN_HEIGHT = 600;
		bool FULL_SCREEN = false;
		bool VSYNC_ENABLED = false;           // vsync is disabled by default
		
		float CAMERA_SENSITIVITY = 0.01f;     // aka mouse sensivity
	};

public:
	SETTINGS()
	{
		params = new SETTINGS::settingsParams();
	}

	static settingsParams* GetSettings() { return params; }

private:
	static settingsParams* params;
};

