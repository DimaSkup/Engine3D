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
#include "../Engine/macros.h"
#include "../Engine/Log.h"

#include "../Engine/d3dclass.h"
#include "../Model/modelclass.h"
#include "../Camera/cameraclass.h"
//#include "movelookcontroller.h" // for controlling of the camera movement using mouse/keyboard
#include "../ShaderClass/lightshaderclass.h"
#include "../Render/lightclass.h"
#include "../ShaderClass/textureshaderclass.h"
#include "../2D/bitmapclass.h"
#include "../2D/character2d.h"
#include "../Input/inputclass.h"
//#include "textclass.h"
#include "../UI/Text/debugtextclass.h" // for printing the debug data onto the screen
#include "frustumclass.h"   // for frustum culling
#include "../Model/modellistclass.h" // for making a list of models which are in the scene
#include "../Camera/positionclass.h"  // for handling the camera movement


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
	bool GraphicsClass::Frame(PositionClass* pPosition);
	bool Render(InputClass* pInput, int fps, int cpu, float frameTime);  // render the scene on the screen

	void* operator new(size_t i);
	void operator delete(void* ptr);

public:
	bool FULL_SCREEN;

private:
	bool Initialize3D(D3DClass* m_D3D, HWND hwnd);
	bool Initialize2D(HWND hwnd, DirectX::XMMATRIX baseViewMatrix);

	bool Render3D(int& renderCount);  // render all the 3D stuff on the screen
	bool Render2D(InputClass* pInput, int fps, int cpu, int renderCount); // render all the 2D stuff on the screen

private:
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_orthoMatrix;

	int m_screenWidth, m_screenHeight;

	D3DClass*           m_D3D;
	ModelClass*         m_Model;
	CameraClass*        m_Camera;
	//MoveLookController* m_pMoveLook;    // for controlling of the camera movement using mouse/keyboard
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
