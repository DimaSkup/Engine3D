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
#include <DirectXCollision.h>


// Entity-Component-System
#include "Entity/EntityManager.h"

// engine stuff
#include "../Engine/SystemState.h"     // contains the current information about the engine
#include "../Engine/Settings.h"

// render stuff
#include "Render.h"
#include "InitializeGraphics.h"        // for initialization of the graphics
#include "RenderToTextureClass.h"      // for rendering to some particular texture

// input devices events
#include "../Keyboard/KeyboardEvent.h"
#include "../Mouse/MouseEvent.h"


// mesh, models, game objects and related stuff
#include "../GameObjects/TextureManager.h"
#include "../GameObjects/MeshStorage.h"
#include "../GameObjects/ModelsCreator.h"
#include "../Render/frustumclass.h"              // for frustum culling


// physics / interaction with user
#include "../Physics/IntersectionWithGameObjects.h"

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

	// restrict a copying of this class instance
	GraphicsClass(const GraphicsClass& obj) = delete;
	GraphicsClass& operator=(const GraphicsClass& obj) = delete;

	// main functions
	bool Initialize(HWND hwnd, SystemState& systemState);
	void Shutdown();

	void UpdateScene(
		SystemState& systemState,
		const float deltaTime,
		const float totalGameTime);

	// ------------------------------------
	// render related methods

	void RenderFrame(
		SystemState & systemState, 
		const float deltaTime,
		const float totalGameTime);

	// ----------------------------------

	void ComputeFrustumCulling(SystemState& sysState);

	// handle events from the keyboard and mouse
	void HandleKeyboardInput(const KeyboardEvent& kbe, const float deltaTime);
	void HandleMouseInput(const MouseEvent& me, const MouseEvent::EventType eventType, const POINT & windowDimensions, const float deltaTime);

	// change render states using keyboard
	void ChangeModelFillMode();   
	void ChangeCullMode();


	// ---------------------------------------
	// INLINE GETTERS

	inline D3DClass& GetD3DClass() { return d3d_; }
	inline EditorCamera& GetEditorCamera() { return editorCamera_; }
	inline CameraClass& GetCameraForRenderToTexture() { return cameraForRenderToTexture_; }
	inline UserInterfaceClass& GetUserInterface() { return userInterface_; }

	// matrices getters
	inline const DirectX::XMMATRIX& GetWorldMatrix()    const { return worldMatrix_; }
	inline const DirectX::XMMATRIX& GetBaseViewMatrix() const { return baseViewMatrix_; }
	inline const DirectX::XMMATRIX& GetOrthoMatrix()    const { return orthoMatrix_; }
	//const DirectX::XMMATRIX & GetViewMatrix() const;
	//const DirectX::XMMATRIX & GetProjectionMatrix() const;

	// memory allocation (because we have some XM-data structures)
	void* operator new(std::size_t count);                              // a replaceable allocation function
	void* operator new(std::size_t count, const std::nothrow_t & tag);  // a replaceable non-throwing allocation function
	void* operator new(std::size_t count, void* ptr);                   // a non-allocating placement allocation function
	void operator delete(void* ptr);


private: 
	// private initialization API
	void InitCamerasHelper(InitializeGraphics& init, Settings& settings);
	void InitSceneHelper(InitializeGraphics& init, Settings& settings);
	void InitGuiHelper(InitializeGraphics& init, Settings& settings);

	// private rendering API
	void Render3D();

	void RenderEntts(const std::vector<EntityID>& enttsIds);

	void SetupLightsForFrame(
		const ECS::LightSystem& lightSys,
		std::vector<Render::DirLight>& outDirLights,
		std::vector<Render::PointLight>& outPointLights,
		std::vector<Render::SpotLight>& outSpotLights);


	void GetTexSRVsForMeshAndEntts(
		const std::vector<EntityID>& inEntts,          // in: entts sorted by meshes ids
		const std::vector<TexIDsArr>& texIdsArrPerMesh,
		const std::vector<ptrdiff_t> enttsPerMesh,
		const size meshesCount,
		ECS::TexturesSystem& texSys,
		std::vector<EntityID>& outEntts,               // out: entts REsorted in order [first: textured_with_mesh_textures; after: textured_with_own_textures]
		std::vector<SRV*>& outTexSRVs,
		std::vector<u32>& outNumInstancesPerTexSet);

	void GetTexIDsForMeshAndEntts(
		const std::vector<EntityID>& inEntts,
		const std::vector<TexID>& meshTexIds,
		ECS::TexturesSystem& texSys,
		std::vector<EntityID>& outEntts,       // entts sorted in order [first: textured_with_mesh_textures; after: textured_with_own_textures]
		std::vector<TexID>& outTexIds,
		std::vector<u32>& outNumInstancesPerTexSet);

	void PrepareTexTransformsForRendering(
		ECS::EntityManager& entityMgr,
		const std::set<EntityID>& enttsIDs,
		std::vector<XMMATRIX>& outTexTransforms);

	void PrepareTexturesSRV_ToRender(
		const std::vector<TextureClass*>& textures,
		std::vector<ID3D11ShaderResourceView* const*>& outTexturesSRVs);

	void GetEnttsWorldMatricesForRendering(
		const std::vector<EntityID>& visibleEntts,
		const std::vector<EntityID>& enttsIDsToGetMatrices,
		const std::vector<DirectX::XMMATRIX>& inWorldMatrices,   // world matrices of all the currently visible entts
		std::vector<DirectX::XMMATRIX>& outWorldMatrices);

	
private:
	DirectX::XMMATRIX WVO_ = DirectX::XMMatrixIdentity();             // main_world * baseView * ortho
	DirectX::XMMATRIX viewProj_ = DirectX::XMMatrixIdentity();        // view * projection

	DirectX::XMMATRIX worldMatrix_ = DirectX::XMMatrixIdentity();     // main_world
	DirectX::XMMATRIX baseViewMatrix_ = DirectX::XMMatrixIdentity();  // for UI rendering
	DirectX::XMMATRIX orthoMatrix_ = DirectX::XMMatrixIdentity();     // for UI rendering

	ID3D11Device*         pDevice_ = nullptr;
	ID3D11DeviceContext*  pDeviceContext_ = nullptr;

	ECS::EntityManager    entityMgr_;
	MeshStorage           meshStorage_;

	Settings              engineSettings_;                        // settings container							   

	UserInterfaceClass    userInterface_;                         // UI/GUI: for work with the graphics user interface (GUI)
	std::vector<BoundingFrustum> frustums_;

	D3DClass              d3d_;
	Render::Render        render_;                                // rendering module
	EditorCamera          editorCamera_;                          // editor's main camera; ATTENTION: this camera is also used and modified in the ZoneClass
	CameraClass           cameraForRenderToTexture_;              // this camera is used for rendering into textures
	ZoneClass             zone_;                                  // terrain / clouds / etc.

	TextureManager        textureManager_;                        // main container/manager of all the textures
	RenderToTextureClass  renderToTexture_;                       // rendering to some texture
	
	// physics / interaction with user
	IntersectionWithGameObjects* pIntersectionWithGameObjects_ = nullptr;
	
	
	// different boolean flags
	bool isWireframeMode_ = false;             // do we render everything is the WIREFRAME mode?
	bool isCullBackMode_ = true;               // do we cull back faces?
	bool isBeginCheck_ = false;                // a variable which is used to determine if the user has clicked on the screen or not
	bool isIntersect_ = false;                 // a flag to define if we clicked on some model or not

};






