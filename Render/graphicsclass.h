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
#include "RenderToTextureClass.h"      // for rendering to some particular texture

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
	bool RenderFrame(SystemState* systemState, HWND hwnd, float deltaTime);

	// handle events from the keyboard and mouse
	void HandleKeyboardInput(const KeyboardEvent& kbe, HWND hwnd, const float deltaTime);
	void HandleMouseInput(const MouseEvent& me, const POINT & windowDimensions, const float deltaTime);

	// functions for the picking: 
	// 1. the first one is the general intersection check that
	//    forms the vector for checking the intersection and then calls the specific type
	//    of intersection check required. 
	//    If an intersection occured with some model we return a pointer to this model;
	//
	// 2. the second function is the ray-sphere intersection check function; this function
	//    is called by TestIntersection. For other intersection tests such as ray-triangle,
	//    ray-rectangle, and so forth you would add them here
	Model*  TestIntersection(const int mouseX, const int mouseY, const POINT & windowDimensions);
	bool RaySphereIntersect(const DirectX::XMVECTOR & rayOrigin,
		const DirectX::XMVECTOR & rayDirection, 
		const float radius);

	// toggling on and toggling off the wireframe fill mode for the models
	void ChangeModelFillMode();   

	D3DClass* GetD3DClass() const;
	EditorCamera* GetCamera() const;      // returns a pointer to the main editor's camera
	ShadersContainer* GetShadersContainer() const;
	ModelListClass* GetModelsList() const;
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
	bool RenderScene(SystemState* systemState, HWND hwnd);   // render all the stuff on the scene
	
private:
	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX viewMatrix_;
	DirectX::XMMATRIX baseViewMatrix_;   // for UI rendering
	DirectX::XMMATRIX projectionMatrix_;
	DirectX::XMMATRIX orthoMatrix_;

	InitializeGraphics*   pInitGraphics_ = nullptr;
	Settings*             pEngineSettings_ = nullptr;       // engine settings
	D3DClass*             pD3D_ = nullptr;                  // DirectX stuff

	// editor's main camera; ATTENTION: this camera is also used and modified in the ZoneClass
	EditorCamera*         pCamera_ = nullptr;   
	CameraClass*          pCameraForRenderToTexture_ = nullptr;  // this camera is used for rendering into textures

	ZoneClass*            pZone_ = nullptr;                 // terrain / clouds / etc.
	ShadersContainer*     pShadersContainer_ = nullptr;     // shaders system
	RenderGraphics*       pRenderGraphics_ = nullptr;       // rendering system
	RenderToTextureClass* pRenderToTexture_ = nullptr;      // rendering to some texture

	// models system
	ModelListClass*       pModelList_ = nullptr;            // for making a list of models which are in the scene
	FrustumClass*         pFrustum_ = nullptr;              // for frustum culling
	TextureManagerClass*  pTextureManager_ = nullptr;
	ModelInitializerInterface* pModelInitializer_ = nullptr;
	 
	// light
	std::vector<LightClass*> arrDiffuseLights_;             // array of diffuse light sources (for instance: sun)
	std::vector<LightClass*> arrPointLights_;               // array of poing light sources (for instance: candle, lightbulb)
	
	// UI
	UserInterfaceClass* pUserInterface_ = nullptr;          // for work with the graphics user interface (GUI)

	float               deltaTime_ = 0.0f;                  // time between frames

	// different boolean flags
	bool                wireframeMode_ = false;
	bool                isBeginCheck_ = false;              // a variable which is used to determine if the user has clicked on the screen or not
	bool                isIntersect_ = false;               // a flat to define if we clicked on some model or not
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
	RenderGraphics(GraphicsClass* pGraphics, Settings* pSettings);
	~RenderGraphics();

	bool RenderModels(GraphicsClass* pGraphics, HWND hwnd, int & renderCount, float deltaTime);

	// render all the GUI parts onto the screen
	bool RenderGUI(GraphicsClass* pGraphics, 
		SystemState* systemState, 
		const float deltaTime);

	inline void SetCurrentlyPickedModel(Model* pModel) { pCurrentPickedModel = pModel; }

private:  // restrict a copying of this class instance
	RenderGraphics(const RenderGraphics & obj);
	RenderGraphics & operator=(const RenderGraphics & obj);

private:
	void RenderModelsObjects(ID3D11DeviceContext* pDeviceContext, int & renderCount);

	void Render2DSprites(ID3D11DeviceContext* pDeviceContext,
		GraphicsClass* pGraphics,
		const float deltaTime);

	void RenderPickedModelToTexture(ID3D11DeviceContext* pDeviceContext, Model* pModel);
	bool RenderSceneToTexture(ID3D11DeviceContext* pDeviceContext, Model* pModel, const float rotation);

private:
	GraphicsClass* pGraphics_ = nullptr;
	Model* pCurrentPickedModel = nullptr;   // a pointer to the currently picked model

	UINT numPointLights_ = 0;     // the number of point light sources on the scene
	std::vector<DirectX::XMFLOAT4> arrPointLightsPositions_;
	std::vector<DirectX::XMFLOAT4> arrPointLightsColors_;
	UINT windowWidth_ = 0;
	UINT windowHeight_ = 0;

	
};