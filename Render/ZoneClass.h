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

// camera
#include "../Camera/EditorCamera.h"

// models
#include "../Model/Model.h"
#include "../Model/modellistclass.h"
#include "../Model/TerrainClass.h"
#include "../Model/SkyPlaneClass.h"






//////////////////////////////////
// Class name: ZoneClass
//////////////////////////////////
class ZoneClass final
{
public:
	ZoneClass(Settings* pEngineSettings);
	~ZoneClass();

	bool Initialize();
	void Render(int & renderCount, D3DClass* pD3D, float deltaTime);

	// handle events from the keyboard/mouse
	void HandleMovementInput(const KeyboardEvent& kbe, float deltaTime);
	void HandleMovementInput(const MouseEvent& me, float deltaTime);
	
	EditorCamera* GetCamera() { return pCamera_; };

	// when an instance of a models list is created we have to setup a local pointer to it
	void SetModelsList(ModelListClass* pModelsList);     

private:  // restrict a copying of this class instance
	ZoneClass(const ZoneClass & obj);
	ZoneClass & operator=(const ZoneClass & obj);

private:
	// handle keyboard input to control the zone state (state of the camera, terrain, etc.)
	void HandleZoneControlInput(const KeyboardEvent& kbe);   

	// there are main parts of the zone: sky, terrain, etc.
	void RenderSkyElements(int & renderCount, D3DClass* pD3D);
	void RenderTerrainElements(int & renderCount, D3DClass* pD3D);

	// render particular zone element
	void RenderTerrain(Model* pTerrain, int & renderCount, D3DClass* pD3D, FrustumClass* pFrustum);
	void RenderSkyDome(Model* pSkyDome, int & renderCount, D3DClass* pD3D);
	void RenderSkyPlane(Model* pSkyPlane, int & renderCount, D3DClass* pD3D);

	Model* GetModelByID(const std::string & modelID);   // get a model ptr by this model's ID 

private:
	Settings* pEngineSettings_ = nullptr;   // a pointer to the engine settings object
	ID3D11DeviceContext* pDeviceContext_ = nullptr;
	EditorCamera* pCamera_ = nullptr;
	FrustumClass* pFrustum_ = nullptr;
	ModelListClass* pModelsList_ = nullptr;  // a pointer to the models list object

	float deltaTime_ = 0.0f;                 // time between frames

	// states
	bool showCellLines_ = false;             // a boolean variable indicating whether the bounding boxes around the terrain cells should be drawn or not
	bool heightLocked_ = false;              // a variable for indicating if the camera should be locked to the height of the terrain or not
};
