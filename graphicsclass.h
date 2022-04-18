////////////////////////////////////////////////////////////////////
// Filename:     graphicsclass.h
// Description:  controls all the main parts of rendering process
// Revising:     18.04.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "includes.h"
#include "log.h"

#include "d3dclass.h"
#include "modelclass.h"
#include "cameraclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"


//////////////////////////////////
// GRAPHIC GLOBAL VARIABLES
//////////////////////////////////
const bool VSYNC_ENABLED = false;
const float SCREEN_NEAR = 0.1f;
const float SCREEN_DEPTH = 1000.0f;

//////////////////////////////////
// Class name: GraphicsClass
//////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass(void);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass(void);

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, bool fullscreen);
	void Shutdown(void);
	bool Frame(void);

public:
	bool FULL_SCREEN;

private:
	bool Render(float rotation);

	D3DClass*         m_D3D;
	ModelClass*       m_Model;
	CameraClass*      m_Camera;
	LightShaderClass* m_LightShader;
	LightClass*       m_Light;
};
