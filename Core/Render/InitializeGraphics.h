////////////////////////////////////////////////////////////////////
// Filename:     InitializeGraphics.h
// Description:  this class is responsible for initialization all the 
//               graphics in the engine;
//
// Created:      02.12.22
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <DirectXMath.h>

#include "../GameObjects/TextureManager.h"
#include "../Camera/cameraclass.h"
#include "../UI/UserInterfaceClass.h"
#include "../Render/RenderToTextureClass.h"
//#include "../Render/frustumclass.h"
#include "../GameObjects/ModelsCreator.h"
#include "../Engine/Settings.h"

#include "Entity/EntityManager.h"   // from the ECS module


//////////////////////////////////
// Class name: InitializeGraphics
//////////////////////////////////
class InitializeGraphics final
{
public:
	InitializeGraphics();

	// initialized all the DirectX stuff
	bool InitializeDirectX(
		D3DClass& d3d,
		HWND hwnd,
		Settings& settings);

	bool InitializeCameras(
		CameraClass& editorCamera,
		CameraClass& cameraForRenderToTexture,
		DirectX::XMMATRIX& baseViewMatrix,      // is used for 2D rendering
		Settings& settings);

	bool InitializeScene(
		D3DClass& d3d,
		ECS::EntityManager& entityMgr,
		MeshStorage& meshStorage,
		Settings& settings,
		RenderToTextureClass& renderToTexture,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext);

	bool InitializeModels(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		ECS::EntityManager& entityMgr,
		MeshStorage& meshStorage,
		Settings & settings,
		const float farZ);

#if 0
	// initialize the main wrapper for all of the terrain processing 
	bool InitializeTerrainZone(
		ZoneClass & zone,
		CameraClass & editorCamera,
		Settings & settings,
		const float farZ);                            // screen depth
#endif


	bool InitializeSprites(const UINT screenWidth, const UINT screenHeight);
	bool InitializeLightSources(ECS::EntityManager& mgr, Settings & settings);

	// initialize the GUI of the game/engine (interface elements, text, etc.)
	bool InitializeGUI(D3DClass & d3d, UserInterfaceClass & UI,	Settings & settings);

private:  // restrict a copying of this class instance
	InitializeGraphics(const InitializeGraphics & obj);
	InitializeGraphics & operator=(const InitializeGraphics & obj);

};

