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

#include "graphicsclass.h"

#include "../GameObjects/TextureManagerClass.h"
#include "../Camera/cameraclass.h"
#include "../UI/UserInterfaceClass.h"
#include "../Render/RenderToTextureClass.h"
#include "../Render/frustumclass.h"
#include "../GameObjects/ModelsCreator.h"
#include "../Light/LightStorage.h"

#include "ECS_Entity/EntityManager.h"   // from the ECS module


//////////////////////////////////
// Class name: InitializeGraphics
//////////////////////////////////
class InitializeGraphics final
{
public:
	InitializeGraphics();

	// initialized all the DirectX stuff
	bool InitializeDirectX(
		D3DClass & d3d,
		HWND hwnd,
		const UINT windowWidth,
		const UINT windowHeight,
		const float nearZ,        // near Z-coordinate of the screen/frustum
		const float farZ,         // far Z-coordinate of the screen/frustum (screen depth)
		const bool vSyncEnabled,
		const bool isFullScreenMode,
		const bool enable4xMSAA);

	// initialize all the shaders (color, texture, light, etc.)
	bool InitializeShaders(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		Shaders::ShadersContainer & shadersContainer);

	bool InitializeCameras(
		CameraClass & editorCamera,
		CameraClass & cameraForRenderToTexture,
		DirectX::XMMATRIX & baseViewMatrix,      // is used for 2D rendering
		const UINT windowWidth,
		const UINT windowHeight,
		const float nearZ,               // near Z-coordinate of the frustum/camera
		const float farZ,                // far Z-coordinate of the frustum/camera
		const float fovDegrees,          // field of view
		const float cameraSpeed,         // camera movement speed
		const float cameraSensitivity);  // camera rotation speed

	bool InitializeScene(
		D3DClass& d3d,
		EntityManager& entityMgr,
		MeshStorage& meshStorage,
		LightStorage& lightStore,
		Settings& settings,
		RenderToTextureClass& renderToTexture,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		HWND hwnd,
		const float nearZ,               // near Z-coordinate of the frustum/camera
		const float farZ);               // far Z-coordinate of the frustum/camera (screen depth)

	bool InitializeModels(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		EntityManager& entityMgr,
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
	bool InitializeLight(Settings & settings, LightStorage & lightStore);

	// initialize the GUI of the game/engine (interface elements, text, etc.)
	bool InitializeGUI(D3DClass & d3d,
		UserInterfaceClass & UI,
		Settings & settings,
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const UINT windowWidth,
		const UINT windowHeight);

private:  // restrict a copying of this class instance
	InitializeGraphics(const InitializeGraphics & obj);
	InitializeGraphics & operator=(const InitializeGraphics & obj);

};

