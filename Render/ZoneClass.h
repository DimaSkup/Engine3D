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

//
#include "../Render/d3dclass.h"

// camera
#include "../Camera/cameraclass.h"
#include "../Camera/EditorCamera.h"

// models
#include "../Model/modelclass.h"
#include "../Model/TerrainClass.h"





//////////////////////////////////
// Class name: ZoneClass
//////////////////////////////////
class ZoneClass final
{
public:
	ZoneClass();
	ZoneClass(const ZoneClass& copy);
	~ZoneClass();

	bool Initialize(SETTINGS::settingsParams* settingsParams);
	void Render(const std::map<std::string, ModelClass*> & modelsList, 
		int & renderCount,
		D3DClass* pD3D
	);

	void HandleMovementInput(const KeyboardEvent& kbe, float deltaTime);
	void HandleMovementInput(const MouseEvent& me, float deltaTime);

	EditorCamera* GetCamera() { return pCamera_; };


private:
	void RenderSkyDome(ModelClass* pSkyDome, int & renderCount, D3DClass* pD3D);
	void RenderTerrain(ModelClass* pTerrain, int & renderCount, D3DClass* pD3D);

private:
	ID3D11DeviceContext* pDeviceContext_ = nullptr;
	EditorCamera* pCamera_ = nullptr;
	bool showCellLines_ = false;   // a boolean variable indicating whether the bounding boxes around the terrain cells should be drawn or not

};
