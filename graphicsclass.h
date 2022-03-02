///////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
///////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////
// MY CLASS INCLUDES
//////////////////////
#include "includes.h"
#include "d3dclass.h"

//////////////////////
// GLOBALS
//////////////////////
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, bool);
	void Shutdown(void);
	bool Frame(void);

public:
	bool FULL_SCREEN;

private:
	bool Render();

private:
	D3DClass* m_D3D;
};