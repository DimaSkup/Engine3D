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
//#include "TerrainClass.h"
//#include "ShaderManagerClass.h"
//#include "PositionClass.h"

// camera
#include "../Camera/cameraclass.h"
#include "../Camera/EditorCamera.h"


//////////////////////////////////
// Class name: ZoneClass
//////////////////////////////////
class ZoneClass
{
public:
	ZoneClass();
	ZoneClass(const ZoneClass& copy);
	~ZoneClass();

	//bool Initialize(int screenWidth, int screenHeight, float screenDepth);
	void HandleMovementInput(const KeyboardEvent& kbe, const MouseEvent& me, float deltaTime);

	EditorCamera* GetCamera() { return pCamera_; };

private:


private:
	EditorCamera* pCamera_ = nullptr;

};
