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
#include "../Engine/SystemState.h"     // contains the current information about the engine

// camera
#include "../Camera/EditorCamera.h"
#include "../Render/frustumclass.h"

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
		DataContainerForShaders* pDataContainer,
		const std::shared_ptr<SystemState> & pSystemState);
	~ZoneClass();

	bool Initialize();
	bool Render(D3DClass* pD3D, 
		const float deltaTime, 
		const float timerValue);

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
	void RenderSkyElements(D3DClass* pD3D);

	void RenderTerrainElements();

	// render the terrain plane
	void RenderTerrainPlane();

	void RenderSkyDome();
	void RenderSkyPlane();

	void ComputePointLightSpheresOnTerrain();

private:
	Settings*             pEngineSettings_ = nullptr;      // a pointer to the engine settings object
	std::shared_ptr<SystemState>  pSystemState_;           // contains the current information about the engine
	ID3D11DeviceContext*  pDeviceContext_ = nullptr;
	EditorCamera*         pEditorCamera_ = nullptr;        // ATTENTION: this camera object is initialized in the GraphicsClass object but we have this local pointer for handy using within the ZoneClass
	FrustumClass*         pFrustum_ = nullptr;
	DataContainerForShaders* pDataForShaders_ = nullptr;

	float deltaTime_ = 0.0f;                               // time between frames
	float cameraHeightOffset_ = 0.0f;                      // camera's height above the terrain
	float localTimer_ = 0.0f;

	// states
	bool showCellLines_ = false;                           // a boolean variable indicating whether the bounding boxes around the terrain cells should be drawn or not
	bool heightLocked_ = false;                            // a variable for indicating if the camera should be locked to the height of the terrain or not

	UINT numPointLights_ = 0;                              // the number of point lights on the terrain
};
