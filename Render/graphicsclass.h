////////////////////////////////////////////////////////////////////
// Filename:     graphicsclass.h
// Description:  controls all the main parts of rendering process
// Revising:     07.11.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include "../Engine/macros.h" 
#include "../Engine/Log.h"             // logger
#include "d3dclass.h"                  // for initialization of DirectX stuff

// shaders
#include "../ShaderClass/colorshaderclass.h"
#include "../ShaderClass/textureshaderclass.h"
#include "../ShaderClass/lightshaderclass.h"

// models
#include "../2D/bitmapclass.h"
#include "../2D/character2d.h"

#include "../Model/Triangle.h"
#include "../Model/modelclass.h"
#include "../Model/modellistclass.h"   // for making a list of models which are in the scene
#include "../Render/frustumclass.h"    // for frustum culling

// light
#include "../Render/lightclass.h"

// camera
#include "../Camera/positionclass.h"   // for controlling of the camera movement using mouse/keyboard
#include "../Camera/cameraclass.h"

// input
#include "../Input/inputclass.h"

// UI
//#include "textclass.h"               // basic text class (in UI) 
#include "../UI/Text/debugtextclass.h" // for printing the debug data onto the screen




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
	bool InitializeDirectX(HWND hwnd, int width, int height);      // initialized all the DirectX stuff

	bool InitializeShaders(HWND hwnd);                             // initialize all the shaders (color, texture, light, etc.)
	bool InitializeScene(HWND hwnd);
	
	bool InitializeModels(HWND hwnd);                              // initialize all the list of models on the scene
	bool InitializeModel(LPSTR modelFilename, WCHAR* textureName); // initialize a single model by its name and texture
	bool InitializeCamera(DirectX::XMMATRIX& baseViewMatrix);
	bool InitializeLight(HWND hwnd);
	bool InitializeGUI(HWND hwnd, const DirectX::XMMATRIX& baseViewMatrix); // initialize the GUI of the game/engine (interface elements, text, etc.)


	bool RenderScene();                                            // render all the stuff on the scene
	bool RenderModels(int& renderCount);
	bool RenderGUI();
	bool RenderGUIDebugText();
	
	//bool Render3D(int& renderCount);  // render all the 3D stuff on the screen
	//bool Render2D(InputClass* pInput, int fps, int cpu, int renderCount); // render all the 2D stuff on the screen

private:
	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX viewMatrix_;
	DirectX::XMMATRIX projectionMatrix_;
	DirectX::XMMATRIX orthoMatrix_;

	D3DClass*           pD3D_ = nullptr;           // DirectX stuff
	
	// shaders
	ColorShaderClass*   pColorShader_ = nullptr;   // for rendering models with only colour but not textures
	TextureShaderClass* pTextureShader_ = nullptr; // for texturing models
	LightShaderClass*   pLightShader_ = nullptr;   // for light effect on models

	// models
	BitmapClass*        pBitmap_ = nullptr;        // for a 2D texture plane 
	Character2D*        pCharacter2D_ = nullptr;   // for a 2D character
	ModelClass*         pModel_ = nullptr;		   // some model
	Triangle*           pTriangleRed_ = nullptr;
	Triangle*           pTriangleGreen_ = nullptr;
	ModelListClass*     pModelList_ = nullptr;     // for making a list of models which are in the scene
	FrustumClass*       pFrustum_ = nullptr;       // for frustum culling
	 
	// light
	LightClass*         pLight_ = nullptr;         // contains light data
	
	// camera	
	CameraClass*        pCamera_ = nullptr;        // camera system

	// UI
	//TextClass*          m_pText;
	DebugTextClass*     pDebugText_ = nullptr;     // for printing the debug data onto the screen
	
	const float screenNear_ = 0.1f;                // near render plane
	const float screenDepth_ = 1000.0f;            // far render plane
	bool fullScreen_ = false;                      // full screen or windowed mode
	bool vsyncEnabled_ = false;                    // vsync is disabled by default
	int screenWidth_ = 800;                        // default screen width
	int screenHeight_ = 600;                       // default screen height

}; // GraphicsClass
