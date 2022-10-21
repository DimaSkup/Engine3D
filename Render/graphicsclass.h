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
#include "../Engine/Log.h"             // logger

#include "d3dclass.h"                  // for initialization of DirectX stuff
#include "../Model/modelclass.h"
#include "../Camera/cameraclass.h"
#include "../ShaderClass/lightshaderclass.h"
#include "../Render/lightclass.h"
#include "../ShaderClass/textureshaderclass.h"
#include "../2D/bitmapclass.h"
#include "../2D/character2d.h"
#include "../Input/inputclass.h"
//#include "textclass.h"               // basic text class (in UI) 
#include "../UI/Text/debugtextclass.h" // for printing the debug data onto the screen
#include "frustumclass.h"              // for frustum culling
#include "../Model/modellistclass.h"   // for making a list of models which are in the scene
#include "../Camera/positionclass.h"   // for controlling of the camera movement using mouse/keyboard

//////////////////////////////////
// Class name: GraphicsClass
//////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass(void);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass(void);

	bool Initialize(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen);
	void Shutdown(void);
	bool RenderFrame();
	//bool Frame(PositionClass* pPosition);
	//bool Render(InputClass* pInput, int fps, int cpu, float frameTime);  // render the scene on the screen

	void* operator new(size_t i);
	void operator delete(void* ptr);

private:
	bool InitializeDirectX(HWND hwnd, int width, int height); // initialized all the DirectX stuff
	bool InitializeCamera(DirectX::XMMATRIX& baseViewMatrix);
	//bool Initialize3D(D3DClass* m_D3D, HWND hwnd);
	//bool Initialize2D(HWND hwnd, DirectX::XMMATRIX baseViewMatrix);

	//bool Render3D(int& renderCount);  // render all the 3D stuff on the screen
	//bool Render2D(InputClass* pInput, int fps, int cpu, int renderCount); // render all the 2D stuff on the screen

private:
	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX viewMatrix_;
	DirectX::XMMATRIX projectionMatrix_;
	DirectX::XMMATRIX orthoMatrix_;

	D3DClass*           pD3D_ = nullptr;           // DirectX stuff
	ModelClass*         pModel_ = nullptr;         // models
	CameraClass*        pCamera_ = nullptr;        // camera system
	
	LightShaderClass*   pLightShader_ = nullptr;   // for rendering light effect on models
	LightClass*         pLight_ = nullptr;         // contains light data
	BitmapClass*        pBitmap_ = nullptr;        // for a 2D texture plane 
	Character2D*        pCharacter2D_ = nullptr;   // for a 2D character
	TextureShaderClass* pTextureShader_ = nullptr; // for texturing

	//TextClass*          m_pText;
	DebugTextClass*     pDebugText_ = nullptr;    // for printing the debug data onto the screen
	ModelListClass*     pModelList_ = nullptr;    // for making a list of models which are in the scene
	FrustumClass*       pFrustum_ = nullptr;      // for frustum culling

	const float screenNear_ = 0.1f;     // near render plane
	const float screenDepth_ = 1000.0f; // far render plane
	bool fullScreen_ = false;           // full screen or windowed mode
	bool vsyncEnabled_ = false;         // vsync is disabled by default
	int screenWidth_ = 800;             // default screen width
	int screenHeight_ = 600;            // default screen height

}; // GraphicsClass
