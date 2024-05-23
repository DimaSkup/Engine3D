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

// Entity-Component-System
#include "../ECS_Entity/EntityManager.h"

// SHADERS
#include "../EffectsAndShaders/ShadersContainer.h"

// engine stuff
#include "../Engine/macros.h" 
#include "../Engine/Log.h"             // logger
#include "../Engine/SystemState.h"     // contains the current information about the engine
#include "../Engine/Settings.h"

// render stuff
#include "d3dclass.h"                  // for initialization of DirectX stuff
#include "InitializeGraphics.h"        // for initialization of the graphics
#include "RenderGraphics.h"
#include "RenderToTextureClass.h"      // for rendering to some particular texture

// input devices events
#include "../Keyboard/KeyboardEvent.h"
#include "../Mouse/MouseEvent.h"


// mesh, models, game objects and related stuff
#include "../GameObjects/TextureManagerClass.h"
#include "../GameObjects/MeshStorage.h"
#include "../GameObjects/ModelsCreator.h"
#include "../Render/frustumclass.h"              // for frustum culling


// physics / interaction with user
#include "../Physics/IntersectionWithGameObjects.h"

// light
#include "../Light/LightStore.h"

// UI
//#include "textclass.h"               // basic text class (in UI) 
#include "../UI/UserInterfaceClass.h"  // a main UI class

// timers
//#include "../Timers/timerclass.h"
#include "../Timers/timer.h"

// camera
#include "../Camera/cameraclass.h"
#include "../Camera/EditorCamera.h"

// terrain / camera movement handling
#include "ZoneClass.h"



//////////////////////////////////
// Class name: GraphicsClass
//////////////////////////////////
class GraphicsClass final
{
public:
	GraphicsClass();
	~GraphicsClass();

	// main functions
	bool Initialize(HWND hwnd, const SystemState & systemState);
	void Shutdown(void);

	void RenderFrame(SystemState & systemState, 
		const float deltaTime,
		const float totalGameTime);

	// handle events from the keyboard and mouse
	void HandleKeyboardInput(const KeyboardEvent& kbe, const float deltaTime);
	void HandleMouseInput(const MouseEvent& me, const MouseEvent::EventType eventType, const POINT & windowDimensions, const float deltaTime);

	// change render states using keyboard
	void ChangeModelFillMode();   
	void ChangeCullMode();

	D3DClass & GetD3DClass();
	EditorCamera & GetEditorCamera();
	CameraClass & GetCameraForRenderToTexture();
	UserInterfaceClass & GetUserInterface();

	 // get a refference to the storage of all the light sources
	const LightStore & GetLightStore();

	// matrices getters
	const DirectX::XMMATRIX & GetWorldMatrix() const;
	const DirectX::XMMATRIX & GetBaseViewMatrix() const;
	const DirectX::XMMATRIX & GetOrthoMatrix() const;
	//const DirectX::XMMATRIX & GetViewMatrix() const;
	//const DirectX::XMMATRIX & GetProjectionMatrix() const;


	// memory allocation (because we have some XM-data structures)
	void* operator new(std::size_t count);                              // a replaceable allocation function
	void* operator new(std::size_t count, const std::nothrow_t & tag);  // a replaceable non-throwing allocation function
	void* operator new(std::size_t count, void* ptr);                   // a non-allocating placement allocation function
	void operator delete(void* ptr);


private:  // restrict a copying of this class instance
	GraphicsClass(const GraphicsClass & obj);
	GraphicsClass & operator=(const GraphicsClass & obj);
	
private:
	DirectX::XMMATRIX WVO_;                                       // main_world * baseView * ortho
	DirectX::XMMATRIX viewProj_;                                  // view * projection

	DirectX::XMMATRIX worldMatrix_;                               // main_world
	DirectX::XMMATRIX baseViewMatrix_;                            // for UI rendering
	DirectX::XMMATRIX orthoMatrix_;                               // for UI rendering

	ID3D11Device*         pDevice_ = nullptr;
	ID3D11DeviceContext*  pDeviceContext_ = nullptr;

	EntityManager         entityMgr_;
	MeshStorage           meshStorage_;
	ModelsCreator         modelsCreator_;

	Settings              engineSettings_;                        // settings container							   
	D3DClass              d3d_;                                   // DirectX stuff
	
	LightStore            lightsStore_;                           // a storage for light sources data
	UserInterfaceClass    userInterface_;                         // UI/GUI: for work with the graphics user interface (GUI)
	FrustumClass          editorFrustum_;                         // for frustum culling

	Shaders::ShadersContainer shaders_;                           // a struct with shader classes objects

	EditorCamera          editorCamera_;                          // editor's main camera; ATTENTION: this camera is also used and modified in the ZoneClass
	CameraClass           cameraForRenderToTexture_;              // this camera is used for rendering into textures
	ZoneClass             zone_;                                  // terrain / clouds / etc.

	RenderGraphics        renderGraphics_;                        // rendering system
	TextureManagerClass   textureManager_;                        // main container/manager of all the textures
	RenderToTextureClass  renderToTexture_;                       // rendering to some texture
	
	// physics / interaction with user
	IntersectionWithGameObjects* pIntersectionWithGameObjects_ = nullptr;
	
	
	// different boolean flags
	bool                isWireframeMode_ = false;             // do we render everything is the WIREFRAME mode?
	bool                isCullBackMode_ = true;               // do we cull back faces?
	bool                isBeginCheck_ = false;                // a variable which is used to determine if the user has clicked on the screen or not
	bool                isIntersect_ = false;                 // a flag to define if we clicked on some model or not

}; // GraphicsClass






