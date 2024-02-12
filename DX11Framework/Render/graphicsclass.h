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
#include "../ShaderClass/ShadersContainer.h"
#include "../ShaderClass/colorshaderclass.h"           // for rendering models with only colour but not textures
#include "../ShaderClass/textureshaderclass.h"         // for texturing models

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
#include "../Render/frustumclass.h"              // for frustum culling
#include "../GameObjects/TextureManagerClass.h"

// physics / interaction with user
#include "../Physics/IntersectionWithGameObjects.h"

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
	EditorCamera* GetCamera() const;      // returns a pointer to the main editor's camera

	const std::vector<LightClass*> & GetDiffuseLigthsArr();    // get an array of diffuse light sources (for instance: sun)
	const std::vector<LightClass*> & GetPointLightsArr();     // get an array of point light sources (for instance: candle, lightbulb)



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
	DirectX::XMMATRIX viewMatrix_;
	DirectX::XMMATRIX projectionMatrix_;
	DirectX::XMMATRIX viewProj_;                                  // view * projection
	DirectX::XMMATRIX WVO_;                                       // world * baseView * ortho

	D3DClass             d3d_;  // DirectX stuff
	ColorShaderClass     colorShader_;
	TextureShaderClass   textureShader_;
	ModelsStore          models_;

	
	Settings*             pEngineSettings_ = nullptr;             // engine settings
	                   
	std::shared_ptr<SystemState> pSystemState_;
	
	EditorCamera*         pCamera_ = nullptr;                     // editor's main camera; ATTENTION: this camera is also used and modified in the ZoneClass
	CameraClass*          pCameraForRenderToTexture_ = nullptr;   // this camera is used for rendering into textures

	ZoneClass*                pZone_ = nullptr;                   // terrain / clouds / etc.

	RenderGraphics*           pRenderGraphics_ = nullptr;         // rendering system
	RenderToTextureClass*     pRenderToTexture_ = nullptr;        // rendering to some texture

	// game objects system
	FrustumClass*         pFrustum_ = nullptr;                    // for frustum culling
	std::unique_ptr<TextureManagerClass>  pTextureManager_;

	// physics / interaction with user
	IntersectionWithGameObjects* pIntersectionWithGameObjects_ = nullptr;

	 
	// light
	std::vector<LightClass*> arrDiffuseLights_;             // array of diffuse light sources (for instance: sun)
	std::vector<LightClass*> arrPointLights_;               // array of point light sources (for instance: candle, lightbulb)
	
	// UI
	UserInterfaceClass* pUserInterface_ = nullptr;          // for work with the graphics user interface (GUI)

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

	bool InitializeDirectX(HWND hwnd);   // initialized all the DirectX stuff
	bool InitializeTerrainZone();        // initialize the main wrapper for all of the terrain processing 
	bool InitializeShaders(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd);   // initialize all the shaders (color, texture, light, etc.)
	bool InitializeScene(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hwnd);

	bool InitializeModels(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);             // initialize all the models on the scene
	bool InitializeSprites();
	bool InitializeLight();

	bool InitializeGUI(D3DClass & d3d, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext); // initialize the GUI of the game/engine (interface elements, text, etc.)

	bool InitializeInternalDefaultModels();


private:  // restrict a copying of this class instance
	InitializeGraphics(const InitializeGraphics & obj);
	InitializeGraphics & operator=(const InitializeGraphics & obj);


private:

	// local copies of pointers to the graphics class, device, and device context
	GraphicsClass*       pGraphics_ = nullptr;
	Settings*            pEngineSettings_ = Settings::Get();

}; // class InitializeGraphics



//////////////////////////////////
// Class name: RenderGraphics
//////////////////////////////////
class RenderGraphics final
{
public:
	RenderGraphics(GraphicsClass* pGraphics,
		Settings* pSettings,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext);
	~RenderGraphics();

	bool Render(D3DClass & d3d, 
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX & WVO,  // world * basic_view * ortho
		HWND hwnd,
		SystemState & systemState, 
		const float deltaTime,
		const int gameCycles,
		ModelsStore & models);

	// render all the 2D / 3D models onto the screen
	bool RenderModels(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext, 
		SystemState & systemState, 
		const float deltaTime,
		ModelsStore & models);

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
	DataContainerForShaders* pDataForShaders_ = nullptr;             

	UINT windowWidth_ = 0;
	UINT windowHeight_ = 0;

	float localTimer_ = 0.0f;
}; // class RenderGraphics