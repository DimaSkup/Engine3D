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
#include "../ShaderClass/ModelsToShaderMediator.h"
#include "../ShaderClass/DataContainerForShaders.h"


// models, game objects and related stuff
#include "../Model/GameObjectCreator.h"
#include "../2D/SpriteClass.h"
#include "../2D/character2d.h"
#include "../Model/GameObjectsListClass.h"       // for making a list of game objects which are in the scene
#include "../Render/frustumclass.h"              // for frustum culling
#include "../Model/ModelInitializerInterface.h"  // a common interface for models' initialization
#include "../Model/GameObject.h"
#include "../Model/TextureManagerClass.h"

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
	bool Initialize(HWND hwnd);
	void Shutdown(void);
	bool RenderFrame(SystemState* systemState, HWND hwnd, float deltaTime);

	// handle events from the keyboard and mouse
	void HandleKeyboardInput(const KeyboardEvent& kbe, HWND hwnd, const float deltaTime);
	void HandleMouseInput(const MouseEvent& me, const POINT & windowDimensions, const float deltaTime);

	

	// toggling on and toggling off the wireframe fill mode for the models
	void ChangeModelFillMode();   

	D3DClass* GetD3DClass() const;
	EditorCamera* GetCamera() const;      // returns a pointer to the main editor's camera
	ShadersContainer* GetShadersContainer() const;
	GameObjectsListClass* GetGameObjectsList() const;
	void SetDeltaTime(float deltaTime) { deltaTime_ = deltaTime; };

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
	bool RenderScene(SystemState* systemState, HWND hwnd);   // render all the stuff on the scene
	
private:
	DirectX::XMMATRIX worldMatrix_;
	DirectX::XMMATRIX viewMatrix_;
	DirectX::XMMATRIX baseViewMatrix_;                            // for UI rendering
	DirectX::XMMATRIX projectionMatrix_;
	DirectX::XMMATRIX orthoMatrix_;

	InitializeGraphics*   pInitGraphics_ = nullptr;
	Settings*             pEngineSettings_ = nullptr;             // engine settings
	D3DClass*             pD3D_ = nullptr;                        // DirectX stuff

	
	EditorCamera*         pCamera_ = nullptr;                     // editor's main camera; ATTENTION: this camera is also used and modified in the ZoneClass
	CameraClass*          pCameraForRenderToTexture_ = nullptr;   // this camera is used for rendering into textures

	ZoneClass*                pZone_ = nullptr;                   // terrain / clouds / etc.
	ShadersContainer*         pShadersContainer_ = nullptr;       // contains all the pointers to the shaders
	ModelToShaderMediator*   pModelsToShaderMediator_ = nullptr;  // a mediator between models and shaders; this mediator is used for calling the shader rendering function within the model's class;

	RenderGraphics*           pRenderGraphics_ = nullptr;         // rendering system
	RenderToTextureClass*     pRenderToTexture_ = nullptr;        // rendering to some texture

	// game objects system
	GameObjectsListClass* pGameObjectsList_ = nullptr;            // for making a list of game objects which are in the scene
	FrustumClass*         pFrustum_ = nullptr;                    // for frustum culling
	std::unique_ptr<TextureManagerClass>  pTextureManager_;
	ModelInitializerInterface* pModelInitializer_ = nullptr;

	// physics / interaction with user
	IntersectionWithGameObjects* pIntersectionWithGameObjects_ = nullptr;

	 
	// light
	std::vector<LightClass*> arrDiffuseLights_;             // array of diffuse light sources (for instance: sun)
	std::vector<LightClass*> arrPointLights_;               // array of point light sources (for instance: candle, lightbulb)
	
	// UI
	UserInterfaceClass* pUserInterface_ = nullptr;          // for work with the graphics user interface (GUI)

	float               deltaTime_ = 0.0f;                  // time between frames

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
	bool InitializeShaders(HWND hwnd);   // initialize all the shaders (color, texture, light, etc.)
	bool InitializeScene(HWND hwnd);

	bool InitializeModels();             // initialize all the models on the scene
	bool InitializeSprites();
	bool InitializeLight();
	bool InitializeGUI(HWND hwnd, const DirectX::XMMATRIX & baseViewMatrix); // initialize the GUI of the game/engine (interface elements, text, etc.)
	bool InitializeInternalDefaultModels();

	// create usual default game objects (models)
	GameObject* CreateLine3D(const DirectX::XMFLOAT3 & startPos, const DirectX::XMFLOAT3 & endPos);
	GameObject* CreateTriangle();
	GameObject* CreateCube(GameObject* pOriginCube = nullptr);
	GameObject* CreateSphere(GameObject* pOriginSphere = nullptr);
	GameObject* CreatePlane();
	GameObject* CreateTree();
	GameObject* Create2DSprite(const std::string & setupFilename, const std::string & spriteID, const POINT & renderAtPos);
	GameObject* CreateGameObjectFromFile(const std::string & modelFilename, const std::string & gameObjID = "");

	// create the zone's elements
	GameObject* CreateTerrain();
	GameObject* CreateSkyDome();
	GameObject* CreateSkyPlane();

	bool SetupModels(const ShadersContainer* pShadersContainer);  // setup some models for using different shaders


private:  // restrict a copying of this class instance
	InitializeGraphics(const InitializeGraphics & obj);
	InitializeGraphics & operator=(const InitializeGraphics & obj);


private:
	// initialization of the default models which will be used for creation other basic models;
	void InitializeDefaultModels();  

private:
	// game objects' creators
	std::unique_ptr<GameObjectCreator<Sphere>>      pSphereCreator_;
	std::unique_ptr<GameObjectCreator<Cube>>        pCubeCreator_;
	std::unique_ptr<GameObjectCreator<Plane>>       pPlaneCreator_;
	std::unique_ptr<GameObjectCreator<CustomModel>> pCustomGameObjCreator_;
	std::unique_ptr<GameObjectCreator<SpriteClass>> p2DSpriteCreator_;

	
	// local copies of pointers to the graphics class, device, and device context
	GraphicsClass*       pGraphics_ = nullptr;
	ID3D11Device*        pDevice_ = nullptr;
	ID3D11DeviceContext* pDeviceContext_ = nullptr;
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
		ID3D11DeviceContext* pDeviceContext,
		DataContainerForShaders* pDataContainerForShaders);
	~RenderGraphics();

	bool Render(HWND hwnd, SystemState* pSystemState, const float deltaTime);

	bool RenderModels(int & renderCount, const float deltaTime);

	// render all the GUI parts onto the screen
	bool RenderGUI(SystemState* systemState, const float deltaTime);

	inline void SetCurrentlyPickedGameObj(GameObject* pPickedGameObj) 
	{ 
		pCurrentPickedGameObj = pPickedGameObj; 
	}

private:  // restrict a copying of this class instance
	RenderGraphics(const RenderGraphics & obj);
	RenderGraphics & operator=(const RenderGraphics & obj);

private:
	void SetupRenderTargetPlanes();
	void SetupGameObjectsForRenderingToTexture();

	void RenderGameObjectsFromList(const std::map<std::string, GameObject*> gameObjRenderList, int & renderCount);
	void RenderModelsObjects(int & renderCount);
	void RenderReflectionPlane(int & renderCount);

	void UpdateGUIData(SystemState* pSystemState);

	void Render2DSprites(const float deltaTime);

	void RenderPickedGameObjToTexture(GameObject* pGameObj);
	void RenderSceneToTexture(const std::vector<GameObject*> & gameObjArr);
	void RenderReflectedSceneToTexture(const std::vector<GameObject*> & gameObjArr, const GameObject* pRelfectionPlane);

	// a function for dynamic modification game objects' positions, rotation, etc. during the rendering of the scene
	void MoveRotateScaleGameObjects(GameObject* pGameObj,
		const float t,
		const UINT modelIndex);

	
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
	GameObject* pSphereForReflection_ = nullptr;
	GameObject* pMirrorPlane_ = nullptr;

private:
	// a local copies of some pointers for easier using of it
	ID3D11Device*            pDevice_ = nullptr;
	ID3D11DeviceContext*     pDeviceContext_ = nullptr;
	GraphicsClass*           pGraphics_ = nullptr;             
	DataContainerForShaders* pDataForShaders_ = nullptr;
	GameObject*              pCurrentPickedGameObj = nullptr;                

	UINT windowWidth_ = 0;
	UINT windowHeight_ = 0;

	// plane objects which will be an another render target to render to
	GameObject* pPlane2DRenderTargetObj_ = nullptr;  // for 2D
	GameObject* pPlane3DRenderTargetObj_ = nullptr;  // for 3D

	std::vector<GameObject*> wallPlanesArr_;
	std::vector<GameObject*> floorPlanesArr_;
	std::vector<GameObject*> renderToTextureGameObjArr_;

	float localTimer_ = 0.0f;
	const float inv_thousand_ = 1.0f / 1000.0f;   // is used to update the local time value

}; // class RenderGraphics