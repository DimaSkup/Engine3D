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
#include "../UI/UserInterfaceClass.h"
#include "../EffectsAndShaders/ShadersContainer.h"
#include "../Light/LightStore.h"
#include "../Animation/TextureAtlasAnimation.h"
#include "../Render/frustumclass.h"
#include "../GameObjects/MeshStorage.h"

#include "ECS_Entity/EntityManager.h"

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

	// public updating API
	void Update(
		ID3D11DeviceContext* pDeviceContext,
		EntityManager& entityMgr,
		Shaders::ShadersContainer & shaderContainer,
		LightStore & lightsStore,
		SystemState & sysState,
		UserInterfaceClass& UI,
		const DirectX::XMFLOAT3 & cameraPos,
		const DirectX::XMFLOAT3 & cameraDir,
		const float deltaTime,
		const float totalGameTime);


	// public rendering API
	void Render(
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		EntityManager& entityMgr,
		MeshStorage& meshStorage,
		Shaders::ShadersContainer & shadersContainer,
		SystemState & systemState,
		D3DClass & d3d,
		LightStore & lightsStore,
		UserInterfaceClass & UI,
		FrustumClass & editorFrustum,

		const DirectX::XMMATRIX & WVO,        // is used for 2D rendering (world * basic_view * ortho)
		const DirectX::XMMATRIX & viewProj,   // view * projection
		const DirectX::XMFLOAT3 & cameraPos,
		const DirectX::XMFLOAT3 & cameraDir,
		const float deltaTime,
		const float totalGameTime,
		const float cameraDepth);


private:  // restrict a copying of this class instance
	RenderGraphics(const RenderGraphics & obj);
	RenderGraphics & operator=(const RenderGraphics & obj);

private:
	// render all the 2D / 3D models onto the screen
	void RenderModels(
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		EntityManager& entityMgr,
		MeshStorage& meshStorage,
		FrustumClass & editorFrustum,
		ColorShaderClass & colorShader,
		TextureShaderClass & textureShader,
		LightShaderClass & lightShader,
		SystemState & systemState,
		LightStore & lightsStore,
		const DirectX::XMMATRIX & viewProj,   // view * projection
		const DirectX::XMFLOAT3 & cameraPos,
		const DirectX::XMFLOAT3 & cameraDir,
		const float deltaTime,
		const float totalGameTime,
		const float cameraDepth);


	void PrepareIAStageForRendering(
		ID3D11DeviceContext* pDeviceContext,
		const Mesh::DataForRendering& meshData,
		const D3D11_PRIMITIVE_TOPOLOGY topologyType);

	void PrepareTexturesSRV_ToRender(
		const std::vector<TextureClass*>& textures,
		std::vector<ID3D11ShaderResourceView* const*>& outTexturesSRVs);

private:
	TextureAtlasAnimation fireTexAnimData_;
};
