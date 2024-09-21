////////////////////////////////////////////////////////////////////
// Filename:     Render.cpp
// Description:  there are functions for rendering graphics;
// Created:      01.01.23
// Revising:     01.01.23
////////////////////////////////////////////////////////////////////
#include "Render.h"

//#include "Common/MathHelper.h"
//#include "Common/Utils.h"
#include "Common/log.h"
#include "InitRender.h"

//#include <DirectXCollision.h>

namespace Render
{


Render::Render()
{
	//Log::Debug();
}

Render::~Render()
{
	//Log::Debug();
}




////////////////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////



bool Render::Initialize(
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX& worldViewOrtho) 
{
	try
	{
		//Log::Debug();

		bool result = true;
		InitRender init;

		result = init.InitializeShaders(
			pDevice, 
			pDeviceContext,
			shadersContainer_,
			worldViewOrtho);
		Assert::True(result, "can't initialize shaders");
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, true);
		Log::Error("can't initialize the Render module");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool Render::UpdatePerFrame(
	ID3D11DeviceContext* pDeviceContext,
	const DirectX::XMMATRIX& viewProj,    // (is already transposed)
	const DirectX::XMFLOAT3& cameraPos,
	const std::vector<DirLight>& dirLights,
	const std::vector<PointLight>& pointLights,
	const std::vector<SpotLight>& spotLights)
{
	try 
	{
		shadersContainer_.lightShader_.UpdatePerFrame(
			pDeviceContext,
			viewProj,
			cameraPos,
			dirLights,
			pointLights,
			spotLights,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, true);
		return false;
	}
	catch (...)
	{
		Log::Error("something went wrong during updating data for rendering");
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////

bool Render::UpdateInstancedBuffer(
	ID3D11DeviceContext* pDeviceContext_,
	const std::vector<DirectX::XMMATRIX>& worlds,
	const std::vector<DirectX::XMMATRIX>& texTransforms,
	const std::vector<Material>& meshesMaterials)
{
	try
	{
		shadersContainer_.lightShader_.UpdateInstancedBuffer(
			pDeviceContext_,
			worlds,
			texTransforms,
			meshesMaterials);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, true);
		Log::Error("can't update instanced buffer for rendering");
		return false;
	}
	catch (...)
	{
		Log::Error("can't update instanced buffer for rendering for some unknown reason :)");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

bool Render::RenderInstances(
	ID3D11DeviceContext* pDeviceContext,
	ID3D11Buffer* pMeshVB,
	ID3D11Buffer* pMeshIB,
	const std::vector<ID3D11ShaderResourceView*>& texturesSRVs,
	const std::vector<UINT>& instancesCountsPerTexSet,          // the same geometry can have different textures;
	const uint32_t indexCount,
	const uint32_t vertexSize)
{
	try
	{
		shadersContainer_.lightShader_.Render(
			pDeviceContext,
			pMeshVB,
			pMeshIB,
			texturesSRVs,
			instancesCountsPerTexSet,
			indexCount,
			vertexSize);
	}
	catch (LIB_Exception& e)
	{
		Log::Error(e, true);
		Log::Error("can't render the mesh instances onto the screen");
		return false;
	}
	catch (...)
	{
		Log::Error("can't render mesh instances for some unknown reason :)");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////

}; // namespace Render