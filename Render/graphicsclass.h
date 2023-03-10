////////////////////////////////////////////////////////////////////
// Filename:     graphicsclass.h
// Description:  controls all the main parts of rendering process
// Revising:     07.11.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <string>
#include <map>





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
#include "../ShaderClass/DataContainerForShadersClass.h"  // data container for shaders
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

//#include "../Model/Triangle.h"
//#include "../Model/Square.h"


#include "../Model/modelclass.h"
#include "../Model/ModelToShaderMediator.h"
#include "../Model/modellistclass.h"   // for making a list of models which are in the scene
#include "../Render/frustumclass.h"    // for frustum culling

// terrain related stuff



// light
#include "../Render/lightclass.h"

// input
//#include "../Input/inputclass.h"

// UI
//#include "textclass.h"               // basic text class (in UI) 
#include "../UI/Text/debugtextclass.h" // for printing the debug data onto the screen

// timers
//#include "../Timers/timerclass.h"
#include "../Timers/timer.h"

#include "ZoneClass.h"




class InitializeGraphics;
class RenderGraphics;

//////////////////////////////////
// Class name: GraphicsClass
//////////////////////////////////
class GraphicsClass
{
public:
	friend InitializeGraphics;              // for initialization of the graphics
	friend RenderGraphics;                  // for rendering of the graphics

public:
	GraphicsClass(void);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass(void);


	bool Initialize(HWND hwnd);
	void Shutdown(void);
	bool RenderFrame(SystemState* systemState);
		//KeyboardEvent& kbe,
		//MouseEvent& me,
		//float deltaTime);


	// handle events from the keyboard and mouse
	void HandleMovementInput(const KeyboardEvent& kbe, const MouseEvent& me, float deltaTime);

	const D3DClass* GetD3DClass() const
	{
		return pD3D_;
	}

	void AddShader(std::string shaderName, ShaderClass* pShader);
	ShaderClass* GetShaderByName(std::string shaderName)
	{
		return shadersMap_.at(shaderName);
	}


	// matrices getters
	const DirectX::XMMATRIX & GetWorldMatrix() const;
	const DirectX::XMMATRIX & GetViewMatrix() const;
	const DirectX::XMMATRIX & GetProjectionMatrix() const;
	const DirectX::XMMATRIX & GetOrthoMatrix() const;


	// memory allocation
	void* operator new(size_t i);
	void operator delete(void* ptr);

	

private:
	bool RenderScene(SystemState* systemState);              // render all the stuff on the scene
	
private:
	

	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX viewMatrix_;
	DirectX::XMMATRIX projectionMatrix_;
	DirectX::XMMATRIX orthoMatrix_;

	SETTINGS::settingsParams* settingsList;       // engine settings

	D3DClass*           pD3D_ = nullptr;           // DirectX stuff

	// shaders
	std::map<std::string, ShaderClass*> shadersMap_;
	DataContainerForShadersClass* pDataForShaders_ = nullptr;
	ModelConverterClass           modelConverter_;
	/*
	ColorShaderClass*        pColorShader_ = nullptr;         // for rendering models with only colour but not textures
	TextureShaderClass*      pTextureShader_ = nullptr;       // for texturing models
	LightShaderClass*        pLightShader_ = nullptr;         // for light effect on models
	MultiTextureShaderClass* pMultiTextureShader_ = nullptr;  // for multitexturing
	LightMapShaderClass*     pLightMapShader_ = nullptr;      // for light mapping
	AlphaMapShaderClass*     pAlphaMapShader_ = nullptr;      // for alpha mapping
	BumpMapShaderClass*      pBumpMapShader_ = nullptr;       // for bump mapping
	CombinedShaderClass*     pCombinedShader_ = nullptr;      // for different shader effects (multitexturing, lighting, alpha mapping, etc.)

	*/
	// zone / terrain
	ZoneClass*          pZone_ = nullptr;

	// models
	BitmapClass*        pBitmap_ = nullptr;             // for a 2D texture plane 
	Character2D*        pModelCharacter2D_ = nullptr;   // for a 2D character
	ModelClass*         pModel_ = nullptr;		        // some model

	// default models

	ModelListClass*     pModelList_ = nullptr;     // for making a list of models which are in the scene
	FrustumClass*       pFrustum_ = nullptr;       // for frustum culling
	 
	// light
	LightClass*         pLight_ = nullptr;         // contains light data
	
	// UI
	DebugTextClass*     pDebugText_ = nullptr;     // for printing the debug data onto the screen           
}; // GraphicsClass





//////////////////////////////////
// Class name: InitializeGraphics
//////////////////////////////////
class InitializeGraphics
{
public:
	bool InitializeDirectX(GraphicsClass* pGraphics, HWND hwnd, int windowWidth, int windowHeight, bool vsyncEnabled, bool fullScreen, float screenNear, float screenDepth);      // initialized all the DirectX stuff
	bool InitializeShaders(GraphicsClass* pGraphics, HWND hwnd);                             // initialize all the shaders (color, texture, light, etc.)
	bool InitializeScene(GraphicsClass* pGraphics, HWND hwnd, SETTINGS::settingsParams* settingsList);

	bool InitializeModels(GraphicsClass* pGraphics);                              // initialize all the list of models on the scene
	bool InitializeCamera(GraphicsClass* pGraphics, DirectX::XMMATRIX& baseViewMatrix, SETTINGS::settingsParams* settingsList);
	bool InitializeLight(GraphicsClass* pGraphics);
	bool InitializeGUI(GraphicsClass* pGraphics, HWND hwnd, const DirectX::XMMATRIX& baseViewMatrix); // initialize the GUI of the game/engine (interface elements, text, etc.)
	bool InitializeInternalDefaultModels(GraphicsClass* pGraphics, ID3D11Device* pDevice);

private:
	bool InitializeDefaultModels(ID3D11Device* pDevice);
	bool CreateCube(ID3D11Device* pDevice, ShaderClass* pShader, size_t cubesCount = 1);
	bool CreateSphere(ID3D11Device* pDevice, ShaderClass* pShader, size_t spheresCount = 1);
	bool CreateTerrain(ID3D11Device* pDevice, ShaderClass* pShader);

private:
	// number of models on the screen
	const size_t SPHERES_NUMBER_ = 10;
	const size_t CUBES_NUMBER_ = 10;
};




//////////////////////////////////
// Class name: RenderGraphics
//////////////////////////////////
class RenderGraphics
{
public:
	bool RenderModels(GraphicsClass* pGraphics, int& renderCount);
	bool RenderGUI(GraphicsClass* pGraphics, SystemState* systemState);                // render all the GUI parts onto the screen

private:
	bool RenderGUIDebugText(GraphicsClass* pGraphics, SystemState* systemState);

};