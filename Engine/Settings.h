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
		bool FULL_SCREEN = false;
		std::string WINDOW_TITLE = "Title";
		int SCREEN_WIDTH = 800;
		int SCREEN_HEIGHT = 600;
		float CAMERA_SENSITIVITY = 0.01f;  // aka mouse sensivity
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

