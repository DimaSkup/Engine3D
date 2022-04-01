///////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
///////////////////////////////////////////////////////////////////////////////
#pragma once

/////////////////////////////
// MY CLASS INCLUDES
/////////////////////////////
#include "includes.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"

/////////////////////////////
// GLOBALS
/////////////////////////////
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, bool fullScreen);
	void Shutdown(void);
	bool Frame(void);

public:
	bool FULL_SCREEN;

private:
	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ColorShaderClass* m_ColorShader;
};