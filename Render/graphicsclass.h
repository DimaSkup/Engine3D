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
#include <memory>





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
#include "../ShaderClass/ShadersContainer.h"


// models
#include "../2D/SpriteClass.h"
#include "../2D/character2d.h"
#include "../Model/ConcreteModelCreator.h"       // for creation of models
#include "../Model/modellistclass.h"             // for making a list of models which are in the scene
#include "../Render/frustumclass.h"              // for frustum culling
#include "../Model/TextureManagerClass.h"
#include "../Model/ModelInitializerInterface.h"  // a common interface for models' initialization


// light
#include "../Render/lightclass.h"

// UI
//#include "textclass.h"               // basic text class (in UI) 
#include "../UI/UserInterfaceClass.h"  // a main UI class

// timers
//#include "../Timers/timerclass.h"
#include "../Timers/timer.h"

// camera
#include "../Camera/EditorCamera.h"

// terrain / camera movement handling
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
	void HandleKeyboardInput(const KeyboardEvent& kbe, float deltaTime);
	void HandleMouseInput(const MouseEvent& me, float deltaTime);

	// toggling on and toggling off the wireframe fill mode for the models
	void ChangeModelFillMode();   

	D3DClass* GetD3DClass() const;
	EditorCamera* GetCamera() const _NOEXCEPT;      // returns a pointer to the main editor's camera
	ShadersContainer* GetShadersContainer() const;
	void SetDeltaTime(float deltaTime) { deltaTime_ = deltaTime; };


	// matrices getters
	const DirectX::XMMATRIX & GetWorldMatrix() const;
	const DirectX::XMMATRIX & GetViewMatrix() const;
	const DirectX::XMMATRIX & GetBaseViewMatrix() const;
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

	InitializeGraphics* pInitGraphics_ = nullptr;
	Settings*           pEngineSettings_ = nullptr;       // engine settings
	D3DClass*           pD3D_ = nullptr;                  // DirectX stuff

	// editor's main camera; ATTENTION: this camera is also used and modified in the ZoneClass
	EditorCamera*       pCamera_ = nullptr;         

	ZoneClass*          pZone_ = nullptr;                 // terrain / clouds / etc.
	ShadersContainer*   pShadersContainer_ = nullptr;     // shaders system
	RenderGraphics*     pRenderGraphics_ = nullptr;       // rendering system

	// models system
	ModelListClass*     pModelList_ = nullptr;     // for making a list of models which are in the scene
	FrustumClass*       pFrustum_ = nullptr;       // for frustum culling
	TextureManagerClass* pTextureManager_ = nullptr;
	ModelInitializerInterface* pModelInitializer_ = nullptr;
	 
	// light
	std::vector<LightClass*> arrDiffuseLights_;
	std::vector<LightClass*> arrPointLights_;
	//LightClass*         pDiffuseLights_ = nullptr;        // array of diffuse light sources (for instance: sun)
	//LightClass*         pPointLights_ = nullptr;          // array of poing light sources (for instance: candle, lightbulb)
	
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
	bool InitializeSprites();
	bool InitializeLight(GraphicsClass* pGraphics);
	bool InitializeGUI(GraphicsClass* pGraphics, HWND hwnd, const DirectX::XMMATRIX & baseViewMatrix); // initialize the GUI of the game/engine (interface elements, text, etc.)
	bool InitializeInternalDefaultModels(GraphicsClass* pGraphics, ID3D11Device* pDevice);

	Model* CreateCube(ID3D11Device* pDevice);
	Model* CreateSphere(ID3D11Device* pDevice);
	Model* CreatePlane(ID3D11Device* pDevice);
	Model* CreateTree(ID3D11Device* pDevice);

	// create the zone's elements
	TerrainClass* CreateTerrain(ID3D11Device* pDevice);
	SkyDomeClass* CreateSkyDome(ID3D11Device* pDevice);
	SkyPlaneClass* CreateSkyPlane(ID3D11Device* pDevice);

	bool SetupModels(const ShadersContainer* pShadersContainer);  // setup some models for using different shaders


private:  // restrict a copying of this class instance
	InitializeGraphics(const InitializeGraphics & obj);
	InitializeGraphics & operator=(const InitializeGraphics & obj);


private:
	// create basic models (cube, sphere, etc.)
	void InitializeDefaultModels(ID3D11Device* pDevice);   // // initialization of the default models which will be used for creation other basic models;   for default models we use a color shader

private:
	// models' creators
	std::unique_ptr<CubeModelCreator>   pCubeCreator_ = std::make_unique<CubeModelCreator>();
	std::unique_ptr<SphereModelCreator> pSphereCreator_ = std::make_unique<SphereModelCreator>();
	std::unique_ptr<PlaneModelCreator>  pPlaneCreator_ = std::make_unique<PlaneModelCreator>();
	std::unique_ptr<TreeModelCreator>   pTreeCreator_ = std::make_unique<TreeModelCreator>();


	GraphicsClass* pGraphics_ = nullptr;
	Settings* pEngineSettings_ = Settings::Get();
};








//////////////////////////////////
// Class name: RenderGraphics
//////////////////////////////////
class RenderGraphics final
{
public:
	RenderGraphics(Settings* pSettings);
	~RenderGraphics();

	bool RenderModels(GraphicsClass* pGraphics, int& renderCount, float deltaTime);
	bool RenderGUI(GraphicsClass* pGraphics, SystemState* systemState);     // render all the GUI parts onto the screen

private:  // restrict a copying of this class instance
	RenderGraphics(const RenderGraphics & obj);
	RenderGraphics & operator=(const RenderGraphics & obj);


private:
	UINT numPointLights_ = 0;     // the number of point light sources on the scene
	std::vector<DirectX::XMFLOAT4> arrPointLightsPositions_;
	std::vector<DirectX::XMFLOAT4> arrPointLightsColors_;
};