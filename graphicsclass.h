////////////////////////////////////////////////////////////////////
// Filename:     graphicsclass.h
// Description:  controls all the main parts of rendering process
// Revising:     18.04.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
//#include "includes.h"
#include "macros.h"
#include "log.h"

#include "d3dclass.h"
#include "modelclass.h"
#include "cameraclass.h"
#include "movelookcontroller.h" // for controlling of the camera movement using mouse/keyboard
#include "lightshaderclass.h"
#include "lightclass.h"
#include "textureshaderclass.h"
#include "bitmapclass.h"
#include "character2d.h"
#include "inputclass.h"
//#include "textclass.h"
#include "debugtextclass.h" // for printing the debug data onto the screen
#include "frustumclass.h"   // for frustum culling
#include "modellistclass.h" // for making a list of models which are in the scene


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
	bool Frame(float rotationY);
	bool Render(InputClass* input, int fps, int cpu, float frameTime);  // render the scene on the screen

	void* operator new(size_t i);
	void operator delete(void* ptr);

public:
	bool FULL_SCREEN;

private:
	bool Initialize3D(D3DClass* m_D3D, HWND hwnd);
	bool Initialize2D(HWND hwnd, DirectX::XMMATRIX baseViewMatrix);

	bool Render3D(void);  // render all the 3D stuff on the screen
	bool Render2D(InputClass* input, int fps, int cpu, int renderCount); // render all the 2D stuff on the screen

private:
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_orthoMatrix;

	int m_screenWidth, m_screenHeight;

	D3DClass*           m_D3D;
	ModelClass*         m_Model;
	CameraClass*        m_Camera;
	MoveLookController* m_pMoveLook;    // for controlling of the camera movement using mouse/keyboard
	LightShaderClass*   m_LightShader;
    LightClass*         m_Light;
	BitmapClass*        m_Bitmap;
	Character2D*        m_Character2D;
	TextureShaderClass* m_TextureShader;
	//TextClass*          m_pText;
	//InputClass*         m_pInput;
	DebugTextClass*     m_pDebugText;  // for printing the debug data onto the screen
	ModelListClass*     m_pModelList;  // for making a list of models which are in the scene
	FrustumClass*       m_pFrustum;    // for frustum culling
};
