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
#include "movelookcontroller.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "textureshaderclass.h"
#include "bitmapclass.h"
#include "character2d.h"
#include "inputclass.h"
//#include "textclass.h"
#include "debugtextclass.h" // for printing the debug data onto the screen

#include "frustumclass.h"


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
	bool Frame(InputClass* input, int fps, int cpu, float frameTime);

	void* operator new(size_t i);
	void operator delete(void* ptr);

public:
	bool FULL_SCREEN;

private:
	bool Initialize3D(D3DClass* m_D3D, HWND hwnd);
	bool Initialize2D(HWND hwnd, DirectX::XMMATRIX baseViewMatrix);

	bool Render(InputClass* input, float rotation, int fps, int cpu, float frameTime);
	bool Render3D(float rotation);
	bool Render2D(InputClass* input, float rotation, int fps, int cpu);

private:
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_orthoMatrix;

	int m_screenWidth, m_screenHeight;

	D3DClass*           m_D3D;
	ModelClass*         m_Model;
	CameraClass*        m_Camera;
	MoveLookController* m_pMoveLook;
	LightShaderClass*   m_LightShader;
    LightClass*         m_Light;
	BitmapClass*        m_Bitmap;
	Character2D*        m_Character2D;
	TextureShaderClass* m_TextureShader;
	//TextClass*          m_pText;
	//InputClass*         m_pInput;
	DebugTextClass*     m_pDebugText;
	

	FrustumClass*       m_pFrustum;
};
