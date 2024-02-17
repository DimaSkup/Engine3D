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



// SHADERS
#include "../ShaderClass/colorshaderclass.h"           // for rendering models with only colour but not textures
#include "../ShaderClass/textureshaderclass.h"         // for texturing models
#include "../ShaderClass/LightShaderClass.h"           // for light effect on models
#include "../ShaderClass/PointLightShaderClass.h"      // for point lighting

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


// models, game objects and related stuff
#include "../GameObjects/ModelsStore.h"
#include "../GameObjects/ModelsCreator.h"
#include "../Render/frustumclass.h"              // for frustum culling
#include "../GameObjects/TextureManagerClass.h"

// physics / interaction with user
#include "../Physics/IntersectionWithGameObjects.h"

// light
#include "../Render/LightStore.h"

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
	struct ShadersContainer
	{
		ColorShaderClass      colorShader_;
		TextureShaderClass    textureShader_;
		LightShaderClass      lightShader_;
		PointLightShaderClass pointLightShader_;
	};

public:
	friend InitializeGraphics;              // for initialization of the graphics
	friend RenderGraphics;                  // for rendering of the graphics

public:
	GraphicsClass(void);
	~GraphicsClass(void);

	// main functions
	bool Initialize(HWND hwnd, const SystemState & systemState);
	void Shutdown(void);

	void RenderFrame(HWND hwnd, 
		SystemState & systemState, 
		const float deltaTime,
		const int gameCycles);

	// handle events from the keyboard and mouse
	void HandleKeyboardInput(const KeyboardEvent& kbe, const float deltaTime);
	void HandleMouseInput(const MouseEvent& me, const MouseEvent::EventType eventType, const POINT & windowDimensions, const float deltaTime);

	

	// toggling on and toggling off the wireframe fill mode for the models
	void ChangeModelFillMode();   

	D3DClass & GetD3DClass();
	EditorCamera & GetEditorCamera();
	CameraClass & GetCameraForRenderToTexture();

	 // get a refference to the storage of all the light sources
	const LightStore & GraphicsClass::GetLightStore();

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
	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX baseViewMatrix_;                            // for UI rendering
	DirectX::XMMATRIX orthoMatrix_;                               // for UI rendering
	//DirectX::XMMATRIX viewMatrix_;
	//DirectX::XMMATRIX projectionMatrix_;
	DirectX::XMMATRIX viewProj_;                                  // view * projection
	DirectX::XMMATRIX WVO_;                                       // world * baseView * ortho

	D3DClass              d3d_;                                   // DirectX stuff
	Settings              engineSettings_;                        // settings container							   

	ShadersContainer      shaders_;                               // a struct with shader classes objects
	ModelsStore           models_;                                // models data storage/container
	LightStore            lightsStore_;                           // a storage for light sources data
	
	UserInterfaceClass    userInterface_;                         // UI/GUI: for work with the graphics user interface (GUI)
	
	
	           
	std::shared_ptr<SystemState> pSystemState_;
	
	EditorCamera          editorCamera_;                          // editor's main camera; ATTENTION: this camera is also used and modified in the ZoneClass
	CameraClass           cameraForRenderToTexture_;              // this camera is used for rendering into textures
	ZoneClass*                pZone_ = nullptr;                   // terrain / clouds / etc.

	RenderGraphics*           pRenderGraphics_ = nullptr;         // rendering system
	RenderToTextureClass*     pRenderToTexture_ = nullptr;        // rendering to some texture

	// game objects system
	FrustumClass*         pFrustum_ = nullptr;                    // for frustum culling
	std::unique_ptr<TextureManagerClass>  pTextureManager_;

	// physics / interaction with user
	IntersectionWithGameObjects* pIntersectionWithGameObjects_ = nullptr;
	
	
	// different boolean flags
	bool                wireframeMode_ = false;             // do we render everything is the WIREFRAME mode?
	bool                isBeginCheck_ = false;              // a variable which is used to determine if the user has clicked on the screen or not
	bool                isIntersect_ = false;               // a flag to define if we clicked on some model or not

}; // GraphicsClass




//////////////////////////////////
// Class name: InitializeGraphics
//////////////////////////////////
class InitializeGraphics final
{
public:
	InitializeGraphics(GraphicsClass* pGraphics);

	// initialized all the DirectX stuff
	bool InitializeDirectX(HWND hwnd,
		const UINT windowWidth,
		const UINT windowHeight,
		const float nearZ,        // near Z-coordinate of the screen/frustum
		const float farZ,         // far Z-coordinate of the screen/frustum (screen depth)
		const bool vSyncEnabled,
		const bool isFullScreenMode);   

	// initialize all the shaders (color, texture, light, etc.)
	bool InitializeShaders(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		GraphicsClass::ShadersContainer & shaders);

	bool InitializeScene(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd,
		ModelsStore & modelsStore,
		Settings & settings,
		const UINT windowWidth,
		const UINT windowHeight,
		const float nearZ,        // near Z-coordinate of the screen/frustum
		const float farZ,         // far Z-coordinate of the screen/frustum (screen depth)
		const float fovDegrees,
		const float cameraSpeed,         // camera movement speed
		const float cameraSensitivity);  // camera rotation speed
	

	// initialize all the models on the scene
	bool InitializeModels(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		ModelsStore & modelsStore,
		Settings & settings,
		const float farZ);

	// initialize the main wrapper for all of the terrain processing 
	bool InitializeTerrainZone(
		Settings & settings,
		const float farZ);                            // screen depth


	bool InitializeSprites(const UINT screenWidth, const UINT screenHeight);
	bool InitializeLight(Settings & settings);

	// initialize the GUI of the game/engine (interface elements, text, etc.)
	bool InitializeGUI(D3DClass & d3d, 
		Settings & settings,
		ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext,
		const UINT windowWidth,
		const UINT windowHeight); 

private:  // restrict a copying of this class instance
	InitializeGraphics(const InitializeGraphics & obj);
	InitializeGraphics & operator=(const InitializeGraphics & obj);


private:

	// local copies of pointers to the graphics class, device, and device context
	GraphicsClass*       pGraphics_ = nullptr;

}; // class InitializeGraphics



//////////////////////////////////
// Class name: RenderGraphics
//////////////////////////////////
class RenderGraphics final
{
public:
	RenderGraphics(GraphicsClass* pGraphics,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const Settings & settings);
	~RenderGraphics();

	bool Render(D3DClass & d3d, 
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & WVO,  // world * basic_view * ortho
		//const DirectX::XMMATRIX & viewMatrix,
		//const DirectX::XMMATRIX & projMatrix,
		const DirectX::XMMATRIX & viewProj,   // view * projection
		HWND hwnd,
		SystemState & systemState, 
		const float deltaTime,
		const int gameCycles,
		ModelsStore & models,
		const DirectX::XMFLOAT3 & cameraPos);

	// render all the 2D / 3D models onto the screen
	bool RenderModels(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		SystemState & systemState,
		ModelsStore & models,
		//const DirectX::XMMATRIX & viewMatrix,
		//const DirectX::XMMATRIX & projMatrix,
		const DirectX::XMMATRIX & viewProj,   // view * projection
		const DirectX::XMFLOAT3 & cameraPos,
		const float deltaTime);

	// render all the GUI parts onto the screen
	bool RenderGUI(D3DClass & d3d,
		ID3D11DeviceContext* pDeviceContext,
		SystemState & systemState,
		const DirectX::XMMATRIX & WVO,
		const float deltaTime,
		const int gameCycles);

private:  // restrict a copying of this class instance
	RenderGraphics(const RenderGraphics & obj);
	RenderGraphics & operator=(const RenderGraphics & obj);

private:
#if 0
	void SetupRenderTargetPlanes();
	void SetupGameObjectsForRenderingToTexture();

	void RenderRenderableGameObjects();
	void RenderReflectionPlane();

	void UpdateGUIData(SystemState* pSystemState);
	void Render2DSprites(const float deltaTime);
	void RenderPickedGameObjToTexture(RenderableGameObject* pGameObj);
	void RenderSceneToTexture(const std::vector<RenderableGameObject*> & gameObjArr);
	void RenderReflectedSceneToTexture(const std::vector<RenderableGameObject*> & gameObjArr, const GameObject* pRelfectionPlane);

	// a function for dynamic modification game objects' positions, rotation, etc. during the rendering of the scene
	void MoveRotateScaleGameObjects(GameObject* pGameObj,
		const float t,
		const UINT modelIndex);
#endif
	
private:   // MIRROR / SHADOW DEMO

	void SetupRoom();
	UINT SetupWall();
	UINT SetupFloor(UINT planeIndex);

	void DrawRoom();
	void DrawSphere();
	void DrawMirror();

	void MarkMirrorOnStencil();

	void DrawFloorReflection();
	void DrawSphereReflection();

private:
	// a local copies of some pointers for easier using of it
	GraphicsClass*           pGraphics_ = nullptr;             

	float localTimer_ = 0.0f;

}; // class RenderGraphics