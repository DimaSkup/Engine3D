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
#include "../Engine/Settings.h"

// render stuff
#include "d3dclass.h"                  // for initialization of DirectX stuff
#include "InitializeGraphics.h"        // for initialization of the graphics

// input devices events
#include "../Keyboard/KeyboardEvent.h"
#include "../Mouse/MouseEvent.h"

// shaders
#include "../ShaderClass/DataContainerForShadersClass.h"  // data container for shaders
#include "../ShaderClass/ShadersContainer.h"


// models
#include "../2D/bitmapclass.h"
#include "../2D/character2d.h"
//#include "../Model/Triangle.h"
//#include "../Model/Square.h"
//#include "../Model/Model.h"
#include "../Model/ModelToShaderMediator.h"
#include "../Model/modellistclass.h"   // for making a list of models which are in the scene
#include "../Render/frustumclass.h"    // for frustum culling
#include "../Model/TextureManagerClass.h"


// light
#include "../Render/lightclass.h"

// UI
//#include "textclass.h"               // basic text class (in UI) 
#include "../UI/UserInterfaceClass.h"  // a main UI class

// timers
//#include "../Timers/timerclass.h"
#include "../Timers/timer.h"

// terrain / camera / camera movement
#include "ZoneClass.h"




class InitializeGraphics;
class RenderGraphics;

//////////////////////////////////
// Class name: GraphicsClass
//////////////////////////////////
class GraphicsClass final
{
public:
	friend InitializeGraphics;              // for initialization of the graphics
	friend RenderGraphics;                  // for rendering of the graphics

public:
	GraphicsClass(void);
	~GraphicsClass(void);

	// main functions
	bool Initialize(HWND hwnd);
	void Shutdown(void);
	bool RenderFrame(SystemState* systemState, float deltaTime);


	// handle events from the keyboard and mouse
	void HandleMovementInput(const KeyboardEvent& kbe, float deltaTime);
	void HandleMovementInput(const MouseEvent& me, float deltaTime);

	// toggling on and toggling off the wireframe fill mode for the models
	void ChangeModelFillMode();   

	D3DClass* GetD3DClass() const;
	ShadersContainer* GetShadersContainer() const;
	void SetDeltaTime(float deltaTime) { deltaTime_ = deltaTime; };


	// matrices getters
	const DirectX::XMMATRIX & GetWorldMatrix() const;
	const DirectX::XMMATRIX & GetViewMatrix() const;
	const DirectX::XMMATRIX & GetProjectionMatrix() const;
	const DirectX::XMMATRIX & GetOrthoMatrix() const;


	// memory allocation (because we have some XM-data structures)
	void* operator new(std::size_t count);                              // a replaceable allocation function
	void* operator new(std::size_t count, const std::nothrow_t & tag);  // a replaceable non-throwing allocation function
	void* operator new(std::size_t count, void* ptr);                   // a non-allocating placement allocation function
	void operator delete(void* ptr);


private:  // restrict a copying of this class instance
	GraphicsClass(const GraphicsClass & obj);
	GraphicsClass & operator=(const GraphicsClass & obj);
	
private:
	bool RenderScene(SystemState* systemState);   // render all the stuff on the scene
	
private:
	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX viewMatrix_;
	DirectX::XMMATRIX baseViewMatrix_;   // for UI rendering
	DirectX::XMMATRIX projectionMatrix_;
	DirectX::XMMATRIX orthoMatrix_;

	Settings* pEngineSettings_ = nullptr;       // engine settings
	D3DClass*           pD3D_ = nullptr;          // DirectX stuff

	// shaders system
	ShadersContainer*             pShadersContainer_ = nullptr;
	DataContainerForShadersClass* pDataForShaders_ = nullptr;

	// rendering system
	RenderGraphics*     pRenderGraphics_ = nullptr;

	// zone / terrain / clouds / etc.
	ZoneClass*          pZone_ = nullptr;

	// models system
	ModelListClass*     pModelList_ = nullptr;     // for making a list of models which are in the scene
	FrustumClass*       pFrustum_ = nullptr;       // for frustum culling
	TextureManagerClass* pTextureManager_ = nullptr;
	 
	// light
	LightClass*         pLight_ = nullptr;         // contains light data
	
	// UI
	UserInterfaceClass* pUserInterface_ = nullptr; // for work with the graphics user interface (GUI)

	float               deltaTime_ = 0.0f;         // time between frames

	// graphics rendering states
	bool                wireframeMode_ = false;
}; // GraphicsClass





//////////////////////////////////
// Class name: InitializeGraphics
//////////////////////////////////
class InitializeGraphics final
{
public:
	InitializeGraphics(GraphicsClass* pGraphics);

	bool InitializeDirectX(GraphicsClass* pGraphics, HWND hwnd);   // initialized all the DirectX stuff
	bool InitializeTerrainZone(GraphicsClass* pGraphics);  // initialize the main wrapper for all of the terrain processing 
	bool InitializeShaders(GraphicsClass* pGraphics, HWND hwnd);                             // initialize all the shaders (color, texture, light, etc.)
	bool InitializeScene(GraphicsClass* pGraphics, HWND hwnd);

	bool InitializeModels(GraphicsClass* pGraphics);                              // initialize all the list of models on the scene
	bool InitializeLight(GraphicsClass* pGraphics);
	bool InitializeGUI(GraphicsClass* pGraphics, HWND hwnd, const DirectX::XMMATRIX & baseViewMatrix); // initialize the GUI of the game/engine (interface elements, text, etc.)
	bool InitializeInternalDefaultModels(GraphicsClass* pGraphics, ID3D11Device* pDevice);

private:  // restrict a copying of this class instance
	InitializeGraphics(const InitializeGraphics & obj);
	InitializeGraphics & operator=(const InitializeGraphics & obj);


private:
	// create basic models (cube, sphere, etc.)
	void InitializeDefaultModels(ID3D11Device* pDevice, ShaderClass* pColorShader);   // // initialization of the default models which will be used for creation other basic models;   for default models we use a color shader
	void CreateCube(ID3D11Device* pDevice, ShaderClass* pShader, size_t cubesCount = 1);
	void CreateSphere(ID3D11Device* pDevice, ShaderClass* pShader, size_t spheresCount = 1);
	void CreatePlane(ID3D11Device* pDevice, ShaderClass* pShader, UINT planesCount = 1);
	void CreateTree(ID3D11Device* pDevice, ShaderClass* pShader, UINT treesCount = 1);

	// create the zone's elements
	bool CreateTerrain(ID3D11Device* pDevice, ShaderClass* pTerrainShader);
	bool CreateSkyDome(GraphicsClass* pGraphics, ID3D11Device* pDevice, ShaderClass* pSkyDomeShader);
	bool CreateSkyPlane(ID3D11Device* pDevice, ShaderClass* pSkyPlaneShader);

	bool SetupModels(const ShadersContainer* pShadersContainer);  // setup some models for using different shaders

	GraphicsClass* pGraphics_ = nullptr;
	Settings* pEngineSettings_ = Settings::Get();
};




//////////////////////////////////
// Class name: RenderGraphics
//////////////////////////////////
class RenderGraphics final
{
public:
	RenderGraphics();
	~RenderGraphics();

	bool RenderModels(GraphicsClass* pGraphics, int& renderCount, float deltaTime);
	bool RenderGUI(GraphicsClass* pGraphics, SystemState* systemState);                // render all the GUI parts onto the screen

private:  // restrict a copying of this class instance
	RenderGraphics(const RenderGraphics & obj);
	RenderGraphics & operator=(const RenderGraphics & obj);
};