////////////////////////////////////////////////////////////////////
// Filename:     graphicsclass.h
// Description:  controls all the main parts of rendering process
// Revising:     07.11.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////

// engine stuff
#include "../Engine/macros.h" 
#include "../Engine/Log.h"             // logger
#include "../Engine/SystemState.h"     // contains the current information about the engine

// render stuff
#include "d3dclass.h"                  // for initialization of DirectX stuff
#include "InitializeGraphics.h"        // for initialization of the graphics

// input devices events
#include "../Keyboard/KeyboardEvent.h"
#include "../Mouse/MouseEvent.h"

// shaders
#include "../ShaderClass/colorshaderclass.h"
#include "../ShaderClass/textureshaderclass.h"
#include "../ShaderClass/lightshaderclass.h"
#include "../ShaderClass/MultiTextureShaderClass.h"
#include "../ShaderClass/LightMapShaderClass.h"
#include "../ShaderClass/AlphaMapShaderClass.h"
#include "../ShaderClass/BumpMapShaderClass.h"
#include "../ShaderClass/CombinedShaderClass.h"

// models
#include "../2D/bitmapclass.h"
#include "../2D/character2d.h"

#include "../Model/Triangle.h"
#include "../Model/Square.h"
#include "../Model/modelclass.h"
#include "../Model/modellistclass.h"   // for making a list of models which are in the scene
#include "../Render/frustumclass.h"    // for frustum culling

// light
#include "../Render/lightclass.h"

// camera
#include "../Camera/cameraclass.h"
#include "../Camera/EditorCamera.h"

// input
//#include "../Input/inputclass.h"

// UI
//#include "textclass.h"               // basic text class (in UI) 
#include "../UI/Text/debugtextclass.h" // for printing the debug data onto the screen

// timers
//#include "../Timers/timerclass.h"
#include "../Timers/timer.h"


//////////////////////////////////
// Class name: GraphicsClass
//////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass(void);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass(void);

	bool Initialize(HWND hwnd);
	void Shutdown(void);
	bool RenderFrame(SystemState* systemState, KeyboardEvent& kbe, MouseEvent& me, MouseClass& mouse, float deltaTime);
	//bool ResizeBuffers();

	void* operator new(size_t i);
	void operator delete(void* ptr);

	EditorCamera editorCamera_;

private:
	friend bool InitializeDirectX(GraphicsClass* pGraphics, HWND hwnd, int windowWidth, int windowHeight, bool vsyncEnabled, bool fullScreen, float screenNear, float screenDepth);      // initialized all the DirectX stuff
	friend bool InitializeShaders(GraphicsClass* pGraphics, HWND hwnd);                             // initialize all the shaders (color, texture, light, etc.)
	friend bool InitializeScene(GraphicsClass* pGraphics, HWND hwnd, SETTINGS::settingsParams* settingsList);

	friend bool InitializeModels(GraphicsClass* pGraphics);                              // initialize all the list of models on the scene
	friend bool InitializeModel(GraphicsClass* pGraphics, const string& modelFilename, const string& modelId, WCHAR* textureName1, WCHAR* textureName2 = nullptr); // initialize a single model by its name and textures
	friend bool InitializeCamera(GraphicsClass* pGraphics, DirectX::XMMATRIX& baseViewMatrix, SETTINGS::settingsParams* settingsList);
	friend bool InitializeLight(GraphicsClass* pGraphics);
	friend bool InitializeGUI(GraphicsClass* pGraphics, HWND hwnd, const DirectX::XMMATRIX& baseViewMatrix); // initialize the GUI of the game/engine (interface elements, text, etc.)
	friend bool InitializeInternalDefaultModels(GraphicsClass* pGraphics, ID3D11Device* pDevice);

	bool RenderScene(SystemState* systemState);              // render all the stuff on the scene
	friend bool RenderModels(GraphicsClass* pGraphics, int& renderCount);
	friend bool RenderGUI(GraphicsClass* pGraphics, SystemState* systemState);                // render all the GUI parts onto the screen
	friend bool RenderGUIDebugText(GraphicsClass* pGraphics, SystemState* systemState);
	
private:
	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX viewMatrix_;
	DirectX::XMMATRIX projectionMatrix_;
	DirectX::XMMATRIX orthoMatrix_;

	SETTINGS::settingsParams* settingsList;       // engine settings
	friend class InitializeGraphics;              // for initialization of the graphics

	D3DClass*           pD3D_ = nullptr;           // DirectX stuff

	// shaders
	ColorShaderClass*        pColorShader_ = nullptr;         // for rendering models with only colour but not textures
	TextureShaderClass*      pTextureShader_ = nullptr;       // for texturing models
	LightShaderClass*        pLightShader_ = nullptr;         // for light effect on models
	MultiTextureShaderClass* pMultiTextureShader_ = nullptr;  // for multitexturing
	LightMapShaderClass*     pLightMapShader_ = nullptr;      // for light mapping
	AlphaMapShaderClass*     pAlphaMapShader_ = nullptr;      // for alpha mapping
	BumpMapShaderClass*      pBumpMapShader_ = nullptr;       // for bump mapping
	CombinedShaderClass*     pCombinedShader_ = nullptr;      // for different shader effects (multitexturing, lighting, alpha mapping, etc.)

	// models
	BitmapClass*        pBitmap_ = nullptr;             // for a 2D texture plane 
	Character2D*        pModelCharacter2D_ = nullptr;   // for a 2D character
	ModelClass*         pModel_ = nullptr;		        // some model
	ModelClass*         pCube_ = nullptr;               // a 3D cube
	Triangle*           pModelTriangleRed_ = nullptr;
	Triangle*           pModelTriangleGreen_ = nullptr;
	Square*             pModelYellowSquare_ = nullptr;
	Square*             pModelCatSquare_ = nullptr;
	Square*             pModelSquareLightMapped_ = nullptr;
	Square*             pModelSquareAlphaMapped_ = nullptr;

	ModelListClass*     pModelList_ = nullptr;     // for making a list of models which are in the scene
	FrustumClass*       pFrustum_ = nullptr;       // for frustum culling
	 
	// light
	LightClass*         pLight_ = nullptr;         // contains light data
	
	// camera	
	//CameraClass*        pCamera_ = nullptr;        // camera system
	//EditorCamera        editorCamera_;             // the camera of the engine editor


	// UI
	//TextClass*          m_pText;
	DebugTextClass*     pDebugText_ = nullptr;     // for printing the debug data onto the screen           
}; // GraphicsClass
