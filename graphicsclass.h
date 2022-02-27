///////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <windows.h>

//////////////////////
// GLOBALS
//////////////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown(void);
	bool Frame(void);

private:
	bool Render();

private:

};