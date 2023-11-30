////////////////////////////////////////////////////////////////////
// Filename::    ZoneClass.h
// Description:  the zone class is the main wrapper for all of the terrain
//               processing as well as anything that would be related to
//               the terrain. For instance moving the camera around the 
//               terrain, handling sky, trees, foliage, etc.
//
// Created:      10.03.23
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <map>

// common
#include "../Render/d3dclass.h"
#include "../Engine/Settings.h"

// models
#include "../Model/Model.h"
#include "../Model/GameObjectsListClass.h"
#include "../Model/TerrainClass.h"
#include "../Model/SkyPlaneClass.h"
#include "../Model/SkyDomeClass.h"

// shaders for rendering
#include "../ShaderClass/ShadersContainer.h"
#include "../ShaderClass/SkyDomeShaderClass.h"

// camera
#include "../Camera/EditorCamera.h"

// light sources
#include "../Render/lightclass.h"







//////////////////////////////////
// Class name: ZoneClass
//////////////////////////////////
class ZoneClass final
{
public:
	ZoneClass(Settings* pEngineSettings,
		ID3D11DeviceContext* pDeviceContext,
		EditorCamera* pEditorCamera,
		GameObjectsListClass* pGameObjList);
	~ZoneClass();

	bool Initialize();
	bool Render(int & renderCount, 
		D3DClass* pD3D, 
		const float deltaTime, 
		std::vector<LightClass*> & arrDiffuseLightSources,
		std::vector<LightClass*> & arrPointLightSources);

	// handle events from the keyboard/mouse
	void HandleMovementInput(const KeyboardEvent& kbe, const float deltaTime);
	void HandleMovementInput(const MouseEvent& me, const float deltaTime);


private:  // restrict a copying of this class instance
	ZoneClass(const ZoneClass & obj);
	ZoneClass & operator=(const ZoneClass & obj);

private:
	// handle keyboard input to control the zone state (state of the camera, terrain, etc.)
	void HandleZoneControlInput(const KeyboardEvent& kbe);   

	// there are main parts of the zone: sky, terrain, etc.
	void RenderSkyElements(int & renderCount, D3DClass* pD3D);

	void RenderTerrainElements(int & renderCount);

	// render the terrain plane
	void RenderTerrainPlane(int & renderCount);

	void RenderSkyDome(int & renderCount);
	void RenderSkyPlane(int & renderCount, D3DClass* pD3D);

	void RenderPointLightsOnTerrain(ID3D11DeviceContext* pDeviceContext,
		std::vector<LightClass*> & arrDiffuseLightSources,
		std::vector<LightClass*> & arrPointLightSources);

private:
	Settings*             pEngineSettings_ = nullptr;      // a pointer to the engine settings object
	ID3D11DeviceContext*  pDeviceContext_ = nullptr;
	EditorCamera*         pEditorCamera_ = nullptr;        // ATTENTION: this camera object is initialized in the GraphicsClass object but we have this local pointer for handy using within the ZoneClass
	FrustumClass*         pFrustum_ = nullptr;
	GameObjectsListClass* pGameObjList_ = nullptr;         // a pointer to the game objects list 
	DataContainerForShaders* pDataContainer_ = nullptr;

	GameObject*  pTerrainGameObj_ = nullptr;               // a pointer to the whole terrain game object
	GameObject*  pSkyPlaneGameObj_ = nullptr;              // a pointer to the sky plane game object
	GameObject*  pSkyDomeGameObj_ = nullptr;               // a pointer to the sky dome game object

	float deltaTime_ = 0.0f;                               // time between frames
	float cameraHeightOffset_ = 0.0f;                      // camera's height above the terrain

	// states
	bool showCellLines_ = false;                           // a boolean variable indicating whether the bounding boxes around the terrain cells should be drawn or not
	bool heightLocked_ = false;                            // a variable for indicating if the camera should be locked to the height of the terrain or not
};
