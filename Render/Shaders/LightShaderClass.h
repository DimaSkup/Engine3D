////////////////////////////////////////////////////////////////////
// Filename:     LightShaderClass.h
// Description:  this class is needed for rendering textured models 
//               with simple DIFFUSE light on it using HLSL shaders.
// Created:      09.04.23
////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////
// INCLUDES
//////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "../Common/MemHelpers.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "SamplerState.h"        // for using the ID3D11SamplerState 
#include "ConstantBuffer.h"
#include "Helpers/LightHelperTypes.h"

//#include <assimp/material.h>



namespace Render
{

//**********************************************************************************
//                DECLARATIONS OF STRUCTURES FOR CONST BUFFERS
//**********************************************************************************

namespace buffTypes
{
	struct InstancedData
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldInvTranspose;
		DirectX::XMMATRIX texTransform;
		Material  material;
		//UINT              alphaClipping;     // 1 - enabled; 0 - disabled
	};

	struct cbvsPerFrame
	{
		// a structure for vertex shader data which is changed each frame
		DirectX::XMMATRIX viewProj;
	};

	struct cbpsPerFrame
	{
		// a structure for pixel shader data which is changed each frame

		DirLight          dirLights[3];
		PointLight        pointLights[25];
		SpotLight         spotLights;
		DirectX::XMFLOAT3 cameraPos;
	};

	struct cbpsRareChanged
	{
		// a structure for pixel shader data which is rarely changed
		float debugMode;
		float debugNormals;
		float debugTangents;
		float debugBinormals;
		
		DirectX::XMFLOAT3 fogColor;
		float fogEnabled;
		float fogStart;
		float fogRange;
		
		float turnOnFlashLight;
		float numOfDirLights;

		float enableAlphaClipping;
	};
};


//**********************************************************************************
// Class name: LightShaderClass
//**********************************************************************************
class LightShaderClass final
{
public:
	LightShaderClass();
	~LightShaderClass();

	// restrict a copying of this class instance
	LightShaderClass(const LightShaderClass& obj) = delete;
	LightShaderClass& operator=(const LightShaderClass& obj) = delete;


	// initialize the shader class object
	bool Initialize(
		ID3D11Device* pDevice, 
		ID3D11DeviceContext* pDeviceContext);

	// prepare the rendering pipeline to render using this kind of shaders
	void UpdatePerFrame(
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMMATRIX& viewProj,
		const DirectX::XMFLOAT3& cameraPos,
		const std::vector<DirLight>& dirLights,
		const std::vector<PointLight>& pointLights,
		const std::vector<SpotLight>& spotLights,
		const D3D11_PRIMITIVE_TOPOLOGY topologyType);

	void UpdateInstancedBuffer(
		ID3D11DeviceContext* pDeviceContext,
		const std::vector<DirectX::XMMATRIX>& worlds,
		const std::vector<DirectX::XMMATRIX>& texTransforms,
		const std::vector<Material>& materials);

	void Render(
		ID3D11DeviceContext* pDeviceContext,
		std::vector<ID3D11Buffer*>& ptrsMeshVB,                     // arr of ptrs to meshes vertex buffers
		std::vector<ID3D11Buffer*>& ptrsMeshIB,                     // arr of ptrs to meshes index buffers
		const std::vector<ID3D11ShaderResourceView*>& texturesSRVs,
		const std::vector<ptrdiff_t>& numInstancesPerMesh,
		const std::vector<uint32_t>& instancesCountsPerTexSet,          // the same geometry can have different textures;
		const std::vector<uint32_t>& indexCounts,
		const uint32_t vertexSize);

	inline const std::string& GetShaderName() const { return className_; }

	// for controlling of different shader states
	void EnableDisableDebugNormals(ID3D11DeviceContext* pDeviceContext);
	void EnableDisableDebugTangents(ID3D11DeviceContext* pDeviceContext);
	void EnableDisableDebugBinormals(ID3D11DeviceContext* pDeviceContext);

	void EnableDisableFogEffect(ID3D11DeviceContext* pDeviceContext);
	void ChangeFlashLightState(ID3D11DeviceContext* pDeviceContext);
	void SetDirLightsCount(ID3D11DeviceContext* pDeviceContext, const UINT numOfLights);

	void SetFogParams(
		ID3D11DeviceContext* pDeviceContext,
		const DirectX::XMFLOAT3& fogColor,
		const float fogStart,
		const float fogRange);

	void SetAlphaClipping(ID3D11DeviceContext* pDeviceContext, const bool param)
	{
		cbpsRareChanged_.data.enableAlphaClipping = param;
		cbpsRareChanged_.ApplyChanges(pDeviceContext);
	}


private:
	//
	// INITIALIZATION RELATED FUNCTIONAL
	//
	void InitializeShaders(
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pDeviceContext,
		const WCHAR* vsFilename,
		const WCHAR* psFilename);

	void BuildInstancedBuffer(ID3D11Device* pDevice);


	//
	// DEBUG RELATED FUNCTIONAL
	//
	PixelShader* CreatePS_ForDebug(
		ID3D11DeviceContext* pDeviceContext,
		const std::string& funcName);

	inline void TurnOffDebug()
	{
		cbpsRareChanged_.data.debugNormals = false;
		cbpsRareChanged_.data.debugTangents = false;
		cbpsRareChanged_.data.debugBinormals = false;
	}

	inline void SetDefaultPS()
	{
		// delete the debug PS, and set the default pixel shader for rendering
		//SafeDelete(pDebugPS_);

		pDebugPS_->Shutdown();
		delete(pDebugPS_);
		pDebugPS_ = nullptr;

		pPS_ = &psDefault_;
	}


private:
	VertexShader vs_;
	PixelShader  psDefault_;                        // a basic pixel shader 
	SamplerState samplerState_;                     // a sampler for texturing

	PixelShader* pPS_ = nullptr;                    // ptr to the current pixel shader (a shader function)
	PixelShader* pDebugPS_ = nullptr;               // ptr to a pixel shader for debugging (a shader debug function)

	ID3D11Buffer* pInstancedBuffer_ = nullptr;
	std::vector<buffTypes::InstancedData> instancedData_;

	ConstantBuffer<buffTypes::cbvsPerFrame>    cbvsPerFrame_;    // for vertex shader 
	ConstantBuffer<buffTypes::cbpsPerFrame>    cbpsPerFrame_;    // for pixel shader
	ConstantBuffer<buffTypes::cbpsRareChanged> cbpsRareChanged_; // for pixel shader

	const std::string className_{ "light_shader" };
};


} // namespace Render