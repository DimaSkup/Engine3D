////////////////////////////////////////////////////////////////////
// Filename:    Settings.h
// Description: contains the settings for engine, camera, player, etc.
// Revising:    27.11.22
////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

class SETTINGS
{
public:
	struct settingsParams
	{
		std::string WINDOW_TITLE = "Title";
		int WINDOW_TOP_POS = 0;
		int WINDOW_LEFT_POS = 0;
		int  WINDOW_WIDTH = 800;
		int  WINDOW_HEIGHT = 600;             
		bool FULL_SCREEN = false;             // full screen mode flag
		bool VSYNC_ENABLED = false;           // vsync is disabled by default
		
		float CAMERA_SPEED = 0.02f;           // speed of the camera movement
		float CAMERA_SENSITIVITY = 0.01f;     // aka mouse sensivity
		float FOV_DEGREES = 90.0f;            // field of view
		float NEAR_Z = 0.1f;                  // near render plane
		float FAR_Z = 1000.0f;                // far render plane
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

