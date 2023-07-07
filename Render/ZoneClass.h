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
#include "../Camera/cameraclass.h"
#include "../Camera/EditorCamera.h"

// models
#include "../Model/Model.h"
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
	void Render(const std::map<std::string, Model*> & modelsList, 
		int & renderCount,
		D3DClass* pD3D);

	void HandleMovementInput(const KeyboardEvent& kbe, float deltaTime);
	void HandleMovementInput(const MouseEvent& me, float deltaTime);

	EditorCamera* GetCamera() { return pCamera_; };

private:  // restrict a copying of this class instance
	ZoneClass(const ZoneClass & obj);
	ZoneClass & operator=(const ZoneClass & obj);

private:
	
	void RenderTerrain(Model* pTerrain, int & renderCount, D3DClass* pD3D, FrustumClass* pFrustum);
	void RenderSkyDome(Model* pSkyDome, int & renderCount, D3DClass* pD3D);
	void RenderSkyPlane(Model* pSkyPlane, int & renderCounts, D3DClass* pD3D);

private:
	Settings* pEngineSettings_ = nullptr; // a pointer to the engine settings class
	ID3D11DeviceContext* pDeviceContext_ = nullptr;
	EditorCamera* pCamera_ = nullptr;
	FrustumClass* pFrustum_ = nullptr;

	// states
	bool showCellLines_ = false;          // a boolean variable indicating whether the bounding boxes around the terrain cells should be drawn or not
	bool heightLocked_ = false;           // a variable for indicating if the camera should be locked to the height of the terrain or not
};
