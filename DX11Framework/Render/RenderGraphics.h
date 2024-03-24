////////////////////////////////////////////////////////////////////
// Filename:     RenderGraphics.h
// Description:  this class is responsible for rendering all the 
//               graphics onto the screen;
// Created:      02.12.22
// Revising:     01.01.22
////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>

#include "../Render/d3dclass.h"
#include "../Engine/Settings.h"
#include "../Engine/SystemState.h"
#include "../GameObjects/ModelsStore.h"
#include "../UI/UserInterfaceClass.h"

//////////////////////////////////
// Class name: RenderGraphics
//////////////////////////////////
class RenderGraphics final
{
public:
	RenderGraphics();
	~RenderGraphics();

	// initialize the rendering subsystem
	void Initialize(ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const Settings & settings);

	// public rendering API
	bool Render(
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,

		SystemState & systemState,
		D3DClass & d3d,
		ModelsStore & modelsStore,
		LightStore & lightsStore,
		UserInterfaceClass & UI,
		FrustumClass & editorFrustum,

		ColorShaderClass & colorShader,
		TextureShaderClass & textureShader,
		LightShaderClass & lightShader,
		PointLightShaderClass & pointLightShader,

		const DirectX::XMMATRIX & WVO,        // is used for 2D rendering (world * basic_view * ortho)
		const DirectX::XMMATRIX & viewProj,   // view * projection
		const DirectX::XMFLOAT3 & cameraPos,
		const float deltaTime,
		const float totalGameTime,
		const float cameraDepth);

	

private:  // restrict a copying of this class instance
	RenderGraphics(const RenderGraphics & obj);
	RenderGraphics & operator=(const RenderGraphics & obj);

private:
	// render all the 2D / 3D models onto the screen
	bool RenderModels(
		FrustumClass & editorFrustum,
		ColorShaderClass & colorShader,
		TextureShaderClass & textureShader,
		LightShaderClass & lightShader,
		PointLightShaderClass & pointLightShader,

		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		SystemState & systemState,
		ModelsStore & modelsStore,
		LightStore & lightsStore,
		const DirectX::XMMATRIX & viewProj,   // view * projection
		const DirectX::XMFLOAT3 & cameraPos,
		const float deltaTime,
		const float totalGameTime,
		const float cameraDepth);

	// render all the GUI parts onto the screen
	bool RenderGUI(
		ID3D11DeviceContext* pDeviceContext,
		D3DClass & d3d,
		SystemState & systemState,
		UserInterfaceClass & UI,
		const DirectX::XMMATRIX & WVO,
		const float deltaTime,
		const int gameCycles);


#if 0
	void SetupRenderTargetPlanes();
	void SetupGameObjectsForRenderingToTexture();

	void RenderRenderableGameObjects();
	void RenderReflectionPlane();

	void UpdateGUIData(SystemState* pSystemState);
	void Render2DSprites(const float deltaTime);
	void RenderPickedGameObjToTexture(RenderableGameObject* pGameObj);
	void RenderSceneToTexture(const std::vector<RenderableGameObject*> & gameObjArr);
	void RenderReflectedSceneToTexture(const std::vector<RenderableGameObject*> & gameObjArr, const GameObject* pRelfectionPlane);

	// a function for dynamic modification game objects' positions, rotation, etc. during the rendering of the scene
	void MoveRotateScaleGameObjects(GameObject* pGameObj,
		const float t,
		const UINT modelIndex);
#endif

private:   // MIRROR / SHADOW DEMO

	void SetupRoom();
	UINT SetupWall();
	UINT SetupFloor(UINT planeIndex);

	void DrawRoom();
	void DrawSphere();
	void DrawMirror();

	void MarkMirrorOnStencil();

	void DrawFloorReflection();
	void DrawSphereReflection();

}; // class RenderGraphics
